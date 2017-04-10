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

#ifndef GOL_ROUTE_PLANNER_H_
#define GOL_ROUTE_PLANNER_H_

//boost
#include <boost/filesystem.hpp>

#include "common.h"
#include "data_extraction/sqlite/sqlite_database_helper.h"
#include "engine.h"

namespace gol {

/**
* Route Planner
*/ 
class route_planner {
    typedef boost::filesystem::path sys_path;
 public:
  route_planner(bool updateDB) : 
    _SPengine(DEFAULT_PBF_OSMFILE, updateDB) {}
  ~route_planner() {}  

  void 
  route_optimization(
        std::string optimization,
        std::string source, 
        std::string target,  
        std::string request_time,
        std::string data_graph_path,
        std::string data_timetable_path,
        std::string filename = std::string())
  {
    
#ifndef NLOG
    log_policy::get_instance().umtx();
#endif  

    optimized_routes_solution* sol =
      new optimized_routes_solution(); 

    if (optimization.find("foot_optimization") != std::string::npos)
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Foot Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("pedestrian_simplified_model");
      
      std::string strategy("shortest_weight_function");
      if (optimization.find("shortest_") != std::string::npos)
        strategy = "shortest_weight_function";
      if (optimization.find("quietest_") != std::string::npos)
        strategy = "quietest_pedestrian_weight_function";

      logger(logINFO) 
        << left("[*]", 14) 
        << "Weight Function: " 
        << strategy;

      _SPengine.dijkstra_based(
        "dijkstra",
        source, 
        target, 
        request_time,
        model,
        strategy, 
        data_graph_path, 
        data_timetable_path, 
        sol);
    }
    else if (optimization.find("car_optimization") != std::string::npos)
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Car Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("road_compact_representation_model");
      
      std::string strategy("shortest_weight_function");
      if (optimization.find("fastest_") != std::string::npos)
        strategy = "fastest_road_weight_function"; 

      logger(logINFO) 
        << left("[*]", 14) 
        << "Weight Function: " 
        << strategy;

      _SPengine.dijkstra_based(
        "compact_dijkstra",
        source, 
        target, 
        request_time,
        model,
        strategy, 
        data_graph_path, 
        data_timetable_path, 
        sol);
    }    
    else if (optimization == "bike_optimization") 
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Bicycle Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("bicriterion_bicycle_model");

      std::string strategy("shortest_weight_function");
      if (optimization.find("safest_fastest_") != std::string::npos)
        strategy = "safest_fastest_bicycle_weight_function";

      engine_t::bicriterion_epsMOA_star(
        source, 
        target, 
        request_time,
        model,
        strategy,          
        data_graph_path, 
        data_timetable_path, 
        sol);
    }
/*    else if (optimization == "public_transit_optimization") 
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Public Transit Optimization >>"
        << source << ", t = " << target <<"]";

      std::string model("");
      std::string strategy("");

      engine::dijkstra_raptor(
        source, 
        target, 
        request_time,
        model,
        strategy,          
        data_graph_path, 
        data_timetable_path,
        500, // source radius 
        500, // target radius
        sol);
    }   
*/
    sol->dump_geojson(filename);
    delete sol; 

  }

  void
  update_OSMdb(bool updateDB = true) {
    _SPengine.cache_refresh(updateDB); 
  }  

 private:
  engine_t _SPengine;

};


} // namespace gol

#endif // GOL_ROUTE_PLANNER_H_	