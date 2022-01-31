// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_BUFFER_H_
#define SRC_UTILS_BUFFER_H_

#include <gtest/gtest.h>

#include <vector>


#define eof '\1';

/**
 * @brief representing a buffer using intern c like string (char*)
 * 
 */
class Buffer {
 public:
  // default constructor
  Buffer();
  // constructor setting _len and creating a char* of _len
  explicit Buffer(unsigned int bufferSize);
  // copy constructor
  Buffer(const Buffer& str);
  // constructor setting _content to str
  explicit Buffer(const char* str);
  // destructor
  ~Buffer();

  /**
   * @brief set _content to str
   * 
   * @param str
   */
  void setContent(const char* str);

  /**
   * @brief read _len bytes from fp into _content
   * 
   * @param fp   -> file pointer
   * @return int -> number of bytes actually read
   */
  int setContentFromFile(FILE* fp, unsigned int minNumBytes = 0, bool toNewLine = true);

  /**
   * @brief search for pattern in _content starting at _content[shift]
   * 
   * @param pattern       -> searching pattern
   * @param shift         -> start at _content[shift]
   * @param caseSensitive -> true for case sensitive search, else false
   * @return int          -> number of matches
   */
  int find(const char* pattern, unsigned int shift = 0, bool caseSensitive = true);

  /**
   * @brief search for pattern in _content. After finding a match, jump to next line ('\n')
   * 
   * @param pattern                    -> searching pattern
   * @param bytePositionShift          -> is added to match position for multi-buffer-search
   * @param caseSensitive              -> true for case sensitive search, else false
   * @return std::vector<unsigned int> -> byte position of matches relative to _content
   */
  std::vector<unsigned int> findPerLine(const char* pattern, unsigned int bytePositionShift = 0,
                                        bool caseSensitive = true);

  /**
   * @brief get c like string (char*)
   * 
   * @return const char* 
   */
  const char* cstring();

  unsigned int length() const;

  size_t compress(int compressionLevel);
  size_t decompress(size_t originalSize);

  static size_t compressToBuffer(int compressionLevel, Buffer& buffer);
  static size_t decompressBuffer(size_t originalSize, Buffer& buf);

  bool operator==(const Buffer& compStr);
  bool operator!=(const Buffer& compStr);

 private:

  /**
   * @brief get position of next new line character ('\n') in _content starting at shift (_content[shift])
   * 
   * @param shift -> start at _content[shift]
   * @return int  -> position of next new line character
   */
  int findNewLine(unsigned int shift);

  char* _content;
  unsigned int _bufferSize;
  unsigned int _len;

  FRIEND_TEST(BufferTest, Constructor);
  FRIEND_TEST(BufferTest, setContentFromFile);
  FRIEND_TEST(BufferTest, findPerLine);
};

#endif  // SRC_UTILS_Buffer_H_
