/*
 * File: Scanner.h
 */
#ifndef SCANNER_H
#define SCANNER_H
#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h> // for "isspace"
#include <cstdio>
#include "Token.h"
#include "LinkedList.h"
using namespace std;

const string CONST = "$CONST";
const string IF = "$IF";
const string VAR = "<$var>";
const string THEN = "$THEN";
const string PROCEDURE = "$PROCEDURE";
const string WHILE = "$WHILE";
const string CALL = "$CALL";
const string DO = "$DO";
const string ELSE = "$ELSE";
const string ODD = "$ODD";
const string CLASS = "$CLASS";
const string READ = "$READ";
const string WRITE = "$WRITE";

const string PROGRAMNAME = "<ProgramName>";
const string PROCEDURENAME = "<PROCEDURE>";
const string VARNAME = "<var>";
const string LBRACE = "$LB";
const string RBRACE = "$RB";
const string LPAREN = "$LP";
const string RPAREN = "$RP";
const string SEMICOLON = "<semi>";
const string ASSIGN = "<assign>";
const string INTEGER = "<integer>";
const string COMMA = "<comma>";
const string ADDOP = "<addop>";
const string MOP = "<mop>";
const string END_OF_FILE = "EOF";
const string RELOP = "<relop>";
const string ADDASSIGN = "<addAssign>";
const string MOPASSIGN = "<mopAssign>";
const string NOT = "<NOT>";
const string TEMP = "<temp>";

const string PROC_LBRACE = "PROC_LBRACE";
const string PROC_RBRACE = "PROC_RBRACE";

const string CS = "CS"; // code segment
const string DS = "DS"; // data segment

const Token reservedWords[] = { {"CONST", CONST}, {"IF", IF}, {"VAR", VAR}, {"CLASS", CLASS},
{"THEN", THEN}, {"ELSE", ELSE}, {"WHILE", WHILE}, {"DO", DO}, {"CALL", CALL}, {"ODD", ODD}, {"PROCEDURE", PROCEDURE}, {"READ", READ}, 
{"WRITE", WRITE} };

class Scanner {
private:
	int rows, cols;
	int** scannerTable;
        int** firstPassTable;
        fstream sourceFile;     // input source code
        ofstream tokenFile;     // output token list
        ofstream symbolTableFile;   // output symbol table
        char lastChar = ' ';
        int numReservedWords = 13;
        int isReservedWord(Token token);
        int charToNumeric(char ch);
        char getNextChar();
        int tokenToNumeric(Token token);

public:
	Scanner(string sourceFileName);
        Token buildToken();
        void buildSymbolTable();
        void resetFilePointer();
        LinkedList *symbolTable;
};

#endif /* SCANNER_H */

