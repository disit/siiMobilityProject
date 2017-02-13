
#ifndef GOL_GRAPH_BUILDER_FACTORY_H_
#define GOL_GRAPH_BUILDER_FACTORY_H_

// std
#include <string>

#include "../common.h"
#include "graph_builder.h"

namespace gol {	

/**
*
*/
template <
  typename GraphT, 
  typename VertexMap,
  typename RestrictionMap, 
  typename WeightT>
class graph_builder_factory {
 public:
  static 
  graph_builder<
      GraphT, 
      VertexMap, 
      RestrictionMap,
      WeightT>*
    get_builder_for(
        std::string     model, 
        GraphT&         g, 
        VertexMap&      vtxmap,
        RestrictionMap& resmap) 
    {
      if (model == "footway_simplified_model") 
        return new footway_simplified_model<
                GraphT, 
                VertexMap, 
                RestrictionMap,
                WeightT>(g, vtxmap, resmap, model);
      if (model == "car_simplified_model") 
        return new car_simplified_model<
                GraphT, 
                VertexMap, 
                RestrictionMap,
                WeightT>(g, vtxmap, resmap, model);      
      if (model == "bicriteria_cycleway_model") 
        return new bicriteria_cycleway_model<
                GraphT, 
                VertexMap, 
                RestrictionMap,
                WeightT>(g, vtxmap, resmap, model);
      throw solver_exception();
    }

 private:
  // always declare assignment operator and default and copy constructor
  graph_builder_factory();
  ~graph_builder_factory();  
  graph_builder_factory(const graph_builder_factory&);
  graph_builder_factory& operator=(const graph_builder_factory&);
      
};

} // namespace gol


#endif // GOL_GRAPH_BUILDER_FACTORY_H_

