#ifndef GOL_ROUND_BASED_SOLVER_H_
#define GOL_ROUND_BASED_SOLVER_H_

namespace gol {

class raptor_solver 
{
 public:
  virtual void solve() = 0;

 private:
  virtual round_based_solver_result get_result() = 0;

 protected:
  raptor_solver(timetable_Rt& tt): 
    _tt(tt), 
    _stats() {}
  ~raptor_solver() {}

  timetable_Rt& _tt;	
  struct stats_t _stats;   

 public:
  optimized_routes get_optimized_routes()
  {
    optimized_routes opt;
    for (auto path : get_result().paths) 
    {
      Route route;
      for (auto c : path.connections) 
      {
        route.add_route_edge(
          0,  // TODO: length
          "", // TODO: description
          _tt.stops[c.sidx_from].id,
          _tt.stops[c.sidx_from].lon, 
          _tt.stops[c.sidx_from].lat,
          _tt.stops[c.sidx_to].id,
          _tt.stops[c.sidx_to].lon, 
          _tt.stops[c.sidx_to].lat);
      }
      opt.push_back(route);
    }
    return opt;
  }

  double get_run_time() { return _stats.run_time; }

};

} // namespace gol

#include "round_based_solver/raptor_timetable_solver.h"

#endif // GOL_ROUND_BASED_SOLVER_H_