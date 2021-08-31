#include "parsegen_error.hpp"

namespace parsegen {

parse_error::parse_error(const std::string& msg) : std::invalid_argument(msg) {}

void parse_error::out_of_line_virtual_method() {}

}
