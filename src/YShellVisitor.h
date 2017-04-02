#ifndef SHELL_MYVISITOR_H
#define SHELL_MYVISITOR_H

#include <zconf.h>
#include <sys/param.h>
#include <dirent.h>
#include <Utils.h>
#include <wait.h>
#include <atn/SemanticContext.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../gen/ShellGrammarBaseVisitor.h"

class YShellVisitor : public ShellGrammarBaseVisitor {
public:
    int ERRFD = 2;
    int OUTFD = 1;
    int INFD = 0;

    YShellVisitor() : ShellGrammarBaseVisitor() {};

    ~YShellVisitor() {};

    antlrcpp::Any visitShell(ShellGrammarParser::ShellContext *ctx) override;

    antlrcpp::Any visitCDCommand(ShellGrammarParser::CDCommandContext *ctx) override;

    antlrcpp::Any visitExitCommand(ShellGrammarParser::ExitCommandContext *ctx) override;

    antlrcpp::Any visitRunProgram(ShellGrammarParser::RunProgramContext *ctx) override;

    antlrcpp::Any visitSTDOUTToFile(ShellGrammarParser::STDOUTToFileContext *ctx) override;

    antlrcpp::Any visitFileToSTDIN(ShellGrammarParser::FileToSTDINContext *ctx) override;

    antlrcpp::Any visitPipe(ShellGrammarParser::PipeContext *ctx) override;

    antlrcpp::Any visitArguments(ShellGrammarParser::ArgumentsContext *ctx) override;

    antlrcpp::Any visitQuotedFilepath(ShellGrammarParser::QuotedFilepathContext *ctx) override;

    antlrcpp::Any visitEscapedFilepath(ShellGrammarParser::EscapedFilepathContext *ctx) override;

    antlrcpp::Any visitQuotedString(ShellGrammarParser::QuotedStringContext *ctx) override;

    antlrcpp::Any visitEscapedString(ShellGrammarParser::EscapedStringContext *ctx) override;

    antlrcpp::Any visitAndCommand(ShellGrammarParser::AndCommandContext *ctx) override;

    antlrcpp::Any visitOrCommand(ShellGrammarParser::OrCommandContext *ctx) override;

    antlrcpp::Any visitSTDERRToFile(ShellGrammarParser::STDERRToFileContext *ctx) override;

    antlrcpp::Any visitSTDOUTAppendToFile(ShellGrammarParser::STDOUTAppendToFileContext *ctx) override;

    antlrcpp::Any visitBGCommand(ShellGrammarParser::BGCommandContext *ctx) override;

    antlrcpp::Any visitSequenceCommand(ShellGrammarParser::SequenceCommandContext *ctx) override;

    static int exec(string file1, vector<string> args, int input = 0, int output = 1, int error = 2);

};


#endif //SHELL_MYVISITOR_H
