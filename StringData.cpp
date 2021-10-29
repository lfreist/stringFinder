//
// Created by lfreist on 29.10.21.
//

#include <getopt.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "./StringData.hpp"

using namespace std;

// ____________________________________________________________________________
StringData::StringData() {}

// ____________________________________________________________________________
StringData::~StringData() {
    //for (string& str : _data) {
    //    delete str;
    //}
}

// ____________________________________________________________________________
void StringData::parseCommandLineArguments(int argc, char **argv) {
    struct option options[] = {
            {NULL, 0, NULL, 0 }
    };
    optind = 1;
    while (true) {
        char c = getopt_long(argc, argv, "", options, NULL);
        if (c == -1) {
            break;
        }
        switch (c) {
            default: break;
        }
    }
    if (optind >= argc) {
        exit(1);
    }
    readFile(argv[optind]);
}

// ____________________________________________________________________________
void StringData::readFile(string path, bool deleteOld) {
    string line;
    ifstream file(path.c_str());
    if (!file.is_open()) {
        cerr << "Error: Cannot open file" << path << endl;
        exit(1);
    }
    if (deleteOld) {
        //for (string& str : _data) {
        //    delete str;
        //}
        _data.clear();
    }
    while (!file.eof()) {
        getline(file, line);
        _data.push_back(line);
    }
}

// ____________________________________________________________________________
vector<string> StringData::find(string expression, bool matchCase) {
    vector<string> results;
    string newStr = "";
    cout << endl << "Search for: " << expression;
    if (matchCase) {
        cout << " match case!" << endl;
    }
    if (!matchCase) {
        expression = toLower(expression);
    }
    for (string& str : _data) {
        newStr = str;
        if (!matchCase) {
            newStr = toLower(newStr);
        }
        if (newStr.find(expression) != string::npos) {
            results.push_back(str);
            cout << "added " << str << endl;
        }
    }
    return results;
}

// ____________________________________________________________________________
// ____________________________________________________________________________
string toLower(string str) {
    string strLower = "";
    for (char& c : str) {
        if (c >= 65 && c <= 90) {
            strLower += c + 32;
        } else {
            strLower += c;
        }
    }
    return strLower;
}