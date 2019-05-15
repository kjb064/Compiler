/*
 *  File: main.cpp
 *  
 */

#include "Syntax/Parser.h"
#include <iostream>

int main(){
    string sourceFile;
    cout << "Enter source file name: ";
    cin >> sourceFile;
    Parser parser(sourceFile);
    parser.parseSource();

}
