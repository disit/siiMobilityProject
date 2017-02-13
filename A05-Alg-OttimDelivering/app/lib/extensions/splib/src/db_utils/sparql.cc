
// curlpp
#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include "curlpp/Exception.hpp"

#include "sparql.h"

namespace gol { 

sparql_query_handler::sparql_query_handler(const std::string& query, const std::string& url)
    : _query(query),
      _url(url) {}

std::size_t write_callback(void* contents, std::size_t size, std::size_t nmemb, void* userp) {

  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;

}

std::string sparql_query_handler::retrieve() {
    
  CURL* curl = curl_easy_init();
  std::string readBuffer = "";
  if (curl) {
    // convert string query to URL format
    char* parameters = curl_easy_escape(curl, _query.c_str(), (int)_query.size());

    std::string query = "query=";
    std::string tothis(parameters);
    std::string buffer1 = query + tothis;

    std::string format = "format=";
    std::string value = "text/tab-separated-values";
    char* form = curl_easy_escape(curl, value.c_str(), (int)value.size());
    std::string second(form );
    std::string buffer2 = format + value;

    std::string buffer = buffer1 + "&" + buffer2;

    curl_free(parameters);
    curl_free(form);

    // launch query and retrieve informations in form of an xml structure
    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode result = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return readBuffer;

}


}  // namespace gol
