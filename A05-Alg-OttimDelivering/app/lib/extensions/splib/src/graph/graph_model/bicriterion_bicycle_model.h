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

#ifndef GOL_GRAPH_BI_CYCLEWAY_MODEL_H_
#define GOL_GRAPH_BI_CYCLEWAY_MODEL_H_

namespace gol {

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
class bicriterion_bicycle_model : 
  public graph_builder<GraphT, 
                       VertexMap,
                       EdgeMap,
                       Constraints, 
                       WeightT> 
{
  typedef graph_builder<
    GraphT, VertexMap, EdgeMap, Constraints, WeightT> Base; 
  typedef boost::graph_traits<GraphT>                 Traits;
  typedef typename Traits::vertex_descriptor          vertex_descriptor;
 public:
  bicriterion_bicycle_model(
    GraphT&         g, 
    VertexMap&      vtxmap,
    EdgeMap&        edgmap,
    Constraints&    constr,
    std::string     model) : 
      graph_builder<
          GraphT, 
          VertexMap,
          EdgeMap,
          Constraints, 
          WeightT>
        (g, vtxmap, edgmap, constr, model) {}

  ~bicriterion_bicycle_model() {}

  virtual void add_node(
    std::string   id,
    features_map& fmap, 
    double        lon, 
    double        lat,
    double        ele);
  
  virtual void add_section(
    std::string   sid, 
    std::string   tid, 
    features_map& fmap);

  virtual WeightT get_model_edge_weight(
    vertex_descriptor s, 
    vertex_descriptor t, 
    features_map&     fmap);

  virtual void build_graph(
    sqlite_database_helper_t* dbh);  

};

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
WeightT bicriterion_bicycle_model<
    GraphT, 
    VertexMap,
    EdgeMap,
    Constraints, 
    WeightT
>::get_model_edge_weight(
    vertex_descriptor s, 
    vertex_descriptor t, 
    features_map&     fmap) 
{
  double epsilon = 0.01;
  double length = 
    vincenty_distance( (Base::_g)[s].geo.lon, (Base::_g)[s].geo.lat, 
                       (Base::_g)[t].geo.lon, (Base::_g)[t].geo.lat );  
  fmap.set_length(length + epsilon);      
  return make_edge_weight<WeightT>::instance(Base::_model, fmap);
}	

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
void bicriterion_bicycle_model<
    GraphT, 
    VertexMap,
    EdgeMap,
    Constraints, 
    WeightT
>::add_node(
    std::string   id,
    features_map& fmap, 
    double        lon, 
    double        lat,
    double        ele) 
{
  vertex_descriptor v   = boost::add_vertex((Base::_g));
  (Base::_g)[v].id      = id; 
  (Base::_g)[v].geo.lon = lon; 
  (Base::_g)[v].geo.lat = lat;
  (Base::_g)[v].geo.ele = ele;  
  (Base::_vtxmap)[id]   = v;
}

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
void bicriterion_bicycle_model<
    GraphT, 
    VertexMap,
    EdgeMap,
    Constraints, 
    WeightT
>::add_section(
    std::string   sid, 
    std::string   tid, 
    features_map& fmap)  
{
  vertex_descriptor s = (Base::_vtxmap)[sid];
  vertex_descriptor t = (Base::_vtxmap)[tid];

  if ( fmap.is_cyclable_highway() )          
  { 
    typename Traits::edge_descriptor e; bool inserted;

    boost::tie(e, inserted) = boost::add_edge(s, t, (Base::_g));
    if (!inserted) {      
      /*logger(logWARNING) 
        << left("[builder]", 14) 
        << "Edge not inserted : "
        << left(sid, 12) << " > " << left(tid, 14)
        << ((s && t) ?  "duplicate" : "out of bounding box");*/ 
    } else { 
      (Base::_g)[e].weight = 
        this->get_model_edge_weight(s, t, fmap);

      (Base::_g)[e].properties.highway_value = 
        fmap.get_highway_value();
      (Base::_g)[e].properties.desc = 
        fmap.get_highway_name();
    }           
    
    // TODO: we should consider details about forward or backward oneway 
    //if ( fmap.is_oneway_cycleway() ) 
    //{  
      boost::tie(e, inserted) = boost::add_edge(t, s, (Base::_g));
      if (!inserted) {
        /*logger(logWARNING) 
          << left("[builder]", 14) 
          << "Edge not inserted : "
          << left(tid, 12) << " > " << left(sid, 14) 
          << ((s && t) ?  "duplicate" : "out of bounding box");*/ 
      } else {
      (Base::_g)[e].weight = 
        this->get_model_edge_weight(t, s, fmap);

      (Base::_g)[e].properties.highway_value = 
        fmap.get_highway_value();
      (Base::_g)[e].properties.desc = 
        fmap.get_highway_name();
      }             
    //}

  }  
}

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
void bicriterion_bicycle_model<
    GraphT, 
    VertexMap,
    EdgeMap,
    Constraints, 
    WeightT
>::build_graph(sqlite_database_helper_t* dbh) 
{
  try 
  {
    std::vector<osm::node> nds = {};
    dbh->retrieve(nds);
    Base::create_network_junctions(nds);  

    std::vector<osm::way> wys = {}; 
    dbh->retrieve(wys);
    Base::create_network_segments(wys);
  }
  catch(std::exception& e) {
    throw builder_exception(e.what());
  }

}	

} // namespace gol

#endif // GOL_GRAPH_BI_CYCLEWAY_MODEL_H_