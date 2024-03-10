#include "Lexer.h"
#include "Common.h"

Lexer::Lexer(const std::string& fileContent, const std::string& filePath):
    m_src(std::move(fileContent)), m_path(std::move(filePath)), m_curr(0), m_bol(0), m_row(0)
{
}

void Lexer::chop_char(){
    if(not_empty()){
        char c = m_src.at(m_curr);
        m_curr++;
        if(c == '\n'){
            m_bol = m_curr;
            m_row++;
        }
    }
}

void Lexer::trim_left(){
    while(not_empty() && std::isspace(m_src.at(m_curr))){
        chop_char();
    }
}

void Lexer::drop_line(){
    while(not_empty() && m_src.at(m_curr) != '\n'){
        chop_char();
    }
    if(not_empty()){
        chop_char();
    }
}

bool Lexer::next_token(Token& token){
    trim_left();

    while(not_empty() && m_src.at(m_curr) == '#'){
        drop_line();
        trim_left();
    }

    if(!not_empty()) return false;

    update_loc();
    char first = m_src.at(m_curr);
    size_t start = m_curr;

    if(std::isalpha(first)){
        start = m_curr;
        while(not_empty() && std::isalnum(m_src.at(m_curr))){
            chop_char();
        }
        
        token.loc = m_loc;
        token.type = TokenType::TK_NAME;
        token.value = m_src.substr(start, m_curr - start);
        return true;
    }

    switch (first)
    {
    case '(':
        chop_char();
        token.loc = m_loc;
        token.type = TokenType::TK_OPAREN;
        token.value = first;
        return true;

    case ')':
        chop_char();
        token.loc = m_loc;
        token.type = TokenType::TK_CPAREN;
        token.value = first;
        return true;

    case '{':
        chop_char();
        token.loc = m_loc;
        token.type = TokenType::TK_OCURLY;
        token.value = first;
        return true;

    case '}':
        chop_char();
        token.loc = m_loc;
        token.type = TokenType::TK_CCURLY;
        token.value = first;
        return true;

    case ';':
        chop_char();
        token.loc = m_loc;
        token.type = TokenType::TK_SEMICOL;
        token.value = first;
        return true;

    case '"':
        chop_char();
        start = m_curr;
        while(not_empty() && m_src.at(m_curr) != '"'){
            chop_char();
        }

        if(not_empty()){
            std::string text = m_src.substr(start, m_curr - start);
            chop_char();
            token.loc = m_loc;
            token.type = TokenType::TK_LIT_STR;
            token.value = text;
            return true;
        }

        LogError(m_loc.display() << ": unclosed string literal.");
        return false;
    
    default:
        break;
    }

    if(std::isdigit(first)){
        start = m_curr;
        while(not_empty() && std::isdigit(m_src.at(m_curr))){
            chop_char();
        }
        std::string val = m_src.substr(start, m_curr - start);
        token.loc = m_loc;
        token.type = TokenType::TK_LIT_INT;
        token.value = val;
        return true;
    }

    return false;
}

bool Lexer::GetNextToken(Token& token){
    bool ret = next_token(token);
    LogDebug(token.loc.display() << ": " << token.value);
    return ret;
}