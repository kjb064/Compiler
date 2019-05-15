/*
 * File Optimizer.h
 */
#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "./../Quad.h"
#include <list>
#include <string>
#include <iostream>

class Optimizer {
public:
    Optimizer(std::list<Quad> q);
    std::list<Quad> optimizeAssignment();
private:
    std::list<Quad> quads;
};


#endif /* OPTIMIZER_H */

