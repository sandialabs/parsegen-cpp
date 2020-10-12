#ifndef OMEGA_H_BUILD_PARSER_HPP
#define OMEGA_H_BUILD_PARSER_HPP

#include <memory>
#include <set>

#include "parsegen_parser.hpp"
#include "parsegen_parser_graph.hpp"

namespace parsegen {

struct Config {
  int production;
  int dot;
};

using Configs = std::vector<Config>;

using Context = std::set<int>;

/* nonterminal transitions will be stored as SHIFT
   actions while in progress */
struct action_in_progress {
  Action action;
  Context context;
};

struct state_in_progress {
  std::vector<int> configs;
  std::vector<action_in_progress> actions;
};

using StatesInProgress = std::vector<std::unique_ptr<state_in_progress>>;

struct state_configuration {
  int state;
  int config_in_state;
};

using state_configurations = std::vector<state_configuration>;

struct ParserInProgress {
  StatesInProgress states;
  Configs configs;
  state_configurations state_configs;
  ParserGraph states2state_configs;
  GrammarPtr grammar;
};

state_configurations form_state_configs(StatesInProgress const& states);
ParserGraph form_states_to_state_configs(
    state_configurations const& scs, StatesInProgress const& states);

void print_dot(std::string const& filepath, ParserInProgress const& pip);

ParserInProgress build_lalr1_parser(GrammarPtr grammar, bool verbose = false);

Parser accept_parser(ParserInProgress const& pip);

}  // namespace parsegen

#endif
