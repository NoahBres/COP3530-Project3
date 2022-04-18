#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "httplib.h"

#include "AdjacencyList.hpp"
#include "BellmanFord.cpp"

using namespace std::chrono;

httplib::Server server;

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
unsigned int randomWeightMax = 50;

int main() {
  AdjacencyList<unsigned int> graph;

  switch (graphToUse) {
  case RandomGraph: {
    for (int i = 0; i < randomGraphCount; i++) {
      auto edge1 = std::rand() % randomGraphCount;
      auto edge2 = std::rand() % randomGraphCount;
      auto weight = std::rand() % randomWeightMax;

      // Undirected graph -> directed
      graph.insertEdge(edge1, edge2, weight);
      graph.insertEdge(edge2, edge1, weight);
    }

    break;
  }
  case DataSet1: {
    auto start = high_resolution_clock::now();

    ifstream dataset("../data/roadNet-CA.txt");

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
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Ingesting dataset took: " << (duration.count() / 1000.0)
         << " seconds" << endl;

    break;
  }
  default:
    break;
  }

  // graph.printGraph(true);

  server.Get("/graph-type", [](const httplib::Request &,
                               httplib::Response &res) {
    switch (graphToUse) {
    case RandomGraph: {
      res.set_content("{ type: \"random\"}", "application/json");
      break;
    }
    case DataSet1:
      res.set_content("{ type: \"dataset\", payload: 1}", "application/json");
      break;
    default:
      res.set_content("{ type: \"none\"}", "application/json");
      break;
    }
  });

  server.Get("/solve", [](const httplib::Request &, httplib::Response &res) {
    // res.set_content("Hey", "text/plain");
    res.set_content("{hey: \"3\"}", "application/json");
  });

  server.Get("/stop", [&](const httplib::Request &req, httplib::Response &res) {
    server.stop();
  });

  server.listen("0.0.0.0", 8888);
}