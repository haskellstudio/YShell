//
// Created by bemacized on 4/1/17.
//

#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <cstring>
#include <string>
#include <vector>
#include <zconf.h>

using namespace std;

class Utils {
public:

    // From stack overflow answer: http://stackoverflow.com/a/53878
    static vector<string> split(const char *str, char c = ' ');

    // From stack overflow answer: http://stackoverflow.com/a/20446239
    static bool hasSuffix(const std::string &str, const std::string &suffix);

    // From stack overflow answer: http://stackoverflow.com/a/14678894
    static string replaceString(string subject, const string &search, const string &replace);

};


#endif //SHELL_UTILS_H
