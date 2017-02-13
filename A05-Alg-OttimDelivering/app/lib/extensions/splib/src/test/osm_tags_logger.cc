
#include "../data_extraction/road_network_parser.h"

namespace gol {

class osm_tags_logger : public pbf::default_handler {
 public:
  osm_tags_logger(
    std::string input, 
    std::string output = std::string("highway.tags")): 
      _filename(input),
      _os(output),
      _n_nodes(0),
      _n_ways(0)     {} 
  ~osm_tags_logger() {}

  // * Handlers PBF interface
  void node_callback(
    uint64_t           osm_id, 
    double             lon, 
    double             lat, 
    const pbf::tags_t& tags) { _n_nodes++; }
  
  void way_callback(
    uint64_t                     osm_id, 
    const pbf::tags_t&           tags, 
    const std::vector<uint64_t>& refs) 
  { 
    _n_ways++;
    std::string key, value; 
   
    for (auto tag : tags) {
      tie(key, value) = tag;
      _os << key <<" > " << value << std::endl;
    } 
  }
  
  void relation_callback(
    uint64_t                 osm_id, 
    const pbf::tags_t&       tags, 
    const pbf::references_t& refs) {};   
  // * Handlers PBF interface

  void construct_model() 
  {
    pbf::osm_reader<osm_tags_logger> parser(_filename, *this);
    parser.parse();
    
    _os <<"#nodes = " << _n_nodes
        <<" #ways = " << _n_ways 
        << std::endl;
  }
 
 private:
  std::string _filename;
  std::ofstream _os;

  uint64_t _n_nodes = 0;
  uint64_t _n_ways  = 0; 

};

} // namespace gol  

int main(int argc, char* argv[]) {

  // print parsed tags
  gol::osm_tags_logger* logger = new gol::osm_tags_logger(argv[1]);
  logger->construct_model();

 return 0;

}

