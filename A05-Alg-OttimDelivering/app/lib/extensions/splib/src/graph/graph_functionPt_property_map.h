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

#ifndef GRAPH_FUNCTION_PROPERTY_MAP_HPP
#define GRAPH_FUNCTION_PROPERTY_MAP_HPP

#include <boost/config.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <utility>

namespace boost {

template<typename Func, 
         typename Key, 
         typename Ret = typename boost::result_of<const Func(const Key&)>::type>
class functionPt_property_map: 
public put_get_helper<Ret, functionPt_property_map<Func, Key, Ret> > 
{
 public:
  typedef Key key_type;
  typedef Ret reference;
  typedef typename boost::remove_cv<
    typename boost::remove_reference<Ret>::type
      >::type value_type;

  typedef typename boost::mpl::if_<
      boost::mpl::and_<
        boost::is_reference<Ret>,
        boost::mpl::not_<boost::is_const<Ret> > >,
      boost::lvalue_property_map_tag,
      boost::readable_property_map_tag
    >::type category;

  functionPt_property_map(Func* fPt = &Func()) : fPt(fPt) {}

  reference operator[](const Key& k) const {
    return (*fPt)(k);
  }

 private:
  Func* fPt;

};

template<typename Key, typename Func>
functionPt_property_map<Func, Key>
make_functionPt_property_map(const Func* fPt) {
  return functionPt_property_map<Func, Key>(fPt);
}

template<typename Key, typename Ret, typename Func>
functionPt_property_map<Func, Key, Ret>
make_functionPt_property_map(const Func* fPt) {
  return functionPt_property_map<Func, Key, Ret>(fPt);
}

} // boost

#endif /* GRAPH_FUNCTION_PROPERTY_MAP_HPP */
