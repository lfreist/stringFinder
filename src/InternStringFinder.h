// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRINGFINDER_H_
#define SRC_STRINGFINDER_H_

#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <map>

#include "StringFinder.h"

using std::vector;
using std::string;
using std::map;

namespace sf {

// Class StringData
class InternStringFinder : public StringFinder {
 public:
  InternStringFinder();
  ~InternStringFinder();

  void buildThreads(vector<string::size_type> &matchPositions, string &pattern);
  void buildThreads(vector<string::size_type> &matchPositions, string &pattern, const std::function<int(int)>& transformer);
  void buildThreads(vector<string::size_type> &matchPositions, string &pattern, const std::function<string(string)>& transformer);
  void readChunks(std::istream &input, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
  void readChunks(std::string_view &input, utils::TSQueue<utils::FileChunk*> &popFromQueue, utils::TSQueue<utils::FileChunk*> &pushToQueue);
};

}  // namespace sf

#endif  // SRC_STRINGFINDER_H_
