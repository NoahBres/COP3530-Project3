#pragma once

#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// Read from cache
// Returns Pair<Distance, Path>
// Path represented as a stack
// Returns INT_MAX as the distance if from -> to is not found
// From -> to order does not matter. This method assumes undirected and will
// check both
pair<int, stack<int>> cacheRead(int from, int to, string filePath) {
  stack<int> output;

  ifstream cache(filePath);

  string line;
  string delimeter = ",";

  if (!cache.is_open())
    return make_pair(INT_MAX, output);

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

    // Check from to, irrrespective of order
    // If found, read the path and convert it to a stack
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

// Write to cache
// Appends to existing file. Does not overwrite
void cacheWrite(int from, int to, int distance, stack<int> path,
                string filePath) {
  fstream cacheFile;

  cacheFile.open(filePath, ios_base::app);

  if (!cacheFile.is_open())
    return;

  string cacheLine = "";
  cacheLine += to_string(from);
  cacheLine += ",";
  cacheLine += to_string(to);
  cacheLine += ",";
  cacheLine += to_string(distance);
  cacheLine += ",";

  // We create a second stack because we want to store it in reverse
  // This is so we can read the string directly and push it on to the stack when
  // reading from cache
  stack<int> pathCopy;
  while (!path.empty()) {
    pathCopy.push(path.top());
    path.pop();
  }

  // Write reversed stack to string
  while (!pathCopy.empty()) {
    cacheLine += to_string(pathCopy.top());
    pathCopy.pop();

    if (!pathCopy.empty())
      cacheLine += ",";
  }

  cacheFile << cacheLine << endl;

  cacheFile.close();
}