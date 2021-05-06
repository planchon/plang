#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <string>
#include "iostream"

class plangError {
    std::string message;
public:
    plangError() {};
    virtual void what() = 0;
};

class ParsingError : plangError {
    std::string message;
public:
    ParsingError(std::string message) : message(message) {};
    void what() { std::cout << "Parsing Error : " << message << std::endl; }
};

#endif