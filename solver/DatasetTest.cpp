#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "AdjacencyList.hpp"
#include "BellmanFord.cpp"
#include "Dijkstra.cpp"

using namespace std::chrono;

enum GraphSample {
  // To be implemented
  DataSetCA,
  DataSetPA,
  DataSetTX,

  // Randomly generated graphs
  RandomGraph
};
GraphSample graphToUse = DataSetCA;
int randomGraphCount = 50;
unsigned int randomWeightMax = 50;

int main() {
  AdjacencyList<int> graph;

  int searchFrom, searchTo;

  if (graphToUse == RandomGraph) {
    for (int i = 0; i < randomGraphCount; i++) {
      auto edge1 = std::rand() % randomGraphCount;
      auto edge2 = std::rand() % randomGraphCount;
      auto weight = std::rand() % randomWeightMax;

      // Undirected graph -> directed
      graph.insertEdge(edge1, edge2, weight);
      graph.insertEdge(edge2, edge1, weight);
    }

    searchFrom = std::rand() & randomGraphCount;
    searchTo = std::rand() & randomGraphCount;
  } else {
    string filePath;
    if (graphToUse == DataSetCA) {
      filePath = "roadNet-CA.txt";

      // Randomly selected
      searchFrom = 990444;
      searchTo = 444275;
    } else if (graphToUse == DataSetPA) {
      filePath = "roadNet-PA.txt";

      // Randomly selected
      searchFrom = 6309;
      searchTo = 586434;
    } else if (graphToUse == DataSetTX) {
      filePath = "roadNet-TX.txt";

      // Randomly selected
      searchFrom = 4;
      searchTo = 1285961;
    }

    auto start = high_resolution_clock::now();

    ifstream dataset("../data/" + filePath);

    // Skip first 4 lines
    for (int i = 0; i < 4; i++)
      dataset.ignore(numeric_limits<streamsize>::max(), dataset.widen('\n'));

    string delimeter = "	";

    if (dataset.is_open()) {
      string line;
      while (getline(dataset, line)) {
        int from, to;

        auto found = line.find(delimeter);
        string first = line.substr(0, found);
        from = stoi(first);
        line.erase(0, found + delimeter.length());
        to = stoi(line);

        graph.insertEdge(from, to, std::rand() & randomWeightMax);
      }

      dataset.close();

      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<milliseconds>(stop - start);

      cout << "Ingesting dataset took: " << (duration.count() / 1000.0)
           << " seconds" << endl;
    }
  }

  // Scope each. Ideally these be threaded but don't really have the time
  {
    cout << "Starting Dijkstra Search..." << endl;

    auto start = high_resolution_clock::now();
    auto solution = Dijkstra(graph, searchFrom, searchTo);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Distance: " << solution.first << endl;
    cout << "Dijkstra took: " << (duration.count() / 1000.0) << " seconds"
         << endl;
  }

  {
    cout << "Starting Bellman Ford Search..." << endl;

    auto start = high_resolution_clock::now();
    auto solution = BellmanFord(graph, searchFrom, searchTo);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Distance: " << solution.first << endl;
    cout << "Bellman Ford took: " << (duration.count() / 1000.0) << " seconds"
         << endl;
  }
}