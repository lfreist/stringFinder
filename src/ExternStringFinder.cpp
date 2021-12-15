// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <getopt.h>

#include <iostream>

#include "ExternStringFinder.hpp"
#include "Timer.hpp"

// ____________________________________________________________________________________________________________________
int nextBuffer1(FILE *fp, char *buffer) {
  char c;
  size_t s = fread(buffer, INIT_BUFFER_SIZE, 1, fp);
  if (s == 0) { return 0; }
  for (int i = INIT_BUFFER_SIZE; i < MAX_BUFFER_SIZE; i++) {
    if ((c = fgetc(fp)) == EOF) {
      buffer[i] = '\0';
      return i;
    }
    if (c == '\n') {
      buffer[i] = c;
      buffer[i + 1] = '\0';
      return i;
    }
    buffer[i] = c;
  }
  return -1;
}

int nextBuffer2(int fd, char *buffer) {
  char c;
  ssize_t s = read(fd, buffer, INIT_BUFFER_SIZE);
  if (s == 0) { return 0; }
  for (int i = INIT_BUFFER_SIZE; i < MAX_BUFFER_SIZE; i++) {
    if (read(fd, &c, 1) == 0) {
      buffer[i] = '\0';
      return i;
    }
    if (c == '\n') {
      buffer[i] = c;
      buffer[i+1] = '\0';
      return i;
    }
    buffer[i] = c;
  }
  return -1;
}

int findPattern(char *pattern, char *content) {
  char *tmp = content;
  char *tmp2;
  int len;
  int position;
  int counter = 0;
  unsigned long shift;
  while (true) {
    tmp = strstr(tmp, pattern);
    if (tmp == nullptr) {
      return counter;
    }
    counter++;
    tmp2 = tmp;
    tmp = strchr(tmp, '\n');
    if (tmp == nullptr) {
      return counter;
    }
    position = strlen(content) - strlen(tmp2);
    shift = 0;
    while (position >= 0 && content[position] != '\n') {
      position--;
      shift++;
    }
    len = strlen(tmp2) - strlen(tmp) + shift;
    printf("%.*s", len, content + position + 1);
  }
}

int find1(char *pattern, char *filename, bool performance, bool count) {
  FILE *fp = fopen(filename, "r");
  int s;
  int counter = 0;
  char textBuffer[MAX_BUFFER_SIZE];
  Timer timer;
  if (performance) {
    timer.start(true);
  }
  while (true) {
    s = nextBuffer1(fp, textBuffer);
    if (s < 0) {
      exit(1);
    } else if (s == 0) {
      break;
    }
    counter += findPattern(pattern, textBuffer);
  }
  if (count) {
    std::cout << "Found " << counter << " Matches" << std::endl;
  }
  if (performance) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std:: endl;
  }
  fclose(fp);
  return counter;
}

int find2(char *pattern, char *filename, bool performance, bool count) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    exit(1);
  }
  int s;
  int counter = 0;
  char textBuffer[MAX_BUFFER_SIZE];
  Timer timer;
  if (performance) {
    timer.start(true);
  }
  while (true) {
    s = nextBuffer2(fd, textBuffer);
    if (s < 0) {
      exit(1);
    } else if (s == 0) {
      break;
    }
    counter += findPattern(pattern, textBuffer);
  }
  if (count) {
    std::cout << "Found " << counter << " Matches" << std::endl;
  }
  if (performance) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std:: endl;
  }
  close(fd);
  return counter;
}

// ____________________________________________________________________________________________________________________
// ExternFinder Class

ExternFinder::ExternFinder() {
  buffer = new char[MAX_BUFFER_SIZE];
  performance = false;
  silent = false;
  count = false;
  filename = nullptr;
  pattern = nullptr;
}

ExternFinder::~ExternFinder() {
  delete[] buffer;
}

void ExternFinder::parseCommandLineArguments(int argc, char **argv) {
  struct option options[] = {
    {"performance", 0, nullptr, 'p'},
    {"silent", 0, nullptr, 's'},
    {"count", 0, nullptr, 'c'},
    {nullptr, 0, nullptr, 0}
  };
  optind = 1;
  while (true) {
    int c = getopt_long(argc, argv, "p:s:c", options, nullptr);
    if (c == -1) { break; }
    switch (c) {
      case 'p': performance = true; break;
      case 's': silent = true; break;
      case 'c': count = true; break;
      default: break;
    }
  }
  if (optind >= argc) {
    std::cout << "Missing input file or pattern" << std::endl;
  }
  pattern = argv[optind++];
  filename = argv[optind];
}

int ExternFinder::find(char *pat) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    printf("Could not open file '%s'\n", filename);
    exit(1);
  }
  int s;
  int counter = 0;
  if (performance) {
    timer.start(true);
  }
  while (true) {
    s = nextBuffer(fd);
    if (s < 0) {
      exit(1);
    } else if (s == 0) {
      break;
    }
    counter += findPattern(pat, buffer);
  }
  if (count) {
    std::cout << "Found " << counter << " Matches" << std::endl;
  }
  if (performance) {
    timer.stop();
    std::cout << "Time: " << timer.elapsedSeconds() << " s" << std:: endl;
  }
  close(fd);
  return counter;
}

int ExternFinder::find() {
  return find(pattern);
}

void ExternFinder::setFilePath(char *filepath) {
  delete[] filename;
  filename = filepath;
}

int ExternFinder::nextBuffer(int fd) {
  char c;
  ssize_t s = read(fd, buffer, INIT_BUFFER_SIZE);
  if (s == 0) { return 0; }
  for (int i = INIT_BUFFER_SIZE; i < MAX_BUFFER_SIZE; i++) {
    if (read(fd, &c, 1) == 0) {
      buffer[i+1] = '\0';
      return i;
    }
    if (c == '\n') {
      buffer[i] = c;
      buffer[i+1] = '\0';
      return i;
    }
    buffer[i] = c;
  }
  return -1;
}

int ExternFinder::findPattern(char *pat, char *content) const {
  char *tmp = content;
  char *tmp2;
  int len;
  int position;
  int counter = 0;
  unsigned long shift;
  while (true) {
    tmp = strstr(tmp, pat);
    if (tmp == nullptr) {
      return counter;
    }
    counter++;
    tmp2 = tmp;
    tmp = strchr(tmp, '\n');
    if (tmp == nullptr) {
      return counter;
    }
    if (silent) {
      continue;
    }
    position = strlen(content) - strlen(tmp2);
    shift = 0;
    while (position >= 0 && content[position] != '\n') {
      position--;
      shift++;
    }
    len = strlen(tmp2) - strlen(tmp) + shift;
    printf("%.*s", len, content + position + 1);
  }
}
