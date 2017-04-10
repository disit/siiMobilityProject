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

#ifndef GOL_GRAPH_VISITOR_H_
#define GOL_GRAPH_VISITOR_H_

//std
#include <vector>
#include <stdint.h>

#include "../common.h"

namespace gol {

template<typename GraphT>
class null_stopping_criteria: 
  public boost::default_dijkstra_visitor 
{
  typedef boost::graph_traits<GraphT> Traits;
 public:
  null_stopping_criteria() {}

  void stats_initialization(stats_t* pt) {}  

  void examine_vertex(
      const typename Traits::vertex_descriptor v, 
      const GraphT& g) {}
    
};

// visitor that terminates when we find the target
template<typename GraphT>
class target_dijkstra_stopping_criteria: 
  public boost::default_dijkstra_visitor 
{
  typedef boost::graph_traits<GraphT> Traits; 
 public:
  target_dijkstra_stopping_criteria(
      const typename Traits::vertex_descriptor target): 
        _target(target), 
        _stats(nullptr) {}
  
  void stats_initialization(stats_t* pt) 
  { 
    _stats = pt;
    if (_stats != nullptr)
      _stats->visited_nodes = 0; 
  }

  void examine_vertex(
      const typename Traits::vertex_descriptor v, 
      const GraphT& g) 
  {
    if (_stats != nullptr)
    _stats->visited_nodes++;

    if (v == _target) {
      throw target_found();
    }
  }

 private:
  typename Traits::vertex_descriptor _target;
  struct   stats_t*                  _stats;
    
};

// visitor that terminates when we find all targets
template<typename GraphT>
class multi_target_dijkstra_stopping_criteria: 
  public boost::default_dijkstra_visitor 
{
  typedef boost::graph_traits<GraphT> Traits;
 public:
  multi_target_dijkstra_stopping_criteria(
      const typename std::vector<
          typename Traits::vertex_descriptor>& targets): 
        _targets(targets),
        _targets_found(0),
        _n_targets(targets.size()), 
        _stats(nullptr) {}

  void stats_initialization(stats_t* pt) 
  { 
    _stats = pt;
    if (_stats != nullptr)
      _stats->visited_nodes = 0; 
  }
    
  void examine_vertex(
      const typename Traits::vertex_descriptor v, 
      const GraphT& g) 
  {
    if (_stats != nullptr)
    _stats->visited_nodes++;
    
    uint32_t i     = 0; 
    bool     found = false;

    while ( !found && (i < _targets.size()) ) {
      if (_targets[i] == v) {
        _targets_found++;
        _targets.erase(_targets.begin() + i);
        if (_targets_found ==_n_targets)
            throw all_targets_found();
        found = true;
      }
      else i++;
    }
  } 

 private:
  typename std::vector<
    typename Traits::vertex_descriptor
    >             _targets;
  uint32_t        _targets_found;
  uint32_t        _n_targets; 
  struct stats_t* _stats;
    
};

//// TODO
template<typename GraphT, typename WeightT>
class neighborhood_target_dijkstra_stopping_criteria: 
  public boost::default_dijkstra_visitor 
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
 public:

  neighborhood_target_dijkstra_stopping_criteria(
      time_Rt begin_time,
      std::vector<std::pair<std::string, time_Rt> >& near_stops,
      timetable_Rt& timetable,
      std::vector<WeightT>& dmap,
      double radius)
      : _btime(begin_time),
        _near_stops(near_stops),
        _timetable(timetable),
        _radius(radius),
        _dmap(dmap),
        _stats() { _stats.visited_nodes = 0; }
    
  void examine_vertex(
      const typename boost::graph_traits<GraphT>::vertex_descriptor v, 
      const GraphT& g) 
  {
    _stats.visited_nodes++;
    
    if (_dmap[v] > _radius) 
      throw target_found(); 

    // WARNING: is a performance problem?
    auto it = _timetable.stopidx_map.find( g[v].id );
    if (it != _timetable.stopidx_map.end()) {
      _near_stops.push_back( 
         std::make_pair(
           g[v].id, (_btime + (int)(_dmap[v] / AVERAGE_WALKING_SPEED)) ));
    } 

  }

 private:
  std::vector<
     std::pair<std::string, time_Rt> >& _near_stops;
  time_Rt               _btime;
  timetable_Rt&         _timetable;
  std::vector<WeightT>& _dmap;
  double                _radius;
  struct stats_t        _stats;
    
}; 

// visitor explore in a neighbourhood of source to find near stops
// WARNING: use if stop identifiers aren't in user request  
/*template<typename GraphT, typename WeightT>
class near_stops_dijkstra_visitor : public boost::default_dijkstra_visitor 
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
 public:

  near_stops_dijkstra_visitor(
      time_Rt begin_time,
      std::vector<std::pair<std::string, time_Rt> >& near_stops,
      timetable_Rt& timetable,
      std::vector<WeightT>& dmap,
      double radius)
      : _btime(begin_time),
        _near_stops(near_stops),
        _timetable(timetable),
        _radius(radius),
        _dmap(dmap),
        _stats() { _stats.visited_nodes = 0; }
    
  void examine_vertex(
      const typename boost::graph_traits<GraphT>::vertex_descriptor v, 
      const GraphT& g) 
  {
    _stats.visited_nodes++;
    
    if (_dmap[v] > _radius) 
      throw target_found(); 

    // WARNING: is a performance problem?
    auto it = _timetable.stopidx_map.find( g[v].id );
    if (it != _timetable.stopidx_map.end()) {
      _near_stops.push_back( 
         std::make_pair(g[v].id, 
          (_btime + (int)(_dmap[v] / AVERAGE_WALKING_SPEED)) ));
    } 

  }

 private:
  std::vector<
     std::pair<std::string, time_Rt> >& _near_stops;
  time_Rt               _btime;
  timetable_Rt&         _timetable;
  std::vector<WeightT>& _dmap;
  double                _radius;
  struct stats_t        _stats;
    
};*/ 

} // namespace gol

#endif  // GOL_GRAPH_VISITOR_H_
