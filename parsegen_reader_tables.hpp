#ifndef PARSEGEN_READER_TABLES_HPP
#define PARSEGEN_READER_TABLES_HPP

#include <memory>

#include "parsegen_finite_automaton.hpp"
#include "parsegen_parser.hpp"

namespace parsegen {

struct indentation {
  bool is_sensitive;
  int indent_token;
  int dedent_token;
  int newline_token;
};

struct reader_tables {
  parser parser;
  finite_automaton lexer;
  indentation indent_info;
};

using reader_tables_ptr = std::shared_ptr<reader_tables const>;

}  // namespace parsegen

#endif
