#ifndef GOL_RAPTOR_ARRAY_TIMETABLE_H_
#define GOL_RAPTOR_ARRAY_TIMETABLE_H_

#include "../common.h"

namespace gol { 

// raptor time 
//typedef int time_Rt; // move to utility.h

struct stop_Rt 
{
  uint32_t stop_routes_offset;
  uint32_t transfers_offset;
  double   lon;
  double   lat;

  std::string id;
  bool operator<(const stop_Rt& s) const {
    return (id.compare(s.id) < 0);
  }

  friend class boost::serialization::access;
  // when the class Archive corresponds to an output archive, the
  // & operator is defined similar to << likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>.    
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) 
  {
    ar &
      BOOST_SERIALIZATION_NVP(id)  & 
      BOOST_SERIALIZATION_NVP(lon) & 
      BOOST_SERIALIZATION_NVP(lat) &
      BOOST_SERIALIZATION_NVP(transfers_offset) &      
      BOOST_SERIALIZATION_NVP(stop_routes_offset);
  }

  friend std::ostream& operator<<(std::ostream &os, const stop_Rt& s) {
    return os << "stop = [ lon, lat ] = [ " 
              << s.lon << ", " 
              << s.lat << " ]" 
              << std::endl;
  }; 
};

struct stime_Rt // stoptime 
{
  time_Rt departure_time; 
  time_Rt arrival_time;

  friend class boost::serialization::access;   
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) 
  {
    ar &
      BOOST_SERIALIZATION_NVP(departure_time) &    
      BOOST_SERIALIZATION_NVP(arrival_time);
  }

  friend std::ostream& operator<<(std::ostream &os, const stime_Rt& s) {
    return os << "stop_time = [ dep, arr ] = [ " 
              << s.departure_time 
              << ", " << s.arrival_time 
              << " ]" << std::endl;
  };   
};

struct trip_Rt 
{
  uint32_t stop_times_offset;  // The offset of the first stoptime of the time demand type used by this trip
  time_Rt  begin_time;         // The absolute start time since at the departure of the first stop
  int16_t  realtime_delay;     // This is signed to indicate early or late. All zeros upon creation (but serves as padding).

  std::string id;
  bool operator<(const trip_Rt& t) const {
    return (id.compare(t.id) < 0); 
  }

  friend class boost::serialization::access;   
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) 
  {
    ar &
      BOOST_SERIALIZATION_NVP(id) &
      BOOST_SERIALIZATION_NVP(stop_times_offset) &
      BOOST_SERIALIZATION_NVP(begin_time) &    
      BOOST_SERIALIZATION_NVP(realtime_delay);
  }  
};

struct transfer_Rt 
{  
  uint32_t sidx_to;
  uint32_t length; // integer part 

  friend class boost::serialization::access;   
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) 
  {
    ar &
      BOOST_SERIALIZATION_NVP(sidx_to) &    
      BOOST_SERIALIZATION_NVP(length);
  }
};
 
struct route_Rt 
{
  uint16_t n_stops;
  uint16_t n_trips;
  uint32_t route_stops_offset;
  uint32_t trips_offset;

  std::string id;
  bool operator<(const route_Rt& r) const {
    return (id.compare(r.id) < 0); 
  }

  friend class boost::serialization::access;   
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) 
  {
    ar &
      BOOST_SERIALIZATION_NVP(id)      &
      BOOST_SERIALIZATION_NVP(n_stops) &
      BOOST_SERIALIZATION_NVP(n_trips) & 
      BOOST_SERIALIZATION_NVP(route_stops_offset) &           
      BOOST_SERIALIZATION_NVP(trips_offset);
  }
};

struct timetable_Rt 
{
  uint32_t n_stops;
  uint32_t n_routes;
  uint32_t n_trips;
  
  std::vector<route_Rt>    routes;
  std::vector<trip_Rt>     trips;
  std::vector<stop_Rt>     stops;
  std::vector<uint32_t>   route_stops;
  std::vector<uint32_t>   stop_routes;
  std::vector<stime_Rt>    stop_times; 
  std::vector<transfer_Rt> transfers;

  std::map<std::string, uint32_t> stopidx_map;

  friend class boost::serialization::access;   
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) 
  {
    ar &
      BOOST_SERIALIZATION_NVP(n_stops)     &
      BOOST_SERIALIZATION_NVP(n_routes)    & 
      BOOST_SERIALIZATION_NVP(n_trips)     & 
      BOOST_SERIALIZATION_NVP(stops)       &
      BOOST_SERIALIZATION_NVP(trips)       & 
      BOOST_SERIALIZATION_NVP(routes)      & 
      BOOST_SERIALIZATION_NVP(route_stops) &
      BOOST_SERIALIZATION_NVP(stop_times)  & 
      BOOST_SERIALIZATION_NVP(stop_routes) &           
      BOOST_SERIALIZATION_NVP(transfers)   &
      BOOST_SERIALIZATION_NVP(stopidx_map);
  }

  void dump_routes() 
  {
    logger(logDEBUG) 
      << "Timetable routes: ";
    int ridx = 0;
    for (auto r : routes){
      logger(logDEBUG) 
        << "> " 
        << left("["+ std::to_string(ridx) + "] ", 10) 
        << r.id;
      ++ridx;
    }
  } 

  void dump_route_trips(uint32_t ridx)
  {
    logger(logDEBUG) 
      << "Timetable trips [route - " << routes[ridx].id << "]:";
    for (uint32_t tidx = routes[ridx].trips_offset;
          tidx < (ridx == (n_routes - 1) ? 
            trips.size() : 
            routes[ridx + 1].trips_offset );
          ++tidx)  
    {
      trip_Rt  trip   = trips[tidx];
      route_Rt route  = routes[ridx];
      logger(logDEBUG) 
        << "trip [" 
        << tidx - routes[ridx].trips_offset << "]";
      for (uint32_t idx = 0; idx < route.n_stops; ++idx)
      {        
        stop_Rt stop = 
           stops[route_stops[route.route_stops_offset + idx]];
        time_Rt departure_time = 
           stop_times[trip.stop_times_offset + idx].departure_time;
        time_Rt arrivale_time = 
           stop_times[trip.stop_times_offset + idx].arrival_time;
        logger(logDEBUG) 
          << "> stop id = "
          << left(stop.id, 12) 
          << " [" 
          << to_string(departure_time) 
          << ", " 
          << to_string(arrivale_time) 
          << "] ";      
      }        
    }        
  }
    
}; 

struct label_Rt 
{
  time_Rt  time;         // The time when this stop was reached    
  uint32_t back_stop;    // The index of the previous stop in the itinerary
  uint32_t back_route;   // The index of the route used to travel from back_stop to here, or WALK
  uint32_t back_trip;    // The index of the trip used to travel from back_stop to here, or WALK
  time_Rt  board_time;   // The time at which the trip within back_route left back_stop
  time_Rt  walk_time;    // The time when this stop was reached by walking (2nd phase)
  uint32_t walk_from;    // The stop from which this stop was reached by walking (2nd phase)
 
  time_Rt   from_location_time; // The walk time from road-network source  
  time_Rt   to_location_time;   // The walk time to road-network target 
  
  friend std::ostream& operator<<(std::ostream &os, const label_Rt& s) {
    return os << "         --- stop state ---          " << std::endl 
      << "time               = " << s.time               << std::endl          
      << "back_stop          = " << s.back_stop          << std::endl
      << "back_route         = " << s.back_route         << std::endl 
      << "back_trip          = " << s.back_trip          << std::endl    
      << "board_time         = " << s.board_time         << std::endl    
      << "walk_time          = " << s.walk_time          << std::endl 
      << "walk_from          = " << s.walk_from          << std::endl
      << "from_location_time = " << s.from_location_time << std::endl
      << "to_location_time   = " << s.to_location_time   << std::endl;
  };   
};

struct data_Rt 
{
  uint8_t   n_rounds;
  time_Rt*  minimun_arrival_times;            // the best arrival times
  label_Rt* round_earliest_arrival_times;     // all raptor states
  bitset_t* marked_stops;                     // used to track which routes might have changed during each round
   
  std::vector<uint32_t>        sources;       // stops were reached from road-origin
  std::vector<uint32_t>        targets;       // stops were reached from road-destination          
  std::map<uint32_t,/*ridx*/ 
           uint32_t /*rsidx*/> Q;             // only routes from Q are considered for scanning in round N
  std::map<uint32_t,/*ridx*/ 
       std::vector<uint32_t> > route_sources; // for each route give stops (rsidx) that were reached from road-origin   
};

// a type where we will hold shortest path as lists of connection
// a connection represents one ride or walking transfer. 
struct connection_Rt 
{
  uint32_t sidx_from;    
  uint32_t sidx_to;   
  time_Rt  departure_time;    
  time_Rt  arrival_time;  
  uint32_t ridx;
  uint32_t tidx;
  //time_Rt  delay; // signed realtime delay, in seconds
};

// a path is a chain of connections leading from one place to another. 
struct path_Rt 
{
  uint32_t n_rides;
  uint32_t n_connctions;  
  std::list<connection_Rt> connections;
  //struct connection_Rt connections[RAPTOR_MAX_ROUNDS * 2 + 1];
};

// several pareto-optimal paths connecting the same two stops. 
struct round_based_solver_result 
{
  uint32_t n_paths;
  std::list<path_Rt> paths;
  //struct path_Rt paths[RAPTOR_MAX_ROUNDS];
};


} // namespace gol

#endif // GOL_RAPTOR_ARRAY_TIMETABLE_H_