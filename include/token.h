#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include<cstring>

enum token {
    // end of file
    TOK_NULL = 0,
    TOK_EOF = -1,

    // commands
    TOK_DEF = -2,
    TOK_EXTERN = -3,

    TOK_IDENTIFIER = -4,
    TOK_NUMBER = -5,

    TOK_OPERATOR = -6,
};

class Token {
public:
    token tok;
    std::string identifier;
    std::string raw_data;
    int pos;
    double value;
    char ope;

    Token();
    Token(token token, std::string raw_data, int pos) : tok(token), raw_data(raw_data), pos(pos) {};
    Token(token token, std::string identifier, std::string raw_data, int pos) : tok(token), identifier(identifier), raw_data(raw_data), pos(pos) {};
    Token(token token, char ope, std::string raw_data, int pos) : tok(token), ope(ope), raw_data(raw_data), pos(pos) {};
    Token(token token, double value, std::string raw_data, int pos) : tok(token), value(value), raw_data(raw_data), pos(pos) {};

    friend std::ostream& operator << (std::ostream& os, Token& tok);
};

class Tokenizer {
public:
    std::string filename;
    std::ifstream file;
    bool jit_console;

    Tokenizer() : jit_console(true) {};
    void from_data(std::string data);
    void from_file(std::string filename);
    std::vector<Token> tokenizer_loop();

private:
    bool get_next_char();
    void skip_space();
    Token get_token();

    char* filedata;
    int file_pos;
    int file_size;
    char current_char;
};

#endif