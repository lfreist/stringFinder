// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>

#include "InternStringFinder.h"
#include "./utils/Timer.h"

using std::vector;
using std::string;
using std::map;
using std::pair;
using std::ifstream;

void mergeArrays(vector<const string *> *out, vector<const string *> *in) {
  vector<const string *> tmp(out->size() + in->size());
  std::merge(out->begin(), out->end(), in->begin(), in->end(), tmp.begin());
  *out = std::move(tmp);
}

// _____________________________________________________________________________________________________________________
InternStringFinder::InternStringFinder(const string &filepath, bool verbose, unsigned nThreads) {
  _verbose = verbose;
  _nThreads = nThreads;
  readFile(filepath);
}

// _____________________________________________________________________________________________________________________
InternStringFinder::InternStringFinder(bool verbose, unsigned int nThreads) {
  _verbose = verbose;
  _nThreads = nThreads;
  std::cout << _fileDataMap.size() << std::endl;
}

// _____________________________________________________________________________________________________________________
InternStringFinder::~InternStringFinder() = default;

// _____________________________________________________________________________________________________________________
void InternStringFinder::readFile(const string &filepath, string alias) {
  if (alias.empty()) {
    alias = filepath;
  }
  if (_fileDataMap.find(alias) != _fileDataMap.end()) {
    std::cout << "File " << filepath << " already hold in data!" << std::endl;
    return;
  }
  _fileDataMap.insert(pair<const string, vector<string>>(alias, vector<string>()));
  std::cout << "Reading file " << filepath << " as " << alias << std::endl;
  string line;
  ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open file " << filepath << std::endl;
    return;
  }
  int counter = 0;
  int innerCounter = 1;
  while (!file.eof()) {
    getline(file, line);
    if (line.empty()) {
      continue;
    }
    _fileDataMap[alias].push_back(line);
    if (_verbose) {
      counter++;
      if (counter == 100000) {
        std::cout << "\r" << innerCounter * counter << " lines..." << std::flush;
        counter = 0;
        innerCounter++;
      }
    }
  }
  if (_verbose) {
    std::cout << "\r" << _fileDataMap[filepath].size() << " lines ✓" << std::endl;
  }
}

// _____________________________________________________________________________________________________________________
vector<const string *> InternStringFinder::find(string expression, bool matchCase) const {
#pragma omp declare reduction (merge: vector<const string*> : mergeArrays(&omp_out, &omp_in))
  vector<const string *> results;
  if (!matchCase) {
    transform(expression.begin(), expression.end(), expression.begin(), ::tolower);
  }
  for (auto mit = _fileDataMap.begin(); mit != _fileDataMap.end(); ++mit) {
#pragma omp parallel for reduction(merge: results) num_threads(_nThreads)
    for (auto vit = mit->second.begin(); vit != mit->second.end(); ++vit) {
      if (!matchCase) {
        string newStr = *vit;
        transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
        if (newStr.find(expression) != string::npos) {
#pragma omp critical
          results.push_back(&(*vit));
        }
      } else {
        if (vit->find(expression) != string::npos) {
#pragma omp critical
          results.push_back(&(*vit));
        }
      }
    }
  }
  return results;
}

// _____________________________________________________________________________________________________________________
void InternStringFinder::measurePerformance(const string &expression, bool matchCase) const {
  Timer timer;
  timer.start();
  vector<const string *> results = find(expression, matchCase);
  timer.stop();
  string matchCaseStr = matchCase ? string("true") : string("false");
  std::cout << "Performance Report for file(s):";
  std::cout << std::endl;
  std::cout << "Using " << _nThreads << " thread(s)." << std::endl;
  std::cout << "StringFinder.find(expression=" << expression << ", matchCase=" << matchCaseStr << "):" << std::endl;
  std::cout << " total #lines:\t" << numLines() << std::endl;
  std::cout << " total #matches:\t" << results.size() << std::endl;
  std::cout << " query time:\t" << timer.elapsedSeconds() << " s" << std::endl;
  std::cout << " time / match:\t" << timer.elapsedSeconds() / static_cast<double>(results.size()) << " s" << std::endl;
}

// _____________________________________________________________________________________________________________________
template <typename ...ArgsT>
ulong InternStringFinder::numLines(ArgsT ...files) const {
  return numLines({files...});
}


// _____________________________________________________________________________________________________________________
ulong InternStringFinder::numLines(std::initializer_list<string> files) const {
  if (empty(files)) {
    ulong numLines;
    for (auto &pair: _fileDataMap) {
      numLines += pair.second.size();
    }
    return numLines;
  }
  ulong numLines = 0;
  for (const auto& file : files) {
    auto pair = _fileDataMap.find(file);
    if (pair != _fileDataMap.end()) {
      numLines += pair->second.size();
    }
  }
  return numLines;
}


// _____________________________________________________________________________________________________________________
void InternStringFinder::reset() {
  _fileDataMap.clear();
}