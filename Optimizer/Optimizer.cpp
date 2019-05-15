/*
 * File Optimizer.cpp
 */
#include "Optimizer.h"
using namespace std;

Optimizer::Optimizer(std::list<Quad> q){
    quads = q;
}

/*
 * Optimizes assignment quads, removing redundant load, store operations from
 * final code.
 */
list<Quad> Optimizer::optimizeAssignment(){
    list<Quad> newQuads;
    
    while(!quads.empty()){
        if(quads.front().operation == "="){
            if(newQuads.back().operation == "+" || newQuads.back().operation == "-" 
                    || newQuads.back().operation == "*" || newQuads.back().operation == "/"){
                if(newQuads.back().operand3 == quads.front().operand2){
                    newQuads.back().operand3 = quads.front().operand1;
                }
                else{
                    newQuads.push_back(quads.front());
                }
            }
            else{
                newQuads.push_back(quads.front());
            }
                      
        }
        else{
            newQuads.push_back(quads.front());
        }
        
        quads.pop_front();
    } // end while
        
    return newQuads;
} // end optimizeAssignment
