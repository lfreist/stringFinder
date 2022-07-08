// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <fcntl.h>

#include <iostream>
#include <istream>
#include <thread>
#include <algorithm>
#include <fstream>

#include "ExternStringFinder.h"
#include "utils/ESFMetaFile.h"
#include "utils/ZstdWrapper.h"

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

  std::ifstream file(filePath);
  if (!file) {
    throw std::runtime_error("file '" + filePath + "' not found");
  }

  // for use with POSIX ::read()
  // TODO: evaluate! If multiple reader threads are implemented this might be better compared to std::ifstream
  utils::strtype lastOverhead;
  lastOverhead.reserve(1024);
  int fd = ::open(filePath.c_str(), O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error("file '" + filePath + "' not found");
  }
  // ---

  std::vector<TaskAndNumThreads> tasksVector;

  std::shared_ptr<std::ifstream> filePtr = std::make_shared<std::ifstream>(std::move(file));

  uint64_t offset = 0;

  // default reader for ESF
  std::function<std::optional<utils::FileChunk>(void)> reader;

  if (!metaFilePath.empty()) {  // we read an ESF metafile -> read compressed content
    std::shared_ptr<utils::ESFMetaFile> metaFilePtr = std::make_shared<utils::ESFMetaFile>(metaFilePath, std::ios::in);
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
  }
  else {  // no ESF meta file provided -> read plain text
    /*
    reader = [minNumBytes = 16777216, maxNumBytes = 16842752, offset, filePtr]() mutable -> std::optional<utils::FileChunk> {
      std::optional<utils::FileChunk> chunkOpt {utils::FileChunk()};
      auto& chunk = chunkOpt.value();
      chunk.strPtr()->reserve(maxNumBytes);
      chunk.strPtr()->resize(minNumBytes);
      if (filePtr->peek() == EOF) {
        return {};
      }
      filePtr->read(chunk.strPtr()->data(), minNumBytes);
      if (!(*filePtr)) {
        chunk.strPtr()->resize(filePtr->gcount());
        chunk.strPtr()->pop_back();
      }
      else {
        char nextByte = 0;
        while (true) {
          if (filePtr->eof()) { break; }
          filePtr->get(nextByte);
          if (nextByte == '\n') { break; }
          chunk.strPtr()->push_back(nextByte);
        }
      }
      chunk.setOffset(offset);
      offset += chunk.strPtr()->size();
      return chunkOpt;
    };
    */
    reader = [maxNumBytes = 16777216, offset, fd, &lastOverhead]() mutable -> std::optional<utils::FileChunk> {
      std::optional<utils::FileChunk> chunkOpt{utils::FileChunk(lastOverhead, offset)};
      auto &chunk = chunkOpt.value();
      chunk.strPtr()->resize(maxNumBytes);
      auto bytes_read = ::read(fd, &chunk.strPtr()->at(lastOverhead.length()), maxNumBytes - (lastOverhead.length()));
      if (bytes_read == 0) {
        return {};
      }
      lastOverhead.resize(0);
      for (unsigned index = maxNumBytes - 1; index > 0; --index) {
        char current = chunk.strPtr()->at(index);
        chunk.strPtr()->pop_back();
        if (current == '\n') {
          break;
        }
        lastOverhead.push_back(current);
      }
      if (chunk.strPtr()->empty()) {
        throw std::runtime_error("Could not find new line.");
      }
      offset += chunk.strPtr()->size();
      return chunkOpt;
    };
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

  filePtr->close();
  ::close(fd);

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