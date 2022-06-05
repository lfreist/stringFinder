//
// Created by lfreist on 05.06.22.
//

#include <iostream>

#include "src/utils/output/tree.h"

int main (int argc, char** argv) {
  sf_utils::output::Node root("root");
  sf_utils::output::Tree tree(root);
  tree.setTitle("Test tree");
  for (int i = 0; i < 4; ++i) {
    tree.getRoot().addChild(sf_utils::output::Node("child_" + std::to_string(i)));
  }
  for (int i = 0; i < 2; ++i) {
    tree.getRoot().getChild(2).addChild(sf_utils::output::Node("child_" + std::to_string(i)));
  }
  std::cout << tree.parse() << std::endl;
}