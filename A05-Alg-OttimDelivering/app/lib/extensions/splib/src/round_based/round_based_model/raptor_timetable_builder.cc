
#include "raptor_timetable_builder.h"

namespace gol {

void raptor_timetable_builder::add_route(std::string rid) {}

void raptor_timetable_builder::add_trip(std::string tid) {}

void raptor_timetable_builder::add_stop(
     std::string sid, 
     double lon, 
     double lat, 
     std::string rid) 
{

  stop_Rt s;
  s.id = sid;
  //s.stop_routes_offset 
  //s.transfers_offset 
  s.lon = lon;
  s.lat = lat;

  route_Rt r;
  r.id = rid;

  // routes for stop
  auto srit = _sr_map.find(s);
  if (srit != _sr_map.end()) 
    (*srit).second.push_back(r);
  else 
    _sr_map.insert(std::make_pair(s, std::list<route_Rt>{r}));
   
  // stops for route
  auto rsit = _rs_map.find(r);
  if (rsit != _rs_map.end())
    (*rsit).second.push_back(s);
  else 
    _rs_map.insert(std::make_pair(r, std::list<stop_Rt>{s}));

}

void raptor_timetable_builder::add_stop_time(
     std::string rid, 
     std::string tid, 
     std::string sid, 
     time_Rt departure_time, 
     time_Rt arrival_time) 
{
  stime_Rt st;
  st.departure_time = departure_time;
  st.arrival_time = arrival_time;

  auto rtsit = _rtstime_map.find(rid);
  if(rtsit != _rtstime_map.end()) 
  {
    auto tsit = (*rtsit).second.find(tid);
    if (tsit != (*rtsit).second.end())
      (*tsit).second.push_back(st);
    else
      _rtstime_map[rid].insert(std::make_pair(tid, std::list<stime_Rt>{st}));
  }
  else
  {  
    _rtstime_map.insert(std::make_pair(rid, std::map<std::string, std::list<stime_Rt> >
        {{tid, std::list<stime_Rt>{st}}})); 
  }  

}
 
void raptor_timetable_builder::add_transfer(
     std::string sid_from, 
     std::string sid_to, 
     double dist) 
{
  // TODO: can we use sparql query?
  
  // retrieve road network
  /*auto g = generic_edge_weighted_graph_t<
            extra_vertex_properties, 
            extra_edge_properties, 
            double>();
  g.create_model("footway_simplified_model", data_graph_dir);*/    
}

// WARNING optimatizion code?
//void raptor_timetable_builder::construct_array_timetable() 
void raptor_timetable_builder::rtimetable() 
{
  uint16_t n_stops, n_trips; 
  uint32_t stop_times_offset = 0, trips_offset = 0, route_stops_offset = 0, stop_routes_offset = 0;
  uint32_t tt_stops = 0, tt_routes = 0, tt_trips = 0;
  // routes
  for (auto it = _rtstime_map.begin(); it != _rtstime_map.end(); ++it) 
  {
    std::string rid = (*it).first;
    ++tt_routes;

    route_Rt r;
    r.id = rid;
    r.trips_offset = trips_offset;
    
    // trips
    n_trips = 0;
    for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2)
    {
      std::string tid = (*it2).first;
      ++tt_trips;
          
      trip_Rt t;
      t.id = tid;
      t.stop_times_offset = stop_times_offset;
      //t.begin_time
      t.realtime_delay = 0;

      // stop times
      n_stops = 0;
      for (stime_Rt st : (*it2).second)
      {
        _tt->stop_times.push_back(st);
        ++stop_times_offset;
        ++n_stops;  
      }
      _tt->trips.push_back(t);
      ++trips_offset;
      ++n_trips; 
    }
    r.n_stops = n_stops;
    r.n_trips = n_trips;

    r.route_stops_offset = route_stops_offset;
    for (stop_Rt s : _rs_map[r])
    {
      auto sit = _sr_map.find(s);
      if (sit == _sr_map.end()) 
        std::cout<<" error: stop idx don't find! "<<std::endl; // TODO Exception
      
      _tt->route_stops.push_back( std::distance(_sr_map.begin(), sit) );        
      ++route_stops_offset;  
    }

    _tt->routes.push_back(r);

  } // routes

  for (auto it = _sr_map.begin(); it != _sr_map.end(); ++it) 
  {
    stop_Rt s = (*it).first;
    s.stop_routes_offset = stop_routes_offset;
    s.transfers_offset = 0; // if all 0 algorithm work without walk transfers.
    _tt->stops.push_back(s);
    _tt->stopidx_map.insert(std::make_pair(s.id, tt_stops)); 
    for (route_Rt r : (*it).second)
    {
      auto rit = _rtstime_map.find(r.id);
      if (rit == _rtstime_map.end()) 
        std::cout<<" error: route idx don't find! "<<std::endl; // TODO Exception
        
      _tt->stop_routes.push_back( std::distance(_rtstime_map.begin(), rit) );  
      ++stop_routes_offset;
    }
    ++tt_stops;
  }

  _tt->n_routes = tt_routes;
  _tt->n_trips = tt_trips;
  _tt->n_stops = tt_stops;

  //add_transfers();

}


} // namespace gol