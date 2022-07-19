// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <functional>

#include "./InternStringFinder.h"

#include "./utils/ZstdWrapper.h"

namespace sf {

std::vector<uint64_t> InternStringFinder::find(const std::string &pattern,
                                               std::unique_ptr<utils::readers::BaseReader> reader,
                                               bool ignoreCase,
                                               bool performanceMeasuring,
                                               unsigned nDecompressionThreads,
                                               unsigned nTransformThreads,
                                               unsigned nSearchThreads) {
  if (nSearchThreads == 0) {
    throw std::runtime_error("Searching requires at least 1 thread.");
  }
  if (ignoreCase && nTransformThreads == 0) {
    throw std::runtime_error("Transforming requires at least 1 thread.");
  }

  std::vector<TaskAndNumThreads> tasksVector;

  if (nDecompressionThreads >= 1) {
    std::function<void(utils::FileChunk *)> decompressor = [](utils::FileChunk *chunk) -> void {
      utils::strtype data;
      data.resize(chunk->getOriginalSize());
      ZstdWrapper::decompressToBuffer(chunk->str().data(),
                                      chunk->str().length(),
                                      data.data(),
                                      chunk->getOriginalSize());
      chunk->setContent(std::move(data));
    };
    tasksVector.push_back(TaskAndNumThreads {"Decompressor", decompressor, nDecompressionThreads});
  }

  if (ignoreCase) {  // use ::toLower to perform case-insensitive search
    // default ignoreCase task for ESF
    std::function<void(utils::FileChunk *)> transformer = [](utils::FileChunk *chunk) -> void {
      utils::strtype data;
      data.resize(chunk->str().size());
      std::transform(chunk->str().begin(), chunk->str().end(), data.begin(), ::tolower);
      chunk->setContent(std::move(data));
    };
    tasksVector.push_back(TaskAndNumThreads {"::tolower", transformer, nTransformThreads});
  }

  std::vector<std::pair<uint64_t, std::vector<uint64_t>>> results;
  std::function<void(utils::FileChunk *)> searcher = [&results, &pattern](utils::FileChunk *chunk) -> void {
    std::pair<uint64_t, std::vector<uint64_t>> result = {chunk->getOffset(), {}};
    uint64_t shift = 0;
    while ((shift = chunk->str().find(pattern, shift)) != std::string::npos) {
      result.second.push_back(shift);
      shift += pattern.size();
    }
    results.push_back(result);
  };
  tasksVector.push_back(TaskAndNumThreads {"Searcher", searcher, nSearchThreads});

  _sf = StringFinder(std::move(reader), tasksVector, performanceMeasuring);
  _sf.find();

  std::vector<uint64_t> ret;
  for (auto &pair: results) {
    std::transform(pair.second.begin(), pair.second.end(),
                   pair.second.begin(),
                   [pair](uint64_t a) { return a + pair.first; }
    );
    ret.insert(ret.end(), pair.second.begin(), pair.second.end());
  }
  std::sort(ret.begin(), ret.end());
  return ret;
}

// _____________________________________________________________________________________________________________________
std::string InternStringFinder::toString() {
  return _sf.toString();
}

}  // namespace sf