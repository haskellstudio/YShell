//
// Created by Kone M on 31/03/2017.
//

#ifndef SHELL_MYVISITOR_H
#define SHELL_MYVISITOR_H


#include <zconf.h>
#include <sys/param.h>
#include "../gen/ShellGrammarBaseVisitor.h"

class MyVisitor : public ShellGrammarBaseVisitor{
    public:
        MyVisitor() : ShellGrammarBaseVisitor(){};
        ~MyVisitor() {};

    antlrcpp::Any visitShell(ShellGrammarParser::ShellContext *ctx) override {
        return visit(ctx->command());
    }

    antlrcpp::Any visitGoCommand(ShellGrammarParser::GoCommandContext *ctx) override {
        chdir(ctx->PATH()->getText().c_str());

        return NULL;

    }

    antlrcpp::Any visitHereCommand(ShellGrammarParser::HereCommandContext *ctx) override {
        char temp[MAXPATHLEN];
        std::cout << (getcwd(temp, MAXPATHLEN) ? std::string(temp) : std::string("")) << std::endl;
        return NULL;
    }

    antlrcpp::Any visitRunCommand(ShellGrammarParser::RunCommandContext *ctx) override {
        return ShellGrammarBaseVisitor::visitRunCommand(ctx);
    }
};


#endif //SHELL_MYVISITOR_H
