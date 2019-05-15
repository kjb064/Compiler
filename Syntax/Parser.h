/*
 * File Parser.h
 */
#ifndef PARSER_H
#define PARSER_H
#include "../Scanner/Scanner.h"
#include <queue>
#include <stack>
#include "../Quad.h"
#include "../CodeGenerator/CodeGenerator.h"
#include "../Optimizer/Optimizer.h"

class Parser {
public:
    Parser(string sourceFileName);
    void parseSource();
private:
    ofstream quadFile;
    int numOperators;
    char **precedenceTable;
    int classToNumeric(string str);
    string numericToClass(int num);
    Token errorRecovery(Token currentInput, int &topColNum, int &choice);                     
    void errorCheck(string oper, Quad &quad, queue <Token> &q);
    void addQuad(Quad quad);
    bool variableExistenceCheck(list <Quad> quads, LinkedList *table);
    bool assignmentCheck(list <Quad> quads, LinkedList *table);
    Scanner *scanner;
    string label;
    string whileLabel;
    int labelNum = 1;
    int whileLabelNum = 1;
    int tempNum = 1;
    bool noErrors = true;
    bool isProcedureQuad = false;
    stack <Token> s;
    stack <string> startWhile;
    stack <string> fixUp;   
    stack <string> bracketStack;
    stack <string> parenStack;
    list <Quad> mainQuads;
    list <Quad> procedureQuads;
    
    // Below are DFSAs in tabular form for use with determining proper
    // syntax of VAR and CONST statements.
    int varStates[4][3] = { 1, -1, -1,
                         -1,  2,  3,
                          1, -1, -1,
                         -1, -1, -1 };
    
    int constStates[6][5] = { 1, -1, -1, -1, -1,
                             -1, 2, -1, -1, -1,
                             -1, -1, 3, -1, -1,
                             -1, -1, -1, 4, 5,
                              1, -1, -1, -1, -1,
                              -1, -1, -1, -1, -1 };
};

#endif /* PARSER_H */

