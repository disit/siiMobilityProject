
#ifndef GOL_GRAPH_HEURISTIC_H_
#define GOL_GRAPH_HEURISTIC_H_

#include "../common.h"
//#include "generic_edge_weighted_graph.h" 

namespace gol {

// dummy heuristics that always returns 0, 
// when A* is used this way it falls back to a standard Dijkstra search.
template <typename GraphT, typename WeightT>
class astar_zero_heuristic : 
  public boost::astar_heuristic<GraphT, WeightT> 
{
 public:
  typedef typename boost::graph_traits<GraphT>::vertex_descriptor vertex_descriptor;
  
  astar_zero_heuristic() {}  
  WeightT operator()(vertex_descriptor) { return 0; }
};

// bicriterion heuristic
template < typename BiGraphT, typename BiWeightT>
class heuristic : public boost::astar_heuristic<BiGraphT, BiWeightT>{
 public:
  typedef typename boost::graph_traits<BiGraphT>::vertex_descriptor vertex_descriptor;
  //virtual BiWeightT operator()(vertex_descriptor u) const = 0;
  BiWeightT operator()(vertex_descriptor) const { return BiWeightT(); }

  heuristic() {};
  ~heuristic() {};
};

template <typename BiGraphT, typename BiWeightT>
class null_heuristic : public heuristic<BiGraphT, BiWeightT> {
 public:
  typedef typename boost::graph_traits<BiGraphT>::vertex_descriptor vertex_descriptor;
  null_heuristic(): heuristic<BiGraphT, BiWeightT>() {}
  BiWeightT operator()(vertex_descriptor) const { return std::make_pair(0, 0); }
};

/** \brief Heuristic functor.
 *
 * This class holds a reference to a target node and a graph and
 * returns the euclidean distance of a node to the target.
 *
 * The vertex must have a position.
 *
 */
template <typename BiGraphT, typename BiWeightT>
class distance_heuristic : public heuristic<BiGraphT, BiWeightT> {
 public:
  typedef typename boost::graph_traits<BiGraphT>::vertex_descriptor vertex;
  distance_heuristic(BiGraphT& graph, vertex target) : g(graph), t(target) {}
  
  BiWeightT operator()(vertex u) const {
    double dx = g[t].lon - g[u].lon;
    double dy = g[t].lat - g[u].lat;
    int second = std::numeric_limits<int>::max();
    typename boost::graph_traits<BiGraphT>::out_edge_iterator ei, ei_end;
    if(u!=t)
      for(boost::tie(ei, ei_end) = boost::out_edges(u, g); 
	        ei != ei_end; ++ei)
	      second = std::min(second, g[*ei].weight.second);
    else
      second = 0;
    return std::make_pair(std::floor(::sqrt(dx*dx+dy*dy)), second);
  }

 private:
  BiGraphT& g;
  vertex t;

};

} // namespace gol

#endif //GOL_HEURISTIC_H_
