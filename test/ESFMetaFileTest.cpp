// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>

#include <gtest/gtest.h>

#include "../src/utils/ESFMetaFile.h"

TEST(ESFMetaFileTest, constructor) {
  ESFMetaFile m(std::string("test/_esfMetaFileTest"));
  ASSERT_EQ(m._maxChunkSize, 3478563);
}

TEST(ESFMetaFileTest, nextChunkSize) {
  ESFMetaFile m(std::string("test/_esfMetaFileTest"));
  std::pair<unsigned, unsigned> pair;
  pair = m.nextChunkPair();
  ASSERT_EQ(pair.first, 435235);
  ASSERT_EQ(pair.second, 23421);
  pair = m.nextChunkPair();
  ASSERT_EQ(pair.first, 435245);
  ASSERT_EQ(pair.second, 12345);
  pair = m.nextChunkPair();
  ASSERT_EQ(pair.first, 3452342);
  ASSERT_EQ(pair.second, 12316);
  pair = m.nextChunkPair();
  ASSERT_EQ(pair.first, 534322);
  ASSERT_EQ(pair.second, 6546);
  pair = m.nextChunkPair();
  ASSERT_EQ(pair.first, 1354456);
  ASSERT_EQ(pair.second, 45745);
  pair = m.nextChunkPair();
  ASSERT_EQ(pair.first, 0);
  ASSERT_EQ(pair.second, 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}