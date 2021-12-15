
#include <gtest/gtest.h>
#include "../src/ExternStringFinder.hpp"

// ____________________________________________________________________________
TEST(ExternStringFinder, find1) {
  char filename[26] = "test/externFinderTest.txt";
  char pattern[12] = "ratzefummel";
  int c = find1(pattern, filename, false, false);
  ASSERT_EQ(c, 3);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
