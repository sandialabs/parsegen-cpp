#ifndef PARSEGEN_BUILD_PARSER_HPP
#define PARSEGEN_BUILD_PARSER_HPP

#include <memory>
#include <set>

#include "parsegen_parser.hpp"
#include "parsegen_parser_graph.hpp"

namespace parsegen {

struct configuration {
  int production;
  int dot;
};

using configurations = std::vector<configuration>;

using Context = std::set<int>;

/* nonterminal transitions will be stored as SHIFT
   actions while in progress */
struct action_in_progress {
  action action;
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

struct parser_in_progress {
  StatesInProgress states;
  configurations configs;
  state_configurations state_configs;
  parserGraph states2state_configs;
  grammarPtr grammar;
};

state_configurations form_state_configs(StatesInProgress const& states);
parserGraph form_states_to_state_configs(
    state_configurations const& scs, StatesInProgress const& states);

void print_dot(std::string const& filepath, parser_in_progress const& pip);

parser_in_progress build_lalr1_parser(grammarPtr grammar, bool verbose = false);

parser accept_parser(parser_in_progress const& pip);

}  // namespace parsegen

#endif
