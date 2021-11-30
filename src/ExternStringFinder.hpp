//
// Created by lfreist on 23.11.21.
//

#ifndef LIB_FILEREADER_HPP_
#define LIB_FILEREADER_HPP_

#include <stdio.h>
#include <vector>

using std::vector;

#define NR_OF_CHARS 256
#define MAX_BUFFER_SIZE 2 << 16

/* Read file content from current file pointer position + shift
   into buffer
   assuming fp is not NULL */
bool nextBuffer(FILE* fp, char* buffer, int shift);

/* get maximum of two integers */
int max(int a, int b);

// ____________________________________________________________________________
// Boyer-Moore-Search


vector<int> search(FILE* fp, char* pattern);

#endif  // LIB_FILEREADER_HPP_
