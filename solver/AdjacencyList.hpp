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

using uint = unsigned int;

// Directed graph
template <typename T,
          typename =
              typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class AdjacencyList {
private:
  unordered_map<uint, unordered_map<uint, T>> adjMap;

public:
  void insertEdge(uint from, uint to, T weight) { adjMap[from][to] = weight; }

  bool isEdge(uint from, uint to) {
    return adjMap[from].find(to) != adjMap[from].end();
  }

  uint getWeight(uint from, uint to, T defaultValue = 0) {
    if (adjMap[from].find(to) == adjMap[from].end())
      return defaultValue;

    return adjMap[from][to];
  }

  // Returns an unsorted list
  vector<T> getEdges(uint vertex) {
    vector<T> output;

    for (auto e : adjMap[vertex]) {
      output.push_back(e);
    }

    return output;
  }

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