// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>

#include "StringData.hpp"

using std::string;

// ____________________________________________________________________________
TEST(StringDataTest, readFile) {
    StringData sd;
    ASSERT_EQ(sd._data.size(), 0);
    sd.readFile(string("data.test.txt"));
    ASSERT_EQ(sd._data.size(), 5);
    sd.readFile(string("data.test.txt"));
    ASSERT_EQ(sd._data.size(), 5);
    sd.readFile(string("data.test.txt"), false);
    ASSERT_EQ(sd._data.size(), 10);
}

// ____________________________________________________________________________
TEST(StringDataTest, findExpression) {
    StringData sd;
    sd.readFile(string("data.test.txt"));
    {
        vector<string> result = sd.find("NotInFile", true);
        ASSERT_EQ(result.size(), 0);
    }
    {
        vector<string> result = sd.find("l", false);
        ASSERT_EQ(result.size(), 5);
        ASSERT_STREQ(
                result[0].c_str(),
                "LinE with uppEr casE E");
        ASSERT_STREQ(
                result[1].c_str(),
                "Line without upper case e");
        ASSERT_STREQ(
                result[2].c_str(),
                "Lin3 without that charact3r");
        ASSERT_STREQ(
                result[3].c_str(),
                "Line with word eureka in lower case");
        ASSERT_STREQ(
                result[4].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
    {
        vector<string> result = sd.find("E", false);
        ASSERT_EQ(result.size(), 4);
        ASSERT_STREQ(
                result[0].c_str(),
                "LinE with uppEr casE E");
        ASSERT_STREQ(
                result[1].c_str(),
                "Line without upper case e");
        ASSERT_STREQ(
                result[2].c_str(),
                "Line with word eureka in lower case");
        ASSERT_STREQ(
                result[3].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
    {
        vector<string> result = sd.find("e", false);
        ASSERT_EQ(result.size(), 4);
        ASSERT_STREQ(
                result[0].c_str(),
                "LinE with uppEr casE E");
        ASSERT_STREQ(
                result[1].c_str(),
                "Line without upper case e");
        ASSERT_STREQ(
                result[2].c_str(),
                "Line with word eureka in lower case");
        ASSERT_STREQ(
                result[3].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
    {
        vector<string> result = sd.find("E", true);
        ASSERT_EQ(result.size(), 2);
        ASSERT_STREQ(
                result[0].c_str(),
                "LinE with uppEr casE E");
        ASSERT_STREQ(
                result[1].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
    {
        vector<string> result = sd.find("e", true);
        ASSERT_EQ(result.size(), 2);
        ASSERT_STREQ(
                result[0].c_str(),
                "Line without upper case e");
        ASSERT_STREQ(
                result[1].c_str(),
                "Line with word eureka in lower case");
    }
    {
        vector<string> result = sd.find("EurEka", true);
        ASSERT_EQ(result.size(), 1);
        ASSERT_STREQ(
                result[0].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
    {
        vector<string> result = sd.find("EurEka", false);
        ASSERT_EQ(result.size(), 2);
        ASSERT_STREQ(
                result[0].c_str(),
                "Line with word eureka in lower case");
        ASSERT_STREQ(
                result[1].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
    {
        vector<string> result = sd.find("eureka", true);
        ASSERT_EQ(result.size(), 1);
        ASSERT_STREQ(
                result[0].c_str(),
                "Line with word eureka in lower case");
    }
    {
        vector<string> result = sd.find("eureka", false);
        ASSERT_EQ(result.size(), 2);
        ASSERT_STREQ(
                result[0].c_str(),
                "Line with word eureka in lower case");
        ASSERT_STREQ(
                result[1].c_str(),
                "LinE with word EurEka with uppEr casE E");
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
