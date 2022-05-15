// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <string>

#include "../FileChunk.h"

// ____________________________________________________________________________________________________________________
TEST(FileChunkTest, constructor) {
  {
    FileChunk chunk;
    ASSERT_EQ(chunk._isUncompressed, false);
    ASSERT_EQ(chunk._isCompressed, false);
  }
  {
    std::string tmp("some basic content...\n...in multiple lines.");
    FileChunk chunk(tmp);
    ASSERT_EQ(chunk._isUncompressed, true);
    ASSERT_EQ(chunk._isCompressed, false);
    ASSERT_EQ(chunk.length(), 43);
    ASSERT_EQ(chunk._originalSize, 43);
    ASSERT_EQ(chunk._uncompressedContent, tmp);
  }
  {
    // TODO: test constructor for compressed input
  }
}


// ____________________________________________________________________________________________________________________
TEST(FileChunkTest, setContentFromFile) {
  // read exactly 50 bytes per chunk
  {
    std::ifstream _bufferTest("files/_chunk.test.txt");
    FileChunk chunk;
    // read first 50 bytes
    size_t bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(chunk.length(), 50);
    ASSERT_EQ(chunk.getUncompressedContent(), "declamations artal jejunoileitis monochlorinated s");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // second 50 bytes
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(chunk.length(), 50);
    ASSERT_EQ(chunk.getUncompressedContent(), "uperthick\ndamns kodakked mermithized Indy scrod Ha");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // third 50 bytes
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(chunk.length(), 50);
    ASSERT_EQ(chunk.getUncompressedContent(), "imes\npirr SM zarnich Elsey yard-thick teniacide Sa");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // forth 50 bytes
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(chunk.length(), 50);
    ASSERT_EQ(chunk.getUncompressedContent(), "mnorwood rejectees overstay isoclinically\nfrogtong");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // fifth 50 bytes
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(chunk.length(), 50);
    ASSERT_EQ(chunk.getUncompressedContent(), "ue pot-au-feu settergrass cuneated\nDunseath Innes ");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // sixth 50 bytes
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(chunk.length(), 50);
    ASSERT_EQ(chunk.getUncompressedContent(), "untailorlike adnouns compressible dead-light Stall");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // last 13 bytes
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 13);
    ASSERT_EQ(chunk.length(), 13);
    ASSERT_EQ(chunk.getUncompressedContent(), "worth Kiangpu");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);
    // read empty
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 0);
    ASSERT_EQ(chunk.length(), 0);
    ASSERT_EQ(chunk.getUncompressedContent(), "");
    ASSERT_FALSE(chunk._isCompressed);
    ASSERT_TRUE(chunk._isUncompressed);

    _bufferTest.close();
  }
  // read (50 + x) bytes to new line char per chunk
  {
    std::ifstream _bufferTest("files/_chunk.test.txt");
    FileChunk chunk;
    // read first chunk (59 bytes)
    size_t bytes_read = chunk.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 59);
    ASSERT_EQ(chunk.length(), 59);
    ASSERT_EQ(chunk.getUncompressedContent(), "declamations artal jejunoileitis monochlorinated superthick");
    // second chunk (131 bytes)
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 131);
    ASSERT_EQ(chunk.length(), 131);
    ASSERT_EQ(chunk.getUncompressedContent(), "damns kodakked mermithized Indy scrod Haimes\npirr SM zarnich Elsey yard-thick teniacide Samnorwood rejectees overstay isoclinically");
    // last chunk (121 bytes)
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 121);
    ASSERT_EQ(chunk.length(), 121);
    ASSERT_EQ(chunk.getUncompressedContent(), "frogtongue pot-au-feu settergrass cuneated\nDunseath Innes untailorlike adnouns compressible dead-light Stallworth Kiangpu");
    // read empty
    bytes_read = chunk.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 0);
    ASSERT_EQ(chunk.length(), 0);
    ASSERT_EQ(chunk.getUncompressedContent(), "");

    _bufferTest.close();
  }
  {
    // TODO: test setContentFromFile for zstd compressed file
  }
}

// ____________________________________________________________________________________________________________________
TEST(FileChunkTest, de_compression) {
  std::ifstream _chunkTest("files/_chunk.test.txt");
  FileChunk chunk0;
  size_t bytes_read = chunk0.setContentFromFile(_chunkTest);
  _chunkTest.close();
  ASSERT_EQ(bytes_read, 313);
  std::ifstream  _compressedFile("files/_chunk.test.zst", std::ios::binary);
  FileChunk chunk1;
  bytes_read = chunk1.setContentFromZstdFile(_compressedFile, 313);
  _compressedFile.close();
  ASSERT_EQ(bytes_read, 229);
  ASSERT_EQ(chunk1._originalSize, 313);
  {  // decompression
    ASSERT_NE(chunk0._uncompressedContent, chunk1._uncompressedContent);
    ASSERT_TRUE(chunk1._isCompressed);
    ASSERT_FALSE(chunk1._isUncompressed);
    chunk1.decompress();
    ASSERT_TRUE(chunk1._isCompressed);
    ASSERT_TRUE(chunk1._isUncompressed);
    ASSERT_EQ(chunk1.length(), 313);
    ASSERT_EQ(chunk0._uncompressedContent, chunk1._uncompressedContent);
  }
  {  // compression
    ASSERT_NE(chunk0._compressedContent, chunk1._compressedContent);
    ASSERT_FALSE(chunk0._isCompressed);
    ASSERT_TRUE(chunk0._isUncompressed);
    chunk0.compress();
    ASSERT_TRUE(chunk1._isCompressed);
    ASSERT_TRUE(chunk1._isUncompressed);
    ASSERT_EQ(chunk0.length(), 313);
    ASSERT_EQ(chunk0._compressedContent, chunk1._compressedContent);
  }
}

/*
// ____________________________________________________________________________________________________________________
TEST(FileChunkTest, operators) {
  {
    FileChunk buf1("some string");
    FileChunk buf2("some string");
    ASSERT_TRUE(buf1 == buf2);
    ASSERT_FALSE(buf1 != buf2);
    buf2.setContent("some String");
    ASSERT_FALSE(buf1 == buf2);
    ASSERT_TRUE(buf1 != buf2);
    buf1.setContent("some longer string");
    ASSERT_FALSE(buf1 == buf2);
    ASSERT_TRUE(buf1 != buf2);
  }
}

// ____________________________________________________________________________________________________________________
TEST(FileChunkTest, compressDecompress) {
  FILE* uncompressedFile = fopen("test/_bufferCompressTest", "r");
  if (uncompressedFile == nullptr) {
    exit(1);
  }
  FileChunk buf(10037000);
  chunk.setContentFromFile(uncompressedFile, 10037000);
  FileChunk originalBuffer(buf);
  {  // check, if file is read correctly
    ASSERT_EQ(chunk.cstring()[0], 's');
    ASSERT_EQ(chunk.cstring()[1], 'o');
    ASSERT_EQ(chunk.cstring()[2], 'm');
    ASSERT_EQ(chunk.cstring()[chunk.length() - 1], 'd');
    ASSERT_EQ(chunk.cstring()[chunk.length() - 2], 'e');
    ASSERT_EQ(chunk.cstring()[chunk.length() - 3], 'n');
    ASSERT_EQ(chunk.length(), 10036057);
    ASSERT_EQ(std::string(chunk._content), std::string(originalBuffer._content));
  }
  {  // compress buffer
    auto compressedSize = chunk.compress(3);
    ASSERT_EQ(chunk._originalSize, originalBuffer.length());
    ASSERT_EQ(std::string(chunk._content), std::string(originalBuffer._content));
    ASSERT_EQ(chunk._compressedContent.size(), compressedSize);
    // overwrite _content to make sure decompress() does not just return the already existing _content as result
    chunk.setContent("not the original content");
    ASSERT_EQ(chunk._len, 24);
    auto decompressedSize = chunk.decompress();
    ASSERT_EQ(chunk._len, decompressedSize);
    ASSERT_EQ(strlen(chunk._content), strlen(originalBuffer._content));
    ASSERT_EQ(chunk._content[0], 's');
    ASSERT_EQ(chunk._content[1], 'o');
    ASSERT_EQ(chunk._content[2], 'm');
    ASSERT_EQ(chunk._content[chunk.length() - 1], 'd');
    ASSERT_EQ(chunk._content[chunk.length() - 2], 'e');
    ASSERT_EQ(chunk._content[chunk.length() - 3], 'n');
  }
}
*/

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
