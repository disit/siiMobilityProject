#include "route_planner_extC.h"

namespace gol {

  Rice::Array
  route_planner::to_rice(
      optimized_routes_solution* sol, 
      time_Rt                    time, 
      std::string                optimization)
  {    
    double      distance = 0;
    std::string transport = std::string();
    std::string transport_provider = std::string();
    std::string transport_service_type = std::string();;
    
    Rice::Array _routes = Rice::Array();
    for (auto r : sol->get_routes())
    {
       double speed_ms;
       if (optimization == "foot_optimization")
       {
         speed_ms  = (AVERAGE_WALKING_SPEED/(3.6));
         transport = "foot";
       }
       if (optimization == "bicycle_optimization")
       {
         speed_ms  = (AVERAGE_BICYCLE_SPEED/(3.6));
         transport = "bicycle";
       }
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
         
         time += (e.length/speed_ms);
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

    if (optimization == "foot_optimization")
    {
      logger(logINFO) 
        << left("[*]", 14) 
        << "Foot Optimization >> [s = " 
        << source << ", t = " << target <<"]";

      std::string model("footway_simplified_model");
      std::string strategy("priority_highway_optimization");

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
        << "Bicycle Optimization >>"
        << source << ", t = " << target <<"]";

      std::string model("bicriteria_cycleway_model");
      std::string strategy("bicycle_travel_time_optimization");

      engine::bicriteria_emoa_star(
        source, target, 
        request_time,
        model,
        strategy,          
        data_graph_path, 
        data_timetable_path, 
        sol);
    }
    /*else if (optimization == "public_transit_optimization") 
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
    }//*/

    Rice::Array ret = to_rice(sol, to_rtime(request_time, get_today()), optimization);
    delete sol;
    
    return ret;

  }

} // namespace gol

extern "C"
void Init_splib() {
    Rice::Class rb_cRoutePlanner =
      Rice::define_class<gol::route_planner>("RoutePlanner")
          .define_constructor(Rice::Constructor<gol::route_planner>())
          .define_method("route_optimization", &gol::route_planner::route_optimization);
}
