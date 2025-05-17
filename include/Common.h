#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <memory>

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
    TK_LIT_FLOAT,
    TK_LIT_STR,
    TK_LIT_CHAR,
    TK_RETURN,
    TK_EQUAL,
    TK_EQUAL_EQUAL,
    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,
    TK_PERCENT,
    TK_AND,
    TK_AND_AND,
    TK_OR,
    TK_OR_OR,
    TK_PLUS_PLUS,
    TK_MINUS_MINUS,
    TK_GREATER,
    TK_GREATER_EQUAL,
    TK_LESS,
    TK_LESS_EQUAL
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
    "TK_LIT_FLOAT",
    "TK_LIT_STR",
    "TK_LIT_CHAR",
    "TK_RETURN",
    "TK_EQUAL",
    "TK_EQUAL_EQUAL",
    "TK_PLUS",
    "TK_MINUS",
    "TK_STAR",
    "TK_SLASH",
    "TK_PERCENT",
    "TK_AND",
    "TK_AND_AND",
    "TK_OR",
    "TK_OR_OR",
    "TK_PLUS_PLUS",
    "TK_MINUS_MINUS",
    "TK_GREATER",
    "TK_GREATER_EQUAL",
    "TK_LESS",
    "TK_LESS_EQUAL"
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

struct ArgDecl{
    Token type;
    Token name;
};

struct Expression{
    virtual ~Expression() {}
};

struct LitIdentExpression: Expression{
    Token lit_ident;
};

enum UnaryOpType {Paren=0};
static const std::string UnaryOpTypeNames[] = {
    "UnaryOpParen"
    };

struct UnaryOp: Expression{
    UnaryOpType type;
    std::shared_ptr<Expression> expr;
};

enum BinOpType {Add=0, Sub, Mul, Div, And, Or, Gt, Lt, GtEq, LtEq, Eq};
static const std::string BinOpTypeNames[] = {
    "BinOpAdd", "BinOpSub", "BinOpMul", "BinOpDiv",
    "BinOpAnd", "BinOpOr",
    "BinOpGt", "BinOpLt", "BinOpGtEq", "BinOpLtEq", "BinOpEq"
    };

struct BinOp: Expression{
    BinOpType type;
    std::shared_ptr<Expression> lhs, rhs;
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
    std::shared_ptr<Expression> expr;
};

struct VarDeclStmt: Statement
{
    std::string name;
    std::shared_ptr<Expression> expr;
};

struct Func: Statement
{
    Token name;
    std::vector<ArgDecl> args;
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

inline void printExpression(std::shared_ptr<Expression> expr, int indent = 0) {
    std::string ind(indent, ' ');
    std::string ind2(indent + 2, ' ');

    if (auto lit = std::dynamic_pointer_cast<LitIdentExpression>(expr)) {
        std::cerr << ind << "type: LitIdentExpression\n";
        std::cerr << ind << "value: \"" << lit->lit_ident.value << "\"\n";
    } else if (auto unary = std::dynamic_pointer_cast<UnaryOp>(expr)) {
        std::cerr << ind << "type: " << UnaryOpTypeNames[unary->type] << "\n";
        std::cerr << ind << "lhs:\n";
        printExpression(unary->expr, indent + 2);
    } else if (auto bin = std::dynamic_pointer_cast<BinOp>(expr)) {
        std::cerr << ind << "type: " << BinOpTypeNames[bin->type] << "\n";
        std::cerr << ind << "lhs:\n";
        printExpression(bin->lhs, indent + 2);
        std::cerr << ind << "rhs:\n";
        printExpression(bin->rhs, indent + 2);
    } else {
        std::cerr << ind << "Unknown Expression\n";
    }
}

inline void printStatement(std::shared_ptr<Statement> stmt, int indent = 0) {
    std::string ind(indent, ' ');
    std::string ind2(indent + 2, ' ');

    if (auto funcCall = std::dynamic_pointer_cast<FuncCallStmt>(stmt)) {
        std::cerr << ind << "type: FuncCallStmt\n";
        std::cerr << ind << "name: " << funcCall->name.value << "\n";
        std::cerr << ind << "args:\n";
        for (auto& arg : funcCall->args) {
            std::cerr << ind << "- " << arg.value << "\n";
        }
    } else if (auto ret = std::dynamic_pointer_cast<RetStmt>(stmt)) {
        std::cerr << ind << "type: RetStmt\n";
        std::cerr << ind << "expr:\n";
        printExpression(ret->expr, indent + 2);
    } else if (auto decl = std::dynamic_pointer_cast<VarDeclStmt>(stmt)) {
        std::cerr << ind << "type: VarDeclStmt\n";
        std::cerr << ind << "name: " << decl->name << "\n";
        std::cerr << ind << "expr:\n";
        printExpression(decl->expr, indent + 2);
    } else if (auto func = std::dynamic_pointer_cast<Func>(stmt)) {
        std::cerr << ind << "type: Func\n";
        std::cerr << ind << "name: " << func->name.value << "\n";
        std::cerr << ind << "args: \n";
        for (size_t i = 0; i < func->args.size(); i++)
        {
            std::cerr << ind2 << i << ":\n";
            auto arg = func->args[i];
            std::cerr << ind2 << ind2 << "type: " << arg.type.value << "\n";
            std::cerr << ind2 << ind2 << "name: " << arg.name.value << "\n";
        }
        std::cerr << ind << "body:\n";
        for (auto& bodyStmt : func->body) {
            printStatement(bodyStmt, indent + 2);
        }
    } else {
        std::cerr << ind << "Unknown Statement\n";
    }
}

inline void terminateCompilation(int ret){
    LogError("Compliation returned error " << ret);
    exit(ret);
}