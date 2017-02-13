#ifndef GOL_MAPS_PROPERTIES_H_
#define GOL_MAPS_PROPERTIES_H_

// std
#include <string>
#include <map>
// boost
#include <boost/any.hpp>
#include "boost/variant.hpp"

namespace gol {

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

enum feature_Kt 
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
  highway_length                     =  255  // street length in metres 
}; 

typedef std::map<feature_Kt, boost::any> features_map;

} // namespace gol

#endif // GOL_MAPS_PROPERTIES_H_