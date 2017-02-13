#ifndef GOL_ROUTE_PLANNER_H_
#define GOL_ROUTE_PLANNER_H_

#include "common.h"
#include "engine.h"

namespace gol {

/**
* Route Planner
*/ 
class route_planner {
 public:

  static void 
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

    if (optimization == "foot_optimization")
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Foot Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("footway_simplified_model");
      std::string strategy("priority_footway_weight_function");

      engine::dijkstra(
        source, 
        target, 
        request_time,
        model,
        strategy, 
        data_graph_path, 
        data_timetable_path, 
        sol);
    }
    else if (optimization == "car_optimization")
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Car Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("car_simplified_model");
      std::string strategy("identity_weight_function");

      engine::dijkstra(
        source, 
        target, 
        request_time,
        model,
        strategy, 
        data_graph_path, 
        data_timetable_path, 
        sol);
    }    
    else if (optimization == "bicycle_optimization") 
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Bicycle Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("bicriteria_cycleway_model");
      std::string strategy("safe_and_vertical_cycleway_weight_function");

      engine::bicriteria_emoa_star(
        source, 
        target, 
        request_time,
        model,
        strategy,          
        data_graph_path, 
        data_timetable_path, 
        sol);
    }
    else if (optimization == "public_transit_optimization") 
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

    sol->dump_geojson(filename); 

  }

 private:
  route_planner();
  ~route_planner();

};


} // namespace gol

#endif // GOL_ROUTE_PLANNER_H_	