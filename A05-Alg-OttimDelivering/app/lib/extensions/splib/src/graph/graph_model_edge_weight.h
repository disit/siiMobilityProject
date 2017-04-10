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

#ifndef GOL_GRAPH_MAKE_EDGE_WEIGHT_H_
#define GOL_GRAPH_MAKE_EDGE_WEIGHT_H_

namespace gol {

template <typename WeightT>
struct make_edge_weight
{
  static 
  WeightT instance(std::string model, features_map fmap) {
    return WeightT();
  }
};

template <>
struct make_edge_weight<double>
{
  static 
  double instance(std::string model, features_map fmap)
  {
    double length = fmap.get_length();
    
    if (length == 0)
      logger(logWARNING) 
        << left("[builder] ", 14) 
        << "Edge weight zero ";  

    return length;
  }

};

template <>
struct make_edge_weight<int>
{
  static 
  int instance(std::string model, features_map fmap)
  {
    int length = std::ceil(fmap.get_length()); // round up

    if (length == 0)
      logger(logWARNING) 
        << left("[builder] ", 14) 
        << "Edge weight zero ";  

    return length;  
  }
};

template <>
struct make_edge_weight<std::pair<int, int> >
{
  static 
  std::pair<int, int> instance(std::string model, features_map fmap)
  {
    int length, cyclable_length;

    length = std::ceil(fmap.get_length()); // round up   
    cyclable_length = fmap.is_safe_cyclable_highway() ? 0 : length; 

    if (length == 0)
      logger(logWARNING) 
        << left("[builder] ", 14) 
        << "Edge weight zero ";     

    return std::make_pair(length, cyclable_length);  
  }
};

template <>
struct make_edge_weight<std::pair<double, double> >
{
  static 
  std::pair<double, double> instance(std::string model, features_map fmap)
 {
    double length, cyclable_length;

    length = fmap.get_length();    
    cyclable_length = fmap.is_safe_cyclable_highway() ? 0 : length; 

    if (length == 0)
      logger(logWARNING) 
        << left("[builder] ", 14) 
        << "Edge weight zero ";  
            
    return std::make_pair(length, cyclable_length);  
  }
};


} // namespace gol

#include "graph_edge_weight_adaptor.h"

#endif // GOL_GRAPH_MAKE_EDGE_WEIGHT_H_	