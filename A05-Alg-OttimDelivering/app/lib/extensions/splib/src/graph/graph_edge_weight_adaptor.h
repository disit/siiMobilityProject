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

#ifndef GOL_GRAPH_EDGE_ADAPTOR_H_
#define GOL_GRAPH_EDGE_ADAPTOR_H_

//#include "../exception.h"

namespace gol {

template <typename WeightT>
struct edge_weight_adaptor
{
  static 
  double to_length(WeightT w) {
    throw runtime_exception(
      "edge_weight_adaptor::to_length() "
      "not defined for current weight type");
  }
};

template <>
struct edge_weight_adaptor<double>
{
  static 
  double to_length(double w) {
    return w;
  }   
};

template <>
struct edge_weight_adaptor<int>
{
  static 
  double to_length(int w) {
    return (double) w;
  }  
};

template <>
struct edge_weight_adaptor<std::pair<int, int> >
{
  static 
  double to_length(std::pair<int, int> w) {
    return (double) (w.first);
  } 
};

template <>
struct edge_weight_adaptor<std::pair<double, double> >
{
  static 
  double to_length(std::pair<double, double> w) {
    return w.first; 
  }     
};

} // namespace gol

#endif // GOL_GRAPH_EDGE_ADAPTOR_H_	