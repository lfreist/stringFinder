// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "FileChunk.h"
#include "ZstdWrapper.h"
#include "../string_search_algorithms/using_stdstring.h"

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk() {
  _isUncompressed = false;
  _isCompressed = false;
  _offset = 0;
}

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(string &uncompressedContent) {
  _uncompressedContent = uncompressedContent;
  _originalSize = _uncompressedContent.length();
  _isUncompressed = true;
  _isCompressed = false;
  _offset = 0;
}

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(vector<char> compressedContent, size_t originalSize) {
  _compressedContent = compressedContent;
  _originalSize = originalSize;
  _isUncompressed = false;
  _isCompressed = true;
  _offset = 0;
}

// _____________________________________________________________________________________________________________________
FileChunk::~FileChunk() = default;

// _____________________________________________________________________________________________________________________
void FileChunk::setContent(const string uncompressedContent) {
  _uncompressedContent = uncompressedContent;
  _originalSize = _uncompressedContent.length();
  _isUncompressed = true;
  _isCompressed = false;
  _offset = 0;
}

// _____________________________________________________________________________________________________________________
void FileChunk::setContent(const vector<char> compressedContent, size_t originalSize) {
  _compressedContent = compressedContent;
  _originalSize = originalSize;
  _isUncompressed = false;
  _isCompressed = true;
  _offset = 0;
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::setContentFromFile(std::ifstream &file, std::streamsize minNumBytes, bool toNewLine) {
  if (file.peek() == EOF) {
    _uncompressedContent = "";
    _isUncompressed = true;
    _isCompressed = false;
    _originalSize = length();
    return length();
  }
  if (minNumBytes == 0) {  // read to end of file
    _uncompressedContent.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    _uncompressedContent.pop_back();
    _isUncompressed = true;
    _isCompressed = false;
    _originalSize = length();
    return length();
  }
  _uncompressedContent.resize(minNumBytes);;
  file.read(_uncompressedContent.data(), minNumBytes);
  if (!file) {
    _uncompressedContent.resize(file.gcount());
    _uncompressedContent.pop_back();
    _isUncompressed = true;
    _isCompressed = false;
    _originalSize = length();
  }
  if (toNewLine) {
    char nextByte = 0;
    while(true) {
      if (file.eof()) { break; }
      file.get(nextByte);
      if (nextByte == '\n') { break; }
      _uncompressedContent += nextByte;
    }
  }
  _isUncompressed = true;
  _isCompressed = false;
  _originalSize = length();
  return length();
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::setContentFromZstdFile(std::ifstream &file, size_t originalSize, std::streamsize numBytes) {
  if (file.peek() == EOF) {
    _compressedContent.resize(0);
    _isUncompressed = false;
    _isCompressed = true;
    _originalSize = 0;
    return 0;
  }
  if (numBytes == 0) {  // read to end of file
    _compressedContent.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    _originalSize = originalSize;
    _isUncompressed = false;
    _isCompressed = true;
    return compressedLength();
  }
  _compressedContent.resize(numBytes);
  file.read((char*) &_compressedContent[0], numBytes);
  _originalSize = originalSize;
  _isUncompressed = false;
  _isCompressed = true;
  return compressedLength();
}

// _____________________________________________________________________________________________________________________
vector<string::size_type> FileChunk::searchAll(string &pattern) {
  if (!_isUncompressed) { throw NotUncompressedException(); }
  vector<string::size_type> result;
  string::size_type shift = 0;
  while ((shift = search(pattern, shift)) != string::npos) {
    result.push_back(shift);
    shift += pattern.length();
  }
  return result;
}

// _____________________________________________________________________________________________________________________
vector<string::size_type> FileChunk::searchAllPerLine(string &pattern) {
  if (!_isUncompressed) { throw NotUncompressedException(); }
  vector<string::size_type> result;
  size_t shift = 0;
  while ((shift = search(pattern, shift)) != string::npos) {
    result.push_back(shift);
    shift += pattern.length();
    shift = _uncompressedContent.find('\n', shift);
    if (shift == string::npos) {
      break;
    }
    shift++;
  }
  return result;
}

// _____________________________________________________________________________________________________________________
string::size_type FileChunk::length() const {
  return _uncompressedContent.length();
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::compressedLength() const {
  if (!_isCompressed) { return 0; }
  return _compressedContent.size();
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::compress(int compressionLevel) {
  if (_isCompressed) {
    return compressedLength();
  }
  if (!_isUncompressed) { throw NotUncompressedException(); }
  _compressedContent = ZstdWrapper::compress(_uncompressedContent.data(), length(), compressionLevel);
  _isCompressed = true;
  return compressedLength();
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::decompress(size_t originalSize) {
  if (_isUncompressed) {
    return length();
  }
  if (!_isCompressed) { throw NotCompressedException(); }
  originalSize = originalSize == 0 ? _originalSize : originalSize;
  auto uncompressedData = ZstdWrapper::decompress<char>(_compressedContent.data(), compressedLength(), originalSize);
  _uncompressedContent.assign(uncompressedData.begin(), uncompressedData.end());
  _isUncompressed = true;
  return length();
}

// _____________________________________________________________________________________________________________________
vector<char> FileChunk::getCompressedContent() {
  if (!_isCompressed) {
    compress();
  }
  return _compressedContent;
}

// _____________________________________________________________________________________________________________________
string FileChunk::getUncompressedContent() {
  if (!_isUncompressed) {
    decompress(_originalSize);
  }
  return _uncompressedContent;
}

// _____________________________________________________________________________________________________________________
string::size_type FileChunk::search(string &pattern, size_t offset) {
  return _uncompressedContent.find(pattern, offset);
}

// _____________________________________________________________________________________________________________________
bool FileChunk::operator==(const FileChunk &compChunk) {
  if (length() != compChunk.length()) {
    return false;
  }
  if (_isUncompressed) {
    return _uncompressedContent == compChunk._uncompressedContent;
  }
  if (_isCompressed) {
    return _compressedContent == compChunk._compressedContent;
  }
  return true;
}

// _____________________________________________________________________________________________________________________
bool FileChunk::operator!=(const FileChunk &compStr) {
  return !(operator==(compStr));
}
