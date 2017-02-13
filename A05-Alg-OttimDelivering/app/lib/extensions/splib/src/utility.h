
#ifndef GOL_UTILITY_H__
#define GOL_UTILITY_H__

// std
#include <cstdlib>
#include <string>
#include <algorithm>
#include <sstream>
#include <ctime>
// xercesc
#include <xercesc/util/XMLString.hpp>

namespace gol {

XERCES_CPP_NAMESPACE_USE

// time format 
typedef int time_Rt;

std::string ch16tostr(const XMLCh* ch16);
bool ch16strcmp(const XMLCh* ch16, std::string str);
std::string no_space_str(std::string str);

std::string get_today();
time_Rt to_rtime(const std::string& str_time, const std::string& today);
std::string to_string(const time_Rt time);

std::string to_osm_nd(const std::string& nd);

}  // namespace gol

#endif  // GOL_UTILITY_H__ 
