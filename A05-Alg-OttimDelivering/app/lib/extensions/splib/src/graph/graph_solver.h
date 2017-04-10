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

#ifndef GOL_GRAPH_SOLVER_H_
#define GOL_GRAPH_SOLVER_H_

#include "graph_functionPt_property_map.h"
#include "graph_weight_function.h"
#include "graph_heuristic.h"
#include "graph_model_edge_weight.h"

namespace gol {

/**
*
*/
template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class graph_solver
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<WeightT, path_t> > graph_solver_result;
 
 public:
  virtual void solve(
    WeightFunctionT   weight_functor,
    IndexMap          edge_index_map, 
    StoppingCriteriaT stopping_criteria) = 0;

 private:
  virtual graph_solver_result get_result() = 0;    

 protected:
  graph_solver(GraphT& g) : _g(g), _stats() {}
  ~graph_solver() {} 
  
  GraphT&        _g;
  struct stats_t _stats; 
 
 public:
  optimized_routes get_optimized_routes()
  {
    optimized_routes opt;
    for (auto length_path_KV : get_result())
    { 
      Route route; // TODO check constructor
      path_t path = length_path_KV.second;
      for (edge_descriptor e : path) 
      {
        route.add_route_edge(
          edge_weight_adaptor<WeightT>::to_length(_g[e].weight),
          _g[e].properties.highway_value,
          _g[e].properties.desc,
          _g[boost::source(e,_g)].id,
          _g[boost::source(e,_g)].geo.lon, 
          _g[boost::source(e,_g)].geo.lat,
          _g[boost::target(e,_g)].id,
          _g[boost::target(e,_g)].geo.lon, 
          _g[boost::target(e,_g)].geo.lat);

      }
      opt.push_back(route);
    }
    return opt;
  }  

  unsigned int get_visited_nodes() { 
    return _stats.visited_nodes; 
  }
  
  double get_run_time() { 
    return _stats.run_time; 
  }

};	

} // namespace gol

#include "graph_solver/single_source_single_target_solver.h"
#include "graph_solver/single_source_multi_target_solver.h"
#include "graph_solver/bicriterion_single_source_single_target_solver.h"
#include "graph_solver/arc_based_single_source_single_target_solver.h"


#endif // GOL_GRAPH_SOLVER_H_