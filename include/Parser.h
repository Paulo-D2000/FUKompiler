#pragma once

#include "Common.h"
#include "Lexer.h"
#include <stdarg.h>

bool enable_read = true;
bool expect_token(Lexer& lexer, Token& token, const TokenType& type){
    if(enable_read){
        if(!lexer.GetNextToken(token)){
            LogError(token.loc.display() << ": Expected " << TokenTypeNames[token.type] << " but got end of file.");
            return false;
        }
    }
    
    if(token.type != type){
        return false;
    }

    enable_read = true;
    return true;
}

template<typename... Types>
bool expect_token(Lexer& lexer, Token& token, const TokenType& first, const Types&... rest) {
    if (expect_token(lexer, token, first)) {
        return true;
    } else {
        enable_read = false;
        return expect_token(lexer, token, rest...);
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

bool parse_arglist(Lexer& lexer, std::vector<Token>& arglist){
    Token token, expr;
    bool end = false;
    while(!end){
        if(!expect_token(lexer, expr, TK_LIT_STR, TK_LIT_INT, TK_LIT_CHAR)) return false;
        if(!expect_token(lexer, token, TK_CPAREN, TK_COMMA)) return false;
        if(token.type == TK_CPAREN) end = true;
        arglist.push_back(expr);
    }
    return true;
}

bool parse_expression(Lexer& lexer, Expression& expr){
    Token token;
    if(!expect_token(lexer, token, TK_LIT_INT, TK_NAME)) return false;
    if(token.type == TK_NAME){
        // FUNC OR VAR
        // update expr
        LogError(token.loc.display() << ": Parsing of TK_NAME " << token.value << " is not implemented yet!")
    }
    expr.lit = token;
    return true;
}

bool parse_block(Lexer& lexer, std::vector<std::shared_ptr<Statement>>& block){
    Token token;
    if(!expect_token(lexer, token, TK_OCURLY)) return false;

    Token name, expr, value;
    Expression ret_expr;
    std::vector<Token> arglist;
    while(1){
        if(!expect_token(lexer, name, TK_NAME, TK_CCURLY)) return false;
        if(name.type == TK_CCURLY) break;

        if(name.value == "return"){
            //if(!expect_token(lexer, expr, TK_LIT_INT, TK_NAME)) return false;
            //if(expr.type == TK_NAME){
            //    // FUNC OR VAR
            //    // update expr
            //    LogError(expr.loc.display() << ": TK_NAME " << expr.value << " is unknown")
            //}
            if(!parse_expression(lexer, ret_expr)) return false;
            std::shared_ptr<RetStmt> retStmt = std::make_shared<RetStmt>();
            retStmt->expr = ret_expr;
            block.push_back(std::move(retStmt));
        }
        else{
            if(!expect_token(lexer, expr, TK_OPAREN, TK_NAME)) return false;

            if(expr.type == TK_OPAREN){
                // funccall
                if(!parse_arglist(lexer, arglist)) return false;
                std::shared_ptr<FuncCallStmt> funcCallStmt = std::make_shared<FuncCallStmt>();
                funcCallStmt->name = name;
                funcCallStmt->args = std::move(arglist);
                block.push_back(std::move(funcCallStmt));
            }
            else if(expr.type == TK_NAME){
                // variable
                if(!expect_token(lexer, token, TK_EQUAL)) return false;
                if(!expect_token(lexer, value, TK_LIT_CHAR, TK_LIT_INT, TK_LIT_STR)) return false;
                switch (value.type)
                {
                case TK_LIT_CHAR:
                    if(name.value.compare("char") != 0){
                        return false;
                    }
                    break;

                case TK_LIT_INT:
                    if(name.value.compare("int") != 0){
                        return false;
                    }
                    break;

                case TK_LIT_STR:
                    LogError(token.loc.display() << ": String literal variable not implemented!");
                    return false;//name.value.compare("char*") == 0 || name.value.compare("std::string") == 0;
                
                default:
                    return false;
                }
                std::shared_ptr<VarDeclStmt> varStmt = std::make_shared<VarDeclStmt>();
                varStmt->type = name.value;
                varStmt->name = expr.value;
                varStmt->value = value;
                block.push_back(std::move(varStmt));
            }
        }

        if(!expect_token(lexer, token, TK_SEMICOL)){
            LogError(token.loc.display() << ": Expected ; here but got " << token.value);
            return false;
        }
    }
    return true;
}

int parse_function(Lexer& lexer, Token& token, Func& func){
    if(lexer.isEOF()){
        return 0;
    }
    if(!parse_type(lexer, token)) return -1;

    Token name;
    if(!expect_token(lexer,  name, TK_NAME)) return -1;

    if(!expect_token(lexer, token, TK_OPAREN)) return -1;
    if(!expect_token(lexer, token, TK_CPAREN)) return -1;

    std::vector<std::shared_ptr<Statement>> body;
    if(!parse_block(lexer, body)) return -1;

    func.name = name;
    func.body = body;
    return true;
}