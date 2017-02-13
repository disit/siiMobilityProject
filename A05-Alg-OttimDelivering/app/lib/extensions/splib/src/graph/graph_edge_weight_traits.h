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
  typedef boost::directedS boost_graph_t;
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

}  // namespace gol

#include "graph_edge_weight/single_edge_weight.h"
#include "graph_edge_weight/pair_edge_weight.h"
#include "graph_edge_weight/tuple_edge_weight.h"

#endif // GOL_GRAPH_WEIGHT_TRAITS_FACTORY_H_