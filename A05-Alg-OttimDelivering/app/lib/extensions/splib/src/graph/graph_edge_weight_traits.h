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

#ifndef GOL_GRAPH_WEIGHT_TRAITS_FACTORY_H_
#define GOL_GRAPH_WEIGHT_TRAITS_FACTORY_H_

// std
#include <type_traits>
//boost
#include <boost/graph/adjacency_list.hpp>

namespace gol {

template <typename... WeightsT>
struct graph_weight_traits
{
  typedef std::tuple<WeightsT... > edge_weight_t;
  typedef boost::bidirectionalS boost_graph_t;
};

template <typename WeightFT, typename WeightST>
struct graph_weight_traits<WeightFT, WeightST>
{
  typedef std::pair<WeightFT, WeightST> edge_weight_t;
  typedef boost::bidirectionalS boost_graph_t;
};

template <typename WeightT>
struct graph_weight_traits<WeightT>
{
  typedef WeightT edge_weight_t;
  //typedef boost::directedS boost_graph_t;
  typedef boost::bidirectionalS boost_graph_t;
};
	

template<typename> 
struct is_pair_edge_weight: 
  public std::false_type {};

template<typename T, typename K> 
struct is_pair_edge_weight<std::pair<T, K> > :
  public  std::true_type {};

template<typename> 
struct is_tuple_edge_weight: 
  public std::false_type {};

template<typename... T> 
struct is_tuple_edge_weight<std::tuple<T...> >: 
  public std::true_type {};

template<typename T>
struct remove_pointer
{
    typedef T type;
};

template<typename T>
struct remove_pointer<T*>
{
    typedef typename remove_pointer<T>::type type;
};  	

}  // namespace gol

#include "graph_edge_weight/single_edge_weight.h"
#include "graph_edge_weight/pair_edge_weight.h"
#include "graph_edge_weight/tuple_edge_weight.h"

#endif // GOL_GRAPH_WEIGHT_TRAITS_FACTORY_H_