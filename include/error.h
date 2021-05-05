#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <string>
#include "iostream"

struct ParsingError : public std::exception {
    const char* message;
public:
    ParsingError(const char* message);
    virtual const char *what() const noexcept override;
};

#endif