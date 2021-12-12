//
// Created by lfreist on 23.11.21.
//

#ifndef LIB_FILEREADER_HPP_
#define LIB_FILEREADER_HPP_

#include <stdio.h>
#include <vector>

using std::vector;

#define BUFFER_OVERFLOW (2 << 11)  // max 4kb longer
#define MAX_BUFFER_SIZE (2 << 12)-BUFFER_OVERFLOW

void readBuffer1(char *path);

void readBuffer2(char *path);

int nextBuffer1(FILE* fp, char* buffer);

int nextBuffer2(int fd, char* buffer);

int find1(char* pattern, char* filename);

int find2(char* pattern, char* filename);

int findPattern(char* pattern, char* content);

#endif  // LIB_FILEREADER_HPP_
