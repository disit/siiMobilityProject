// This file is part of Sii-Mobility - Algorithms Optimized Delivering.
//
// Copyright (C) 2017 GOL Lab http://webgol.dinfo.unifi.it/ - University of Florence
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with This program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GOL_MULTIMODAL_JOURNEY_H_
#define GOL_MULTIMODAL_JOURNEY_H_

//#include "common.h"
#include "utils/json_stream.h"

namespace gol {

/*enum optimise_t {
    o_shortest  =   1,   // output shortest time
    o_transfers =   2,   // output least transfers
    o_all       =   255  // output all rides
};

enum tmode_t {
    tm_tram      =   1,
    tm_subway    =   2,
    tm_rail      =   4,
    tm_bus       =   12,
    tm_ferry     =   16,
    tm_all       =   255
};*/

struct route_node
{
  // node properties
  enum route_node_type 
  {
     wheelchair_boarding  =  1,  // wheelchair accessible
     visual_accessible    =  2,  // accessible for blind people
     shelter              =  4,  // roof against rain
     bikeshed             =  12, // you can put your bike somewhere
     bicyclerent          =  16, // you can rent a bicycle
     parking              =  32  // carparking is available
  };

  std::string id;
  double      lon;
  double      lat;
  int         arrival_time;
  int         departure_time;

};

// a part of a route where the transport type is constant
struct route_edge
{
   enum route_edge_type 
   {
     road_network     = 0,
     public_transprt  = 1,
     transfer         = 2
   };

  route_node   nFrom;
  route_node   nTo;
  double       length;
  std::string  highway_value;
  std::string  desc;
  std::string  transport;
  std::string  transport_provider;
  std::string  transport_service_type;

};

class Route
{
  typedef route_node nodeT;
  typedef route_edge edgeT;  
 public:
  Route() : _edges() {}
  ~Route() {}

  Route(Route&&) = default;
  Route(const Route&) = default;
  Route& operator=(Route&&) = default;
  Route& operator=(const Route&) = default;

  void
  add_route_edge(
    double      length,
    std::string highway_value,    
    std::string desc,
    std::string id_from,
    double      lon_from,
    double      lat_from,
    std::string id_to,
    double      lon_to,
    double      lat_to)
  {
   nodeT nFrom, nTo;
   
   nFrom.id  = id_from;
   nFrom.lon = lon_from;
   nFrom.lat = lat_from;
   nTo.id    = id_to;
   nTo.lon   = lon_to;
   nTo.lat   = lat_to;

   edgeT edge;
   edge.nFrom   = nFrom;
   edge.nTo     = nTo;
   edge.desc    = desc;
   edge.length  = length;

   edge.highway_value = highway_value;

   _edges.push_back(edge);
  }

  std::list<route_edge> get_edges() {return _edges;}

  std::string get_begin_id()
  {
    std::string id;
    auto reit = (this->_edges).begin();
    if (reit == (this->_edges).end())
      throw solver_exception("Route::get_end_id(): Stop not found");
    id = (*reit).nFrom.id;
    return id;
  }

  std::string get_end_id() 
  {
    std::string id;
    auto rerit = (this->_edges).rbegin();
    if (rerit == (this->_edges).rend())
      throw solver_exception("Route::get_end_id(): Stop not found");
    id = (*rerit).nTo.id;
    return id;
  }

 private:
  // Route is a list of edges
  std::list<route_edge> _edges;  

};

typedef std::map<std::string, Route> route_map;
typedef std::list<Route> optimized_routes;

class optimized_routes_solution
{
 public:
  optimized_routes_solution(): _routes() {}
  ~optimized_routes_solution() {}

  void 
  insert_route(Route r) {_routes.push_back(r);}

  std::list<Route> 
  get_routes() {return _routes;}

  void
  dump_geojson(std::string filename)
  {
    // geojson struct
    typedef std::vector<double>                    point;       // [v, ..]
    typedef std::vector<point>                     coordinates; // [[], ..]
    typedef std::map<std::string,
        boost::variant<std::string, coordinates> > geometry;    // {k:v, k:[[], ..], ..}
    typedef std::map<std::string,
        boost::variant<std::string, double> >      properties;        
    typedef std::map<std::string,
        boost::variant<
           std::string, geometry, properties> >    feature;     // {k:v, k:{k:v, k:[[]], ..}, ..}
    typedef std::vector<feature>                   features;    // [{k:v, k:{k:v, k:[[]], ..}, ..}, ..]
    typedef std::map<std::string,
        boost::variant<std::string, features> >    geojson;     // {k:v, k:[{k:v, {k:v, k:[[]], ..}, ..}, ..], ..}

    if (_routes.empty())
      return;

    std::vector<std::string> color =
      {"#FF8100", "#00204F", "#A00000", "#004200", "#FFE118", "#820040", "#00FFFF", "#00120C", "#2F2F2F"};
    int ncolor = color.size();   

    int  nroute = 0;
    features ftrs;

    for (auto route : _routes)
    {
      ++nroute;

      properties prop;
      prop["stroke"]         =  color[((nroute % ncolor) + ncolor) % ncolor];
      prop["stroke-width"]   =  6;
      prop["stroke-opacity"] =  0.4;
      prop["fill"]           =  color[((nroute % ncolor) + ncolor) % ncolor];                 
      prop["fill-opacity"]   =  0.5;
      prop["lanes"]          =  10;

      auto redges        = route.get_edges();
      auto reit          = redges.begin();
      auto route_endit   = redges.end();

      coordinates coords;
      if (reit != route_endit) {
        point source;
        source.push_back((*reit).nFrom.lon);
        source.push_back((*reit).nFrom.lat);
        coords.push_back(source);
      }
      while (reit != route_endit) {
        point p;
        p.push_back((*reit).nTo.lon);
        p.push_back((*reit).nTo.lat);
        coords.push_back(p);
        ++reit;
      }

      geometry geo;
      geo["type"]        = "LineString";
      geo["coordinates"] = coords;
      feature ftr;
      ftr["type"]        = "Feature";
      ftr["geometry"]    = geo;
      ftr["properties"]  = prop;
      ftrs.push_back(ftr);

    }
    geojson out_json;
    out_json["type"]     = "FeatureCollection";
    out_json["features"] = ftrs;

    try
    {
      boost::filesystem::path outfile; 
      if (!filename.empty()) 
        outfile = boost::filesystem::path(filename.append(".geojson"));
      else
        outfile = boost::filesystem::path(DEFAULT_GEOJSON_OUTPUTFILE);  

      boost::filesystem::path output = 
        boost::filesystem::path(RELATIVE_DIR) / outfile;

      logger(logINFO) 
        << left("[*]", 14) 
        << "Saving GEOjson >> " 
        << output;

      jofstream jstream( output.generic_string() ); 
      jstream << out_json;
    } catch (std::exception& e) {
        throw solver_exception(e.what());
    }

  }

private:
  std::list<Route> _routes;

};


} // namespace gol

#endif // GOL_MULTIMODAL_JOURNEY_H_
