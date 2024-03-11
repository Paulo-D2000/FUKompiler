#pragma once

#include <iostream>
#include <string>
#include <sstream>

#define LogError(err) std::cerr << "ERROR - " << __FUNCTION__ << "() " << err << "\n";
#define LogDebug(dbg) std::cerr << "DEBUG - " << __FUNCTION__ << "() " << dbg << "\n";

enum TokenType{
    TK_NAME=0,
    TK_OPAREN,
    TK_CPAREN,
    TK_OCURLY,
    TK_CCURLY,
    TK_SEMICOL,
    TK_COMMA,
    TK_LIT_INT,
    TK_LIT_STR,
    TK_LIT_CHAR,
    TK_RETURN,
    TK_EQUAL,
};

static const std::string TokenTypeNames[] = {
    "TK_NAME",
    "TK_OPAREN",
    "TK_CPAREN",
    "TK_OCURLY",
    "TK_CCURLY",
    "TK_SEMICOL",
    "TK_COMMA",
    "TK_LIT_INT",
    "TK_LIT_STR",
    "TK_LIT_CHAR",
    "TK_RETURN",
    "TK_EQUAL"
};

struct Location{
    std::string path;
    size_t row, col;

    inline std::string display(){
        std::stringstream ss;
        ss << path << ":" << row + 1 << ":" << col + 1;
        return ss.str();
    }
};

struct Token{
    TokenType type;
    std::string value;
    Location loc;
};

struct Statement {
    virtual ~Statement() {}
};

struct FuncCallStmt: Statement
{
    Token name;
    std::vector<Token> args;
};

struct RetStmt: Statement
{
    std::string expr;
};

struct VarDeclStmt: Statement
{
    std::string type;
    std::string name;
    Token value;
};

struct Func: Statement
{
    Token name;
    std::vector<std::shared_ptr<Statement>> body;
};


inline std::string args2str(std::vector<Token> args){
    std::string ret = "";
    for (size_t i = 0; i < args.size(); i++)
    {
        ret += args[i].value;
        if(i != args.size()-1){
            ret += ", ";
        }
    }
    return ret;
}