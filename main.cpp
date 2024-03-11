#include <iostream>
#include <vector>
#include <string>

#include "Common.h"
#include "Lexer.h"
#include "Parser.h"

std::string lit2python(const Token& tk){
    std::string ret = "";
    if(tk.type == TK_LIT_STR){
        ret += "\"";
        ret += tk.value;
        ret += "\"";
    }
    else if(tk.type == TK_LIT_CHAR){
        ret += "\'";
        ret += tk.value;
        ret += "\'";
    }
    else{
        ret += tk.value;
    }
    return ret;
}

std::string printf2python(const std::vector<Token>& args){
    std::string ret = "";
    bool has_fmt = false;
    for (size_t i = 0; i < args.size(); i++)
    {
        if(i == 1){
            has_fmt = true;
            ret += " % (";
        }
        ret += lit2python(args[i]);
        if(i != args.size()-1 && i > 0){
            ret += ",";
        }else if(has_fmt){
            ret += ")";
        }
    }
    return ret;
}

void GeneratePy(const Func& func){
    std::cout << "def " << func.name.value << "(" << "" << "):\n";
    for (const auto& stmt : func.body)
    {
        if (auto* type = dynamic_cast<RetStmt*>(stmt.get())) {
            LogDebug(typeid(*type).name() << " " << type->expr);
            std::cout << "  return " << type->expr << "\n";
        }
        else if (auto* type = dynamic_cast<FuncCallStmt*>(stmt.get())) {
            if(type->name.value == "printf"){
                std::cout << "  print(";
                std::cout << printf2python(type->args);
                std::cout << ")\n";
            }
            else{
                LogError(type->name.loc.display() << ": unknowm function " << type->name.value);
            }
        }
        else if (auto* type = dynamic_cast<VarDeclStmt*>(stmt.get())) {
            LogDebug(typeid(*type).name() << " " << type->type << " " << type->name << " " << type->value.value);
            std::cout << "  " << type->name << " = " << type->type << "(" << lit2python(type->value) << ")\n";
        }
    }
    std::cout << "\n";
}

void Compile(const std::string& InputFileName, const std::string& OutpuFileName){
    std::ifstream inFile(InputFileName);
    if(!inFile.good()){
        LogError("Unable to open file \'" << InputFileName << "\'");
        exit(-1);
    }
    std::string contents;
    while(inFile){
        contents += inFile.get();
    }
    
    Lexer lexer(contents, InputFileName);
    Token token;
    Func func;
    bool ret = parse_function(lexer, token, func);
    while(ret){
        GeneratePy(func);
        ret = parse_function(lexer, token, func);
    }
}

void printUsage(const std::vector<std::string>& args){
    std::string cmdLine = "";
    std::string inputFileName = "";
    std::string outputFileName = "output.asm";

    std::cerr << "Welcome to FUKompiler!\n";
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
    LogDebug("\nFUKargs:\n  CmdLine: " << cmdLine << "\n  InputFile: " << inputFileName << "\n  OutputFile: " << outputFileName);
    
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