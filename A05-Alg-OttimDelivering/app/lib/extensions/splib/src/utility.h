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
typedef int      time_Rt;
typedef uint32_t edge_index_t;	// TODO: move

std::string ch16tostr(const XMLCh* ch16);
bool ch16strcmp(const XMLCh* ch16, std::string str);
std::string no_space_str(std::string str);

std::string get_today();
time_Rt to_rtime(const std::string& str_time, const std::string& today);
std::string to_string(const time_Rt time);

std::string to_osm_nd(const std::string& nd);

}  // namespace gol

#endif  // GOL_UTILITY_H__ 
