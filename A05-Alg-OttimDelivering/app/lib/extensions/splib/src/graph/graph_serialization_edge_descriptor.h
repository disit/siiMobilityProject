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

#ifndef GRAPH_MULTI_ARRAY_SERIALIZATION_H_
#define GRAPH_MULTI_ARRAY_SERIALIZATION_H_
#include <boost/multi_array.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost{ 
  namespace serialization{

template<class Archive,class T>
inline void load(
    Archive & ar,
    boost::multi_array<T,2> & t,
    const unsigned int file_version)
{
  typedef boost::multi_array<T,2> multi_array_;
  typedef typename multi_array_::size_type size_;
        
  size_ n0;
  ar >> BOOST_SERIALIZATION_NVP(n0);
  size_ n1;
  ar >> BOOST_SERIALIZATION_NVP(n1);
            
  t.resize(boost::extents[n0][n1]);
  ar >> make_array(t.data(), t.num_elements());
}

template<typename Archive,typename T>
inline void save(
    Archive & ar,
    const boost::multi_array<T,2> & t,
    const unsigned int file_version)
{
  typedef boost::multi_array<T,2> multi_array_;
  typedef typename multi_array_::size_type size_;
    
  size_ n0 = (t.shape()[0]);
  ar << BOOST_SERIALIZATION_NVP(n0);
  size_ n1 = (t.shape()[1]);
  ar << BOOST_SERIALIZATION_NVP(n1);
  ar << boost::serialization::make_array(t.data(), t.num_elements());
} 

template<class Archive,typename T>
inline void serialize(
    Archive & ar,
    boost::multi_array<T,2>& t,
    const unsigned int file_version)
{
  split_free(ar, t, file_version);
}       

} // serialization
} // boost

#endif // GRAPH_MULTI_ARRAY_SERIALIZATION_H_



