#pragma once

#include "Common.h"
#include "Lexer.h"
#include <stdarg.h>

bool enable_read = true;
bool expect_token(Lexer& lexer, Token& token, const TokenType& type){
    if(enable_read){
        if(!lexer.GetNextToken(token)){
            LogError("Expected " << TokenTypeNames[type] << " but got end of file or invalid token.");
            terminateCompilation(-1);
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
        bool ret = expect_token(lexer, token, rest...);
        if (!ret)
            LogError(token.loc.display() << ": Expected " << TokenTypeNames[first] << " but got " << TokenTypeNames[token.type]);
        return ret;
    }
}

bool parse_type(Lexer& lexer, Token& token){
    bool ret = expect_token(lexer, token, TK_NAME);

    if(token.value == std::string("int")){
        return ret;
    }
    else if (token.value == std::string("double")){
        return ret;
    }
    else if (token.value == std::string("float")){
        return ret;
    }
    else if (token.value == std::string("char")){
        return ret;
    }
    else if (token.value == std::string("void")){
        return ret;
    }
    else
    {
        LogError(token.loc.display() << ": Unexpected type " << token.value);
        return false;
    }
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

bool parse_expression_lit(Lexer& lexer, std::shared_ptr<Expression>& expr){
    Token token;
    if(!expect_token(lexer, token, TK_LIT_CHAR, TK_LIT_INT, TK_LIT_FLOAT, TK_LIT_STR, TK_NAME)) return false;
    auto lit_expr = std::make_shared<LitIdentExpression>();
    lit_expr->lit_ident = token;
    expr = std::move(lit_expr);
    return true;
}
bool parse_expression(Lexer& lexer, std::shared_ptr<Expression>& expr);


bool parse_expression_unary(Lexer& lexer, std::shared_ptr<Expression>& expr){
    Token token;
    if(!expect_token(lexer, token, TK_OPAREN, TK_CPAREN, TK_LIT_CHAR, TK_LIT_INT, TK_LIT_FLOAT, TK_LIT_STR, TK_NAME)) return false;
    if(token.type == TK_OPAREN){
        std::shared_ptr<Expression> expr_start;
        if(!parse_expression(lexer, expr_start)) return false;
        auto unary_expr = std::make_shared<UnaryOp>();
        unary_expr->type = Paren;
        unary_expr->expr = std::move(expr_start);
        expr = std::move(unary_expr);
        return true;
    }
    else if(token.type == TK_CPAREN){
        LogError("Parethesis closed too son...")
        return false;
    }
    else {
        auto lit_expr = std::make_shared<LitIdentExpression>();
        lit_expr->lit_ident = token;
        expr = std::move(lit_expr);
        return true;
    }
}

bool parse_expression(Lexer& lexer, std::shared_ptr<Expression>& expr){
    Token token;
    std::shared_ptr<Expression> expr_start;

    // Begin Paren or Lit Parsing
    if(!parse_expression_unary(lexer, expr_start)) return false;

    // Begin Lit Parsing
    //if(!parse_expression_lit(lexer, expr_start)) return false;

    // Fisish Lit or Begin Binary exression
    if(!expect_token(lexer, token, TK_SEMICOL,
        TK_CPAREN,
        TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH,
        TK_AND, TK_OR, TK_AND_AND, TK_OR_OR,
        TK_GREATER, TK_LESS, TK_GREATER_EQUAL, TK_LESS_EQUAL, TK_EQUAL_EQUAL
    )) return false;

    // normal expr (lit | var_name)
    if(token.type == TK_SEMICOL){
        LogDebug("Lit | Varname");
        expr = std::move(expr_start);
        return true;
    } // unary_paren
    else if(token.type == TK_CPAREN){
        LogDebug("Closing Paren...");
        expr = std::move(expr_start);
        return true;
    } // add
    else if(token.type == TK_PLUS){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Add;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// sub
    else if(token.type == TK_MINUS){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Sub;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// mul
    else if(token.type == TK_STAR){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Mul;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// div
    else if(token.type == TK_SLASH){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Div;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// and
    else if(token.type == TK_AND || token.type == TK_AND_AND){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = And;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// or
    else if(token.type == TK_OR || token.type == TK_OR_OR){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Or;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// gt
    else if(token.type == TK_GREATER){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Gt;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }
    // lt
    else if(token.type == TK_LESS){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Lt;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// gteq
    else if(token.type == TK_GREATER_EQUAL){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = GtEq;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }// lteq
    else if(token.type == TK_LESS_EQUAL){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = LtEq;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }
    // eq
    else if(token.type == TK_EQUAL_EQUAL){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Eq;
        LogDebug(BinOpTypeNames[addexpr->type]);
        addexpr->lhs = std::move(expr_start);
        if(!parse_expression(lexer, addexpr->rhs)) return false;
        expr = std::move(addexpr);
        return true;
    }
    LogError(token.loc.display() << ": Error " << token.value);
    return false;
}

bool parse_block(Lexer& lexer, std::vector<std::shared_ptr<Statement>>& block){
    Token token;
    if(!expect_token(lexer, token, TK_OCURLY)) return false;

    Token name, expr, value;
    std::shared_ptr<Expression> parse_expr;
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
            if(!parse_expression(lexer, parse_expr)) return false;
            std::shared_ptr<RetStmt> retStmt = std::make_shared<RetStmt>();
            retStmt->expr = std::move(parse_expr);
            block.push_back(std::move(retStmt));
            continue;
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
                if(!expect_token(lexer, token, TK_EQUAL)){
                    LogError(token.loc.display() << ": Expected = here but got " << token.value);
                    return false;
                }
                //if(!expect_token(lexer, value, TK_LIT_CHAR, TK_LIT_INT, TK_LIT_STR)){
                //    LogError(value.loc.display() << ": Expected literal here but got " << TokenTypeNames[value.type]);
                //    return false;
                //}
                //switch (value.type)
                //{
                //case TK_LIT_CHAR:
                //    if(name.value.compare("char") != 0){
                //        return false;
                //    }
                //    break;
//
                //case TK_LIT_INT:
                //    if(name.value.compare("int") != 0){
                //        return false;
                //    }
                //    break;
//
                //case TK_LIT_STR:
                //    LogError(token.loc.display() << ": String literal variable not implemented!");
                //    return false;//name.value.compare("char*") == 0 || name.value.compare("std::string") == 0;
                //
                //default:
                //    return false;
                //}
                if(!parse_expression(lexer, parse_expr)) return false;
                std::shared_ptr<VarDeclStmt> varStmt = std::make_shared<VarDeclStmt>();
                varStmt->name = expr.value;
                varStmt->expr = parse_expr;
                block.push_back(std::move(varStmt));
                continue;
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
    // Trim left
    if(!lexer.Preprocessor()) return false;

    // Return type
    if(!parse_type(lexer, token)) return -1;

    // Func name
    Token name;
    if(!expect_token(lexer,  name, TK_NAME)) return -1;

    // Args
    std::vector<ArgDecl> args;
    if(!expect_token(lexer, token, TK_OPAREN)) return -1;
    while(1){
        if(!expect_token(lexer, token, TK_NAME, TK_COMMA, TK_CPAREN)) return -1;
        if(token.type == TK_CPAREN)
            break;
        else if(token.type == TK_NAME){
            ArgDecl arg;
            arg.type = token;
            if(!expect_token(lexer, token, TK_NAME)) return -1;
            arg.name = token;
            args.push_back(arg);
        }
        else if(token.type == TK_COMMA){
            continue;
        }
    }

    // body
    std::vector<std::shared_ptr<Statement>> body;
    if(!parse_block(lexer, body)) return -1;
    
    func.name = name;
    func.args = args;
    func.body = body;
    return true;
}