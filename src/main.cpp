#include <iostream>
#include <ANTLRInputStream.h>
#include <CommonTokenStream.h>
#include <tree/ParseTree.h>
#include "../gen/ShellGrammarLexer.h"
#include "../gen/ShellGrammarParser.h"
#include "MyVisitor.h"
#include "string"

int main() {
	std::cout << "Hello, World!" << std::endl;

    for (;;) {
        char temp[MAXPATHLEN];
        std::string path = (getcwd(temp, MAXPATHLEN) ? std::string(temp) : std::string("")) ;
        std::string::size_type pos = path.find_last_of( "\\/" );
        path = path.substr(pos + 1,path.length());
        path = path.length() == 0 ? "/" : path ;
        std::cout << path<< "$ ";
        std::string input;
        std::getline(std::cin, input);
//
        std::cout <<"THe input is: "  << input << std::endl;
        // Create input stream, create lexer and use lexer to create stream of tokens
        antlr4::ANTLRInputStream inputStream(input);
        ShellGrammarLexer lexer(&inputStream);
        antlr4::CommonTokenStream tokens(&lexer);

        // Create parser
        ShellGrammarParser parser(&tokens);
        antlr4::tree::ParseTree *parseTree = parser.command();

//	 Then, visit your tree
        MyVisitor visitor;
        visitor.visit(parseTree);
    }// Read some line of input....

	return 0;
}
