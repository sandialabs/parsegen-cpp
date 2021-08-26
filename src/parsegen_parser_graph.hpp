#ifndef PARSEGEN_GRAPH_HPP
#define PARSEGEN_GRAPH_HPP

#include <iosfwd>
#include <vector>

namespace parsegen {

using node_edges = std::vector<int>;
using parser_graph = std::vector<node_edges>;

parser_graph make_graph_with_nnodes(int nnodes);
int get_nnodes(parser_graph const& g);
void add_edge(parser_graph& g, int i, int j);
node_edges const& get_edges(parser_graph const& g, int i);
node_edges& get_edges(parser_graph& g, int i);
parser_graph make_transpose(parser_graph const& g);
int at(parser_graph const& g, int i, int j);
std::ostream& operator<<(std::ostream& os, parser_graph const& g);

}  // namespace parsegen

#endif
