#ifndef OMEGA_H_READER_TABLES_HPP
#define OMEGA_H_READER_TABLES_HPP

#include <memory>

#include "parsegen_finite_automaton.hpp"
#include "parsegen_parser.hpp"

namespace parsegen {

struct IndentInfo {
  bool is_sensitive;
  int indent_token;
  int dedent_token;
  int newline_token;
};

struct reader_tables {
  Parser parser;
  FiniteAutomaton lexer;
  IndentInfo indent_info;
};

using reader_tablesPtr = std::shared_ptr<reader_tables const>;

}  // namespace parsegen

#endif
