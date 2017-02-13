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

#include "data_extraction/road_network_parser.h"
#include "data_extraction/public_transport_parser.h"

namespace gol { 
  namespace cache {
  
  typedef boost::filesystem::path sys_path;

  bool has(std::string filename);

  // Gets the serialized entry from cache for that filename.
  template <
      typename GraphT, 
      typename VertexMap,
      typename RestrictionMap>
  bool load_road_network(
      GraphT*         g, 
      VertexMap*      vtx, 
      RestrictionMap* res,
      std::string     filename) 
  {
    try {
      std::ifstream ifs(filename.c_str());
      if (ifs.good()) {
        boost::archive::binary_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(*g);
        ia >> BOOST_SERIALIZATION_NVP(*vtx);
        ia >> BOOST_SERIALIZATION_NVP(*res);
        ifs.close();
        return true;
      } else {
        return false;
      }
    } catch (boost::archive::archive_exception& e) {
      throw data_exception( 
        std::string("load_road_network(): ") +  e.what());
    }
  }

  // Puts a serialized object in the cache with key filename
  template <
      typename GraphT, 
      typename VertexMap,
      typename RestrictionMap>
  void save_road_network(
      const GraphT&         g, 
      const VertexMap&      vtx, 
      const RestrictionMap& res,
      std::string           filename) 
  {
    std::ofstream ofs(filename.c_str());
    assert(ofs.good());
    boost::archive::binary_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(g);
    oa << BOOST_SERIALIZATION_NVP(vtx);
    oa << BOOST_SERIALIZATION_NVP(res);
    ofs.close();
  }

  template <
      typename GraphT, 
      typename VertexMap,
      typename RestrictionMap, 
      typename BuilderT>
  void parse_road_network(
    GraphT*         g, 
    VertexMap*      vtx, 
    RestrictionMap* res,    
    BuilderT*       builder, 
    sys_path        filename) 
  { 
    // OpenStreetMap
    if (((filename).extension()) == ".pbf") 
    {
      logger(logINFO) 
        << left("[cache]", 14) 
        << "Parsing OSM Protocolbuffer Binary Format";
      
      osm::roadn_pbf_parser<BuilderT> director(
        filename.generic_string(), builder);
      director.construct_model();
    } 
    else if (((filename).extension()) == ".osm") 
    {
      logger(logINFO) 
        << left("[cache]", 14) 
        << "Open the OSM XML Format"; 
      
      osm::roadn_sax2_parser<BuilderT> director(
        filename.generic_string(), builder);
      director.construct_model();
    } 
    else {
      logger(logERROR) 
        << left("[cache]", 14) 
        << "parse_road_network(): "
        << "File extension unknown";
      throw data_exception( 
        std::string("parse_road_network(): File extension unknown ") 
        + sys_path(filename).extension().generic_string() );
    }    
  }

  template <
      typename GraphT, 
      typename VertexMap,
      typename RestrictionMap, 
      typename BuilderT>
  void retrieve_road_network(
    GraphT*         g, 
    VertexMap*      vtx,
    RestrictionMap* res, 
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
          << "Loading > " 
          << serialized.generic_string();        
        load_road_network(
          g, vtx, res, serialized.generic_string());
      } 
      else 
      {
        parse_road_network(
          g, vtx, res, builder, data.generic_string());
        
        logger(logINFO) 
          << left("[cache]", 14) 
          << "Serializing Road Network Graph ";        
        save_road_network(
          *g, *vtx, *res, serialized.generic_string());
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
