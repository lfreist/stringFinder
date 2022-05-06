// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>

using std::string;

namespace stdstring_alg {
int findAllPerLine(const string &pattern, const string &content) {
  int matches = 0;
  size_t shift = 0;
  while (true) {
    shift = content.find(pattern, shift) + pattern.length();
    if (shift == string::npos) {
      break;
    }
    matches++;
    shift = content.find('\n', shift) + 1;
  }
  return matches;
}

int findAll(const string &pattern, const string &content) {
  int matches = 0;
  string::size_type shift = 0;
  std::cout << pattern << std::endl;
  while ((shift = content.find(pattern, shift)) != string::npos) {
    std::cout << shift << std::endl;
    matches++;
    shift += pattern.length();
  }
  return matches;
}
}
