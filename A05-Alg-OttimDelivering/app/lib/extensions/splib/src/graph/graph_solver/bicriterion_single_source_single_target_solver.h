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

#ifndef GOL_GRAPH_BSS_SOLVER_H_
#define GOL_GRAPH_BSS_SOLVER_H_

#include "../graph_decision_maker.h"

namespace gol {

/**
*  
*/ 
template <typename GraphT, 
          typename WeightT,
          typename IndexMap,
          typename BiSPAlgorithm, 
          typename WeightFunctionT,
          typename StoppingCriteriaT>
class BSP_gsolver: 
  public graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT> 
{
  typedef graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>                       Base;   
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<WeightT, path_t> > graph_solver_result;

 public:
  BSP_gsolver(
    GraphT& g, 
    vertex_descriptor source, 
    vertex_descriptor target): 
        graph_solver<
           GraphT, 
           WeightT,
           IndexMap, 
           WeightFunctionT, 
           StoppingCriteriaT>(g),      
        _s(source), 
        _t(target), 
        _pareto_set() {}
  ~BSP_gsolver() {}
    
  virtual void solve(
      WeightFunctionT   weight_functor,
      IndexMap          /*edge_index_map,*/,        
      StoppingCriteriaT /*stopping_criteria*/) override 
  {
    typedef typename remove_pointer<WeightFunctionT>::type FunctionT;   
    boost::functionPt_property_map<
        FunctionT,
        edge_descriptor, 
        WeightT> 
      weight_function(weight_functor);
    heuristic<GraphT, WeightT>* h = new null_heuristic<GraphT, WeightT>(); 
    try
    {  
      BiSPAlgorithm::compute(
        Base::_g, _s, _t, *h, 
        _pareto_set,
        weight_function,
        Base::_stats);
    } catch (std::exception& e) {
       // logger(logWARNING)
       //   << left("[solver]", 14)
       //   << e.what(); 
    }     
    delete h;

  }

  virtual graph_solver_result get_result() override 
  {   
    graph_solver_result ret = 
      decision_maker<GraphT, WeightT>::
         euclidean_distance_choice(_pareto_set);
         //best_single_objective_choice(_pareto_set);  
    _pareto_set.clear();
    
    return ret;    
  }  
    
 private:
  vertex_descriptor   _s;
  vertex_descriptor   _t;
  graph_solver_result _pareto_set;

};

} // namespace gol

#endif // GOL_GRAPH_BSS_SOLVER_H_