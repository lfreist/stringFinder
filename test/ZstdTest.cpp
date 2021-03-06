
#include <gtest/gtest.h>

#include "../src/utils/ZstdWrapper.h"

// _____________________________________________________________________________
TEST(CompressionTest, Basic) {
  std::vector<int> x{1, 2, 3, 4};
  std::vector<char> comp = ZstdWrapper::compress(x.data(), x.size() * sizeof(int));
  auto decomp = ZstdWrapper::decompress<int>(comp.data(), comp.size(), 4);
  ASSERT_EQ(x, decomp);
}
