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

namespace gol { namespace osm {

// XERCES-C SAX2 OSM Format
template <typename BuilderT>
void parserSAX2<BuilderT>::startElement(
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname,
    const Attributes& attrs) 
{
  // TODO
}

template <typename BuilderT>
void parserSAX2<BuilderT>::endElement(
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname) 
{
  // TODO
}

template <typename BuilderT>
void parserSAX2<BuilderT>::construct_model() 
{
  /*try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    // cerr
  }
  SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
  // The ContentHandler and ErrorHandler instances required by
  // the SAX2 API are provided using the DefaultHandle
  (*parser).setContentHandler(this);
  (*parser).setErrorHandler(this);
  // No schema validation
  try {
    (*parser).parse(_filename.c_str());
  } catch (const XMLException& toCatch) {
    // cerr
  } catch (const SAXParseException& toCatch) {
    // cerr
  }
  XMLPlatformUtils::Terminate();
  */

}

}  // namespace osm
}  // namespace gol
