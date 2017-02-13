#ifndef GOL_GRAPH_TUPLE_EDGE_WEIGHT_H_
#define GOL_GRAPH_TUPLE_EDGE_WEIGHT_H_

namespace gol {

// Some Standard Arithmetic Operators 

template <std::size_t index, typename... WeightsT>
typename std::tuple_element< index, std::tuple<WeightsT...> >::type 
    weight_element_sum(const std::tuple<WeightsT...>&a, const std::tuple<WeightsT...>&b)
{
  return std::get<index>(a) + std::get<index>(b);
}

template <size_t... index, typename... WeightsT>
std::tuple<WeightsT...> weight_sum(const std::tuple<WeightsT...>&a, 
	const std::tuple<WeightsT...>&b, std::index_sequence<index...>)
{
  return std::make_tuple(weight_element_sum<index>(a,b)...);
}

template <typename... WeightsT>
std::tuple<WeightsT...> operator+(const std::tuple<WeightsT...>& a, const std::tuple<WeightsT...>& b) 
{
  return weight_sum(a, b, std::index_sequence_for<WeightsT...>{}); 
} 

template <std::size_t index, typename... WeightsT>
typename std::tuple_element< index, std::tuple<WeightsT...> >::type 
    weight_element_sub(const std::tuple<WeightsT...>&a, const std::tuple<WeightsT...>&b)
{
  return std::get<index>(a) - std::get<index>(b);
}

template <size_t... index, typename... WeightsT>
std::tuple<WeightsT...> weight_sub(const std::tuple<WeightsT...>&a, 
	const std::tuple<WeightsT...>&b, std::index_sequence<index...>)
{
  return std::make_tuple(weight_element_sub<index>(a,b)...);
}

template <typename... WeightsT>
std::tuple<WeightsT...> operator-(const std::tuple<WeightsT...>& a, const std::tuple<WeightsT...>& b) 
{
  return weight_sub(a, b, std::index_sequence_for<WeightsT...>{}); 
} 

template <std::size_t index, typename... WeightsT>
typename std::tuple_element< index, std::tuple<WeightsT...> >::type 
    weight_element_prd(const std::tuple<WeightsT...>&a, const std::tuple<WeightsT...>&b)
{
  return std::get<index>(a) * std::get<index>(b);
}

template <size_t... index, typename... WeightsT>
std::tuple<WeightsT...> weight_prd(const std::tuple<WeightsT...>&a, 
	const std::tuple<WeightsT...>&b, std::index_sequence<index...>)
{
  return std::make_tuple(weight_element_prd<index>(a,b)...);
}

template <typename... WeightsT>
std::tuple<WeightsT...> operator*(const std::tuple<WeightsT...>& a, const std::tuple<WeightsT...>& b) 
{
  return weight_prd(a, b, std::index_sequence_for<WeightsT...>{}); 
} 

// Pareto Dominance

// this class performs the comparison operations on weights ( tuples )
template<typename WeightT, typename WeightU, size_t index, size_t size>
struct element_wise_weight_compare 
{
  static constexpr bool
  less_than_equal_to(const WeightT& t, const WeightU& u)
  {
    return bool(!(std::get<index>(u) < std::get<index>(t))
       && element_wise_weight_compare<WeightT, WeightU, index + 1, size>::less_than_equal_to(t, u));
  }
};

// for empty tuple return false
template<typename WeightT, typename WeightU, size_t size>
struct element_wise_weight_compare<WeightT, WeightU, size, size>
{   
      static constexpr bool
      less_than_equal_to(const WeightT&, const WeightU&) {return false;}
};

// WARNING: is there a performance problem?
template <typename... WeightsT> 
struct pareto_dominance
{
  bool operator()(const std::tuple<WeightsT...>& a, const std::tuple<WeightsT...>& b) 
  {
    using element_wise_compare = element_wise_weight_compare<
        const std::tuple<WeightsT...>&, const std::tuple<WeightsT...>&, 0, sizeof...(WeightsT)>;   	
    return ((element_wise_compare::less_than_equal_to(a, b)) && (a != b)); // std::tuple operator<= uses lexographically compare  
  } 
};

// MultiLabel Sets


// Weight Dump

template<typename Ch, typename Tr, typename Tuple, std::size_t... index>
void weight_dump(std::basic_ostream<Ch, Tr>& os, Tuple const& t, std::index_sequence<index...>)
{
  using stream = int[]; // guaranties left to right order
  (void)stream{0, (void(os << (index == 0? "" :  ", ") << std::get<index>(t)), 0)...};
}
 
template<typename Ch, typename Tr, typename... WeightsT>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<WeightsT...> const& t)
    -> std::basic_ostream<Ch, Tr>&
{
  os << "(";
  weight_dump(os, t, std::index_sequence_for<WeightsT...>{});
  return os << ")";
}

} // namespace gol

#endif // GOL_GRAPH_TUPLE_EDGE_WEIGHT_H_