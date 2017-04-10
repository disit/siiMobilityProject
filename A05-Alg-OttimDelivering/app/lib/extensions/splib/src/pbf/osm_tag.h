/*
Copyright (c) 2012, Canal TP
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Canal TP nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL CANAL TP BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
