#ifndef GOL_RAPTOR_ARRAY_TT_BUILDER_H_
#define GOL_RAPTOR_ARRAY_TT_BUILDER_H_

#include "../../common.h"
#include "../raptor_timetable.h"

namespace gol {

/**
*
*/
class raptor_timetable_builder {
 public:
  raptor_timetable_builder(timetable_Rt* tt)
      : _tt(tt), _rtstime_map(), _sr_map(), _rs_map() {}
  ~raptor_timetable_builder() {}

  void add_route(std::string rid);
  void add_trip(std::string tid); 
  void add_stop(
  	   std::string sid, 
  	   double lon, 
  	   double lat, 
  	   std::string rid);
  void add_stop_time(
  	   std::string rid, 
  	   std::string tid, 
  	   std::string sid, 
  	   time_Rt departure_time, 
  	   time_Rt arrival_time);
  void add_transfer(
  	   std::string sid_from, 
  	   std::string sid_to, 
  	   double dist);

  //void construct_array_timetable();
  void rtimetable();

 private:
  raptor_timetable_builder(const raptor_timetable_builder&);
  raptor_timetable_builder& operator=(const raptor_timetable_builder&);

  timetable_Rt* _tt;
  std::map<std::string /*route id*/, 
     std::map<std::string /*trip id*/, std::list<stime_Rt> > > _rtstime_map;
  std::map<stop_Rt, std::list<route_Rt> > _sr_map;
  std::map<route_Rt, std::list<stop_Rt> > _rs_map;

};

} // namespace gol

//#include "raptor_timetable_builder.cc"

#endif // GOL_RAPTOR_ARRAY_TT_BUILDER_H_