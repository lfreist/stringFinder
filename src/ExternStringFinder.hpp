//
// Created by lfreist on 23.11.21.
//

#ifndef LIB_FILEREADER_HPP_
#define LIB_FILEREADER_HPP_

#include <stdio.h>
#include <vector>

using std::vector;

#define MAX_BUFFER_SIZE (2 << 21)
#define BUFFER_OVERFLOW (2 << 11)

void readBuffer1(char *path);

void readBuffer2(char *path);

int nextBuffer1(FILE* fp, char* buffer);

int nextBuffer2(int fd, char* buffer);

#endif  // LIB_FILEREADER_HPP_
