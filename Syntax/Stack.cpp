#include "Stack.h"
#include <iostream>
// VERIFY THIS WORKS, IMPLEMENT OTHER TYPICAL STACK FUNCTIONS

Stack::Stack() {
    top = NULL;
}

void Stack::push(Token token){
    Node *newNode;
    newNode->token = token;
    newNode->next = top;
    top = newNode;
    std::cout << "PUSHED" << std::endl;
}

Token Stack::pop(){
    Token token;

    if(top == NULL){
        return token;    // empty stack, return null token
    }
    else{
        Node *temp = top;
        token = top->token;
        top = top->next;
        delete temp;
        return token;
    }
}

Token Stack::peek(){
    Token token;
    if(top == NULL){
        return token; 
    }
    else{
        token = top->token;
        return token;
    }
}



