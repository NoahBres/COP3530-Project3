#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "httplib.h"

#include "AdjacencyList.hpp"
#include "BellmanFord.cpp"
#include "Dijkstra.cpp"
#include "cache.cpp"

using namespace std::chrono;

httplib::Server server;

int main() {
  AdjacencyList<int> caliGraph;

  cout << "Reading California roads..." << endl;

  // Start data ingestion timer
  auto start = high_resolution_clock::now();
  ifstream caliRoads("../data/cal-road-edges.txt");

  if (!caliRoads.is_open()) {
    cout << "Could not read cal-road-edges.txt" << endl;
    return 0;
  }

  // Scope the file processing section
  {
    string line;
    string delimeter = " ";
    while (getline(caliRoads, line)) {
      int from, to;
      int weight;

      // Read line by line. Split via spaced delimeters
      // Parse to, found, and weight
      auto found = line.find(delimeter);
      line.erase(0, found + delimeter.length());
      found = line.find(delimeter);
      from = stoi(line.substr(0, found));
      line.erase(0, found + delimeter.length());
      found = line.find(delimeter);
      to = stoi(line.substr(0, found));
      line.erase(0, found + delimeter.length());
      weight = (int)(stod(line) * 1000000);

      // Insert reverse edges as our graph is directed
      caliGraph.insertEdge(from, to, weight);
      caliGraph.insertEdge(to, from, weight);
    }
  }

  caliRoads.close();

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(stop - start);

  cout << "Building California graph took " << (duration.count() / 1000.0)
       << " seconds" << endl;

  // Search REST endpoint
  // Handles Dijkstra and Bellman Ford search requests from the client
  server.Get("/search", [&caliGraph](const httplib::Request &req,
                                     httplib::Response &res) {
    //  Solve CORS issue
    res.set_header("Access-Control-Allow-Origin", "*");

    // Check if params are valid
    if (!req.has_param("from") || !req.has_param("to") ||
        !req.has_param("type")) {
      res.set_content("Request requires from, to, and type", "text/plain");
    } else {
      auto from = stoi(req.get_param_value("from"));
      auto to = stoi(req.get_param_value("to"));
      auto type = req.get_param_value("type");

      // Handle dijkstra and bellman ford searches separately
      string cacheReadFilePath = "";
      pair<int, stack<int>> (*searchMethod)(AdjacencyList<int>, int, int);

      stack<int> pathStack;
      int distance;

      // Switch between search methods based on request
      if (type == "dijkstra") {
        cacheReadFilePath = "cache_dijkstra";
        searchMethod = &Dijkstra;
      } else if (type == "bellmanford") {
        cacheReadFilePath = "cache_bellman";
        searchMethod = &BellmanFord;
      }

      auto cacheReadOut = cacheRead(from, to, cacheReadFilePath);

      auto start = high_resolution_clock::now();

      if (cacheReadOut.first != INT_MAX) {
        cout << "Result retrieved from cache" << endl;

        distance = cacheReadOut.first;
        pathStack = cacheReadOut.second;
      } else {
        cout << "Starting search..." << endl;

        auto solution = searchMethod(caliGraph, from, to);
        distance = solution.first;
        pathStack = solution.second;

        cacheWrite(from, to, solution.first, solution.second,
                   cacheReadFilePath);
      }

      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<milliseconds>(stop - start);

      string path = "";
      while (!pathStack.empty()) {
        path += to_string(pathStack.top());
        path += ",";
        pathStack.pop();
      }

      res.set_content(
          "{\"path\":\"" + path + "\", \"distance\": " + to_string(distance) +
              ", \"time\": " + to_string(duration.count()) + " " + "}",
          "application/json");
    }
  });

  server.Get("/stop", [&](const httplib::Request &req, httplib::Response &res) {
    server.stop();
  });

  server.listen("0.0.0.0", 8888);
}