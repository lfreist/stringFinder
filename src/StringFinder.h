// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <memory>

#include "./utils/output/tree.h"
#include "./utils/Timer.h"
#include "./utils/FileChunk.h"
#include "./utils/ThreadSafeQueue.h"
#include "./utils/Task.h"

#ifndef STRINGFINDER_SRC_STRINGFINDERBASE_H_
#define STRINGFINDER_SRC_STRINGFINDERBASE_H_

namespace sf {

struct TaskAndNumThreads {
  std::string name;
  std::function<void(utils::FileChunk*)> task;
  unsigned numThreads;
};

class StringFinder {
 public:
  StringFinder();
  StringFinder(std::function<std::optional<utils::FileChunk>(void)> reader,
               const std::vector<TaskAndNumThreads>& tasksAndNumThreadsVector);

  void setProcessingPipeline(const std::vector<TaskAndNumThreads>& tasksAndNumThreadsVector);

  void enablePerformanceMeasuring(bool enabled);

  void find();

  double getTotalRealTime() const;
  double getThreadsTime();
  std::vector<std::pair<std::string, double>> getPartialTimes();

  std::string toString();

 protected:
  void readChunks();
  void collectPartialResults(std::vector<std::pair<ulong, std::vector<ulong>>> &matchPositions);

  std::function<std::optional<utils::FileChunk>(void)> _reader;
  utils::TaskPipeline<utils::FileChunk> _processingPipeline;

  utils::TSQueue<std::pair<ulong, std::vector<ulong>>> _partialResultsQueue;

  // performance stuff:
  bool _performanceMeasuring = false;
  ulong _totalNumberBytesRead = 0;

  double _totalTime = 0;
  double _readingTime = 0;
  double _readWaitingTime = 0;
};

}  // namespace sf



#endif //STRINGFINDER_SRC_STRINGFINDERBASE_H_
