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

#ifndef GOL_DECISION_MAKER_H_
#define GOL_DECISION_MAKER_H_

namespace gol {

template<typename GraphT, typename WeightT>
class decision_maker
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<WeightT, path_t> > graph_solver_result;  
 public:  
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
  decision_maker();
  ~decision_maker();
};

template<typename GraphT>
class decision_maker<GraphT, std::pair<int, int> >
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<
                  std::pair<int, int>, path_t> > graph_solver_result;  
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
      minC1_route = all.front();
    selected_routes.push_back(minC1_route);
    all.pop_front();

    if (all.empty())
      return selected_routes;

    std::pair<std::pair<int, int>, path_t> 
      minC2_route = all.back();
    selected_routes.push_back(minC2_route);

    return selected_routes;
  }

  static 
  graph_solver_result
  euclidean_distance_choice(graph_solver_result& all)
  {
    typedef std::pair<int, int> weight_t;

    double maxC1;
    double minC1;
    double maxC2;
    double minC2;

    graph_solver_result selected_routes;
    if (all.empty())
      return selected_routes;

    std::pair<weight_t, path_t> 
      minC1_route = all.front();
    selected_routes.push_back(minC1_route);
    all.pop_front();

    minC1 = minC1_route.first.first;
    maxC2 = minC1_route.first.second;

    if (all.empty())
      return selected_routes;

    std::pair<weight_t, path_t> 
      minC2_route = all.back();
    all.pop_back();

    maxC1 = minC2_route.first.first;
    minC2 = minC2_route.first.second;

    weight_t selected_current_weight = minC1_route.first;
    for(auto WP : all) 
    {
      weight_t weight = WP.first;

      // criteria values normalized to the [0,1]
      double d_C1_from_prev = (
          (weight.first - minC1) - 
          (selected_current_weight.first - minC1)
        ) / (maxC1 - minC1);      
      double d_C2_from_prev = (
          (selected_current_weight.second - minC2) - 
          (weight.second - minC2)
        ) / (maxC2 - minC2);
      double d_C1_from_last = (
          (minC2_route.first.first - minC1) - 
          (weight.first - minC1)
        ) / (maxC1 - minC1);      
      double d_C2_from_last = (
          (weight.second - minC2) - 
          (minC2_route.first.second - minC2)
        ) / (maxC2 - minC2);
      
      if ( d_C1_from_prev > EUCLIDEAN_DISTANCE_DELTA && 
           d_C2_from_prev > EUCLIDEAN_DISTANCE_DELTA &&
           d_C1_from_last > EUCLIDEAN_DISTANCE_DELTA && 
           d_C2_from_last > EUCLIDEAN_DISTANCE_DELTA) 
      {
        selected_routes.push_back(WP); 
        selected_current_weight = weight;        
      }      
    }
    selected_routes.push_back(minC2_route);
    
    return selected_routes;
  }

  static 
  graph_solver_result
  jaccard_distance_choice(graph_solver_result& all) {
    // TODO
    return all;
  }

  static 
  graph_solver_result
  euclidean_jaccard_distance_choice(graph_solver_result& all) {
    // TODO
    return all;
  }

 private:
  decision_maker();
  ~decision_maker();

};


} // namespace gol

#endif // GOL_DECISION_MAKER_H_
