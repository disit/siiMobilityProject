#ifndef GOL_PBF_REFERENCE_H_
#define GOL_PBF_REFERENCE_H_

// std
#include <stdint.h>
#include <string>
// osmpbf
#include <osmpbf/osmformat.pb.h> // high-level OSM objects

namespace gol { namespace pbf {

struct reference_t {
  reference_t() {}
    reference_t(OSMPBF::Relation::MemberType member_type, uint64_t member_id, std::string role) :
        member_type(member_type), member_id(member_id), role(role) {}

  OSMPBF::Relation::MemberType member_type; // type of relation
  uint64_t                     member_id;   // osm id
  std::string                  role;        // role
};

typedef std::vector<reference_t> references_t;

} // namespace pbf
} // namespace gol

#endif // GOL_PBF_REFERENCE_H_