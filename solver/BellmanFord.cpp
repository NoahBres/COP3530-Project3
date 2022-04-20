#pragma once

#include "AdjacencyList.hpp"

#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <utility>

using namespace std;

pair<int, stack<int>> BellmanFord(AdjacencyList<int> graph, int src, int dest) {
  int numtotalVertices = graph.getTotalVerticesCount();

  unordered_map<uint, int> distances;
  unordered_map<uint, int> parent;

  // Initialization
  for (auto &it : graph.getTotalVertices()) {
    distances[it] = (INT_MAX);
    parent[it] = -1;
  }

  distances[src] = 0;
  for (int i = 1; i <= numtotalVertices - 1; ++i) {
    for (auto &it : graph.getAdjMap()) {
      auto iterator = it.second.begin();

      while (iterator != it.second.end()) {
        if (distances[it.first] == INT_MAX) {
          ++iterator;
          continue;
        }

        if (iterator->second + distances[it.first] <
            distances[iterator->first]) {
          distances[iterator->first] = iterator->second + distances[it.first];
          parent[iterator->first] = it.first;
        }
        ++iterator;
      }
    }
  }

  // Check for negative cycles
  for (auto &it : graph.getAdjMap()) {
    auto iterator = it.second.begin();
    while (iterator != it.second.end()) {
      if (distances[it.first] == INT_MAX) {
        ++iterator;
        continue;
      }
      if (iterator->second + distances[it.first] < distances[iterator->first]) {
        // Throw error on negative cycle
        throw std::runtime_error("Negative edge cycle not supported");
      }

      ++iterator;
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