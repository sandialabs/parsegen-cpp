#pragma once

#include <stdexcept>

namespace parsegen {

class parse_error : public std::invalid_argument {
 public:
  parse_error(const std::string& msg);
  virtual void out_of_line_virtual_method();
};

}
