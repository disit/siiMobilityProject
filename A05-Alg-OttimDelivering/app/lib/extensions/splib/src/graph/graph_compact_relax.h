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

#ifndef GOL_GRAPH_TURN_RELAX_H_
#define GOL_GRAPH_TURN_RELAX_H_

#include <boost/graph/relax.hpp>

namespace gol {

template <
    class GraphT, 
    class WeightMap, 
    class PredecessorMap, 
    class DistanceMap, 
    class BinaryFunction, 
    class BinaryPredicate>
bool compact_relax(
  typename boost::graph_traits<GraphT>
    ::edge_descriptor    incoming_e,   	
  typename boost::graph_traits<GraphT>
    ::edge_descriptor    outgoing_e, 
  const GraphT&          g, 
  const WeightMap&       w, 
  PredecessorMap&        p, 
  DistanceMap&           d, 
  const BinaryFunction&  combine, 
  const BinaryPredicate& compare)
{
  typedef typename boost::graph_traits<GraphT>::directed_category DirCat;
  //bool is_undirected = boost::is_same<DirCat, undirected_tag>::value;
  typedef typename boost::graph_traits<GraphT>::vertex_descriptor Vertex; 
   
  Vertex u = boost::target(incoming_e, g);

  int incoming_idx = g[incoming_e].entry_point;
  int outgoing_idx = g[outgoing_e].exit_point;      
      
  typedef typename boost::property_traits<DistanceMap>::value_type D;
  typedef typename boost::property_traits<WeightMap>::value_type   W;
  const D  d_u     = get(d, g[incoming_e].edge_index);
  const D  d_v     = get(d, g[outgoing_e].edge_index);
  const W& w_e     = get(w, outgoing_e);
  // represent the cost of turning from the i-th incoming arc into the j-th
  // outgoing arc at u
  const W  w_turn  = (*(g[u].turn_table))[incoming_idx][outgoing_idx];

  // The seemingly redundant comparisons after the distance puts are to
  // ensure that extra floating-point precision in x87 registers does not
  // lead to relax() returning true when the distance did not actually
  // change.
  if ( compare( combine(d_u, combine(w_e, w_turn)), d_v) ) 
  {
        put(d, g[outgoing_e].edge_index, combine(d_u, combine(w_e, w_turn)));
        if (compare(get(d, g[outgoing_e].edge_index), d_v)) {
          put(p, g[outgoing_e].edge_index, g[incoming_e].edge_index);
          return true;
        } else {
          return false;
        }
  } else
    return false;
}

} // namespace gol

#endif // GOL_GRAPH_TURN_RELAX_H_