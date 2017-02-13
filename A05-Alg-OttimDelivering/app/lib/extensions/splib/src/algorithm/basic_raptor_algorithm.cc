#ifndef GOL_BASIC_RAPTOR_ALGORITHM_H_
#define GOL_BASIC_RAPTOR_ALGORITHM_H_

// std
#include <vector>
#include <string>
#include <list>
#include <stdlib.h>
#include <stdio.h>

#include "../utils/logger.h"

namespace gol { 

void basic_raptor_algorithm::raptor_resource_allocation(
      data_Rt& rdata, 
      timetable_Rt& timetable, 
      uint8_t n_rounds)
{
  // allocate static memory blocks
  rdata.n_rounds = n_rounds;
  rdata.minimun_arrival_times = 
      (time_Rt*) malloc(sizeof(time_Rt) * timetable.n_stops);
  rdata.round_earliest_arrival_times =
      ((label_Rt*) malloc(sizeof(label_Rt) * (n_rounds * timetable.n_stops))); 
  rdata.marked_stops = bitset_new(timetable.n_stops);

  if ( ! (rdata.minimun_arrival_times        && 
          rdata.round_earliest_arrival_times && 
          rdata.marked_stops)) 
      throw solver_exception(" Failed allocate static memory blocks for raptor ");         
}

void basic_raptor_algorithm::raptor_resource_release(data_Rt& rdata) 
{
  // free static memory blocks
  free(rdata.minimun_arrival_times);
  free(rdata.round_earliest_arrival_times);
  bitset_destroy(rdata.marked_stops);
}

uint32_t basic_raptor_algorithm::get_route_stops_index(
      timetable_Rt& timetable, 
      uint32_t ridx, 
      uint32_t sidx) 
{ 
  for (uint32_t rsidx = timetable.routes[ridx].route_stops_offset;
	     rsidx < timetable.routes[ridx].route_stops_offset + timetable.routes[ridx].n_stops;
	     ++rsidx) {
	  
    if (timetable.route_stops[rsidx] == sidx) 
      return rsidx;
  }
  return UNDEFINED;

}

bool basic_raptor_algorithm::is_earliest_marked_stop_in_route(
      data_Rt& rdata, 
      uint32_t ridx, 
      uint32_t rsidx) 
{
  auto qit = rdata.Q.find(ridx);
  // if ridx is already in routes to visit, check stop order in route
  return ! (qit != rdata.Q.end() && (*qit).second < rsidx); 

}

// WARNING : real-time updates can ruin FIFO ordering of trips within routes,
// scanning through the whole list of trips reduces speed
void basic_raptor_algorithm::accumulate_routes_for_stop(
      data_Rt& rdata,
      timetable_Rt& timetable, 
      uint32_t sidx) 
{ 
  // check all routes that service this stop
  for (uint32_t sridx = timetable.stops[sidx].stop_routes_offset;                            
    	   sridx < (sidx == (timetable.n_stops - 1) ? timetable.stop_routes.size() 
    	   	                        :timetable.stops[sidx + 1].stop_routes_offset); 
    	   ++sridx) 
  {    
    uint32_t ridx = timetable.stop_routes[sridx];
    uint32_t rsidx = get_route_stops_index(timetable, ridx, sidx);
    if (rsidx != UNDEFINED) 
    {
      if (!is_earliest_marked_stop_in_route(rdata, ridx, rsidx)) 
        continue;
      (rdata.Q)[ridx] = rsidx; 
    }
  }

} 

// the transfer phase will flag routes that should be explored in the next round 
void basic_raptor_algorithm::look_at_foot_paths( 
	    data_Rt& rdata,
      timetable_Rt& timetable,   
	    uint8_t round) 
{ 
  label_Rt* round_offset = rdata.round_earliest_arrival_times + (round * timetable.n_stops); 

  rdata.Q.clear();
  for (uint32_t msidx = bitset_next_set_bit(rdata.marked_stops, 0); 
        msidx < timetable.n_stops; 
        msidx = bitset_next_set_bit(rdata.marked_stops, msidx + 1)) 
  { 
    // the minimun arrival time may have been updated by transfers 
    label_Rt* mslbl = round_offset + msidx;
    time_Rt earliest_arrival_time = mslbl->time; // for current round 
  
    if (earliest_arrival_time == UNREACHED) 
      continue; // exception!

    // no improvements from other transfers
    if (earliest_arrival_time == rdata.minimun_arrival_times[msidx]) 
    {
      mslbl->walk_time = earliest_arrival_time;     
      mslbl->walk_from = msidx; // stop to itself
      assert (rdata.minimun_arrival_times[msidx] == earliest_arrival_time);
      accumulate_routes_for_stop(rdata, timetable, msidx);
      // unflag_banned_routes
    } 
    // apply transfers from the stop to nearby stops 
    uint32_t tr     = timetable.stops[msidx].transfers_offset;
    uint32_t tr_end = (msidx == (timetable.n_stops - 1) ? timetable.transfers.size() 
                                      :timetable.stops[msidx + 1].transfers_offset );
    for ( ; tr < tr_end ; ++tr) 
    {	
      uint32_t sidx_to = timetable.transfers[tr].sidx_to; 
      time_Rt transfer_time = timetable.transfers[tr].length / AVERAGE_WALKING_SPEED;

      label_Rt* slbl_to = round_offset + sidx_to; 
      if (rdata.minimun_arrival_times[sidx_to] == UNREACHED || 
          earliest_arrival_time + transfer_time < rdata.minimun_arrival_times[sidx_to]) 
      {  
        // stop improve by walk transfer
        slbl_to->walk_time = earliest_arrival_time + transfer_time;
        slbl_to->walk_from = msidx;
        rdata.minimun_arrival_times[sidx_to] = earliest_arrival_time + transfer_time;;
        accumulate_routes_for_stop(rdata, timetable, sidx_to);
        // unflag_banned_routes  
      }
    }

  } // end marked stops
  // done with all transfers, reset stop-reached bits for the next round    
  bitset_reset(rdata.marked_stops);    

}

// return the earliest trip in route that one can catch at stop
std::pair<uint32_t, time_Rt> basic_raptor_algorithm::get_earlier_trip(
      timetable_Rt& timetable, 
      uint32_t ridx, 
      uint32_t rsidx, 
      time_Rt prev_time) 
{  
  // trips are time-ordered (do realtime delay change order?) 
  for (uint32_t tidx = timetable.routes[ridx].trips_offset;
		    tidx < (ridx == (timetable.n_routes - 1) ? timetable.trips.size()
				                       :timetable.routes[ridx + 1].trips_offset );
		    ++tidx) 
  {
  	trip_Rt   trip      = timetable.trips[tidx];
    route_Rt  route     = timetable.routes[ridx];
    uint32_t trip_sidx = rsidx - route.route_stops_offset;
    // skip this trip if the realtime delay equals CANCELED  	
    if (trip.realtime_delay == CANCELED) continue; 

  	time_Rt trip_departure_time = 
        timetable.stop_times[trip.stop_times_offset + trip_sidx].departure_time;
    if (trip_departure_time > prev_time) 
	    return std::make_pair(tidx, trip_departure_time);
  }
  return std::make_pair(UNDEFINED, UNREACHED);

}

void basic_raptor_algorithm::round(
      data_Rt& rdata,
      timetable_Rt& timetable, 
      uint8_t round) 
{  
  label_Rt (*round_earliest_arrival_times)[timetable.n_stops] = 
      (label_Rt(*)[timetable.n_stops]) rdata.round_earliest_arrival_times;  

  uint8_t last_round = (round == 0) ? 1/*support round*/ : round - 1;

  // only routes from Q are considered for scanning in current round,   
  // select routes which contain a stop that was marked in the last round 
  for (auto qit = rdata.Q.begin(); qit != rdata.Q.end(); ++qit) 
  {
    uint32_t current_tidx = UNDEFINED;  // means not yet boarded  	
    route_Rt route = timetable.routes[(*qit).first]; 
  	 	
  	uint32_t earlier_tidx; 
    uint32_t back_stop; 
    uint32_t board_stop; 
    time_Rt   trip_departure_time; 
    time_Rt   board_time; 

    // iterate over stop indexes whitin the route
    for (uint32_t rsidx = (*qit).second; // earliest marked stop in route
    	    rsidx < route.route_stops_offset + route.n_stops; 
    	    ++rsidx) 
    {
      uint32_t sidx = timetable.route_stops[rsidx]; // current stop idx

      // if we are not already on a trip, or if we might be able to board a better trip on
      // this route at this location, indicate that we want to search for a trip
      bool catch_trip = false;    
      time_Rt prev_time = round_earliest_arrival_times[last_round][sidx].walk_time;
      if (prev_time != UNREACHED) 
      { 
        // only board at placed that have been reached.
        if (current_tidx == UNDEFINED) {
          catch_trip = true;
        } 
        else {
          // mark trip for boarding if it improves on the last 
          // round's post-walk time at this stop (no transfer slack)
          time_Rt trip_departure_time = 
              timetable.stop_times[
                  timetable.trips[current_tidx].stop_times_offset 
                  + (rsidx - route.route_stops_offset)]
                      .departure_time;          

          if (trip_departure_time != UNREACHED && 
              prev_time < trip_departure_time) 
          { 
            // can we catch an earlier trip at the current stop?
            catch_trip = true; 
          }
        }
      }
      // NOTE: check stop attributes here

      // case : we can board a trip or handle the case where
      // we hit a stop with an existing better departure time
      if (catch_trip) 
      {
      	// scan trips in timetable to find the soonest trip that can be boarded
        // WARNING : check FIFO property, else scan all trips
      	boost::tie(earlier_tidx, trip_departure_time) = 
            get_earlier_trip(timetable, (*qit).first, rsidx, prev_time);
        
        if (earlier_tidx != UNDEFINED)//&& 
            //current_tidx != earlier_tidx) 
        {
          if (trip_departure_time != UNREACHED && 
              trip_departure_time >= prev_time ) 
          { 
            // update current trip
            current_tidx = earlier_tidx;
            board_time   = trip_departure_time;
            board_stop   = sidx;
          }
        }
        continue; // to the next stop in the route
      } 
      // case : we have already boarded a trip along this route 
      else if (current_tidx != UNDEFINED) 
      {        
        time_Rt trip_arrival_time = timetable.stop_times[
            timetable.trips[current_tidx].stop_times_offset 
            + (rsidx - route.route_stops_offset)]
                .arrival_time; 
        
        if (trip_arrival_time == UNREACHED) 
          continue;

        // target pruning : there is no need to mark stops
        // whose arrival time are greater of best time

        /*if ((rdata.minimun_arrival_times[target] != UNREACHED) &&
              (trip_arrival_time > rdata.minimun_arrival_times[target])) 
          continue;*/
        
        // pruning with more targets 
        std::vector<time_Rt> targets_minimun_arrival_times;
        for (uint32_t target : rdata.targets) 
        {
          if (target == UNDEFINED) continue;
          targets_minimun_arrival_times.
            push_back(rdata.minimun_arrival_times[target]); 
        }

        auto upper_minimun_arrival_time = 
          std::max_element(targets_minimun_arrival_times.begin(), 
            targets_minimun_arrival_times.end());  

        if (((*upper_minimun_arrival_time) != UNREACHED) &&
              (trip_arrival_time > (*upper_minimun_arrival_time))) 
          continue;
        
        // local pruning : we are interesting to mark stop during route 
        // traversal when arrival time is earlier that best time at stop
        bool improved = (rdata.minimun_arrival_times[sidx] == UNREACHED) ||
            (trip_arrival_time < rdata.minimun_arrival_times[sidx]); 
        
        if (!improved) 
          continue;
 
        // update stop state 
        round_earliest_arrival_times[round][sidx].time       = trip_arrival_time;
        round_earliest_arrival_times[round][sidx].back_route = (*qit).first; //ridx
        round_earliest_arrival_times[round][sidx].back_trip  = current_tidx;
        round_earliest_arrival_times[round][sidx].back_stop  = board_stop;
        round_earliest_arrival_times[round][sidx].board_time = board_time;

        rdata.minimun_arrival_times[sidx] = trip_arrival_time;
        bitset_set(rdata.marked_stops, sidx); // mark stop for next round.
      }

    } // end for stop
  } // end for route

  // look at foot-paths, handle also the list 
  // of routes to visit for next round based 
  // on stops that were touched in this round.
  look_at_foot_paths(rdata, timetable, round);

  // re-initialize support round before use.
  if (round == 0) { 
    label_Rt* round_labels = rdata.round_earliest_arrival_times + (1 * timetable.n_stops);
    for (uint32_t source : rdata.sources) {
      round_labels[source].walk_time = UNREACHED;
      round_labels[source].walk_from = UNDEFINED;
      uint32_t tr  = timetable.stops[source].transfers_offset;
      uint32_t trN = (source == (timetable.n_stops - 1) ? timetable.transfers.size()  
                                     :timetable.stops[source + 1].transfers_offset );
      for ( ; tr < trN ; ++tr) {
          uint32_t sidx_to = timetable.transfers[tr].sidx_to;
          round_labels[sidx_to].walk_time = UNREACHED;
          round_labels[sidx_to].walk_from = UNDEFINED;
      }
    } 
  }

}

void basic_raptor_algorithm::raptor_initialization(
    data_Rt& rdata,
    timetable_Rt& timetable, 
    const std::vector<
        std::pair<std::string, time_Rt> 
    >& near_stops_src,
    const std::vector<
        std::pair<std::string, time_Rt> 
    >& near_stops_trg)
{ 
  label_Rt (*round_earliest_arrival_times)[timetable.n_stops] = 
    (label_Rt(*)[timetable.n_stops]) rdata.round_earliest_arrival_times; 
  
  // stops were reached from road origin
  for (auto stop : near_stops_src) 
  {
    // mapping road-identifiers on raptor-indexes 
    auto sidx_it = timetable.stopidx_map.find(stop.first);
    if (sidx_it == timetable.stopidx_map.end())
      continue; // throw exception
    rdata.sources.push_back((*sidx_it).second);
    // /*
    uint32_t sidx = (*sidx_it).second;
    time_Rt   time_to_reach_stop = stop.second; 
    
    // check all routes that service this stop
    for (uint32_t sridx = timetable.stops[sidx].stop_routes_offset; 
         sridx < (sidx == (timetable.n_stops - 1) ? timetable.stop_routes.size() 
                                  :timetable.stops[sidx + 1].stop_routes_offset); 
         ++sridx) 
    { 
      uint32_t ridx  = timetable.stop_routes[sridx];
      uint32_t rsidx = get_route_stops_index(timetable, ridx, sidx);

      if (rsidx != UNDEFINED) 
      {     
        auto it = rdata.route_sources.find(ridx);
        if (it == rdata.route_sources.end())
          rdata.route_sources.
            insert(std::make_pair(ridx, std::vector<uint32_t>{rsidx}));
        else
          rdata.route_sources[ridx].push_back(rsidx);
      }
    }
    //*/

    // initialization of states
    rdata.minimun_arrival_times[sidx]               = time_to_reach_stop;    
    round_earliest_arrival_times[1][sidx].time      = time_to_reach_stop;
    round_earliest_arrival_times[1][sidx].walk_time = time_to_reach_stop;
    round_earliest_arrival_times[1][sidx].walk_from = sidx;

    round_earliest_arrival_times[0][sidx].time      = time_to_reach_stop;
    //round_earliest_arrival_times[0][sidx].walk_time = time_to_reach_stop;
    //round_earliest_arrival_times[0][sidx].walk_from = sidx;
    
    round_earliest_arrival_times[0][sidx].from_location_time = time_to_reach_stop;   
    
    //bitset_set(rdata.marked_stops, sidx);
    accumulate_routes_for_stop(rdata, timetable, sidx);   

  }
  if (near_stops_src.empty())
    throw solver_exception("raptor_initialization(): sources empty");

  // stops were reached from road destination
  for (auto stop : near_stops_trg) 
  {
    auto sidx_it = timetable.stopidx_map.find(stop.first);
    if (sidx_it == timetable.stopidx_map.end())
      continue; // throw exception
    rdata.targets.push_back((*sidx_it).second);
 
    uint32_t sidx = (*sidx_it).second;   
    time_Rt  time_to_reach_stop = stop.second; 
    round_earliest_arrival_times[0][sidx].to_location_time = time_to_reach_stop;
  }
  if (near_stops_trg.empty())
    throw solver_exception("raptor_initialization(): targets empty"); 

}

void basic_raptor_algorithm::compute (
    timetable_Rt& timetable, 
    std::vector<
        std::pair<std::string, time_Rt> 
    >& near_stops_src, 
    std::vector<
        std::pair<std::string, time_Rt> 
    >& near_stops_trg, 
    round_based_solver_result& pareto_set, 
    uint8_t n_transfers) 
{  
  stopwatch chrono;
  // rounds iter upper bound 
  uint8_t n_rounds = n_transfers + 1;
  if (n_rounds > RAPTOR_MAX_ROUNDS)
    n_rounds = RAPTOR_MAX_ROUNDS;

  data_Rt rdata; 
  raptor_resource_allocation(rdata, timetable, n_rounds);

  // algorithm associates with each stop a multilabel where ith element 
  // rapresents the earliest known arrival time at stop with up to i trips
  label_Rt (*round_earliest_arrival_times)[timetable.n_stops] = 
    (label_Rt(*)[timetable.n_stops]) rdata.round_earliest_arrival_times; // cast to [round][stop] 

  // initialization of the algorithm
  for (uint32_t sidx = 0; sidx < timetable.n_stops; ++sidx) rdata.minimun_arrival_times[sidx] = UNREACHED;
  for (uint8_t rnd = 0; rnd < n_rounds; ++rnd) 
  {
    for (uint32_t sidx = 0; sidx < timetable.n_stops; ++sidx) 
    { 
      // we use the time fields to record when stops have been reached,
      // when times are UNREACHED the other fields in the same round state should never be read.
      round_earliest_arrival_times[rnd][sidx].time       = UNREACHED;
      round_earliest_arrival_times[rnd][sidx].walk_time  = UNREACHED;
      round_earliest_arrival_times[rnd][sidx].walk_from  = UNDEFINED;
      round_earliest_arrival_times[rnd][sidx].back_stop  = UNDEFINED;
      round_earliest_arrival_times[rnd][sidx].back_route = UNDEFINED;
      round_earliest_arrival_times[rnd][sidx].back_trip  = UNDEFINED;
      round_earliest_arrival_times[rnd][sidx].board_time = UNREACHED;
      // from previus road-network phase
      round_earliest_arrival_times[rnd][sidx].from_location_time = UNREACHED;
      round_earliest_arrival_times[rnd][sidx].to_location_time   = UNREACHED;
    }
  }
  // multi-source multi-target initialization    
  raptor_initialization(rdata, timetable, near_stops_src, near_stops_trg);

  // iterate over rounds. In round N, we have made N transfers
  for (uint8_t rnd = 0; rnd < n_rounds; ++rnd) {
    round (rdata, timetable, rnd);
  }

  // retrieve multimodal paths
  pareto_set = get_transit_route(rdata, timetable, n_rounds, pareto_set);
#ifdef DEBUG  
  dump(pareto_set, timetable);
#endif  
  raptor_resource_release(rdata);
  
};

round_based_solver_result basic_raptor_algorithm::get_transit_route(
      data_Rt& rdata, 
      timetable_Rt& timetable,
      uint8_t n_rounds, 
      round_based_solver_result& pareto_set) 
{  
  //round_based_solver_result pareto_set; 
  pareto_set.n_paths = 0;

  label_Rt (*states)[timetable.n_stops] = 
      (label_Rt(*)[timetable.n_stops]) rdata.round_earliest_arrival_times;
  
  // loop over the rounds to get ending states of itineraries 
  // using different numbers of vehicles/transfer
  for (int n_xfers = 0; n_xfers < n_rounds; ++n_xfers) 
  {
    path_Rt path;      
    uint32_t sidx = UNDEFINED;
    
    // TODO: can we introduce another criterio for targets?
    time_Rt round_earliest_location_arrival_time = UNREACHED;             
    // backward reconstruction from best target at round k
    for (uint32_t target_sidx : rdata.targets) 
    {
      //std::cout << states[n_xfers][target_sidx] << std::endl;
            // TODO: check time_Rt (int) overflow, sum became negative
      if (states[n_xfers][target_sidx].walk_time == UNREACHED 
          || states[n_xfers][target_sidx].to_location_time == UNREACHED) 
        continue; // skip target that was not reached
      
      time_Rt location_arrival_time = 
          states[n_xfers][target_sidx].walk_time + 
          states[n_xfers][target_sidx].to_location_time;
      
      if (location_arrival_time < round_earliest_location_arrival_time)       
      {
        round_earliest_location_arrival_time = location_arrival_time;
        sidx = target_sidx;
      }
    }

    if (sidx == UNDEFINED) 
      continue; // skip rounds that were not reached   
    
    // if (states[n_xfers][sidx].walk_time == UNREACHED) 
    //   continue; // skip rounds that were not reached 
    
    path.n_rides = n_xfers + 1;
    path.n_connctions = path.n_rides * 2 + 1; // always same number of connections for same number of transfers       
    
    // follow the chain of states backward         
    for (int round = n_xfers; round >= 0; --round) 
    { 
      if (sidx > timetable.n_stops) {
        break; // stop out of range
      }      

      // walk phase 
      label_Rt walk = states[round][sidx]; 
      if (walk.walk_time == UNREACHED) {
        break; // stop was unreached by walking
      }
      //std::cout <<  walk << std::endl;
      uint32_t walk_stop = sidx;
      sidx = walk.walk_from;  // follow the chain of states backward
 
      // ride phase 
      label_Rt ride = states[round][sidx];
      if (ride.time == UNREACHED) {
        break; // stop was unreached by riding
      }
      //std::cout << ride << std::endl;
      uint32_t ride_stop = sidx;
      sidx = ride.back_stop; // follow the chain of states backward
           
      // walk phase
      connection_Rt walk_c;
      walk_c.sidx_from = walk.walk_from;
      walk_c.sidx_to = walk_stop;
      walk_c.departure_time = ride.time; // rendering the walk requires already having the ride arrival time
      walk_c.arrival_time = walk.walk_time;
      walk_c.ridx = WALK;
      walk_c.tidx = WALK;
      path.connections.push_front(walk_c); // next connection 

      // ride phase
      connection_Rt ride_c; 
      ride_c.sidx_from = ride.back_stop;
      ride_c.sidx_to = ride_stop;
      ride_c.departure_time = ride.board_time;
      ride_c.arrival_time = ride.time;
      ride_c.ridx = ride.back_route;
      ride_c.tidx  = ride.back_trip;
      path.connections.push_front(ride_c); // next connection
    }

    // look at previus sources within the route that were  
    // reached from origin and that were ignored by algorithm
    auto     first_ride          = path.connections.begin(); 
    // /* with more sources
    uint32_t current_rsidx       = get_route_stops_index(timetable, first_ride->ridx, first_ride->sidx_from);
    time_Rt   minimun_walk_time   = states[0][first_ride->sidx_from].from_location_time;
    time_Rt   trip_departure_time = first_ride->departure_time;

    for (auto rsidx : rdata.route_sources[first_ride->ridx])  
    { 
      // only previus stops within route
      if (rsidx >= current_rsidx) 
        continue;

      uint32_t source_sidx   = timetable.route_stops[rsidx];
      label_Rt current_state = states[0][source_sidx];
      uint32_t tidx;
      time_t   departure_time;
      boost::tie(tidx, departure_time) = 
          get_earlier_trip(timetable, first_ride->ridx, rsidx, current_state.from_location_time);
            
      if (current_state.from_location_time != UNREACHED         &&
          current_state.from_location_time <= minimun_walk_time &&
          tidx == first_ride->tidx) 
      {
        minimun_walk_time = current_state.from_location_time;
        trip_departure_time = departure_time;
        sidx = source_sidx; 
      }
    }
    // handle update of start state of first ride in path
    label_Rt first_state = states[0][sidx];
    if (first_state.time == UNREACHED) {
      break; // throw exception!
    }     
    first_ride->sidx_from = sidx;
    first_ride->departure_time = trip_departure_time; 
    //*/

    // the initial walk connection leading out of the road-origin, 
    // this is inferred, not stored explicitly
    connection_Rt road_walk_c;
    road_walk_c.sidx_from = UNDEFINED; // road-origin
    road_walk_c.sidx_to = sidx;
    // it would also be possible to work from s1 to s0 and compress out the wait time.
    road_walk_c.departure_time = UNREACHED;
    road_walk_c.arrival_time = first_ride->departure_time;
    road_walk_c.ridx = WALK;
    road_walk_c.tidx = WALK;
    //path.connections.push_front(road_walk_c);
      
    // move to the next path from target
    pareto_set.paths.push_back(path);
    ++pareto_set.n_paths;
   
  } // for xfers

  //check_invariants (pareto_set);
  return pareto_set;
}

void basic_raptor_algorithm::dump(round_based_solver_result& pareto_set, timetable_Rt& timetable)
{
  if (pareto_set.paths.empty()) {
    logger(logDEBUG) << left("[raptor]", 14) << " * route not found";
    return;
  }  
  for (auto p : pareto_set.paths) {
    logger(logDEBUG) << left("[raptor]", 14) << "[" << p.n_rides - 1 << "]xsfers";
    for (auto c : p.connections) {
      if ( c.tidx == WALK ) 
        logger(logDEBUG) 
          << left("[raptor]", 14) 
          << left(">", 3) 
          << left("Walk : [-]", 80); 
      else 
        logger(logDEBUG) 
          << left("[raptor]", 14) 
          << left(">", 3) 
          << left("Ride : [ " + timetable.trips[c.tidx].id + " ]", 80);
      
      logger(logDEBUG) << left("[raptor]", 14) << left(">", 3)
        << "  [ id = "<< left(timetable.stops[c.sidx_from].id, 14) 
        << " at "<< to_string(c.departure_time) 
        << ", id = "
        << left(timetable.stops[c.sidx_to].id, 14) 
        << " at "<< to_string(c.arrival_time) << "]";      
    }
  }

}


} // namespace gol

#endif // GOL_BASIC_RAPTOR_ALGORITHM_H_