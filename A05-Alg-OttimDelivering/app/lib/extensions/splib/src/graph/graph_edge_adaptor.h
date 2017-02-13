#ifndef GOL_GRAPH_EDGE_ADAPTER_H_
#define GOL_GRAPH_EDGE_ADAPTER_H_

namespace gol {

template <typename WeightT>
struct edge_weight_adaptor
{
  static 
  double to_length(WeightT w)
  {
    return 1; // TODO
  }
};

template <>
struct edge_weight_adaptor<double>
{
  static 
  double to_length(double w)
  {
    return w;
  }   
};

template <>
struct edge_weight_adaptor<int>
{
  static 
  double to_length(int w)
  {
    return (double) w;
  }  
};

template <>
struct edge_weight_adaptor<std::pair<int, int> >
{
  static 
  double to_length(std::pair<int, int> w)
  {
    return (double) (w.first);
  } 
};

template <>
struct edge_weight_adaptor<std::pair<double, double> >
{
  static 
  double to_length(std::pair<double, double> w)
  {
    return w.first; 
  }     
};

} // namespace gol

#endif // GOL_GRAPH_EDGE_ADAPTER_H_	