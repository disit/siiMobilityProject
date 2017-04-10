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

#ifndef GOL_GRAPH_WEIGHT_FC_FUNCTOR_H_
#define GOL_GRAPH_WEIGHT_FC_FUNCTOR_H_

namespace gol {  

template<
    typename GraphT, 
    typename VertexMap,
    typename WeightT>
struct fastest_road_functor: 
public generic_weight_functor<
    GraphT, 
    VertexMap,
    WeightT>
{
  typedef generic_weight_functor<
      GraphT, VertexMap, WeightT >           Base;

  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;

  explicit
    fastest_road_functor(GraphT& g, VertexMap& vtxmap):
     generic_weight_functor<
       GraphT, VertexMap, WeightT>(g, vtxmap) {}

  virtual WeightT operator()(const edge_descriptor edge) const {
   return (Base::_g)[edge].weight;    
  }

};

template<typename GraphT, typename VertexMap>
struct fastest_road_functor<
  GraphT, 
  VertexMap, 
  double>: 
public generic_weight_functor<
    GraphT, 
    VertexMap,
    double>
{
  typedef generic_weight_functor<
      GraphT, VertexMap, double >            Base;
  
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;

  explicit
    fastest_road_functor(GraphT& g, VertexMap& vtxmap):
     generic_weight_functor<
       GraphT, VertexMap, double>(g, vtxmap) {}

  virtual double operator()(const edge_descriptor edge) const 
  {   
    std::string hval = 
      (Base::_g)[edge].properties.highway_value;    

    double max_speed = 45; // Km/h
    
    if (hval == "motorway"         || 
        hval == "motorway_link" )
      max_speed = 130;     
    if (hval == "trunk"            || 
        hval == "trunk_link" )
      max_speed = 90; 
    if (hval == "primary"          || 
        hval == "primary_link")
      max_speed = 70;
    if (hval == "secondary"        || 
        hval == "secondary_link")
      max_speed = 55;         
    if (hval == "residential"      ||
        hval == "residential_link" ||
        hval == "living_street")       // Pedestrians friendly      
      max_speed = 35;
    if (hval == "pedestrian"       ||
        hval == "footway")             
      max_speed = 15;    

    return (Base::_g)[edge].weight / (max_speed / 3.6);    
  }

};


} // namespace gol 

#endif // GOL_GRAPH_WEIGHT_FC_FUNCTOR_H_