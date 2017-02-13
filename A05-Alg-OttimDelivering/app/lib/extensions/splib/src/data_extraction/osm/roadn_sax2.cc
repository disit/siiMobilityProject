
namespace gol { namespace osm {

// XERCES-C SAX2 OSM Format
template <typename BuilderT>
void roadn_sax2_parser<BuilderT>::startElement(
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname,
    const Attributes& attrs) 
{
  // OSM node
/*  if (ch16strcmp(qname, "node")) {
    std::string osm_id; double lon = 0, lat = 0;
    // result of getLength() will be zero if there are no attributes
    for (XMLSize_t i = 0; i < attrs.getLength(); i++) {
      std::string key = ch16tostr(attrs.getQName(i));
      std::string value = ch16tostr(attrs.getValue(i));
      if (key == "id") {
        //std::cout<<ch16tostr(value)<<std::endl;
        osm_id = value;
      }
      if (key == "lon") {
        lon = ::atof(value.c_str());
      }
      if (key == "lat") {
        lat = ::atof(value.c_str());
      }
    }
    (*_builder).add_node(osm_id, lon, lat, _pMap);
  }

  // OSM way
  if (ch16strcmp(qname, "way")) {
    _is_way = true; // relations have tag highway
  }
  if (ch16strcmp(qname, "nd")) {
    for (XMLSize_t i = 0; i < attrs.getLength(); i++) {
      std::string key = ch16tostr(attrs.getQName(i));
      std::string value = ch16tostr(attrs.getValue(i));
      if (key == "ref") {
        //std::cout<<ch16tostr(value)<<std::endl;
        _refs.push_back(value);
      }
    }    
  } // nd
  if (ch16strcmp(qname, "tag")) {
    std::string key, value;
    for (XMLSize_t i = 0; i < attrs.getLength(); i++) {
      if (ch16strcmp(attrs.getQName(i), "k")) {
        key = ch16tostr(attrs.getValue(i));
      }
      if (ch16strcmp(attrs.getQName(i), "v")) {
        value = ch16tostr(attrs.getValue(i));
      }
    }
    if (key == "highway" && _is_way) { // default ROAD
      if (value != "construction" &&  // Road under construction
          value != "proposed"     &&  // For roads that are about to be built
          value != "services"     &&  // Service area places along a road
          value != "bus_guideway") {  // Not suitable for other traffic
        
        _is_highway = true; 

        if (value == "pedestrian"    ||  // Reserved for pedestrian-only use
            value == "footway"       ||  // Used mainly by pedestrians (also allowed for bicycles)
            value == "steps"         ||  // Steps on footways
            value == "living_street" ||  // tmp.. TODO
            value == "track"         ||  // Dirt roads for mostly agricultural or forestry uses
            value == "bridleway"     ||  // Use by horse riders (primarily) and pedestrians
            value == "service"       ||  // Generally for access to a building (not frontage road)
            value == "road") {           // Road with an unknown classification 
          _pMap[hwy_type] = PEDESTRIAN;
        } 
        if (value == "motorway" ||
            value == "motorway_link") { 
          _pMap[hwy_type] = MOTORWAY;
          _pMap[hwy_oneway] = FORWARD;
        }
        if (value == "cycleway") { 
          _pMap[is_cycleway] = true;
        }
        if (value == "path") { // open to all non-motorized vehicles
          _pMap[hwy_type] = PEDESTRIAN; // WARNING: check key == "foot" and key == "is_cycleway"
          _pMap[is_cycleway] = true;
        }
        if (value == "living_street") { // Road with very low speed limits (pedestrian friendly) 
          // TODO
        } 
      }
    }     
    if (key == "hwy_oneway" || key == "junction") { // default BOTH
      if (value == "yes" || 
          value == "1" || 
          value == "true") {
        _pMap[hwy_oneway] = FORWARD;
      }
      if (value == "-1" || 
          value == "reverse") {
        _pMap[hwy_oneway] = BACKWARD;
      }
      if (value == "roundabout") {
        _pMap[hwy_oneway] = FORWARD;
      }
    }
    if (key == "sidewalk") { // ROAD-PEDESTRIAN
      //if (value == "both" || value == "right" || value == "left" || value == "yes")
        // TODO
    }
    if (key == "cycleway") { // mapping a cycle track
      _pMap[is_cycleway] = true;
    }   
  } // tag

  */  
}

template <typename BuilderT>
void roadn_sax2_parser<BuilderT>::endElement(
    const XMLCh* const uri,
    const XMLCh* const localname,
    const XMLCh* const qname) 
{
  // construct the way
  /*if (ch16strcmp(qname, "way")) {
    auto it = _refs.begin();
    while (it != _refs.end() && _is_highway) {
      std::string sid = (*it);
      it++; // tid
      if (it != _refs.end()) {         
        (*(_builder)).add_section(sid, (*it), _pMap); 
      }
    }   
    // reset
    _refs.clear();
    _pMap = pMaps_map(); // default values
    _is_highway = false; _is_way = false;
  } // way
*/
}

template <typename BuilderT>
void roadn_sax2_parser<BuilderT>::construct_model() 
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
    //info() << "[XERCES-C-READER] " << "Parsing.. " << "\n" << boost::filesystem::path(_filename);
    (*parser).parse(_filename.c_str());
  } catch (const XMLException& toCatch) {
    // cerr
  } catch (const SAXParseException& toCatch) {
    // cerr
  }
  //info() << "[BUILDER] " << "Road Network Graph: |V| = " << _builder->get_nodes() << " |E| = " << _builder->get_edges();
  XMLPlatformUtils::Terminate();
  */

}

}  // namespace osm
}  // namespace gol
