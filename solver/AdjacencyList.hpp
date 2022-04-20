#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// Unsigned ints used for ids
using uint = unsigned int;

// Directed graph
// Accept only arithmetic typing
template <typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class AdjacencyList {
private:
  unordered_map<uint, unordered_map<uint, T>> adjMap;
  unordered_set<uint> fromVertices;
  unordered_set<uint> toVertices;
  unordered_set<uint>
      totalVertices; // Ideally this just be a union of from/to but it would be
                     // much faster to just keep track of total on insertion
                     // since we dont support edge delete operations

public:
  // Insert edge (directed)
  void insertEdge(uint from, uint to, T weight) {
    adjMap[from][to] = weight;

    fromVertices.insert(from);
    toVertices.insert(to);

    totalVertices.insert(to);
    totalVertices.insert(from);
  }

  const bool isEdge(uint from, uint to) {
    return adjMap[from].find(to) != adjMap[from].end();
  }

  const uint getWeight(uint from, uint to, T defaultValue = 0) {
    if (adjMap[from].find(to) == adjMap[from].end())
      return defaultValue;

    return adjMap[from][to];
  }

  // Returns an unsorted list
  const vector<T> getEdges(uint vertex) {
    vector<T> output;

    for (auto e : adjMap[vertex]) {
      output.push_back(e);
    }

    return output;
  }

  // Getters for the internal properties
  const decltype(adjMap) getAdjMap() const { return adjMap; }

  const decltype(fromVertices) getFromVertices() const { return fromVertices; }

  const decltype(toVertices) getToVertices() const { return toVertices; }

  const decltype(totalVertices) getTotalVertices() const {
    return totalVertices;
  }

  const decltype(fromVertices.size()) getFromVerticesCount() const {
    return fromVertices.size();
  }

  const decltype(toVertices.size()) getToVerticesCount() const {
    return toVertices.size();
  }

  const decltype(totalVertices.size()) getTotalVerticesCount() const {
    return totalVertices.size();
  }

  // Print graph for debugging purposes
  // Prints out from vertices and connected edges with their weights
  // Edge ID: To ID (weight), To ID (weight), To ID (Weight)
  void printGraph(bool sorted = false) {
    cout << "Graph: " << endl << endl;

    if (sorted) {
      list<pair<uint, unordered_map<uint, T>>> adjList;

      for (auto e : adjMap)
        adjList.push_back(make_pair(e.first, e.second));

      adjList.sort([](pair<uint, unordered_map<uint, T>> item1,
                      pair<uint, unordered_map<uint, T>> item2) {
        return item1.first < item2.first;
      });

      for (auto e : adjList) {
        cout << e.first << ": ";

        list<pair<uint, T>> toList;
        for (auto f : e.second)
          toList.push_back(make_pair(f.first, f.second));

        toList.sort([](pair<uint, T> item1, pair<uint, T> item2) {
          return item1.first < item2.first;
        });

        for (auto f : toList)
          cout << f.first << " (" << f.second << ")"
               << ", ";

        cout << endl;
      }

    } else {
      for (auto e : adjMap) {
        cout << e.first << ": ";

        for (auto f : e.second) {
          cout << f.first << ", ";
        }
        cout << endl;
      }
    }
  }
};