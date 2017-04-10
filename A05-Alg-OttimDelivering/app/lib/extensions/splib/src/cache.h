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

#ifndef GOL_CACHE_H_
#define GOL_CACHE_H_

// std
#include <iostream>
#include <fstream>
// boost
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/filesystem.hpp>

//#include "data_extraction/sqlite/sqlite_helper.h"
#include "data_extraction/road_network_dext.h"
#include "data_extraction/public_transport_dext.h"

namespace gol { 
  namespace cache {
  
  typedef boost::filesystem::path sys_path;

  bool has(std::string filename);

  // Gets the serialized entry from cache for that filename.
  template <
      typename GraphT, 
      typename VertexMap,
      typename EdgeMap,
      typename Constraints>
  bool load_road_network(
      GraphT*         g, 
      VertexMap*      vtx,
      EdgeMap*        edg,
      Constraints*    ctr, 
      std::string     filename) 
  {
    typedef typename boost::graph_traits<GraphT>::edge_iterator 
      edge_iterator;

    try {
      std::ifstream ifs(filename.c_str());
      if (ifs.good()) {
        boost::archive::binary_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(*g);
        ia >> BOOST_SERIALIZATION_NVP(*vtx);
        //ia >> BOOST_SERIALIZATION_NVP(*edg);
        BGL_FORALL_EDGES_T(e, *g, GraphT) {
          edg->insert( std::make_pair((*g)[e].edge_index, e) );
        }       
        //ia >> BOOST_SERIALIZATION_NVP(*ctr);
        ifs.close();
        return true;
      } else {
        return false;
      }
    } catch (boost::archive::archive_exception& e) {
      throw data_exception( 
        std::string("load_road_network(): ") + e.what());
    }
  }

  // Puts a serialized object in the cache with key filename
  template <
      typename GraphT, 
      typename VertexMap,
      typename EdgeMap,
      typename Constraints>     
  void save_road_network(
      const GraphT&         g, 
      const VertexMap&      vtx,
      const EdgeMap&        edg,
      const Constraints&    ctr, 
      std::string           filename) 
  {
    std::ofstream ofs(filename.c_str());
    assert(ofs.good());
    boost::archive::binary_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(g);
    oa << BOOST_SERIALIZATION_NVP(vtx);
    //oa << BOOST_SERIALIZATION_NVP(edg);
    //oa << BOOST_SERIALIZATION_NVP(ctr);
    
    ofs.close();
  }

  template <
      typename GraphT, 
      typename VertexMap,
      typename EdgeMap,
      typename Constraints,        
      typename BuilderT>
  void parse_road_network(
    GraphT*         g, 
    VertexMap*      vtx,
    EdgeMap*        edg,
    Constraints*    ctr, 
    BuilderT*       builder, 
    sys_path        filename) 
  {
    logger(logINFO) 
        << left("[cache]", 14) 
        << "Open Connection to DB";   
    
    // OpenStreetMap
    if (((filename).extension()) == ".pbf") 
    {
      osm::parserPBF<BuilderT> director(
        filename.generic_string(), 
        ( boost::filesystem::current_path() / 
          sys_path(RELATIVE_DIR)            /
          sys_path("data")                  /
          sys_path(DEFAULT_DB_NAME) ).
        generic_string(), 
        builder);
      director.construct_model();

      //osm::roadn_pbf_parser<BuilderT> director2(
      //  filename.generic_string(), builder);
      //director2.construct_model();
    } 
    else if (((filename).extension()) == ".osm") 
    {      
      osm::parserSAX2<BuilderT> director(
        filename.generic_string(),
        ( boost::filesystem::current_path() / 
          sys_path(RELATIVE_DIR)            /
          sys_path("data")                  /
          sys_path(DEFAULT_DB_NAME) ).
        generic_string(),          
        builder);
      director.construct_model();
    } 
    else {
      logger(logERROR) 
        << left("[cache]", 14) 
        << "File extension unknown";
      throw data_exception( 
        std::string("parse_road_network(): File extension unknown ") 
        + sys_path(filename).extension().generic_string() );
    }    
  }

  template <
      typename GraphT, 
      typename VertexMap,
      typename EdgeMap,
      typename Constraints,        
      typename BuilderT>
  void retrieve_road_network(
    GraphT*         g, 
    VertexMap*      vtx,
    EdgeMap*        edg,
    Constraints*    ctr,
    BuilderT*       builder, 
    std::string     filename, 
    std::string     model) 
  {
    try 
    { 
      sys_path root = 
        boost::filesystem::current_path() / 
        sys_path(RELATIVE_DIR);
      sys_path serialized = 
        root / 
        sys_path("cache") / 
        sys_path((std::string(filename)
                      .append(".")
                      .append(model))
                    .append(".gsrz"))
              .filename();
      sys_path data = 
        root / 
        sys_path(filename); 
      
      if (has(serialized.generic_string())) 
      {
        logger(logINFO) 
          << left("[cache]", 14) 
          << "Loading Graph >> " 
          <<  sys_path((std::string(filename)
                  .append(".")
                  .append(model))
                .append(".gsrz"))
              .filename();          
        
        load_road_network(
          g, vtx, edg, ctr, serialized.generic_string());
      } 
      else 
      {
        parse_road_network(
          g, vtx, edg, ctr, builder, data.generic_string());
        
        logger(logINFO) 
            << left("[cache]", 14) 
            << "<< Serializing Graph ";            
        
        save_road_network(
          *g, *vtx, *edg, *ctr, serialized.generic_string());
      }

    } catch (std::exception& e) {
      logger(logERROR) 
        << left("[cache]", 14) 
        << e.what();
      throw data_exception( 
        std::string("retrieve_road_network(): ") + e.what()); 
    }
  } 

  // Gets the serialized entry from cache for that filename.
  bool load_public_transport(timetable_Rt* tt, std::string filename);

  // Puts a serialized object in the cache with key filename
  void save_public_transport(const timetable_Rt& tt, std::string filename);
  
  void parse_public_transport(
    timetable_Rt* tt, 
    raptor_timetable_builder* builder, 
    std::string filename);

  void retrieve_public_transport(
    timetable_Rt* tt, 
    raptor_timetable_builder* builder, 
    std::string filename, 
    std::string model);


  }  // namespace cache    
}  // namespace gol

#endif  // GOL_CACHE_H_
