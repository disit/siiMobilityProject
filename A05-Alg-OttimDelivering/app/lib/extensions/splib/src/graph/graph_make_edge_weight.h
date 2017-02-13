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
    double length;
    try 
    {
      length = fmap.count(feature_Kt::highway_length) ?
      boost::any_cast<double>(fmap[feature_Kt::highway_length]) : 
      std::numeric_limits<double>::max();
    } catch (std::exception& e) {
       throw builder_exception(e.what());
    }

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
    double length;
    try 
    {
      length = fmap.count(feature_Kt::highway_length) ?
        std::ceil(boost::any_cast<double>(fmap[feature_Kt::highway_length])): // round up
        std::numeric_limits<int>::max();
    } catch (std::exception& e) {
       throw builder_exception(e.what());
    }

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
    try 
    {
      length = 
        fmap.count(feature_Kt::highway_length) ?
        std::ceil(boost::any_cast<double>(fmap[feature_Kt::highway_length])):  // round up
        std::numeric_limits<int>::max();
     
      if (fmap.count(feature_Kt::highway_is_cycleway))
        cyclable_length = 
          boost::any_cast<bool>(fmap[feature_Kt::highway_is_cycleway]) ? 
          0 :
          length;
      else
        cyclable_length = length;    
      
    } catch (std::exception& e) {
      throw builder_exception(e.what());
    }

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
    try 
    {
      length = 
        fmap.count(feature_Kt::highway_length) ?
        boost::any_cast<double>(fmap[feature_Kt::highway_length]) : 
        std::numeric_limits<double>::max();

      if (fmap.count(feature_Kt::highway_is_cycleway))
        cyclable_length = 
          boost::any_cast<bool>(fmap[feature_Kt::highway_is_cycleway]) ? 
          0 :
          length;
      else
        cyclable_length = length;       
      
    } catch (std::exception& e) {
       throw builder_exception(e.what());
    }

    if (length == 0)
      logger(logWARNING) 
        << left("[builder] ", 14) 
        << "Edge weight zero ";  
            
    return std::make_pair(length, cyclable_length);  
  }
};


} // namespace gol

#include "graph_edge_adaptor.h"

#endif // GOL_GRAPH_MAKE_EDGE_WEIGHT_H_	