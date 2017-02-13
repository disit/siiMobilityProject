
#include "pair_edge_weight.h"

namespace gol {

std::pair<int, int> operator*(const std::pair<int, int>& a, double c) 
{
  return std::make_pair( (int)(a.first * c), (int)(a.second * c) );
}


} // namespace gol