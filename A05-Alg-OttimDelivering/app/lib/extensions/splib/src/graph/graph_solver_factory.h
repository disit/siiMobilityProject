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

#ifndef GOL_GRAPH_SOLVER_FACTORY_H_
#define GOL_GRAPH_SOLVER_FACTORY_H_

// std
#include <unordered_map>
#include <sstream>
// boost
#include <boost/graph/graph_traits.hpp>

#include "graph_edge_weight_traits.h" 
#include "../algorithm.h"
#include "graph_solver.h" 

namespace gol {

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class gsolver_creator
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor 
    vertex_descriptor;
  typedef typename Traits::edge_descriptor 
    edge_descriptor;
  typedef typename Traits::edge_iterator  edge_iterator;  
  typedef std::map<edge_index_t, edge_descriptor
  > EdgeMap;            
 public:

  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm, 
      vertex_descriptor s, 
      vertex_descriptor t) { return nullptr; }  

  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm, 
      vertex_descriptor s, 
      std::vector<vertex_descriptor> tvec) { return nullptr; }

  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm,
      EdgeMap& edgmap, 
      vertex_descriptor s, 
      vertex_descriptor t) { return nullptr; }    
  
  virtual ~gsolver_creator() {}

};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class SSST_gsolver_creator : 
  public gsolver_creator<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;    
 public:

  SSST_gsolver_creator() {}
  ~SSST_gsolver_creator() {}
 
  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm, 
      vertex_descriptor s, 
      vertex_descriptor t) override 
  {
    if (algorithm == "dijkstra")
    { 
      logger(logINFO) 
        << left("[solver] ", 14) 
        << "Dijkstra algorithm [ s = " 
        << g[s].id << ", t = " << g[t].id << " ]";   
      return new SSST_gsolver<
        GraphT, 
        WeightT,
        IndexMap, 
        dijkstra_algorithm, 
        WeightFunctionT, 
        StoppingCriteriaT>(g, s, t);
    }    
    else
      throw solver_exception();  
  }

};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class arc_based_gsolver_creator : 
  public gsolver_creator<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor 
    vertex_descriptor;
  typedef typename Traits::edge_descriptor 
    edge_descriptor;
  typedef typename Traits::edge_iterator  edge_iterator; 
  typedef std::map<edge_index_t, edge_descriptor
  > EdgeMap;          
 public:

  arc_based_gsolver_creator() {}
  ~arc_based_gsolver_creator() {}
 
  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm,
      EdgeMap& edgmap, 
      vertex_descriptor s, 
      vertex_descriptor t) override 
  {
    if (algorithm == "compact_dijkstra")
    { 
      logger(logINFO) 
        << left("[solver] ", 14) 
        << "Arc-Based Dijkstra algorithm [ s = " 
        << g[s].id << ", t = " << g[t].id << " ]";   
      return new arc_based_gsolver<
        GraphT, 
        WeightT,
        IndexMap, 
        compact_graph_dijkstra_algorithm, 
        WeightFunctionT, 
        StoppingCriteriaT>(g, edgmap, s, t);
    }   
    else
      throw solver_exception();  
  }

};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class SSMT_gsolver_creator : 
  public gsolver_creator<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor 
    vertex_descriptor;
 public:

  SSMT_gsolver_creator() {}
  ~SSMT_gsolver_creator() {}

  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm, 
      vertex_descriptor s, 
      std::vector<vertex_descriptor> tvec) override 
  {
    if (algorithm == "multi_target_dijkstra")
    {
      logger(logINFO) 
        << left("[solver] ", 14) 
        << "Multi-target Dijkstra algorithm [ s = "
        << g[s].id << ", t = * ]";      
      return new SSMT_gsolver <
        GraphT, 
        WeightT,
        IndexMap, 
        multi_target_dijkstra_algorithm, 
        WeightFunctionT, 
        StoppingCriteriaT> (g, s, tvec);
    }
    else
      throw solver_exception();
  }

};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class BSP_gsolver_creator : 
  public gsolver_creator<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor 
    vertex_descriptor;
 public:

  BSP_gsolver_creator() {}
  ~BSP_gsolver_creator() {}

  virtual graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* make_solver(
      GraphT& g,
      std::string algorithm, 
      vertex_descriptor s, 
      vertex_descriptor t) override 
  {
    if (algorithm == "bicriterion_epsMOA_star")
    { 
      logger(logINFO) 
        << left("[solver] ", 14) 
        << "Bicriterion epsilon-MOA* algorithm [ s = "
        << g[s].id <<", t = " << g[t].id << " ]";       
      return new BSP_gsolver <
        GraphT, 
        WeightT, 
        IndexMap,
        bicriterion_epsMOA_star_algorithm, 
        WeightFunctionT, 
        StoppingCriteriaT> (g, s, t);
    }
    else
      throw solver_exception();
  }

};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class gsolver_factory 
{  
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor 
    vertex_descriptor;
  typedef typename Traits::edge_descriptor 
    edge_descriptor;
  typedef typename Traits::edge_iterator  edge_iterator;  
  typedef std::map<edge_index_t, edge_descriptor
  > EdgeMap;   

  typedef std::unordered_map< 
    std::string, 
    gsolver_creator<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>* 
  > registry_map;
 public:
  
  virtual ~gsolver_factory() {
    _registry.clear();
  }            
  
  static gsolver_factory& instance() 
  {
    static gsolver_factory impl;
    return impl;
  }

  graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* get_solver_for(
      GraphT&           g,
      std::string       algorithm, 
      vertex_descriptor s, 
      vertex_descriptor t) 
  {       
    auto it = _registry.find(algorithm);
    if (it == _registry.end()) {
      throw solver_exception("unrecognized solver");
    }    
    gsolver_creator<
      GraphT, 
      WeightT,
      IndexMap, 
      WeightFunctionT, 
      StoppingCriteriaT>* c = it->second;
    return (c->make_solver(g, algorithm, s, t));
  }

  graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* get_solver_for(
      GraphT&                        g,
      std::string                    algorithm, 
      vertex_descriptor              s, 
      std::vector<vertex_descriptor> tvec) 
  {       
    auto it = _registry.find(algorithm);
    if (it == _registry.end()) {
      throw solver_exception("unrecognized solver");
    }    
    gsolver_creator<
      GraphT, 
      WeightT,
      IndexMap, 
      WeightFunctionT, 
      StoppingCriteriaT>* c = it->second;
    return (c->make_solver(g, algorithm, s, tvec));
  }

   graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>* get_solver_for(
      GraphT&            g,
      std::string        algorithm,
      EdgeMap&           edgmap, 
      vertex_descriptor  s, 
      vertex_descriptor  t) 
  {       
    auto it = _registry.find(algorithm);
    if (it == _registry.end()) {
      throw solver_exception("unrecognized solver");
    }    
    gsolver_creator<
      GraphT, 
      WeightT,
      IndexMap, 
      WeightFunctionT, 
      StoppingCriteriaT>* c = it->second;
    return (c->make_solver(g, algorithm, edgmap, s, t));
  } 

  void register_creator(
       const std::string& algorithm, 
       gsolver_creator<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>* ptr)
  {
    if (_registry.find(algorithm) != _registry.end()) {
      throw solver_exception("multiple solvers for given algorithm");
    }
    if (ptr != nullptr)
      _registry[algorithm] = ptr;
  }

 private:
  gsolver_factory();
  gsolver_factory(const gsolver_factory &) = default;
  gsolver_factory &operator=(const gsolver_factory &) = default;
  
  registry_map _registry;

};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT,
          typename Enable = void>
struct gsolver_registry
{
  static void register_compatible_solvers(
      gsolver_factory<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>* f) 
  {
    std::cerr<<"no solvers"<<std::endl;
  }
};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
struct gsolver_registry<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT,
  typename std::enable_if< 
      !(is_tuple_edge_weight<WeightT>::value) && 
      !(is_pair_edge_weight<WeightT >::value) >::type 
      >
{ 
  static void register_compatible_solvers(
      gsolver_factory<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>* f)
  {  
    f->register_creator("dijkstra", 
      new SSST_gsolver_creator<
        GraphT, 
        WeightT,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT>());
    f->register_creator("compact_dijkstra", 
      new arc_based_gsolver_creator<
        GraphT, 
        WeightT,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT>());             
/*    f->register_creator("turn_restriction_dijkstra", 
      new SSST_gsolver_creator<
        GraphT, 
        WeightT, 
        WeightFunctionT, 
        StoppingCriteriaT>());
    f->register_creator("multi_target_dijkstra", 
      new SSMT_gsolver_creator<
        GraphT, 
        WeightT,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT>());*/
  }
};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
struct gsolver_registry<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT,
  typename std::enable_if<is_pair_edge_weight<WeightT>::value>::type >
{ 
  static void register_compatible_solvers(
      gsolver_factory<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>* f)
  {  
    f->register_creator("bicriterion_epsMOA_star", 
      new BSP_gsolver_creator<
        GraphT, 
        WeightT,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT>());
  }
};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
struct gsolver_registry<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT,
  typename std::enable_if<is_tuple_edge_weight<WeightT>::value>::type >
{ 
  static void register_compatible_solvers(
      gsolver_factory<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>* f)
  {  
    // tuple weight
  }
};

template <typename GraphT, 
          typename WeightT,
          typename IndexMap,  
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
gsolver_factory<GraphT, WeightT, IndexMap, WeightFunctionT, StoppingCriteriaT>::gsolver_factory()
{
  gsolver_registry<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>::register_compatible_solvers(this);
}

}  // namespace gol

#endif  // GOL_GRAPH_SOLVER_FACTORY_H_
