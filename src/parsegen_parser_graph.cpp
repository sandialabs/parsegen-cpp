#include "parsegen_parser_graph.hpp"

#include <iostream>

#include "parsegen_std_vector.hpp"

namespace parsegen {

parser_graph make_graph_with_nnodes(int nnodes) {
  return parser_graph(std::size_t(nnodes));
}

int get_nnodes(parser_graph const& g) { return size(g); }

void add_edge(parser_graph& g, int i, int j) { at(g, i).push_back(j); }

node_edges const& get_edges(parser_graph const& g, int i) { return at(g, i); }

node_edges& get_edges(parser_graph& g, int i) { return at(g, i); }

parser_graph make_transpose(parser_graph const& g) {
  auto nnodes = get_nnodes(g);
  auto transpose = make_graph_with_nnodes(nnodes);
  for (int i = 0; i < nnodes; ++i) {
    for (auto j : get_edges(g, i)) {
      add_edge(transpose, j, i);
    }
  }
  return transpose;
}

int at(parser_graph const& g, int i, int j) { return at(at(g, i), j); }

std::ostream& operator<<(std::ostream& os, parser_graph const& g) {
  for (int i = 0; i < get_nnodes(g); ++i) {
    os << i << ":";
    for (auto j : get_edges(g, i)) os << " " << j;
    os << '\n';
  }
  return os;
}

}  // namespace parsegen
