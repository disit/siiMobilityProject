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

#ifndef GOL_ENGINE_H_
#define GOL_ENGINE_H_

// boost
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
//#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/variant.hpp>

// road_network
#include "graph/generic_edge_weighted_graph.h"
// public transport
#include "round_based/raptor_timetable.h"
#include "round_based/round_based_model/raptor_timetable_builder.h"
#include "round_based/raptor_solver_factory.h"

#include "cache.h"
#include "route.h"

#include "graphs.h"

namespace gol {

class engine_cache_t 
{   
  typedef boost::filesystem::path sys_path;
 private:
  static engine_cache_t* _cache_instance_ptr; 

  engine_cache_t(std::string data_graph_path): 
    _data_graph_path(data_graph_path),
    _road_compact_graph_ptr(nullptr),
    _pedestrian_graph_ptr(nullptr)    {}

  // don't implement
  engine_cache_t(engine_cache_t const &);
  void operator=(engine_cache_t const &);

  ~engine_cache_t();   

  std::string         _data_graph_path;
  road_graphT*        _road_compact_graph_ptr;
  pedestrian_graphT*  _pedestrian_graph_ptr;     

 public:
      
  static engine_cache_t*
  get_instance(std::string data_graph_path) {
    if ( _cache_instance_ptr == NULL ) 
      _cache_instance_ptr =  new engine_cache_t(data_graph_path);
    return _cache_instance_ptr;
  }

  void refresh( bool updateDB )
  {
    if (updateDB) 
    {
      logger(logINFO)
        << left("[*]", 14)
        << "Update OSM database >>";

      std::string db = ( 
        boost::filesystem::current_path() /
             sys_path(RELATIVE_DIR)       /
             sys_path("data")             /
             sys_path(DEFAULT_DB_NAME) ).
          generic_string();

      try {
        logger(logINFO)
          << left("[*]", 14)
          << "Open Connection to DB";
        sqlite_database_helper_t _dbh(db);

        logger(logINFO)
          << left("[*]", 14)
          << "> Reset";
        //if (!_dbh.empty())
        _dbh.reset();

        // clear old cached data
        std::string cache_dir = ( 
          boost::filesystem::current_path() /
          sys_path(RELATIVE_DIR)            /
          sys_path("cache") ).generic_string();
        std::string command = std::string("exec rm ")
          .append( cache_dir.append(std::string("/*")) );
        std::system(command.c_str());        

      } catch (std::exception& e) {
        logger(logERROR)
          << left("[DB]", 14)
          << "Reset and Update OSMdb error: "
          << e.what();
        return;
      }
    }

    logger(logINFO)
      << left("[cache]", 14)
      << "Refresh Cached Route Planning Models";    
    try 
    {
      logger(logINFO)
        << left("[cache]", 14)
        << "> Road Compact Representation Model";

      if (!(_road_compact_graph_ptr == nullptr))
        delete _road_compact_graph_ptr;
    
      _road_compact_graph_ptr = new road_graphT();       
      _road_compact_graph_ptr->create_model(
          "road_compact_representation_model", 
          _data_graph_path);

    } catch (std::exception& e) {
      logger(logERROR)
        << left("[DB]", 14)
        << "refresh road_compact_representation_model error: "
        << e.what();
    }
    try 
    {
      logger(logINFO)
        << left("[cache]", 14)
        << "> Pedestrian Simplified Model";

      if (!(_pedestrian_graph_ptr == nullptr))
        delete _pedestrian_graph_ptr;
    
      _pedestrian_graph_ptr = new pedestrian_graphT();      
      _pedestrian_graph_ptr->create_model(
          "pedestrian_simplified_model", 
          _data_graph_path);

    } catch (std::exception& e) {
      logger(logERROR)
        << left("[DB]", 14)
        << "refresh pedestrian_simplified_model error: "
        << e.what();
    }    
            
  }

  road_graphT& get_cached_road_network_for(std::string model) {
    //if ( model == "road_compact_representation_model" )
      return (*_road_compact_graph_ptr);
    //if ( model == "road_simplified_model" )
    //  return (*_road_graph_ptr);
  }

  pedestrian_graphT& get_cached_pedestrian_network_for(std::string model) {
    //if ( model == "pedestrian_simplified_model" )
      return (*_pedestrian_graph_ptr);
  }  
        
}; 

class engine_t {
 public:
  engine_t(std::string data_graph_path, bool updateDB) { 
#ifndef NLOG
    log_policy::get_instance().umtx();
#endif      
    _cache = engine_cache_t::get_instance(data_graph_path);
    _cache->refresh(updateDB); 
  }
  
  ~engine_t() { 
    //delete _cache; 
  } 

  void cache_refresh(bool updateDB) { 
    _cache->refresh(updateDB); 
  }   

  void
  dijkstra_based(
    std::string algorithm,
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy, 
    std::string data_graph_path, 
    std::string data_timetable_path,
    optimized_routes_solution* sol); 

  static void
  bicriterion_epsMOA_star(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy,     
    std::string data_graph_path, 
    std::string data_timetable_path,
    optimized_routes_solution* sol); 

/*  void
  dijkstra_raptor(
    std::string source, 
    std::string target, 
    std::string request_time,
    std::string model,
    std::string strategy, 
    std::string data_graph_path, 
    std::string data_timetable_path,
    double      search_radius_src,
    double      search_radius_trg,
    optimized_routes_solution* sol); */        

   
private:
  engine_cache_t* _cache;

};

} // namespace gol

#endif // GOL_ENGINE_H_
