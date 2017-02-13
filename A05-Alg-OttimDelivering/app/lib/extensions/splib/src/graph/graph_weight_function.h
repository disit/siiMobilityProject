#ifndef GOL_GRAPH_WEIGHT_FUNCTION_H_
#define GOL_GRAPH_WEIGHT_FUNCTION_H_

//std
#include <functional>

#include "graph_edge_adaptor.h"

namespace gol {

template<typename GraphT, typename WeightT>
struct generic_weight_functor: 
public std::unary_function<
    typename boost::graph_traits<GraphT>::edge_descriptor,
    WeightT>
{
  typedef boost::graph_traits<GraphT>      Traits;
  typedef typename Traits::edge_descriptor edge_descriptor;

  explicit
    generic_weight_functor(GraphT& g): _g(g) {}

  virtual 
  WeightT operator()(const edge_descriptor edge) const {
    return _g[edge].weight;
  }

protected:
  GraphT& _g;
};

template<typename GraphT, typename WeightT>
struct priority_footway_functor: 
public generic_weight_functor<GraphT, WeightT>
{
  typedef boost::graph_traits<GraphT>      Traits;
  typedef typename Traits::edge_descriptor edge_descriptor;
 
  explicit
    priority_footway_functor(GraphT& g): 
     generic_weight_functor<GraphT, WeightT>(g) {}

  virtual WeightT operator()(const edge_descriptor edge) const 
  {
    std::string edge_type = 
      (this->_g)[edge].properties.edge_type;

    double priority = 30;
    if (edge_type == "trunk"         || 
        edge_type == "trunk_link" )
      priority = 50; 
    if (edge_type == "primary"       || 
        edge_type == "primary_link" )
      priority = 40;
    if (edge_type == "secondary"     || 
        edge_type == "secondary_link" )
      priority = 35;
    //if (edge_type == "cycleway")
    //  priority = 35;
    if (edge_type == "steps"         ||  // Steps on footways
        edge_type == "track"         ||  // Dirt roads for mostly agricultural or forestry uses
        edge_type == "bridleway"     || 
        edge_type == "path"          || 
        edge_type == "viaduct"       ||
        edge_type == "private"       ||  
        edge_type == "via_ferrata")      // For traversing a mountainside   
      priority = 20;          
    if (edge_type == "residential"   ||
        edge_type == "living_street" ||  // Pedestrians friendly
        edge_type == "residential_link")
      priority = 5;
    if (edge_type == "pedestrian"    ||  // Reserved for pedestrian-only use
        edge_type == "footway"       ||  // Used mainly by pedestrians (also allowed for bicycles)
        edge_type == "corridor")         // Maps hallway inside of a building 
      priority = 1;

    return ((this->_g)[edge].weight) * 
      ( 0.5 + (priority/100) ); // penalty coeff in interval [0.5, 1] 
  }

};

template<typename GraphT, typename WeightT>
struct safe_vertical_cycleway_functor: 
public generic_weight_functor<GraphT, WeightT>
{ 
  typedef boost::graph_traits<GraphT>      Traits;
  typedef typename Traits::edge_descriptor edge_descriptor;
 
  explicit
    safe_vertical_cycleway_functor(GraphT& g): 
     generic_weight_functor<GraphT, WeightT>(g) {}

  // functor works only for template specialization relative to cycleway
  virtual WeightT operator()(const edge_descriptor edge) const {
    return (this->_g)[edge].weight;
  }
};  

template<typename GraphT>
struct safe_vertical_cycleway_functor<GraphT, std::pair<int, int> >: 
public generic_weight_functor<GraphT, std::pair<int, int> >
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename Traits::edge_descriptor edge_descriptor;
 
  explicit
    safe_vertical_cycleway_functor<GraphT, std::pair<int, int> >(GraphT& g): 
     generic_weight_functor<GraphT, std::pair<int, int>>(g) {}

  virtual std::pair<int, int> operator()(const edge_descriptor edge) const 
  {
    double  travel_time;
    double  length           = (this->_g)[edge].weight.first;
    double  cyclable_length  = (this->_g)[edge].weight.second;
    double  vertical_ascent  = 0;
    double  vertical_descent = 0;
    double  desc_grad        = 0;
    double  downhill_speed_multiplier;

    double source_elevation = 
      (this->_g)[boost::source(edge, this->_g)].geo.ele;
    double target_elevation = 
      (this->_g)[boost::target(edge, this->_g)].geo.ele;
    
    if (source_elevation > target_elevation)
      vertical_descent = source_elevation - target_elevation;
    else
      vertical_ascent  = target_elevation - source_elevation;

    if (length != 0)
      desc_grad = vertical_descent / length;

    if (desc_grad > MAX_DESC_GRAD)
      desc_grad = MAX_DESC_GRAD;  

    downhill_speed_multiplier = 
      ( (MAX_DOWNHILL_SPEED_MULTIPLIER - 1) * 
        (desc_grad / MAX_DESC_GRAD)
      ) + 1;
    
    travel_time = std::ceil
    ( (length + PENALTY_COEFF_UPHILL * vertical_ascent) / 
      ((AVERAGE_BICYCLE_SPEED/3.6) * downhill_speed_multiplier) 
    );
    
    std::string edge_type = 
      (this->_g)[edge].properties.edge_type;

    double priority = 40;
    if (edge_type == "trunk"         || 
        edge_type == "trunk_link" )
      priority = 50; 
    if (edge_type == "primary"       || 
        edge_type == "primary_link" )
      priority = 45;
    /*if (edge_type == "secondary"     || 
        edge_type == "secondary_link" )
      priority = 30;
    if (edge_type == "tertiary")
      priority = 30;    
    if (edge_type == "residential")
      priority = 30;
    if (edge_type == "cycleway")
      priority = 20;*/
    if (edge_type == "pedestrian"    ||  // Reserved for pedestrian-only use
        edge_type == "steps"         ||  // Steps on footways
        edge_type == "corridor"      ||  // Maps hallway inside of a building
        edge_type == "trail"         ||  // For cross-country trails
        edge_type == "via_ferrata")      // For traversing a mountainside   
      priority = 45;
    /*if (edge_type == "unclassified"  ||
        edge_type == "footway"       ||  // Used mainly by pedestrians (also allowed for bicycles)
        edge_type == "living_street" ||  // Pedestrians friendly
        edge_type == "track"         ||  // Dirt roads for mostly agricultural or forestry uses
        edge_type == "bridleway"     ||  // Use by horse riders (primarily) and pedestrians
        edge_type == "service"       ||  // Generally for access to a building (not frontage road)
        edge_type == "trail")            // For cross-country trails     
      priority = 35;*/

    return std::make_pair(
      std::ceil(travel_time * ( 0.5 + (priority/100))), // penalty coeff in interval [0.5, 1] 
      cyclable_length ); 
  }
   
};

template<typename GraphT, typename WeightT>
struct turn_restricion_functor: 
public generic_weight_functor<GraphT, WeightT>
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;
  typedef std::map< 
    vertex_descriptor,
    std::vector<
      std::pair<
        std::set<vertex_descriptor>, // from             
        std::set<vertex_descriptor>  // to              
      >
    >  
  > turn_restriction_map; 

  explicit
    turn_restricion_functor(GraphT& g): 
     generic_weight_functor<GraphT, WeightT>(g) {}

  virtual WeightT operator()(const edge_descriptor edge) const {
   return (this->_g)[edge].weight;    
  }

};

template<typename GraphT>
struct turn_restricion_functor<GraphT, double>: 
public generic_weight_functor<GraphT, double>
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;  
  typedef typename Traits::edge_descriptor   edge_descriptor;
  typedef std::map< 
    vertex_descriptor,
    std::vector<
      std::pair<
        std::set<vertex_descriptor>, // from             
        std::set<vertex_descriptor>  // to              
      >
    >  
  > turn_restriction_map; 

  explicit
    turn_restricion_functor(GraphT& g): 
     generic_weight_functor<GraphT, double>(g) {}

  virtual double operator()(const edge_descriptor edge) const 
  {
   
   /*vertex_descriptor v, from, to;
   std::set<vertex_descriptor> from_set, to_set;

   v    = boost::source(edge, this->_g);
   if (v != _pred_map[v])
     from = _pred_map[v];
   to   = boost::target(edge, this->_g);

   tie(from_set, to_set) = _resmap[v];

   penalty = 0
   if ( ( from_set.find(from) !=  from_set.end() ) &&
        ( to_set.find(to) != to_set.end() ) )
      penalty = std::numeric_limits<double>::max();*/

   return (this->_g)[edge].weight; // + penalty;    
  }

 private:
  std::vector<vertex_descriptor>& _pred_map;
  turn_restriction_map&           _resmap;  

};

template <typename GraphT, typename WeightT>
class weight_function_factory 
{
 public: 
  static 
  generic_weight_functor<GraphT, WeightT>*
  get_functor_for(std::string strategy, GraphT& g) 
  {
    if (strategy == "identity_weight_function") 
      return new generic_weight_functor<
                    GraphT, 
                    WeightT>(g);
    if (strategy == "priority_footway_weight_function") 
      return new priority_footway_functor<
                    GraphT, 
                    WeightT>(g);
    if (strategy == "safe_and_vertical_cycleway_weight_function") 
      return new safe_vertical_cycleway_functor<
                    GraphT, 
                    WeightT>(g);
    /*if (strategy == "turn_restricion_weight_function") 
      return new turn_restricion_functor<
                    GraphT, 
                    WeightT>(g);*/
      
    logger(logWARNING) 
      << left("[engine] ", 14) 
      << "Weight Function not find, "
      << "return Identity Weight Function";
    return new generic_weight_functor<GraphT, WeightT>(g);
  }
 
 private:
  // always declare assignment operator and default and copy constructor
  weight_function_factory();
  ~weight_function_factory();  
  weight_function_factory(const weight_function_factory&);
  weight_function_factory& operator=(const weight_function_factory&);
      
};


} // namespace gol


#endif  // GOL_GRAPH_WEIGHT_FUNCTION_H_
