#ifndef PARSEGEN_PARSER_HPP
#define PARSEGEN_PARSER_HPP

#include <stack>
#include <stdexcept>

#include "parsegen_grammar.hpp"
#include "parsegen_table.hpp"

namespace parsegen {

enum actionKind {
  ACTION_NONE,
  ACTION_SHIFT,
  ACTION_REDUCE,
};

struct action {
  actionKind kind;
  union {
    int production;
    int next_state;
  };
};

struct parser {
  grammar_ptr grammar;
  /* (state x terminal) -> action */
  table<action> terminal_table;
  /* (state x non-terminal) -> new state */
  table<int> nonterminal_table;
  parser() = default;
  parser(grammar_ptr g, int nstates_reserve);
};

int add_state(parser& p);
int get_nstates(parser const& p);
void add_terminal_action(parser& p, int state, int terminal, action action);
void add_nonterminal_action(
    parser& p, int state, int nonterminal, int next_state);
action const& get_action(parser const& p, int state, int terminal);
int execute_action(
    parser const& p, std::vector<int>& stack, action const& action);
grammar_ptr const& get_grammar(parser const& p);

class parse_error : public std::invalid_argument {
 public:
  parse_error(const std::string& msg);
  virtual void out_of_line_virtual_method();
};

}  // namespace parsegen

#endif
