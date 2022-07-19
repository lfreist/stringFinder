// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <fcntl.h>

#include <istream>
#include <algorithm>

#include "./ExternStringFinder.h"
#include "./utils/SFMetaFile.h"
#include "./utils/ZstdWrapper.h"
#include "./utils/readers/ESFPOSIXReader.h"
#include "./utils/readers/ESFIFSTREAMReader.h"
#include "./utils/readers/ESFCompressedPOSIXReader.h"

namespace sf {

std::vector<uint64_t> ExternStringFinder::find(const std::string &pattern,
                                            const std::string &filePath,
                                            const std::string &metaFilePath,
                                            bool ignoreCase,
                                            bool performanceMeasuring,
                                            unsigned nDecompressionThreads,
                                            unsigned nTransformThreads,
                                            unsigned nSearchThreads) {
  if (!metaFilePath.empty() && nDecompressionThreads == 0) {
    throw std::runtime_error("Decompression requires at least 1 thread.");
  }
  if (nSearchThreads == 0) {
    throw std::runtime_error("Searching requires at least 1 thread.");
  }
  if (ignoreCase && nTransformThreads == 0) {
    throw std::runtime_error("Transforming requires at least 1 thread.");
  }

  std::vector<TaskAndNumThreads> tasksVector;

  // default reader for ESF
  std::unique_ptr<utils::readers::BaseReader> reader;

  if (!metaFilePath.empty()) {  // we have an ESF metafile -> read compressed content
    reader =
      std::make_unique<utils::readers::ESFCompressedPOSIXReader>(utils::readers::ESFCompressedPOSIXReader(
        filePath,
        metaFilePath));

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
  } else {  // no ESF meta file provided -> read plain text
    // POSIX read:
    reader = std::make_unique<utils::readers::ESFPOSIXReader>(utils::readers::ESFPOSIXReader(filePath));
    // IFSTREAM read:
    //reader = std::make_unique<utils::readers::ExternalIFSTREAMreader>(utils::readers::ExternalIFSTREAMreader(filePath));
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

  // default search task for ESF
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
std::string ExternStringFinder::toString() {
  return _sf.toString();
}

}  // namespace sf