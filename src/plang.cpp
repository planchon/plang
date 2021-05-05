#include <iostream>
#include "../include/token.h"
#include "../include/ast.h"

int main(int argc, char** argv) {
    std::cout << "plang compiler v0.1" << std::endl;
    Tokenizer tokenizer = Tokenizer();
    // we give the program a file to analyse
    if (argc == 2) {
        std::string filename = argv[1];
        tokenizer.from_file(filename);
    } else {
        fprintf(stderr, "plang> ");
        std::string data;
        std::getline(std::cin, data);
        tokenizer.from_data(data);
    }
    
    auto test = tokenizer.tokenizer_loop();

    auto ast = AST(test);
    AST_Node* res;

    try {
        res = ast.parse();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    ast.show_tree(res, "test");

    return 0;
}
