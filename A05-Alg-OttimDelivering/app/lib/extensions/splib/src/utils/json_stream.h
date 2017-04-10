#ifndef GOL_UTILS_JSON_OFS_H_
#define GOL_UTILS_JSON_OFS_H_

// std
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <limits>
#include <type_traits>
// boost
#include <boost/variant.hpp>

#include "json_utf8.h"

namespace gol {

template<typename T> 
 struct is_container : std::false_type {};
template<typename T> 
 struct is_associative_container : std::false_type {};
// vector overload
template<typename T>  
 struct is_container< std::vector<T> > : std::true_type {};
// map overload 
template<typename K, typename V>
 struct is_container< std::map<K, V> > : std::true_type {}; 
template<typename... Args>
 struct is_associative_container< 
     std::map <std::string, boost::variant<Args... > > > : std::true_type {};

class jofstream {
 public:
  jofstream(const std::string& path) : _stream(path.c_str()) {}

  template<typename T> 
  jofstream& operator<<(const T& data) {
    if (_stream.is_open()) {
      _stream.precision(7);
      _stream << dump(data);
    }
    return *this;
  }

 private:
  // forward standard manipulator like std::endl
  typedef std::ostream& (*ostream_manipulator)(std::ostream&);
  
  jofstream& operator<<(ostream_manipulator pf) {
    if (_stream.is_open()) {
      _stream << pf;
      _stream.precision(7);
    }
    return *this;
  }

  // dispatch to actual dump method
  template<typename T> 
  std::string dump(const T& t) const {
    return dump_value_or_container(t, typename is_container<T>::type());
  }

  // dispatch to correct dump method
  template<typename T> 
  std::string dump_value_or_container(const T& t, std::false_type) const {
    return dump_value(t);
  }

  template<typename T> std::string dump_value_or_container(const T& t, std::true_type) const {
    return dump_simple_or_associative_container(t, typename is_associative_container<T>::type());
  }

  template<typename T> 
  std::string dump_simple_or_associative_container(
      const T& t, std::false_type) const {
    return dump_simple_container(t);
  }

  template<typename T> 
  std::string dump_simple_or_associative_container(
      const T& t, std::true_type) const {
    return dump_associative_container(t);
  }

  // implement type specific serialization
  template<typename V> 
  std::string dump_value(const V& value) const {
    std::ostringstream oss;
    oss.precision(7);
    oss << transcode_rfc4627(value);
    return oss.str();
  }

  std::string dump_value(const std::string& value) const {
    return "\"" + transcode_rfc4627(value) + "\"";
  }

  template<typename K, typename V> 
  std::string dump_value(const std::pair<const K, V>& pair) const {
    std::ostringstream oss; 
    oss.precision(7);
    oss << "[" << dump(pair.first) << ", " << dump(pair.second) << "]";
    return oss.str();
  }

  template<typename... Args> 
  std::string dump_pair(const std::pair<const std::string, boost::variant<Args... > >& pair) const {
    std::ostringstream oss;
    oss << dump(pair.first) << ": "<< boost::apply_visitor(
      visitor (*this), pair.second);
    return oss.str();
  }

  template<typename C> 
  std::string dump_simple_container(const C& container) const {
    std::ostringstream oss;
    oss.precision(7);
    typename C::const_iterator it = container.begin();
    oss << "[" << dump(*it);
    for (++ it ; it != container.end() ; ++ it) {
      oss << ", " << dump(*it);
    }
    oss << "]";
    return oss.str();
  }

  template<typename M> 
  std::string dump_associative_container(const M& map) const {
    std::ostringstream oss;
    oss.precision(7);
    typename M::const_iterator it = map.begin();
    oss << "{" << dump_pair(*it);
    for (++ it ; it != map.end() ; ++ it) {
      oss << ", " << dump_pair(*it);
    }
    oss << "}";
    return oss.str();
  }

  template<typename T> 
  T transcode_rfc4627(const T& t) const {
    return t;
  }

  template <typename T> 
  int sgn(const T&  val) const {
    return (T(0) < val) - (val < T(0));
  }

  double transcode_rfc4627(const double d) const {
    using namespace std;
    if(isfinite(d)) {
      return d;
    } else {
      if(std::isinf(d)) {
        return sgn(d) * std::numeric_limits<double>::max();
      }
      return 0.;
    }
  }

  double transcode_rfc4627(const float f) const {
    return transcode_rfc4627(static_cast<double>(f));
  }

  std::string transcode_rfc4627(std::string const& input) const {
    return json_utf8::json_encode_codepoints(json_utf8::decode_utf8(input));
  }

 private :
  std::ofstream _stream;
  // friend
  class visitor : public boost::static_visitor<std::string> {
   public:
    visitor(const jofstream& parent): _parent(parent) {}

    template <typename T>
    std::string operator()( const T& operand ) const {
      return (_parent).dump(operand);
    }
  
   private:
    const jofstream& _parent;
  };

};

} // namespace gol

#endif // GOL_UTILS_JSON_OFS_H_

