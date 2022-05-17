// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_FILECHUNK_H_
#define SRC_UTILS_FILECHUNK_H_

#include <string>
#include <vector>
#include <fstream>
#include <exception>

#include "gtest/gtest.h"

using std::string;
using std::vector;

class FileChunk {
 public:
  /// default constructor
  FileChunk();
  /**
   * Constructor for uncompressed content
   * @param uncompressedContent
   */
  explicit FileChunk(string &uncompressedContent);
  /**
   * Constructor for compressed content
   * @param compressedContent
   */
  FileChunk(vector<char> compressedContent, size_t originalSize);
  /// destructor
  ~FileChunk();

  /**
   * Set uncompressed content
   * @param uncompressedContent
   */
  void setContent(string uncompressedContent);
  /**
   * Set compressed content
   * @param compressedContent
   * @param originalSize size of uncompressed data
   */
  void setContent(vector<char> compressedContent, size_t originalSize);
  /**
   * Read a minimum of minNumBytes from file. If toNewLine is set to true, minNumBytes are read and further bytes until
   * the next new line character are appended.
   * @param file input file
   * @param minNumBytes minimum numbers of bytes that will be read. Read to end of file if 0.
   * @param toNewLine indicates whether to read exactly minNumBytes or fill until the next new line character (ignored when zstdCompressed is true)
   * @return number of bytes actually read.
   */
  size_t setContentFromFile(
      std::istream &file,
      std::streamsize minNumBytes = 0,
      bool toNewLine = false
  );
  /**
   * Read numBytes from zstd compressed file
   * @param file input file (zstd compressed)
   * @param numBytes number of bytes that will be read. Read to end of file if 0.
   * @param originalSize size of uncompressed data
   * @return number of bytes actually read.
   */
  size_t setContentFromZstdFile(
      std::istream &file,
      size_t originalSize,
      std::streamsize numBytes = 0
      );
  /**
   * Search all occurrences of pattern in _uncompressedContent
   * @param pattern pattern to be searched for
   * @return positions of all matches
   */
  vector<string::size_type> searchAll(string &pattern);
  /**
   * Search all occurrences of pattern in _uncompressedContent per line (jump to next line if a match was found)
   * @param pattern pattern to be searched for
   * @return positions of all matches
   */
  vector<string::size_type> searchAllPerLine(string &pattern);
  /**
   * Get length of _uncompressedContent
   * @return length of _uncompressedContent
   */
  [[nodiscard]] string::size_type length() const;
  /**
   * Get length of _compressedContent
   * @return length of _compressedContent
   */
  [[nodiscard]] size_t compressedLength() const;
  /**
   * Compress _uncompressedContent into _compressedContent using zstd
   * @param compressionLevel
   * @return size of compressed data
   */
  size_t compress(int compressionLevel = 3);
  /**
   * Decompress _compressedContent into _uncompressedContent using zstd
   * @param originalSize size of uncompressed result. Use _originalSize if 0.
   * @return size of uncompressed data
   */
  size_t decompress(size_t originalSize = 0);

  vector<char> getCompressedContent();
  string getUncompressedContent();

  bool operator==(const FileChunk &compChunk);
  bool operator!=(const FileChunk &compChunk);
 private:
  /**
   * Search pattern in _uncompressedContent starting at offset
   * @param pattern pattern to be searched for
   * @param offset start search at this offset
   * @return position of match
   */
  string::size_type search(string &pattern, size_t offset);

  /// indicates whether the content is available uncompressed
  bool _isUncompressed;
  /// indicates whether the content is available compressed
  bool _isCompressed;
  /// uncompressed content
  string _uncompressedContent;
  /// compressed content
  vector<char> _compressedContent;
  /// original size (length of _uncompressedContent)
  string::size_type _originalSize;
  /// offset of content relative to the beginning of a file
  size_t _offset;

  FRIEND_TEST(FileChunkTest, constructor);
  FRIEND_TEST(FileChunkTest, setContentFromFile);
  FRIEND_TEST(FileChunkTest, de_compression);
};


struct NotUncompressedException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "There is no uncompressed content to perform this method on.";
  }
};

struct NotCompressedException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "There is no compressed content to perform this method on.";
  }
};

#endif  // SRC_UTILS_FILECHUNK_H_
