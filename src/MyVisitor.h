//
// Created by Kone M on 31/03/2017.
//

#ifndef SHELL_MYVISITOR_H
#define SHELL_MYVISITOR_H

#include <zconf.h>
#include <sys/param.h>
#include <dirent.h>
#include <Utils.h>
#include <wait.h>
#include <atn/SemanticContext.h>
#include "../gen/ShellGrammarBaseVisitor.h"

class MyVisitor : public ShellGrammarBaseVisitor {
public:
    int OUTFD = 1;
    int INFD = 0;

    MyVisitor() : ShellGrammarBaseVisitor() {};

    ~MyVisitor() {};

    antlrcpp::Any visitShell(ShellGrammarParser::ShellContext *ctx) override {
        visit(ctx->command());
        return NULL;
    }

    antlrcpp::Any visitGoCommand(ShellGrammarParser::GoCommandContext *ctx) override {
        chdir(visit(ctx->filePath()).as<string>().c_str());
        return (int) 0;
    }

    antlrcpp::Any visitListCommand(ShellGrammarParser::ListCommandContext *ctx) override {
        int PID = fork();
        if (PID == 0) {
            //Load given path or current directory
            char temp[MAXPATHLEN];
            if (ctx->filePath() == NULL) getcwd(temp, MAXPATHLEN);
            else strcpy(temp, visit(ctx->filePath()).as<string>().c_str());

            //Print to console
            DIR *dir;
            dir = opendir(temp);
            struct dirent *dp;
            while ((dp = readdir(dir)) != NULL) {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                    string str = dp->d_name;
                    write(OUTFD, str.c_str(), str.length());
                    write(OUTFD, "\n", 1);
                }
            }
            exit(0);
        }
        return PID;
    }

    antlrcpp::Any visitHereCommand(ShellGrammarParser::HereCommandContext *ctx) override {
        int PID = fork();
        if (PID == 0) {
            char temp[MAXPATHLEN];
            string str = getcwd(temp, MAXPATHLEN) ? string(temp) : string("");
            write(OUTFD, str.c_str(), str.length());
            write(OUTFD, "\n", 1);
            exit(0);
        }
        return PID;
    }

    antlrcpp::Any visitExitCommand(ShellGrammarParser::ExitCommandContext *ctx) override {
        exit(0);
    }

    antlrcpp::Any visitRunProgram(ShellGrammarParser::RunProgramContext *ctx) override {
        return ShellGrammarBaseVisitor::visitRunProgram(ctx);
    }

    antlrcpp::Any visitSTDOUTToFile(ShellGrammarParser::STDOUTToFileContext *ctx) override {
        return ShellGrammarBaseVisitor::visitSTDOUTToFile(ctx);
    }

    antlrcpp::Any visitMultipleCommands(ShellGrammarParser::MultipleCommandsContext *ctx) override {
        return ShellGrammarBaseVisitor::visitMultipleCommands(ctx);
    }

    antlrcpp::Any visitStandaloneProcess(ShellGrammarParser::StandaloneProcessContext *ctx) override {
        int PID = visit(ctx->process()).as<int>();
        if (PID > 0) {
            int status;
            waitpid(PID, &status, 0);
        }
        return NULL;
    }

    antlrcpp::Any visitFileToSTDIN(ShellGrammarParser::FileToSTDINContext *ctx) override {
        return ShellGrammarBaseVisitor::visitFileToSTDIN(ctx);
    }

    antlrcpp::Any visitPipe(ShellGrammarParser::PipeContext *ctx) override {
        return ShellGrammarBaseVisitor::visitPipe(ctx);
    }

    antlrcpp::Any visitArguments(ShellGrammarParser::ArgumentsContext *ctx) override {
        vector<string> args;
        for (vector<antlr4::tree::ParseTree *>::size_type i = 0; i != ctx->children.size(); i++) {
            args.push_back((string) visit(ctx->children[i]).as<string>());
        }
        return args;
    }

    antlrcpp::Any visitQuotedFilepath(ShellGrammarParser::QuotedFilepathContext *ctx) override {
        return Utils::replaceString(
                ctx->QUOTED_FILEPATH()->getText().substr(1, ctx->QUOTED_FILEPATH()->getText().length() - 1), "~",
                getenv("HOME"));
    }

    antlrcpp::Any visitEscapedFilepath(ShellGrammarParser::EscapedFilepathContext *ctx) override {
        return Utils::replaceString(Utils::replaceString(ctx->ESCAPED_FILEPATH()->getText(), "\ ", " "), "~",
                                    getenv("HOME"));
    }

    antlrcpp::Any visitQuotedString(ShellGrammarParser::QuotedStringContext *ctx) override {
        return Utils::replaceString(
                ctx->QUOTED_STRING()->getText().substr(1, ctx->QUOTED_STRING()->getText().length() - 1), "~",
                getenv("HOME"));
    }

    antlrcpp::Any visitEscapedString(ShellGrammarParser::EscapedStringContext *ctx) override {
        return Utils::replaceString(Utils::replaceString(ctx->ESCAPED_STRING()->getText(), "\ ", " "), "~",
                                    getenv("HOME"));
    }

};


#endif //SHELL_MYVISITOR_H
