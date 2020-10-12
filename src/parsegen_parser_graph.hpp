#ifndef PARSEGEN_GRAPH_HPP
#define PARSEGEN_GRAPH_HPP

#include <iosfwd>
#include <vector>

namespace parsegen {

using node_edges = std::vector<int>;
using parserGraph = std::vector<node_edges>;

parserGraph make_graph_with_nnodes(int nnodes);
int get_nnodes(parserGraph const& g);
void add_edge(parserGraph& g, int i, int j);
node_edges const& get_edges(parserGraph const& g, int i);
node_edges& get_edges(parserGraph& g, int i);
parserGraph make_transpose(parserGraph const& g);
int at(parserGraph const& g, int i, int j);
std::ostream& operator<<(std::ostream& os, parserGraph const& g);

}  // namespace parsegen

#endif
