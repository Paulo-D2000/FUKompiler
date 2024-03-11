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

    while(not_empty() && (m_src.at(m_curr) == '#' || is_comment())){
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
        token.type = TK_NAME;
        token.value = m_src.substr(start, m_curr - start);
        return true;
    }

    switch (first)
    {
    case '(':
        chop_char();
        token.loc = m_loc;
        token.type = TK_OPAREN;
        token.value = first;
        return true;

    case ')':
        chop_char();
        token.loc = m_loc;
        token.type = TK_CPAREN;
        token.value = first;
        return true;

    case '{':
        chop_char();
        token.loc = m_loc;
        token.type = TK_OCURLY;
        token.value = first;
        return true;

    case '}':
        chop_char();
        token.loc = m_loc;
        token.type = TK_CCURLY;
        token.value = first;
        return true;

    case ';':
        chop_char();
        token.loc = m_loc;
        token.type = TK_SEMICOL;
        token.value = first;
        return true;

    case ',':
        chop_char();
        token.loc = m_loc;
        token.type = TK_COMMA;
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
            token.type = TK_LIT_STR;
            token.value = text;
            return true;
        }

        LogError(m_loc.display() << ": unclosed string literal.");
        return false;

    case 39:
        chop_char();
        start = m_curr;
        while(not_empty() && m_src.at(m_curr) != '\''){
            chop_char();
        }

        if(not_empty()){
            std::string text = m_src.substr(start, m_curr - start);
            chop_char();
            if(text.length() > 1){
                LogError(m_loc.display() << ": char literal must contain a single character.");
            }
            token.loc = m_loc;
            token.type = TK_LIT_CHAR;
            token.value = text;
            return true;
        }

        LogError(m_loc.display() << ": unclosed char literal.");
        return false;

    case '=':
        chop_char();
        token.loc = m_loc;
        token.type = TK_EQUAL;
        token.value = first;
        return true;

    case '+':
        chop_char();
        token.loc = m_loc;
        token.type = TK_PLUS;
        token.value = first;
        if(not_empty()){
            char next = m_src.at(m_curr);
            if(next == '+'){
                token.loc = m_loc;
                token.type = TK_PLUS_PLUS;
                token.value = std::string({first,next});
                return true;
            }else if(std::isdigit(next) || std::isspace(next)){
                return true;
            }else{
                return false;
            }
        }
        break;

    case '-':
        chop_char();
        token.loc = m_loc;
        token.type = TK_MINUS;
        token.value = first;
        return true;

    case '*':
        chop_char();
        token.loc = m_loc;
        token.type = TK_STAR;
        token.value = first;
        return true;

    case '%':
        chop_char();
        token.loc = m_loc;
        token.type = TK_PERCENT;
        token.value = first;
        return true;

    case '/':
        chop_char();
        token.loc = m_loc;
        token.type = TK_SLASH;
        token.value = first;
        return true;
        
    case '&':
        chop_char();
        token.loc = m_loc;
        token.type = TK_AND;
        token.value = first;
        return true;

    case '|':
        chop_char();
        token.loc = m_loc;
        token.type = TK_OR;
        token.value = first;
        return true;
    
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
        token.type = TK_LIT_INT;
        token.value = val;
        return true;
    }

    LogError(m_loc.display() << ": token error.");

    return false;
}

bool Lexer::GetNextToken(Token& token){
    bool ret = next_token(token);
    LogDebug(token.loc.display() << ": " << TokenTypeNames[token.type] << ": " << token.value);
    return ret;
}