#include "../include/error.h"

ParsingError::ParsingError(const char* message) {
    message = message;
    std::cout << message << std::endl;
    std::cout << "new " << message << std::endl;
}

const char* ParsingError::what() const throw () {
    std::cout << message << std::endl;
    return message;
}