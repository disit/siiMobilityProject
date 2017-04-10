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

// rice gem
#include <rice/Class.hpp>
#include <rice/Constructor.hpp>
#include <rice/String.hpp>
#include <rice/Array.hpp>
#include <rice/Hash.hpp>
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

  double get_average_speed(
      std::string mode,
      std::string highway);

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

 private:
  engine_t _SPengine;

};


} // namespace gol


#endif // GOL_ROUTE_PLANNER_H_
