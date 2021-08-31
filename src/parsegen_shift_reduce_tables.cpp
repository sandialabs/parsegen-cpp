#include "parsegen_shift_reduce_tables.hpp"

namespace parsegen {

shift_reduce_tables::shift_reduce_tables(grammar_ptr g, int nstates_reserve)
    : grammar(g),
      terminal_table(g->nterminals, nstates_reserve),
      nonterminal_table(get_nnonterminals(*g), nstates_reserve) {}

int get_nstates(shift_reduce_tables const& p) { return get_nrows(p.terminal_table); }

int add_state(shift_reduce_tables& p) {
  auto state = get_nstates(p);
  resize(p.terminal_table, state + 1, get_ncols(p.terminal_table));
  resize(p.nonterminal_table, state + 1, get_ncols(p.nonterminal_table));
  for (int t = 0; t < p.grammar->nterminals; ++t) {
    action action;
    action.kind = action::kind::none;
    at(p.terminal_table, state, t) = action;
  }
  for (int nt = 0; nt < get_nnonterminals(*(p.grammar)); ++nt) {
    at(p.nonterminal_table, state, nt) = -1;
  }
  return state;
}

void add_terminal_action(shift_reduce_tables& p, int state, int terminal, action action) {
  assert(at(p.terminal_table, state, terminal).kind == action::kind::none);
  assert(action.kind != action::kind::none);
  if (action.kind == action::kind::shift) {
    assert(0 <= action.next_state);
    assert(action.next_state < get_nstates(p));
  } else {
    assert(0 <= action.production);
    assert(action.production < size(p.grammar->productions));
  }
  at(p.terminal_table, state, terminal) = action;
}

void add_nonterminal_action(
    shift_reduce_tables& p, int state, int nonterminal, int next_state) {
  assert(0 <= next_state);
  assert(next_state < get_nstates(p));
  assert(at(p.nonterminal_table, state, nonterminal) == -1);
  at(p.nonterminal_table, state, nonterminal) = next_state;
}

action const& get_action(shift_reduce_tables const& p, int state, int terminal) {
  return at(p.terminal_table, state, terminal);
}

int execute_action(
    shift_reduce_tables const& p, std::vector<int>& stack, action const& action) {
  assert(action.kind != action::kind::none);
  if (action.kind == action::kind::shift) {
    stack.push_back(action.next_state);
  } else {
    auto& prod = at(p.grammar->productions, action.production);
    resize(stack, size(stack) - size(prod.rhs));
    assert(p.grammar.get());
    auto& grammar = *(p.grammar);
    auto nt = as_nonterminal(grammar, prod.lhs);
    assert(!stack.empty());
    auto next_state = at(p.nonterminal_table, stack.back(), nt);
    stack.push_back(next_state);
  }
  return stack.back();
}

grammar_ptr const& get_grammar(shift_reduce_tables const& p) { return p.grammar; }

}  // end namespace parsegen
