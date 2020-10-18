#include <iostream>
#include <map>
#include <string>
#include "map.h"

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;

int main () {
  std::map<string, cs246e::map<int, int>> allMaps;
  string cmdString, mapName;

  while (cin >> cmdString >> mapName) {
    if (cmdString == "newDefault") {
      allMaps[mapName];
    }
    if (cmdString == "newInit") {
      cs246e::map<int, int> m {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
      allMaps[mapName] = m;
    }
    else if (cmdString == "=") {
      string otherName;
      cin >> otherName;
      allMaps[mapName] = allMaps[otherName];
    }
    else if (cmdString == "[]=") {
      int key, val;
      cin >> key >> val;
      allMaps[mapName][key] = val;
    }
    else if (cmdString == "[]") {
      int key;
      cin >> key;
      cout << allMaps[mapName][key] << endl;
    }
    else if (cmdString == "at") {
      int key;
      cin >> key;
      try {
        cout << allMaps[mapName].at(key) << endl;
      }
      catch (...) {
        cout << "Exception" << endl;
      }
    }
    else if (cmdString == "empty") {
      cout << allMaps[mapName].empty() << endl;
    }
    else if (cmdString == "size") {
      cout << allMaps[mapName].size() << endl;
    }
    else if (cmdString == "erase") {
      int key;
      cin >> key;
      allMaps[mapName].erase(key);
    }
    else if (cmdString == "clear") {
      allMaps[mapName].clear();
    }
    else if (cmdString == "count") {
      int key;
      cin >> key;
      cout << allMaps[mapName].count(key) << endl;
    }
#if 0
    for (auto &x: allMaps) {
      cout << x.first << ":";
      for (cs246e::map<int, int>::iterator &y = x.second.begin();
        y != x.second.end(); ++y) {
        cout << " (" << y.first << "," << y.second << ")";
      }
#endif
/*
      for (auto &y: x.second) {
        cout << " (" << y.first << "," << y.second << ")";
      }*/
    for (auto &x: allMaps) {
      //x.second.printstuff();
      cout << x.first << ":";
      for (auto &y: x.second) {
        cout << " (" << y.first << "," << y.second << ")";
      }
      cout << endl;
    }
  }
}
