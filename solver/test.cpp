#include "AdjacencyList.hpp"
#include "BellmanFord.cpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Bellman Ford Search 1", "[bf]") {
  AdjacencyList<int> graph1;

  // insertion of edges without negative cycle
  graph1.insertEdge(0, 1, -1);
  graph1.insertEdge(0, 2, 4);
  graph1.insertEdge(1, 2, 3);
  graph1.insertEdge(1, 3, 2);
  graph1.insertEdge(1, 4, 2);
  graph1.insertEdge(3, 2, 5);
  graph1.insertEdge(3, 1, 1);
  graph1.insertEdge(4, 3, -3);

  auto solution1 = BellmanFord(graph1, 0, 3);
  REQUIRE(solution1.first == -2);

  AdjacencyList<int> graph2;

  // insertion of edges without negative cycle
  graph2.insertEdge(0, 2, 4);
  graph2.insertEdge(1, 2, 10);
  graph2.insertEdge(1, 3, 2);
  graph2.insertEdge(1, 4, 2);
  graph2.insertEdge(3, 2, 5);
  graph2.insertEdge(2, 4, 15);
  graph2.insertEdge(2, 3, 1);
  graph2.insertEdge(2, 1, 1);
  graph2.insertEdge(3, 1, 2);
  graph2.insertEdge(3, 1, 1);

  auto solution2 = BellmanFord(graph2, 2, 4);
  REQUIRE(solution2.first == 3);
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

TEST_CASE("Bellman Ford Path", "[bf]") {
  AdjacencyList<int> graph;

  graph.insertEdge(0, 2, 4);
  graph.insertEdge(1, 2, 10);
  graph.insertEdge(1, 3, 2);
  graph.insertEdge(1, 4, 2);
  graph.insertEdge(3, 2, 5);
  graph.insertEdge(2, 4, 15);
  graph.insertEdge(2, 3, 1);
  graph.insertEdge(2, 1, 1);
  graph.insertEdge(3, 1, 2);
  graph.insertEdge(3, 1, 1);

  auto solution = BellmanFord(graph, 2, 4);

  REQUIRE(solution.second.top() == 1);
  solution.second.pop();
  REQUIRE(solution.second.top() == 4);
  solution.second.pop();
  REQUIRE(solution.second.size() == 0);
}