#pragma once

#include "AdjacencyList.hpp"

#include <stack>

using namespace std;

pair<int, stack<int>> Dijkstra(AdjacencyList<int> graph, int src, int dest) {
  int numtotalVertices = graph.getTotalVerticesCount();
  unordered_map<uint, int> distances;
  unordered_map<uint, bool> checker;
  unordered_map<uint, int> parent;

  // Initialization
  for (auto &it : graph.getTotalVertices()) {
    distances[it] = INT_MAX;
    checker[it] = false;
    parent[it] = -1;
  }

  distances[src] = 0;
  for (int i = 0; i < numtotalVertices - 1; i++) {
    int minimum = INT_MAX;
    int minimumIndex;

    for (auto &it : graph.getTotalVertices()) {
      if (distances[it] <= minimum && checker[it] == false) {
        minimum = distances[it];
        minimumIndex = it;
      }
    }

    checker[minimumIndex] = true;

    for (int n = 0; n < numtotalVertices; n++) {
      bool edge = graph.isEdge(minimumIndex, n);
      uint weight = graph.getWeight(minimumIndex, n, 0);

      if (checker[n] == false && edge == true &&
          distances[minimumIndex] != (INT_MAX) &&
          distances[minimumIndex] + weight < distances[n]) {

        distances[n] = distances[minimumIndex] + weight;
        parent[n] = minimumIndex;
      }
    }
  }

  // Calculate path
  int pathDest = dest;
  stack<int> st;
  while (parent[pathDest] != -1) {
    st.push(pathDest);
    pathDest = parent[pathDest];
  }

  return make_pair(distances[dest], st);
}