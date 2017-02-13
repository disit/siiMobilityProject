
#ifndef GOL_EXCEPTION_H_
#define GOL_EXCEPTION_H_

 // std
#include <string>
#include <exception>

namespace gol {

class exception_t : public std::exception { 
 public:
  exception_t() throw() 
      : std::exception(), 
        _message() {}
  explicit exception_t(std::string msg) throw()
      : std::exception(), 
        _message(msg) {}
  ~exception_t() throw() {}

  const char* what() const throw() { 
    return _message.c_str(); 
  }
  
  virtual int code() const { return 101; }

 protected:
  std::string _message;

};

struct data_exception : public exception_t { 
  data_exception(std::string msg) throw() 
      : exception_t(msg) {}
  
  int code() const { return 107; }

};

struct runtime_exception : public exception_t { 
  runtime_exception(std::string msg) throw() 
      : exception_t(msg) {}
    
  int code() const { return 108; }

};

struct parser_exception : public exception_t {
  parser_exception() 
      : exception_t() {}
  
  explicit parser_exception(std::string msg) 
      : exception_t(msg) {}

};

struct builder_exception : public exception_t {
  builder_exception() 
      : exception_t() {}
  
  explicit builder_exception(std::string msg) 
      : exception_t(msg) {}

};

struct solver_exception : public exception_t {
  solver_exception() 
      : exception_t() {}
  
  explicit solver_exception(std::string msg) 
      : exception_t(msg) {}

};

// the search algorithm has found the target node 
struct target_found : public exception_t {     
  int code() const { return 103; }

};

// the search algorithm has found the target node 
struct all_targets_found : public exception_t {     
  int code() const { return 104; }

};

// the search algorithm was not able to find the target node 
struct target_not_found : public exception_t { 
  const char* what() const throw() { return "Target Not Found"; }
  int code() const { return 105; }

};


}  // namespace gol

#endif  // GOL_EXCEPTION_H_
