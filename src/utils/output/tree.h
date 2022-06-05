// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#pragma once

#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::string;
using std::vector;

#ifndef STRINGFINDER_SRC_UTILS_OUTPUT_TREE_H_
#define STRINGFINDER_SRC_UTILS_OUTPUT_TREE_H_

namespace sf_utils::output {

class Node {
 public:
  explicit Node(string content = "") {
    _content = std::move(content);
  }
  ~Node() = default;

  string parse(int stage, bool last = false, bool parentIsLast = true) {
    std::stringstream output;
    if (parentIsLast) {
      for (int i = 0; i < stage; ++i) {
        output << " ";
      }
    } else {
      for (int i = 0; i < stage - 3; ++i) {
        output << " ";
      }
      output << "│  ";
    }
    if (last) {
      output << "└─ ";
    } else {
      output << "├─ ";
    }
    output << _content << std::endl;
    for (ulong i = 0; i < _children.size(); ++i) {
      output << _children[i].parse(stage+3, (i == _children.size() -1), last);
    }
    return output.str();
  }

  void addChild(const Node& child) {
    _children.push_back(child);
  }

  Node& getChild(int index) {
    index = index < 0 ? static_cast<int>(_children.size() + index) : index;
    return _children[index];
  }

 private:
  string _content;
  vector<Node> _children = {};
};

class Tree {
 public:
  explicit Tree(Node& root, string title = "", string subtitle = "") {
    _root = root;
    _title = std::move(title);
    _subtitle = std::move(subtitle);
  }
  ~Tree() = default;

  string parse() {
    std::stringstream output;
    output << (_title.empty() ? "" : '\n' + _title + '\n')
           << (_subtitle.empty() ? "" : " " + _subtitle + '\n');
    output << _root.parse(1, true);
    return output.str();
  }

  Node& getRoot() {
    return _root;
  }

  void setTitle(string title) {
    _title = std::move(title);
  }

  void setSubtitle(string subtitle) {
    _subtitle = std::move(subtitle);
  }

 private:
  string _title;
  string _subtitle;
  Node _root;
};

}

#endif //STRINGFINDER_SRC_UTILS_OUTPUT_TREE_H_
