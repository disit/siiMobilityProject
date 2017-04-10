#ifndef GOL_OSM_JSON_TEST_H_
#define GOL_OSM_JSON_TEST_H_

// std
#include <list>             
#include <map>
#include <ctime>  // for C89 std clock()
#include <streambuf>
#include <sstream>
#include <memory>   // std::auto_ptr
#include <unordered_map>
// boost
#include <boost/lexical_cast.hpp>
// json
#include "../../utils/json_parser.h"

#include "../public_transport_dext.h"

namespace gol { namespace osm {

/// Timetalble json Parser 

template <typename BuilderT>
class json_director {
 public:
  json_director(std::string filename, BuilderT* builder)
      : _builder(builder), _filename(filename) {} 
  ~json_director() {}
  json_director(json_director&&) = delete;
  json_director(const json_director&) = delete;
  json_director& operator=(json_director&&) = delete;
  json_director& operator=(const json_director&) = delete;

  void construct_model();
 
 private:
  BuilderT* _builder;
  std::string _filename; 

};  

}  // namespace osm
}  // namespace gol

#include "ptran_json.cc"

#endif  // GOL_OSM_JSON_TEST_H_
