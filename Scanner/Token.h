/*
 * File: Token.h
 */
#ifndef TOKEN_H
#define TOKEN_H
#include <string>

struct Token{
    std::string tokenString;                
    std::string tokenClass;
    std::string value;
    int address;
    std::string segment;
};


#endif /* TOKEN_H */

