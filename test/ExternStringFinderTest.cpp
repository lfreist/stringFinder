
#include <gtest/gtest.h>
#include "../src/ExternStringFinder.hpp"

// ____________________________________________________________________________
TEST(ExternStringFinder, find1) {
  char filename[26] = "test/externFinderTest.txt";
  char pattern[8] = "keyword";
  ExternFinder ef;
  ef.setFile(filename);
  {
    int count = ef.find(pattern);
    ASSERT_EQ(count, 2);
    ASSERT_EQ(ef.getResult()->size(), 2);
    ASSERT_EQ(ef.getResult()->at(0), 15639);
    ASSERT_EQ(ef.getResult()->at(1), 28350);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
