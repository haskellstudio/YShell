#include "YShellVisitor.h"

antlrcpp::Any YShellVisitor::visitShell(ShellGrammarParser::ShellContext *ctx) {
    int PID = (int) visit(ctx->command()).as<int>();
    int status = 0;
    if (PID > 0) waitpid(PID, &status, 0);
    return status;
}

antlrcpp::Any YShellVisitor::visitSequenceCommand(ShellGrammarParser::SequenceCommandContext *ctx) {
    int PID = 0;
    for (int i = 0; i < ctx->command().size(); ++i) {
        PID = (int) visit(ctx->command()[i]).as<int>();
        int status;
        if (PID > 0) waitpid(PID, &status, 0);
    }
    return PID;
}

antlrcpp::Any YShellVisitor::visitCDCommand(ShellGrammarParser::CDCommandContext *ctx) {
    chdir(ctx->filePath() == NULL ? getenv("HOME") : visit(ctx->filePath()).as<string>().c_str());
    return (int) 0;
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
            string str = "No executable has been found at path '" + filePath + "'.";
            write(ERRFD, str.c_str(), str.length());
            write(ERRFD, "\n", 1);
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
            string str = "No executable has been found at path '" + filePath + "'.";
            write(ERRFD, str.c_str(), str.length());
            write(ERRFD, "\n", 1);
            return 0;
        }

        filePath = newFilePath;
    }

    // Execute process
    return YShellVisitor::exec(filePath, arguments, INFD, OUTFD, ERRFD);
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
    int PID = (int) visit(ctx->command()).as<int>();

    // Close output stream on this side
    close(fdo);

    // Reset output stream back to STDOUT
    OUTFD = fdBackup;

    // Return the PID
    return PID;
}

antlrcpp::Any YShellVisitor::visitFileToSTDIN(ShellGrammarParser::FileToSTDINContext *ctx) {
    // Get file name
    string file = (string) visit(ctx->filePath()).as<string>();

    // Open file descriptor for file
    int fdi = open(file.c_str(), O_RDONLY);

    // Check if file exists
    if (fdi < 0) {
        string str = "File '" + file + "' does not exist!";
        write(ERRFD, str.c_str(), str.length());
        write(ERRFD, "\n", 1);
        return (int) 1;
    }

    // Set input stream to file
    int fdBackup = INFD;
    INFD = fdi;

    // Run process
    int PID = (int) visit(ctx->command()).as<int>();

    // Close input stream on this side
    close(fdi);

    // Reset input stream
    INFD = fdBackup;

    // Return the PID
    return PID;
}

antlrcpp::Any YShellVisitor::visitSTDERRToFile(ShellGrammarParser::STDERRToFileContext *ctx) {
    // Get file name
    string file = (string) visit(ctx->filePath()).as<string>();

    // Open file descriptor for file
    int fde = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);

    // Set error stream to file
    int fdBackup = ERRFD;
    ERRFD = fde;

    // Run process
    int PID = (int) visit(ctx->command()).as<int>();

    // Close error stream on this side
    close(fde);

    // Reset error stream back to STDOUT
    ERRFD = fdBackup;

    // Return the PID
    return PID;
}

antlrcpp::Any YShellVisitor::visitSTDOUTAppendToFile(ShellGrammarParser::STDOUTAppendToFileContext *ctx) {
    // Get file name
    string file = (string) visit(ctx->filePath()).as<string>();

    // Open file descriptor for file
    int fdo = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);

    // Set output stream to file
    int fdBackup = OUTFD;
    OUTFD = fdo;

    // Run process
    int PID = (int) visit(ctx->command()).as<int>();

    // Close output stream on this side
    close(fdo);

    // Reset output stream back to STDOUT
    OUTFD = fdBackup;

    // Return the PID
    return PID;
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
    int PID = (int) visit(ctx->command()[0]).as<int>();

    //Await first command and get its status
    int status = 0;
    if (PID > 0) waitpid(PID, &status, 0);

    //Only execute second command if the first executed successfully
    if (status == 0) PID = (int) visit(ctx->command()[1]).as<int>();

    //Await second command and return its status
    if (PID > 0) waitpid(PID, &status, 0);
    return status;
}

antlrcpp::Any YShellVisitor::visitOrCommand(ShellGrammarParser::OrCommandContext *ctx) {
    //Execute first command
    int PID = (int) visit(ctx->command()[0]).as<int>();

    //Await first command and get its status
    int status = 0;
    if (PID > 0) waitpid(PID, &status, 0);

    //Only execute second command if the first executed successfully
    if (status != 0) PID = (int) visit(ctx->command()[1]).as<int>();

    //Await second command and return its status
    status = 0;
    if (PID > 0) waitpid(PID, &status, 0);
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
    visit(ctx->command()[0]);

    //Reset output stream
    OUTFD = fdBackup;

    //Redirect pip output to process input
    fdBackup = INFD;
    INFD = p[0];

    //Execute second process
    int PID = (int) visit(ctx->command()[1]).as<int>();

    //Reset input stream
    INFD = fdBackup;

    //Close pipes if they have not yet
    close(p[0]);
    close(p[1]);

    //return PID of second process
    return PID;
}

antlrcpp::Any YShellVisitor::visitBGCommand(ShellGrammarParser::BGCommandContext *ctx) {
    // Open file descriptors for /dev/null
    int fde = open("/dev/null", O_RDONLY);
    int fdo = open("/dev/null", O_RDONLY);
    int fdi = open("/dev/null", O_WRONLY);

    // Check if /dev/null exists
    if (fdo < 0 || fdi < 0 || fde < 0) {
        string str = "UNSUPPORTED SYSTEM! /dev/null IS NOT AVAILABLE.";
        write(ERRFD, str.c_str(), str.length());
        write(ERRFD, "\n", 1);
        return (int) 1;
    }

    // Route process IO from/to /dev/null
    int outFdBackup = OUTFD;
    int errFdBackup = ERRFD;
    int inFdBackup = INFD;
    OUTFD = fde;
    ERRFD = fdo;
    INFD = fdi;

    // Run process
    int PID = (int) visit(ctx->command()).as<int>();

    // Close file descriptors to /dev/null
    close(fdo);
    close(fdi);
    close(fde);

    // Restore STDOUT STDIN and STDERR
    OUTFD = outFdBackup;
    ERRFD = errFdBackup;
    INFD = inFdBackup;

    string str = "Started process with PID (" + to_string(PID) + ") in background.";
    write(OUTFD, str.c_str(), str.length());
    write(OUTFD, "\n", 1);
    return (int) 0;
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

int YShellVisitor::exec(string file, vector<string> args, int input, int output, int error) {

    //Fork process
    int fid = fork();

    if (fid == 0) {
        //Set output, error and input streams
        if (input != 0) {
            dup2(input, 0);
            close(input);
        }
        if (output != 1) {
            dup2(output, 1);
            close(output);
        }
        if (error != 2) {
            dup2(error, 2);
            close(error);
        }

        // Add executable name as first argument
        vector<string> pathSplit = Utils::split(file.c_str(), '/');
        args.insert(args.begin(), pathSplit[pathSplit.size() - 1]);

        // Convert args to c style
        char **_args = (char **) calloc(args.size() + 1, sizeof(string));
        for (vector<string>::size_type i = 0; i != args.size(); i++)
            _args[i] = (char *) args[i].c_str();
        _args[args.size()] = (char *) NULL;

        //Execute program
        execv(file.c_str(), _args);
        //Exit process
        exit(0);
    }

    //Close useless pipe ends
    if (input != 0) close(input);
    if (output != 1) close(output);
    if (error != 2) close(error);

    //Return pid
    return fid;
}