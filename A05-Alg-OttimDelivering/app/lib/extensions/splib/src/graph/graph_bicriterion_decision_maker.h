#ifndef GOL_DECISION_MAKER_H_
#define GOL_DECISION_MAKER_H_

namespace gol {

template<typename GraphT, typename WeightT>
class bicriterion_decision_maker
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<WeightT, path_t> > graph_solver_result;  
 public:  
  // bicriterion solution is already sorted
  static 
  graph_solver_result
  best_single_objective_choice(graph_solver_result& all){
    return all;
  }

  static 
  graph_solver_result
  euclidean_distance_choice(graph_solver_result& all){
    return all;
  }

  static 
  graph_solver_result
  jaccard_distance_choice(graph_solver_result& all){
    return all;
  }

  static 
  graph_solver_result
  euclidean_jaccard_distance_choice(graph_solver_result& all){
    return all;
  }

 private:
  bicriterion_decision_maker();
  ~bicriterion_decision_maker();
};

template<typename GraphT>
class bicriterion_decision_maker<GraphT, std::pair<int, int> >
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<
     std::pair<int, int>, path_t> >              graph_solver_result;  
 public:

  // bicriterion solution is already sorted
  static 
  graph_solver_result
  best_single_objective_choice(graph_solver_result& all)
  {
    graph_solver_result selected_routes;
    if (all.empty())
      return selected_routes;

    std::pair<std::pair<int, int>, path_t> 
      best_first_criteria = all.front();
    selected_routes.push_back(best_first_criteria);
    all.pop_front();

    if (all.empty())
      return selected_routes;

    std::pair<std::pair<int, int>, path_t> 
      best_second_criteria = all.back();
    selected_routes.push_back(best_second_criteria);

    return selected_routes;
  }

  static 
  graph_solver_result
  euclidean_distance_choice(graph_solver_result& all)
  {
    double max_first_criteria;
    double max_second_criteria;

    graph_solver_result selected_routes;
    if (all.empty())
      return selected_routes;

    std::pair<std::pair<int, int>, path_t> 
      best_first_criteria = all.front();
    selected_routes.push_back(best_first_criteria);
    max_second_criteria = best_first_criteria.first.second;
    all.pop_front();

    if (all.empty())
      return selected_routes;

    std::pair<std::pair<int, int>, path_t> 
      best_second_criteria = all.back();
    selected_routes.push_back(best_second_criteria);
    max_first_criteria = best_second_criteria.first.first;
    all.pop_back();

    std::pair<int, int> current = best_first_criteria.first;
    for(auto KV : all) {
      std::pair<int, int> weight = KV.first;
      double first_distance = ((weight.first/max_first_criteria) - (current.first/max_first_criteria));
      double second_distance = ((weight.second/max_second_criteria) - (current.second/max_second_criteria));
      if ( first_distance > 0.2 && second_distance > 0.2)
      { 
       selected_routes.push_back(KV); 
       current = weight; 
      }
       
    }

    return selected_routes;
  }

  static 
  graph_solver_result
  jaccard_distance_choice(graph_solver_result& all)
  {
    graph_solver_result selected_routes;
    return selected_routes;
  }

  static 
  graph_solver_result
  euclidean_jaccard_distance_choice(graph_solver_result& all)
  {
    graph_solver_result selected_routes;
    return selected_routes;
  }

 private:
  bicriterion_decision_maker();
  ~bicriterion_decision_maker();

};


} // namespace gol

#endif // GOL_DECISION_MAKER_H_
