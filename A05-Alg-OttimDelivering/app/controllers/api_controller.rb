require 'rubygems'
require 'sii_mobility_api'
require 'rosemary'


class ApiController < ApplicationController
  @@semaphore = Mutex.new
  module Validator
    def validate_input
      controller_name = params['controller'].split('/').last
      begin
        @json_data = params[controller_name.singularize].to_h
        self.class::validator(controller_name + '#' + params['action']).validate(@json_data, nil)
      rescue JSONValidator::JSONValidationException => e
        @json_data[:response] = { :error_code => "-1", :error_message => e.message, :current_operation => "shortest path calculation" }
        render json: @json_data
      end
    end
  end

  include Validator
  before_action :validate_input, except: :doc

  def self.http_route(call)
    controller, method = call.split('#')
    ret = Rails.application.routes.routes.select{ |r| r.requirements == {:controller=>controller, :action=>method } }[0]
    raise ArgumentError.new("No route found for call " + call) if ret.nil?
    ret
  end

  def self.http_method(call)
    method_regexp = http_route(call).verb
    ['GET', 'POST', 'DELETE', 'PUT'].select{ |m| m =~ method_regexp }.join(', ')
  end

  def self.http_address(call)
    http_route(call).path.spec.to_s.gsub("(.:format)", "")
  end

  def self.method_description(call)
    case call
    when 'api#shortest_path'
      'Computes the shortest path between an origin and a destination, taking into account the user preferences and constraints'
    when 'api#shortest_path_geojson'
      'Computes the shortest path between an origin and a destination, taking into account the user preferences and constraints. Return a geojson response'
    else
      raise ArgumentError, 'No description for method ' + call
    end
  end

  def self.validator(call)
    case call
    when 'api#shortest_path'
      JSONValidator::Validators::request
    when 'api#shortest_path_geojson'
      JSONValidator::Validators::request
    when *%w(request node error_status)
      JSONValidator::Validators.send(call)
    else
        raise ArgumentError, 'No validator for method ' + call
    end
  end

  def self.validator_response(call)
    case call
    when 'api#shortest_path'
      JSONValidator::Validators::response
    when 'api#shortest_path_geojson'
      JSONValidator::Validators::geojson_response
    else
      raise ArgumentError, 'No response validator for method ' + call
    end
  end

  def self.request_example(call)
    s = nil
    begin
      File.open("#{Rails.root}/app/examples/#{call}_request.json", 'r') { |f| s = f.read }
    rescue SystemCallError
    end
    unless s.nil?
      s_obj = JSON.parse s.gsub(/\s+/, '')
      validator(call).validate(s_obj, nil)
    end
    s
  end

  def self.response_example(call)
    s = nil
    begin
      File.open("#{Rails.root}/app/examples/#{call}_response.json", 'r') { |f| s = f.read }
    rescue SystemCallError
    end
    unless s.nil?
      s_obj = JSON.parse s.gsub(/\s+/, '')
      validator_response(call).validate(s_obj, nil)
    end
    s
  end

  def doc
  end

  def distance_between(coords)
    lon1, lon2, lat1, lat2 = coords.map {|i| i * Math::PI/180 }
    dlat = lat2 - lat1
    dlon = lon2 - lon1
    a = (Math.sin(dlat / 2))**2 + Math.cos(lat1) *
        (Math.sin(dlon / 2))**2 * Math.cos(lat2)
    c = 2 * Math.atan2( Math.sqrt(a), Math.sqrt(1-a))
    c * 6371 * 1000
  end

  def find_nid(lon, lat)
    api = Rosemary::Api.new
    epsilon = 0.002
    bb = api.find_bounding_box(lon - epsilon, lat - epsilon, lon + epsilon, lat + epsilon)
    nd = []
    bb.ways.each do |way|
      if way.tags.key?("highway")
         nd += way.nodes
      end
    end
    nodes = bb.nodes.select{ |node| nd.include?(node.id) }
    dists = nodes.map{ |node| distance_between([node.lon, lon, node.lat, lat]) }
    nidx  = dists.index( dists.min )
    nodes[nidx].id.to_s
  end

  def to_geojson(routes)
    coords = []
    routes.each do |route|
      arc = route["arc"]
      arc.each do |node|
        geoCoord = [ node["source_node"]["lon"], node["source_node"]["lat"] ]
        coords << geoCoord
      end
      if not arc.empty?
        last_node = arc[-1]
        geoCoord  = [ last_node["destination_node"]["lon"], last_node["destination_node"]["lat"] ]
        coords << geoCoord
      end
    end
    geojson = {
      :type => "FeatureCollection",
      :features => [ {
          :type => "Feature",
          :geometry => {
              :type => "LineString",
              :coordinates => coords
           },
          :properties => {
              "fill" => "#0000CC",
              "fill-opacity" => 0.5,
              "lanes" => 10,
              "stroke" => "#0000CC",
              "stroke-opacity" => 0.3,
              "stroke-width" => 6
          }
       } ]
    }
  end

  def route_optimization
    @@semaphore.synchronize do
      #routePlanner = RoutePlanner.new

      if @json_data[:journey][:source_node].key?("node_id")
        source = @json_data[:journey][:source_node][:node_id]
      else
        source = find_nid(
          @json_data[:journey][:source_node][:lon],
          @json_data[:journey][:source_node][:lat])
      end

      if @json_data[:journey][:destination_node].key?("node_id")
        target = @json_data[:journey][:destination_node][:node_id]
      else
        target = find_nid(
          @json_data[:journey][:destination_node][:lon],
          @json_data[:journey][:destination_node][:lat])
      end

      routes = Array.new
      routes = $routePlanner.route_optimization(
          @json_data[:journey][:search_route_type],
          source,
          target,
          @json_data[:journey][:start_datetime],
          "data/osm/pbf/bounding_box_tuscany.pbf",
          "data/gtfs/filename.gtfs")
    end
  end

  def shortest_path
    routes = route_optimization()
    routes.each do |route|
      route["arc"].each {|arc| arc["desc"].force_encoding('UTF-8')}
    end
    @json_data[:journey][:routes] = routes
    if routes.empty?
      @json_data[:response] = {
        :current_operation => "route optimization",
        :error_code => "-2",
        :error_message => "no route found"
      }
    else
      @json_data[:response] = {
        :current_operation => "route optimization",
        :error_code => "0",
        :error_message => "successful"
      }
    end
    render json: @json_data
  end

  def shortest_path_geojson
    routes = route_optimization()
    render json: to_geojson(routes).to_json
  end

  def model
  end

end
