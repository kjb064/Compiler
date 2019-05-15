#ifndef STACK_H
#define STACK_H
#include "../Scanner/LinkedList.h"

class Stack {
public:
    Stack();
    void push(Token token);
    Token pop();
    Token peek();
    Node *top;
private:

};

#endif /* STACK_H */

