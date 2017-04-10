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

#include <algorithm>

namespace gol { 
  namespace osm {

/// OSM Protocol Protocolbuffer Binary Format
template <typename BuilderT>
void parserPBF<BuilderT>::node_callback(
    uint64_t           osm_id, 
    double             lon, 
    double             lat, 
    const pbf::tags_t& tags) 
{  
  osm::node n;
  n.id   = (long long int)osm_id;
  n.lat  = lat;
  n.lon  = lon;
  n.ele  = MARIANA_TRENCH_DEPTH;
  n.tags = tags;
  
  _dbh.cache_element(n); 
}

template <typename BuilderT>
void parserPBF<BuilderT>::way_callback(
    uint64_t                     osm_id, 
    const pbf::tags_t&           tags, 
    const std::vector<uint64_t>& refs) 
{
  osm::way w;
  w.id   = (long long int)osm_id;
  w.tags = tags;
  for (auto nd : refs)
    w.add_ref( (long long int)nd );
  
  _dbh.cache_element(w);
}

template <typename BuilderT>
void parserPBF<BuilderT>::relation_callback(
    uint64_t                 osm_id, 
    const pbf::tags_t&       tags, 
    const pbf::references_t& refs)
{
  osm::relation r;
  r.id   = (long long int)osm_id;
  r.tags = tags;
  for (auto rm : refs) {
    osm::relation_member mem(rm.member_id, rm.role, rm.member_type);
    r.add_ref(mem);
  }
  
  _dbh.cache_element(r);
}

template <typename BuilderT>
void parserPBF<BuilderT>::construct_model() 
{
  if (_dbh.empty()) 
  { 
    logger(logINFO)
        << left("[DB]", 14)
        << "> Create DB From OSM Protocolbuffer Binary Format";            

    pbf::osm_reader<parserPBF> parser(_filename, *this);
    parser.parse();
    _dbh.commit();    
  }  

  _builder->build_graph(&_dbh);

//#ifdef DEBUG 
  _builder->dump_graph_state(); 
//#endif

}

}  // namespace osm
}  // namespace gol	