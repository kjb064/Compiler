/*
 * File CodeGenerator.cpp
 */
#include <list>

#include "CodeGenerator.h"
using namespace std;

CodeGenerator::CodeGenerator(list <Quad> q, LinkedList *table) {  
    quadQueue = q;
    symbolTable = table;
    string fileName = "CodeGenerator/AssemblyFiles/";     
    
    // Using symbol table, uses name of program/class for output .asm file
    if(symbolTable->head->token.tokenClass == "<ProgramName>"){
        fileName += symbolTable->head->token.tokenString;
    }
    fileName += ".asm";
    outputCode.open(fileName);              
    
    if(!outputCode.is_open()){
        cout << "Failed to open file " << fileName << ". Terminating..." << endl;
        exit(0);
    }
    
    createDataSection();
    createBssSection();
}

/*
 * Prints the .data section of the assembly program using an input file containing
 * the declarations needed for I/O and uses the symbol table to print any constant 
 * values.
 */
void CodeGenerator::createDataSection(){
    ifstream partialDataSection;
    partialDataSection.open("InputFiles/partialDataSection.txt");
    if(partialDataSection.fail()){
        cout << "Could not find file partialDataSection.txt." << endl;
        exit(0);
    }
    string line;
    while(getline(partialDataSection, line)){
        outputCode << line << "\n";
    }
    
    Node *node = symbolTable->head;
    while(node){
        
        if(node->token.tokenClass == "$CONST"){
            outputCode << "\t" << node->token.tokenString << "\tDW " << node->token.value << endl;
        }
        node = node->next;
    }
} // end createDataSection

/*
 * Creates the .bss section using an input file containing the declarations
 * necessary for I/O as well as the symbol table to print variables and temporaries. 
 */
void CodeGenerator::createBssSection(){
    ifstream partialBssSection;
    partialBssSection.open("InputFiles/partialBssSection.txt");
    if(partialBssSection.fail()){
        cout << "Could not find file partialBssSection.txt." << endl;
        exit(0);
    }
    outputCode << "\n";
    string line;
    while(getline(partialBssSection, line)){
        outputCode << line << endl;
    }
    
    Node *node = symbolTable->head;
    while(node){
        if(node->token.tokenClass == "<var>" || node->token.tokenClass == "<temp>"){
            outputCode << "\t" << node->token.tokenString << "\tRESW 1" << endl;
        }
        node = node->next;
    }
} // createBssSection

/*
 * Uses the I/O Routines input file to paste the I/O procedures at the end of each
 * assembly file. This is called only when all quads have finished being processed.
 */
void CodeGenerator::createIORoutines(){
    ifstream routinesFile;
    routinesFile.open("InputFiles/IORoutines.txt");
    if(routinesFile.fail()){
        cout << "Could not open IORoutines.txt." << endl;
        exit(0);
    }
    string line;
    while(getline(routinesFile, line)){
        outputCode << line << endl;
    }
} // end createIORoutines

/*
 * Using the operation of the quad parameter, produces a numeric value associated
 * with all possible operations supported by Java 0. For use with the generateCode
 * procedure to locate the appropriate case in the case structure.
 */
int CodeGenerator::quadToNumeric(Quad quad){
    if(quad.operation == "+"){ return 0; }
    else if(quad.operation == "-"){ return 1; }
    else if(quad.operation == "*"){ return 2; }
    else if(quad.operation == "/"){ return 3; }
    else if(quad.operation == "="){ return 4; }
    else if(quad.operation == "IF"){ return 5; }
    else if(quad.operation == "THEN"){ return 6; }
    // (Below) For labels of form "L#". Used for "IF" stmts.
    else if(quad.operation.at(0) == 'L' && isdigit(quad.operation.at(1)) ){ return 7; }  
    else if(quad.operation == "WHILE"){ return 8; }
    else if(quad.operation == "DO"){ return 9; }
    // (Below) For labels of form "W#". Used for "WHILE" stmts.
    else if(quad.operation.at(0) == 'W' && isdigit(quad.operation.at(1)) ){ return 10; }  
    else if(quad.operation == "CLASS"){ return 11; }
    else if(quad.operation == "PROCEDURE"){ return 12; }
    else if(quad.operation == "+="){ return 13; }
    else if(quad.operation == "-="){ return 14; }
    else if(quad.operation == "*="){ return 15; }
    else if(quad.operation == "/="){ return 16; }
    else if(quad.operation == ">"){ return 17; }
    else if(quad.operation == "<"){ return 18; }
    else if(quad.operation == ">="){ return 19; }
    else if(quad.operation == "<="){ return 20; }
    else if(quad.operation == "!="){ return 21; }
    else if(quad.operation == "=="){ return 22; }
    else if(quad.operation == "CALL"){ return 23; }
    else if(quad.operation == "ODD"){ return 24; }
    else if(quad.operation == "READ"){ return 25; }
    else if(quad.operation == "WRITE"){ return 26; }
    else if(quad.operation == "PROC_LBRACE"){ return 27; }
    else if(quad.operation == "PROC_RBRACE"){ return 28; }
    else if(quad.operation == "EOF"){ return 29; }
    else { return -1; }
} // end quadToNumeric

/*
 * Uses the quads supplied by the Parser to generate the assembly equivalent
 * of the source program. Optimization (folding) will occur for quads that make
 * use of literal values. 
 */
void CodeGenerator::generateCode(){
    outputCode << "\nsection .txt\n\n";
    int choice;
    string LIT = "LIT"; // Used to help identify literal values
    while(quadQueue.size() != 0){
        
        choice = quadToNumeric(quadQueue.front());
        
        switch(choice){
            case 0: // +
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                   outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n"; 
                }
		
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tadd ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tadd ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tmov [" << quadQueue.front().operand3 << "], ax\n\n";
                break;
            case 1: // - 
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                   outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n"; 
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tsub ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tsub ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tmov [" << quadQueue.front().operand3 << "], ax\n\n";
                break;
            case 2: // *                                                               
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                   outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n"; 
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tmov bx, " << quadQueue.front().operand2.substr(3) << "\n" 
                               << "\tmul word bx\n";
                }
                else{
                    outputCode << "\tmul word ["<< quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tmov [" << quadQueue.front().operand3 << "], ax\n\n";
                break;
            case 3: // /
                outputCode << "\tmov dx, 0\n";
                
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tmov bx, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tmov bx, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tdiv bx\n"
                           << "\tmov ["     << quadQueue.front().operand3 << "], ax\n\n";
                break;
            case 4: // =
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tmov word [" << quadQueue.front().operand1 << "], " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand2 << "]\n"
                               << "\tmov ["     << quadQueue.front().operand1 << "], ax\n\n";  
                }     
                break;
            case 5: // IF
                break;
            case 6: // THEN
                outputCode << quadQueue.front().operand1 << "\n\n";
                break;
            case 7: // L# Labels
                outputCode << quadQueue.front().operation << ":\tnop\n";
                break;
            case 8: // WHILE
                outputCode << quadQueue.front().operand1 << ":\tnop\n";
                break;
            case 9: // DO
                outputCode << quadQueue.front().operand1 << "\n";
                break;
            case 10: // W# Labels
                outputCode << "\tjmp " << quadQueue.front().operation << "\n";
                break;
            case 11: // CLASS                                                       
                outputCode << "   global main\nmain:";
                break;
            case 12: // PROCEDURE
                outputCode << quadQueue.front().operand1 << ":\tnop\n";
                break;
            case 13: // +=
                outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tadd ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tadd ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tmov [" << quadQueue.front().operand1 << "], ax\n\n";
                break;
            case 14: // -=
                
                outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tsub ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tsub ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tmov [" << quadQueue.front().operand1 << "], ax\n\n";
                break;
            case 15: // *=                                              
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                   outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n"; 
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tmov bx, " << quadQueue.front().operand2.substr(3) << "\n"
                               << "\tmul bx\n";
                }
                else{
                    outputCode << "\tmul word ["<< quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tmov [" << quadQueue.front().operand1 << "], ax\n\n";
                break;
            case 16: // /=
                outputCode << "\tmov dx, 0\n";
                
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tmov bx, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tmov bx, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tdiv bx\n"
                           << "\tmov [" << quadQueue.front().operand1 << "], ax\n\n";       
                break;
            case 17: // >
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tcmp ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tcmp ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tjle ";
                break;
            case 18: // <
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tcmp ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tcmp ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tjge ";
                break;
            case 19: // >=
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tcmp ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tcmp ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tjl ";
                break;
            case 20: // <=
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tcmp ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tcmp ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tjg ";
                break;
            case 21: // !=
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tcmp ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tcmp ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tje ";
                break;
            case 22: // ==
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                if(quadQueue.front().operand2.compare(0, 3, LIT) == 0){
                    outputCode << "\tcmp ax, " << quadQueue.front().operand2.substr(3) << "\n";
                }
                else{
                    outputCode << "\tcmp ax, [" << quadQueue.front().operand2 << "]\n";
                }
                
                outputCode << "\tjne ";
                break;
            case 23: // CALL
                outputCode << "\tcall " << quadQueue.front().operand1 << "\n";
                break;
            case 24: // ODD
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                    outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n";
                }
                
                outputCode << "\ttest al, 1\n"
                           << "\tjz ";   // Jump if zero (indicates EVEN number)
                break;
            case 25: // READ
                outputCode << "\tcall PrintString\n"
                           << "\tcall GetAnInteger\n"
                           << "\tmov ax, [ReadInt]\n"
                           << "\tmov [" << quadQueue.front().operand1 << "], ax\n\n";
                break;
            case 26: // WRITE
                if(quadQueue.front().operand1.compare(0, 3, LIT) == 0){
	            outputCode << "\tmov ax, " << quadQueue.front().operand1.substr(3) << "\n";
		}
                else{
                   outputCode << "\tmov ax, [" << quadQueue.front().operand1 << "]\n"; 
                }
                
                outputCode << "\tcall ConvertIntegerToString\n\n"
                           << "\tmov eax, 4\n"
                           << "\tmov ebx, 1\n"
                           << "\tmov ecx, Result\n"
                           << "\tmov edx, ResultEnd\n"
                           << "\tint 80h\n\n"; 
                break;
            case 27: // PROC_LBRACE
                break;
            case 28: // PROC_RBRACE
                outputCode << "\tret\n";    
                break;
            case 29: // EOF
                outputCode << "fini:\n"
                           << "\tmov eax, sys_exit\n"
                           << "\txor ebx, ebx\n"
                           << "\tint 80h\n\n";
                break;
                
        } // end switch
        quadQueue.pop_front();
        
    } // end while
      
    createIORoutines();
    
    cout << "CODE GENERATION COMPLETE" << endl;
} // end generateCode


