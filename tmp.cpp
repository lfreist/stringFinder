#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "src/utils/UninitializedAllocator.h"

using strtype = std::basic_string<char, std::char_traits<char>, ad_utility::default_init_allocator<char>>;

int main(int argc, char** argv) {
  int fd = open(argv[1], O_RDONLY);
  strtype content = "dies das ananas";
  std::cout << content << std::endl;
  content.resize(40);
  std::cout << content << std::endl;
  ssize_t len = 1;
  while (len > 0) {
    len = read(fd, &content[15], 40 - 15);
    std::cout << len << std::endl;
  }
  std::cout << len << std::endl;
  close(fd);
}