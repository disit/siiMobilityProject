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

#ifndef GOL_GRAPHS_H_
#define GOL_GRAPHS_H_

namespace gol {

// TODO: optimizing space memory

struct extra_vertex_properties 
{
  // extra porperty

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {}    
};

struct extra_edge_properties 
{
  std::string highway_value;
  std::string desc;

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & 
        BOOST_SERIALIZATION_NVP(highway_value) &
        BOOST_SERIALIZATION_NVP(desc);
    }   
};  	

// Pedestrian graph

struct pedestrian_extra_vertex_properties 
{
  // extra porperty

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {}    
};

struct pedestrian_extra_edge_properties 
{
  std::string highway_value;
  std::string desc;

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & 
        BOOST_SERIALIZATION_NVP(highway_value) &
        BOOST_SERIALIZATION_NVP(desc);
    }   
};  

typedef generic_edge_weighted_graph_t <
      pedestrian_extra_vertex_properties,
      pedestrian_extra_edge_properties,
      double 
> pedestrian_graphT; 

// Road graph

struct road_extra_vertex_properties 
{
  // extra porperty

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {}    
};

struct road_extra_edge_properties 
{
  std::string highway_value;
  std::string desc;

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar &       
        BOOST_SERIALIZATION_NVP(highway_value) &
        BOOST_SERIALIZATION_NVP(desc);
    }   
};

typedef generic_edge_weighted_graph_t <
      road_extra_vertex_properties,
      road_extra_edge_properties,
      double 
> road_graphT; 

} // namespace gol

#endif // GOL_GRAPHS_H_