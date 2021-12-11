//
// Created by lfreist on 23.11.21.
//

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

#include <vector>

using std::vector;

#include "Timer.hpp"

#include "ExternStringFinder.hpp"

void readBuffer1(char *path) {
    FILE *fp = fopen(path, "r");
    char textBuffer[MAX_BUFFER_SIZE+1+BUFFER_OVERFLOW];
    char c;
    while (!feof(fp)) {
        fread(textBuffer, MAX_BUFFER_SIZE, 1, fp);
        for (int i = MAX_BUFFER_SIZE; i < BUFFER_OVERFLOW; i++) {
            if ((c = fgetc(fp)) == EOF) {
                textBuffer[i] = '\0';
                break;
            }
            if (c  == '\n') {
                textBuffer[i] = c;
                textBuffer[i+1] = '\0';
                break;
            }
            textBuffer[i] = c;
        }
        // printf("New Block \n");
    }
    fclose(fp);
}

void readBuffer2(char *path) {
    int fd = open(path, O_RDONLY);
    char textBuffer[MAX_BUFFER_SIZE+1+BUFFER_OVERFLOW];
    char c[2];
    c[1] = '\0';
    ssize_t s;
    while ((s = read(fd, textBuffer, MAX_BUFFER_SIZE)) > 0) {
        c[0] = textBuffer[s-1];
        for (int i = 0; i < BUFFER_OVERFLOW; i++) {
            if (c[0] == '\n') {
                textBuffer[MAX_BUFFER_SIZE+i+1] = '\0';
                break;
            }
            if (read(fd, c, 1) == 0) {
                textBuffer[MAX_BUFFER_SIZE+i+1] = '\0';
                break;
            }
            textBuffer[MAX_BUFFER_SIZE+i] = c[0];
        }
    }
    close(fd);
}

// ____________________________________________________________________________________________________________________
int nextBuffer1(FILE* fp, char* buffer) {
    char c;
    if (feof(fp)) {
        return 0;
    }
    fread(buffer, MAX_BUFFER_SIZE, 1, fp);
    for (int i = MAX_BUFFER_SIZE; i < BUFFER_OVERFLOW+MAX_BUFFER_SIZE; i++) {
        if ((c = fgetc(fp)) == EOF) {
            buffer[i] = '\0';
            return i;
        }
        if (c  == '\n') {
            buffer[i] = c;
            buffer[i+1] = '\0';
            return i;
        }
        buffer[i] = c;
    }
    return -1;
}

int nextBuffer2(int fd, char* buffer) {
    char c[2];
    c[1] = '\0';
    ssize_t s = read(fd, buffer, MAX_BUFFER_SIZE);
    if (s == 0) {
        return 0;
    }
    for (int i = MAX_BUFFER_SIZE; i < BUFFER_OVERFLOW+MAX_BUFFER_SIZE; i++) {
        if (c[0] == '\n') {
            buffer[i] = '\0';
            return i;
        }
        if (read(fd, c, 1) == 0) {
            buffer[i] = '\0';
            return i;
        }
        buffer[i] = c[0];
    }
    return -1;
}
