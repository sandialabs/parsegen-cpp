#pragma once

#include <string>
#include <cassert>

namespace parsegen {

/* some wrappers over std::string to let us
   do all indexing with int */

inline int isize(std::string const& s) { return int(s.size()); }

inline typename std::string::reference at(std::string& s, int i) {
  assert(0 <= i);
  assert(i < int(s.size()));
  return s[std::size_t(i)];
}

inline typename std::string::const_reference at(std::string const& s, int i) {
  assert(0 <= i);
  assert(i < int(s.size()));
  return s[std::size_t(i)];
}

std::string escape(std::string const& s);
std::string unescape(std::string const& s);
std::string double_quote(std::string const& s);
std::string single_quote(std::string const& s);
std::string unquote(std::string const& s);
std::string lowercase(std::string const& s);
std::string uppercase(std::string const& s);

}  // namespace parsegen
