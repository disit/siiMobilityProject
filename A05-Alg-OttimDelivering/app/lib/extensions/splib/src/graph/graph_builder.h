#ifndef GOL_GRAPH_BUILDER_H_
#define GOL_GRAPH_BUILDER_H_

// bosot
//#include <boost/any.hpp>
//#include "../exception.h"

#include "../data_extraction/map_features.h"
#include "graph_make_edge_weight.h"

namespace gol {

/**
*
*/
template <
  typename GraphT, 
  typename VertexMap,
  typename RestrictionMap, 
  typename WeightT>
class graph_builder 
{
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename 
    Traits::vertex_descriptor vertex_descriptor;
 protected:
  GraphT&         _g;
  VertexMap&      _vtxmap;
  RestrictionMap& _resmap;
  std::string     _model; 

 public:
  graph_builder(
    GraphT&         g, 
    VertexMap&      vtxmap, 
    RestrictionMap& resmap,
    std::string     model): 
      _g(g), 
      _vtxmap(vtxmap),
      _resmap(resmap), 
      _model(model) {}
  ~graph_builder() {}

  virtual void add_node(
    std::string   id,
    features_map& fmap, 
    double        lon, 
    double        lat,
    double        ele = MARIANA_TRENCH_DEPTH) = 0;
  
  virtual void add_section(
    std::string   sid, 
    std::string   tid, 
    features_map& fmap) = 0;

  virtual WeightT get_model_edge_weight(
    vertex_descriptor s, 
    vertex_descriptor t, 
    features_map&     fmap) = 0;

  std::string get_model() {return _model;}

  bool is_vertex(std::string id) {
    if (_vtxmap.find(id) != _vtxmap.end())
      return true;
    return false;
  }

  void add_restriction(
    std::string              via,
    std::vector<std::string> from,
    std::vector<std::string> to)
  {    
    vertex_descriptor           v;    
    std::set<vertex_descriptor> vfrom, vto;

    if (_vtxmap.find(via) == _vtxmap.end())
      return; //throw data_exception();
    
    v = _vtxmap[via];
    _g[v].properties.turn_restriction = true;

    for (auto id : from)
      vfrom.insert(_vtxmap[id]);
    for (auto id : to)
      vto.insert(_vtxmap[id]);

    if (_resmap.find(v) == _resmap.end()) 
    {      
      std::vector<
        std::pair<
          std::set<vertex_descriptor>,            
          std::set<vertex_descriptor>            
      > > turn_res = {std::make_pair(vfrom, vto)};
      _resmap.insert(std::make_pair( v, turn_res));
    }  
    else
      _resmap[v].push_back(
        std::make_pair(vfrom, vto));                
  
  }

  void dump_graph_state()
  {
    logger(logDEBUG)
      << left("[builder]", 14)
      << "Graph state: " 
      << "|V| = "
      << boost::num_vertices(_g)
      << " > "
      << "|E| = "
      << boost::num_edges(_g);
  }

 private:
  graph_builder(const graph_builder&);
  graph_builder& operator=(const graph_builder&);
};

} // namespace gol

#include "graph_model/footway_simplified_model.h"
#include "graph_model/car_simplified_model.h"
#include "graph_model/bicriteria_cycleway_model.h"

#endif // GOL_GRAPH_BUILDER_H_