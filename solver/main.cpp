#include <cstdlib>
#include <iostream>

#include "AdjacencyList.hpp"

enum GraphSample {
  // To be implemented
  DataSet1,
  DataSet2,
  DataSet3,

  // Randomly generated graphs
  RandomGraph
};
GraphSample graphToUse = RandomGraph;
int randomGraphCount = 50;

int main() {
  AdjacencyList graph;

  switch (graphToUse) {
  case RandomGraph:
    for (int i = 0; i < randomGraphCount; i++) {
      auto edge1 = std::rand() % randomGraphCount;
      auto edge2 = std::rand() % randomGraphCount;

      // Undirected graph -> directed
      graph.insertEdge(edge1, edge2);
      graph.insertEdge(edge2, edge1);
    }
    break;
  default:
    break;
  }

  graph.printGraph(true);
}