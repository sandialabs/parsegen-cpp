#pragma once

#include <stack>

#include "parsegen_grammar.hpp"
#include "parsegen_table.hpp"

namespace parsegen {

struct action {
  enum class kind {
    none,
    shift,
    reduce,
    skip
  };
  parsegen::action::kind kind;
  union {
    int production;
    int next_state;
  };
};

struct shift_reduce_tables {
  grammar_ptr grammar;
  /* (state x terminal) -> action */
  table<action> terminal_table;
  /* (state x non-terminal) -> new state */
  table<int> nonterminal_table;
  shift_reduce_tables() = default;
  shift_reduce_tables(grammar_ptr g, int nstates_reserve);
};

int add_state(shift_reduce_tables& p);
int get_nstates(shift_reduce_tables const& p);
void add_terminal_action(shift_reduce_tables& p, int state, int terminal, action action);
void add_nonterminal_action(
    shift_reduce_tables& p, int state, int nonterminal, int next_state);
action const& get_action(shift_reduce_tables const& p, int state, int terminal);
int execute_action(
    shift_reduce_tables const& p, std::vector<int>& stack, action const& action);
grammar_ptr const& get_grammar(shift_reduce_tables const& p);

}  // namespace parsegen
