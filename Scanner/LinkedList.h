/*
 * File: LinkedList.h
 * (For creating the symbol table)
 */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "Token.h"
#include <string>

class Node{
public:
    Token token;
    Node* next;
    
    Node(Token tok, Node* n){
        token = tok;
        next = n;
    }
};

class LinkedList {
public:
    LinkedList(std::string outputFileName);
    bool isInList(std::string tokenString);
    void add(Token newToken);
    void printList();
    std::string getSymbolClass(std::string tokenString);
    Node* head;
    Node* last;
private:
    std::string symbolTableFileName;
};

#endif /* LINKEDLIST_H */

