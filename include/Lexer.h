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

    inline bool is_multline_comment(){
        static bool begin;
        if(not_empty() && m_src.at(m_curr) == '/'){
            if(((m_curr + 1) < m_src.length()) && m_src.at(m_curr+1) == '*'){
                chop_char();
                chop_char();
                begin = true;
                return true;
            }
        }
        else if(begin && not_empty() && m_src.at(m_curr) == '*'){
            if(((m_curr + 1) < m_src.length()) && m_src.at(m_curr+1) == '/'){
                chop_char();
                chop_char();
                begin = false;
                return true;
            }
        }
        if(begin){
            return true;
        }
        return false;
    }

    inline bool is_comment(){
        if(not_empty() && m_src.at(m_curr) == '/'){
            if(((m_curr + 1) < m_src.length()) && m_src.at(m_curr+1) == '/'){
                return true;
            }
        }
        return false;
    }

    inline void update_loc(){
        m_loc.path = m_path;
        m_loc.row = m_row;
        m_loc.col = m_curr - m_bol;
    }

    void chop_char();
    void drop_line();
    bool next_token(Token& token);
    void trim_left();
    
    public:
    Lexer() {}
    Lexer(const std::string& fileContent, const std::string& filePath);
    
    bool Preprocessor();

    bool GetNextToken(Token& token);
    inline bool isEOF(){
        bool _eof = (m_curr+1) >= m_src.length();
        if(_eof)
            LogDebug("EOF");
        return _eof;
    }
};
