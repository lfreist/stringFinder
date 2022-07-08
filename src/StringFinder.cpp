// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "./StringFinder.h"

namespace sf {

// ----- public stuff --------------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
StringFinder::StringFinder() = default;

// _____________________________________________________________________________________________________________________
StringFinder::StringFinder(std::function<std::optional<utils::FileChunk>(void)> reader,
                           const std::vector<TaskAndNumThreads> &tasksAndNumThreadsVector,
                           bool performanceMeasuring) {
  _reader = std::move(reader);
  setProcessingPipeline(tasksAndNumThreadsVector);
  _performanceMeasuring = performanceMeasuring;
}

// _____________________________________________________________________________________________________________________
void StringFinder::setReader(std::function<std::optional<utils::FileChunk>()> reader) {
  _reader = std::move(reader);
}

// _____________________________________________________________________________________________________________________
void StringFinder::setProcessingPipeline(const std::vector<TaskAndNumThreads> &tasksAndNumThreadsVector) {
  for (auto &task: tasksAndNumThreadsVector) {
    _processingPipeline.addTask(task.name, task.task, task.numThreads);
  }
}

// _____________________________________________________________________________________________________________________
void StringFinder::enablePerformanceMeasuring(bool enabled) {
  _performanceMeasuring = enabled;
}

// _____________________________________________________________________________________________________________________
void StringFinder::find() {
  utils::Timer timer;
  if (_performanceMeasuring) { timer.start(); }
  std::thread getDataThread(&StringFinder::readChunks, this);

  _processingPipeline.run();
  getDataThread.join();
  if (_performanceMeasuring) {
    timer.stop();
    _totalTime += timer.elapsedSeconds();
  }
  //_processingPipeline.wait();
}

// _____________________________________________________________________________________________________________________
double StringFinder::getTotalRealTime() const {
  return _totalTime;
}

// _____________________________________________________________________________________________________________________
double StringFinder::getThreadsTime() {
  double time = _totalTime;
  auto* task = _processingPipeline.getFirstTask();
  while (true) {
    time += task->getComputeTime();
    task = task->getSubsequentTask();
    if (task == nullptr) {
      break;
    }
  }
  return time;
}

// _____________________________________________________________________________________________________________________
std::vector<std::pair<std::string, double>> StringFinder::getPartialTimes() {
  std::vector<std::pair<std::string, double>> times;
  // TODO: total reader time is not 100% correct since everything besides reading and waiting is ignored...
  times.emplace_back(std::make_pair("reader", _readingTime + _readWaitingTime));
  auto* task = _processingPipeline.getFirstTask();
  while (true) {
    times.emplace_back(std::make_pair(task->getName(), task->getTotalTime()));
    task = task->getSubsequentTask();
    if (task == nullptr) {
      break;
    }
  }
  return times;
}

// _____________________________________________________________________________________________________________________
std::string StringFinder::toString() {
  if (_performanceMeasuring) {
    utils::output::Node root("Total Time (real): " + std::to_string(_totalTime) + " s");
    utils::output::Node ttime("threads:  " + std::to_string(getThreadsTime()));
    utils::output::Node reader("Reading: " + std::to_string(_readingTime + _readWaitingTime) + " s");
    reader.addChild(utils::output::Node("reading: " + std::to_string(_readingTime) + " s"));
    reader.addChild(utils::output::Node("waiting: " + std::to_string(_readWaitingTime) + " s"));
    ttime.addChild(reader);
    auto *task = _processingPipeline.getFirstTask();
    while (task != nullptr) {
      utils::output::Node taskNode(task->getName() + " (" + std::to_string(task->getNumThreads()) + " "
                                     + (task->getNumThreads() > 1 ? "threads" : "thread") + "): "
                                     + std::to_string(task->getTotalTime()) + " s");
      taskNode.addChild(utils::output::Node("computing: " + std::to_string(task->getComputeTime()) + " s"));
      utils::output::Node
        waitNode("waiting: " + std::to_string(task->getPullWaitTime() + task->getPushWaitTime()) + " s");
      waitNode.addChild(utils::output::Node("push: " + std::to_string(task->getPushWaitTime()) + " s"));
      waitNode.addChild(utils::output::Node("pull: " + std::to_string(task->getPullWaitTime()) + " s"));
      taskNode.addChild(waitNode);
      ttime.addChild(taskNode);
      task = task->getSubsequentTask();
    }
    root.addChild(ttime);
    utils::output::Tree ot(root, "StringFinder - Performance Report");
    return ot.parse();
  }
  return "StringFinder - set 'measurePerformance' to get a performance report";
}

// ----- protected stuff -----------------------------------------------------------------------------------------------
// _____________________________________________________________________________________________________________________
void StringFinder::readChunks() {
  utils::Timer readingTimer, readWaitingTimer;
  while (true) {
    if (_performanceMeasuring) { readingTimer.start(false); }
    std::optional<utils::FileChunk> chunk = _reader();
    if (_performanceMeasuring) { readingTimer.stop(); }
    if (!chunk.has_value()) {
      break;
    }
    if (_performanceMeasuring) { readWaitingTimer.start(false); }
    _processingPipeline.getFirstTask()->appendData(std::move(chunk.value()));
    if (_performanceMeasuring) { readWaitingTimer.stop(); }
  }
  _processingPipeline.getFirstTask()->close();
  _readingTime += readingTimer.elapsedSeconds();
  _readWaitingTime += readWaitingTimer.elapsedSeconds();
}

// ----- private stuff -------------------------------------------------------------------------------------------------
}  // namespace sf