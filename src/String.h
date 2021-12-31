// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <vector>

#pragma once
class String {
 public:
  String();
  String(const String& str);
  String(const char* str);
  ~String();

  void set(const char* str);
  int findCaseSensitive(String pattern, unsigned int shift = 0);
  int findCaseInsensitive(String pattern, unsigned int shift = 0);
  std::vector<unsigned int> findPerLineCaseSensitive(String pattern);
  std::vector<unsigned int> findPerLineCaseInsensitive(String Pattern);
  const char* cstring();

  bool operator==(String compStr);
  bool operator!=(String compStr);

 private:
  int findNewLine(unsigned int shift);

  char* _content;
  unsigned _len;

  FRIEND_TEST(StringTest, Constructor);
};
