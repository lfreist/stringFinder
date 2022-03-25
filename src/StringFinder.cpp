// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "StringFinder.h"
#include "./utils/Timer.h"

using std::vector;
using std::string;
using std::ifstream;

void mergeArrays(vector<const string *> *out, vector<const string *> *in) {
  vector<const string *> tmp(out->size() + in->size());
  std::merge(out->begin(), out->end(), in->begin(), in->end(), tmp.begin());
  *out = std::move(tmp);
}

// ____________________________________________________________________________
StringFinder::StringFinder(const string &filepath, bool verbose) {
  _filepath = filepath;
  _verbose = verbose;
  readFile(filepath);
}

// ____________________________________________________________________________
StringFinder::~StringFinder() = default;

// ____________________________________________________________________________
void StringFinder::readFile(const string &path, bool append) {
  std::cout << "Reading file " << path << std::endl;
  string line;
  ifstream file(path.c_str());
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open file " << path << std::endl;
    return;
  }
  if (!append) {
    _data.clear();
  }
  int counter = 0;
  int innerCounter = 1;
  while (!file.eof()) {
    getline(file, line);
    if (line.empty()) {
      continue;
    }
    _data.push_back(line);
    if (_verbose) {
      counter++;
      if (counter == 100000) {
        std::cout << "\r" << innerCounter * counter << " lines" << std::flush;
        counter = 0;
        innerCounter++;
      }
    }
  }
  std::cout << "\r" << dataSize() << " lines" << std::endl;
  std::cout << "done" << std::endl;
}

// ____________________________________________________________________________
vector<const string *> StringFinder::find(string expression, bool matchCase) const {
#pragma omp declare reduction (merge: vector<const string*> : mergeArrays(&omp_out, &omp_in))
  vector<const string *> results;
  if (!matchCase) {
    transform(expression.begin(), expression.end(), expression.begin(), ::tolower);
  }
#pragma omp parallel for reduction(merge: results) num_threads(2)
  for (vector<string>::const_iterator it = _data.begin(); it != _data.end(); ++it) {
    if (!matchCase) {
      string newStr = *it;
      transform(newStr.begin(), newStr.end(), newStr.begin(), ::tolower);
      if (newStr.find(expression) != string::npos) {
#pragma omp critical
        results.push_back(&(*it));
      }
    } else {
      if (it->find(expression) != string::npos) {
#pragma omp critical
        results.push_back(&(*it));
      }
    }
  }
  return results;
}

// ____________________________________________________________________________
void StringFinder::measurePerformance(const string &expression, bool matchCase) const {
  Timer timer;
  timer.start();
  vector<const string *> results = find(expression, matchCase);
  timer.stop();
  string matchCaseStr = matchCase ? string("true") : string("false");
  std::cout << "Performance Report for file: " << _filepath << std::endl;
  std::cout << "StringFinder.find(expression=" << expression << ", matchCase=" << matchCaseStr << "):" << std::endl;
  std::cout << " total lines:\t" << _data.size() << std::endl;
  std::cout << " total matches:\t" << results.size() << std::endl;
  std::cout << " query time:\t" << timer.elapsedSeconds() << " s" << std::endl;
  std::cout << " time / match:\t" << timer.elapsedSeconds() / static_cast<double>(results.size()) << " s" << std::endl;
}

unsigned long StringFinder::dataSize() const {
  return _data.size();
}
