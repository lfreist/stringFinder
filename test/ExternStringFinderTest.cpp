
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../src/ExternStringFinder.h"

// ____________________________________________________________________________
TEST(ExternStringFinder, find1) {
  std::string filename("test/_externFinderTest");
  std::string pattern("keyword");
  ExternStringFinder esf(filename, pattern);
  {
    auto count = esf.find();
    ASSERT_EQ(count.size(), 2);
    ASSERT_EQ(count[0], 15639);
    ASSERT_EQ(count[1], 28350);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
