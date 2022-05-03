// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <cassert>
#include <vector>

#include "FileChunk.h"
#include "ZstdWrapper.h"

#include <iostream>

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk() {
  _len = 0;
  _bufferSize = 1;
  _content = new char[_bufferSize];
  _content[0] = '\0';
  _globalShift = 0;
}

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(unsigned int bufferSize) {
  _len = bufferSize - 1;
  _bufferSize = bufferSize;
  _content = new char[_bufferSize];
  _content[_bufferSize - 1] = '\0';
  _compressedContent.resize(1<<26);
  _globalShift = 0;
}

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(const FileChunk &buffer) {
  _len = buffer._len;
  _bufferSize = buffer._bufferSize;
  _globalShift = buffer._globalShift;
  _content = new char[_bufferSize];
  strcpy(_content, buffer._content);
}

// _____________________________________________________________________________________________________________________
FileChunk::FileChunk(const char *str) {
  _len = strlen(str);
  _bufferSize = _len + 1;
  _content = new char[_bufferSize];
  strcpy(_content, str);
  _globalShift = 0;
}

// _____________________________________________________________________________________________________________________
FileChunk::~FileChunk() {
  delete[] _content;
}

// _____________________________________________________________________________________________________________________
void FileChunk::setContent(const char *content) {
  delete[] _content;
  _len = strlen(content);
  _bufferSize = _len + 1;
  _content = new char[_bufferSize];
  strcpy(_content, content);
}

// _____________________________________________________________________________________________________________________
int FileChunk::setContentFromFile(FILE *fp, unsigned int minNumBytes, bool toNewLine, bool zstdCompressed,
                                  size_t originalSize, unsigned globalShift) {
  assert(minNumBytes <= _bufferSize);
  _globalShift = globalShift;
  if (minNumBytes == 0) {
    return 0;
  }
  if (zstdCompressed) {
    assert(originalSize != 0);
    _originalSize = originalSize;
    size_t bytes_read = fread(&_compressedContent[0], sizeof(char), minNumBytes, fp);
    _compressedSize = bytes_read;
    return (int) bytes_read;
  }
  if (toNewLine) {
    char additional_char;
    size_t bytes_read = fread(_content, sizeof(char), minNumBytes, fp);
    if (bytes_read == 0) {
      _content[0] = '\0';
      _len = 0;
      return 0;
    } else if (bytes_read < minNumBytes) {
      bytes_read--;
      _content[bytes_read] = '\0';
      _len = bytes_read;
      return (int) bytes_read;
    } else if (_content[bytes_read - 1] == '\n') {
      _content[bytes_read] = '\0';
      _len = bytes_read;
      return (int) bytes_read;
    }
    // read until new line
    for (auto i = (unsigned int) bytes_read; i < _bufferSize; i++) {
      _len = i + 1;
      additional_char = (char) fgetc(fp);
      if (additional_char == '\n' || additional_char == EOF) {
        _content[i] = additional_char;
        _content[i + 1] = '\0';
        return (int) i;
      }
      _content[i] = additional_char;
    }
    // overflow: Buffer is full and still no new line found
    _content[_bufferSize - 1] = '\0';
    return -1;
  }
  size_t bytes_read = fread(_content, sizeof(char), minNumBytes, fp);
  if (bytes_read == 0) {
    _content[0] = '\0';
    _len = 0;
    return 0;
  } else if (bytes_read < minNumBytes) {
    bytes_read--;
  }
  _content[bytes_read] = '\0';
  _len = bytes_read;
  return (int) bytes_read;
}

// _____________________________________________________________________________________________________________________
int FileChunk::find(const char *pattern, unsigned shift, bool caseSensitive) {
  assert(shift <= _len);
  char *match;
  if (caseSensitive) {
    match = strstr(_content + shift, pattern);
  } else {
    match = strcasestr(_content + shift, pattern);
  }
  if (match == nullptr) {
    return -1;
  }
  return _len - strlen(match);
}

// _____________________________________________________________________________________________________________________
std::vector<unsigned> FileChunk::findPerLine(const char *pattern, bool caseSensitive) {
  std::vector<unsigned> matches;
  int matchPosition = 0;
  int newLinePosition;
  while (true) {
    matchPosition = find(pattern, matchPosition, caseSensitive);
    if (matchPosition < 0) {
      break;
    }
    matches.push_back(matchPosition + _globalShift);
    newLinePosition = findNewLine(matchPosition);
    if (newLinePosition < 0) {
      break;
    }
    matchPosition += newLinePosition;
  }
  return matches;
}

// _____________________________________________________________________________________________________________________
int FileChunk::findNewLine(unsigned shift) {
  assert(shift <= _len);
  char *match = strchr(_content + shift, '\n');
  if (match == nullptr) {
    return -1;
  }
  return strlen(_content + shift) - strlen(match);
}

// _____________________________________________________________________________________________________________________
const char *FileChunk::cstring() {
  return _content;
}

// _____________________________________________________________________________________________________________________
bool FileChunk::operator==(const FileChunk &compStr) {
  if (_len != compStr._len) {
    return false;
  }
  for (unsigned int i = 0; i < _len; i++) {
    if (_content[i] != compStr._content[i]) {
      return false;
    }
  }
  return true;
}

// _____________________________________________________________________________________________________________________
bool FileChunk::operator!=(const FileChunk &compStr) {
  return !(operator==(compStr));
}

// _____________________________________________________________________________________________________________________
unsigned FileChunk::length() const {
  return _len;
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::compress(int compressionLevel) {
  _compressedContent = ZstdWrapper::compress(_content, _len, compressionLevel);
  _originalSize = _len;
  _compressedSize = _compressedContent.size();
  return _compressedSize;
}

// _____________________________________________________________________________________________________________________
size_t FileChunk::decompress() {
  if (_bufferSize < _originalSize + 1) {
    _bufferSize = _originalSize + 1;
    delete[] _content;
    _content = new char[_bufferSize+1];
  }
  ZstdWrapper::decompressToBuffer(
      _compressedContent.data(),
      _compressedSize,
      _content,
      _bufferSize
  );
  _content[_originalSize] = '\0';
  _len = _originalSize;
  return _len;
}

// _____________________________________________________________________________________________________________________
void FileChunk::setOriginalSize(unsigned origSize) {
  _originalSize = origSize;
}

// _____________________________________________________________________________________________________________________
unsigned int FileChunk::getOriginalSize() const {
  return _originalSize;
}