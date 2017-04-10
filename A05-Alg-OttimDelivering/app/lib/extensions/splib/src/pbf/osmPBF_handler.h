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

#ifndef GOL_PBF_DEFAULT_HEANDLER_H_
#define GOL_PBF_DEFAULT_HEANDLER_H_

// std
#include <stdint.h>
#include <string>

#include "osm_tag.h"
#include "osm_reference.h" 

namespace gol { namespace pbf {

class default_handler {
 public:    
  // This method is called every time a Node is read
  virtual void node_callback(
    uint64_t      osm_id, 
    double        lon, 
    double        lat, 
    const tags_t& tags);
  // This method is called every time a Way is read
  // refs is a vector that contains the reference to the nodes that compose the way
  virtual void way_callback(
    uint64_t                     osm_id, 
    const tags_t&                tags, 
    const std::vector<uint64_t>& refs);
  // This method is called every time a Relation is read
  // refs is a vector of pair corresponding of the relation  
  // type (Node, Way, Relation) and the reference to the object
  virtual void relation_callback(
    uint64_t            osm_id, 
    const tags_t&       tags, 
    const references_t& refs);

  default_handler() {}
  ~default_handler() {}  
};

// Inline default implementations
inline void default_handler::node_callback(
  uint64_t      /*osm_id*/, 
  double        /*lon*/, 
  double        /*lat*/, 
  const tags_t& /*tags*/) {}
inline void default_handler::way_callback(
  uint64_t                     /*osm_id*/, 
  const tags_t&                /*tags*/, 
  const std::vector<uint64_t>& /*refs*/) {}
inline void default_handler::relation_callback(
  uint64_t            /*osm_id*/, 
  const tags_t&       /*tags*/, 
  const references_t& /*refs*/) {}

} // namespace pbf
} // namespace gol

#endif // GOL_PBF_DEFAULT_HEANDLER_H_
