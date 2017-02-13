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
    << left("[emoa*]", 14) 
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