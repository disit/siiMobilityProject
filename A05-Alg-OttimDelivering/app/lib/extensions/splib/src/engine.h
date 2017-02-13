#ifndef GOL_ENGINE_H_
#define GOL_ENGINE_H_

// boost
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
//#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/variant.hpp>

// road_network
#include "graph/generic_edge_weighted_graph.h"
// public transport
#include "round_based/raptor_timetable.h"
#include "round_based/round_based_model/raptor_timetable_builder.h"
#include "round_based/raptor_solver_factory.h"

#include "cache.h"
#include "route.h"

namespace gol {

/// Graph
struct extra_vertex_properties 
{
  bool turn_restriction;

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & 
        BOOST_SERIALIZATION_NVP(turn_restriction);      
    }  
};
struct extra_edge_properties 
{
  std::string edge_type;
  std::string desc;

  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) 
    {
      ar & 
        BOOST_SERIALIZATION_NVP(edge_type) &
        BOOST_SERIALIZATION_NVP(desc);
    }   
};

class engine {
 public:
  
  static void
  dijkstra(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy, 
    std::string data_graph_path, 
    std::string data_timetable_path,
    optimized_routes_solution* sol); 

  static void
  dijkstra_raptor(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy, 
    std::string data_graph_path, 
    std::string data_timetable_path,
    double      search_radius_src,
    double      search_radius_trg,
    optimized_routes_solution* sol); 

  static void
  bicriteria_emoa_star(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy,     
    std::string data_graph_path, 
    std::string data_timetable_path,
    optimized_routes_solution* sol);     

   
private:
  engine();
  ~engine();  

};

} // namespace gol

#endif // GOL_ENGINE_H_
