
#include "engine.h" 

namespace gol {

void
engine::dijkstra(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy,     
    std::string data_graph_path, 
    std::string data_timetable_path,
    optimized_routes_solution* sol) 
{
  auto g = generic_edge_weighted_graph_t <  // road network
               extra_vertex_properties, 
               extra_edge_properties, 
               double >();
  try
  {
    // if cache contains serialized files than
    // retrieve else parse and build structures from data
    g.create_model(model, data_graph_path);

    optimized_routes opt; 
    opt = g.route_optimize("dijkstra", source, target, strategy);

    for (auto route : opt)
      sol->insert_route(route);

  }
  catch (target_not_found& t) {
    logger(logERROR)
      << left("[-]", 14)
      << "Target not found, "
      << "perhaps the conversion from Lon&Lat to NodeIds has failed";
    return;
  }
  catch (std::exception& e) {
    throw solver_exception( std::string("dijkstra(): ") + e.what() );
  }

}

void
engine::bicriteria_emoa_star(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy,   
    std::string data_graph_path, 
    std::string data_timetable_path,
    optimized_routes_solution* sol)
{
  auto g = generic_edge_weighted_graph_t <  // road network
               extra_vertex_properties, 
               extra_edge_properties, 
               int, int >();                // buckets pruning
  try
  {
    // if cache contains serialized files than
    // retrieve else parse and build structures from data
    g.create_model(model, data_graph_path);

    optimized_routes opt; 
    opt = g.route_optimize("bicriteria_emoa_star", source, target, strategy);

    for (auto route : opt)
      sol->insert_route(route);   

  } catch (std::exception& e) {
    throw solver_exception("bicriteria_emoa_star(): ");
  }

} 

void
engine::dijkstra_raptor(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy,     
    std::string data_graph_path, 
    std::string data_timetable_path,
    double      search_radius_src,
    double      search_radius_trg,
    optimized_routes_solution* sol) 
{

  timetable_Rt timetable;                   // public transport network
  auto g = generic_edge_weighted_graph_t <  // road network
               extra_vertex_properties, 
               extra_edge_properties, 
               double >();

  // this vector contains neighbourhood search    
  // result from a road origin to find near stops  
  // and inizialize public transport algorithm 
  // first element  : stop identifier
  // second element : time to reach the stop from origin            
  std::vector<
      std::pair<std::string, time_Rt> 
      > stops_src{},
        stops_trg{};
              

  try 
  {  
   // TODO
         
  } catch (std::exception& e) {
       throw solver_exception("dijkstra_raptor(): ");
  } 

}


} // namespace gol
