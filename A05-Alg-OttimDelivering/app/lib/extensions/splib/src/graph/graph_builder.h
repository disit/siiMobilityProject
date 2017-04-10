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

#ifndef GOL_GRAPH_BUILDER_H_
#define GOL_GRAPH_BUILDER_H_

// bosot
//#include <boost/any.hpp>

#include "graph_model_edge_weight.h"
//#include "../data_extraction/OSM.h"
#include "../data_extraction/sqlite/sqlite_database_helper.h"

namespace gol {  

template <
  typename GraphT, 
  typename VertexMap,
  typename EdgeMap,  
  typename Constraints,
  typename WeightT>
class graph_builder 
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;
  typedef typename Traits::out_edge_iterator out_edge_iterator;
  typedef typename Traits::in_edge_iterator  in_edge_iterator;
  typedef typename Traits::vertex_iterator   vertex_iterator;

 protected:
  GraphT&         _g;
  VertexMap&      _vtxmap;
  EdgeMap&        _edgmap;  
  Constraints&    _gconstraints;
  std::string     _model;

  uint32_t        _edge_index = 0; 

 public:
  graph_builder(
    GraphT&         g, 
    VertexMap&      vtxmap,
    EdgeMap&        edgmap,      
    Constraints&    constraints,     
    std::string     model): 
      _g(g), 
      _vtxmap(vtxmap),
      _edgmap(edgmap),
      _gconstraints(constraints),
      _model(model) {}
  ~graph_builder() {}

  virtual void add_node(
    std::string   id,
    features_map& fmap, 
    double        lon, 
    double        lat,
    double        ele) = 0;
  
  virtual void add_section(
    std::string   sid, 
    std::string   tid, 
    features_map& fmap) = 0;

  virtual WeightT get_model_edge_weight(
    vertex_descriptor s, 
    vertex_descriptor t, 
    features_map&     fmap) = 0;

  virtual void build_graph(
    sqlite_database_helper_t* dbh) = 0; 

  std::string get_model() {
    return _model;
  }

  bool is_vertex(std::string id) {
    if (_vtxmap.find(id) != _vtxmap.end())
      return true;
    return false;
  }       

  void create_network_junctions(
      std::vector<osm::node>& nds) 
  {
    for (auto n : nds) {
      features_map fmap(n.tags);
      this->add_node(std::to_string(n.id), fmap, n.lon, n.lat, n.ele);
    }
  }

  void create_network_segments(
      std::vector<osm::way>& wys) 
  {
    for (auto w : wys) {
      features_map fmap(w.tags);
      auto it = w.refs.begin(); 
      while (it != w.refs.end()) {
        long long int sid = (*it); // source
        it++;                      // target
        if (it != w.refs.end())        
          this->add_section(std::to_string(sid), std::to_string((*it)), fmap);
      }
    } // end ways

  }

  // currently only restrictions where the via objects is a node are supported.
  void add_turn_costs(
    std::vector<osm::node_as_via_turn_restriction>& trs)
  {
    vertex_iterator ui, ui_end;
    for (boost::tie(ui, ui_end) = boost::vertices(_g); ui != ui_end; ++ui) 
    {
      vertex_descriptor u = *ui;
      std::vector<edge_descriptor> 
        incoming_edges, outgoing_edges; 
      
      in_edge_iterator iei, iei_end;
      for (tie(iei, iei_end) = boost::in_edges(u, _g); 
             iei != iei_end; ++iei) 
        incoming_edges.push_back(*iei);       
      
      out_edge_iterator oei, oei_end;
      for (tie(oei, oei_end) = boost::out_edges(u, _g); 
             oei != oei_end; ++oei) 
        outgoing_edges.push_back(*oei);
        
      _gconstraints.create_turn_table(u, incoming_edges, outgoing_edges);    
    }

    in_edge_iterator  iei, iei_end;
    out_edge_iterator oei, oei_end;
    for (auto restriction : trs)
    {      
      auto vit = _vtxmap.find(std::to_string(restriction.via)); 
      if (vit == _vtxmap.end()) 
        continue;

      vertex_descriptor via = (*vit).second;
      for (tie(iei, iei_end) = boost::in_edges(via, _g); iei != iei_end; ++iei)
        for (tie(oei, oei_end) = boost::out_edges(via, _g); oei != oei_end; ++oei)  
          if ( restriction.
                 is_restricted_maneuver(_g[boost::source(*iei, _g)].id, _g[boost::target(*oei, _g)].id) )  
          {    
            (*(_g[via].turn_table)) [_g[*iei].entry_point][_g[*oei].exit_point] = 
                std::numeric_limits<WeightT>::max();        
          }       
    
    }
    //_gconstraints.compact_turn_tables();


/// dump turn restriciton tables
/*     for (auto restriction : trs)
    { 
      std::vector< std::vector<std::string> > tab;     
      
      auto vit = _vtxmap.find(std::to_string(restriction.via)); 
      if (vit == _vtxmap.end()) 
        continue;

      vertex_descriptor via = (*vit).second;
      std::cout << "Turn Restriction Via > " << _g[via].id << std::endl;
      
      std::vector<std::string> row; row.push_back("FromVtx/ToVtx"); 
      for (tie(oei, oei_end) = boost::out_edges(via, _g); oei != oei_end; ++oei) 
         row.push_back( _g[boost::target(*oei, _g)].id );
      
      tab.push_back(row);
      row.clear(); 
      for (tie(iei, iei_end) = boost::in_edges(via, _g); iei != iei_end; ++iei) 
      {
        row.push_back(  _g[boost::source(*iei, _g)].id );       
        for (tie(oei, oei_end) = boost::out_edges(via, _g); oei != oei_end; ++oei)  
          row.push_back( std::to_string(restriction.is_restricted_maneuver(
                           _g[boost::source(*iei, _g)].id, _g[boost::target(*oei, _g)].id)) );
        tab.push_back(row);
        row.clear();                          
      }
      for (auto r : tab) {
        for (auto rc : r) 
          std::cout << left(rc, 16);
        std::cout << std::endl;  
      } 
    }  //*/ 


  }

  void dump_graph_state()
  {
    logger(logINFO)
      << left("[builder]", 14)
      << "Graph > " 
      << "|V| = "
      << boost::num_vertices(_g)
      << ", "
      << "|E| = "
      << boost::num_edges(_g);
  }

 private:
  graph_builder(const graph_builder&);
  graph_builder& operator=(const graph_builder&);
};

} // namespace gol

#include "graph_model/pedestrian_simplified_model.h"
#include "graph_model/road_simplified_model.h"
#include "graph_model/bicriterion_bicycle_model.h"
#include "graph_model/road_compact_representation_model.h"

#endif // GOL_GRAPH_BUILDER_H_