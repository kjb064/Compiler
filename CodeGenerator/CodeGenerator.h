/*
 * File CodeGenerator.h
 */

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include <fstream>
#include "../Quad.h"
#include "../Scanner/Token.h"
#include "../Scanner/LinkedList.h"
#include <queue>
#include <iostream>
#include <list>
using namespace std;

class CodeGenerator {
public:
    CodeGenerator(list <Quad> q, LinkedList *table);
    void generateCode();
private:
    ofstream outputCode;
    void createDataSection();
    void createBssSection();
    void createIORoutines();
    int quadToNumeric(Quad quad);
    list <Quad> quadQueue;
    LinkedList *symbolTable;
    
};

#endif /* CODEGENERATOR_H */

