#include "../include/error.h"

const char* ParsingError::what() const throw () {
    std::cout << ("Error: parsing: " + message).c_str() << std::endl;
    return ("Error: parsing: " + message).c_str();
}