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

#ifndef GOL_IDX_INDIRECT_CMP_H_
#define GOL_IDX_INDIRECT_CMP_H_

#include <functional>
#include <boost/config.hpp>
#include <boost/property_map/property_map.hpp>

namespace gol {

  template <
      class GraphT, 
      class ReadablePropertyMap, 
      class Compare>
  class edge_index_indirect_cmp {
  public:
    typedef typename 
      boost::property_traits<ReadablePropertyMap>::value_type T;
    typedef typename 
      boost::property_traits<ReadablePropertyMap>::key_type K;
    typedef K first_argument_type;
    typedef K second_argument_type;
    typedef T result_type;
    inline edge_index_indirect_cmp(
         GraphT& gph,     
         const ReadablePropertyMap& df, 
         const Compare& c = Compare())
      : g(gph), d(df), cmp(c) { }

    template <class A, class B>
    inline bool 
    operator()(const A& u, const B& v) const {
      T du = get(d, g[u].edge_index), dv = get(d, g[v].edge_index);
      return cmp(du, dv);
    }
  protected:
    GraphT&             g;    
    ReadablePropertyMap d;
    Compare             cmp;
  };

  template <
      class GraphT, 
      class ReadablePropertyMap, 
      class Compare>
  edge_index_indirect_cmp<GraphT, ReadablePropertyMap, Compare>
  make_edge_index_indirect_cmp(const Compare& cmp, ReadablePropertyMap pmap, GraphT& g) {
    edge_index_indirect_cmp<GraphT, ReadablePropertyMap, Compare> p(g, pmap, cmp);
    return p;
  }

} // namespace gol

#endif // GOL_IDX_INDIRECT_CMP_H_