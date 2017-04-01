//
// Created by bemacized on 4/1/17.
//

#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <cstring>
#include <string>

class Utils {
    public:
        static std::string replaceString(std::string subject, const std::string& search, const std::string& replace) {
            size_t pos = 0;
            while ((pos = subject.find(search, pos)) != std::string::npos) {
                subject.replace(pos, search.length(), replace);
                pos += replace.length();
            }
            return subject;
        }
};


#endif //SHELL_UTILS_H
