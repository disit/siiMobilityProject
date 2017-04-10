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

#include "engine.h"

// global statc pointer used to ensure a single instance of the class
gol::engine_cache_t* gol::engine_cache_t::_cache_instance_ptr = NULL; 

namespace gol {

void
engine_t::dijkstra_based(
    std::string algorithm,
    std::string source,
    std::string target,
    std::string request_time,
    std::string model,
    std::string strategy,
    std::string data_graph_path,
    std::string data_timetable_path,
    optimized_routes_solution* sol)
{
  try
  {
    // if cache contains serialized files than
    // retrieve models else parse and build structures from data
    if (model.find("pedestrian_") != std::string::npos)
    {
      pedestrian_graphT& g = 
        _cache->get_cached_pedestrian_network_for(model);

      optimized_routes opt;
      opt = g.route_optimize(algorithm, source, target, strategy);

      for (auto route : opt)
        sol->insert_route(route);        
    }

    if (model.find("road_") != std::string::npos)
    {
      road_graphT& g = 
        _cache->get_cached_road_network_for(model);

      optimized_routes opt;
      opt = g.route_optimize(algorithm, source, target, strategy);

      for (auto route : opt)
        sol->insert_route(route);      
    }      

  }
  catch (target_not_found& t) {
    logger(logINFO)
      << left("[-]", 14)
      << "Target not found ";
    return;
  }
  catch (std::exception& e) {
    throw solver_exception( 
      std::string("dijkstra_based(): ") + e.what() );
  }

}

void
engine_t::bicriterion_epsMOA_star(
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
    opt = g.route_optimize(
      "bicriterion_epsMOA_star", source, target, strategy);

    for (auto route : opt)
      sol->insert_route(route);

  } catch (std::exception& e) {
    throw solver_exception(
      std::string("bicriterion_epsMOA_star(): ") + e.what() );
  }

}

/*void
engine_t::dijkstra_raptor(
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

  route_map srcRoad_prt,
            Roadtrg_prt;

  try
  {
    // if cache contains serialized files than
    // retrieve else parse and build structures from data
    g.create_model("footway_simplified_model", data_graph_path);
    raptor_timetable_builder* rbuilder =
        new raptor_timetable_builder(&timetable);
    cache::retrieve_public_transport(&timetable, rbuilder, data_timetable_path, "raptor_timetable_model");

    time_Rt time = to_rtime(request_time, get_today());

    // neighbourhood seacrh from source and target
    if (search_radius_src > MAX_SEARCH_RADIUS)
        search_radius_src = MAX_SEARCH_RADIUS;
    g.search_near_stops(
      source, time, srcRoad_prt, stops_src, timetable, search_radius_src);

    if (search_radius_trg > MAX_SEARCH_RADIUS)
        search_radius_trg = MAX_SEARCH_RADIUS;
    g.search_near_stops(
      target, 0, Roadtrg_prt, stops_trg, timetable, search_radius_trg);

    //WARNING: if stop ids are in user request
    //std::vector<std::string> targets = {targets};
    //srcRoad_prt = g.apply_solver("multi_target_dijkstra", source, targets);


    // apply round-based public transit routing algorithm from stops
    raptor_solver* solver =
      raptor_solver_factory::get_solver_for(
          timetable, "basic_raptor", stops_src, stops_trg, request_time);

    solver->solve();
    optimized_routes opt = solver-> get_optimized_routes();

    for (auto route : opt)
    {
     sol->insert_route(
        srcRoad_prt[route.get_begin_id()]);
     sol->insert_route(route);
     sol->insert_route(
        Roadtrg_prt[route.get_end_id()]);
    }

   } catch (std::exception& e) {
       throw solver_exception("dijkstra_raptor(): ");
   }

}*/


} // namespace gol
