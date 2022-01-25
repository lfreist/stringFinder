// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <vector>

#include <cassert>

#include "Buffer.h"

// ____________________________________________________________________________________________________________________
Buffer::Buffer() {
  _len = 0;
  _bufferSize = 1;
  _content = new char [_bufferSize];
  _content[0] = '\0';
}

// ____________________________________________________________________________________________________________________
Buffer::Buffer(unsigned int bufferSize) {
  _len = bufferSize-1;
  _bufferSize = bufferSize;
  _content = new char [_bufferSize];
  _content[_bufferSize-1] = '\0';
}

// ____________________________________________________________________________________________________________________
Buffer::Buffer(const Buffer& buffer) {
  _len = buffer._len;
  _bufferSize = buffer._bufferSize;
  _content = new char [_bufferSize];
  strcpy(_content, buffer._content);
}

// ____________________________________________________________________________________________________________________
Buffer::Buffer(const char* str) {
  _len = strlen(str);
  _bufferSize = _len+1;
  _content = new char [_bufferSize];
  strcpy(_content, str);
}

// ____________________________________________________________________________________________________________________
Buffer::~Buffer() {
  delete[] _content;
}

// ____________________________________________________________________________________________________________________
void Buffer::setContent(const char* content) {
  delete[] _content;
  _len = strlen(content);
  _bufferSize = _len+1;
  _content = new char [_bufferSize];
  strcpy(_content, content);
}

// ____________________________________________________________________________________________________________________
int Buffer::setContentFromFile(FILE* fp, unsigned int minNumBytes, bool toNewLine) {
  assert(minNumBytes <= _len);
  if (toNewLine) {
    char additional_char;
    size_t bytes_read = fread(_content, sizeof(char), minNumBytes, fp);
    if (bytes_read == 0) { return 0; }
    for (auto i = (unsigned int) bytes_read; i < _len; i++) {
      if ((additional_char = (char) fgetc(fp)) == EOF) {
        _content[i] = '\0';
        _len = i;
        return i;
      }
      if (additional_char == '\n') {
        _content[i] = additional_char;
        _content[i+1] = '\0';
        _len = i;
        return i;
      }
      _content[i] = additional_char;
    }
    return -1;
  }
  size_t bytes_read = fread(_content, sizeof(char), _len, fp);
  _content[bytes_read] = '\0';
  _len = bytes_read;
  return (int) bytes_read;
}

// ____________________________________________________________________________________________________________________
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

// ____________________________________________________________________________________________________________________
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

// ____________________________________________________________________________________________________________________
int Buffer::findNewLine(unsigned int shift) {
  assert(shift <= _len);
  char* match = strchr(_content+shift, '\n');
  if (match == nullptr) {
    return -1;
  }
  return strlen(_content+shift) - strlen(match);
}

// ____________________________________________________________________________________________________________________
const char* Buffer::cstring() {
  return _content;
}

// ____________________________________________________________________________________________________________________
bool Buffer::operator==(Buffer compStr) {
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

// ____________________________________________________________________________________________________________________
bool Buffer::operator!=(Buffer compStr) {
  return !(operator==(compStr));
}
