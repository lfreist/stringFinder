// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_BUFFER_H_
#define SRC_UTILS_BUFFER_H_

#include <gtest/gtest.h>

#include <vector>

/**
 * @brief representing a buffer using intern c like string (char*)
 * 
 */
class FileChunk {
 public:
  // default constructor
  FileChunk();
  // constructor setting _len and creating a char* of _len
  explicit FileChunk(unsigned int bufferSize);
  // copy constructor
  FileChunk(const FileChunk &str);
  // constructor setting _content to str
  explicit FileChunk(const char *str);
  // destructor
  ~FileChunk();

  /**
   * @brief set _content to str
   * 
   * @param str
   */
  void setContent(const char *str);

  /**
   * @brief read minNumBytes bytes from fp into _content
   * 
   * @param fp   -> file pointer
   * @return int -> number of bytes actually read
   */
  int setContentFromFile(
      FILE *fp,
      unsigned int minNumBytes = 0,
      bool toNewLine = true,
      bool zstdCompressed = false,
      size_t originalSize = 0,
      unsigned startPosition = 0
  );

  /**
   * @brief search for pattern in _content starting at _content[shift]
   * 
   * @param pattern       -> searching pattern
   * @param shift         -> start at _content[shift]
   * @param caseSensitive -> true for case sensitive search, else false
   * @return int          -> number of matches
   */
  int find(const char *pattern, unsigned shift, bool caseSensitive = true);

  /**
   * @brief search for pattern in _content. After finding a match, jump to next line ('\n')
   * 
   * @param pattern                    -> searching pattern
   * @param bytePositionShift          -> is added to match position for multi-buffer-search
   * @param caseSensitive              -> true for case sensitive search, else false
   * @return std::vector<unsigned int> -> byte position of matches relative to _content
   */
  std::vector<unsigned> findPerLine(const char *pattern, bool caseSensitive = true);

  /**
   * @brief get c like string (char*)
   * 
   * @return const char* 
   */
  const char *cstring();

  unsigned length() const;

  size_t compress(int compressionLevel);
  size_t decompress();

  void setOriginalSize(unsigned origSize);
  unsigned int getOriginalSize() const;

  bool operator==(const FileChunk &compStr);
  bool operator!=(const FileChunk &compStr);

 private:

  /**
   * @brief get position of next new line character ('\n') in _content starting at shift (_content[shift])
   * 
   * @param shift -> start at _content[shift]
   * @return int  -> position of next new line character
   */
  int findNewLine(unsigned int shift);

  char *_content;
  unsigned _bufferSize;
  unsigned _len;
  unsigned _globalShift;
  size_t _originalSize;  // in case buffer is compressed
  std::vector<char> _compressedContent;
  unsigned _compressedSize;

  FRIEND_TEST(BufferTest, Constructor);
  FRIEND_TEST(BufferTest, setContentFromFile);
  FRIEND_TEST(BufferTest, findPerLine);
  FRIEND_TEST(BufferTest, compressDecompress);
};

#endif  // SRC_UTILS_Buffer_H_
