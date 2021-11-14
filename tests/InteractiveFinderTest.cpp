// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <iostream>

#include "../src/InteractiveFinder.hpp"

// ____________________________________________________________________________
TEST(InteractiveFinderTest, parseInput) {
    InputParser ip;
    {
        ip.parse("find expression");
        Command* cmd = ip.getCommand();
        ASSERT_EQ(cmd->getName(), 2);
        ASSERT_EQ(*(cmd->getObject()), "expression");
    }
    /*
    // exit
    {
        // exit
        ip.parse("exit");
        ASSERT_EQ(ip._command, 1);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
        // exit with object
        ip.parse("exit object");
        ASSERT_EQ(ip._command, 1);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
        // exit with object and multiple arguments
        ip.parse("exit object --arg0 --arg1");
        ASSERT_EQ(ip._command, 1);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
    }
    // find
    {
        // find expression
        ip.parse("find expression");
        ASSERT_EQ(ip._command, 2);
        ASSERT_EQ(ip._object, "expression");
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
        // find without expression
        ip.parse("find");
        ASSERT_EQ(ip._command, 2);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_FALSE(ip.validInput());

        ip.parse("find --arg1");
        ASSERT_EQ(ip._command, 2);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_EQ(ip._arguments.size(), 1);
        ASSERT_FALSE(ip.validInput());
        // find expression --arg
        ip.parse("find expression --arg1");
        ASSERT_EQ(ip._command, 2);
        ASSERT_EQ(ip._object, "expression");
        ASSERT_EQ(ip._arguments.size(), 1);
        ASSERT_EQ(ip._arguments[0], "arg1");
        ASSERT_TRUE(ip.validInput());
        // find expression with '-'
        ip.parse("find expression-test --arg1");
        ASSERT_EQ(ip._command, 2);
        ASSERT_EQ(ip._object, "expression-test");
        ASSERT_EQ(ip._arguments.size(), 1);
        ASSERT_EQ(ip._arguments[0], "arg1");
        ASSERT_TRUE(ip.validInput());
    }
    // load
    {
        // load file
        ip.parse("load file");
        ASSERT_EQ(ip._command, 3);
        ASSERT_EQ(ip._object, "file");
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
        // load without file
        ip.parse("load");
        ASSERT_EQ(ip._command, 3);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_FALSE(ip.validInput());

        ip.parse("load --arg1");
        ASSERT_EQ(ip._command, 3);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_EQ(ip._arguments.size(), 1);
        ASSERT_FALSE(ip.validInput());
        // find expression --arg
        ip.parse("load file --arg1 --arg2");
        ASSERT_EQ(ip._command, 3);
        ASSERT_EQ(ip._object, "file");
        ASSERT_EQ(ip._arguments.size(), 2);
        ASSERT_EQ(ip._arguments[0], "arg1");
        ASSERT_EQ(ip._arguments[1], "arg2");
        ASSERT_TRUE(ip.validInput());
    }
    // help
    {
        // help
        ip.parse("help");
        ASSERT_EQ(ip._command, 4);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
        // exit with object
        ip.parse("help object");
        ASSERT_EQ(ip._command, 4);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
        // exit with object and multiple arguments
        ip.parse("help object --arg0 --arg1");
        ASSERT_EQ(ip._command, 4);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
    }
    // no input
    {
        ip.parse("");
        ASSERT_EQ(ip._command, 0);
        ASSERT_TRUE(ip._object.empty());
        ASSERT_TRUE(ip._arguments.empty());
        ASSERT_TRUE(ip.validInput());
    }
     */
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
