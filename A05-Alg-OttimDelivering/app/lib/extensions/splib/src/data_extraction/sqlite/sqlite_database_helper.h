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

#ifndef __SQLITE_H__
#define __SQLITE_H__

// std
#include <string>
#include <sstream>
#include <fstream>    // std::fstream
#include <iterator>   // std::istreambuf_iterator
// boost
#include <boost/algorithm/string.hpp>

#include "sqlite3xx.h"
#include "sqlite3xx_ext.h"

#include "../../common.h"
#include "../OSM.h"

namespace gol {

class sqlite_database_helper_t
{
 public:
  sqlite_database_helper_t(
    std::string db_name);
  ~sqlite_database_helper_t() {}

  bool empty();
  void commit();
  void reset();
  
  void clear_cache();

  void retrieve(std::vector<osm::node>& nds);
  void retrieve(std::vector<osm::way>&  wys);
  void retrieve(std::vector<
    osm::node_as_via_turn_restriction>& trs);

  void cache_element(osm::node e) {
    _cached_nds.push_back(e);
  }

  void cache_element(osm::way e) {
    _cached_wys.push_back(e);
  }

  void cache_element(osm::relation e) {
    _cached_rls.push_back(e);
  }

 private:
  void create_schema();

  void insert_cached_nodes();
  void insert_cached_ways();
  void insert_cached_relations();

  sqlite3xx::database        _db;
  // cached osm elements
  std::vector<osm::node>     _cached_nds;
  std::vector<osm::way>      _cached_wys;
  std::vector<osm::relation> _cached_rls;

};

}  // namespace gol

#endif // __SQLITE_H__
