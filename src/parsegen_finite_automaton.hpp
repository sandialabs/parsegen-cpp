#ifndef PARSEGEN_FINITE_AUTOMATON_HPP
#define PARSEGEN_FINITE_AUTOMATON_HPP

#include "parsegen_table.hpp"
#include <iosfwd>
#include <set>

namespace parsegen {

/* This is basically a weird mix between a DFA and
   an NFA-epsilon. It is really a DFA that can have two extra
   epsilon symbols that it accepts transitions with.
   We can simulate epsilon-transitions to multiple new
   states by making trees of nodes connected by
   epsilon-transitions.

   by convention, the start state is state 0
 */
struct finite_automaton {
  parsegen::table<int> table;
  std::vector<int> accepted_tokens;
  bool is_deterministic;
  finite_automaton() = default;
  finite_automaton(
      int nsymbols_init, bool is_deterministic_init, int nstates_reserve);
  static finite_automaton make_single_nfa(
      int nsymbols, int symbol, int token = 0);
  static finite_automaton make_set_nfa(
      int nsymbols, std::set<int> const& accepted, int token = 0);
  static finite_automaton make_range_nfa(
      int nsymbols, int range_start, int range_end, int token = 0);
  static finite_automaton unite(
      finite_automaton const& a, finite_automaton const& b);
  static finite_automaton concat(
      finite_automaton const& a, finite_automaton const& b, int token = 0);
  static finite_automaton plus(finite_automaton const& a, int token = 0);
  static finite_automaton maybe(finite_automaton const& a, int token = 0);
  static finite_automaton star(finite_automaton const& a, int token = 0);
  static finite_automaton make_deterministic(finite_automaton const& nfa);
  static finite_automaton simplify_once(finite_automaton const& fa);
  static finite_automaton simplify(finite_automaton const& fa);
};

int get_nstates(finite_automaton const& fa);
int get_nsymbols(finite_automaton const& fa);
bool get_determinism(finite_automaton const& fa);
int get_epsilon0(finite_automaton const& fa);
int get_epsilon1(finite_automaton const& fa);
int add_state(finite_automaton& fa);
void add_transition(
    finite_automaton& fa, int from_state, int at_symbol, int to_state);
void add_accept(finite_automaton& fa, int state, int token);
void remove_accept(finite_automaton& fa, int state);
int step(finite_automaton const& fa, int state, int symbol);
int accepts(finite_automaton const& fa, int state);
int get_nsymbols_eps(finite_automaton const& fa);
void append_states(finite_automaton& fa, finite_automaton const& other);
void negate_acceptance(finite_automaton& fa);
finite_automaton add_death_state(finite_automaton const& a);

finite_automaton make_char_nfa(bool is_deterministic_init, int nstates_reserve);
void add_char_transition(
    finite_automaton& fa, int from_state, char at_char, int to_state);
bool is_symbol(char c);
int get_symbol(char c);
char get_char(int symbol);
finite_automaton make_char_set_nfa(
    std::set<char> const& accepted, int token = 0);
finite_automaton make_char_range_nfa(
    char range_start, char range_end, int token = 0);
finite_automaton make_char_single_nfa(char symbol_char, int token = 0);
std::set<char> negate_set(std::set<char> const& s);

std::ostream& operator<<(std::ostream& os, finite_automaton const& fa);

}  // namespace parsegen

#endif
