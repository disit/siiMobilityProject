#ifndef GOL_PBF_TAG_H_
#define GOL_PBF_TAG_H_

// std
#include <stdint.h>
#include <string>
// osmpbf
#include <osmpbf/osmformat.pb.h> // high-level OSM objects

namespace gol { namespace pbf {

typedef std::map<std::string, std::string> tags_t; // key:value of an object

template<typename T>
tags_t get_tags(T object, const OSMPBF::PrimitiveBlock &primblock) {
  tags_t result;
  for (int i = 0; i < object.keys_size(); ++i) {
    uint64_t key = object.keys(i);
    uint64_t val = object.vals(i);
    std::string key_string = primblock.stringtable().s(key);
    std::string val_string = primblock.stringtable().s(val);
    result[key_string] = val_string;
  }
  return result;
}

} // namespace pbf
} // namespace gol

#endif // GOL_PBF_TAG_H_
