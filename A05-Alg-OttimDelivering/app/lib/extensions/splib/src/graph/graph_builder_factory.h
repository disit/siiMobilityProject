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

#ifndef GOL_GRAPH_BUILDER_FACTORY_H_
#define GOL_GRAPH_BUILDER_FACTORY_H_

// std
#include <string>

#include "../common.h"
#include "graph_builder.h"

namespace gol {	

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
class graph_builder_factory {
 public:
  static 
  graph_builder<
      GraphT, 
      VertexMap,
      EdgeMap,
      Constraints, 
      WeightT>*
    get_builder_for(
        std::string     model, 
        GraphT&         g, 
        VertexMap&      vtxmap,
        EdgeMap&        edgmap,
        Constraints&    constr) 
    {
      if (model == "pedestrian_simplified_model") 
        return new pedestrian_simplified_model<
                GraphT, 
                VertexMap,
                EdgeMap,
                Constraints, 
                WeightT>(g, vtxmap, edgmap, constr, model);
      if (model == "road_simplified_model") 
        return new road_simplified_model<
                GraphT, 
                VertexMap,
                EdgeMap,
                Constraints, 
                WeightT>(g, vtxmap, edgmap, constr, model);      
      if (model == "bicriterion_bicycle_model") 
        return new bicriterion_bicycle_model<
                GraphT, 
                VertexMap,
                EdgeMap,
                Constraints, 
                WeightT>(g, vtxmap, edgmap, constr, model);
      if (model == "road_compact_representation_model") 
        return new road_compact_representation_model<
                GraphT, 
                VertexMap,
                EdgeMap,
                Constraints,  
                WeightT>(g, vtxmap, edgmap, constr, model);      
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

