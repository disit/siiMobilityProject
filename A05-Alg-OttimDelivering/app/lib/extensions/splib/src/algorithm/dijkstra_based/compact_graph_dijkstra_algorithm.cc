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

#ifndef GOL_ARCB_DIJKSTRA_ALGORITHM_H_
#define GOL_ARCB_DIJKSTRA_ALGORITHM_H_

#include <boost/graph/reverse_graph.hpp>
#include <boost/pending/mutable_queue.hpp>

#include "../../graph/graph_compact_relax.h"
#include "../../graph/graph_edge_index_indirect_cmp.h"

namespace gol {

template <
  typename GraphT,
  typename Vertex,
  typename PredMap,
  typename DistanceMap,
  typename WeightMap,
  typename IndexMap,
  typename ColorMap,
  typename Visitor >
void compact_graph_dijkstra_algorithm::arc_based_search(
    GraphT&      g,
    Vertex       s,
    PredMap      predecessor,
    DistanceMap  distance,
    WeightMap&   weight,
    IndexMap&    index_map,
    ColorMap     color,
    Visitor&     vis)
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::edge_descriptor   Edge;
  typedef typename Traits::vertex_iterator   VertexIterator;
  typedef typename Traits::edge_iterator     EdgeIterator;
  typedef typename Traits::out_edge_iterator OutEdgeIterator;
  typedef typename Traits::in_edge_iterator  InEdgeIterator;
  typedef typename
    boost::property_traits<ColorMap>
      ::value_type                           ColorValue;
  typedef typename
    boost::color_traits<ColorValue>          Color;
  typedef typename
    boost::property_traits<DistanceMap>
      ::value_type                           Distance;

  EdgeIterator ei, ei_end;
  for (tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
  {
    put(distance, g[*ei].edge_index, std::numeric_limits<Distance>::max());
    put(predecessor, g[*ei].edge_index, g[*ei].edge_index);
    put(color, g[*ei].edge_index, Color::white());
  }

  typedef typename std::less<Distance>          Compare;
  typedef typename boost::closed_plus<Distance> Combine;
  typedef edge_index_indirect_cmp<
                  GraphT, DistanceMap, Compare> IndirectCmp;
  typedef typename boost::relaxed_heap<
                 Edge, IndirectCmp, IndexMap>   MutableQueue;

  Compare      compare;
  Combine      combine;
  IndirectCmp  icmp(g, distance, compare);
  MutableQueue Q(boost::num_edges(g), icmp, index_map);

  /*InEdgeIterator iei, iei_end;
  tie(iei, iei_end) = boost::in_edges(s, g);
  if ( iei == iei_end)
    throw solver_exception(
      "arc_based_search(): no incoming edges");

  Edge source_incoming_e = *iei;

  put(distance, g[source_incoming_e].edge_index, 0);
  put(color, g[source_incoming_e].edge_index, Color::gray());
  vis.discover_vertex(s, g);               // <<
  Q.push(source_incoming_e);*/

  OutEdgeIterator soei, soei_end;
  for (tie(soei, soei_end) = out_edges(s, g); soei != soei_end; ++soei) 
  {
    put(distance, g[*soei].edge_index, 0);
    put(color, g[*soei].edge_index, Color::gray());
    vis.discover_vertex(s, g);             // <<
    Q.push(*soei);
  }

  while (! Q.empty())
  {
    Edge incoming_e = Q.top(); Q.pop();
    Vertex u = boost::target(incoming_e, g);
    vis.examine_vertex(u, g);              // <<

    OutEdgeIterator oei, oei_end;
    for (tie(oei, oei_end) = out_edges(u, g); oei != oei_end; ++oei)
    {
      Vertex v = boost::target(*oei, g);

      if (compare(get(weight, *oei), 0))
        throw boost::negative_edge();

      vis.examine_edge(*oei, g);           // <<

      ColorValue v_color = get(color, g[*oei].edge_index);

      bool decreased = false;
      if (v_color == Color::white())
      {
        decreased = compact_relax( incoming_e, *oei, g, weight,
                                  predecessor, distance, combine, compare);
        if (decreased)
          vis.edge_relaxed(*oei, g);       // <<
        else
          vis.edge_not_relaxed(*oei, g);   // <<

        put(color, g[*oei].edge_index, Color::gray());
        vis.discover_vertex(v, g);         // <<
        Q.push(*oei);
      }
      else {
        if (v_color == Color::gray())
        {
          decreased = compact_relax( incoming_e, *oei, g, weight,
                                    predecessor, distance, combine, compare);
          // TODO insert stalling pruning
          if (decreased)
          {
            Q.update(*oei);
            vis.edge_relaxed(*oei, g);     // <<
          } else
            vis.edge_not_relaxed(*oei, g); // <<
        }
        //else vis.black_target(*oei, g);
      }
    } // end for
    put(color, g[incoming_e].edge_index, Color::black());
    vis.finish_vertex(u, g);               // <<
  } // end while

}

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
void compact_graph_dijkstra_algorithm::compute(
     GraphT&      g,
     Vertex       s,
     Vertex       t,
     //Heuristic    h,
     PredMap&     pred_map,
     DistanceMap& distance_map,
     WeightMap&   weight_map,
     IndexMap&    index_map,
     Visitor&     visitor,
     Stats&       stats)
{
  typedef typename WeightMap::value_type WeightT;

  stopwatch chrono;
  std::vector<boost::default_color_type>
    color_map(boost::num_edges(g));

  try {
    arc_based_search (
        g, s,
        &pred_map[0],
	      &distance_map[0],
	      weight_map,
        index_map,
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
    << center("Visited Edges:", 20)
    << " | " << stats.visited_nodes; // labels are edges
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

#endif // GOL_ARCB_DIJKSTRA_ALGORITHM_H_
