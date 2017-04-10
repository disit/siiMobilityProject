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

#ifndef GOL_EXCEPTION_H_
#define GOL_EXCEPTION_H_

 // std
#include <string>
#include <exception>

namespace gol {

class exception_t : public std::exception { 
 public:
  exception_t() throw() 
      : std::exception(), 
        _message() {}
  explicit exception_t(std::string msg) throw()
      : std::exception(), 
        _message(msg) {}
  ~exception_t() throw() {}

  const char* what() const throw() { 
    return _message.c_str(); 
  }
  
  virtual int code() const { return 101; }

 protected:
  std::string _message;

};

struct data_exception : public exception_t { 
  data_exception(std::string msg) throw() 
      : exception_t(msg) {}
  
  int code() const { return 107; }

};

struct runtime_exception : public exception_t { 
  runtime_exception(std::string msg) throw() 
      : exception_t(msg) {}
    
  int code() const { return 108; }

};

struct parser_exception : public exception_t {
  parser_exception() 
      : exception_t() {}
  
  explicit parser_exception(std::string msg) 
      : exception_t(msg) {}

};

struct builder_exception : public exception_t {
  builder_exception() 
      : exception_t() {}
  
  explicit builder_exception(std::string msg) 
      : exception_t(msg) {}

};

struct solver_exception : public exception_t {
  solver_exception() 
      : exception_t() {}
  
  explicit solver_exception(std::string msg) 
      : exception_t(msg) {}

};

// the search algorithm has found the target node 
struct target_found : public exception_t {     
  int code() const { return 103; }

};

// the search algorithm has found the target node 
struct all_targets_found : public exception_t {     
  int code() const { return 104; }

};

// the search algorithm was not able to find the target node 
struct target_not_found : public exception_t { 
  const char* what() const throw() { return "Target Not Found"; }
  int code() const { return 105; }

};


}  // namespace gol

#endif  // GOL_EXCEPTION_H_
