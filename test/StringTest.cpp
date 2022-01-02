// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

// TODO(lfreist): write tests for read() and readToNewLine()

#include <gtest/gtest.h>

#include <cstdio>

#include "../src/String.h"

// ____________________________________________________________________________________________________________________
TEST(StringTest, Constructor) {
  {
    String str;
    ASSERT_EQ(str._len, 0);
    ASSERT_EQ(str._content[0], '\0');
  }
  {
    String str = String(5);
    ASSERT_EQ(str._len, 5);
    ASSERT_EQ(str._content[5], '\0');
  }
  {
    String str1;
    str1.set("test");
    String str2 = String(str1);
    ASSERT_EQ(str1._len, 4);
    ASSERT_EQ(str2._len, str1._len);
    ASSERT_EQ(str2._content[0], 't');
    ASSERT_EQ(str2._content[1], 'e');
    ASSERT_EQ(str2._content[2], 's');
    ASSERT_EQ(str2._content[3], 't');
  }
  {
    String str = String("test");
    ASSERT_EQ(str._len, 4);
    ASSERT_EQ(str._content[0], 't');
    ASSERT_EQ(str._content[1], 'e');
    ASSERT_EQ(str._content[2], 's');
    ASSERT_EQ(str._content[3], 't');
  }
}

// ____________________________________________________________________________________________________________________
TEST(StringTest, findCaseSensitive) {
  {
    String haystack;
    haystack.set("String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and \
    also keyword. nice!");
    String needle;
    needle.set("KeyWord");
    int matchPosition = haystack.findCaseSensitive(needle);
    ASSERT_EQ(matchPosition, 54);
  }
  {
    String haystack;
    haystack.set("String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and \
    also keyword. nice!");
    String needle;
    needle.set("NoKeyWord");
    int matchPosition = haystack.findCaseSensitive(needle);
    ASSERT_EQ(matchPosition, -1);
  }
}

// ____________________________________________________________________________________________________________________
TEST(StringTest, findCaseInsensitive) {
  {
    String haystack;
    haystack.set("String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and \
    also keyword. nice!");
    String needle;
    haystack.set("String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and \
    also keyword. nice!");
    needle.set("KeyWord");
    int matchPosition = haystack.findCaseInsensitive(needle);
    ASSERT_EQ(matchPosition, 34);
  }
  {
    String haystack;
    haystack.set("String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and \
    also keyword. nice!");
    String needle;
    haystack.set("String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and \
    also keyword. nice!");
    needle.set("NoKeyWord");
    int matchPosition = haystack.findCaseInsensitive(needle);
    ASSERT_EQ(matchPosition, -1);
  }
}

// ____________________________________________________________________________________________________________________
TEST(StringTest, findPerLineCaseSensitive) {
  {
    String haystack;
    String needle;
    haystack.set(
        "String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and also keyword. nice!");
    needle.set("KeyWord");
    std::vector<unsigned int> results = haystack.findPerLineCaseSensitive(needle);
    ASSERT_EQ(results.size(), 2);
    ASSERT_EQ(results[0], 54);
    ASSERT_EQ(results[1], 86);
  }
  {
    String haystack(41000);
    String needle("keyword");
    FILE* fp = fopen("test/externFinderTest.txt", "r");
    if (fp == NULL) {
      printf("Error reading file. Please run this file from within the build folder.\n");
      exit(1);
    }
    haystack.readToNewLine(fp, 40000);
    ASSERT_EQ(strlen(haystack._content), 39058);
    std::vector<unsigned int> results = haystack.findPerLineCaseSensitive(needle);
    ASSERT_EQ(results.size(), 2);
    ASSERT_EQ(results[0], 15639);
    ASSERT_EQ(results[1], 28350);
  }
}

// ____________________________________________________________________________________________________________________
TEST(StringTest, findPerLineCaseInsensitive) {
  {
    String haystack;
    String needle;
    haystack.set(
        "String with some lines containing keywords!\nline with KeyWord\nnext line with keyword, KeyWord and also keyword. nice!");
    needle.set("KeyWord");
    std::vector<unsigned int> results = haystack.findPerLineCaseInsensitive(needle);
    ASSERT_EQ(results.size(), 3);
    ASSERT_EQ(results[0], 34);
    ASSERT_EQ(results[1], 54);
    ASSERT_EQ(results[2], 77);
  }
}

// ____________________________________________________________________________________________________________________
TEST(StringTest, operators) {
  {
    String str1 = String("some string");
    String str2 = String("some string");
    ASSERT_TRUE(str1 == str2);
    ASSERT_FALSE(str1 != str2);
    str2.set("some String");
    ASSERT_FALSE(str1 == str2);
    ASSERT_TRUE(str1 != str2);
    str2.set("some longer string");
    ASSERT_FALSE(str1 == str2);
    ASSERT_TRUE(str1 != str2);
  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
