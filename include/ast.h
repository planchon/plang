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

int get_operator_priority(Token);

class AST_Node {
public:
    virtual void dump() = 0;
    virtual int visualize(std::string file, int from, int to, std::string pos = "") = 0;
};

class AST_Number : public AST_Node {
    double val;

public:
    AST_Number(double val) : val(val) {};
    void dump();
    int visualize(std::string file, int from, int to, std::string pos = "");
};

class AST_Variable : public AST_Node {
    std::string val;

public:
    AST_Variable(std::string val) : val(val) {};
    void dump();
    int visualize(std::string file, int from, int to, std::string pos = "");
};

class AST_Operator : public AST_Node {
    char op;
    AST_Node* left;
    AST_Node* right;

public:
    AST_Operator(AST_Node* left, char op, AST_Node* right): left(left), op(op), right(right) {};
    void dump();
    int visualize(std::string file, int from, int to, std::string pos = "");
};

class AST_Call : public AST_Node {
    std::string name;
    std::vector<AST_Node*> args;

public:
    AST_Call(std::string name, std::vector<AST_Node*> args) : name(name), args(args) {};
    void dump();
    int visualize(std::string file, int from, int to, std::string pos = "");
};

class AST_Prototype : public AST_Node {
    std::string name;
    std::vector<AST_Node*> args;

public:
    AST_Prototype(std::string name, std::vector<AST_Node*> args) : name(name), args(args) {};
    const std::string get_name() { return name; };
    const std::vector<AST_Node*> get_args() { return args; };
    void dump();
    int visualize(std::string file, int from, int to, std::string pos = "") {};
};

class AST_Function : public AST_Node {
    AST_Prototype* proto;
    AST_Node* body;
public:
    AST_Function(AST_Prototype* proto, AST_Node* body) : proto(proto), body(body) {};
    void dump();
    int visualize(std::string file, int from, int to, std::string pos = "");
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
    bool undo_token();
    void show_tree(AST_Node* tree, std::string filename);

    AST_Function* parse_function();
    AST_Prototype* parse_function_proto();
    AST_Node* parse();
    AST_Node* parse_call();
    AST_Node* parse_extern();
    AST_Node* parse_top_level();
    AST_Node* parse_priority_operator(int, AST_Node*);
    AST_Node* parse_primary();
    AST_Node* parse_parenthesis();
};

#endif