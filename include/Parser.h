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
        TK_AND, TK_OR, TK_AND_AND, TK_OR_OR, TK_XOR,
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
    }// xor
    else if(token.type == TK_XOR){
        auto addexpr = std::make_shared<BinOp>();
        addexpr->type = Xor;
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

bool parse_block(Lexer& lexer, std::vector<std::shared_ptr<Statement>>& block, const Token& func_ret_type){
    Token token;
    if(!expect_token(lexer, token, TK_OCURLY)) return false;

    Token name, expr, value;
    std::shared_ptr<Expression> parse_expr;
    std::vector<Token> arglist;
    int expected_returns = func_ret_type.value == "void" ? 0 : 1;
    int actual_returns = 0;
    while(1){
        if(!expect_token(lexer, name, TK_NAME, TK_INT, TK_FLOAT, TK_CHAR, TK_VOID, TK_IF, TK_ELSE, TK_WHILE, TK_FOR, TK_RETURN, TK_CCURLY)){
            LogError(name.loc.display() << ": Expected statement here but got " << name.value);
            return false;
        }
        
        
        if(name.type == TK_CCURLY){
            if(func_ret_type.value != "void" && block.size() == 0){
                LogError(name.loc.display() << ": Missing return statement in function returning " << func_ret_type.value);
                return false;
            }
            break;
        }

        if(name.type == TK_RETURN){
            if(func_ret_type.value != "void"){
                if(!parse_expression(lexer, parse_expr)) return false;
                std::shared_ptr<RetStmt> retStmt = std::make_shared<RetStmt>();
                retStmt->expr = std::move(parse_expr);
                block.push_back(std::move(retStmt));
                actual_returns++;
                continue;
            }
            else{
                if(!expect_token(lexer, token, TK_SEMICOL)){
                    LogError(token.loc.display() << ": Expected ; here but got " << token.value);
                    return false;
                }
                continue;
            }
            //if(!expect_token(lexer, expr, TK_LIT_INT, TK_NAME)) return false;
            //if(expr.type == TK_NAME){
            //    // FUNC OR VAR
            //    // update expr
            //    LogError(expr.loc.display() << ": TK_NAME " << expr.value << " is unknown")
            //}
        }
        else if(name.type == TK_IF || name.type == TK_ELSE || name.type == TK_WHILE || name.type == TK_FOR){
            LogError(name.loc.display() << ": Control flow statements not implemented yet!");
            return false;
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
                if(!expect_token(lexer, token, TK_EQUAL, TK_SEMICOL)){
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
                if(token.type == TK_EQUAL){
                    if(!parse_expression(lexer, parse_expr)) return false;
                    std::shared_ptr<VarDeclStmt> varStmt = std::make_shared<VarDeclStmt>();
                    varStmt->name = expr.value;
                    varStmt->expr = parse_expr;
                    block.push_back(std::move(varStmt));
                    continue;
                }
                else{
                    // semicol, just declaration, null expression ?
                    std::shared_ptr<VarDeclStmt> varStmt = std::make_shared<VarDeclStmt>();
                    varStmt->name = expr.value;
                    varStmt->expr = nullptr;
                    block.push_back(std::move(varStmt));
                    continue;
                }
            }
        }

        if(!expect_token(lexer, token, TK_SEMICOL)){
            LogError(token.loc.display() << ": Expected ; here but got " << token.value);
            return false;
        }
    }
    if(expected_returns != actual_returns){
        LogError("Function expected to have " << expected_returns << " return statements, but got " << actual_returns);
        return false;
    }
    return true;
}

int parse_function(Lexer& lexer, Token& token, Func& func){
    // Trim left
    if(!lexer.Preprocessor()) return false;

    // Return type
    if(!expect_token(lexer, token, TK_INT, TK_CHAR, TK_VOID, TK_FLOAT)){
        LogError(token.loc.display() << ": Expected function return type here but got " << token.value);
        return -1;
    }
    Token ret_type = token;

    // Func name
    Token name;
    if(!expect_token(lexer,  name, TK_NAME)){
        LogError(name.loc.display() << ": Expected function name here but got " << name.value);
        return -1;
    }

    // Args
    std::vector<ArgDecl> args;
    if(!expect_token(lexer, token, TK_OPAREN)){
        LogError(token.loc.display() << ": Expected ( here but got " << token.value);
        return -1;
    }
    while(1){
        if(!expect_token(lexer, token, TK_INT, TK_CHAR, TK_VOID, TK_FLOAT, TK_COMMA, TK_CPAREN)){
            LogError(token.loc.display() << ": Expected argument type, comma or ) here but got " << token.value);
            return -1;
        }
        if(token.type == TK_CPAREN)
            break;
        else if(token.type == TK_INT || token.type == TK_CHAR || token.type == TK_VOID || token.type == TK_FLOAT){
            ArgDecl arg;
            arg.type = token;
            if(!expect_token(lexer, token, TK_NAME, TK_STAR)){
                LogError(token.loc.display() << ": Expected argument name here or star but got " << token.value);
                return -1;
            }
            if(token.type == TK_STAR){
                LogError(token.loc.display() << ": Pointer types not supported yet.");
                return -1;
            }
            else{
                if(arg.type.type == TK_VOID){
                    LogError(token.loc.display() << ": Invalid argument name 'void'.");
                    return -1;
                }
            }
            arg.name = token;
            args.push_back(arg);
        }
        else if(token.type == TK_COMMA){
            continue;
        }
    }

    // body
    std::vector<std::shared_ptr<Statement>> body;
    if(!parse_block(lexer, body, ret_type)){
        LogError(token.loc.display() << ": Error parsing function body for function " << name.value);
        return -1;
    }
    
    func.name = name;
    func.return_type = ret_type;
    func.args = args;
    func.body = body;
    return true;
}