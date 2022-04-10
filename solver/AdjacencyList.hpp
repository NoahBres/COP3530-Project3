#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// Directed graph
class AdjacencyList {
private:
  unordered_map<unsigned int, unordered_set<unsigned int>> adjMap;

public:
  void insertEdge(unsigned int from, unsigned int to) {
    adjMap[from].insert(to);
  }

  bool isEdge(unsigned int from, unsigned int to) {
    return adjMap[from].find(to) != adjMap[from].end();
  }

  // Returns an unsorted list
  vector<unsigned int> getEdges(unsigned int vertex) {
    vector<unsigned int> output;

    for (auto e : adjMap[vertex]) {
      output.push_back(e);
    }

    return output;
  }

  void printGraph(bool sorted = false) {
    cout << "Graph: " << endl << endl;

    if (sorted) {
      list<pair<unsigned int, unordered_set<unsigned int>>> adjList;

      for (auto e : adjMap)
        adjList.push_back(make_pair(e.first, e.second));

      adjList.sort([](pair<unsigned int, unordered_set<unsigned int>> item1,
                      pair<unsigned int, unordered_set<unsigned int>> item2) {
        return item1.first < item2.first;
      });

      for (auto e : adjList) {
        cout << e.first << ": ";

        vector<unsigned int> toList;
        for (auto f : e.second)
          toList.push_back(f);
        sort(toList.begin(), toList.end());

        for (auto f : toList)
          cout << f << ", ";

        cout << endl;
      }

    } else {
      for (auto e : adjMap) {
        cout << e.first << ": ";
        for (auto f : e.second) {
          cout << f << ", ";
        }
        cout << endl;
      }
    }
  }
};