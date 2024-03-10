#include <iostream>
#include <vector>
#include <string>

#include "Common.h"
#include "Lexer.h"

void Compile(const std::string& InputFileName, const std::string& OutpuFileName){
    std::ifstream inFile(InputFileName);
    if(!inFile.good()){
        LogError("Unable to open file \'" << InputFileName << "\'");
    }
    std::string contents;
    while(inFile){
        contents += inFile.get();
    }
    
    Lexer lexer(contents, InputFileName);
    Token token;
    bool ret = lexer.GetNextToken(token);
    while(ret){
        ret = lexer.GetNextToken(token);
    }
}

void printUsage(const std::vector<std::string>& args){
    std::string cmdLine = "";
    std::string inputFileName = "";
    std::string outputFileName = "output.asm";

    std::cout << "Welcome to FUKompiler!\n";
    if(args.size() < 2){
        std::cerr << "Usage: FUK input.c [output.asm]\n";
        exit(-1);
    }

    // Get program args
    cmdLine = args[0];
    inputFileName = args[1];
    
    // Get output fname
    if(args.size() == 3){
        outputFileName = args[2];
    }

    // DEBUG
    LogDebug("FUKargs:\n  CmdLine: " << cmdLine << "\n  InputFile: " << inputFileName << "\n  OutputFile: " << outputFileName);
    
    // Compile
    Compile(inputFileName, outputFileName);
}

int main(int argc, char** argv){
    std::vector<std::string> args;
    for (size_t i = 0; i < argc; i++)
        args.push_back(std::string(argv[i]));

    printUsage(args);
    return 0;
}