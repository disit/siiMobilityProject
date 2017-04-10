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

#ifndef GOL_GRAPH_CONSTRAINS_H_
#define GOL_GRAPH_CONSTRAINS_H_

namespace gol {

template<typename GraphT, typename WeightT>
class graph_constraints_t
{
  typedef boost::graph_traits<GraphT>        Traits;
  typedef typename Traits::vertex_descriptor vertex_descriptor;
  typedef typename Traits::edge_descriptor   edge_descriptor;

  // model turn costs
  typedef boost::multi_array<WeightT, 2> turn_table_t; 

 private:
  GraphT&                    _g;  
  std::vector<turn_table_t*> _shared_turn_tables;
 
 public: 
  graph_constraints_t(GraphT& g): 
      _g(g), _shared_turn_tables() {}

  ~graph_constraints_t() {
      for (auto t : _shared_turn_tables)
        delete t;
    } 

  void create_turn_table(
      vertex_descriptor            u, 
      std::vector<edge_descriptor> incoming_edges, 
      std::vector<edge_descriptor> outgoing_edges)
  {
    unsigned int p = incoming_edges.size(); 
    unsigned int q = outgoing_edges.size();

    // using default comparison (operator <) to order edges
    std::sort(incoming_edges.begin(), 
                  incoming_edges.end());
    std::sort(outgoing_edges.begin(), 
                  outgoing_edges.end());

    if ( p > 0 && q > 0 ) 
    {     
      turn_table_t* turn_table = 
        new turn_table_t(boost::extents[p][q]);
        
      for (unsigned i = 0; i < p; ++i)
        for (unsigned j = 0; j < q; ++j) 
        {
          edge_descriptor ie, oe;
          ie = incoming_edges[i];
          oe = outgoing_edges[j];

          _g[ie].entry_point = i;
          _g[oe].exit_point  = j;          
          
          if (boost::source(ie, _g) == boost::target(oe, _g))
            (*turn_table)[i][j] = 
                  std::numeric_limits<WeightT>::max();
          else
            (*turn_table)[i][j] = WeightT(); // TODO add turn cost != 0      
        }        
      
      //_shared_turn_tables.push_back(turn_table);
      _g[u].turn_table = turn_table;

    }

  }     

  void compact_turn_tables() {
      // TODO
  }

  void dump_shared_turn_tables() {}  

  };

} // namepsace gol

#endif // GOL_GRAPH_CONSTRAINS_H_