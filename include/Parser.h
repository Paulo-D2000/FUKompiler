#pragma once

#include "Common.h"
#include "Lexer.h"
#include <stdarg.h>

bool expect_token(Lexer& lexer, Token& token, const TokenType& type, bool tread = true){
    if(tread){
        if(!lexer.GetNextToken(token)){
            LogError(token.loc.display() << ": Expected " << TokenTypeNames[token.type] << " but got end of file.");
            return false;
        }
    }
    
    if(token.type != type){
        return false;
    }

    return true;
}

template<typename... Types>
bool expect_token(Lexer& lexer, Token& token, const TokenType& first, const Types&... rest) {
    if (expect_token(lexer, token, first)) {
        return true;
    } else {
        return expect_token(lexer, token, rest..., false);
    }
    LogError(token.loc.display() << ": Expected " << TokenTypeNames[first] << " but got " << TokenTypeNames[token.type]);
    return false;
}

bool parse_type(Lexer& lexer, Token& token){
    bool ret = expect_token(lexer, token, TK_NAME);
    if(token.value != "int"){
        LogError(token.loc.display() << ": Unexpected type " << token.value);
        return false;
    }
    return ret;
}

bool parse_arglist(Lexer& lexer, std::vector<std::string>& arglist){
    Token token, expr;
    if(!expect_token(lexer, token, TK_OPAREN)) return false;
    while(1){
        if(!expect_token(lexer, expr, TK_LIT_STR, TK_LIT_INT)) return false;
        if(!expect_token(lexer, token, TK_CPAREN)) return false;
        arglist.push_back(expr.value);
        if(token.type == TK_CPAREN) break;
    }
    return true;
}

bool parse_block(Lexer& lexer, std::vector<std::shared_ptr<Statement>>& block){
    Token token;
    if(!expect_token(lexer, token, TK_OCURLY)) return false;

    Token name, expr;
    std::vector<std::string> arglist;
    while(1){
        if(!expect_token(lexer, name, TK_NAME, TK_CCURLY)) return false;
        if(name.type == TK_CCURLY) break;

        if(name.value == "return"){
            if(!expect_token(lexer, expr, TK_LIT_INT)) return false;
            std::shared_ptr<RetStmt> retStmt = std::make_shared<RetStmt>();
            retStmt->expr = expr.value;
            block.push_back(std::move(retStmt));
        }else{
            if(!parse_arglist(lexer, arglist)) return false;
            std::shared_ptr<FuncCallStmt> funcCallStmt = std::make_shared<FuncCallStmt>();
            funcCallStmt->name = name.value;
            funcCallStmt->args = std::move(arglist);
            block.push_back(std::move(funcCallStmt));
        }

        if(!expect_token(lexer, token, TK_SEMICOL)) return false;
    }
    return true;
}

bool parse_function(Lexer& lexer, Token& token, Func& func){
    if(!parse_type(lexer, token)) return false;

    Token name;
    if(!expect_token(lexer,  name, TK_NAME)) return false;

    if(!expect_token(lexer, token, TK_OPAREN)) return false;
    if(!expect_token(lexer, token, TK_CPAREN)) return false;

    std::vector<std::shared_ptr<Statement>> body;
    if(!parse_block(lexer, body)) return false;

    func.name = name;
    func.body = body;
    return true;
}