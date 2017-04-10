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

#ifndef GOL_PRUN_DIJKSTRA_ALGORITHM_H_
#define GOL_PRUN_DIJKSTRA_ALGORITHM_H_

#include <boost/graph/reverse_graph.hpp>
#include <boost/pending/mutable_queue.hpp>

namespace gol {

// Dijkstra algorithm with pruning based
template < 
  typename GraphT, 
  typename Vertex, 
  typename PredMap, 
  typename DistanceMap, 
  typename WeightMap,
  typename IndexMap,
  typename ColorMap, 
  typename Visitor >
void pruning_based_dijkstra_algorithm::pruning_based_search(
    GraphT&      g, 
    Vertex       s,
    PredMap      predecessor,
    DistanceMap  distance,
    WeightMap&   weight,
    IndexMap     index_map,
    ColorMap     color,
    Visitor&     vis)
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::edge_descriptor   Edge;
  typedef typename Traits::vertex_iterator   VertexIterator;
  typedef typename Traits::out_edge_iterator EdgeIterator;
  
  typedef typename 
    boost::property_traits<ColorMap>::value_type    ColorValue;
  typedef typename 
    boost::color_traits<ColorValue>                 Color;
  typedef typename 
    boost::property_traits<DistanceMap>::value_type Distance;   

  VertexIterator ui, ui_end;
  for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) 
  {
    put(distance, *ui, std::numeric_limits<Distance>::max());
    put(predecessor, *ui, *ui);
    put(color, *ui, Color::white());
  }

  typedef typename std::less<Distance> Compare;
  Compare compare;
  typedef typename boost::closed_plus<Distance> Combine;
  Combine combine;
  typedef typename boost::indirect_cmp<DistanceMap, Compare> IndirectCmp;
  IndirectCmp icmp(distance, compare);
  typedef typename boost::relaxed_heap<Vertex, IndirectCmp, IndexMap> 
    MutableQueue;

  Edge e; bool found;

  MutableQueue Q(num_vertices(g), icmp, index_map);

  put(distance, s, 0); 
  put(color, s, Color::gray()); 
  vis.discover_vertex(s, g);             // <<
  Q.push(s);
  while (! Q.empty()) 
  {
    Vertex u = Q.top(); Q.pop();            
    put(color, s, Color::gray()); 
    vis.examine_vertex(u, g);            // <<
        
    EdgeIterator ei, ei_end;
    for (tie(ei, ei_end) = out_edges(u, g); ei != ei_end; ++ei) 
    {
      Vertex v = target(*ei, g);      

      // TODO to avoid relax operation
      //if ( /*Condition*/ )
      //  continue;
  
      if (compare(get(weight, *ei), 0))
        throw boost::negative_edge();

      vis.examine_edge(*ei, g);           // <<
      ColorValue v_color = get(color, v);

      bool decreased = false;
      if (v_color == Color::white())
      {      
        decreased = relax(*ei, g, weight, predecessor, distance,
                                combine, compare);
        if (decreased)
          vis.edge_relaxed(*ei, g);       // <<
        else
          vis.edge_not_relaxed(*ei, g);   // <<

        put(color, v, Color::gray()); 
        vis.discover_vertex(v, g);        // <<
        Q.push(v);
      } else 
      {                              
        if (v_color == Color::gray())       
        {
          decreased = relax(*ei, g, weight, predecessor, distance,
                                  combine, compare);
          if (decreased) 
          {
            Q.update(target(*ei, g));
            vis.edge_relaxed(*ei, g);     // <<
          } else
            vis.edge_not_relaxed(*ei, g); // <<
        }
        //else vis.black_target(*ei, g);
      }
    } // end for
    put(color, u, Color::black()); 
    vis.finish_vertex(u, g);              // <<
  } // end while

}   

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
void pruning_based_dijkstra_algorithm::compute(
     GraphT&      g, 
     Vertex       s, 
     Vertex       t, 
     //Heuristic    h, 
     PredMap&     pred_map, 
     DistanceMap& distance_map,
     WeightMap&   weight_map,
     Visitor&     visitor, 
     Stats&       stats) 
{
  typedef typename WeightMap::value_type WeightT;
  
  stopwatch chrono;
  std::vector<boost::default_color_type> 
    color_map(boost::num_vertices(g));

  try {
    turn_restriction_search (
        g, s, 
        &pred_map[0],
	      &distance_map[0],
	      weight_map,
        get(boost::vertex_index, g),
        &color_map[0],
	      visitor);
    throw target_not_found();  
  } catch (target_found& tf) {
    // target found
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

#endif // GOL_PRUN_DIJKSTRA_ALGORITHM_H_