// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_STRING_H_
#define SRC_STRING_H_

#include <gtest/gtest.h>

#include <vector>


#define eof '\1';

/**
 * @brief representing a string using intern c like string (char*)
 * 
 */
class String {
 public:
  // default constructor
  String();
  // constructor setting _len and creating a char* of _len
  explicit String(unsigned int len);
  // copy constructor
  String(const String& str);
  // constructor setting _content to str
  explicit String(const char* str);
  // destructor
  ~String();

  /**
   * @brief set _content to str
   * 
   * @param str
   */
  void set(const char* str);

  /**
   * @brief read _len bytes from fp into _content
   * 
   * @param fp   -> file pointer
   * @return int -> number of bytes actually read
   */
  int read(FILE* fp);

  /**
   * @brief read minimum minNumBytes from fp into _content and add maximum (_len - minNumBytes)
   * until reaching a new line character ('\n')
   * 
   * @param fp          -> file pointer
   * @param minNumBytes -> minimum number of bytes to be read (must be less than _len)
   * @return int        -> number of bytes actually read
   */
  int readToNewLine(FILE* fp, unsigned int minNumBytes);

  /**
   * @brief case sensitive search for pattern in _content starting at _content[shift]
   * 
   * @param pattern -> searching pattern
   * @param shift   -> start at _content[shift]
   * @return int    -> number of matches
   */
  int findCaseSensitive(const String& pattern, unsigned int shift = 0);
  
  /**
   * @brief 
   * case insensitive search for pattern in _content starting at _content[shift]
   * 
   * @param pattern -> searching pattern
   * @param shift   -> start at _content[shift]
   * @return int    -> number of matches
   */
  int findCaseInsensitive(const String& pattern, unsigned int shift = 0);

  /**
   * @brief case sensitive search for pattern in _content. After finding a match, jump to next line ('\n')
   * 
   * @param pattern                    -> searching pattern
   * @return std::vector<unsigned int> -> byte position of matches relative to _content
   */
  std::vector<unsigned int> findPerLineCaseSensitive(const String& pattern, unsigned int bytePositionShift = 0);

  /**
   * @brief case insensitive search for pattern in _content. After finding a match, jump to next line ('\n')
   * 
   * @param pattern                    -> searching pattern
   * @return std::vector<unsigned int> -> byte position of matches relative to _content
   */
  std::vector<unsigned int> findPerLineCaseInsensitive(const String& Pattern, unsigned int bytePositionShift = 0);

  /**
   * @brief get c like string (char*)
   * 
   * @return const char* 
   */
  const char* cstring();

  bool operator==(String compStr);
  bool operator!=(String compStr);

 private:

  /**
   * @brief get position of next new line character ('\n') in _content starting at shift (_content[shift])
   * 
   * @param shift -> start at _content[shift]
   * @return int  -> position of next new line character
   */
  int findNewLine(unsigned int shift);

  char* _content;
  unsigned int _len;

  FRIEND_TEST(StringTest, Constructor);
  FRIEND_TEST(StringTest, findPerLineCaseSensitive);
};

#endif  // SRC_STRING_H_
