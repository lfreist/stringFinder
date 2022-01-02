// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <vector>

#include <cassert>

#include "String.h"

// ____________________________________________________________________________________________________________________
String::String() {
  _len = 0;
  _content = new char [1];
  _content[0] = '\0';
}

// ____________________________________________________________________________________________________________________
String::String(unsigned int len) {
  _len = len;
  _content = new char [len+1];
  _content[len] = '\0';
}

// ____________________________________________________________________________________________________________________
String::String(const String& str) {
  _content = nullptr;
  _len = str._len;
  set(str._content);
}

// ____________________________________________________________________________________________________________________
String::String(const char* str) {
  _content = nullptr;
  set(str);
}

// ____________________________________________________________________________________________________________________
String::~String() {
  delete[] _content;
}

// ____________________________________________________________________________________________________________________
void String::set(const char* str) {
  delete[] _content;
  _len = strlen(str);
  _content = new char [_len+1];
  strcpy(_content, str);
}

// ____________________________________________________________________________________________________________________
int String::read(FILE* fp) {
  return (int) fread(_content, sizeof(char), _len, fp);
}

// ____________________________________________________________________________________________________________________
int String::readToNewLine(FILE* fp, unsigned int minNumBytes) {
  assert(minNumBytes <= _len);
  char additional_char;
  size_t bytes_read = fread(_content, sizeof(char), minNumBytes, fp);
  if (bytes_read == 0) { return 0; }
  for (auto i = (unsigned int) bytes_read; i < _len; i++) {
    if ((additional_char = (char) fgetc(fp)) == EOF) {
      _content[i] = '\0';
      return i;
    }
    if (additional_char == '\n') {
      _content[i] = additional_char;
      _content[i+1] = '\0';
      return i;
    }
    _content[i] = additional_char;
  }
  return -1;
}

// ____________________________________________________________________________________________________________________
int String::findCaseSensitive(const String& pattern, unsigned int shift) {
  assert(shift <= _len);
  char* match = strstr(_content+shift, pattern._content);
  if (match == nullptr) {
  return -1;
  }
  return strlen(_content) - strlen(match);
}

// ____________________________________________________________________________________________________________________
int String::findCaseInsensitive(const String& pattern, unsigned int shift) {
  assert(shift <= _len);
  char* match = strcasestr(_content+shift, pattern._content);
	if (match == nullptr) {
      return -1;
	}
	return _len - strlen(match);
}

// ____________________________________________________________________________________________________________________
std::vector<unsigned int> String::findPerLineCaseSensitive(const String& pattern) {
  std::vector<unsigned int> matches;
  int matchPosition = 0;
  int newLinePosition;
  while (true) {
    matchPosition = findCaseSensitive(pattern, matchPosition);
    if (matchPosition < 0) {
      break;
    }
    matches.push_back(matchPosition);
    newLinePosition = findNewLine(matchPosition);
    if (newLinePosition < 0) {
      break;
    }
    matchPosition += newLinePosition;
  }
  return matches;
}

// ____________________________________________________________________________________________________________________
std::vector<unsigned int> String::findPerLineCaseInsensitive(const String& pattern) {
  std::vector<unsigned int> matches;
  int matchPosition = 0;
  int newLinePosition;
  while (true) {
    matchPosition = findCaseInsensitive(pattern, matchPosition);
    if (matchPosition < 0) {
      break;
    }
    matches.push_back(matchPosition);
    newLinePosition = findNewLine(matchPosition);
    if (newLinePosition < 0) {
      break;
    }
    matchPosition += newLinePosition;
  }
  return matches;
}

// ____________________________________________________________________________________________________________________
int String::findNewLine(unsigned int shift) {
  assert(shift <= _len);
  char* match = strchr(_content+shift, '\n');
  if (match == nullptr) {
    return -1;
  }
  return strlen(_content+shift) - strlen(match);
}

// ____________________________________________________________________________________________________________________
const char* String::cstring() {
  return _content;
}

// ____________________________________________________________________________________________________________________
bool String::operator==(String compStr) {
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
bool String::operator!=(String compStr) {
  return !(*this == compStr);
}
