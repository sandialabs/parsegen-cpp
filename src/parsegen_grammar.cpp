#include "parsegen_grammar.hpp"

#include <iostream>
#include <set>

#include "parsegen_std_vector.hpp"

namespace parsegen {

int get_nnonterminals(grammar const& g) { return g.nsymbols - g.nterminals; }

bool is_terminal(grammar const& g, int symbol) {
  assert(0 <= symbol);
  assert(symbol <= g.nsymbols);
  return symbol < g.nterminals;
}

bool is_nonterminal(grammar const& g, int symbol) {
  return !is_terminal(g, symbol);
}

int as_nonterminal(grammar const& g, int symbol) {
  return symbol - g.nterminals;
}

int find_goal_symbol(grammar const& g) {
  std::set<int> nonterminals_in_rhss;
  for (auto& p : g.productions) {
    for (auto s : p.rhs) {
      assert(0 <= s);
      if (is_nonterminal(g, s)) nonterminals_in_rhss.insert(s);
    }
  }
  int result = -1;
  for (int s = g.nterminals; s < g.nsymbols; ++s)
    if (!nonterminals_in_rhss.count(s)) {
      if (result != -1) {
        std::cerr << "ERROR: there is more than one root nonterminal (";
        std::cerr << at(g.symbol_names, result) << " and "
                  << at(g.symbol_names, s) << ") in this grammar\n";
        abort();
      }
      result = s;
    }
  if (result == -1) {
    std::cerr << "ERROR: the root nonterminal is unclear for this grammar\n";
    abort();
  }
  return result;
}

void add_end_terminal(grammar& g) {
  for (auto& prod : g.productions) {
    if (is_nonterminal(g, prod.lhs)) prod.lhs++;
    for (auto& rhs_symb : prod.rhs) {
      if (is_nonterminal(g, rhs_symb)) rhs_symb++;
    }
  }
  g.symbol_names.insert(g.symbol_names.begin() + g.nterminals, "EOF");
  g.nterminals++;
  g.nsymbols++;
}

int get_end_terminal(grammar const& g) { return g.nterminals - 1; }

void add_accept_production(grammar& g) {
  auto goal_symbol = find_goal_symbol(g);
  grammar::production p;
  p.lhs = g.nsymbols;
  p.rhs = {goal_symbol};
  g.productions.push_back(p);
  g.symbol_names.push_back("ACCEPT");
  g.nsymbols++;
}

int get_accept_production(grammar const& g) { return isize(g.productions) - 1; }

int get_accept_nonterminal(grammar const& g) { return g.nsymbols - 1; }

std::ostream& operator<<(std::ostream& os, grammar const& g) {
  os << "symbols:\n";
  for (int i = 0; i < isize(g.symbol_names); ++i) {
    os << i << ": " << at(g.symbol_names, i) << "\n";
  }
  os << "productions:\n";
  for (int i = 0; i < isize(g.productions); ++i) {
    auto& prod = at(g.productions, i);
    os << i << ": " << prod.lhs << " ::=";
    for (auto& symb : prod.rhs) {
      os << ' ' << symb;
    }
    os << '\n';
  }
  os << '\n';
  return os;
}

}  // namespace parsegen
