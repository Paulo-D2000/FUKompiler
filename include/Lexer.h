#pragma once
#include <vector>
#include <fstream>
#include <sstream>

#include "Common.h"

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
