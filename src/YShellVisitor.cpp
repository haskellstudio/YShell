//
// Created by Kone M on 31/03/2017.
//

#include "YShellVisitor.h"

antlrcpp::Any YShellVisitor::visitShell(ShellGrammarParser::ShellContext *ctx) {
    visit(ctx->command());
    return NULL;
}

antlrcpp::Any YShellVisitor::visitGoCommand(ShellGrammarParser::GoCommandContext *ctx) {
    chdir(visit(ctx->filePath()).as<string>().c_str());
    return (int) 0;
}

antlrcpp::Any YShellVisitor::visitListCommand(ShellGrammarParser::ListCommandContext *ctx) {
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

antlrcpp::Any YShellVisitor::visitHereCommand(ShellGrammarParser::HereCommandContext *ctx) {
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

antlrcpp::Any YShellVisitor::visitExitCommand(ShellGrammarParser::ExitCommandContext *ctx) {
    exit(0);
}

antlrcpp::Any YShellVisitor::visitRunProgram(ShellGrammarParser::RunProgramContext *ctx) {
    // Obtain arguments
    vector<string> arguments = (vector<string>) visit(ctx->arguments()).as<vector<string>>();

    //Obtain file path
    string filePath = (string) visit(ctx->filePath()).as<string>();
    if (Utils::hasSuffix(filePath, "/")) filePath = filePath.substr(0, filePath.length() - 1);

    //Obtain current path
    char temp[MAXPATHLEN];
    string here = getcwd(temp, MAXPATHLEN) ? string(temp) : string("");
    if (Utils::hasSuffix(here, "/")) here = here.substr(0, here.length() - 1);

    //Check if filepath is absolute
    if (filePath.find("/") == 0) {

        //Check if file exists
        struct stat buffer;
        if (stat(filePath.c_str(), &buffer) != 0) {
            string str = "No executable has been found at path '" + filePath + "'.\n";
            write(OUTFD, str.c_str(), str.length());
            return 0;
        }
    }
        //If relative
    else {
        string newFilePath = here + "/" + filePath;

        //Check if file exists
        struct stat buffer;
        if (stat(newFilePath.c_str(), &buffer) != 0 || Utils::hasSuffix(newFilePath.c_str(), "/")) {
            //Check if file exists in PATH

            //Get PATH
            string PATH = getenv("PATH");

            //Split PATH
            vector<string> locations = Utils::split(PATH.c_str(), ':');

            //Try all paths specified
            for (vector<string>::size_type i = 0; i != locations.size(); i++) {
                //Define location
                string location = locations[i];
                if (Utils::hasSuffix(location, "/")) location = location.substr(0, location.length() - 1);

                //Check if it exists
                newFilePath = location + "/" + filePath;

                //break if we found something
                if (stat(newFilePath.c_str(), &buffer) == 0 && !Utils::hasSuffix(newFilePath.c_str(), "/")) break;

                //Otherwise set it back to null
                newFilePath = "";
            }
        }

        //Quit here if no executable found
        if (newFilePath == "") {
            string str = "No executable has been found at path '" + filePath + "'.\n";
            write(OUTFD, str.c_str(), str.length());
            return 0;
        }

        filePath = newFilePath;
    }

    // Add executable name as first argument
    vector<string> pathSplit = Utils::split(filePath.c_str(), '/');
    arguments.insert(arguments.begin(), pathSplit[pathSplit.size() - 1]);

    // Execute process
    return YShellVisitor::exec(filePath, arguments, INFD, OUTFD);
}

antlrcpp::Any YShellVisitor::visitSTDOUTToFile(ShellGrammarParser::STDOUTToFileContext *ctx) {
    // Get file name
    string file = (string) visit(ctx->filePath()).as<string>();

    // Open file descriptor for file
    int fdo = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);

    // Set output stream to file
    int fdBackup = OUTFD;
    OUTFD = fdo;

    // Run process
    int PID = (int) visit(ctx->process()).as<int>();

    // Close output stream on this side
    close(fdo);

    // Reset output stream back to STDOUT
    OUTFD = fdBackup;

    // Wait for process to finish
    int status = 0;
    if (PID > 0) waitpid(PID, &status, 0);

    // Report success
    string str = "Successfully written to " + file + ".";
    write(OUTFD, str.c_str(), str.length());
    return status;
}

antlrcpp::Any YShellVisitor::visitStandaloneProcess(ShellGrammarParser::StandaloneProcessContext *ctx) {
    int PID = visit(ctx->process()).as<int>();
    int status = 0;
    if (PID > 0) waitpid(PID, &status, 0);
    return status;
}

antlrcpp::Any YShellVisitor::visitFileToSTDIN(ShellGrammarParser::FileToSTDINContext *ctx) {
    // Get file name
    string file = (string) visit(ctx->filePath()).as<string>();

    // Open file descriptor for file
    int fdi = open(file.c_str(), O_RDONLY);

    // Check if file exists
    if (fdi < 0) {
        string str = "File '" + file + "' does not exist!";
        write(OUTFD, str.c_str(), str.length());
        return (int) 1;
    }

    // Set input stream to file
    int fdBackup = INFD;
    INFD = fdi;

    // Run process
    int PID = (int) visit(ctx->process()).as<int>();

    // Close input stream on this side
    close(fdi);

    // Reset input stream
    INFD = fdBackup;

    // Wait for process to finish
    int status = 0;
    if (PID > 0) waitpid(PID, &status, 0);
    return status;
}

antlrcpp::Any YShellVisitor::visitArguments(ShellGrammarParser::ArgumentsContext *ctx) {
    vector<string> args;
    for (vector<antlr4::tree::ParseTree *>::size_type i = 0; i != ctx->children.size(); i++) {
        args.push_back((string) visit(ctx->children[i]).as<string>());
    }
    return args;
}

antlrcpp::Any YShellVisitor::visitAndCommand(ShellGrammarParser::AndCommandContext *ctx) {
    //Execute first command
    int status = (int) visit(ctx->command()[0]).as<int>();

    //Only execute second command if the first executed successfully
    if (status == 0) status = (int) visit(ctx->command()[1]).as<int>();

    //Return final status
    return status;
}

antlrcpp::Any YShellVisitor::visitOrCommand(ShellGrammarParser::OrCommandContext *ctx) {
    //Execute first command
    int status = (int) visit(ctx->command()[0]).as<int>();

    //Only execute second command if the first failed
    if (status != 0) status = (int) visit(ctx->command()[1]).as<int>();

    //Return final status
    return status;
}

antlrcpp::Any YShellVisitor::visitPipe(ShellGrammarParser::PipeContext *ctx) {
    //Create pipe
    int p[2];
    pipe(p);

    //Redirect output of first process to pipe input
    int fdBackup = OUTFD;
    OUTFD = p[1];

    //Execute first process
    visit(ctx->process()[0]);

    //Reset output stream
    OUTFD = fdBackup;

    //Redirect pip output to process input
    fdBackup = INFD;
    INFD = p[0];

    //Execute second process
    int PID = (int) visit(ctx->process()[1]).as<int>();

    //Reset input stream
    INFD = fdBackup;

    //Close pipes if they have not yet
    close(p[0]);
    close(p[1]);

    //return PID of second process
    return PID;
}

antlrcpp::Any YShellVisitor::visitQuotedFilepath(ShellGrammarParser::QuotedFilepathContext *ctx) {
    return Utils::replaceString(
            ctx->QUOTED_FILEPATH()->getText().substr(1, ctx->QUOTED_FILEPATH()->getText().length() - 1), "~",
            getenv("HOME"));
}

antlrcpp::Any YShellVisitor::visitEscapedFilepath(ShellGrammarParser::EscapedFilepathContext *ctx) {
    return Utils::replaceString(Utils::replaceString(ctx->ESCAPED_FILEPATH()->getText(), "\ ", " "), "~",
                                getenv("HOME"));
}

antlrcpp::Any YShellVisitor::visitQuotedString(ShellGrammarParser::QuotedStringContext *ctx) {
    return Utils::replaceString(
            ctx->QUOTED_STRING()->getText().substr(1, ctx->QUOTED_STRING()->getText().length() - 1), "~",
            getenv("HOME"));
}

antlrcpp::Any YShellVisitor::visitEscapedString(ShellGrammarParser::EscapedStringContext *ctx) {
    return Utils::replaceString(Utils::replaceString(ctx->ESCAPED_STRING()->getText(), "\ ", " "), "~",
                                getenv("HOME"));
}

int YShellVisitor::exec(string file1, vector<string> args, int input, int output) {
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
