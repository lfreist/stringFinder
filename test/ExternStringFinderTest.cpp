
#include <gtest/gtest.h>
#include "../src/ExternStringFinder.h"

// ____________________________________________________________________________
TEST(ExternStringFinder, find1) {
  char filename[26] = "test/_externFinderTest";
  char pattern[8] = "keyword";
  ExternStringFinder esf;
  ef.setFile(filename);
  {
    int count = esf.find(pattern);
    ASSERT_EQ(count, 2);
    ASSERT_EQ(esf.getResult()->size(), 2);
    ASSERT_EQ(esf.getResult()->at(0), 15639);
    ASSERT_EQ(esf.getResult()->at(1), 28350);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
