#ifndef GOL_ROUND_BASED_SOLVER_FACTORY_H_
#define GOL_ROUND_BASED_SOLVER_FACTORY_H_


#include "raptor_solver.h"

namespace gol {

/**
*
*/
class raptor_solver_factory {
 public:
  static raptor_solver* get_solver_for(
      timetable_Rt& tt,
      std::string algorithm, 
      std::vector<
          std::pair<std::string, time_Rt> >& svec, 
      std::vector<
          std::pair<std::string, time_Rt> >& tvec,
      std::string request_time) 
  {    
    if (algorithm == "basic_raptor") 
    {
      logger(logINFO) 
          << left("[solver] ", 14) 
          << "Round-Based Public Transit Routing algorithm"; 
      return new DP_raptor_solver <RAPTOR_algorithm>(tt, svec, tvec, request_time);
    }
    throw solver_exception();   
  }

 private:
  // always declare assignment operator and default and copy constructor
  raptor_solver_factory();
  ~raptor_solver_factory();
  raptor_solver_factory(const raptor_solver_factory&);
  raptor_solver_factory& operator=(const raptor_solver_factory&);

};

} // namespace gol

#endif // GOL_ROUND_BASED_SOLVER_FACTORY_H_