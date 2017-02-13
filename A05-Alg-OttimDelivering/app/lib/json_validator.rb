require 'json'
require 'active_support/core_ext/class/attribute_accessors'
require 'active_support/inflector'


module JSONValidator
  
  private

  def self.pluralize_parentheses(s)
    if s[-1,1] == ')'
      i = s.rindex('(')
      if i.nil?
        s
      else
        s[0..(i-2)].pluralize + ' ' + s[i..-1]
      end
    else
      s.pluralize
    end
  end
  
  def self.update_hash_requirements(hash, requirement, primary_key = nil)
      hash.each_with_object({}) { |(key, value), h| h[key] = (key == primary_key ? value : [requirement] + value.drop(1)) }
  end
  
  def self.inpage_link_objects(s)
    s.gsub(/Object(s?) \(([\w\s]*)\)/, 'Object\1 (<a href="#\2">\2</a>)')
  end
  
  class ValidateHash
    attr_reader :name, :description, :hash
    def initialize(name, description, hash)
      hash.each_value { |v|
        raise ArgumentError.new(v) unless [:required, :optional].include?(v[0]) and v[1].is_a?(String) and v[2].respond_to?(:doc_type)
      }
      @name = name
      @description = description
      @hash = hash
    end
    
    def validate(obj, container)
      raise JSONValidationException.new('Object is not an hash', obj.to_json) unless obj.is_a?(Hash)
      objs = obj.symbolize_keys
      diff = objs.keys - @hash.keys
      raise JSONValidationException.new('Extra values in hash', diff.to_json) unless diff.empty?
      @hash.each_pair { |key, value|
        element = objs[key]
        if objs.has_key? key
          value.drop(2).each { |v|
            v.validate(element, objs)
          }
        else
          raise JSONValidationException.new('Required hash key "' + key.to_s + '" not present', objs.to_json) unless value[0] == :optional
        end
      }
    end
    
    def doc_type
      'Object (' + @name + ')'
    end
    
    def doc
      s = "Must be an hash with elements:\n"
      @hash.each_pair { |key, value|
        s += key.to_s + ': (' + value[0].to_s + ') ' + value[1] + "\n"
        value.drop(2).each { |v|
          s += v.doc
          s += " "
        }
        s += "\n"
      }
      s += "\n"
    end

    def doc_html
      
      
      s = '<table class="table" border="1">
<tr>
<th>Name</th>
<th>Description</th>
<th>Type</th>
<th>Is required</th>
<th>Requirements</th>
</tr>'
      @hash.each_pair { |key, value|
        s += '<tr>'
        s += '<td>' + key.to_s + '</td><td>' + value[1] + '</td><td>' + value[2].doc_type + '</td><td>' + (value[0].to_sym == :required ? 'X<div class="required"></div>' : '<div class="optional"></div>') + '</td><td>'
        value.drop(3).each { |v|
          s += v.doc + "<br>"
        }
        s += "</td></tr>\n"
      }
      s += "</table><br>\n"
      s
    end
  end

  class ValidateBoolean
    def validate(obj, container)
      raise JSONValidationException.new('Object is not Boolean', obj.to_json) unless obj.is_a?(TrueClass) or obj.is_a?(FalseClass)
    end
    
    def doc_type
      'Boolean'
    end
    
    def doc
      'Must be Boolean'
    end
  end

  class ValidateInteger
    def validate(obj, container)
      raise JSONValidationException.new('Object is not an integer', obj.to_json) unless obj.is_a?(Integer)
    end

    def doc_type
      'Number (integer)'
    end

    def doc
      'Must be an integer'
    end
  end

  class ValidateFloat
    def validate(obj, container)
      raise JSONValidationException.new('Object is not a floating point value', obj.to_json) unless obj.is_a?(Integer) or obj.is_a?(Float)
    end

    def doc_type
      'Number (float)'
    end

    def doc
      'Must be a floating point value'
    end
  end

  class ValidateNonNegative
    def validate(obj, container)
      raise JSONValidationException.new('Object is negative', obj.to_json) unless obj >= 0
    end

    def doc
      'Must be non-negative'
    end
  end

  class ValidateString
    def validate(obj, container)
      raise JSONValidationException.new('Object is not a string', obj.to_json) unless obj.is_a?(String)
    end

    def doc_type
      'String'
    end

    def doc
      'Must be a string'
    end
  end

  class ValidateEmail < ValidateString
    def validate(obj, container)
      super
      raise JSONValidationException.new('String is not a valid email', obj.to_json) unless obj =~ /\A([^@\s]+)@((?:[-a-z0-9]+\.)+[a-z]{2,})\Z/
    end

    def doc
      'Must be a valid email'
    end
  end

  class ValidateArray
    def initialize(*validators)
      @validators = validators
    end
    
    def validate(obj, container)
      return if obj.nil?
      raise JSONValidationException.new('Object is not an array', obj.to_json) unless obj.is_a?(Array)

      obj.each { |o|
        @validators.each { |v|
          v.validate(o, obj)
        }
      }
    end

    def doc_type
      'Array of ' + JSONValidator::pluralize_parentheses(@validators[0].doc_type)
    end

  end

  class ValidateLength
    def initialize(min, max)
      @min_length = min || 0
      @max_length = max
    end    
    
    def validate(obj, container)
      if @max_length.nil?
        raise JSONValidationException.new(doc) unless obj.length >= @min_length
      else
        raise JSONValidationException.new(doc) unless obj.length.between?(@min_length, @max_length)
      end
    end
    
    def doc
      if @max_length.nil?
        'Length must be greater or equal than ' + @min_length.to_s
      else
        'Length must be between ' + @min_length.to_s + ' and ' + @max_length.to_s
      end
    end
  end

  class ValidateGreaterThan
    def initialize(key)
      @key = key
    end

    def validate(obj, container)
      if container.has_key?(@key)
        raise JSONValidationException.new(doc, obj.to_json) unless obj >= container[@key]
      end
    end
    
    def doc
      'Must be greater than ' + @key.to_s
    end
  end

  class ValidateRange
    def initialize(v1, v2)
      @v1 = v1
      @v2 = v2
    end

    def validate(obj, container)
      raise JSONValidationException.new(doc, obj.to_json) unless obj.between?(@v1, @v2)
    end
    
    def doc
      'Must be between ' + @v1.to_s + ' and ' + @v2.to_s
    end
  end

  class ValidateFitsInTimeWindow
    def initialize(tw)
      @tw = tw
    end
    
    def validate(obj, container)
      if container.has_key?(@tw)
        time_window = container[@tw].symbolize_keys
        raise JSONValidationException.new(doc, container.to_json) unless time_window[:latest] - time_window[:earliest] >= obj
      end
    end
    
    def doc
      'Must fit in ' + @tw.to_s
    end
  end

  class ValidateTimeWindowsNonOverlapping
    def validate(obj, container)
      (obj.length - 1).times do |i|
        curr_time_window = obj[i].symbolize_keys
        next_time_window = obj[i+1].symbolize_keys
        raise JSONValidationException.new(doc, obj.to_json) unless curr_time_window[:latest] <= next_time_window[:earliest]
      end
    end
    
    def doc
      'Time windows must be ordered and non-overlapping'
    end
  end

  class ValidateValues
    def initialize(*values)
      @values = values
    end
    
    def validate(obj, container)
      raise JSONValidationException.new(doc, obj.to_json) unless @values.include?(obj)
    end
    
    def doc
      'Must be' + (@values.length >= 2 ? ' one of: ' : ': ') + @values.map {|x| x.to_s}.join(', ')
    end
  end

  class ValidateEmpty
    def validate(obj, container)
      raise JSONValidationException.new('Request must be empty') unless obj.nil?
    end
    
    def doc_html
      'No additional data must be supplied'
    end
  end
  
  class ValidateDate
    def validate(obj, container)
      raise JSONValidationException.new('Date must be formatted as YYYY/MM/DD') unless obj =~ /^(\d{4})\/(\d{2})\/(\d{2})$/
      begin
        Date.parse(obj)
      rescue ArgumentError
        raise JSONValidationException.new('Date must be valid and formatted as YYYY/MM/DD')
      end
    end
    
    def doc
      'Must be a valid date, formatted as YYYY/MM/DD'
    end
  end
  
  class ValidateNull
    def validate(obj, container)
      raise JSONValidationException.new('Must be null') unless obj.nil?
    end
    
    def doc_type
      'null'
    end
    
    def doc
      'Must be null'
    end
  end
  
  class ValidateOr
    def initialize(*validators)
      @validators = validators
    end
    def validate(obj, container)
      ok = false
      @validators.each { |v|
        begin
          v.validate(obj, container)
          ok = true
        rescue JSONValidationException => e
        end
      }
      raise JSONValidationException.new(doc, obj.to_json) unless ok
    end
    
    def doc_type
      @validators.map{ |v| v.doc_type }.join(' or ')
    end
    
    def doc
      @validators.map{ |v| v.doc.uncapitalize }.join(' or ').capitalize
    end
  end
  
  public

  class Validators
    cattr_reader :node, :journey, :request, :arc, :route, :optimized_routes, :error_status, :response, :geojson_coordinates, :geojson_geometry, :geojson_features, :geojson_response     

    @@node = ValidateHash.new('Node', 'Details about a node in the graph',
      :node_id => [:optional, '', ValidateString.new],
      :lat => [:optional, '', ValidateFloat.new],
      :lon => [:optional, '', ValidateFloat.new])

    @@journey = ValidateHash.new('Route', 'Represents the details about the journey that must be optimized',
      :search_route_type => [:required, '', ValidateString.new],
      :search_max_feet_km => [:optional, '', ValidateFloat.new],
      :start_datetime => [:optional, '', ValidateString.new],
      :source_node => [:required, '', @@node],
      :destination_node => [:required, '', @@node])    

    @@request = ValidateHash.new("Shortest Path Request", nil,
      :message_version => [:optional, '', ValidateString.new],
      :journey => [:required, '', @@journey])

    @@arc = ValidateHash.new('Arc details', 'In the optimization response, provides details about a single node-to-node arc in the computed journey',
      :distance => [:required, '', ValidateFloat.new],
      :desc => [:required, '', ValidateString.new],
      :start_datetime => [:required, '', ValidateString.new],
      :end_datetime => [:required, '', ValidateString.new],
      :transport => [:required, '', ValidateString.new],
      :transport_provider => [:required, '', ValidateOr.new(ValidateString.new, ValidateNull.new)],
      :transport_service_type => [:required, '', ValidateOr.new(ValidateString.new, ValidateNull.new)],
      :source_node => [:required, '', @@node],
      :destination_node => [:required, '', @@node])

    @@route = ValidateHash.new('Itinerary details', 'Describes optimized route',
      :eta => [:required, '', ValidateString.new],
      :distance => [:required, '', ValidateFloat.new],
      :arc => [:required, '', ValidateArray.new(@@arc)])

    @@optimized_routes = ValidateHash.new('Route response', 'Describes an optimized journey',
      @@journey.hash.merge({
        :route => [:required, '', ValidateArray.new(@@route)]}))

    @@error_status = ValidateHash.new('Error status', 'Used to notify any errors during the optimization process',
      :current_operation => [:optional, '', ValidateString.new],
      :error_code => [:required, '', ValidateString.new],
      :error_message => [:required, '', ValidateString.new],
      :more_info => [:optional, '', ValidateString.new],
      :project_name => [:optional, '', ValidateString.new])    

    @@response = ValidateHash.new('Optimization response', nil,
      @@request.hash.merge({
        :journey => [:required, '', @@optimized_routes],
        :response => [:required, '', @@error_status]}))        

    @@geojson_coordinates = ValidateArray.new([ValidateFloat.new, ValidateFloat.new]) 

    @@geojson_geometry = ValidateHash.new('Geojson geometry', nil,
      :type => [:required, '', ValidateString.new],
      :coordinates => [:optional, '', ValidateArray.new(@@geojson_coordinates)]) 

    @@geojson_features = ValidateHash.new('Geojson features', nil,
      :type => [:required, '', ValidateString.new],
      :geometry => [:required, '', @@geojson_geometry])

    @@geojson_response = ValidateHash.new('Computed journey in geojson format', nil,
      :type => [:required, '', ValidateString.new],
      :features => [:required, '', ValidateArray.new(@@geojson_features)])                                    

  end
  
  class JSONValidationException < Exception
    def initialize(msg, obj = "")
      @msg = msg
      @obj = obj
    end

    def to_s
      @msg + ', ' + @obj
    end
  end
end
