#ifndef GOL_GRAPH_SSST_SOLVER_H_
#define GOL_GRAPH_SSST_SOLVER_H_

namespace gol {

/**
*  
*/ 
template <typename GraphT, 
          typename WeightT,
          typename SPAlgorithm, 
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class SSST_gsolver : 
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
  SSST_gsolver( 
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
      	_pred_map(boost::num_vertices(g)), 
        _distance_map(boost::num_vertices(g)){}
  ~SSST_gsolver() {}
    
  virtual void solve(
      WeightFunctionT   weight_function, 
      StoppingCriteriaT stopping_criteria) override 
  {   
    stopping_criteria.add_stats( &(this->_stats) );          
    SPAlgorithm::compute(
      this->_g, 
      _s,
      _t, 
      // h, 
      _pred_map, 
      _distance_map,
      weight_function,
      stopping_criteria,
      this->_stats);
  }
    
  virtual graph_solver_result get_result() override 
  {
    path_t path;
    if (_pred_map[_t] == _t) {
      throw solver_exception(
        "get_result(): Not path to target");
    }
    for (vertex_descriptor v = _t; _pred_map[v] != v; v = _pred_map[v]) 
    {
      edge_descriptor e; bool found;
      boost::tie(e, found) = boost::edge(_pred_map[v], v, (this->_g));
      if (found) {
        path.push_front(e);      
      } else {
        throw solver_exception(
          "get_result(): Edge not found");
      }
    }
    graph_solver_result res = 
        {std::make_pair(_distance_map[_t], path)};
    return res;
  }
    
 private:
  vertex_descriptor              _s;
  vertex_descriptor              _t;
  std::vector<vertex_descriptor> _pred_map;
  std::vector<WeightT>           _distance_map;

};	

} // namespace gol

#endif // GOL_GRAPH_SSST_SOLVER_H_