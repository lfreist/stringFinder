//
// Created by lfreist on 20.05.22.
//

#include <exception>

#ifndef SRC_UTILS_EXCEPTIONS_H
#define SRC_UTILS_EXCEPTIONS_H

namespace sf_utils {
struct NotUncompressedException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "There is no uncompressed content to perform this method on.";
  }
};

struct NotCompressedException : public std::exception {
  [[nodiscard]] const char *what() const noexcept override {
    return "There is no compressed content to perform this method on.";
  }
};

 struct FileReadError : public std::exception {
   [[nodiscard]] const char *what() const noexcept override {
     return "Could not open file for reading.";
   }
 };
}


#endif //  SRC_UTILS_EXCEPTIONS_H
