#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "AdjacencyList.hpp"
#include "BellmanFord.cpp"
#include "Dijkstra.cpp"

using namespace std::chrono;

// Benchmarking large datasets

// Choose between existing road network datasets
// CA, PA, TX datasets are between 1 and 2 million vertices and edges
// RandomGraph allows us to specify synthetic random data
enum GraphSample {
  DataSetCA,
  DataSetPA,
  DataSetTX,

  // Randomly generated graphs
  RandomGraph
};

// Use the CA dataset
GraphSample graphToUse = DataSetCA;

// Vertex for RandomGraph
int randomGraphCount = 50;
// Max value for the random weights
unsigned int randomWeightMax = 50;

int main() {
  AdjacencyList<int> graph;

  int searchFrom, searchTo;

  // Synthetic random graph
  if (graphToUse == RandomGraph) {
    for (int i = 0; i < randomGraphCount; i++) {
      auto edge1 = std::rand() % randomGraphCount;
      auto edge2 = std::rand() % randomGraphCount;
      auto weight = std::rand() % randomWeightMax;

      // Undirected graph -> directed
      graph.insertEdge(edge1, edge2, weight);
      graph.insertEdge(edge2, edge1, weight);
    }

    // Randomize searchFrom and searchTo for testing
    // Not guaranteed to be existing vertices
    searchFrom = std::rand() & randomGraphCount;
    searchTo = std::rand() & randomGraphCount;
  } else {
    string filePath;
    if (graphToUse == DataSetCA) {
      filePath = "roadNet-CA.txt";

      // Randomly selected points from the dataset
      searchFrom = 990444;
      searchTo = 444275;
    } else if (graphToUse == DataSetPA) {
      filePath = "roadNet-PA.txt";

      // Randomly selected points from the dataset
      searchFrom = 6309;
      searchTo = 586434;
    } else if (graphToUse == DataSetTX) {
      filePath = "roadNet-TX.txt";

      // Randomly selected points from the dataset
      searchFrom = 4;
      searchTo = 1285961;
    }

    // Start benchmarking
    auto start = high_resolution_clock::now();

    ifstream dataset("../data/" + filePath);

    // Skip first 4 lines for the datasets
    // Contains metadata and attribution info
    for (int i = 0; i < 4; i++)
      dataset.ignore(numeric_limits<streamsize>::max(), dataset.widen('\n'));

    string delimeter = "	";

    // Ingest dataset
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

  // Scope each search so the memory can be freed once it's done
  // Ideally these be threaded but that is out of the project requirement scope
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