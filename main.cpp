#include <iostream>
#include <vector>
#include <string>

#include "Common.h"
#include "Lexer.h"
#include "Parser.h"

std::string lit2python(const Token& tk){
    std::string ret = "";
    switch(tk.type){
        case TK_LIT_STR:
            ret += "\"";
            ret += tk.value;
            ret += "\"";
            break;
        case TK_LIT_CHAR:
            ret += "\'";
            ret += tk.value;
            ret += "\'";
            break;
        case TK_LIT_INT:
                ret += "int(";
                ret += tk.value;
                ret += ")";
                break;
            case TK_LIT_FLOAT:
                ret += "float(";
                ret += tk.value;
                ret += ")";
                break;
        default:
            ret += tk.value;
            break;
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

void emitPyExpression(std::shared_ptr<Expression> expr, int indent = 0) {
    std::string ind(indent, ' ');

    if (auto lit = std::dynamic_pointer_cast<LitIdentExpression>(expr)) {
        std::cout << ind << lit2python(lit->lit_ident);
    } else if (auto unary = std::dynamic_pointer_cast<UnaryOp>(expr)) {
        if(unary->type == Paren){
            std::cout << "(";
            emitPyExpression(unary->expr);
            std::cout << ")";
        } else
            LogError("Not Implemented!")
    } else if (auto bin = std::dynamic_pointer_cast<BinOp>(expr)) {
        emitPyExpression(bin->lhs);
        switch (bin->type)
        {
        case Add:
            std::cout << ind << " + ";
            break;

        case Sub:
            std::cout << ind << " - ";
            break;

        case Mul:
            std::cout << ind << " * ";
            break;

        case Div:
            std::cout << ind << " / ";
            break;

        case And:
            std::cout << ind << " and ";
            break;

        case Or:
            std::cout << ind << " or ";
            break;

        case Eq:
            std::cout << ind << " == ";
            break;

        case Gt:
            std::cout << ind << " > ";
            break;

        case Lt:
            std::cout << ind << " < ";
            break;

        case GtEq:
            std::cout << ind << " >= ";
            break;

        case LtEq:
            std::cout << ind << " <= ";
            break;
        
        default:
            LogError("Invalid BinOp");
            break;
        }
        emitPyExpression(bin->rhs);
    }
}

void emitPyStatement(std::shared_ptr<Statement> stmt, int indent = 0) {
    std::string ind(indent, ' ');

    if (auto funcCall = std::dynamic_pointer_cast<FuncCallStmt>(stmt)) {
        std::cout << ind << funcCall->name.value << "(";
        for (size_t i = 0; i < funcCall->args.size(); i++)
        {
            std::cout << lit2python(funcCall->args[i]);
            if(i != funcCall->args.size()-1){
                std::cout << ", ";
            }
        }
        std::cout << ")\n";
    } else if (auto ret = std::dynamic_pointer_cast<RetStmt>(stmt)) {
        std::cout << ind << "return ";
        emitPyExpression(ret->expr);
        std::cout << "\n";
    } else if (auto decl = std::dynamic_pointer_cast<VarDeclStmt>(stmt)) {
        std::cout << ind << decl->name << " = ";
        emitPyExpression(decl->expr);
        std::cout << "\n";
    } else if (auto func = std::dynamic_pointer_cast<Func>(stmt)) {
        std::cout << ind << "def " << func->name.value << "(\n";
        for (size_t i = 0; i < func->args.size(); i++)
        {
            auto arg = func->args[i];
            std::cout << arg.name.value << ": " << arg.type.value;
            if(i != func->args.size()-1){
                std::cout << ", ";
            }
        }
        std::cout << "):\n";
        for (auto& bodyStmt : func->body) {
            emitPyStatement(bodyStmt, indent + 2);
        }
        std::cout << "\n";
    } else {
        std::cout << ind << "Unknown Statement\n";
    }
}

void emitPyFunc(const Func& func){
    std::cout << "def " << func.name.value << "(";
    for (size_t i = 0; i < func.args.size(); i++)
        {
            auto arg = func.args[i];
            std::cout << arg.name.value << ": " << arg.type.value;
            if(i != func.args.size()-1){
                std::cout << ", ";
            }
        }
    std::cout << "):\n";
    for (auto& bodyStmt : func.body) {
        emitPyStatement(bodyStmt, 2);
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
    std::vector<Func> code;
    int ret = parse_function(lexer, token, func);
    while(ret == 1){
        code.push_back(func);
        //GeneratePy(func);
        ret = parse_function(lexer, token, func);
    }

    for(auto& func: code)
        emitPyFunc(func);

    std::cerr << "DEBUG - AST:\n";
    for(auto& func: code){
        std::cerr << "  type: Func\n";
        std::cerr << "  name: " << func.name.value << "\n";
        std::cerr << "  args: \n";
        for (size_t i = 0; i < func.args.size(); i++)
        {
            std::cerr << "    " << i << ":\n";
            auto arg = func.args[i];
            std::cerr << "      type: " << arg.type.value << "\n";
            std::cerr << "      name: " << arg.name.value << "\n";
        }
        std::cerr << "  body:\n";
        for (auto& bodyStmt : func.body) {
            printStatement(bodyStmt, 4);
        }
    }

    if(ret != -1){
        std::cerr << "Done. Compilation was sucessfull! return 0\n";
    }else{
        terminateCompilation(ret);
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