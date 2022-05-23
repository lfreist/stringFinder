
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../src/ExternStringFinder.h"

// ____________________________________________________________________________
TEST(ExternStringFinder, find1) {
  std::string filename("test/files/_externStringFinder.test.txt");
  std::string pattern("keyword");
  {
    ExternStringFinder esf(filename, pattern);
    auto matches = esf.find();
    ASSERT_EQ(matches.size(), 2);
    ASSERT_EQ(matches[0], 15639);
    ASSERT_EQ(matches[1], 28350);
  }
  {
    ExternStringFinder esf(filename, pattern, "", false, false, 10, 10);
    auto matches = esf.find();
    ASSERT_EQ(matches.size(), 2);
    ASSERT_EQ(matches[0], 15639);
    ASSERT_EQ(matches[1], 28350);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
