#include "AdjacencyList.hpp"
#include "BellmanFord.cpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Bellman Ford Search", "[bf]") {
  AdjacencyList<int> graph;

  // insertion of edges without negative cycle
  graph.insertEdge(0, 1, -1);
  graph.insertEdge(0, 2, 4);
  graph.insertEdge(1, 2, 3);
  graph.insertEdge(1, 3, 2);
  graph.insertEdge(1, 4, 2);
  graph.insertEdge(3, 2, 5);
  graph.insertEdge(3, 1, 1);
  graph.insertEdge(4, 3, -3);

  REQUIRE(BellmanFord(graph, 0, 3) == -2);
}

TEST_CASE("Bellman Ford Negative Cycle", "[bf]") {
  AdjacencyList<int> graph;

  // insertion of edges without negative cycle
  graph.insertEdge(0, 1, 1);
  graph.insertEdge(1, 2, -1);
  graph.insertEdge(2, 3, -1);
  graph.insertEdge(3, 0, -1);

  CHECK_THROWS(BellmanFord(graph, 0, 3));
}