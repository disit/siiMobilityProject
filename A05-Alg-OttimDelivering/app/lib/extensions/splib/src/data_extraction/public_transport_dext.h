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

#ifndef GOL_PTRAN_SAX_DEXT_H__
#define GOL_PTRAN_SAX_DEXT_H__

// stl
#include <list>             
#include <map>
#include <ctime>  // for C89 std clock()
#include <streambuf>
#include <sstream>
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
// boost
#include <boost/lexical_cast.hpp>
// json
#include "../round_based/round_based_model/raptor_timetable_builder.h"

namespace gol { 

namespace siimobility {

template <typename BuilderT>
class ptran_sax2_parser : public DefaultHandler {
 public:
  ptran_sax2_parser(std::string filename, BuilderT* builder)
     : _builder(builder), _filename(filename), _refs(), _is_highway(false), _is_way(false) {}
  ~ptran_sax2_parser() {}
  ptran_sax2_parser(ptran_sax2_parser&&) = delete;
  ptran_sax2_parser(const ptran_sax2_parser&) = delete;
  ptran_sax2_parser& operator=(ptran_sax2_parser&&) = delete;
  ptran_sax2_parser& operator=(const ptran_sax2_parser&) = delete;
  // * Handlers for the SAX interface
  void startElement(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const Attributes& attrs);

  void endElement(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname);

  void characters(
      const XMLCh* const chars,
      const XMLSize_t length) {}

  void warning(const SAXParseException& exc){}
  void error(const SAXParseException& exc){}
  void fatalError(const SAXParseException& exc){}
  void resetErrors(){}
    // void characters(const XMLCh* const chars, const unsigned int length);
    // void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
    // void resetDocument();
  // * Handlers for the SAX interface

  void construct_model();

 private:
  BuilderT* _builder;
  std::string _filename;

  std::vector<std::string> _refs; 
  bool _is_highway;
  bool _is_way;
  features_map _fmap;

};

}  // namespace siimobility
}  // namespace gol

// parsing timetable.json for OSM public transport test
#include "test/ptran_json.h"

#endif  // GOL_PTRAN_SAX_DEXT_H__
