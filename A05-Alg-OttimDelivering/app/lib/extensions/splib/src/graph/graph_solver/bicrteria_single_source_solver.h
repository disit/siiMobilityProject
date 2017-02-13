#ifndef GOL_GRAPH_BSS_SOLVER_H_
#define GOL_GRAPH_BSS_SOLVER_H_

#include "../graph_decision_maker.h"

namespace gol {

/**
*  
*/ 
template <typename GraphT, 
          typename WeightT,
          typename BiSPAlgorithm, 
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class BSP_gsolver: 
public graph_solver<
    GraphT, 
    WeightT, 
    WeightFunctionT, 
    StoppingCriteriaT> 
{
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
           WeightFunctionT, 
           StoppingCriteriaT>(g),      
        _s(source), 
        _t(target), 
        _pareto_set() {}
  ~BSP_gsolver() {}
    
  virtual void solve(
      WeightFunctionT weight_function, 
      StoppingCriteriaT /*stopping_criteria*/) override 
  {
    heuristic<GraphT, WeightT>* h = new null_heuristic<GraphT, WeightT>(); 
    BiSPAlgorithm::compute(
      this->_g, 
      _s,
      _t, 
      *h, 
      _pareto_set,
      weight_function,
      this->_stats);
    
    delete h;
  }

  virtual graph_solver_result get_result() override 
  {   
    return decision_maker<GraphT, WeightT>::
       euclidean_distance_choice(_pareto_set);
       //best_single_objective_choice(_pareto_set); 
  }  
    
 private:
  vertex_descriptor   _s;
  vertex_descriptor   _t;
  graph_solver_result _pareto_set;

};

} // namespace gol

#endif // GOL_GRAPH_BSS_SOLVER_H_