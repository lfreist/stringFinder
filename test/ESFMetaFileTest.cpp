// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <string>

#include <gtest/gtest.h>

#include "../src/utils/ESFMetaFile.h"

TEST(ESFMetaFileTest, constructor) {
  {  // open metafile for read
    ESFMetaFile metaFile(std::string("test/files/_esfMetaFileTest"), std::ios::in);
  }
}

TEST(ESFMetaFileTest, writeAndRead) {
  unsigned mos = 234872354;
  chunkSize firstChunk{832562, 125341};
  chunkSize secondChunk{34572234, 1234124};
  chunkSize thirdChunk{7469543, 643534};
  chunkSize eofChunk{0, 0};
  {  // write to file
    ESFMetaFile metaFile(std::string("test/files/_tmpEsfMetaFile"), std::ios::out);
    metaFile.writeMaxOriginalSize(mos);
    metaFile.writeChunkSize(firstChunk);
    metaFile.writeChunkSize(secondChunk);
    metaFile.writeChunkSize(thirdChunk);
  }
  {  // read from file
    ESFMetaFile metaFile(std::string("test/files/_tmpEsfMetaFile"), std::ios::in);
    ASSERT_EQ(metaFile.getMaxOriginalSize(), mos);
    ASSERT_EQ(metaFile.nextChunkSize(), firstChunk);
    ASSERT_EQ(metaFile.nextChunkSize(), secondChunk);
    ASSERT_EQ(metaFile.nextChunkSize(), thirdChunk);
    ASSERT_EQ(metaFile.nextChunkSize(), eofChunk);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}