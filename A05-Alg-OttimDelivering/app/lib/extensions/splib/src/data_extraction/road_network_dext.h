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

#ifndef GOL_ROADN_DEXT_H__
#define GOL_ROADN_DEXT_H__

// stl
#include <list>             
#include <map>
#include <ctime>    // for C89 std clock()
#include <streambuf>
#include <sstream>
#include <fstream>
#include <memory>   // std::auto_ptr
#include <unordered_map>
// xercesc
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesVersion.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
// pbf 
#include "../pbf/osm_tag.h"
#include "../pbf/osmPBF_reader.h"
#include "../pbf/osmPBF_handler.h"
// boost
#include <boost/lexical_cast.hpp>

#include "sqlite/sqlite_database_helper.h"
#include "../graph/graph_builder_factory.h"

namespace gol { 
  namespace osm {

/*template <typename BuilderT>
class roadn_pbf_parser : public pbf::default_handler {
 public:
  roadn_pbf_parser(std::string filename, BuilderT* builder)
      : _builder(builder), 
        _filename(filename),
        _add_relation_to_model(false), 
        _relation_area(),
        _relation_restriction(), 
        _restricted_way() {} 
  ~roadn_pbf_parser()   {}

  // * Handlers PBF interface
  void node_callback(
    uint64_t           osm_id,
    double             lon, 
    double             lat, 
    const pbf::tags_t& tags);
  void way_callback(
    uint64_t                     osm_id, 
    const pbf::tags_t&           tags, 
    const std::vector<uint64_t>& refs);
  void relation_callback(
    uint64_t                 osm_id, 
    const pbf::tags_t&       tags, 
    const pbf::references_t& refs);   
  // * Handlers PBF interface

  void construct_model();
 
 private:
  BuilderT*   _builder;
  std::string _filename;
  
  // Realtion tags
  bool                     _add_relation_to_model;
  std::vector<std::string> _relation_area; // way  
  std::map<
    std::string,                           // via                          
    std::vector<
      std::pair< 
        std::string,
        std::pair< 
          std::vector<std::string>,        // from way 
          std::vector<std::string> >       // to   way
      >
    > 
  >                        _relation_restriction;

  std::map< 
    std::string,                           // way
    std::vector<std::string>               // nd                           
  >                        _restricted_way;

  // Stores all the nodes of all the ways that are part of the road network
  // The nodes and ways might not be in .osm order 
  // std::vector< std::pair<std::vector<uint64_t>, pMaps_map> > _ways;

};//*/

template <typename BuilderT>
class parserPBF: 
  public pbf::default_handler 
{
 public:
  parserPBF(
    std::string filename, 
    std::string db_name, 
    BuilderT*   builder)
      : _builder(builder), 
        _filename(filename),
        _dbh(db_name) {} 
  ~parserPBF()                  {}

  // * Handlers PBF interface
  void node_callback(
    uint64_t           osm_id,
    double             lon, 
    double             lat, 
    const pbf::tags_t& tags);
  void way_callback(
    uint64_t                     osm_id, 
    const pbf::tags_t&           tags, 
    const std::vector<uint64_t>& refs);
  void relation_callback(
    uint64_t                 osm_id, 
    const pbf::tags_t&       tags, 
    const pbf::references_t& refs);   
  // * Handlers PBF interface

  void construct_model();

 private:
  BuilderT*       _builder;
  std::string     _filename;  
  sqlite_database_helper_t _dbh;

  
};  

template <typename BuilderT>
class parserSAX2 : public DefaultHandler {
 public:
  parserSAX2(
    std::string filename,
    std::string db_name, 
    BuilderT* builder): 
      _builder(builder), 
      _filename(filename),
      _dbh(db_name) {}       
  ~parserSAX2()               {}

  // * Handlers for the SAX interface
  void startElement(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const Attributes&  attrs);

  void endElement(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname);

  void characters(
      const XMLCh* const chars,
      const XMLSize_t    length) {}

  void warning(const SAXParseException& exc)    {}
  void error(const SAXParseException& exc)      {}
  void fatalError(const SAXParseException& exc) {}
  void resetErrors(){}
    // void characters(const XMLCh* const chars, const unsigned int length);
    // void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
    // void resetDocument();
  // * Handlers for the SAX interface

  void construct_model();

 private:
  BuilderT*       _builder;
  std::string     _filename;
  // OSMdb
  sqlite_database_helper_t _dbh;  

};

}  // namespace osm
}  // namespace gol

//#include "osm/roadn_pbf.cc"

#include "osm/parserPBF.cc"   
#include "osm/parserSAX2.cc" 

#endif  // GOL_ROADN_DEXT_H__
