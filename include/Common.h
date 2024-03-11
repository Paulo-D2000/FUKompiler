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
    TK_LIT_INT,
    TK_LIT_STR,
    TK_RETURN
};

static const std::string TokenTypeNames[] = {
    "TK_NAME",
    "TK_OPAREN",
    "TK_CPAREN",
    "TK_OCURLY",
    "TK_CCURLY",
    "TK_SEMICOL",
    "TK_LIT_INT",
    "TK_LIT_STR",
    "TK_RETURN"
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
    std::string name;
    std::vector<std::string> args;
};

struct RetStmt: Statement
{
    std::string expr;
};

struct Func: Statement
{
    Token name;
    std::vector<std::shared_ptr<Statement>> body;
};

inline std::string args2str(std::vector<std::string> args){
    std::string ret = "";
    for (auto &str : args)
    {
        ret += str;
        ret += ", ";
    }
    return ret;
}