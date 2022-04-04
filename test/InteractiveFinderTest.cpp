// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <iostream>

#include "../src/InteractiveStringFinder.h"

// ____________________________________________________________________________
TEST(InteractiveFinderTest, parseInput) {
  InputParser ip;
  {
    ip.parse("find expression");
    Command *cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_EQ(*cmd->getObject(), "expression");
  }
  // exit
  {
    // exit
    ip.parse("exit");
    Command *cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 1);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
    // exit with object
    ip.parse("exit object");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 1);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
    // exit with object and multiple arguments
    ip.parse("exit object --arg0 --arg1");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 1);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
  }
  // find
  {
    // find expression
    ip.parse("find expression");
    Command *cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_EQ(*cmd->getObject(), "expression");
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
    // find without expression
    ip.parse("find");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_FALSE(ip.validInput());

    ip.parse("find --arg1");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_EQ(cmd->getArguments()->size(), 1);
    ASSERT_FALSE(ip.validInput());
    // find expression --arg
    ip.parse("find expression --lines 5 --arg2");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_EQ(*cmd->getObject(), "expression");
    ASSERT_EQ(cmd->getArguments()->size(), 2);
    ASSERT_EQ(*cmd->getArguments()->at(0).getName(), "lines");
    ASSERT_EQ(cmd->getArguments()->at(0).getValue(), 5);
    ASSERT_EQ(*cmd->getArguments()->at(1).getName(), "arg2");
    ASSERT_TRUE(ip.validInput());
    // find expression with '-'
    ip.parse("find expression-test --arg1");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_EQ(*cmd->getObject(), "expression-test");
    ASSERT_EQ(cmd->getArguments()->size(), 1);
    ASSERT_EQ(*cmd->getArguments()->at(0).getName(), "arg1");
    ASSERT_TRUE(ip.validInput());
    // find expression with spaces
    ip.parse("find \"expression test\" --arg1");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 2);
    ASSERT_EQ(*cmd->getObject(), "expression test");
    ASSERT_EQ(cmd->getArguments()->size(), 1);
    ASSERT_EQ(*cmd->getArguments()->at(0).getName(), "arg1");
    ASSERT_TRUE(ip.validInput());
  }
  // load
  {
    // load file
    ip.parse("load /path/to/file");
    Command *cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 3);
    ASSERT_EQ(*cmd->getObject(), "/path/to/file");
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
    // load without file
    ip.parse("load");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 3);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_FALSE(ip.validInput());

    ip.parse("load --arg1");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 3);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_EQ(cmd->getArguments()->size(), 1);
    ASSERT_EQ(*cmd->getArguments()->at(0).getName(), "arg1");
    ASSERT_FALSE(ip.validInput());
    // find expression --arg
    ip.parse("load file --arg1 5 --arg2");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 3);
    ASSERT_EQ(*cmd->getObject(), "file");
    ASSERT_EQ(cmd->getArguments()->size(), 2);
    ASSERT_EQ(*cmd->getArguments()->at(0).getName(), "arg1");
    ASSERT_EQ(cmd->getArguments()->at(0).getValue(), -1);
    ASSERT_EQ(*cmd->getArguments()->at(1).getName(), "arg2");
    ASSERT_TRUE(ip.validInput());
  }
  // help
  {
    // help
    ip.parse("help");
    Command *cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 4);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
    // help with object
    ip.parse("help object");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 4);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
    // exit with object and multiple arguments
    ip.parse("help object --arg0 --arg1");
    cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 4);
    ASSERT_TRUE(cmd->getObject()->empty());
    ASSERT_TRUE(cmd->getArguments()->empty());
    ASSERT_TRUE(ip.validInput());
  }
  // no input
  {
    ip.parse("");
    Command *cmd = ip.getCommand();
    ASSERT_EQ(cmd->getName(), 0);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
