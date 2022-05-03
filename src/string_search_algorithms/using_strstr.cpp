// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstring>

#include "using_strstr.h"

int findNextNewLine(char* content, unsigned shift) {
  char *match = strchr(content + shift, '\n');
  return match == nullptr ? -1 : (strlen(content) - strlen(match));
}

int findNext(const char* pattern, char* content, unsigned shift) {
  char* match = strstr(content + shift, pattern);
  return match == nullptr ? -1 : (strlen(content) - strlen(match));
}

int findAllPerLine(const char* pattern, char* content) {
  int count = 0;
  unsigned shift = 0;
  int patternLen = strlen(pattern);
  while (true) {
    int match = findNext(pattern, content, shift);
    if (match == -1) {
      break;
    }
    count++;
    shift = match + patternLen;
    match = findNextNewLine(content, shift);
    if (match == -1) {
      break;
    }
    shift = ++match;
  }
  return count;
}

int findAll(const char* pattern, char* content) {
  int count = 0;
  unsigned shift = 0;
  size_t patternLen = strlen(pattern);
  while (true) {
    int match = findNext(pattern, content, shift);
    if (match == -1) {
      break;
    }
    count++;
    shift = match + patternLen;
  }
  return count;
}
