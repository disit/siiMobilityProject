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

#ifndef GOL_GRAPH_WEIGHT_SFB_FUNCTOR_H_
#define GOL_GRAPH_WEIGHT_SFB_FUNCTOR_H_

namespace gol {  

template<
    typename GraphT, 
    typename VertexMap,
    typename WeightT>
struct safest_fastest_bicycle_functor: 
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
    safest_fastest_bicycle_functor(GraphT& g, VertexMap& vtxmap):
     generic_weight_functor<
       GraphT, VertexMap, WeightT>(g, vtxmap) {}

  // functor works only for template specialization relative to bicycle
  virtual WeightT operator()(const edge_descriptor edge) const {
    return (Base::_g)[edge].weight;
  }
}; 

template<typename GraphT, typename VertexMap>
struct safest_fastest_bicycle_functor<
  GraphT, 
  VertexMap, 
  std::pair<int, int> >: 
public generic_weight_functor<
    GraphT, 
    VertexMap,
    std::pair<int, int> >
{
  typedef generic_weight_functor<
    GraphT, VertexMap, std::pair<int, int> > Base;  
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;
 
  explicit
    safest_fastest_bicycle_functor(GraphT& g, VertexMap& vtxmap):
     generic_weight_functor<GraphT, VertexMap, 
       std::pair<int, int> >(g, vtxmap) {}

  virtual std::pair<int, int> operator()(const edge_descriptor edge) const 
  {
    double  travel_time;
    double  length           = (Base::_g)[edge].weight.first;
    double  cyclable_length  = (Base::_g)[edge].weight.second;
    double  vertical_ascent  = 0;
    double  vertical_descent = 0;
    double  desc_grad        = 0;
    double  downhill_speed_multiplier;

    double source_elevation = 
      (Base::_g)[boost::source(edge, Base::_g)].geo.ele;
    double target_elevation = 
      (Base::_g)[boost::target(edge, Base::_g)].geo.ele;

    if( source_elevation == MARIANA_TRENCH_DEPTH || 
        target_elevation == MARIANA_TRENCH_DEPTH)  
      source_elevation = target_elevation; // no vertical components 

    if (source_elevation > target_elevation)
      vertical_descent = source_elevation - target_elevation;
    else
      vertical_ascent  = target_elevation - source_elevation;

    if (length != 0)
      desc_grad = vertical_descent / length;

    if (desc_grad > MAX_DESC_GRAD)
      desc_grad = MAX_DESC_GRAD;  

    downhill_speed_multiplier = 
      ( (MAX_DOWNHILL_SPEED_MULTIPLIER - 1) * 
        (desc_grad / MAX_DESC_GRAD)
      ) + 1;
    
    travel_time = std::ceil
    ( (length + PENALTY_COEFF_UPHILL * vertical_ascent) / 
      ((AVERAGE_BICYCLE_SPEED/3.6) * downhill_speed_multiplier) 
    );
    
    std::string hval = 
      (Base::_g)[edge].properties.highway_value;

    double priority = 25;
    //if (hval == "trunk"         || 
    //    hval == "trunk_link" )
    //  priority = 50; 
    if (hval == "primary"       || 
        hval == "primary_link" )
      priority = 45;
    if (hval == "secondary"       || 
        hval == "secondary_link" )
      priority = 35;    
    if (hval == "pedestrian"    ||  // Reserved for pedestrian-only use
        hval == "steps"         ||  // Steps on footways
        hval == "corridor"      ||  // Maps hallway inside of a building
        hval == "trail"         ||  // For cross-country trails
        hval == "via_ferrata")      // For traversing a mountainside   
      priority = 30;

    return std::make_pair(
      std::ceil(travel_time * ( 0.5 + (priority/100))), 
      cyclable_length ); 
  }
   
}; 

} // namespace gol 

#endif // GOL_GRAPH_WEIGHT_SFB_FUNCTOR_H_