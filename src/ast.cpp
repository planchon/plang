#include "../include/ast.h"

void AST_Number::dump()      {
    std::cout << "AST_Number(" << val << ")" << std::endl;
}

void AST_Variable::dump() {
    std::cout << "AST_Variable(" << val << ")" << std::endl;
}

void AST_Operator::dump() {
    std::cout << "AST_Operator(";
    left->dump();
    std::cout  << ", " << op << ", ";
    right->dump();
    std::cout << ")" << std::endl;
}

void AST_Call::dump() {
    std::cout << "AST_Call(" << name << ", " << args.size() << ")" << std::endl;
}

void AST_Prototype::dump() {
    std::cout << "AST_Prototype(" << name << ", " << args.size() << ")" << std::endl;
}

void AST_Function::dump() {
    std::cout << "AST_Function(";
    proto->dump();
    std::cout << ", ";
    body->dump();
    std::cout << ")" << std::endl;
}

int AST_Variable::visualize(std::string filename, int from, int to, std::string pos) {
    std::ofstream file(filename, std::ios_base::app);
    file << to << " [label=\"Variable: " << val << "\"]" << std::endl;
    std::string pos_ = (pos == "") ? "" : ":" + pos; 
    file << from << pos_ << " -> " << to << std::endl; 
    file.close();

    return to;
}

int AST_Number::visualize(std::string filename, int from, int to, std::string pos) {
    std::ofstream file(filename, std::ios_base::app);
    file << to << " [label=\"Number: " << val << "\"]" << std::endl;
    std::string pos_ = (pos == "") ? "" : ":" + pos; 
    file << from << pos_ << " -> " << to << std::endl; 
    file.close();

    return to;
}

int AST_Operator::visualize(std::string filename, int from, int to, std::string pos) {
    std::ofstream file(filename, std::ios_base::app);
    file << to << " [label=\"Operator: " << op << "\"]" << std::endl;
    int inc = left->visualize(filename, to, to+1);
    inc = right->visualize(filename, to, inc + 1);
    std::string pos_ = (pos == "") ? "" : ":" + pos; 
    file << from << pos_ << " -> " << to << std::endl; 
    file.close();
    
    return inc;
}

int AST_Call::visualize(std::string filename, int from, int to, std::string pos) {
    std::ofstream file(filename, std::ios_base::app);
    file << to << " [label=\"Call: " << name << "|<args> args\", shape=record]" << std::endl;
    std::string pos_ = (pos == "") ? "" : ":" + pos; 
    int inc = to;
    for (AST_Node* arg : args) {
        inc = arg->visualize(filename, to, inc + 1, pos="args");
    }
    file << from << pos_ << " -> " << to << std::endl;  
    file.close();
    return inc;
}

int AST_Function::visualize(std::string filename, int from, int to, std::string pos) {
    std::ofstream file(filename, std::ios_base::app);
    file << to << " [label=\"Function: " << proto->get_name() << "|<args> args|<body> body\", shape=record];" << std::endl;
    
    // visualisze all the args
    int inc = to;
    for (AST_Node* arg : proto->get_args()) {
        inc = arg->visualize(filename, to, inc + 1, pos="args");
    }

    inc = body->visualize(filename, to, inc + 1, pos="body");

    file.close();
    return inc;
}

bool AST::get_token() {
    if (current_token_index + 1 < tokens.size()) {
        current_token_index++;

        if (tokens[current_token_index].tok == TOK_EOF) {
            return false;
        }

        current_token.tok = tokens[current_token_index].tok;
        current_token.ope = tokens[current_token_index].ope;
        current_token.identifier = tokens[current_token_index].identifier;
        current_token.value = tokens[current_token_index].value;
        current_token.raw_data = tokens[current_token_index].raw_data;
        current_token.pos = tokens[current_token_index].pos;

        return true;
    } else {
        return false;
    }
}

bool AST::undo_token() {
    if (current_token_index - 1 > 0) {
        current_token_index--;

        if (tokens[current_token_index].tok == TOK_EOF) {
            return false;
        }

        current_token.tok = tokens[current_token_index].tok;
        current_token.ope = tokens[current_token_index].ope;
        current_token.identifier = tokens[current_token_index].identifier;
        current_token.value = tokens[current_token_index].value;

        return true;
    } else {
        return false;
    }
}

AST_Prototype* AST::parse_function_proto() {
    get_token(); // eat the "def" keyword

    if (current_token.tok != TOK_IDENTIFIER) {
        throw ParsingError(std::string("was excepting a function name but got ") + current_token.raw_data);
    }

    std::string function_name;
    function_name = current_token.identifier;

    get_token();

    if (current_token.tok != TOK_OPERATOR || current_token.ope != '(') {
        throw ParsingError(std::string("was excepting a ( but got ") + current_token.raw_data);
    }

    std::vector<AST_Node*> args;

    while (get_token() && current_token.ope != ')') {
        if (current_token.tok != TOK_IDENTIFIER) {
            throw ParsingError(std::string("was excepting a variable but got ") + current_token.raw_data);
        }

        args.push_back(new AST_Variable(current_token.identifier));
    }

    // check if the last token we eat was a )
    if (current_token.ope != ')') {
        throw ParsingError(std::string("was excepting ) got ") + current_token.raw_data);
    }

    get_token(); // we eat the )

    return new AST_Prototype(function_name, args);
}

AST_Function* AST::parse_function() {
    AST_Prototype* proto = parse_function_proto();
    AST_Node* body = parse_top_level();

    return new AST_Function(proto, body); 
}

AST_Node* AST::parse_call() {
    std::string function_name = current_token.identifier;
    get_token();

    // call a function without parameters
    if (current_token.ope != '(') {
        // we eat the token if we dont undo.
        undo_token();
        return new AST_Variable(function_name);
    }

    std::vector<AST_Node*> args;

    while(get_token() && current_token.ope != ')') {
        switch (current_token.tok) {
        case TOK_NUMBER:
            args.push_back(new AST_Number(current_token.value));
            break;
        case TOK_IDENTIFIER:
            args.push_back(new AST_Variable(current_token.identifier));
            break;
        default:
            throw ParsingError(std::string("was excepting a number or variable but got ") + current_token.raw_data);
        }
    }

    if (current_token.ope != ')') {
        throw ParsingError(std::string("excepting a ')' got ") + current_token.raw_data);
    }

    return new AST_Call(function_name, args);
}

AST_Node* AST::parse_extern() {
    get_token(); //eat extern keyword
    return parse_call();
}

// return the correct tree
// must begin with an operator
AST_Node* AST::parse_priority_operator(int precedence, AST_Node* left) {
    while(1) {
        int priority = get_operator_priority(current_token);
        
        if (priority < precedence) {
            return left;
        }
        
        char op = current_token.ope;
        get_token();

        AST_Node* right = parse_primary();

        get_token();
        
        int new_priority = get_operator_priority(current_token);
        if (priority < new_priority) {
            right = parse_priority_operator(priority + 1, right);
        }

        left = new AST_Operator(left, op, right);
    }
}

AST_Node* AST::parse_parenthesis() {
    if (current_token.tok != TOK_OPERATOR || current_token.ope != '(') {
        throw ParsingError(std::string("was excepting a ( but got ") + current_token.raw_data);
    }

    get_token(); // we eat the (

    AST_Node* value = parse_top_level();

    if (current_token.tok != TOK_OPERATOR || current_token.ope != ')') {
        throw ParsingError(std::string("was excepting a ) but got ") + current_token.raw_data);
    }
    
    return value;
}

AST_Node* AST::parse_primary() {
    switch (current_token.tok) {
        case TOK_OPERATOR:
            return parse_parenthesis();
        case TOK_IDENTIFIER:
            return parse_call();
            break;
        case TOK_NUMBER:    
            return new AST_Number(current_token.value);
            break;
        default:
            throw ParsingError(std::string("was not excepting ") + current_token.raw_data);
    }
}

AST_Node* AST::parse_top_level() {
    AST_Node* left = parse_primary();
    get_token();
    return parse_priority_operator(0, left);
}

AST_Node* AST::parse() {
    get_token();
    switch (current_token.tok)
    {
        case TOK_EOF:
            return nullptr;
        case TOK_DEF:
            return parse_function();
        case TOK_EXTERN:
            return parse_extern();
        case TOK_IDENTIFIER:
            return parse_call();
        default:
            return parse_top_level();
    }
}

void AST::show_tree(AST_Node* tree, std::string filename) {
    std::ofstream filetest(filename);
    filetest << "digraph ast {" << std::endl;
    filetest << "node [shape=oval];" << std::endl;
    filetest.close();
 
    tree->visualize(filename, 0, 1);
 
    std::ofstream file2(filename, std::ios_base::app);
    file2 << "}" << std::endl;
    file2.close();

    std::cout << "compiling the graph..." << std::endl;
    std::string command = "dot -Tpdf " + filename + " -o graph/" + filename + ".pdf";
    std::system(command.c_str());
    std::cout << "graph generated at graph/" << filename + ".pdf" << std::endl;
}

int get_operator_priority(Token tok) {
    if (tok.tok != TOK_OPERATOR) {
        return -1;
    }

    switch (tok.ope) {
        case '<':
            return 10;
        case '+':
            return 20;
        case '-':
            return 20;
        case '*':
            return 40;
        default:
            return -1;
    }
}