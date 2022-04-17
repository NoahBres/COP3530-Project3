#pragma once

#include "AdjacencyList.hpp"
#include <stdexcept>

int BellmanFord(AdjacencyList<int> graph, int src, int dest) {
  int numtotalVertices = graph.getTotalVerticesCount();

  unordered_map<uint, int> distances;
  for (auto &it : graph.getTotalVertices()) {
    distances[it] = (INT_MAX);
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
        }
        ++iterator;
      }
    }
  }

  for (auto &it : graph.getAdjMap()) {
    auto iterator = it.second.begin();
    while (iterator != it.second.end()) {
      if (distances[it.first] == INT_MAX) {
        ++iterator;
        continue;
      }
      if (iterator->second + distances[it.first] < distances[iterator->first]) {
        throw std::runtime_error("Negative edge cycle not supported");
      }

      ++iterator;
    }
  }

  return distances[dest];
}