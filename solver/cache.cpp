#pragma once

#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

const string CACHE_PATH_BELLMAN = "cache_bellman";
const string CACHE_PATH_DIJKSTRA = "cache_dijkstra";

pair<int, stack<int>> cacheRead(int from, int to, string filePath) {
  stack<int> output;

  ifstream cache(filePath);

  string line;
  string delimeter = ",";
  cout << "Read cache: " << endl;

  if (!cache.is_open())
    throw runtime_error("Cannot read file");

  while (getline(cache, line)) {
    int fromRead, toRead;
    int distance;

    auto found = line.find(delimeter);
    fromRead = stoi(line.substr(0, found));
    line.erase(0, found + delimeter.length());
    found = line.find(delimeter);
    toRead = stoi(line.substr(0, found));
    line.erase(0, found + delimeter.length());
    found = line.find(delimeter);
    distance = stoi(line.substr(0, found));
    line.erase(0, found + delimeter.length());

    if ((fromRead == from && toRead == to) ||
        (fromRead == to && toRead == from)) {
      while (line.length() > 0) {
        found = line.find(delimeter);
        if (found != std::string::npos) {

          output.push(stoi(line.substr(0, found)));
          line.erase(0, found + delimeter.length());
        } else {
          output.push(stoi(line));
          line.erase(0, line.length());
        }
      }

      cache.close();

      return make_pair(distance, output);
    } else {
      continue;
    }
  }

  cache.close();
  return make_pair(INT_MAX, output);
}

void cacheWrite(int from, int to, int distance, stack<int> path,
                string filePath) {
  fstream bellmanCache;

  bellmanCache.open(filePath, ios_base::app);

  if (!bellmanCache.is_open())
    return;

  string cacheLine = "";
  cacheLine += to_string(from);
  cacheLine += ",";
  cacheLine += to_string(to);
  cacheLine += ",";
  cacheLine += to_string(distance);
  cacheLine += ",";

  stack<int> pathCopy;
  // Two stacks because we want to store it in reverse
  while (!path.empty()) {
    pathCopy.push(path.top());
    path.pop();
  }

  while (!pathCopy.empty()) {
    cacheLine += to_string(pathCopy.top());
    pathCopy.pop();

    if (!pathCopy.empty())
      cacheLine += ",";
  }

  bellmanCache << cacheLine << endl;

  bellmanCache.close();
}