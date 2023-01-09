#include "parsegen_string.hpp"

#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace parsegen {

std::string escape(std::string const& in)
{
  std::string out;
  for (char c : in) {
    if (c == '\n') {
      out += "\\n";
    } else if (c == '\r') {
      out += "\\r";
    } else if (c == '\t') {
      out += "\\t";
    } else if (c == '"') {
      out += "\\\"";
    } else if (c == '\'') {
      out += "\\'";
    } else {
      out.push_back(c);
    }
  }
  return out;
}

std::string unescape(std::string const& in)
{
  bool is_escaped = false;
  std::string out;
  for (char c : in) {
    if (is_escaped) {
      if (c == 'n') {
        out.push_back('\n');
      } else if (c == 'r') {
        out.push_back('\r');
      } else if (c == 't') {
        out.push_back('\t');
      } else {
        out.push_back(c);
      }
      is_escaped = false;
    } else {
      if (c == '\\') {
        is_escaped = true;
      } else if (c == '"') {
      } else {
        out.push_back(c);
      }
    }
  }
  return out;
}

std::string double_quote(std::string const& s)
{
  return "\"" + escape(s) + "\"";
}

std::string single_quote(std::string const& s)
{
  return "'" + escape(s) + "'";
}

std::string unquote(std::string const& s)
{
  if (s.length() < 2) {
    throw std::runtime_error("parsegen::unquote given a string shorter than two quotes");
  }
  return unescape(s.substr(1, s.length() - 2));
}

std::string lowercase(std::string const& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
  [] (unsigned char c) {
    return std::tolower(c);
  });
  return result;
}

std::string uppercase(std::string const& s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
  [] (unsigned char c) {
    return std::toupper(c);
  });
  return result;
}

}
