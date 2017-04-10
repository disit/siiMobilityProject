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

#ifndef GOL_COMMON_H_
#define GOL_COMMON_H_

// std
#include <iostream>
#include <utility>
#include <stdint.h>
#include <vector>
#include <map>
#include <string>
// boost
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include "config.h"

namespace gol {	

// structure to hold statistical information.
struct stats_t {
  stats_t() 
      : run_time(0), 
        expansions(0),
        visited_nodes(0) {}
  double run_time;
  unsigned int expansions;
  unsigned int visited_nodes;
};

} // namesocae gol

#include "utility.h"
#include "exception.h"
#include "geo.h"
#include "utils/stopwatch.h"
#include "utils/logger.h"
#include "utils/bitset.h"
#include "route.h"

#endif  // GOL_COMMON_H_