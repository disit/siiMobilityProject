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

#ifndef GOL_ALGORITHM_H_
#define GOL_ALGORITHM_H_

// std
#include <list>
#include <vector>
#include <deque>
#include <numeric>
#include <stdint.h>
#include <utility>
// boost
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/astar_search.hpp>
// sii_mobility/round_based
#include "round_based/raptor_timetable.h"
// sii_mobility/graph
#include "graph/graph_edge_weight_traits.h" 
#include "graph/graph_stopping_criteria.h"
#include "graph/graph_heuristic.h"

namespace gol {

/*template<typename GraphT, typename WeightT> 
struct compare 
{  
  bool operator()(WeightT u, WeightT v) const {}
};*/
  
class dijkstra_algorithm 
{
 public:
  static std::string get_name() { 
    return "Dijkstra"; }

template <
  typename GraphT,
  typename Vertex, 
  //typename Heuristic, 
  typename PredMap, 
  typename DistanceMap, 
  typename WeightMap,
  //typename Compare,  
  typename Visitor,  
  typename Stats>
  static void compute(
      GraphT&      g, 
      Vertex       s, 
      Vertex       t, 
      //Heuristic    h, 
      PredMap&     pred_map, 
      DistanceMap& distance_map, 
      WeightMap&   weight_map,
      Visitor&     visitor,  
      Stats&       stats);

 private:
  dijkstra_algorithm();
  ~dijkstra_algorithm();

};

class multi_target_dijkstra_algorithm 
{
 public:
  static std::string get_name() { 
    return "Multi Target Dijkstra"; }

template <
  typename GraphT,
  typename Vertex, 
  //typename Heuristic, 
  typename PredMap, 
  typename DistanceMap, 
  typename WeightMap,
  //typename Compare,  
  typename Visitor,  
  typename Stats>
  static void compute(
      GraphT&             g, 
      Vertex              s, 
      std::vector<Vertex> t_vec, 
      //Heuristic           h, 
      PredMap&            pred_map, 
      DistanceMap&        distance_map, 
      WeightMap&          weight_map,
      Visitor&            visitor, 
      Stats&              stats);

 private:
  multi_target_dijkstra_algorithm();
  ~multi_target_dijkstra_algorithm();

};

class compact_graph_dijkstra_algorithm 
{
 public:
  static std::string get_name() { 
    return "Turn Costs Arc-Based Dijkstra"; }

  template <
    typename GraphT,
    typename Vertex, 
    //typename Heuristic, 
    typename PredMap, 
    typename DistanceMap, 
    typename WeightMap,
    typename IndexMap,
    typename Visitor,  
    typename Stats>
  static void compute(
      GraphT&      g, 
      Vertex       s,
      Vertex       t,  
      //Heuristic    h, 
      PredMap&     pred_map, 
      DistanceMap& distance_map, 
      WeightMap&   weight_map,
      IndexMap&    edge_index_map,
      Visitor&     visitor,  
      Stats&       stats);

  template < 
    typename GraphT, 
    typename Vertex, 
    typename PredMap, 
    typename DistanceMap, 
    typename WeightMap, 
    typename IndexMap,
    typename ColorMap, 
    typename Visitor >
  static void arc_based_search(
      GraphT&     g, 
      Vertex      s,
      PredMap     predecessor,
      DistanceMap distance,
      WeightMap&  weight,
      IndexMap&   index_map,
      ColorMap    color,
      Visitor&    vis);  

 private:
  compact_graph_dijkstra_algorithm();
  ~compact_graph_dijkstra_algorithm();

};

class pruning_based_dijkstra_algorithm 
{
 public:
  static std::string get_name() { 
    return "Speed-Up Dijkstra"; }

  template <
    typename GraphT,
    typename Vertex, 
    //typename Heuristic, 
    typename PredMap, 
    typename DistanceMap, 
    typename WeightMap, 
    typename Visitor,  
    typename Stats>
  static void compute(
      GraphT&      g, 
      Vertex       s,
      Vertex       t,  
      //Heuristic    h, 
      PredMap&     pred_map, 
      DistanceMap& distance_map, 
      WeightMap&   weight_map,
      Visitor&     visitor,  
      Stats&       stats);

  // Dijkstra algorithm with pruning on relaxed edges
  template < 
    typename GraphT, 
    typename Vertex, 
    typename PredMap, 
    typename DistanceMap, 
    typename WeightMap, 
    typename IndexMap,
    typename ColorMap, 
    typename Visitor >
  static void pruning_based_search(
      GraphT&     g, 
      Vertex      s,
      PredMap     predecessor,
      DistanceMap distance,
      WeightMap&  weight,
      IndexMap    index_map,
      ColorMap    color,
      Visitor&    vis);  

 private:
  pruning_based_dijkstra_algorithm();
  ~pruning_based_dijkstra_algorithm();

};

class bicriterion_epsMOA_star_algorithm 
{
 public:
  static std::string get_name() { 
    return "MOA* Stewart + epsilon-approximation"; }

  template <
    typename BiGraphT, 
    typename Vertex, 
    typename Heuristic, 
    typename ParetoSet, 
    typename WeightMap, 
    typename Stats>
  static void compute(
      BiGraphT&  g, 
      Vertex     s, 
      Vertex     t, 
      Heuristic  H, 
      ParetoSet& pareto_set,
      WeightMap& weight_map,  
      Stats&     stats);

 private:
  bicriterion_epsMOA_star_algorithm();
  ~bicriterion_epsMOA_star_algorithm();

  template <typename LabelSet, typename WeightT>
  static bool epsilon_approximation_merge(
      LabelSet&       Y, 
      const LabelSet& X, 
      const WeightT&  len, 
      double          epsilon);

  template <typename LabelSet>
  static bool target_pruning(
      const LabelSet& target, 
      const LabelSet& X);

  template <typename WeightT, typename LabelSet>
  static bool contains(
      const WeightT&  elem, 
      const LabelSet& s);

  template <typename LabelSet, typename Stats>
  static void dump(LabelSet& Gt, Stats& stats);  

}; 

class RAPTOR_algorithm {

 public:
  static std::string get_name() { 
    return "Raound-Based Public Transit Optimized Route"; }

  static void compute(
      timetable_Rt& timetable, 
      std::vector<
          std::pair<std::string, time_Rt> 
      >& near_stops_src, 
      std::vector<
          std::pair<std::string, time_Rt> 
      >& near_stops_trg, 
      round_based_solver_result& pareto_set, 
      uint8_t n_transfers);

 private:
  RAPTOR_algorithm();
  ~RAPTOR_algorithm();

  static void raptor_resource_allocation(
      data_Rt& rdata, 
      timetable_Rt& timetable, 
      uint8_t n_rounds);
 
  static void raptor_resource_release(
      data_Rt& rdata);

  static uint32_t get_route_stops_index(
      timetable_Rt& timetable, 
      uint32_t ridx, 
      uint32_t sidx); 
      
  static bool is_earliest_marked_stop_in_route(
      data_Rt& rdata, 
      uint32_t ridx, 
      uint32_t sidx);

  static void accumulate_routes_for_stop(
      data_Rt& rdata,
      timetable_Rt& timetable, 
      uint32_t sidx);

   static void look_at_foot_paths(
      data_Rt& rdata,
      timetable_Rt& timetable,   
      uint8_t round);   

  static std::pair<uint32_t, time_Rt> get_earlier_trip(
      timetable_Rt& timetable, 
      uint32_t ridx, 
      uint32_t trip_sidx, 
      int prev_time);

  static void round(
      data_Rt& rdata,
      timetable_Rt& timetable,
      uint8_t round);

  static round_based_solver_result get_transit_route(
      data_Rt& rdata, 
      timetable_Rt& timetable, 
      uint8_t n_rounds,
      round_based_solver_result& pareto_set); 

  static void raptor_initialization(
      data_Rt& rdata,
      timetable_Rt& timetable, 
      const std::vector<
          std::pair<std::string, time_Rt> 
      >& near_stops_src,
      const std::vector<
          std::pair<std::string, time_Rt> 
      >& near_stops_trg);

  static void dump(
      round_based_solver_result& pareto_set, 
      timetable_Rt& timetable);  


}; 


}  // namespace gol

// workaround waiting for template compilation
#include "algorithm/dijkstra_based_algorithm.cc"
#include "algorithm/bicriterion_epsMOA_star_algorithm.cc"

// algorithm.cc 
//#include "algorithm/RAPTOR_algorithm.cc"
 

#endif  // GOL_ALGORITHM_H_
