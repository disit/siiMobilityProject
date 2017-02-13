
#include "utility.h"

namespace gol {

std::string ch16tostr(const XMLCh* ch16) {
  char* ch8 = XMLString::transcode(ch16);
  std::string str8(ch8);
  xercesc::XMLString::release(&ch8);
  return str8;
}

bool ch16strcmp(const XMLCh* ch16, std::string str) {
  std::string _str = ch16tostr(ch16);
  return (_str == str);
}

std::string no_space_str(std::string str) {
  std::string ns_str = str;
  std::transform(ns_str.begin(), ns_str.end(), ns_str.begin(), [](char ch) {
    return ch == ' ' ? '?' : ch;});
  return ns_str;
}

std::string get_today() 
{
  std::time_t rawtime;
  std::tm* timeinfo;
  char buffer [80];

  std::time(&rawtime);
  timeinfo = std::localtime(&rawtime);

  std::strftime(buffer,80,"%Y-%m-%d",timeinfo);
  //std::cout << std::string(buffer) << std::endl;
  return "2014-08-11"; // std::string(buffer); // TODO da cambiare!!
}    

time_Rt to_rtime(const std::string& str_time, const std::string& today) 
{ 
  // split request time 
  std::string date = str_time.substr(0, str_time.find("T",0));
  std::string time = str_time.substr(str_time.find("T", 0) + 1, str_time.length());
  time_Rt hh = 0, mm = 0, ss = 0;

  for (uint i = 0; i < time.size(); i++)
    if (time[i] == ':')
      time[i] = ' ';

  std::istringstream str_stream(time);

  str_stream >> hh;
  str_stream >> mm;
  str_stream >> ss;

  if (date.compare(today) != 0) 
    return ( (hh * 3600 + mm * 60 + ss) + (60 * 60 * 24) ); // dep/arr at next day

  return (hh * 3600 + mm * 60 + ss);    
}

std::string to_string(const time_Rt time) 
{
  time_Rt _time = time;
  std::ostringstream str_stream;

  str_stream.width(2);
  str_stream.fill('0');
  //str_stream << time / 3600 << ":";
  str_stream << (_time / 3600) % 24 << ":";

  str_stream.width(2);
  str_stream.fill('0');
  str_stream << (_time % 3600) / 60 << ":";

  str_stream.width(2);
  str_stream.fill('0');
  str_stream << (_time % 60);

  return str_stream.str();
}

std::string to_osm_nd(const std::string& nd) {
  std::string s = nd;
  return s.substr(s.find("n", 0) + 1, s.length());
}


}  // namespace gol
