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

struct parser_tables {
  shift_reduce_tables syntax_tables;
  finite_automaton lexical_tables;
  indentation indent_info;
};

using parser_tables_ptr = std::shared_ptr<parser_tables const>;

}  // namespace parsegen
