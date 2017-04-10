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

#include "route_planner_extC.h"

namespace gol {

  double
  route_planner::get_average_speed(
      std::string mode,
      std::string highway)
  {
    if (mode == "foot")
      return (AVERAGE_WALKING_SPEED/(3.6));
    if (mode == "bike")
      return (AVERAGE_BICYCLE_SPEED/(3.6));
    if (mode == "car")
    {
      if (highway == "motorway"      ||
          highway == "motorway_link" )
        return (MOTORWAY_AVERAGE_CAR_SPEED/(3.6));
      if (highway == "trunk"         ||
          highway == "trunk_link"    ||
          highway == "primary"       ||
          highway == "primary_link" )
        return (COUNTRY_AVERAGE_CAR_SPEED/(3.6));
      //else
      return (CITY_AVERAGE_CAR_SPEED/(3.6));
    }
  }

  Rice::Array
  route_planner::to_rice(
      optimized_routes_solution* sol,
      time_Rt                    time,
      std::string                optimization)
  {
    double      distance               = 0;
    std::string transport              = std::string();
    std::string transport_provider     = std::string();
    std::string transport_service_type = std::string();

    Rice::Array _routes = Rice::Array();
    for (auto r : sol->get_routes())
    {
       if (optimization.find("foot_optimization") != std::string::npos)
         transport = "foot";
       else if (optimization.find("bike_optimization") != std::string::npos)
         transport = "bike";
       else if (optimization.find("car_optimization")  != std::string::npos)
         transport = "car";
       else
        throw runtime_exception("to_rice(): Optimization unknown");

       Rice::Hash  _route = Rice::Hash();
       Rice::Array _arc   = Rice::Array();
       for (auto e : r.get_edges())
       {
         Rice::Hash _a = Rice::Hash();

         distance += e.length;
         _a[Rice::String("distance")]               = to_ruby(e.length/1000);
         _a[Rice::String("start_datetime")]         = Rice::String(to_string(time));

         time += ( e.length/(get_average_speed(transport, e.highway_value)) );

         _a[Rice::String("end_datetime")]           = Rice::String(to_string(time));
         _a[Rice::String("desc")]                   = Rice::String(e.desc);
         _a[Rice::String("transport")]              = Rice::String(transport);
         _a[Rice::String("transport_provider")]     = Rice::String(transport_provider);
         _a[Rice::String("transport_service_type")] = Rice::String(transport_service_type);

         Rice::Hash _snode = Rice::Hash();
         _snode[Rice::String("node_id")]            = Rice::String(e.nFrom.id);
         _snode[Rice::String("lon")]                = to_ruby(e.nFrom.lon);
         _snode[Rice::String("lat")]                = to_ruby(e.nFrom.lat);

         Rice::Hash _tnode = Rice::Hash();
         _tnode[Rice::String("node_id")]            = Rice::String(e.nTo.id);
         _tnode[Rice::String("lon")]                = to_ruby(e.nTo.lon);
         _tnode[Rice::String("lat")]                = to_ruby(e.nTo.lat);

         _a[Rice::String("source_node")]            = _snode;
         _a[Rice::String("destination_node")]       = _tnode;

         _arc.push(_a);
       }
       _route[Rice::String("arc")]                  = _arc;

       _route[Rice::String("eta")]                  = to_ruby(to_string(time));
       _route[Rice::String("distance")]             = to_ruby(distance/1000);
       _routes.push(_route);
    }
    return _routes;
  }

  Rice::Array
  route_planner::route_optimization(
      std::string optimization,
      std::string source,
      std::string target,
      std::string request_time,
      std::string data_graph_path,
      std::string data_timetable_path)
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
/*    else if (optimization.find("bike_optimization") != std::string::npos)
    {
      logger(logINFO)
        << left("[*]", 14)
        << "Safest Bicycle Optimization >>"
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
      engine::dijkstra_raptor(
        source,
        target,
        request_time,
        data_graph_path,
        data_timetable_path,
        500, // source radius
        500, // target radius
        sol);
    }*/

    Rice::Array ret =
      to_rice(sol, to_rtime(request_time, get_today()), optimization);
    delete sol;

    return ret;

  }


} // namespace gol

extern "C"
void Init_splib() {
    Rice::Class rb_cRoutePlanner =
      Rice::define_class<gol::route_planner>("RoutePlanner")
          .define_constructor(Rice::Constructor<gol::route_planner, bool>())
          //.define_constructor(Rice::Constructor<gol::route_planner, std::string>())
          .define_method("route_optimization", &gol::route_planner::route_optimization);
}
