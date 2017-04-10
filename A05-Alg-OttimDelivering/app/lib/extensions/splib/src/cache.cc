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

#include "cache.h"

namespace gol { 
  namespace cache {

  bool has(std::string filename) {
    if (boost::filesystem::exists(filename))
      return true;
    return false;    
  }
  
  // Gets the serialized entry from cache for that filename.
  bool load_public_transport(timetable_Rt* tt, std::string filename) 
  {
    try {
      std::ifstream ifs(filename.c_str());
      if (ifs.good()) {
        boost::archive::binary_iarchive ia(ifs);
        ia >> BOOST_SERIALIZATION_NVP(*tt);
        ifs.close();
        return true;
      } else {
        return false;
      }
    } catch (boost::archive::archive_exception& e) {
      logger(logERROR) << left("[cache]", 14) << e.what();
      throw data_exception( std::string("load_public_transport(): ") +  e.what());
    }
  }

  // Puts a serialized object in the cache with key filename
  void save_public_transport(const timetable_Rt& tt, std::string filename) 
  {
    std::ofstream ofs(filename.c_str());
    assert(ofs.good());
    boost::archive::binary_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(tt);
    ofs.close();
  }

  void parse_public_transport(
    timetable_Rt* tt, 
    raptor_timetable_builder* builder, 
    std::string filename) 
  { 
    // OpenStreetMap
    if ((sys_path(filename).extension()) == ".json") {
      logger(logINFO) << left("[cache]", 14) << "Parsing Test JSON Timetable";
      osm::json_director<raptor_timetable_builder> director(filename.c_str(), builder);
      director.construct_model();  
    } 
    //else if ((sys_path(filename).extension()) == ".gtfs") {
      // TODO
    //} 
    else {
      logger(logERROR) 
        << left("[cache]", 14) 
        << "parse_public_transport(): "
        << "File extension unknown";
      throw data_exception( std::string("parse_public_transport(): File extension unknown ") 
        + sys_path(filename).extension().generic_string() );  
    }    
  }

  void retrieve_public_transport(
    timetable_Rt* tt, 
    raptor_timetable_builder* builder, 
    std::string filename, 
    std::string model) 
  {
    try 
    { 
      sys_path root = boost::filesystem::current_path() / sys_path(RELATIVE_DIR);
      sys_path serialized = root / sys_path("cache") / 
        sys_path((std::string(filename).append(".").append(model)).append(".ttsrz")).filename();
      sys_path data = root / sys_path(filename);         
      
      if (has(serialized.generic_string())) {
        logger(logINFO) << left("[cache]", 14) << "Loading > " << serialized.generic_string();
        load_public_transport(tt, serialized.generic_string());
      } else {
        parse_public_transport(tt, builder, data.generic_string());
        logger(logINFO) << left("[cache]", 14) << "Serializing Road Network Graph ";
        save_public_transport(*tt, serialized.generic_string());
      }
      
    } catch (std::exception& e) {
      logger(logERROR) << left("[cache]", 14) << e.what();
      throw data_exception( std::string("retrieve_public_transport(): ") + e.what());  
    }
  } 

}  // namespace cache


}  // namespace gol


