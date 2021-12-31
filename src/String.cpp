// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstring>
#include <vector>
#include <iostream>

#include <assert.h>

#include "String.h"

String::String() {
  _len = 0;
  _content = new char [1];
  _content[0] = '\0';
}

String::String(const String& str) {
	_content = nullptr;
  set(str._content);
}

String::String(const char* str) {
	_content = nullptr;
  set(str);
}

String::~String() {
	delete[] _content;
}

void String::set(const char* str) {
  delete[] _content;
	_len = strlen(str);
	_content = new char [_len+1];
	strcpy(_content, str);
}

int String::findCaseSensitive(String pattern, unsigned int shift) {
	assert(shift <= _len);
  char* match = strstr(_content+shift, pattern._content);
	if (match == nullptr) {
		return -1;
	}
	return _len - strlen(match);
}

int String::findCaseInsensitive(String pattern, unsigned int shift) {
	assert(shift <= _len);
  char* match = strcasestr(_content+shift, pattern._content);
	if (match == nullptr) {
		return -1;
	}
	return _len - strlen(match);
}

std::vector<unsigned int> String::findPerLineCaseSensitive(String pattern) {
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

std::vector<unsigned int> String::findPerLineCaseInsensitive(String pattern) {
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

int String::findNewLine(unsigned int shift) {
	assert(shift <= _len);
  char* match = strchr(_content+shift, '\n');
	if (match == nullptr) {
		return -1;
	}
	return strlen(_content+shift) - strlen(match);
}

const char* String::cstring() {
	return _content;
}

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

bool String::operator!=(String compStr) {
	return !(*this == compStr);
}
