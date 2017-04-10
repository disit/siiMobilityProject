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


#ifndef GOL_UTILS_LOG_H_
#define GOL_UTILS_LOG_H_

// std
#include <iostream>
#include <iomanip>
#include <ctime>
#include <utility>
#include <atomic>
#include <mutex>
#include <sstream>
// boost 
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "bash_color.h"
#include "stopwatch.h"

/* Convert double to string with specified number of places after the decimal. */
std::string prd(const double x, const int decDigits);

/* Convert double to string with specified number of places after the decimal
   and left padding. */
std::string prd(const double x, const int decDigits, const int width);

/* Convert std::pair<int,int> to string with specified number of places after the decimal
   and left padding. */
std::string prd(const std::pair<int,int>& x, const int decDigits);

/* Center-aligns string within a field of width w. Pads with blank spaces
   to enforce alignment. */
std::string center(const std::string& s, const int w);

/* Right-aligns string within a field of width w. Pads with blank spaces
   to enforce alignment. */
std::string right(const std::string& s, const int w);

/* Left-aligns string within a field of width w. Pads with blank spaces
   to enforce alignment. */
std::string left(const std::string& s, const int w);

enum log_type
{
  logINFO,
  logWARNING,
  logERROR,
  logDEBUG
};

class log_policy
{
  public:
    void umtx();
    void mtx();
    bool is_mtx() const;

    static log_policy &get_instance();

    log_policy(const log_policy &)            = delete;
    log_policy &operator=(const log_policy &) = delete;

  private:
    log_policy(): 
      _is_mtx(true) {}
    
    std::atomic<bool> _is_mtx;
};

class logger
{
 public:
  logger(log_type logT = logINFO);
  logger(log_type logT, std::ostream & stream);

  virtual ~logger();
  std::mutex &get_mutex();

  std::string get_time();
  std::string get_header();
  
  template <typename T> 
  inline 
  std::ostream &operator<<(const T &data) { 
    return _stream << data; }

  protected:
    log_type _logT;
    std::ostringstream _buffer;
    std::ostream & _stream;
};

#endif // GOL_UTILS_LOG_H_
