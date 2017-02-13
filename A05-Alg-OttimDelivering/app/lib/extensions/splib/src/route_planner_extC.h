#ifndef GOL_ROUTE_PLANNER_H_
#define GOL_ROUTE_PLANNER_H_

// rice gem
#include <rice/Class.hpp>
#include <rice/Constructor.hpp>
#include <rice/String.hpp>
#include <rice/Array.hpp>
#include <rice/Hash.hpp>

#include "common.h"
#include "engine.h"

namespace gol {

/**
* Route Planner
*/
class route_planner {
 public:
  route_planner() {};
  ~route_planner() {};

  Rice::Array to_rice(
      optimized_routes_solution* sol, 
      time_Rt                    time, 
      std::string                optimization);

  Rice::Array
  route_optimization(
      std::string optimization,
      std::string source,
      std::string target,
      std::string request_time,
      std::string data_graph_path,
      std::string data_timetable_path); 

};


} // namespace gol


#endif // GOL_ROUTE_PLANNER_H_
