#include "../include/ast.h"

// void AST_Node::dump() {}

void AST_Number::dump()      {
    std::cout << "AST_Number(" << val << ")" << std::endl;
}

void AST_Number::visualize(std::string filename, int from, int to) {
    std::ofstream file(filename, std::ios_base::app);
    file << "n" << to << std::endl;
    file << "n" << to << " [label=\"" << val << "\"]" << std::endl;
    file << "n" << from << "--" << "n" << to << std::endl; 
    file.close();
}

void AST_Variable::dump() {
    std::cout << "AST_Variable(" << val << ")" << std::endl;
}

void AST_Variable::visualize(std::string filename, int from, int to) {
    std::ofstream file(filename, std::ios_base::app);
    file << "n" << to << std::endl;
    file << "n" << to << " [label=\"" << val << "\"]" << std::endl;
    file << "n" << from << "--" << "n" << to << std::endl; 
    file.close();
}

void AST_Operator::dump() {
    std::cout << "AST_Operator(";
    left->dump();
    std::cout  << ", " << op << ", ";
    right->dump();
    std::cout << ")" << std::endl;
}

void AST_Operator::visualize(std::string filename, int from, int to) {
    std::ofstream file(filename, std::ios_base::app);
    file << "n" << to << std::endl;
    file << "n" << to << " [label=\"" << op << "\"]" << std::endl;
    left->visualize(filename, to, to+1);
    right->visualize(filename, to, to+2);
    file << "n" << from << "--" << "n" << to << std::endl;
    file.close();
}

void AST_Call::dump() {
    std::cout << "AST_Call(" << name << ", " << args.size() << ")" << std::endl;
}

void AST_Call::visualize(std::string filename, int from, int to) {
    std::ofstream file(filename, std::ios_base::app);
    file << "n" << to << std::endl;
    file << "n" << to << " [label=\"" << name << "\"]" << std::endl;
    file << "n" << from << "--" << "n" << to << std::endl; 
    file.close();
}

void AST_Prototype::dump() {
    std::cout << "AST_Prototype(" << name << ", " << args.size() << ")" << std::endl;
}

void AST_Function::dump() {
    std::cout << "AST_Function(";
    proto.dump();
    std::cout << ", ";
    body->dump();
    std::cout << ")" << std::endl;
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

        return true;
    } else {
        return false;
    }
}

AST_Prototype* AST::parse_function_proto() {
    get_token(); // eat the "def" keyword

    if (current_token.tok != TOK_IDENTIFIER) {
        throw ParsingError("expecting a function name got " + current_token.tok);
    }

    std::string function_name;
    function_name = current_token.identifier;

    get_token();

    if (current_token.tok != TOK_OPERATOR && current_token.ope != '(') {
        throw ParsingError("expecting '(' got " + current_token.tok);
    }

    std::vector<AST_Variable*> args;

    while (get_token() && current_token.ope != ')') {
        if (current_token.tok != TOK_IDENTIFIER) {
            throw ParsingError("excepting variable got " + current_token.tok);
        }

        args.push_back(new AST_Variable(current_token.identifier));
    }

    // check if the last token we eat was a )
    if (current_token.ope != ')') {
        throw ParsingError("excepting ) got " + current_token.tok);
    }

    return new AST_Prototype(function_name, args);
}

AST_Function* AST::parse_function() {
    AST_Prototype* proto = parse_function_proto();
}

AST_Call* AST::parse_call() {
    std::string function_name = current_token.identifier;
    get_token();

    // call a function without parameters
    if (current_token.ope != '(') {
        return new AST_Call(function_name, {});
    }

    std::vector<AST_Number> args;

    while(get_token() && current_token.ope != ')') {
        if (current_token.tok != TOK_NUMBER) {
            throw ParsingError("excepting a number got " + current_token.tok);
        }

        args.push_back(AST_Number(current_token.value));
    }

    if (current_token.ope != ')') {
        throw ParsingError("excepting a ')' got " + current_token.tok);
    }

    return new AST_Call(function_name, args);
}

AST_Call* AST::parse_extern() {
    get_token(); //eat extern keyword
    return parse_call();
}

AST_Node* AST::parse_top_level() {
    AST_Node* left;
    switch (current_token.tok)
    {
        case TOK_IDENTIFIER:
            left = parse_call();
            break;
        case TOK_NUMBER:    
            left = new AST_Number(current_token.value);
            break;
        default:
            throw ParsingError("parse level was excepting something got " + current_token.tok);
    }

    // we are at the end of the tree, i guess
    if (!get_token()) {
        return left;
    }

    if (current_token.tok != TOK_OPERATOR) {
        throw ParsingError("excepting an operator got " + current_token.tok);
    }

    char op = current_token.ope;
    get_token();

    return new AST_Operator(left, op, parse_top_level());
}

std::ostream& operator << (std::ostream& os, AST& ast) {}

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
    filetest << "graph ast {" << std::endl;
    filetest.close();
 
    tree->visualize(filename, 0, 1);
 
    std::ofstream file2(filename, std::ios_base::app);
    file2 << "}" << std::endl;
    file2.close();

    std::cout << "compiling the graph..." << std::endl;
    std::string command = "dot -Tps " + filename + " -o " + filename + ".ps";
    std::system(command.c_str());
    std::cout << "graph generated at " << filename + ".ps" << std::endl;
}