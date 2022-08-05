#pragma once

#include <string>

namespace parsegen {

inline std::string extract_quoted_string(std::string const& s)
{
  std::string result;
  bool in_string = false;
  bool escaped = false;
  for (char c : s) {
    if (in_string) {
      if (escaped) {
        if (c == 'n') result.push_back('\n');
        else if (c == 't') result.push_back('\t');
        else result.push_back(c);
        escaped = false;
      } else {
        if (c == '\\') escaped = true;
        else if (c == '"') break;
        else if (c == '\'') break;
        else result.push_back(c);
      }
    } else {
      if (c == '"') in_string = true;
      else if (c == '\'') in_string = true;
    }
  }
  return result;
}

}
