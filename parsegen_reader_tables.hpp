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

struct readerTables {
  Parser parser;
  FiniteAutomaton lexer;
  IndentInfo indent_info;
};

using readerTablesPtr = std::shared_ptr<readerTables const>;

}  // namespace parsegen

#endif
