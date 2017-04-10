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

#ifndef GOL_PBF_OSM_READER_H_
#define GOL_PBF_OSM_READER_H_

// std
#include <stdint.h>
#include <netinet/in.h>
#include <zlib.h>
#include <string>
#include <fstream>
#include <iostream>
// osmpbf
#include <osmpbf/fileformat.pb.h> // low-level blob storage
#include <osmpbf/osmformat.pb.h>  // high-level OSM object

#include "osm_tag.h"       // <osmpbf/osmformat.pb.h>
#include "osm_reference.h" // <osmpbf/osmformat.pb.h>

#include "../config.h"
#include "../utils/logger.h"

namespace gol { 
  namespace pbf {

// resolution for longitude/latitude used for conversion 
// between representation as double and as int
const int lonlat_resolution          = 1000 * 1000 * 1000; 
const int max_blob_header_size       = 64 * 1024;         // 64 kB
const int max_uncompressed_blob_size = 32 * 1024 * 1024;  // 32 MB

template<typename ContentHandler>
class osm_reader {
 public:
  osm_reader(const std::string& filename, ContentHandler& content_handler)
      : content_handler(content_handler), 
        file(filename.c_str(), 
        std::ios::binary), 
        finished(false), 
        _filename(filename) 
  {
    if (!file.is_open())
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to open the file " 
        << filename;
    
    buffer        = new char[max_uncompressed_blob_size];
    unpack_buffer = new char[max_uncompressed_blob_size];
    
    boost::filesystem::path root = 
        boost::filesystem::current_path() / 
        boost::filesystem::path(RELATIVE_DIR);
    logger(logINFO) 
      << left("[parserPBF]", 14) 
      << left(">", 3) 
      << ((boost::filesystem::path(filename).generic_string()).
            substr((root.generic_string()).length()));
  }

  ~osm_reader() 
  {
    delete[] buffer;
    delete[] unpack_buffer;  
    google::protobuf::ShutdownProtobufLibrary();
  }

  void reset() 
  {
    file.close();
    file.clear();
    delete[] buffer;
    delete[] unpack_buffer;
    google::protobuf::ShutdownProtobufLibrary();

    buffer        = new char[max_uncompressed_blob_size];
    unpack_buffer = new char[max_uncompressed_blob_size];
  
    file.open(_filename.c_str(), std::ios::binary);
    if (!file.is_open())
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to reset the parser > " 
        << _filename; 
    this->finished = false;     
  }    

  void parse() {
    while (!this->file.eof() && !finished) 
    {
      OSMPBF::BlobHeader header = this->read_header();
      if (!this->finished) {
        int32_t sz = this->read_blob(header);
        if (header.type() == "OSMData") {
          this->parse_primitiveblock(sz);
        } else if (header.type() == "OSMHeader") {
          // nothing
        } else {
          logger(logWARNING) 
            << left("[parserPBF]", 14) 
            << "Unknown blob type: " 
            << header.type();
        }
      }
    }
  }

 private:
  ContentHandler& content_handler;
  std::ifstream file;
  char* buffer;
  char* unpack_buffer;
  bool  finished;
  std::string _filename;

  OSMPBF::BlobHeader read_header() 
  {
    int32_t sz;
    OSMPBF::BlobHeader result;

    // read the first 4 bytes of the file, this is the size of the blob-header
    if ( !file.read((char*)&sz, 4) ) {
      //logger(logINFO) 
      //  << left("[parserPBF]", 14) 
      //  << "we finished reading the file ";
      this->finished = true;
      return result;
    }
    sz = ntohl(sz); // convert the size from network byte-order to host byte-order
    if (sz > max_blob_header_size)
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Blob-header-size is bigger then allowed " 
        << sz << " > " << max_blob_header_size;
    this->file.read(this->buffer, sz);
    if (!this->file.good())
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to read blob-header from file";
    // parse the blob-header from the read-buffer
    if (!result.ParseFromArray(this->buffer, sz))
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to parse blob header";
    
    return result;
  }

  int32_t read_blob(const OSMPBF::BlobHeader & header)
  {
    OSMPBF::Blob blob;
    // size of the following blob
    int32_t sz = header.datasize();
    if (sz > max_uncompressed_blob_size)
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Blob-size is bigger then allowed";
    if (!this->file.read(buffer, sz))
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to read blob from file";
    if (!blob.ParseFromArray(this->buffer, sz))
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to parse blob";

    // if the blob has uncompressed data
    if (blob.has_raw()) {
      sz = blob.raw().size(); // size of the blob-data
      // check that raw_size is set correctly
      if (sz != blob.raw_size())
        logger(logWARNING) 
          << left("[parserPBF]", 14) 
          << "Reports wrong raw_size: " 
          << blob.raw_size() << " bytes";
      memcpy(unpack_buffer, buffer, sz);
      return sz;
    }
    if (blob.has_zlib_data()) {
      sz = blob.zlib_data().size();
      z_stream z;
      z.next_in   = (unsigned char*) blob.zlib_data().c_str();
      z.avail_in  = sz;
      z.next_out  = (unsigned char*) unpack_buffer;
      z.avail_out = blob.raw_size();
      z.zalloc    = Z_NULL;
      z.zfree     = Z_NULL;
      z.opaque    = Z_NULL;

      if (inflateInit(&z) != Z_OK) {
        logger(logERROR) 
          << left("[parserPBF]", 14) 
          << "Failed to init zlib stream";
      }
      if (inflate(&z, Z_FINISH) != Z_STREAM_END) {
        logger(logERROR) 
          << left("[parserPBF]", 14) 
          << "Failed to inflate zlib stream";
      }
      if (inflateEnd(&z) != Z_OK) {
        logger(logERROR) 
          << left("[parserPBF]", 14) 
          << "Failed to deinit zlib stream";
      }
      return z.total_out;
    }
    if (blob.has_lzma_data()) {
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "lzma-decompression is not supported";
    }
    return 0;
  }

  void parse_primitiveblock(int32_t sz) 
  {
    OSMPBF::PrimitiveBlock primblock;
    if (!primblock.ParseFromArray(this->unpack_buffer, sz))
      logger(logERROR) 
        << left("[parserPBF]", 14) 
        << "Unable to parse primitive block";

    for (int i = 0, l = primblock.primitivegroup_size(); i < l; i++) {
      OSMPBF::PrimitiveGroup pg = primblock.primitivegroup(i);
      // Simple Nodes
      for (int i = 0; i < pg.nodes_size(); ++i) {
        OSMPBF::Node n = pg.nodes(i);
        double lon = 0.000000001 * 
          (primblock.lon_offset() + (primblock.granularity() * n.lon())) ;
        double lat = 0.000000001 * 
          (primblock.lat_offset() + (primblock.granularity() * n.lat())) ;
        content_handler.node_callback(n.id(), lon, lat, get_tags(n, primblock));
      }
      // Dense Nodes
      if (pg.has_dense()) {
        OSMPBF::DenseNodes dn = pg.dense();
        uint64_t id = 0;
        double lon  = 0;
        double lat  = 0;
        int current_kv = 0;
        for (int i = 0; i < dn.id_size(); ++i) {
          id += dn.id(i);
          lon +=  0.000000001 * 
            (primblock.lon_offset() + (primblock.granularity() * dn.lon(i)));
          lat +=  0.000000001 * 
            (primblock.lat_offset() + (primblock.granularity() * dn.lat(i)));

          tags_t tags;
          while (current_kv < dn.keys_vals_size() && dn.keys_vals(current_kv) != 0) {
            uint64_t key = dn.keys_vals(current_kv);
            uint64_t val = dn.keys_vals(current_kv + 1);
            std::string key_string = primblock.stringtable().s(key);
            std::string val_string = primblock.stringtable().s(val);
            current_kv += 2;
            tags[key_string] = val_string;
          }
          ++current_kv;
          content_handler.node_callback(id, lon, lat, tags);
        }
      }
      for (int i = 0; i < pg.ways_size(); ++i) {
        OSMPBF::Way w = pg.ways(i);
        uint64_t ref = 0;
        std::vector<uint64_t> refs;
        for (int j = 0; j < w.refs_size(); ++j) {
          ref += w.refs(j);
          refs.push_back(ref);
        }
        uint64_t id = w.id();
        content_handler.way_callback(id, get_tags(w, primblock), refs);
      }
      for (int i=0; i < pg.relations_size(); ++i) {
        OSMPBF::Relation rel = pg.relations(i);
        uint64_t id = 0;
        references_t refs;
        for (int l = 0; l < rel.memids_size(); ++l) {
          id += rel.memids(l);
          refs.push_back(
            reference_t(rel.types(l), id, primblock.stringtable().s(rel.roles_sid(l))));
        }
        content_handler.
          relation_callback(rel.id(), get_tags(rel, primblock), refs);
      }
    }
  }
  
};


} // namespace pbf
} // namespace gol

#endif // GOL_PBF_OSM_READER_H_
