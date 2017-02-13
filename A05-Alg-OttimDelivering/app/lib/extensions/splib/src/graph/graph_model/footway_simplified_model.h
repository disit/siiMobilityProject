#ifndef GOL_GRAPH_FOOTWAY_MODEL_H_
#define GOL_GRAPH_FOOTWAY_MODEL_H_

namespace gol {

template <
  typename GraphT, 
  typename VertexMap,
  typename RestrictionMap, 
  typename WeightT>
class footway_simplified_model : 
  public graph_builder<GraphT, 
                       VertexMap, 
                       RestrictionMap,
                       WeightT>  
{ 
  typedef boost::graph_traits<GraphT> Traits;
  typedef typename 
    Traits::vertex_descriptor vertex_descriptor;
 public:
  footway_simplified_model(
    GraphT&         g, 
    VertexMap&      vtxmap,
    RestrictionMap& resmap, 
    std::string     model) : 
      graph_builder<
          GraphT, 
          VertexMap, 
          RestrictionMap, 
          WeightT>
        (g, vtxmap, resmap, model) {}
  
  ~footway_simplified_model() {}

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
  
};

template <
  typename GraphT, 
  typename VertexMap,
  typename RestrictionMap, 
  typename WeightT>
WeightT footway_simplified_model<
    GraphT, 
    VertexMap, 
    RestrictionMap,
    WeightT
>::get_model_edge_weight(
    vertex_descriptor s, 
    vertex_descriptor t, 
    features_map&     fmap) 
{
  double epsilon = 0.01;
  double length = 
    vincenty_distance( (this->_g)[s].geo.lon, (this->_g)[s].geo.lat, 
                       (this->_g)[t].geo.lon, (this->_g)[t].geo.lat ); 
  fmap[feature_Kt::highway_length] = length + epsilon;  
  return make_edge_weight<WeightT>::instance(this->_model, fmap);
}

template <
  typename GraphT, 
  typename VertexMap,
  typename RestrictionMap, 
  typename WeightT>
void footway_simplified_model<
    GraphT, 
    VertexMap, 
    RestrictionMap,
    WeightT
>::add_node(
    std::string   id,
    features_map& fmap, 
    double        lon, 
    double        lat,
    double        ele) 
{
  vertex_descriptor v   = boost::add_vertex((this->_g));
  (this->_g)[v].id      = id; 
  (this->_g)[v].geo.lon = lon; 
  (this->_g)[v].geo.lat = lat;
  (this->_g)[v].geo.ele = ele; 
  (this->_vtxmap)[id]   = v;
}

template <
  typename GraphT, 
  typename VertexMap,
  typename RestrictionMap, 
  typename WeightT>
void footway_simplified_model<
    GraphT, 
    VertexMap, 
    RestrictionMap,
    WeightT
>::add_section(
    std::string   sid, 
    std::string   tid, 
    features_map& fmap) 
{  
  vertex_descriptor s = (this->_vtxmap)[sid];
  vertex_descriptor t = (this->_vtxmap)[tid];

  if ( (fmap.count(feature_Kt::highway_is_motorway) && 
       !(boost::any_cast<bool>(fmap[feature_Kt::highway_is_motorway])))
    ||
       (fmap.count(feature_Kt::highway_is_cycleway) && 
       !(boost::any_cast<bool>(fmap[feature_Kt::highway_is_cycleway])))
    )          
  { 
    typename Traits::edge_descriptor e; bool inserted;

    boost::tie(e, inserted) = boost::add_edge(s, t, (this->_g));
    if (!inserted) {
      logger(logWARNING)
        << left("[builder]", 14) 
        << "Edge not inserted : "
        << left(sid, 12) << " > " << left(tid, 14) 
        << ((s && t) ?  "duplicate" : "out of bounding box"); // if out of box graph idx = 0
    } else {
      (this->_g)[e].weight = 
        this->get_model_edge_weight(s, t, fmap);
      (this->_g)[e].properties.edge_type = 
        boost::any_cast<std::string>(fmap[feature_Kt::highway_value]);
      (this->_g)[e].properties.desc = 
        boost::any_cast<std::string>(fmap[feature_Kt::highway_name]);   
    }    
    
    boost::tie(e, inserted) = boost::add_edge(t, s, (this->_g));
    if (!inserted) {
      logger(logWARNING) 
        << left("[builder]", 14) 
        << "Edge not inserted : "
        << left(tid, 12) << " > " << left(sid, 14) 
        << ((s && t) ?  "duplicate" : "out of bounding box"); // if out of box graph idx = 0
    } else  {
      (this->_g)[e].weight = 
        this->get_model_edge_weight(t, s, fmap);
      (this->_g)[e].properties.edge_type = 
        boost::any_cast<std::string>(fmap[feature_Kt::highway_value]);
      (this->_g)[e].properties.desc = 
        boost::any_cast<std::string>(fmap[feature_Kt::highway_name]);              
    }
    
  }
}

} // namespace gol

#endif // GOL_GRAPH_FOOTWAY_MODEL_H_	
