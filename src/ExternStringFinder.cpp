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

// char textBuffer[MAX_BUFFER_SIZE+1+BUFFER_OVERFLOW];

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
    // fix this...
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

int findPattern(char* pattern, char* content) {
    char* tmp = content;
    char* tmp2 = content;
    int len;
    int position;
    int counter = 0;
    int shift;
    while (true) {
        tmp = strstr(tmp, pattern);
        if (tmp == NULL) {
            return counter;
        }
        counter++;
        tmp2 = tmp;
        tmp = strchr(tmp, '\n');
        if (tmp == NULL) {
            return counter;
        }
        position = strlen(content) - strlen(tmp2);
        shift = 0;
        while (position >= 0 && content[position] != '\n') {
            position--;
            shift++;
        }
        len = strlen(tmp2) - strlen(tmp) + shift;
        printf("%.*s",  len, content + position + 1);
    }
}

int find1(char* pattern, char* filename) {
    FILE* fp = fopen(filename, "r");
    int s;
    int counter = 0;
    char textBuffer[MAX_BUFFER_SIZE+1+BUFFER_OVERFLOW];
    while (true) {
        s = nextBuffer1(fp, textBuffer);
        if (s < 0) {
            exit(1);
        } else if( s == 0) {
            break;
        }
        counter += findPattern(pattern, textBuffer);
    }
    fclose(fp);
    return counter;
}

int find2(char* pattern, char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        exit(1);
    }
    int s;
    int counter = 0;
    char textBuffer[MAX_BUFFER_SIZE+1+BUFFER_OVERFLOW];
    while (true) {
        s = nextBuffer2(fd, textBuffer);
        if (s < 0) {
            exit(1);
        } else if( s == 0) {
            break;
        }
        counter += findPattern(pattern, textBuffer);
    }
    close(fd);
    return counter;
}
