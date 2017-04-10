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

#ifndef __SPARQL_H__
#define __SPARQL_H__

// std
#include <string>
#include <sstream>
#include <fstream>    // std::fstream
#include <iterator>   // std::istreambuf_iterator


namespace gol {

class sparql_query_helper_t {
 public:
  sparql_query_helper_t(const std::string&, const std::string&);
  ~sparql_query_helper_t() {}

  std::string retrieve();

 private:
  std::string _query; // sparql query
  std::string _url;   // url sparql endpoint
};

}  // namespace gol

#endif // __SPARQL_H_
