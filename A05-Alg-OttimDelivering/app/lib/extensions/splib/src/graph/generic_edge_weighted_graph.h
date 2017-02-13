#ifndef GOL_GENERIC_EDGE_WEIGHTED_GRAPH_H_
#define GOL_GENERIC_EDGE_WEIGHTED_GRAPH_H_

// std
#include <iostream>
#include <utility>
#include <stdint.h>
#include <tuple>
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
#include <boost/config.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/property_map/function_property_map.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "../config.h"
#include "../utility.h"
#include "../exception.h"
#include "../utils/stopwatch.h"
#include "../utils/logger.h"
#include "../utils/bitset.h"

#include "../cache.h"

#include "graph_functionPt_property_map.h"
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
      return os << "node = [ lon : " 
                << p.lon << ", lat : " 
                << p.lon << ", ele : " 
                << p.ele << " ]" 
                << std::endl;
    }; 
  };  

  struct vertex_properties_t 
  {
    std::string id; // move to vertex_map
    point_t     geo;

    ExtraVertexProperties properties;
 
    friend class boost::serialization::access;  
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) 
    {
      ar &
        BOOST_SERIALIZATION_NVP(id)   &
        BOOST_SERIALIZATION_NVP(geo)  & 
        BOOST_SERIALIZATION_NVP(properties);
    }
  };

  struct edge_properties_t 
  {
    weight_t weight;

    ExtraEdgeProperties properties;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & 
        BOOST_SERIALIZATION_NVP(weight)     &
        BOOST_SERIALIZATION_NVP(properties);
    }
  };

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
      boost_graph_t,             // directed, bidirectional
      vertex_properties_t, 
      edge_properties_t,
      graph_properties_t
  > graph_t;

  typedef boost::graph_traits<graph_t>       Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;

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

  typedef std::map< 
    vertex_descriptor,
    std::vector<
      std::pair<
        std::set<vertex_descriptor>, // from way            
        std::set<vertex_descriptor>  // to   way           
      >
    >  
  > turn_restriction_map;  

  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor> path_t;
  typedef std::list<std::pair<weight_t, path_t> > gRoute;

  graph_t              _g;
  vertex_map           _vtxmap;
  turn_restriction_map _resmap;
  size_t               _n_weights;
  std::string          _model;

public:
  generic_edge_weighted_graph_t(): 
      _g(), 
      _vtxmap(),
      _resmap(), 
      _n_weights(sizeof...(WeightsT)),
      _model() {};

  ~generic_edge_weighted_graph_t() {} // TODO   
  
  //turn_restriction_map& get_turn_restriction() { return _resmap }

  void create_model(std::string model, std::string filename) 
  {
    graph_builder<
        graph_t, 
        vertex_map, 
        turn_restriction_map, 
        weight_t>* gbuilder = 
      graph_builder_factory<
        graph_t, vertex_map, turn_restriction_map, weight_t>::get_builder_for(
          model, _g, _vtxmap, _resmap);
    
    _model = model;
    cache::retrieve_road_network(
      &_g, &_vtxmap, &_resmap, gbuilder, filename, model);

    //dump_turn_restriction();    
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

    graph_solver<
        graph_t, 
        weight_t, 
        WeightFunctionT, 
        StoppingCriteriaT >* gsolver =  
      gsolver_factory<
        graph_t, 
        weight_t, 
        WeightFunctionT, 
        StoppingCriteriaT >::instance()
         .get_solver_for( _g, algorithm, (_vtxmap)[source], (_vtxmap)[target]);    
    
    gsolver->solve(weight_function, stopping_criteria);  
    return gsolver->get_optimized_routes();
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
    std::vector<vertex_descriptor> tvec;
    for (std::string t : targets)
      tvec.push_back((_vtxmap)[t]);
    
    graph_solver<graph_t, weight_t, WeightFunctionT, StoppingCriteriaT >* gsolver =  
      gsolver_factory<graph_t, weight_t, WeightFunctionT, StoppingCriteriaT >::instance().
         get_solver_for( _g, algorithm, (_vtxmap)[source], tvec);    
    
    gsolver->solve(weight_function, stopping_criteria);
    return gsolver->get_optimized_routes();  
  }

  optimized_routes route_optimize(
      std::string algorithm,  
      std::string source, 
      std::string target,
      std::string strategy = "identity_weight_function") 
  {
    generic_weight_functor<graph_t, weight_t>* functor =
      weight_function_factory<graph_t, weight_t>::get_functor_for(strategy, _g);
    boost::functionPt_property_map<
        generic_weight_functor<graph_t, weight_t>, 
        edge_descriptor, 
        weight_t> 
      weight_function(functor);    
    
    if (algorithm == "dijkstra") 
    {
      target_dijkstra_stopping_criteria<graph_t> stopping_criteria = 
        target_dijkstra_stopping_criteria<graph_t>((_vtxmap)[target]);
        
      return apply_solver(
          algorithm,   
          source, 
          target,
          weight_function,
          stopping_criteria);          
    }
    else if (algorithm == "bicriteria_emoa_star") 
    {
      return apply_solver(
          algorithm,   
          source, 
          target,
          weight_function);                             
    }  
    else
     return optimized_routes();

  }

  void dump_turn_restriction()
  {
    for (auto KV : _resmap) 
    {
      std::cout << "Via Node ID: "
                << _g[KV.first].id
                << std::endl
                << std::endl;
      for (auto FromTo : KV.second) {
        std::cout << "Turn Rule From (<<) - To (>>)" 
                  << std::endl; 
        for (auto v : FromTo.first)
          std::cout << "<< id = "
                    << _g[v].id
                    << std::endl;
        for (auto v : FromTo.second)
          std::cout << ">> id = "
                    << _g[v].id
                    << std::endl;
        std::cout << std::endl;
      } 
      std::cout << std::endl;                           
    }

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

  }


    
};  

} // namespace gol

#endif // GOL_GENERIC_EDGE_WEIGHTED_GRAPH_H_