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

#ifndef GOL_MAPS_PROPERTIES_H_
#define GOL_MAPS_PROPERTIES_H_

// std
#include <string>
#include <vector>
#include <map>
// boost
#include <boost/any.hpp>
#include "boost/variant.hpp"
// osmpbf
#include <osmpbf/osmformat.pb.h> // high-level OSM objects

namespace gol {
  namespace osm{  

/*
enum tmode_t {
    tm_tram      =   1,
    tm_subway    =   2,
    tm_rail      =   4,
    tm_bus       =   8,
    tm_ferry     =   16,
    tm_all       =   255
};

enum stop_attribute_t {
  sa_wheelchair_boarding  =   1,  // wheelchair accessible
  sa_visual_accessible    =   2,  // accessible for blind people
  sa_shelter              =   4,  // roof against rain
  sa_bikeshed             =   8,  // you can put your bike somewhere
  sa_bicyclerent          =   16, // you can rent a bicycle
  sa_parking              =   32  // carparking is available
};
*/

struct node 
{
  // INT primary key 
  long long int id;

  double   lat;
  double   lon;
  double   ele;

  std::map<std::string, std::string> tags; // tag           

  void add_tag(std::string k, std::string v) {
    tags.insert(std::make_pair(k,v));
  }   

};

struct way 
{
  // INT primary key 
  long long int id;

  std::map<std::string, std::string> tags; // tag
  std::vector<long long int>         refs; // node_in_way

  void add_tag(std::string k, std::string v) {
    tags.insert(std::make_pair(k,v));
  } 

  void add_ref(long long int nd) {
    refs.push_back(nd);
  }

};

struct relation_member 
{
  long long int id;
  std::string   role;
  
  OSMPBF::Relation::MemberType member_type;

  relation_member(long long int id, std::string role,
    OSMPBF::Relation::MemberType member_type):
      id(id), role(role), member_type(member_type) {}

};

struct relation 
{  
  // INT primary key  
  long long int id;

  std::map<std::string, std::string> tags; // tag
  std::vector<relation_member>       refs; // member_in_relation  

  void add_tag(std::string k, std::string v) {
    tags.insert(std::make_pair(k,v));
  }

  void add_ref(osm::relation_member mem) {
    refs.push_back(mem);
  }   

};

struct node_as_via_turn_restriction 
{
  long long int    via;  // The via member(s) connect the beginning and
      
  // no_entry and no_exit restriction can have more than 1 from member
  std::vector<way> from; // A way from which restriction starts
  std::vector<way> to;   // The other end of the restriction

  std::string restriction;

  bool is_only_restriction() 
  {
    bool is_only_restriction;

    if (restriction.find("only_") == 0) { 
      is_only_restriction = true;
    }
    else if (restriction.find("no_") == 0) { 
      is_only_restriction = false;
    }
    else {
      return false; // unrecognized role type
    } 
    return is_only_restriction;     
  }

  bool is_restricted_maneuver(std::string nf, std::string nt)
  {
    bool nfFound = false; 
    bool ntFound = false;

    for(auto w : from)
      if ( std::find(w.refs.begin(), w.refs.end(), 
             std::atoll(nf.c_str())) != w.refs.end() )
        nfFound = true;

    for(auto w : to)
      if ( std::find(w.refs.begin(), w.refs.end(), 
             std::atoll(nt.c_str())) != w.refs.end() )
        ntFound = true;     

    return ( is_only_restriction() != (nfFound && ntFound) ) && nfFound;
  }

};

}  // namespace osm

/*enum feature_Kt 
{
  highway_value                      =  0,   // the value indicate the kind of road
  highway_is_motorway                =  1,   // selected roads with limited access
  highway_is_pedestrian              =  2,   // exclusively by pedestrians
  highway_is_oneway                  =  3,   // if false street is NOT a oneway street
  highway_is_oneway_forward          =  4,
  highway_is_cycleway                =  5,   // exclusively by bicycle
  highway_is_cycleway_oneway         =  6,
  highway_is_oneway_cycleway_forward =  7,
  highway_name                       =  8,   // highway description
  highway_is_road                    =  9,   // selected roads for car traffic
  highway_length                     =  255  // street length in metres 
}; 

typedef std::map<feature_Kt, boost::any> features_map;*/

class features_map 
{  
 private:  
  std::map<std::string, std::string> _tags;
  double _length;

  std::vector<std::string> highwayK = {
    "highway", 
    "highway_1", 
    "area:highway"};

  // Motorway

  std::vector<std::string> motorwayV = {
    "motorway",     
    "motorway_link"};

  // Trunk

  std::vector<std::string> trunkV = {
    "trunk",     
    "trunk_link"};

  // Pedestrian

  std::vector<std::string> pedestrianV = {
    "pedestrian",   // Reserved for pedestrian-only use
    "footway",      // Used mainly by pedestrians (also allowed for bicycles)
    "steps",        // Steps on footways
    "track",        // Dirt roads for mostly agricultural or forestry uses
    "bridleway",    // Use by horse riders (primarily) and pedestrians
    "corridor",     // Maps hallway inside of a building
    "trail",        // For cross-country trails
    "gate",         // A section in a wall which can be opened to allow or restrict access
    "stile",        // Provides people a passage through or over a boundary via steps
    "cattle_grid",  // Grid in road surface 
    "viaduct",      // Bridge composed of several small spans for crossing a valley or a gorge
    "path",         // A generic path, either multi-use or unspecified usage, open to all non-motorized vehicles      
    "via_ferrata"}; // For traversing a mountainside   

  std::vector<std::string> sidewalkK = {
    "sidewalk"};    // To provide information about sidewalks associated with streets

   std::vector<std::string> sidewalkV = {
    "yes",
    "both",
    //"no",
    //"none",
    "right",
    "left",
    "separate"};

  std::vector<std::string> footwayK = {
    "footway"};     // Refines the tag highway=footway for sidewalk

  std::vector<std::string> footwayV = {
    "yes",
    "both",    
    "sidewalk",
    "crossing"};

  std::vector<std::string> footK = {
    "foot"};        // Refines the tag highway=footway for sidewalk

   std::vector<std::string> footV = {
    //"no",
    //"private",
    "yes",
    "designated",
    "official",
    "permissive",
    "destination"};      

  // Bicycle

  std::vector<std::string> cyclewayV = {
    "cycleway",     // Only for bicycle
    "track",        // Dirt roads for mostly agricultural or forestry uses
    "trail"};       // For cross-country trails 

  std::vector<std::string> cyclableK = {
    "bicycle",
    "cycleway",
    "cycleway:left",
    "cycleway:right"};      

  std::vector<std::string> cyclableV = {
    //"no",
    "yes",
    "track",
    "designated",
    "lane",
    "opposite_lane",
    "dismount",
    "permissive",
    "shared_lane",
    //"private",
    "yes;permissive",
    "unknown",
    "unclassified",
    "opposite_track",
    "tolerated",
    "dismounted",
    "share_busway"};    

  // Road

  std::vector<std::string> motor_accessK = {
    "access",
    "access:car",
    "access:lanes",    
    "motor_vehicle",
    "motorroad",
    "vehicle",
    "vehicle:forward",
    "motorcar",
    "motorcycle",
    "motorcar:conditional",
    "motor_vehicle:conditional"  }; 

  std::vector<std::string> motor_accessV = {
    "yes",
    //"private",
    //"no",
    "designated",
    "permissive",
    "destination",
    "customers",
    "forestry",
    "agricultural",
    "unknown",
    "public",
    "yes|yes|yes|yes|yes|yes|yes|yes|no",
    "||||||||no",
    //"only motor bikes",
    //"unknown",
    "per",
    "yes;permissive"};                   

  // Features  

  std::vector<std::string> onewayK = {
    "oneway",
    "source:oneway",
    "oneway:psv",
    //"oneway:bus",
    "oneway:bicycle",     
    "junction"};

  std::vector<std::string> FonewayV = {
    "yes", 
    "true",    
    "1",           
    "roundabout", 
    "mini_roundabout"};

  std::vector<std::string> RonewayV = {
    "reverse", 
    "-1"}; 

  std::vector<std::string> nameK = {
    "name",
    "official_name",
    "loc_name",
    "alt_name",
    "short_name",
    "reg_name",
    "old_name"};     


  bool contains(std::string v, std::vector<std::string>& vec) 
  { 
    if ( std::find(vec.begin(), vec.end(), v) != vec.end() )
      return true;
    return false;
  } 

 public:

  features_map(): 
   _tags(), _length(0) {} 

  features_map(std::map<std::string, std::string> tags): 
   _tags(tags), _length(0) {}

  void set_length(double l) { 
    _length = l; 
  }     

  double get_length() { 
    return _length; 
  }  

  bool is_pedestrian_highway() 
  { 
    std::string k, v;
    bool motorway(false), trunk(false), //cycleway(false), 
         no_foot(false), no_access(false);    
    for (auto t : _tags)
    {
      std::tie(k, v) = t;
      if ( contains(k, highwayK) && contains(v, motorwayV) )
        motorway = true;
      if ( contains(k, highwayK) && contains(v, trunkV) )
        trunk = true;      
      //if ( contains(k, highwayK) && contains(v, cyclewayV) )
      //  cycleway = true;           
      if ( contains(k, footK) && !contains(v, footV) )
        no_foot = true; 
      if ( k == "access" && (v == "private" || v == "no") )
        no_access = true;                                
    }
    return !(motorway || trunk || /*cycleway ||*/ no_foot || no_access);
  } 

  bool is_safe_pedestrian_highway() 
  { 
    std::string k, v;
    bool pedestrian(false), sidewalk(false), footway(false);
    for (auto t : _tags)
    {
      std::tie(k, v) = t;
      if ( contains(k, highwayK) && contains(v, pedestrianV) )
        pedestrian = true;
      if ( contains(k, sidewalkK) && contains(v, sidewalkV) )
        sidewalk = true;
      if ( contains(k, footwayK) && contains(v, footwayV) )
        footway = true;                
    }
    return (pedestrian || sidewalk || footway);
  } 

  bool is_cyclable_highway() 
  { 
    std::string k, v;
    bool motorway(false), trunk(false), no_cyclable(false);
    for (auto t : _tags)
    {
      std::tie(k, v) = t;
      if ( contains(k, highwayK) && contains(v, motorwayV) )
        motorway = true;
      if ( contains(k, highwayK) && contains(v, trunkV) )
        trunk = true;       
      if ( contains(k, cyclableK) && !contains(v, cyclableV) )
        no_cyclable = true;                            
    }
    return !(motorway || trunk || no_cyclable);
  } 

  bool is_safe_cyclable_highway() 
  { 
    std::string k, v;
    bool cycleway(false), cyclable(false); 
    for (auto t : _tags)
    {
      std::tie(k, v) = t;
      if ( contains(k, highwayK) && contains(v, cyclewayV) )
        cycleway = true;
      if ( contains(k, cyclableK) && contains(v, cyclableV) )
        cyclable = true;                            
    }
    return (cycleway || cyclable);
  } 

  bool is_road_highway() 
  { 
    if(_tags.empty()) 
      return false;

    std::string k, v, pk, pv;
    bool pedestrian(false), cycleway(false), no_motor_access(false);
    for (auto t : _tags)
    {
      std::tie(k, v) = t;
      if ( contains(k, highwayK) && contains(v, pedestrianV) ) {
        for (auto pt : _tags) {
          std::tie(pk, pv) = pt;
          if ( contains(pk, motor_accessK) && contains(pv, motor_accessV)) 
            return true;
        }
        pedestrian = true;
      }
      if ( contains(k, highwayK) && contains(v, cyclewayV) )
        cycleway = true;      
      if ( contains(k, motor_accessK) && !contains(v, motor_accessV) )
        no_motor_access = true;               
    }
    return !(pedestrian || cycleway || no_motor_access);
  } 

  bool is_not_oneway_highway() 
  { 
    if ( is_forward_oneway_highway() )
      return false;              
    if ( is_reverse_oneway_highway() )
      return false;     
    return true;   
  } 

  bool is_forward_oneway_highway() 
  { 
    std::string k, v;
    bool motorway(false), foneway(false);
    for (auto t : _tags)
    {
      std::tie(k, v) = t;  
      if ( contains(k, highwayK) && v == "motorway" )
        motorway = true;
      if ( contains(k, onewayK) && contains(v, FonewayV) )
        foneway = true;              
    }
    return (motorway || foneway);
  } 

  bool is_reverse_oneway_highway() 
  { 
    std::string k, v;
    for (auto t : _tags)
    {
      std::tie(k, v) = t;  
      if ( contains(k, onewayK) && contains(v, RonewayV) )
        return true;              
    }
    return false;
  }

  std::string get_highway_value() 
  { 
    std::string k, v;
    for (auto t : _tags)
    {
      std::tie(k, v) = t;  
      if ( contains(k, highwayK) )
        return v;            
    }
    return "nd";
  } 

  std::string get_highway_name() 
  { 
    std::string k, v;
    for (auto t : _tags)
    {
      std::tie(k, v) = t;  
      if ( contains(k, nameK) )
        return v;            
    }
    return "nd";
  }       

};

} // namespace gol

#endif // GOL_MAPS_PROPERTIES_H_