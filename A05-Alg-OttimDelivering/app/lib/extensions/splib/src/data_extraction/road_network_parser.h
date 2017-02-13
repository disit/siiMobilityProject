#ifndef GOL_ROADN_PARSER_H__
#define GOL_ROADN_PARSER_H__

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

#include "../graph/graph_builder_factory.h"

namespace gol { 
  namespace osm {

template <typename BuilderT>
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
        std::vector<std::string>,          // from way 
        std::vector<std::string> >         // to   way
    > 
  >                        _relation_restriction;

  std::map< 
    std::string,                           // way
    std::vector<std::string>               // nd                           
  >                        _restricted_way;

  // Stores all the nodes of all the ways that are part of the road network
  // The nodes and ways might not be in .osm order 
  // std::vector< std::pair<std::vector<uint64_t>, pMaps_map> > _ways;

}; 

template <typename BuilderT>
class roadn_sax2_parser : public DefaultHandler {
 public:
  roadn_sax2_parser(std::string filename, BuilderT* builder): 
      _builder(builder), 
      _filename(filename), 
      _refs(), 
      _is_highway(false), 
      _is_way(false) {}
  ~roadn_sax2_parser() {}

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
  BuilderT*   _builder;
  std::string _filename;

  std::vector<std::string> _refs; 
  bool         _is_highway;
  bool         _is_way;
  features_map _fmap;

};

}  // namespace osm
}  // namespace gol

#include "osm/roadn_sax2.cc" 
#include "osm/roadn_pbf.cc"  

#endif  // GOL_ROADN_PARSER_H__
