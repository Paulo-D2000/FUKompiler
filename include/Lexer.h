#pragma once
#include <vector>
#include <fstream>
#include <sstream>

enum class TokenType{
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

class Lexer
{
private:
    Location m_loc;
    std::string m_src;
    std::string m_path;
    size_t m_curr;
    size_t m_bol;
    size_t m_row;

    inline bool not_empty(){
        return m_curr < m_src.length();
    }

    inline void update_loc(){
        m_loc.path = m_path;
        m_loc.row = m_row;
        m_loc.col = m_curr - m_bol;
    }

    void chop_char();
    void trim_left();
    void drop_line();
    bool next_token(Token& token);

public:
    Lexer() {}
    Lexer(const std::string& fileContent, const std::string& filePath);

    bool GetNextToken(Token& token);
};
