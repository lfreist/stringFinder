// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <fcntl.h>

#include <istream>
#include <thread>
#include <algorithm>
#include <memory>

#include "./ExternStringFinder.h"
#include "./utils/ESFMetaFile.h"
#include "./utils/ZstdWrapper.h"
#include "./utils/readers/externalPOSIXreader.h"
#include "./utils/readers/externalIFSTREAMreader.h"

namespace sf {

ExternStringFinder::ExternStringFinder() = default;

std::vector<ulong> ExternStringFinder::find(const std::string &pattern,
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

  if (!metaFilePath.empty()) {  // we read an ESF metafile -> read compressed content
    std::shared_ptr<utils::ESFMetaFile> metaFilePtr = std::make_shared<utils::ESFMetaFile>(metaFilePath, std::ios::in);
    /*
    reader = [offset, filePtr, metaFilePtr]() mutable -> std::optional<utils::FileChunk> {
      sf::utils::chunkSize chunkSize = metaFilePtr->nextChunkSize();
      utils::strtype data;
      data.resize(chunkSize.compressedSize);
      if (filePtr->peek() == EOF) {
        return {};
      }
      filePtr->read(data.data(), chunkSize.compressedSize);
      utils::FileChunk chunk(data, chunkSize.originalSize, offset);
      // !offset is not num bytes read but their original size here!
      offset += chunkSize.originalSize;
      return chunk;
    };
    std::function<void(utils::FileChunk *)> decompressor = [](utils::FileChunk *chunk) -> void {
      utils::strtype data;
      data.resize(chunk->getOriginalSize());
      ZstdWrapper::decompressToBuffer(chunk->str().data(),
                                      chunk->str().length(),
                                      data.data(),
                                      chunk->getOriginalSize());
      chunk->setContent(std::move(data));
    };
    TaskAndNumThreads tnt;
    tnt.name = "Decompressor";
    tnt.task = decompressor;
    tnt.numThreads = nDecompressionThreads;
    tasksVector.push_back(tnt);
     */
  }
  else {  // no ESF meta file provided -> read plain text
    utils::readers::externalPOSIXreader posixReader(filePath);
    reader = std::make_unique<utils::readers::externalPOSIXreader>(std::move(posixReader));
    //utils::readers::externalIFSTREAMreader ifReader(filePath);
    //reader = std::make_unique<utils::readers::externalIFSTREAMreader>(std::move(ifReader));
  }

  if (ignoreCase) {  // use ::toLower to perform case-insensitive search
    // default ignoreCase task for ESF
    std::function<void(utils::FileChunk *)> transformer = [](utils::FileChunk *chunk) -> void {
      utils::strtype data;
      data.resize(chunk->str().size());
      std::transform(chunk->str().begin(), chunk->str().end(), data.begin(), ::tolower);
      chunk->setContent(std::move(data));
    };
    TaskAndNumThreads tntTransform;
    tntTransform.name = "::tolower";
    tntTransform.task = transformer;
    tntTransform.numThreads = nTransformThreads;
    tasksVector.push_back(tntTransform);
  }

  // default search task for ESF
  std::vector<std::pair<uint64_t, std::vector<uint64_t>>> results;
  std::function<void(utils::FileChunk *)> searcher = [&results, &pattern](utils::FileChunk *chunk) -> void {
    std::pair<ulong, std::vector<ulong>> result = {chunk->getOffset(), {}};
    ulong shift = 0;
    while ((shift = chunk->str().find(pattern, shift)) != std::string::npos) {
      result.second.push_back(shift);
      shift += pattern.size();
    }
    results.push_back(result);
  };
  TaskAndNumThreads tntSearch;
  tntSearch.name = "Searcher";
  tntSearch.task = searcher;
  tntSearch.numThreads = nSearchThreads;
  tasksVector.push_back(tntSearch);

  _sf = StringFinder(std::move(reader), tasksVector, performanceMeasuring);

  _sf.find();

  std::vector<ulong> ret;
  for (auto &pair: results) {
    std::transform(pair.second.begin(), pair.second.end(),
                   pair.second.begin(),
                   [pair](ulong a) { return a + pair.first; }
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