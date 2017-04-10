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

#ifndef GOL_GRAPH_ARCB_SOLVER_H_
#define GOL_GRAPH_ARCB_SOLVER_H_

namespace gol {

/**
*  
*/ 
template <typename GraphT, 
          typename WeightT,
          typename IndexMap,
          typename SPAlgorithm, 
          typename WeightFunctionT, 
          typename StoppingCriteriaT>
class arc_based_gsolver :
  public graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT> 
{
  typedef graph_solver<
    GraphT, 
    WeightT,
    IndexMap, 
    WeightFunctionT, 
    StoppingCriteriaT>                       Base; 
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor; 
  typedef typename Traits::edge_iterator     edge_iterator;  
  
  //typedef uint32_t edge_index_t;
  typedef std::map<edge_index_t, edge_descriptor
    > EdgeMap;  
  
  // a type where we will hold shortest path as lists of edges 
  typedef std::list<edge_descriptor>             path_t;
  typedef std::list<std::pair<WeightT, path_t> > graph_solver_result;

 public:
  arc_based_gsolver( 
    GraphT&           g,
    EdgeMap&          edgmap, 
    vertex_descriptor source, 
    vertex_descriptor target):
        graph_solver<
            GraphT, 
            WeightT,
            IndexMap, 
            WeightFunctionT, 
            StoppingCriteriaT>(g),
        _edgmap(edgmap),    
        _s(source), 
        _t(target), 
      	_pred_map(), 
        _distance_map() {}
  ~arc_based_gsolver() {}
    
  virtual void solve(
      WeightFunctionT   weight_functor,
      IndexMap          edge_index_map, 
      StoppingCriteriaT stopping_criteria) override 
  {   
    _pred_map.resize(boost::num_edges(Base::_g));
    _distance_map.resize(boost::num_edges(Base::_g));    

    typedef typename remove_pointer<WeightFunctionT>::type FunctionT;
    boost::functionPt_property_map<
        FunctionT, 
        edge_descriptor, 
        WeightT> 
      weight_function(weight_functor);
    stopping_criteria.stats_initialization( &(Base::_stats) );          
    try 
    {
      SPAlgorithm::compute(
        Base::_g, _s, _t, // h, 
        _pred_map, 
        _distance_map,
        weight_function,
        edge_index_map,
        stopping_criteria,
        Base::_stats);
    } 
    catch (std::exception& e) {
       // logger(logWARNING)
       //   << left("[solver]", 14)
       //   << e.what(); 
    }

  }
    
  virtual graph_solver_result get_result() override 
  {
    typedef typename Traits::in_edge_iterator in_edge_iterator;    

    path_t          path;
    edge_descriptor ie_t;
    WeightT         d_t   = std::numeric_limits<WeightT>::max();    
    bool            found = false;

    in_edge_iterator iei, iei_end;
    for (tie(iei, iei_end) = boost::in_edges(_t, Base::_g); iei != iei_end; ++iei) {     
      if (   (_pred_map[Base::_g[*iei].edge_index] != Base::_g[*iei].edge_index) 
           && 
             (_distance_map[Base::_g[*iei].edge_index] < d_t) ) {
        ie_t  = *iei;
        d_t   = _distance_map[Base::_g[ie_t].edge_index];  
        found = true;
      }
    }
    if (!found) {
      throw solver_exception(
        "get_result(): Not path to target");
    }
    for ( edge_descriptor e = ie_t; 
          _pred_map[Base::_g[e].edge_index] != Base::_g[e].edge_index; 
          e = _edgmap[_pred_map[Base::_g[e].edge_index]] ) 
      path.push_front(e);  

    // add first edge 
    if (found)
      path.push_front(
        _edgmap[_pred_map[Base::_g[path.front()].edge_index]]);      
    
    graph_solver_result res = 
        {std::make_pair(_distance_map[Base::_g[ie_t].edge_index], path)};

    _pred_map.clear();
    _distance_map.clear();
            
    return res;
  }
    
 private:
  vertex_descriptor              _s;
  vertex_descriptor              _t;
  std::vector<edge_index_t>      _pred_map;
  std::vector<WeightT>           _distance_map;
  EdgeMap&                       _edgmap;

};	

} // namespace gol

#endif // GOL_GRAPH_ARCB_SOLVER_H_