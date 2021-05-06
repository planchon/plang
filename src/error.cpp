#include "../include/error.h"

ParsingError::ParsingError(const char* message) {
    message = message;
    std::cout << "Parsing error: " << message << std::endl;
}

const char* ParsingError::what() const throw () {
    return message;
}