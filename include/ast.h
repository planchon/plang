#ifndef AST_H
#define AST_H

#include "iostream"
#include "string.h"
#include "memory"
#include "vector"
#include "map"
#include <bits/stdc++.h>

#include "token.h"
#include "error.h"

// std::map<char, int> BINARY_OPERATORS = {{'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};

class AST_Node {
public:
    virtual void dump() = 0;
    virtual void visualize(std::string file, int from, int to) = 0;
};

class AST_Number : public AST_Node {
    double val;

public:
    AST_Number(double val) : val(val) {};
    void dump();
    void visualize(std::string file, int from, int to);
};

class AST_Variable : public AST_Node {
    std::string val;

public:
    AST_Variable(std::string val) : val(val) {};
    void dump();
    void visualize(std::string file, int from, int to);
};

class AST_Operator : public AST_Node {
    char op;
    AST_Node* left;
    AST_Node* right;

public:
    AST_Operator(AST_Node* left, char op, AST_Node* right): left(left), op(op), right(right) {};
    void dump();
    void visualize(std::string file, int from, int to);
};

class AST_Call : public AST_Node {
    std::string name;
    std::vector<AST_Number> args;

public:
    AST_Call(std::string name, std::vector<AST_Number> args) : name(name), args(args) {};
    void dump();
    void visualize(std::string file, int from, int to);
};

class AST_Prototype : public AST_Node {
    std::string name;
    std::vector<AST_Variable*> args;

public:
    AST_Prototype(std::string name, std::vector<AST_Variable*> args) : name(name), args(args) {};
    const std::string get_name() { return name; };
    void dump();
    void visualize(std::string file, int from, int to) {};
};

class AST_Function : public AST_Node {
    AST_Prototype proto;
    AST_Node* body;
public:
    AST_Function(AST_Prototype proto, AST_Node* body) : proto(proto), body(body) {};
    void dump();
    void visualize(std::string file, int from, int to) {};
};

class AST {
private:
    std::vector<Token> tokens;
    int current_token_index;
    Token current_token;
    AST_Node* root;

public:
    AST(std::vector<Token> tokens) : tokens(tokens), current_token_index(-1) {};
    bool get_token();
    void show_tree(AST_Node* tree, std::string filename);

    AST_Function* parse_function();
    AST_Prototype* parse_function_proto();
    AST_Node* parse();
    AST_Call* parse_call();
    AST_Call* parse_extern();
    AST_Node* parse_top_level();

    friend std::ostream& operator << (std::ostream& os, AST& ast);
};

#endif