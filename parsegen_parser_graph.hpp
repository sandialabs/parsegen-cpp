#ifndef OMEGA_H_GRAPH_HPP
#define OMEGA_H_GRAPH_HPP

#include <iosfwd>
#include <vector>

namespace parsegen {

using NodeEdges = std::vector<int>;
using parserGraph = std::vector<NodeEdges>;

parserGraph make_graph_with_nnodes(int nnodes);
int get_nnodes(parserGraph const& g);
void add_edge(parserGraph& g, int i, int j);
NodeEdges const& get_edges(parserGraph const& g, int i);
NodeEdges& get_edges(parserGraph& g, int i);
parserGraph make_transpose(parserGraph const& g);
int at(parserGraph const& g, int i, int j);
std::ostream& operator<<(std::ostream& os, parserGraph const& g);

}  // namespace parsegen

#endif
