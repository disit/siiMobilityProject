#ifndef GOL_GRAPH_SOLVER_H_
#define GOL_GRAPH_SOLVER_H_

#include "graph_weight_function.h"
#include "graph_heuristic.h"
#include "graph_make_edge_weight.h"

namespace gol {

/**
*
*/
template <typename GraphT, 
          typename WeightT, 
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class graph_solver
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor edge_descriptor;
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor> path_t;
  typedef std::list<std::pair<WeightT, path_t> > graph_solver_result;
 
 public:
  virtual void solve(
    WeightFunctionT weight_function, 
    StoppingCriteriaT stopping_criteria) = 0;

 private:
  virtual graph_solver_result get_result() = 0;    

 protected:
  graph_solver(GraphT& g) : _g(g), _stats() {}
  ~graph_solver() {} 
  
  GraphT& _g;
  struct stats_t _stats; 
 
 public:
  optimized_routes get_optimized_routes()
  {
    optimized_routes opt;
    for (auto length_path_KV : get_result())
    { 
      Route route;
      path_t path = length_path_KV.second;
      for (edge_descriptor e : path) 
      {
        route.add_route_edge(
          edge_weight_adaptor<WeightT>::to_length(_g[e].weight),
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

  unsigned int get_visited_nodes() { return _stats.visited_nodes; }
  double get_run_time() { return _stats.run_time; }

};	

} // namespace gol

#include "graph_solver/single_source_single_target_solver.h"
#include "graph_solver/single_source_multi_target_solver.h"
#include "graph_solver/bicrteria_single_source_solver.h"


#endif // GOL_GRAPH_SOLVER_H_