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
  String(unsigned int len);
  // copy constructor
  String(const String& str);
  // constructor setting _content to str
  String(const char* str);
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
  int readToNewLine(FILE* fp, int minNumBytes);

  /**
   * @brief case sensitive search for pattern in _content starting at _content[shift]
   * 
   * @param pattern -> searching pattern
   * @param shift   -> start at _content[shift]
   * @return int    -> number of matches
   */
  int findCaseSensitive(String pattern, unsigned int shift = 0);
  
  /**
   * @brief 
   * case insensitive search for pattern in _content starting at _content[shift]
   * 
   * @param pattern -> searching pattern
   * @param shift   -> start at _content[shift]
   * @return int    -> number of matches
   */
  int findCaseInsensitive(String pattern, unsigned int shift = 0);

  /**
   * @brief case sensitive search for pattern in _content. After finding a match, jump to next line ('\n')
   * 
   * @param pattern                    -> searching pattern
   * @return std::vector<unsigned int> -> byte position of matches relative to _content
   */
  std::vector<unsigned int> findPerLineCaseSensitive(String pattern);

  /**
   * @brief case insensitive search for pattern in _content. After finding a match, jump to next line ('\n')
   * 
   * @param pattern                    -> searching pattern
   * @return std::vector<unsigned int> -> byte position of matches relative to _content
   */
  std::vector<unsigned int> findPerLineCaseInsensitive(String Pattern);

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
  unsigned _len;

  FRIEND_TEST(StringTest, Constructor);
};

#endif  // SRC_STRING_H_
