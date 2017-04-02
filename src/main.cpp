#include <iostream>
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <tree/ParseTree.h>
#include "../gen/ShellGrammarLexer.h"
#include "../gen/ShellGrammarParser.h"
#include "YShellVisitor.h"

int main() {

    //Home directory is default
    //chdir(getenv("HOME"));

    for (;;) {

        // Construct & show prompt line
        char temp[MAXPATHLEN];
        std::string path = (getcwd(temp, MAXPATHLEN) ? std::string(temp) : std::string(""));
        std::string::size_type pos = path.find_last_of("\\/");
        path = path.substr(pos + 1, path.length());
        path = path.length() == 0 ? "/" : path;
        std::cout << std::endl << path << "$ ";

        // Obtain user input
        std::string input;
        std::getline(std::cin, input);

        // Create input stream, create lexer and use lexer to create stream of tokens
        antlr4::ANTLRInputStream inputStream(input);
        ShellGrammarLexer lexer(&inputStream);
        antlr4::CommonTokenStream tokens(&lexer);

        // Create parser
        ShellGrammarParser parser(&tokens);
        antlr4::tree::ParseTree *parseTree = parser.shell();

        // Visit the tree
        YShellVisitor visitor;
        visitor.visit(parseTree);
    }

}
