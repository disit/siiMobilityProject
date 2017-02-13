
#ifndef GOL_UTILS_JSON_UTF8_H_
#define GOL_UTILS_JSON_UTF8_H_

// std
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

namespace gol {
namespace json_utf8 {

inline unsigned int mask8(char const value) {
  return value & 0xff;
}

inline bool is_valid_continuation_byte(unsigned int byte) {
  return ((byte & 0xC0) == 0x80);
}

inline int get_next_byte(std::string::const_iterator& iterator, std::string::const_iterator end_iterator) {
  if(iterator != end_iterator) {
    return mask8(*(++ iterator));
  } else {
    return 0; // invalid continuation byte
  }
}

void insert_replacement(std::vector<unsigned int>& output, unsigned int replacement, unsigned int count);
std::vector<unsigned int> decode_utf8(const std::string& input, const int replacement=0xfffd);
std::string json_encode_control_char(unsigned int codepoint);
std::string utf8_encode(unsigned int codepoint);
std::string json_encode_codepoints(std::vector<unsigned int> const& codepoints);


} // namespace json_utf8
} // namespace gol

#endif // GOL_UTILS_JSON_UTF8_H__
