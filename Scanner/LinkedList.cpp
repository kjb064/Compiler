/*
 * File: LinkedList.cpp
 */
#include "LinkedList.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

LinkedList::LinkedList(string outputFileName) {
    this->head = NULL;
    this->symbolTableFileName = outputFileName;
}

void LinkedList::add(Token newToken){

    if(this->head == NULL){
        this->head = new Node(newToken, NULL);
        this->last = this->head;
    }
    else{
        Node* temp = this->head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = new Node(newToken, NULL); 
        this->last = temp->next;
    }
      
}

void LinkedList::printList(){   
    
    if(this->head == NULL)
        return;
   
    Node* temp = this->head;
    ofstream symbolTableFile;
    symbolTableFile.open(symbolTableFileName);
    symbolTableFile << left << setw(40) << "SYMBOL";
    symbolTableFile << left << setw(20) << "CLASSIFICATION";
    symbolTableFile << right << setw(10) << "VALUE";
    symbolTableFile << right << setw(12) << "ADDRESS";
    symbolTableFile << right << setw(5) << "  SEGMENT" << endl;
    
    while(temp){
        symbolTableFile << left << setw(40) << temp->token.tokenString;
        symbolTableFile << left << setw(20) << temp->token.tokenClass;
        symbolTableFile << right << setw(10) <<temp->token.value;
        symbolTableFile << right << setw(12) << temp->token.address;
        symbolTableFile << right << setw(5) << temp->token.segment << endl;
        temp = temp->next;
    }
    
    symbolTableFile.close();
}

bool LinkedList::isInList(string tokenString){
    Node* temp = this->head;
    while(temp){
        if(temp->token.tokenString == tokenString){
            return true;
        }
        temp = temp->next;
    }
    return false;
}

string LinkedList::getSymbolClass(std::string tokenString){
    Node* temp = this->head;
    while(temp){
        if(temp->token.tokenString == tokenString){
            return temp->token.tokenClass;
        }
        temp = temp->next;
    }
}