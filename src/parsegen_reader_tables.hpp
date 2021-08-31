#pragma once

#include <memory>

#include "parsegen_finite_automaton.hpp"
#include "parsegen_shift_reduce_tables.hpp"

namespace parsegen {

struct indentation {
  bool is_sensitive;
  int indent_token;
  int dedent_token;
  int newline_token;
};

struct reader_tables {
  shift_reduce_tables parser;
  finite_automaton lexer;
  indentation indent_info;
};

using reader_tables_ptr = std::shared_ptr<reader_tables const>;

}  // namespace parsegen
