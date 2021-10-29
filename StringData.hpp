//
// Created by lfreist on 29.10.21.
//

#ifndef BACHELORPROJEKT_STRINGDATA_H
#define BACHELORPROJEKT_STRINGDATA_H

#include <gtest/gtest.h>

#include <vector>
#include <string>

using namespace std;

// Class StringData
class StringData {
public:
    StringData();
    ~StringData();
    void parseCommandLineArguments(int argc, char** argv);
    void readFile(string path, bool deleteOld = true);
    vector<string> find(string expression, bool matchCase = false);
private:
    vector<string> _data;

    FRIEND_TEST(StringDataTest, readFile);
};

// Converts string to lower case (not in place as in algorithm::transform)
string toLower(string str);

#endif //BACHELORPROJEKT_STRINGDATA_H
