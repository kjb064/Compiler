/*
 * File Quad.h
 */

#ifndef QUAD_H
#define QUAD_H
#include <string>

struct Quad{
    std::string operation;
    std::string operand1;
    std::string operand2;
    std::string operand3;
    
    std::string printQuad(){
        std::string str = operation + ", " + operand1 + ", " + operand2 + ", " + operand3 + "\n";
        return str;
    }
};

#endif /* QUAD_H */

