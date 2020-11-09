#include "parsegen_string.hpp"

namespace parsegen {

std::string escape_for_c_string(std::string const& s)
{
  std::string escaped;
  for (auto c : s) {
    switch (c) {
      case '\n':
        escaped.append("\\n");
        break;
      case '\t':
        escaped.append("\\t");
        break;
      case '\r':
        escaped.append("\\r");
        break;
      case '\\':
        escaped.append("\\");
        break;
      default:
        escaped.push_back(c);
    }
  }
  return escaped;
}

}
