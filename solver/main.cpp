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
  // AdjacencyList<unsigned int> graph;

  // switch (graphToUse) {
  // case RandomGraph: {
  //   for (int i = 0; i < randomGraphCount; i++) {
  //     auto edge1 = std::rand() % randomGraphCount;
  //     auto edge2 = std::rand() % randomGraphCount;
  //     auto weight = std::rand() % randomWeightMax;

  //     // Undirected graph -> directed
  //     graph.insertEdge(edge1, edge2, weight);
  //     graph.insertEdge(edge2, edge1, weight);
  //   }

  //   break;
  // }
  // case DataSet1: {
  //   auto start = high_resolution_clock::now();

  //   ifstream dataset("../data/roadNet-CA.txt");

  //   // Skip first 4 lines
  //   for (int i = 0; i < 4; i++)
  //     dataset.ignore(numeric_limits<streamsize>::max(), dataset.widen('\n'));

  //   string delimeter = "	";

  //   if (dataset.is_open()) {
  //     string line;
  //     while (getline(dataset, line)) {
  //       int from, to;

  //       auto found = line.find(delimeter);
  //       string first = line.substr(0, found);
  //       from = stoi(first);
  //       line.erase(0, found + delimeter.length());
  //       to = stoi(line);

  //       graph.insertEdge(from, to, std::rand() & randomWeightMax);
  //     }

  //     dataset.close();
  //   }

  //   auto stop = high_resolution_clock::now();
  //   auto duration = duration_cast<milliseconds>(stop - start);

  //   cout << "Ingesting dataset took: " << (duration.count() / 1000.0)
  //        << " seconds" << endl;

  //   break;
  // }
  // default:
  //   break;
  // }

  // graph.printGraph(true);

  AdjacencyList<int> caliGraph;

  cout << "Reading California roads..." << endl;
  auto start = high_resolution_clock::now();
  ifstream caliRoads("../data/cal-road-edges.txt");

  if (!caliRoads.is_open()) {
    cout << "Could not read cal-road-edges.txt" << endl;
    return 0;
  }

  // Scope this process just cuz
  {
    string line;
    string delimeter = " ";
    while (getline(caliRoads, line)) {
      int from, to;
      int weight;

      auto found = line.find(delimeter);
      line.erase(0, found + delimeter.length());
      found = line.find(delimeter);
      from = stoi(line.substr(0, found));
      line.erase(0, found + delimeter.length());
      found = line.find(delimeter);
      to = stoi(line.substr(0, found));
      line.erase(0, found + delimeter.length());
      weight = (int)(stod(line) * 1000000);

      caliGraph.insertEdge(from, to, weight);
      caliGraph.insertEdge(to, from, weight);
    }
  }

  caliRoads.close();

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);

  cout << "Building California graph took " << (duration.count() / 1000.0)
       << " seconds" << endl;

  server.Get("/graph-type",
             [](const httplib::Request &, httplib::Response &res) {
               res.set_header("Access-Control-Allow-Origin", "*");

               switch (graphToUse) {
               case RandomGraph: {
                 res.set_content("{ \"type\": \"random\"}", "application/json");
                 break;
               }
               case DataSet1:
                 res.set_content("{ \"type\": \"dataset\", payload: 1}",
                                 "application/json");
                 break;
               default:
                 res.set_content("{ \"type\": \"none\"}", "application/json");
                 break;
               }
             });

  server.Get("/search", [&caliGraph](const httplib::Request &req,
                                     httplib::Response &res) {
    // res.set_content("Hey", "text/plain");
    res.set_header("Access-Control-Allow-Origin", "*");

    if (!req.has_param("from") || !req.has_param("to") ||
        !req.has_param("type")) {
      res.set_content("Request requires from, to, and type", "text/plain");
    } else {
      auto from = stoi(req.get_param_value("from"));
      auto to = stoi(req.get_param_value("to"));
      auto type = req.get_param_value("type");

      if (type == "dijkstra") {
        res.set_content("Dijkstra not implemented", "text/plain");
      } else if (type == "bellmanford") {
        cout << "Starting Bellman Ford search..." << endl;

        auto start = high_resolution_clock::now();

        auto solution = BellmanFord(caliGraph, from, to);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        string path = "";
        while (!solution.second.empty()) {
          path += to_string(solution.second.top());
          path += ",";
          solution.second.pop();
        }

        res.set_content("{\"path\":\"" + path +
                            "\", \"distance\": " + to_string(solution.first) +
                            ", \"time\": " + to_string(duration.count()) + " " +
                            "}",
                        "application/json");
      } else {
        res.set_content("Unsupported type", "text/plain");
      }
    }
  });

  server.Get("/stop", [&](const httplib::Request &req, httplib::Response &res) {
    server.stop();
  });

  server.listen("0.0.0.0", 8888);
}