// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

/**
 * @brief get (absolute) position of next new line character ('\n') in content starting at content[shift]
 *
 * @param content [char*]: char array in which nl is searched
 * @param shift [unsigned]: start searching at content[shift]
 * @return [int]: -1 if nl char was not found, else position of next nl char in content
 */
int findNextNewLine(char* content, unsigned shift);

/**
 * @brief get (absolute) position of next pattern in content starting at content[shift]
 *
 * @param pattern [char*]: pattern to be searched for
 * @param content [char*]: char array in which pattern is searched
 * @param shift [unsigned]: start searching at content[shift]
 * @return [int]: -1 if pattern was not found, else position of next pattern match in content
 */
int findNext(const char* pattern, char* content, unsigned shift);

int findAllPerLine(const char* pattern, char* content);

int findAll(const char* pattern, char* content);
