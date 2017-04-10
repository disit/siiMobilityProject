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

#ifndef GOL_GRAPH_WEIGHT_FUNCTION_H_
#define GOL_GRAPH_WEIGHT_FUNCTION_H_

//std
#include <functional>

#include "graph_edge_weight_adaptor.h"

namespace gol {

template<
    typename GraphT, 
    typename VertexMap,
    typename WeightT>
struct generic_weight_functor: 
public std::unary_function<
    typename boost::graph_traits<GraphT>::edge_descriptor,
    WeightT>
{

  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;

  explicit
    generic_weight_functor(GraphT& g, VertexMap& vtxmap): 
        _g(g), _vtxmap(vtxmap) {}     

  virtual 
  WeightT operator()(const edge_descriptor edge) const {
    return _g[edge].weight;
  }

protected:
  GraphT&     _g;
  VertexMap&  _vtxmap;

};

} // namespace gol

#include "graph_weight_function/identity_weight_functor.h"
#include "graph_weight_function/quietest_pedestrian_functor.h"
#include "graph_weight_function/safest_fastest_bicycle_functor.h"
#include "graph_weight_function/fastest_road_functor.h"

namespace gol {

template <
    typename GraphT, 
    typename VertexMap, 
    typename WeightT>
class weight_function_factory 
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;

 public: 
  static 
  generic_weight_functor<GraphT, VertexMap, WeightT>*
  get_functor_for(
      std::string         strategy, 
      GraphT&             g,
      VertexMap&          vtxmap)
  {
    if (strategy == "shortest_weight_function") 
      return new identity_weight_functor<
            GraphT, VertexMap, WeightT>(g, vtxmap); 
    if (strategy == "quietest_pedestrian_weight_function") 
      return new quietest_pedestrian_functor<
            GraphT, VertexMap, WeightT>(g, vtxmap);                   
    if (strategy == "safest_fastest_bicycle_weight_function") 
      return new safest_fastest_bicycle_functor<
            GraphT, VertexMap, WeightT>(g, vtxmap);
    if (strategy == "fastest_road_weight_function") 
      return new fastest_road_functor<
            GraphT, VertexMap, WeightT>(g, vtxmap);                                  
      
    logger(logWARNING) 
      << left("[engine] ", 14) 
      << "Weight Function unknown, "
      << "select Shortest Weight Function";
    return new identity_weight_functor<
        GraphT, VertexMap, WeightT>(g, vtxmap);
  }
 
 private:
  // always declare assignment operator and default and copy constructor
  weight_function_factory();
  ~weight_function_factory();  
  weight_function_factory(const weight_function_factory&);
  weight_function_factory& operator=(const weight_function_factory&);
      
};


} // namespace gol


#endif  // GOL_GRAPH_WEIGHT_FUNCTION_H_
