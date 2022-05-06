// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

namespace strstr_alg {
/**
 * @brief get (absolute) position of next new line character ('\n') in content starting at content[shift]
 *
 * @param content [char*]: char array in which nl is searched
 * @param shift [unsigned]: start searching at content[shift]
 * @return [int]: -1 if nl char was not found, else position of next nl char in content
 */
int findNextNewLine(char* content, unsigned shift) {
  char *match = strchr(content + shift, '\n');
  return match == nullptr ? -1 : (strlen(content) - strlen(match));
}

/**
 * @brief get (absolute) position of next pattern in content starting at content[shift]
 *
 * @param pattern [char*]: pattern to be searched for
 * @param content [char*]: char array in which pattern is searched
 * @param shift [unsigned]: start searching at content[shift]
 * @return [int]: -1 if pattern was not found, else position of next pattern match in content
 */
int findNext(const char* pattern, char* content, unsigned shift) {
  char* match = strstr(content + shift, pattern);
  return match == nullptr ? -1 : (strlen(content) - strlen(match));
}

int findAllPerLine(const char* pattern, char* content) {
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
}
