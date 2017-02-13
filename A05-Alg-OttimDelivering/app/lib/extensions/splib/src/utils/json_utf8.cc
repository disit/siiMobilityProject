
#include "json_utf8.h"

namespace gol {
namespace json_utf8 {

void insert_replacement(std::vector<unsigned int>& output, unsigned int replacement, unsigned int count) {
  for (unsigned int i = 0 ; i < count ; ++ i) {
    output.push_back(replacement);
  }
}

std::vector<unsigned int> decode_utf8(const std::string& input, const int replacement) {
  unsigned int code_unit1, code_unit2, code_unit3, code_unit4;
  std::vector<unsigned int> codepoints;

  for (std::string::const_iterator iterator = input.begin() ; iterator != input.end() ; ++ iterator) {
    code_unit1 = mask8(*iterator);
    if (code_unit1 < 0x80) {
      codepoints.push_back(code_unit1);
    } else if (code_unit1 < 0xC2) { // continuation or overlong 2-byte sequence
      codepoints.push_back(replacement);
    } else if (code_unit1 < 0xE0) { // 2-byte sequence
      code_unit2 = get_next_byte(iterator, input.end());
      if (!is_valid_continuation_byte(code_unit2)) {
        insert_replacement(codepoints, replacement, 2);
      } else {
        codepoints.push_back((code_unit1 << 6) + code_unit2 - 0x3080);
      }
    } else if (code_unit1 < 0xF0) { // 3-byte sequence
      code_unit2 = get_next_byte(iterator, input.end());
      if (!is_valid_continuation_byte(code_unit2) ||
          (code_unit1 == 0xE0 && code_unit2 < 0xA0)) /* overlong */ {
        insert_replacement(codepoints, replacement, 2);
      } else {
        code_unit3 = get_next_byte(iterator, input.end());
        if (!is_valid_continuation_byte(code_unit3)) {
          insert_replacement(codepoints, replacement, 3);
        } else {
          codepoints.push_back((code_unit1 << 12) + (code_unit2 << 6) + code_unit3 - 0xE2080);
        }
      }
    } else if (code_unit1 < 0xF5) { // 4-byte sequence
      code_unit2 = get_next_byte(iterator, input.end());
      if(!is_valid_continuation_byte(code_unit2) ||
          (code_unit1 == 0xF0 && code_unit2 < 0x90) || /* overlong */
          (code_unit1 == 0xF4 && code_unit2 >= 0x90)) {  /* > U+10FFFF */
        insert_replacement(codepoints, replacement, 2);
      } else {
        code_unit3 = get_next_byte(iterator, input.end());
        if(!is_valid_continuation_byte(code_unit3)) {
          insert_replacement(codepoints, replacement, 3);
        } else {
          code_unit4 = get_next_byte(iterator, input.end());
          if(!is_valid_continuation_byte(code_unit4)) {
            insert_replacement(codepoints, replacement, 4);
          } else {
            codepoints.push_back((code_unit1 << 18) + (code_unit2 << 12) + (code_unit3 << 6) + code_unit4 - 0x3C82080);
          }
        }
      }
    } else {
      /* > U+10FFFF */
      insert_replacement(codepoints, replacement, 1);
    }
  }
  return codepoints;
}

std::string json_encode_control_char(unsigned int codepoint) {
  std::ostringstream oss;
  oss.fill('0');
  oss << "\\u" << std::setw(4) << std::hex << codepoint;
  return oss.str();
}

std::string utf8_encode(unsigned int codepoint) {
  std::string output;
  if (codepoint > 0x590 && codepoint < 0x5F4) {
    return output;
  }
  // out of range
  if(codepoint > 1114112) {
    return utf8_encode(0xfffd);
  }
  if (codepoint < 0x80) {
    output.push_back(codepoint);
  } else if (codepoint <= 0x7FF) {
    output.push_back((codepoint >> 6) + 0xC0);
    output.push_back((codepoint & 0x3F) + 0x80);
  } else if (codepoint <= 0xFFFF) {
    output.push_back((codepoint >> 12) + 0xE0);
    output.push_back(((codepoint >> 6) & 0x3F) + 0x80);
    output.push_back((codepoint & 0x3F) + 0x80);
  } else if (codepoint <= 0x10FFFF) {
    output.push_back((codepoint >> 18) + 0xF0);
    output.push_back(((codepoint >> 12) & 0x3F) + 0x80);
    output.push_back(((codepoint >> 6) & 0x3F) + 0x80);
    output.push_back((codepoint & 0x3F) + 0x80);
  }
  return output;
}

std::string json_encode_codepoints(std::vector<unsigned int> const& codepoints) {
  std::string json_string;

  for (std::vector<unsigned int>::const_iterator codepoint = codepoints.begin() ; codepoint != codepoints.end() ; ++ codepoint) {
    if (*codepoint == 8) { // \b
      json_string.push_back('\\');
      json_string.push_back('b');
    } else if(*codepoint == 9) {  // \t
      json_string.push_back('\\');
      json_string.push_back('t');
    } else if(*codepoint == 10) { // \n
      json_string.push_back('\\');
      json_string.push_back('n');
    } else if(*codepoint == 12) { // \f
      json_string.push_back('\\');
      json_string.push_back('f');
    } else if(*codepoint == 13) { // \r
      json_string.push_back('\\');
      json_string.push_back('r');
    } else if(*codepoint == 34) { // "
      json_string.push_back('\\');
      json_string.push_back('"');
    } else if(*codepoint == 47) { // /
      json_string.push_back('\\');
      json_string.push_back('/');
    } else if(*codepoint == 92) {
      json_string.push_back('\\');
      json_string.push_back('\\');
    } else if(*codepoint < 32 || *codepoint == 127 || (*codepoint >= 128 && *codepoint <= 159)) {
      json_string += json_encode_control_char(*codepoint);
    } else {
      json_string += utf8_encode(*codepoint);
    }
  }
  return json_string;
}


} // namespace json_utf8
} // namespace gol
