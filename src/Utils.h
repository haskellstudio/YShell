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
    static string replaceString(string subject, const string &search, const string &replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    static int xexec(string file1, vector<string> args, int input, int output) {
        //Fork process
        int cid = fork();

        if (cid == 0) {
            //Set output and input streams
            if (input != 0) {
                dup2(input, 0);
                close(input);
            }
            if (output != 1) {
                dup2(output, 1);
                close(output);
            }

            // Convert args to c style
            char **_args = (char **) calloc(args.size() + 1, sizeof(string));
            for (vector<string>::size_type i = 0; i != args.size(); i++)
                _args[i] = (char *) args[i].c_str();
            _args[args.size()] = (char *) NULL;

            //Execute program
            execv(file1.c_str(), _args);
            //Exit process
            exit(0);
        }

        //Close useless pipe ends
        if (input != 0) close(input);
        if (output != 1) close(output);

        //Return pid
        return cid;
    }
};


#endif //SHELL_UTILS_H
