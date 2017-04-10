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

#ifndef GOL_GRAPH_WEIGHT_ID_FUNCTOR_H_
#define GOL_GRAPH_WEIGHT_ID_FUNCTOR_H_

namespace gol {  

// Equivalent to shortest path
template<
    typename GraphT, 
    typename VertexMap,
    typename WeightT>
struct identity_weight_functor : 
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
    identity_weight_functor(GraphT& g, VertexMap& vtxmap):
     generic_weight_functor<
       GraphT, VertexMap, WeightT>(g, vtxmap) {}

  virtual 
  WeightT operator()(const edge_descriptor edge) const {
    return (Base::_g)[edge].weight;
  }

}; 


} // namespace gol 

#endif // GOL_GRAPH_WEIGHT_ID_FUNCTOR_H_