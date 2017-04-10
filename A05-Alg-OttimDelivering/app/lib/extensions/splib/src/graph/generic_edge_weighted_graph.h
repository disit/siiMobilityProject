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

#ifndef GOL_GENERIC_EDGE_WEIGHTED_GRAPH_H_
#define GOL_GENERIC_EDGE_WEIGHTED_GRAPH_H_

// std
#include <iostream>
#include <utility>
#include <stdint.h>
#include <tuple>
#include <memory>
#include <functional>
// boost
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/utility.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/config.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/multi_array.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "../config.h"
#include "../utility.h"
#include "../exception.h"
#include "../utils/stopwatch.h"
#include "../utils/logger.h"
#include "../utils/bitset.h"

#include "graph_serialization_multi_array.h"
#include "graph_constraints.h"
#include "../cache.h"

#include "graph_builder_factory.h"
#include "graph_solver_factory.h"

#include "../round_based/raptor_timetable.h"

namespace gol {       

template <
   typename ExtraVertexProperties, 
   typename ExtraEdgeProperties, 
   typename... WeightsT
   >
class generic_edge_weighted_graph_t 
{ 
  typedef typename 
    graph_weight_traits<WeightsT... >::edge_weight_t weight_t;
  typedef typename 
    graph_weight_traits<WeightsT... >::boost_graph_t boost_graph_t;

  struct point_t
  {
    double lon; // x
    double lat; // y
    double ele; // z

    friend class boost::serialization::access;
    // when the class Archive corresponds to an output archive, the
    // & operator is defined similar to << likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.    
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) 
    {
      ar &
        BOOST_SERIALIZATION_NVP(lon) & 
        BOOST_SERIALIZATION_NVP(lat) &
        BOOST_SERIALIZATION_NVP(ele);
    }

    friend std::ostream& operator<<(std::ostream &os, const point_t& p) {
      return os << "node.geo [ lon : " 
                << p.lon << ", lat : " 
                << p.lon << ", ele : " 
                << p.ele << " ]" 
                << std::endl;
    }; 
  }; 

  struct vertex_properties_t;
  struct edge_properties_t;    

private:
  struct graph_properties_t 
  {
    std::string graph_id;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & 
        BOOST_SERIALIZATION_NVP(graph_id);
    }
  };

  typedef boost::adjacency_list<
      boost::setS, 
      boost::vecS, 
      boost_graph_t,       // directed, bidirectional
      vertex_properties_t, 
      edge_properties_t,
      graph_properties_t
  > graph_t;

  typedef boost::graph_traits<graph_t>        traitsT;
  typedef typename traitsT::vertex_descriptor vertex_descriptor;
  typedef typename traitsT::edge_descriptor   edge_descriptor;
  typedef typename traitsT::edge_iterator     edge_iterator;
  typedef typename traitsT::out_edge_iterator out_edge_iterator;
  typedef typename traitsT::in_edge_iterator  in_edge_iterator;   

  // model turn costs
  typedef boost::multi_array<weight_t, 2> turn_table_t;  

  struct vertex_properties_t 
  {
    std::string id;  // move to vertex_map
    point_t     geo;

    turn_table_t* turn_table = 0;  

    ExtraVertexProperties properties;
 
    friend class boost::serialization::access;  
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) 
    {     
      if (!turn_table) turn_table = new turn_table_t();
      ar &
        BOOST_SERIALIZATION_NVP(id)         &
        BOOST_SERIALIZATION_NVP(geo)        &
        BOOST_SERIALIZATION_NVP(turn_table) & 
        BOOST_SERIALIZATION_NVP(properties);
    }

  };

  struct edge_properties_t 
  {
    edge_index_t edge_index;
    weight_t     weight;
    
    // turn table indexes
    unsigned short int entry_point = 0;
    unsigned short int exit_point  = 0;

    ExtraEdgeProperties properties;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar &
        BOOST_SERIALIZATION_NVP(edge_index)  &       
        BOOST_SERIALIZATION_NVP(weight)      &
        BOOST_SERIALIZATION_NVP(entry_point) &
        BOOST_SERIALIZATION_NVP(exit_point)  &        
        BOOST_SERIALIZATION_NVP(properties);
    }
  };  

  struct mapped_vertex_ord 
  {
    bool operator()(const std::string& v, const std::string& u) const {
      return v.compare(u) < 0;
    }
  };

  typedef std::map<
      std::string, 
      vertex_descriptor, 
      mapped_vertex_ord
  > vertex_map;

  // typedef uint32_t edge_index_t; //see utility.cc
  typedef std::map<
      edge_index_t, 
      edge_descriptor 
      //mapped_edge_ord
  > edge_map;  

  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>              path_t;
  typedef std::list<std::pair<weight_t, path_t> > gRoute;

  graph_t                _g;
  vertex_map             _vtxmap;
  edge_map               _edgmap;
  graph_constraints_t<
    graph_t, weight_t >  _constraints;
  size_t                 _n_weights;
  std::string            _model;

public:
  generic_edge_weighted_graph_t(): 
      _g(), 
      _vtxmap(),
      _edgmap(),
      _constraints(_g),
      _n_weights(sizeof...(WeightsT)),
      _model() {};

  ~generic_edge_weighted_graph_t() {} 

  void create_model(std::string model, std::string filename) 
  {
    graph_builder<
        graph_t, 
        vertex_map,
        edge_map,
        graph_constraints_t<graph_t, weight_t >, 
        weight_t>* gbuilder = 
      graph_builder_factory<
        graph_t, 
        vertex_map, 
        edge_map,   
        graph_constraints_t<graph_t, weight_t >, 
        weight_t>::get_builder_for(
          model, _g, _vtxmap, _edgmap, _constraints);
    
    _model = model;
    cache::retrieve_road_network(
      &_g, &_vtxmap, &_edgmap, &_constraints, gbuilder, filename, model);  
  } 
  
  template <typename WeightFunctionT, 
            typename StoppingCriteriaT = null_stopping_criteria<graph_t> >
  optimized_routes apply_solver(
      std::string       algorithm, 
      std::string       source, 
      std::string       target,
      WeightFunctionT   weight_function,
      StoppingCriteriaT stopping_criteria = null_stopping_criteria<graph_t>()) 
  {          
    // edge index map
    typedef typename boost::property_map<
      graph_t, edge_index_t edge_properties_t::*>::type IndexMap; 

    IndexMap edge_index_map = boost::get(&edge_properties_t::edge_index, _g);  

    if( (_vtxmap).find(source) == _vtxmap.end() || 
        (_vtxmap).find(target) == _vtxmap.end() ) {
      logger(logINFO) 
          << left("[*]", 14) 
          << ">> no route found, invalid points";
      return optimized_routes();
    }
    
    graph_solver<
        graph_t, 
        weight_t,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT >* gsolver =  
      gsolver_factory<
        graph_t, 
        weight_t,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT >::instance()
         .get_solver_for( _g, algorithm, (_vtxmap)[source], (_vtxmap)[target]);         
    
    try 
    {
      gsolver->solve(weight_function, edge_index_map, stopping_criteria);
      return gsolver->get_optimized_routes();        
    } catch (std::exception) 
    {
      logger(logINFO) 
          << left("[*]", 14) 
          << ">> no route found!";        
      return optimized_routes();
    }
  }

  template <typename WeightFunctionT, 
            typename StoppingCriteriaT = null_stopping_criteria<graph_t> >
  optimized_routes apply_ARCsolver(
      std::string       algorithm, 
      std::string       source, 
      std::string       target,
      WeightFunctionT   weight_function,
      StoppingCriteriaT stopping_criteria = null_stopping_criteria<graph_t>()) 
  {          
    // edge index map
    typedef typename boost::property_map<
      graph_t, edge_index_t edge_properties_t::*>::type IndexMap; 

    IndexMap edge_index_map = boost::get(&edge_properties_t::edge_index, _g);  

    if( (_vtxmap).find(source) == _vtxmap.end() || 
        (_vtxmap).find(target) == _vtxmap.end() ) 
    {
      logger(logINFO) 
          << left("[*]", 14) 
          << ">> no route found, invalid points";
      return optimized_routes();
    }

    graph_solver<
        graph_t, 
        weight_t,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT >* gsolver =  
      gsolver_factory<
        graph_t, 
        weight_t,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT >::instance()
         .get_solver_for( _g, algorithm, _edgmap, (_vtxmap)[source], (_vtxmap)[target]);         
    
    try {
      gsolver->solve(weight_function, edge_index_map, stopping_criteria);
      return gsolver->get_optimized_routes();        
    } catch (std::exception) 
    {
      logger(logINFO) 
          << left("[*]", 14) 
          << ">> no route found!";      
      return optimized_routes();
    }
  }  

  template <typename WeightFunctionT, 
            typename StoppingCriteriaT = null_stopping_criteria<graph_t> >
  optimized_routes apply_solver(
      std::string              algorithm, 
      std::string              source, 
      std::vector<std::string> targets, 
      WeightFunctionT          weight_function,
      StoppingCriteriaT        stopping_criteria = null_stopping_criteria<graph_t>())  
  {
    // edge index map
    typedef typename boost::property_map<
      graph_t, edge_index_t edge_properties_t::*>::type IndexMap; 

    IndexMap edge_index_map = boost::get(&edge_properties_t::edge_index, _g); 

    std::vector<vertex_descriptor> tvec;
    for (std::string t : targets)
      tvec.push_back((_vtxmap)[t]);
    
    graph_solver<
        graph_t, 
        weight_t,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT >* gsolver =  
      gsolver_factory<
        graph_t, 
        weight_t,
        IndexMap, 
        WeightFunctionT, 
        StoppingCriteriaT >::instance()
         .get_solver_for( _g, algorithm, (_vtxmap)[source], tvec);    
    
    try {
      gsolver->solve(weight_function, edge_index_map, stopping_criteria);
      return gsolver->get_optimized_routes();        
    } catch (std::exception) 
    {
      logger(logINFO) 
          << left("[*]", 14) 
          << ">> no route found!";      
      return optimized_routes();
    }

  }

  optimized_routes route_optimize(
      std::string algorithm,  
      std::string source, 
      std::string target,
      std::string strategy = "shortest_weight_function") 
  {
    generic_weight_functor<graph_t, vertex_map, weight_t>* functor =
      weight_function_factory<graph_t, vertex_map, weight_t>::get_functor_for(
        strategy, _g, _vtxmap); 
    
    if (algorithm == "dijkstra") 
    {
      target_dijkstra_stopping_criteria<graph_t> stopping_criteria = 
        target_dijkstra_stopping_criteria<graph_t>((_vtxmap)[target]);
        
      return apply_solver(
          algorithm,   
          source, 
          target,
          functor,
          stopping_criteria);          
    }
    else if (algorithm == "compact_dijkstra") 
    {
      target_dijkstra_stopping_criteria<graph_t> stopping_criteria = 
        target_dijkstra_stopping_criteria<graph_t>((_vtxmap)[target]);
              
      return apply_ARCsolver(
          algorithm,   
          source, 
          target,
          functor,
          stopping_criteria);                              
    }  
    else if (algorithm == "bicriterion_epsMOA_star") 
    {
      return apply_solver(
          algorithm,   
          source, 
          target,
          functor);                             
    }  
    else
     return optimized_routes();

  }

  // WARNING: use if stop identifiers aren't in user request  
  void search_near_stops(
    std::string source,
    time_Rt request_time,
    route_map& jmap, 
    std::vector<
       std::pair<std::string, time_Rt> >& near_stops,  
    timetable_Rt& timetable, 
    double radius)
  {
    /*typedef std::list<edge_descriptor> path_t;    

    stopwatch chrono;
    std::vector<vertex_descriptor> pred_map(boost::num_vertices(_g));
    std::vector<double> distance_map(boost::num_vertices(_g));
    generic_weight_property_map<graph_t, weight_t> weight_map(_g);

    near_stops_dijkstra_visitor<graph_t, double> 
       nsdvisitor(request_time, near_stops, timetable, distance_map, radius);
    try 
    {
      logger(logINFO) 
        << left("[solver] ", 14) 
        << "Near Stop Dijkstra algorithm "
        << "[ s = " << source 
        << ", time = " << to_string(request_time) 
        << ", radius = " << radius << " ]";

      boost::dijkstra_shortest_paths (_g, (_vtxmap)[source],
        boost::predecessor_map(&pred_map[0]).
          distance_map(&distance_map[0]).
            weight_map(weight_map).
              visitor(nsdvisitor));
      throw target_not_found();
    } catch (target_found& tf) {
      // target found
      chrono.lap();
    } catch (std::exception& e) {
        throw solver_exception(e.what());
    }
    
    gRoute route;
    for (unsigned int i = 0; i < near_stops.size(); ++i) 
    {
      vertex_descriptor t = (_vtxmap)[ near_stops[i].first ];
      path_t path;
      if (pred_map[t] == t) {
        //throw solver error();
      }
      for(vertex_descriptor v = t; pred_map[v] != v; v = pred_map[v]) {
        edge_descriptor e; bool found;
        boost::tie(e, found) = boost::edge(pred_map[v], v, _g);
        if (found) {
          path.push_front(e);      
        } else {
          //throw solver error();
        }
      }
      route.push_back(std::make_pair(distance_map[t], path));
    }
    
    make_journey_map(jmap, route);

#ifdef DEBUG
    for(auto s : near_stops) 
    { 
      logger(logDEBUG) 
        << left("[engine]", 14) 
        << left(">", 3)
        << left("Find near stop - [ id = ", 10) 
        << left(s.first, 15) 
        << " at "
        << to_string(s.second) 
        <<" ]"; 
    } 
#endif   */  

  }

    
}; 


} // namespace gol

#endif // GOL_GENERIC_EDGE_WEIGHTED_GRAPH_H_