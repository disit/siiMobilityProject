
#ifndef __SPARQL_H__
#define __SPARQL_H__

// std
#include <string>
#include <sstream>
#include <fstream>    // std::fstream
#include <iterator>   // std::istreambuf_iterator


namespace gol {

class sparql_query_handler {
 public:
  sparql_query_handler(const std::string&, const std::string&);
  ~sparql_query_handler() {}

  std::string retrieve();

 private:
  std::string _query; // sparql query
  std::string _url;   // url sparql endpoint
};

}  // namespace gol

#endif // end SPARQL_H_
