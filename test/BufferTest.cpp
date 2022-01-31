// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

// TODO(lfreist): write tests for read() and readToNewLine()

#include <gtest/gtest.h>

#include <cstdio>

#include "../src/utils/Buffer.h"

// ____________________________________________________________________________________________________________________
TEST(BufferTest, Constructor) {
  {
    Buffer buf;
    ASSERT_EQ(buf._len, 0);
    ASSERT_EQ(buf._bufferSize, 1);
    ASSERT_EQ(buf._content[0], '\0');
  }
  {
    Buffer buf(5);
    ASSERT_EQ(buf._len, 4);
    ASSERT_EQ(buf._bufferSize, 5);
    ASSERT_EQ(buf._content[4], '\0');
  }
  {
    Buffer buf1("test");
    Buffer buf2(buf1);
    ASSERT_EQ(buf2._len, 4);
    ASSERT_EQ(buf2._bufferSize, 5);
    ASSERT_EQ(buf2._content[0], 't');
    ASSERT_EQ(buf2._content[1], 'e');
    ASSERT_EQ(buf2._content[2], 's');
    ASSERT_EQ(buf2._content[3], 't');
    ASSERT_EQ(buf2._content[4], '\0');
  }
  {
    Buffer buf("test");
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
    Buffer buf(51);
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
    Buffer buf(140);
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
    std::cout << buf._content << std::endl;
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
    Buffer buf(20);
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
TEST(StringTest, operators) {
  {
    Buffer buf1("some string");
    Buffer buf2("some string");
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


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
