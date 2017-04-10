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

#ifndef GOL_GRAPH_WEIGHT_QP_FUNCTOR_H_
#define GOL_GRAPH_WEIGHT_QP_FUNCTOR_H_

namespace gol {  

template<
    typename GraphT, 
    typename VertexMap,
    typename WeightT>
struct quietest_pedestrian_functor: 
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
    quietest_pedestrian_functor(GraphT& g, VertexMap& vtxmap):
     generic_weight_functor<GraphT, VertexMap, WeightT>(g, vtxmap) {}

  virtual WeightT operator()(const edge_descriptor edge) const 
  {
    std::string hval = 
      (Base::_g)[edge].properties.highway_value;

    double priority = 25;
    //if (hval == "trunk"         || 
    //    hval == "trunk_link" )
    //  priority = 50; 
    if (hval == "primary"       || 
        hval == "primary_link" )
      priority = 40;
    if (hval == "secondary"     || 
        hval == "secondary_link" )
      priority = 30;
    if (hval == "cycleway"      ||
        hval == "steps"         ||  // Steps on footways
        hval == "track"         ||  // Dirt roads for mostly agricultural or forestry uses
        hval == "bridleway"     || 
        hval == "path"          || 
        hval == "viaduct"       ||
        hval == "private"       ||  
        hval == "via_ferrata")      // For traversing a mountainside   
      priority = 15;          
    if (hval == "residential"   ||
        hval == "living_street" ||  // Pedestrians friendly
        hval == "residential_link")
      priority = 10;
    if (hval == "pedestrian"    ||  // Reserved for pedestrian-only use
        hval == "footway"       ||  // Used mainly by pedestrians (also allowed for bicycles)
        hval == "corridor")         // Maps hallway inside of a building 
      priority = 5;

    return ((Base::_g)[edge].weight) * 
      ( 0.5 + (priority/100) ); // penalty coeff in interval [0.5, 1] 
  }

};


} // namespace gol 

#endif // GOL_GRAPH_WEIGHT_QP_FUNCTOR_H_