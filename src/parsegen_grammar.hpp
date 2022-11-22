#ifndef PARSEGEN_GRAMMAR_HPP
#define PARSEGEN_GRAMMAR_HPP

#include <memory>
#include <string>
#include <vector>

namespace parsegen {

/* convention: symbols are numbered with all
   terminal symbols first, all non-terminal symbols after */

struct grammar {
  using right_hand_side = std::vector<int>;
  struct production {
    int lhs;
    right_hand_side rhs;
  };
  using production_vector = std::vector<production>;
  int nsymbols;
  int nterminals;
  production_vector productions;
  std::vector<std::string> symbol_names;
  std::vector<int> ignored_terminals;
};

using grammar_ptr = std::shared_ptr<grammar const>;

int get_nnonterminals(grammar const& g);
bool is_terminal(grammar const& g, int symbol);
bool is_nonterminal(grammar const& g, int symbol);
int as_nonterminal(grammar const& g, int symbol);
int find_goal_symbol(grammar const& g);
void add_end_terminal(grammar& g);
int get_end_terminal(grammar const& g);
void add_accept_production(grammar& g);
int get_accept_production(grammar const& g);
int get_accept_nonterminal(grammar const& g);

std::ostream& operator<<(std::ostream& os, grammar const& g);

}  // namespace parsegen

#endif
