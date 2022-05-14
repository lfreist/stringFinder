// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>

using std::string;

namespace stdstring_alg {
int findAllPerLine(const string &pattern, const string &content) {
  int matches = 0;
  size_t shift = 0;
  while ((shift = content.find(pattern, shift)) != string::npos) {
    matches++;
    shift = content.find('\n', shift);
    if (shift == string::npos) {
      break;
    }
    shift++;
  }
  return matches;
}

int findAll(const string &pattern, const string &content) {
  int matches = 0;
  string::size_type shift = 0;
  while ((shift = content.find(pattern, shift)) != string::npos) {
    matches++;
    shift += pattern.length();
  }
  return matches;
}
} // end namespace
