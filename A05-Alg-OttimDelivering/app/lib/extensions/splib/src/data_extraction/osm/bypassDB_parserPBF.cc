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

#include <algorithm>

namespace gol { 
  namespace osm {

/// OSM Protocol Protocolbuffer Binary Format
template <typename BuilderT>
void roadn_pbf_parser<BuilderT>::node_callback(
    uint64_t           osm_id, 
    double             lon, 
    double             lat, 
    const pbf::tags_t& tags) 
{
  features_map  fmap;
  std::string   key, value;
  double        ele;
  bool          elevation_defined = false; 
  
  if(!_add_relation_to_model)
  {
    for (auto tag : tags) { //TODO
      tie(key, value) = tag;    
      if ( key   == "ele"     && 
           value != "NaN"   && 
           std::stod(value) > MARIANA_TRENCH_DEPTH ) { 
        ele = std::stod(value);
        elevation_defined = true;    
      }
    }   

    if (elevation_defined)
     _builder->add_node(
        boost::lexical_cast<std::string>(osm_id), 
        fmap, 
        lon, 
        lat, 
        ele); 
    else//*/
    {
      /*logger(logWARNING) 
          << left("[parser]", 14) 
          << "Elevation not defined, id: "
          << left(boost::lexical_cast<std::string>(osm_id), 12)
          << " val: " << left(value, 14);*/ 
     _builder->add_node(
        boost::lexical_cast<std::string>(osm_id), 
        fmap, 
        lon, 
        lat);      
    }
  }  
}

template <typename BuilderT>
void roadn_pbf_parser<BuilderT>::way_callback(
    uint64_t                     osm_id, 
    const pbf::tags_t&           tags, 
    const std::vector<uint64_t>& refs) 
{
  features_map  fmap;

  fmap[feature_Kt::highway_is_motorway]        = false;
  fmap[feature_Kt::highway_is_road]            = false; 
  fmap[feature_Kt::highway_is_pedestrian]      = false;
  fmap[feature_Kt::highway_is_cycleway]        = false;
  fmap[feature_Kt::highway_is_cycleway_oneway] = false;   
  fmap[feature_Kt::highway_is_oneway]          = false;
  fmap[feature_Kt::highway_is_oneway_forward]  = false;   

  fmap[feature_Kt::highway_value] = std::string(); 
  fmap[feature_Kt::highway_name]  = std::string();     

  std::string   key, value; 
  bool          is_highway = false;

  for (auto tag : tags) 
  {
    tie(key, value) = tag;
    
    if (!_add_relation_to_model  &&
         ( key == "highway"    ||
           key == "highway_1"  ||
           key == "area:highway" ) 
        ) 
    { 
      if (
          value != "construction" &&  // Road under construction
          value != "proposed"     &&  // For roads that are about to be built
          value != "abandoned"    &&  // Fallen into serious disrepair
          value != "services"     &&  // Service area places along a road
          value != "collapsed"    &&
          value != "disused"      &&
          //value != "path"         &&  // Road is not to be used by the general public          
          value != "bus_guideway")    // Not suitable for other traffic
      { 
         
        is_highway = true;       
        fmap[feature_Kt::highway_value] = value;   
        
        if (
            value == "motorway"      ||
            value == "motorway_link") 
        { 
          fmap[feature_Kt::highway_is_motorway]       = true;
          fmap[feature_Kt::highway_is_road]           = true;   
          fmap[feature_Kt::highway_is_oneway]         = true;
          fmap[feature_Kt::highway_is_oneway_forward] = true;

        }
        else if (
            value == "pedestrian"    ||  // Reserved for pedestrian-only use
            value == "footway"       ||  // Used mainly by pedestrians (also allowed for bicycles)
            value == "steps"         ||  // Steps on footways
            value == "track"         ||  // Dirt roads for mostly agricultural or forestry uses
            value == "bridleway"     ||  // Use by horse riders (primarily) and pedestrians
            value == "corridor"      ||  // Maps hallway inside of a building
            value == "trail"         ||  // For cross-country trails
            value == "gate"          ||  // A section in a wall which can be opened to allow or restrict access
            value == "stile"         ||  // Provides people a passage through or over a boundary via steps
            value == "cattle_grid"   ||  // Grid in road surface 
            value == "viaduct"       ||  // Bridge composed of several small spans for crossing a valley or a gorge      
            value == "via_ferrata")      // For traversing a mountainside 
        {
          fmap[feature_Kt::highway_is_pedestrian] = true; 
        }
        else if ( 
            value == "cycleway"      ||  // Only for bicycle
            value == "track"         ||  // Dirt roads for mostly agricultural or forestry uses
            value == "trail")            // For cross-country trails 
        { 
          fmap[feature_Kt::highway_is_cycleway] = true;
        }
        else 
        {
         fmap[feature_Kt::highway_is_road] = true;   
        }
      } 
    }  
    if (
        key == "oneway"   || 
        key == "junction") 
    {  
      if (
          value == "yes"            || 
          value == "true"           || 
          value == "1") 
      {
        fmap[feature_Kt::highway_is_oneway]         = true;
        fmap[feature_Kt::highway_is_oneway_forward] = true;
      }
      if (
          value == "reverse"        || 
          value == "-1") 
      {
        fmap[feature_Kt::highway_is_oneway]         = true;
        fmap[feature_Kt::highway_is_oneway_forward] = false;
      }
      if (
          value == "roundabout"     ||
          value == "mini_roundabout" ) 
      {
        fmap[feature_Kt::highway_is_oneway]         = true;
        fmap[feature_Kt::highway_is_oneway_forward] = true;
      }
    }
    if (
        (key == "cycleway" && value != "no") || 
         key == "cycleway:left"              ||
         key == "cycleway:right") 
    { 
      fmap[feature_Kt::highway_is_cycleway] = true;      
    }
    if (key == "oneway:bicycle" && value != "yes")
    {
      fmap[feature_Kt::highway_is_cycleway_oneway] = true; 
    }
    if (key == "name")
    {
      fmap[feature_Kt::highway_name] = value; 
    }
    if (key == "access" && value == "no")
    {
      is_highway = false;
      break;
    }

  } // end for tags
  
  if (_add_relation_to_model && 
         ( std::find(
              _relation_area.begin(), 
              _relation_area.end(), 
              boost::lexical_cast<std::string>(osm_id)) 
           != _relation_area.end() )
    ) 
  {
    is_highway = true;
    fmap[feature_Kt::highway_is_pedestrian] = true;
    
    fmap[feature_Kt::highway_value] = 
      std::string("pedestrian"); 
    fmap[feature_Kt::highway_name]  = 
      std::string();     
  }

  if (_add_relation_to_model && 
         ( _restricted_way.find(
              boost::lexical_cast<std::string>(osm_id)) 
           != _restricted_way.end() )
    ) 
  {
    std::string id = 
        boost::lexical_cast<std::string>(osm_id);
    
    for (auto ref : refs) 
      _restricted_way[id].push_back(
        boost::lexical_cast<std::string>(ref));
  }       
  
  // Add the edge to road graph
  if (is_highway) 
  {    
    //_ways.push_back(std::make_pair(refs, fmap));
    auto it = refs.begin(); 
    while (it != refs.end()) 
    {
      uint64_t sid = (*it);
      it++; // tid
      if (it != refs.end()) 
      {        
        _builder->add_section(
            boost::lexical_cast<std::string>(sid),
            boost::lexical_cast<std::string>((*it)),
            fmap);
      }
    }
  }
  
}

template <typename BuilderT>
void roadn_pbf_parser<BuilderT>::relation_callback(
    uint64_t                 osm_id, 
    const pbf::tags_t&       tags, 
    const pbf::references_t& refs)
{
  std::string key, value;

  if (!_add_relation_to_model)
  {
    bool        is_highway     = false; 
    bool        is_restriction = false;
    std::string restriction_desc; 

    for (auto tag : tags) 
    {
      tie(key, value) = tag;
      if ( key   == "highway"   && 
           value == "pedestrian") { // check?
        is_highway = true;
      }
      if ( key == "restriction") {
        is_restriction   = true;
        restriction_desc = value;
      }    
    }
    
    if (is_highway) 
    {  
      for (auto ref : refs) 
      {
        if (ref.role == "outer") 
          _relation_area.push_back(
            boost::lexical_cast<std::string>(
              ref.member_id));     
      }
    }  

    if (is_restriction)
    {
      std::vector<std::string> way_from;
      std::vector<std::string> way_to;
      std::vector<std::string> via;
      std::string              id, role;

      for (auto ref : refs) 
      {
        id   = boost::
          lexical_cast<std::string>(ref.member_id);
        role = ref.role;

        if (role == "via") {
          via.push_back(id);
        }
        if (role == "from") {
          way_from.push_back(id);
          _restricted_way[id];
        }
        if (role == "to") { 
          way_to.push_back(id);
          _restricted_way[id];
        }      
      }

      for (auto v : via) 
      { 
        if ( _relation_restriction.find(v) == 
               _relation_restriction.end() )
        {
          std::vector<
            std::pair< 
              std::string,
              std::pair<
                std::vector<std::string>, 
                std::vector<std::string> >  
            > > turn_res = { std::make_pair(
                  restriction_desc,
                  std::make_pair(way_from, way_to)) };
            _relation_restriction.insert(
              std::make_pair( v, turn_res ));
        }
        else
          _relation_restriction[v].push_back( 
            std::make_pair( restriction_desc,
              std::make_pair(way_from, way_to) ));
      }     
    }

  }  

}

template <typename BuilderT>
void roadn_pbf_parser<BuilderT>::construct_model() 
{
  pbf::osm_reader<roadn_pbf_parser> parser(_filename, *this);

  logger(logINFO)
    << left("[builder]", 14)
    <<"Creating Vertices and Edges"; 

  parser.parse();
  // nodes and ways might not be in order as in .osm file - (see lib osmosis)
  /*for (auto way : _ways) { 
    std::vector<uint64_t> refs; features_map fmap;
    tie(refs, fmap) = way;
    auto it = refs.begin(); 
    while (it != refs.end()) {
      uint64_t sid = (*it);
      it++; // tid
      if (it != refs.end()) {         
        (*_builder).add_section(
            boost::lexical_cast<std::string>(sid),
            boost::lexical_cast<std::string>((*it)),
            fmap);
      }
    }  
  }*/
#ifdef DEBUG 
  _builder->dump_graph_state(); 
#endif 

  logger(logINFO)
    << left("[builder]", 14)
    <<"Creating Relations"; 

  // Insert Relation, at the moment I need to parse data again
  _add_relation_to_model = true;   
  std::sort(_relation_area.begin(), _relation_area.end());   

  parser.reset();  
  parser.parse();

  for (auto KV : _relation_restriction)
  {
     std::string              via, turn_desc; 
     std::vector<std::string> from, to;

     via = KV.first; 
     for (auto FS : KV.second)
     {
       turn_desc = FS.first;
       for(auto wayId : FS.second.first) 
         for (auto nId : _restricted_way[wayId])
           from.push_back(nId);
       for(auto wayId : FS.second.second) 
         for (auto nId : _restricted_way[wayId])
           to.push_back(nId);

       _builder-> add_turn_restriction(
           via, turn_desc, from, to);           
     }
     
  } 

#ifdef DEBUG 
  _builder->dump_graph_state(); 
#endif

}

}  // namespace osm
}  // namespace gol	