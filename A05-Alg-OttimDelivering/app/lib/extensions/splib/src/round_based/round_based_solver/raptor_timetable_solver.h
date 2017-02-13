#ifndef GOL_RAPTOR_ARRAY_TIMETABLE_SOLVER_H_
#define GOL_RAPTOR_ARRAY_TIMETABLE_SOLVER_H_

namespace gol {

template <typename DPAlgorithm>
class DP_raptor_solver : 
  public raptor_solver 
{
 public:
  DP_raptor_solver( 
      timetable_Rt& tt,
      std::vector<
          std::pair<std::string, time_Rt> >& svec, 
      std::vector<
          std::pair<std::string, time_Rt> >& tvec,
      std::string request_time)
      : raptor_solver(tt),
        _svec(svec), 
        _tvec(tvec),
        _request_time(request_time),
        _pareto_set() {} 
  ~DP_raptor_solver() {}
    
  virtual void solve() override {    
    DPAlgorithm::compute(
      (this->_tt), 
      _svec, 
      _tvec, 
      _pareto_set, 
      MAX_TRANSFER);  
  }

  virtual round_based_solver_result get_result() override { 
    return _pareto_set; 
  }
    
 private: 
  std::vector<
      std::pair<std::string, time_Rt> >& _svec; 
  std::vector<
      std::pair<std::string, time_Rt> >& _tvec;
  std::string _request_time;
  round_based_solver_result _pareto_set;  

};

} // namespace gol

#endif // GOL_RAPTOR_ARRAY_TIMETABLE_SOLVER_H_