// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>

#include <cstdio>

#include "../src/utils/FileChunk.h"

// ____________________________________________________________________________________________________________________
TEST(BufferTest, Constructor) {
  {
    FileChunk buf;
    ASSERT_EQ(buf._len, 0);
    ASSERT_EQ(buf._bufferSize, 1);
    ASSERT_EQ(buf._content[0], '\0');
  }
  {
    FileChunk buf(5);
    ASSERT_EQ(buf._len, 4);
    ASSERT_EQ(buf._bufferSize, 5);
    ASSERT_EQ(buf._content[4], '\0');
  }
  {
    FileChunk buf1("test");
    FileChunk buf2(buf1);
    ASSERT_EQ(buf2._len, 4);
    ASSERT_EQ(buf2._bufferSize, 5);
    ASSERT_EQ(buf2._content[0], 't');
    ASSERT_EQ(buf2._content[1], 'e');
    ASSERT_EQ(buf2._content[2], 's');
    ASSERT_EQ(buf2._content[3], 't');
    ASSERT_EQ(buf2._content[4], '\0');
  }
  {
    FileChunk buf("test");
    ASSERT_EQ(buf._len, 4);
    ASSERT_EQ(buf._bufferSize, 5);
    ASSERT_EQ(buf._content[0], 't');
    ASSERT_EQ(buf._content[1], 'e');
    ASSERT_EQ(buf._content[2], 's');
    ASSERT_EQ(buf._content[3], 't');
    ASSERT_EQ(buf._content[4], '\0');
  }
}

// ____________________________________________________________________________________________________________________
TEST(BufferTest, setContentFromFile) {
  // read exactly 50 bytes per chunk
  {
    FILE* _bufferTest = fopen("test/_bufferTest", "r");
    FileChunk buf(51);
    // read first 50 bytes
    int bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(buf._len, 50);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'd');
    ASSERT_EQ(buf._content[49], 's');
    ASSERT_EQ(buf._content[50], '\0');
    // second 50 bytes
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(buf._len, 50);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'u');
    ASSERT_EQ(buf._content[49], 'a');
    ASSERT_EQ(buf._content[50], '\0');
    // third 50 bytes
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(buf._len, 50);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'i');
    ASSERT_EQ(buf._content[49], 'a');
    ASSERT_EQ(buf._content[50], '\0');
    // forth 50 bytes
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(buf._len, 50);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'm');
    ASSERT_EQ(buf._content[49], 'g');
    ASSERT_EQ(buf._content[50], '\0');
    // fifth 50 bytes
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(buf._len, 50);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'u');
    ASSERT_EQ(buf._content[49], ' ');
    ASSERT_EQ(buf._content[50], '\0');
    // sixth 50 bytes
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 50);
    ASSERT_EQ(buf._len, 50);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'u');
    ASSERT_EQ(buf._content[49], 'l');
    ASSERT_EQ(buf._content[50], '\0');
    // last 13 bytes
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 13);
    ASSERT_EQ(buf._len, 13);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], 'w');
    ASSERT_EQ(buf._content[12], 'u');
    ASSERT_EQ(buf._content[13], '\0');
    // read empty
    bytes_read = buf.setContentFromFile(_bufferTest, 50, false);
    ASSERT_EQ(bytes_read, 0);
    ASSERT_EQ(buf._len, 0);
    ASSERT_EQ(buf._bufferSize, 51);
    ASSERT_EQ(buf._content[0], '\0');
    // close filestream
    fclose(_bufferTest);
  }
  // read (50 + x) bytes to new line char per chunk
  {
    FILE* _bufferTest = fopen("test/_bufferTest", "r");
    FileChunk buf(140);
    // read first chunk (59 bytes)
    int bytes_read = buf.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 59);
    ASSERT_EQ(buf._len, 60);
    ASSERT_EQ(buf._bufferSize, 140);
    ASSERT_EQ(buf._content[0], 'd');
    ASSERT_EQ(buf._content[58], 'k');
    ASSERT_EQ(buf._content[59], '\n');
    ASSERT_EQ(buf._content[60], '\0');
    // second chunk (131 bytes)
    bytes_read = buf.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 131);
    ASSERT_EQ(buf._len, 132);
    ASSERT_EQ(buf._bufferSize, 140);
    ASSERT_EQ(buf._content[0], 'd');
    ASSERT_EQ(buf._content[129], 'l');
    ASSERT_EQ(buf._content[130], 'y');
    ASSERT_EQ(buf._content[131], '\n');
    ASSERT_EQ(buf._content[132], '\0');
    // last chunk (121 bytes)
    bytes_read = buf.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 121);
    ASSERT_EQ(buf._len, 122);
    ASSERT_EQ(buf._bufferSize, 140);
    ASSERT_EQ(buf._content[0], 'f');
    ASSERT_EQ(buf._content[119], 'p');
    ASSERT_EQ(buf._content[120], 'u');
    ASSERT_EQ(buf._content[121], '\n');
    ASSERT_EQ(buf._content[122], '\0');
    // read empty
    bytes_read = buf.setContentFromFile(_bufferTest, 50, true);
    ASSERT_EQ(bytes_read, 0);
    ASSERT_EQ(buf._len, 0);
    ASSERT_EQ(buf._bufferSize, 140);
    ASSERT_EQ(buf._content[0], '\0');
    // close filestream
    fclose(_bufferTest);
  }
  // setContentFromFile will fail and return -1 here
  {
    FILE* _bufferTest = fopen("test/_bufferTest", "r");
    FileChunk buf(20);
    // read first chunk (60 bytes)
    int bytes_read = buf.setContentFromFile(_bufferTest, 10, true);
    ASSERT_EQ(bytes_read, -1);
    ASSERT_EQ(buf._len, 20);
    ASSERT_EQ(buf._bufferSize, 20);
    ASSERT_EQ(buf._content[0], 'd');
    ASSERT_EQ(buf._content[18], ' ');
    ASSERT_EQ(buf._content[19], '\0');
    // close filestream
    fclose(_bufferTest);
  }
}

// ____________________________________________________________________________________________________________________
TEST(BufferTest, operators) {
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
TEST(BufferTest, compressDecompress) {
  FILE* uncompressedFile = fopen("test/_bufferCompressTest", "r");
  if (uncompressedFile == nullptr) {
    std::cout << "Error reading file." << std::endl;
    exit(6);
  }
  FileChunk buf(10037000);
  buf.setContentFromFile(uncompressedFile, 10037000);
  FileChunk originalBuffer(buf);
  {  // check, if file is read correctly
    ASSERT_EQ(buf.cstring()[0], 's');
    ASSERT_EQ(buf.cstring()[1], 'o');
    ASSERT_EQ(buf.cstring()[2], 'm');
    ASSERT_EQ(buf.cstring()[buf.length() - 1], 'd');
    ASSERT_EQ(buf.cstring()[buf.length() - 2], 'e');
    ASSERT_EQ(buf.cstring()[buf.length() - 3], 'n');
    ASSERT_EQ(buf.length(), 10036057);
    ASSERT_EQ(std::string(buf._content), std::string(originalBuffer._content));
  }
  {  // compress buffer
    auto compressedSize = buf.compress(3);
    ASSERT_EQ(buf._originalSize, originalBuffer.length());
    ASSERT_EQ(std::string(buf._content), std::string(originalBuffer._content));
    ASSERT_EQ(buf._compressedContent.size(), compressedSize);
    // overwrite _content to make sure decompress() does not just return the already existing _content as result
    buf.setContent("not the original content");
    ASSERT_EQ(buf._len, 24);
    auto decompressedSize = buf.decompress();
    ASSERT_EQ(buf._len, decompressedSize);
    ASSERT_EQ(strlen(buf._content), strlen(originalBuffer._content));
    ASSERT_EQ(buf._content[0], 's');
    ASSERT_EQ(buf._content[1], 'o');
    ASSERT_EQ(buf._content[2], 'm');
    ASSERT_EQ(buf._content[buf.length() - 1], 'd');
    ASSERT_EQ(buf._content[buf.length() - 2], 'e');
    ASSERT_EQ(buf._content[buf.length() - 3], 'n');
  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
