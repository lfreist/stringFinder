//
// Created by lfreist on 23.11.21.
//

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <vector>

using std::vector;

#include "ExternStringFinder.hpp"

// ____________________________________________________________________________
bool nextBuffer(FILE* fp, char* buffer, int shift) {
    if (shift != 0) {
        fseek(fp, shift, SEEK_CUR);
    }
    char c;
    for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
        c = fgetc(fp);
        if (c == EOF) {
            buffer[i] = '\0';
            return false;
        }
        buffer[i] = c;
    }
    buffer[MAX_BUFFER_SIZE+1] = '\0';
    return true;
}

// ____________________________________________________________________________
int max(int a, int b) {
    return (a > b) ? a : b;
}

// ____________________________________________________________________________
void buildBadCharTable(char* pattern, int size, int badCharTable[NR_OF_CHARS]) {
    int i;
    for (i = 0; i < 256; i++) {
        badCharTable[i] = -1;
    }
    for (i = 0; i < size; i++) {
        badCharTable[static_cast<int>(pattern[i])] = i;
    }
}

// ____________________________________________________________________________
vector<int> search(FILE* fp, char* pattern) {
    vector<int> results;
    char textBuffer[MAX_BUFFER_SIZE+1];

    int patternLength = strlen(pattern);
    int textLength;

    // 256
    int badCharTable[NR_OF_CHARS];

    buildBadCharTable(pattern, patternLength, badCharTable);

    int shift = 0;
    int oldShift = 0;
    bool lastBuffer = !nextBuffer(fp, textBuffer, 0);

    int bufferLength = strlen(textBuffer);

    // printf("Buffer: %s, %d\n", textBuffer, bufferLength);

    int bytePosition = 0;
    int bufferShift = 0;

    bool newBuffer = false;

    while (true) {
        if (shift > (bufferLength - patternLength) || newBuffer) {
            if (bufferShift == 0) {
                bufferShift = shift-bufferLength+1;
            }
            bytePosition += shift-bufferShift;
            lastBuffer = !nextBuffer(fp, textBuffer, bufferShift);
            oldShift = 0;
            shift = 0;
            bufferLength = strlen(textBuffer);
            newBuffer = false;
            // printf("Buffer: %s, len: %d\n", textBuffer, bufferLength);
        }
        int j = patternLength - 1;
        // printf(" shift: %d\n", shift);
        while (j >= 0 && pattern[j] == textBuffer[shift+j]) {
            j--;
        }
        if (j < 0) {
            // printf("-> Match at byte %d\n", bytePosition+shift);
            results.push_back(bytePosition+shift);
            if (shift + patternLength < bufferLength && !lastBuffer) {
                oldShift = shift;
                shift += patternLength-badCharTable[textBuffer[shift+patternLength]];
            } else {
                oldShift = shift;
                shift += 1;
            }
        } else {
            oldShift = shift;
            // printf("\tnum: %d (%c) -> %d\n", j, textBuffer[shift+j], badCharTable[textBuffer[shift+j]]);
            oldShift = shift;
            shift += max(1, j - badCharTable[textBuffer[shift+j]]);
        }
        if (lastBuffer && shift > (bufferLength - patternLength)) {
            break;
        }
        if (shift+j >= bufferLength) {
            // bufferShift = 1-(patternLength - (bufferLength - shift));
            bufferShift = -(bufferLength+1 - (oldShift + patternLength));
            newBuffer = true;
            // printf("  continue... (-(%d - (%d + %d))) -> %d\n", bufferLength, oldShift, patternLength, bufferShift);
            continue;
        }
    }
    return results;
}
