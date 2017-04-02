#include "Utils.h"

// From stack overflow answer: http://stackoverflow.com/a/53878
vector<string> Utils::split(const char *str, char c) {
    vector<string> result;
    do {
        const char *begin = str;
        while(*str != c && *str) str++;
        result.push_back(string(begin, str));
    } while (0 != *str++);
    return result;
}

// From stack overflow answer: http://stackoverflow.com/a/20446239
bool Utils::hasSuffix(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// From stack overflow answer: http://stackoverflow.com/a/14678894
string Utils::replaceString(string subject, const string &search, const string &replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}