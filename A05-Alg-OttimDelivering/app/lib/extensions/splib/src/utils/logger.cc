// This file is part of Sii-Mobility - Algorithms Optimized Delivering.
//
// Copyright (C) 2017 GOL Lab http://webgol.dinfo.unifi.it/ - University of Florence
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with This program.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>

#include "../utils/logger.h"

std::string prd(const double x, const int decDigits) {
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(decDigits);  // set # places after decimal
  ss << x;
  return ss.str();
}
std::string prd(const double x, const int decDigits, const int width) {
  std::stringstream ss;
  ss << std::fixed << std::right;
  ss.fill(' ');            // fill space around displayed #
  ss.width(width);         // set  width around displayed #
  ss.precision(decDigits); // set # places after decimal
  ss << x;
  return ss.str();
}

std::string prd(const std::pair<int,int>& x, const int decDigits) {
  return "(" + prd(x.first, decDigits) + ", " + prd(x.second, decDigits) + ")";
}

std::string center(const std::string& s, const int w) {
  std::stringstream ss, spaces;
  int padding = w - s.size();                 // count excess room to pad
  for(int i=0; i<padding/2; ++i)
      spaces << " ";
  ss << spaces.str() << s << spaces.str();    // format with padding
  if(padding>0 && padding%2!=0)               // if odd #, add 1 space
      ss << " ";
  return ss.str();
}

std::string right(const std::string& s, const int w) {
  std::stringstream ss, spaces;
  int padding = w - s.size();                 // count excess room to pad
  for(int i=0; i<padding; ++i)
      spaces << " ";
  ss << spaces.str() << s;                    // format with padding
  return ss.str();
}

std::string left(const std::string& s, const int w) {
  std::stringstream ss, spaces;
  int padding = w - s.size();                 // count excess room to pad
  for(int i=0; i<padding; ++i)
      spaces << " ";
  ss << s << spaces.str();                    // format with padding
  return ss.str();
}

void log_policy::umtx() { _is_mtx = false; }

void log_policy::mtx() { _is_mtx = true; }

bool log_policy::is_mtx() const { return _is_mtx; }

log_policy &log_policy::get_instance()
{
  static log_policy instance;
  return instance;
}

std::string logger::get_time()
{
  std::string time_str;
  time_t raw_time;
  time( & raw_time );
  time_str = ctime( &raw_time );
  //without the newline character
  return time_str.substr( 0 , time_str.size() - 1 );
}

std::string logger::get_header()
{
  std::stringstream header;
  header <<"[ "<<get_time()<<" - ";
  header.fill('0');
  header.width(10);
  header <<clock()<<" ]";
  return header.str();
}

logger::logger(log_type logT, std::ostream & stream): 
   _logT(logT), 
   _stream(stream)
{
  std::lock_guard<std::mutex> lock(get_mutex());
  switch (_logT) 
  {
    case logWARNING:
        _stream << get_header() << "[wrn] ";
        break;
    case logERROR:
        _stream << get_header() << "[err] ";
        break;
    case logDEBUG:
#ifndef NDEBUG
        _stream << get_header() << "[dbg] ";
#endif
        break;
    default: 
        _stream << get_header() << "[inf] ";
        break;
  }
}

logger::logger(log_type logT): 
   logger(logT, _buffer) {}

std::mutex &logger::get_mutex()
{
  static std::mutex mtx;
  return mtx;
}

logger::~logger()
{
  std::lock_guard<std::mutex> lock(get_mutex());
  const bool usestd = (&_stream == &_buffer);
  if (!log_policy::get_instance().is_mtx())
  {
      if (usestd) 
      { 
        switch (_logT) 
        {
          case logERROR:
              std::cerr << _buffer.str();
              //std::cerr << RESET;
              std::cerr << std::endl;
              break;
          case logDEBUG:
#ifdef NDEBUG
              break;
#endif
          case logWARNING:
          case logINFO:
            default:
              std::cout << _buffer.str();
              //std::cout << RESET;
              std::cout << std::endl;
              break;
        }
      }
      else 
      {
          //_stream << RESET;
          _stream << std::endl;
      }
  }
}
