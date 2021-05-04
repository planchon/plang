#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <string>
#include "iostream"

struct ParsingError : public std::exception {
    std::string message;
public:
    ParsingError(std::string message) : message(message) {};
    virtual const char *what() const noexcept override;
};

#endif