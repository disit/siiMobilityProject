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

#ifndef GOL_MT_DIJKSTRA_ALGORITHM_H_
#define GOL_MT_DIJKSTRA_ALGORITHM_H_

namespace gol {

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
void multi_target_dijkstra_algorithm::compute(
     GraphT& g, 
     Vertex s, 
     std::vector<Vertex> t_vec, 
     //Heuristic h, 
     PredMap& pred_map, 
     DistanceMap& distance_map,
     WeightMap& weight_map,
     Visitor& visitor, 
     Stats& stats) 
{
  stopwatch chrono;
  try {
    boost::dijkstra_shortest_paths (g, s,
      boost::predecessor_map(&pred_map[0]).
        distance_map(&distance_map[0]).
          weight_map(weight_map).
            visitor(visitor));
    throw target_not_found();
  } catch (all_targets_found& tf) {
    // targets found
    chrono.lap();
    stats.run_time = chrono.partial_wall_time();
#ifdef DEBUG
  logger(logDEBUG) 
    << left("[dijkstra]", 14) 
    << left(">", 3);       
  logger(logDEBUG) 
    << left("[dijkstra]", 14) 
    << left(">", 3) 
    << center("Visited Nodes:", 20) 
    << " | " << stats.visited_nodes;
  logger(logDEBUG) 
    << left("[dijkstra]", 14) 
    << left(">", 3) 
    << std::string(20 + 40 + 2*2, '-');
  logger(logDEBUG) 
    << left("[dijkstra]", 14) 
    << left(">", 3) 
    << center(" ", 20) << "  " 
    << right("run-time: " + prd(stats.run_time, 5) + "s", 40);    
#endif     
  }  

}

}  // namespace gol

#endif // GOL_MT_DIJKSTRA_ALGORITHM_H_