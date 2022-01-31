// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <cassert>
#include <vector>

#include "Buffer.h"
#include "ZstdWrapper.h"

#include <iostream>

// _____________________________________________________________________________________________________________________
Buffer::Buffer() {
  _len = 0;
  _bufferSize = 1;
  _content = new char [_bufferSize];
  _content[0] = '\0';
}

// _____________________________________________________________________________________________________________________
Buffer::Buffer(unsigned int bufferSize) {
  _len = bufferSize-1;
  _bufferSize = bufferSize;
  _content = new char [_bufferSize];
  _content[_bufferSize-1] = '\0';
}

// _____________________________________________________________________________________________________________________
Buffer::Buffer(const Buffer& buffer) {
  _len = buffer._len;
  _bufferSize = buffer._bufferSize;
  _content = new char [_bufferSize];
  strcpy(_content, buffer._content);
}

// _____________________________________________________________________________________________________________________
Buffer::Buffer(const char* str) {
  _len = strlen(str);
  _bufferSize = _len+1;
  _content = new char [_bufferSize];
  strcpy(_content, str);
}

// _____________________________________________________________________________________________________________________
Buffer::~Buffer() {
  delete[] _content;
}

// _____________________________________________________________________________________________________________________
void Buffer::setContent(const char* content) {
  delete[] _content;
  _len = strlen(content);
  _bufferSize = _len+1;
  _content = new char [_bufferSize];
  strcpy(_content, content);
}

// _____________________________________________________________________________________________________________________
int Buffer::setContentFromFile(FILE* fp, unsigned int minNumBytes, bool toNewLine) {
  assert(minNumBytes <= _len);
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
    } else if (_content[bytes_read-1] == '\n') {
      _content[bytes_read] = '\0';
      _len = bytes_read;
      return (int) bytes_read;
    }
    // read until new line
    for (auto i = (unsigned int) bytes_read; i < _bufferSize; i++) {
      _len = i+1;
      additional_char = (char) fgetc(fp);
      if (additional_char == '\n' || additional_char == EOF) {
        // TODO: Question: additional_char is never EOF. At the end of the file a nl-char is read (out of nowhere?)
        _content[i] = additional_char;
        _content[i+1] = '\0';
        return (int) i;
      }
      _content[i] = additional_char;
    }
    // overflow: Buffer is full and still no new line found
    std::cout << _content << std::endl;
    _content[_bufferSize-1] = '\0';
    return -1;
  }
  size_t bytes_read = fread(_content, sizeof(char), minNumBytes, fp);
  if (bytes_read == 0) {
    _content[0] = '\0';
    _len = 0;
    return 0;
  } else if (bytes_read < minNumBytes) {  // skip EOF (new line - see TODO described in 84)
    bytes_read--;
  }
  _content[bytes_read] = '\0';
  _len = bytes_read;
  return (int) bytes_read;
}

// _____________________________________________________________________________________________________________________
int Buffer::find(const char* pattern, unsigned int shift, bool caseSensitive) {
  assert(shift <= _len);
  char* match;
  if (caseSensitive) {
    match = strstr(_content+shift, pattern);
  } else {
    match = strcasestr(_content+shift, pattern);
  }
  if (match == nullptr) {
  return -1;
  }
  return _len - strlen(match);
}

// _____________________________________________________________________________________________________________________
std::vector<unsigned int> Buffer::findPerLine(const char* pattern, unsigned int bytePositionShift,
                                              bool caseSensitive) {
  std::vector<unsigned int> matches;
  int matchPosition = 0;
  int newLinePosition;
  while (true) {
    matchPosition = find(pattern, matchPosition, caseSensitive);
    if (matchPosition < 0) {
      break;
    }
    matches.push_back(matchPosition+bytePositionShift);
    newLinePosition = findNewLine(matchPosition);
    if (newLinePosition < 0) {
      break;
    }
    matchPosition += newLinePosition;
  }
  return matches;
}

// _____________________________________________________________________________________________________________________
int Buffer::findNewLine(unsigned int shift) {
  assert(shift <= _len);
  char* match = strchr(_content+shift, '\n');
  if (match == nullptr) {
    return -1;
  }
  return strlen(_content+shift) - strlen(match);
}

// _____________________________________________________________________________________________________________________
const char* Buffer::cstring() {
  return _content;
}

// _____________________________________________________________________________________________________________________
bool Buffer::operator==(const Buffer& compStr) {
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
bool Buffer::operator!=(const Buffer& compStr) {
  return !(operator==(compStr));
}

// _____________________________________________________________________________________________________________________
unsigned int Buffer::length() const {
  return _len;
}

// _____________________________________________________________________________________________________________________
size_t Buffer::compress(int compressionLevel) {
  std::vector<char> comp = ZstdWrapper::compress(_content, _len, compressionLevel);
  setContent(reinterpret_cast<char*>(comp.data()));
  return _len;
}

// _____________________________________________________________________________________________________________________
size_t Buffer::decompress(size_t originalSize) {
  std::vector<char> decomp = ZstdWrapper::decompress<char>(_content, _len, originalSize);
  setContent(reinterpret_cast<char*>(decomp.data()));
  return _len;
}

// TODO: implement if needed or delete
size_t Buffer::compressToBuffer(int compressionLevel, Buffer& buffer) {
  return 0;
}

size_t Buffer::decompressBuffer(size_t originalSize, Buffer& buffer) {
  return 0;
}
