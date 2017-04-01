//
// Created by Kone M on 31/03/2017.
//

#ifndef SHELL_MYVISITOR_H
#define SHELL_MYVISITOR_H


#include <zconf.h>
#include <sys/param.h>
#include <dirent.h>
#include <Utils.h>
#include "../gen/ShellGrammarBaseVisitor.h"

class MyVisitor : public ShellGrammarBaseVisitor {
public:
    MyVisitor() : ShellGrammarBaseVisitor() {};

    ~MyVisitor() {};

    antlrcpp::Any visitShell(ShellGrammarParser::ShellContext *ctx) override {
        visit(ctx->command());
        return NULL;
    }

    antlrcpp::Any visitGoCommand(ShellGrammarParser::GoCommandContext *ctx) override {
        chdir(visit(ctx->filePath()).as<std::string>().c_str());
        return NULL;

    }

    antlrcpp::Any visitListCommand(ShellGrammarParser::ListCommandContext *ctx) override {
        char temp[MAXPATHLEN];
        if (ctx->filePath() == NULL) {
            getcwd(temp, MAXPATHLEN);
        } else {
            strcpy(temp, visit(ctx->filePath()).as<std::string>().c_str());
        }

        DIR *dir;
        dir = opendir(temp);
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL) {
            if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            } else {
                std::cout << " " << dp->d_name << std::endl;
            }

        }
        std::cout << std::endl;
        return NULL;
    }

    antlrcpp::Any visitHereCommand(ShellGrammarParser::HereCommandContext *ctx) override {
        char temp[MAXPATHLEN];
        std::cout << (getcwd(temp, MAXPATHLEN) ? std::string(temp) : std::string("")) << std::endl;
        return NULL;
    }

    antlrcpp::Any visitRunCommand(ShellGrammarParser::RunCommandContext *ctx) override {
        return NULL;
    }

    antlrcpp::Any visitSTDOUTToFile(ShellGrammarParser::STDOUTToFileContext *ctx) override {
        return ShellGrammarBaseVisitor::visitSTDOUTToFile(ctx);
    }

    antlrcpp::Any visitMultipleCommands(ShellGrammarParser::MultipleCommandsContext *ctx) override {
        return ShellGrammarBaseVisitor::visitMultipleCommands(ctx);
    }

    antlrcpp::Any visitStandaloneProcess(ShellGrammarParser::StandaloneProcessContext *ctx) override {
        return ShellGrammarBaseVisitor::visitStandaloneProcess(ctx);
    }

    antlrcpp::Any visitFileToSTDIN(ShellGrammarParser::FileToSTDINContext *ctx) override {
        return ShellGrammarBaseVisitor::visitFileToSTDIN(ctx);
    }

    antlrcpp::Any visitPipe(ShellGrammarParser::PipeContext *ctx) override {
        return ShellGrammarBaseVisitor::visitPipe(ctx);
    }

    antlrcpp::Any visitArguments(ShellGrammarParser::ArgumentsContext *ctx) override {
        return ShellGrammarBaseVisitor::visitArguments(ctx);
    }

    antlrcpp::Any visitQuotedFilepath(ShellGrammarParser::QuotedFilepathContext *ctx) override {
        return Utils::replaceString(ctx->QUOTED_FILEPATH()->getText().substr(1, ctx->QUOTED_FILEPATH()->getText().length()), "~", getenv("HOME"));
    }

    antlrcpp::Any visitEscapedFilepath(ShellGrammarParser::EscapedFilepathContext *ctx) override {
        return Utils::replaceString(Utils::replaceString(ctx->ESCAPED_FILEPATH()->getText(), "\ ", " "), "~", getenv("HOME"));
    }

    antlrcpp::Any visitQuotedString(ShellGrammarParser::QuotedStringContext *ctx) override {
        return Utils::replaceString(ctx->QUOTED_STRING()->getText().substr(1, ctx->QUOTED_STRING()->getText().length()), "~", getenv("HOME"));
    }

    antlrcpp::Any visitEscapedString(ShellGrammarParser::EscapedStringContext *ctx) override {
        return Utils::replaceString(Utils::replaceString(ctx->ESCAPED_STRING()->getText(), "\ ", " "), "~", getenv("HOME"));
    }


};


#endif //SHELL_MYVISITOR_H
