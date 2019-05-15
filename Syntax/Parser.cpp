/*
 * File Parser.cpp
 */
#include <list>
#include "Parser.h"

Parser::Parser(string sourceFileName) {

    ifstream precedenceTableFile;
    precedenceTableFile.open("InputFiles/precedenceTable.txt");
    
    if(!precedenceTableFile){
        cerr << "Could not open precedenceTable.txt." << endl;
        exit(1);
    }
    
    precedenceTableFile >> numOperators;
    
    precedenceTable = new char*[numOperators];            // build table as 2D array
    for(int i = 0; i < numOperators; i++){
        precedenceTable[i] = new char[numOperators];
    }
    
    for(int i = 0; i < numOperators; i++){                  // populate table
        for(int j = 0; j < numOperators; j++){
            precedenceTableFile >> precedenceTable[i][j];            
        }
    }
    
    precedenceTableFile.close();
    
    scanner = new Scanner(sourceFileName);

    quadFile.open("quadFile.txt");
    if(quadFile.fail()){
        cout << "Could not open quadFile.txt" << endl;
        exit(1);
    }
    
} // end Parser constructor

/*
 * Converts a token's classification to a numeric value based on its column index 
 * in the precedence table. For use with the parseSource procedure to get precedence
 * between operators.
 */
int Parser::classToNumeric(string str){
    
    if (str == VARNAME || str == INTEGER || str == END_OF_FILE || str == TEMP){ return -1; }
    else if(str == SEMICOLON){ return 0; }
    else if(str == ASSIGN){ return 1; }
    else if(str == ADDOP){ return 2; }
    else if(str == LPAREN){ return 3; }
    else if(str == RPAREN){ return 4; }
    else if(str == MOP){ return 5; }
    else if(str == IF){ return 6; }
    else if(str == THEN){ return 7; }
    else if(str == ODD){return 8; }
    else if(str == RELOP){ return 9; }
    else if(str == LBRACE || str == PROC_LBRACE){ return 10; }
    else if(str == RBRACE || str == PROC_RBRACE){ return 11; }
    else if(str == CALL){ return 12; }
    else if(str == WHILE){ return 13; }
    else if(str == DO){ return 14; }
    else if(str == COMMA){ return 15; }
    else if(str == CLASS){ return 16; }
    else if(str == VAR){ return 17; }
    else if(str == CONST){ return 18; }
    else if(str == PROCEDURE){ return 19; }
    else if(str == READ){ return 20; }
    else if(str == WRITE){ return 21; }
    else if(str == ADDASSIGN){ return 22; }
    else if(str == MOPASSIGN){ return 23; }
  
} // end classToNumeric

/*
 * For use with displaying error messages. Returns the class associated
 * with the value of the token.
 */
string Parser::numericToClass(int num){
    
    switch(num){
        case 0: 
            return ";";
            break;
        case 1:
            return "=";
            break;
        case 2:
            return "+', '-";
            break;
        case 3:
            return "(";
            break;
        case 4:
            return ")";
            break;
        case 5:
            return "*', '/";
            break;
        case 6:
            return "IF";
            break;
        case 7: 
            return "THEN";
            break;
        case 8:
            return "ODD";
            break;
        case 9:
            return ">', '<', '!=', '<=', '>=";
            break;
        case 10:
            return "{";
            break;
        case 11: 
            return "}";
            break;
        case 12:
            return "CALL";
            break;
        case 13:
            return "WHILE";
            break;
        case 14:
            return "DO";
            break;
        case 15:
            return ",";
            break;
        case 16: 
            return "CLASS";
            break;
        case 17:
            return "VAR";
            break;
        case 18: 
            return "CONST";
            break;
        case 19: 
            return "PROCEDURE";
            break;
        case 20:
            return "READ";
            break;
        case 21:
            return "WRITE";
            break;
        case 22: 
            return "+=, -=";
            break;
        case 23:
            return "*=, /=";
            break;
    }
    
} // end numericToClass

/*
 * Gets one token at a time from the scanner and either shifts onto the PDA (stack) or pops the stack to
 * generate quads. Makes use of the parse table to determine precedence of the operators as they come in.
 */
void Parser::parseSource(){
            
    bool finished;
    Token token;
   
    // push initial semicolon into stack
    Token semiToken;
    semiToken.tokenString = ";";
    semiToken.tokenClass = SEMICOLON;
    int topColNum = 0;      // Semicolon -> precedence table column index 0
    s.push(semiToken);
    
    int choice;
    char precedence;

    token = scanner->buildToken();
    choice = classToNumeric(token.tokenClass);
    
    while(token.tokenClass != END_OF_FILE){ // Stop parsing after reaching EOF
        finished = false;
        while(!finished){
            
            if(token.tokenClass == END_OF_FILE){
                break;
            }
            else if(choice == -1) {   // Must be variable or integer, so push into stack
                
                if(token.tokenClass == INTEGER){
                    token.tokenString = "LIT" + token.tokenString;
                }
                s.push(token);

                token = scanner->buildToken();
                choice = classToNumeric(token.tokenClass);
            }
            else if(s.top().tokenClass == SEMICOLON && token.tokenClass == SEMICOLON){   // Matched semicolon, parse complete
                token = scanner->buildToken();
                choice = classToNumeric(token.tokenClass);
                finished = true;
            }
            else{ 
                // must be operator
                // get its precedence compared to top operator in stack

                precedence = precedenceTable[topColNum][choice];  
                                
                // if top operator in stack yields to incoming operator, add incoming op. to stack and make it the new "top operator"
                if(precedence == '<' || precedence == '='){
                    // Balance checking
                     if(token.tokenClass == LPAREN){
                            parenStack.push(token.tokenClass);
                            topColNum = choice;
                            s.push(token);
                            token = scanner->buildToken();
                            choice = classToNumeric(token.tokenClass);
                        }
                     else if(token.tokenClass == RPAREN){
                        if(parenStack.size() > 0){
                            parenStack.pop();
                            topColNum = choice;
                            s.push(token);
                            token = scanner->buildToken();
                            choice = classToNumeric(token.tokenClass);
                        }
                        else{
                            cout << "ERROR: No matching '(' for ')'" << endl;
                            errorRecovery(token, topColNum, choice);
                        }
                     }
                     else if(token.tokenClass == LBRACE || token.tokenClass == PROC_LBRACE){
                         bracketStack.push(token.tokenClass);
                         s.push(token);
                         topColNum = classToNumeric(token.tokenClass);

                         Quad lBrace = {token.tokenString, "?", "?", "?"};
                         quadFile << lBrace.printQuad();
                         addQuad(lBrace);
                                                  
                         // get next token
                         token = scanner->buildToken();
                         choice = classToNumeric(token.tokenClass);
                     }
                     else if(token.tokenClass == RBRACE || token.tokenClass == PROC_RBRACE){
                         
                         if(!bracketStack.empty()){ // if it wasn't empty, no error since RB has matching LB.
                             Quad rBrace;
                             if(bracketStack.top() == PROC_LBRACE){
                                rBrace = {PROC_RBRACE, "?", "?", "?"};
                                procedureQuads.push_back(rBrace);
                                isProcedureQuad = false;
                                rBrace.operation = "}";                             
                             }
                             else{
                                rBrace = {token.tokenString, "?", "?", "?"};
                                mainQuads.push_back(rBrace);
                             }
                             
                             bracketStack.pop();
                             s.pop();   // pop LBRACE
                             
                             quadFile << rBrace.printQuad();
                                                                                
                             topColNum = classToNumeric(s.top().tokenClass);    
                             token = semiToken;                                 
                             choice = 0;                                        
                         }
                         else{
                             cout << "ERROR: No matching '{' for '}'" << endl;
                             errorRecovery(token, topColNum, choice);
                         }
                           
                     }
                     else{ // Not bracket or parentheses...
                     
                        s.push(token);
                        
                        // Check special cases
                        if(token.tokenClass == IF){
                            Quad ifQuad = {token.tokenString, "?", "?", "?"};
                            quadFile << ifQuad.printQuad();
                            addQuad(ifQuad);
                        }
                        else if(token.tokenClass == WHILE){
                            whileLabel = "W" + to_string(whileLabelNum);
                            whileLabelNum++;
                            startWhile.push(whileLabel);
                            Quad whileQuad = {token.tokenString, whileLabel, "?", "?"};
                            quadFile << whileQuad.printQuad();   
                            addQuad(whileQuad);  
                        }
                        else if(token.tokenClass == THEN || token.tokenClass == DO){
                            label = "L" + to_string(labelNum);
                            fixUp.push(label);
                            labelNum++;
                            Quad thenOrDoQuad = {token.tokenString, label, "?", "?"};
                            quadFile << thenOrDoQuad.printQuad();
                            addQuad(thenOrDoQuad);   
                        }
                       
                        topColNum = choice;

                        token = scanner->buildToken();
                        choice = classToNumeric(token.tokenClass);
                     }
                }
                else if(precedence == '>'){
                    // else if top op. in stack takes precedence, scan down to find head of the handle (yields in precedence)
                      
                    queue <Token> q;
                    int opNum;
                    int poppedTerminal = -1;   // Classification value for most recently popped terminal (-1 before popping)
                    
                    while(precedence != '<'){      
                        
                        opNum = classToNumeric(s.top().tokenClass);
                        
                        if(opNum == -1){
                            if(s.top().tokenClass == TEMP){
                                // Re-use temporaries that are popped from the stack
                                tempNum = stoi(s.top().tokenString.substr(1));
                            }
                            
                            q.push(s.top()); s.pop();
                        }
                        else{
                            if(poppedTerminal == -1){
                                poppedTerminal = opNum;
                                q.push(s.top()); s.pop();
                            }
                            else{
                                precedence = precedenceTable[opNum][poppedTerminal];   
                                if(precedence != '<'){
                                    poppedTerminal = opNum;
                                    q.push(s.top()); s.pop();
                                }
                            }
                        }
                    } // end while
                    
                    // reduce the handle, place result in stack (if necessary)
                    // Using the column index ("opNum") of the operator to reduce

                    // Quad form: operator, op1, op2, op3
                    Quad quad;
                    
                    switch(poppedTerminal){
                        case 0: // Semicolon 
                            break;
                        case 1: // Assign
                        {
                            errorCheck("=", quad, q);   
                            quad.operand3 = "?";
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 2: // Addop
                        {   
                            errorCheck("ADDOP", quad, q);
                            Token temp;                     
                            temp.tokenString = "T" + to_string(tempNum);
                            tempNum++;
                            quad.operand3 = temp.tokenString;
                            temp.tokenClass = TEMP;
                            s.push(temp);
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 3: // LPAREN
                        {
                            if(q.front().tokenClass == RPAREN) q.pop(); // throw out parentheses
                                  
                            while(q.size() != 0){
                                if(classToNumeric(q.front().tokenClass) == -1){
                                    s.push(q.front());  // push nonterminal between parentheses back onto stack
                                }
                                q.pop();
                            }
                            break;
                        }
                        case 4: // RPAREN
                            break;
                        case 5: // MOP
                        {
                            errorCheck("MOP", quad, q);
                            Token temp;                     
                            temp.tokenString = "T" + to_string(tempNum);
                            tempNum++;
                            temp.tokenClass = TEMP;
                            quad.operand3 = temp.tokenString;
                            s.push(temp);

                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 6: // IF  
                        {                               
                            quad.operation = fixUp.top(); fixUp.pop();
                            quad.operand1 = "?";
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 7: // THEN
                            break;
                        
                        case 8: // ODD
                        {
                            if(q.front().tokenClass == VARNAME || q.front().tokenClass == INTEGER){
                                quad.operand1 = q.front().tokenString; q.pop();
                            }
                            else{
                                cout << "Missing operand after 'ODD'." << endl;
                                quad.operand1 = "ERROR";
                                noErrors = false;
                            }
                            
                            quad.operation = q.front().tokenString; q.pop();
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 9: // RELOP
                        {
                            errorCheck("RELOP", quad, q);
                            quad.operand3 = "?";
                        
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 10: // LBRACE
                            break;
                        case 11: // RBRACE
                            break;
                        case 12: // CALL
                        {
                            if(q.front().tokenClass == RPAREN){         
                                while(q.front().tokenClass != LPAREN){
                                    q.pop();
                                }
                                q.pop();
                            }
                            if(q.front().tokenClass == VARNAME){
                                quad.operand1 = q.front().tokenString; q.pop();
                            }
                            else{
                                noErrors = false;
                                cout << "Missing procedure identifier after 'CALL'." << endl;
                                quad.operand1 = "ERROR";
                            }
                            quad.operation = q.front().tokenString; q.pop();
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 13: // WHILE
                        {
                            quad.operation = startWhile.top(); startWhile.pop();
                            quad.operand1 = "?";
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            quadFile << quad.printQuad();
                            
                            addQuad(quad);
                            
                            quad.operation = fixUp.top(); fixUp.pop();
                            quadFile << quad.printQuad();
                            addQuad(quad);
           
                            break;
                        }
                        case 14: // DO         
                            break;
                        case 15: // COMMA
                            break;
                        case 16: // CLASS
                        {
                            if(q.front().tokenClass == VARNAME){
                                quad.operand1 = q.front().tokenString; q.pop();
                            }
                            else{
                                noErrors = false;
                                cout << "Missing class identifier." << endl;
                                quad.operand1 = "ERROR";
                            }
                            
                            quad.operation = q.front().tokenString; q.pop();
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 17: // VAR                       
                        {
                            int nextState = 0;
                            int index; 
                            bool varChecking = true;
                            
                            while(varChecking){
                                if(q.size() > 0){
                                    if(q.front().tokenClass == VARNAME) index = 0;
                                    else if(q.front().tokenClass == COMMA) index = 1;
                                    else if(q.front().tokenClass == VAR) index = 2;
                                    else{
                                        noErrors = false;
                                        cout << "Unexpected token " << q.front().tokenString << " in 'VAR' statement." << endl;
                                        break;
                                    }  
                                    
                                }
                                
                                switch(nextState){
                                    case 0: // Start state
                                        if(index != 0){
                                            cout << "Missing variable in 'VAR' statement." << endl;
                                            varChecking = false;
                                            noErrors = false;
                                        }
                                        else{
                                            nextState = varStates[0][index];
                                        }
                                        break;
                                    case 1:
                                        if(index != 1 && index != 2){
                                            cout << "Unexpected token " << q.front().tokenString << " in 'VAR' "
                                                 << "statement, missing ','." << endl;
                                            varChecking = false;
                                            noErrors = false;
                                        }
                                        else{
                                            nextState = varStates[1][index];
                                        }
                                        break;
                                    case 2:
                                        if(index != 0){
                                            cout << "Missing variable in 'VAR' statement." << endl;
                                            varChecking = false;
                                            noErrors = false;
                                        }
                                        else{
                                            nextState = varStates[2][index];
                                        }
                                        break;
                                    case 3:
                                        varChecking = false;
                                        break;
                                } // end switch
                                
                                if(q.size() > 0) q.pop();
                                
                            } // end while 
                            
                            break;
                        }
                        case 18: // CONST
                        {
                            int nextState = 0;
                            int index; 
                            bool constChecking = true;
                            
                            while(constChecking){
                                if(q.size() > 0){
                                    if(q.front().tokenClass == INTEGER) index = 0;
                                    else if(q.front().tokenClass == ASSIGN) index = 1;
                                    else if(q.front().tokenClass == VARNAME) index = 2;
                                    else if(q.front().tokenClass == COMMA) index = 3;
                                    else if(q.front().tokenClass == CONST) index = 4;
                                    else{
                                        noErrors = false;
                                        cout << "Unexpected token " << q.front().tokenString << " in 'CONST' statement." << endl;
                                        break;  
                                    }
                                    
                                }
                                
                                switch(nextState){
                                    case 0: // start state
                                        if(index != 0){
                                           cout << "Missing integer value in 'CONST' statement." << endl;
                                           constChecking = false;
                                           noErrors = false;
                                        }
                                        else{
                                            nextState = constStates[0][index];
                                        }
                                        break;
                                    case 1:
                                        if(index != 1){
                                            cout << "Missing '=' in 'CONST' statement." << endl;
                                            constChecking = false;
                                            noErrors = false;
                                        }
                                        else{
                                            nextState = constStates[1][index];
                                        }
                                        break;
                                    case 2:
                                        if(index != 2){
                                            cout << "Missing variable in 'CONST' statement." << endl;
                                            constChecking = false;
                                            noErrors = false;
                                        }
                                        else{
                                            nextState = constStates[2][index];
                                        }
                                        break;
                                    case 3:
                                        if(index != 3 && index != 4){
                                            cout << "Unexpected token " << q.front().tokenString << " in 'CONST' "
                                                 << "statement." << endl;
                                            constChecking = false;
                                            noErrors = false;
                                        }
                                        else{
                                            nextState = constStates[3][index];
                                        }
                                        break;
                                    case 4:
                                        if(index != 0){
                                           cout << "Missing integer value in 'CONST' statement." << endl;
                                           constChecking = false;
                                           noErrors = false;
                                        }
                                        else{
                                            nextState = constStates[4][index];
                                        }
                                        break;
                                    case 5:
                                        constChecking = false;
                                        break;
                                } // end switch
                                
                                if(q.size() > 0) q.pop();
                                
                            } // end while
                            break;
                        }
                        case 19: // PROCEDURE
                        {
                            isProcedureQuad = true;
                            token.tokenClass = PROC_LBRACE;
                                                                                
                            if(q.front().tokenClass == RPAREN){         
                                while(q.front().tokenClass != LPAREN){
                                    q.pop();
                                }
                                q.pop();
                            }
                            if(q.front().tokenClass == VARNAME){
                                quad.operand1 = q.front().tokenString; q.pop();     // Proc name
                            }
                            else{
                                noErrors = false;
                                cout << "Missing procedure identifier." << endl;
                                quad.operand1 = "ERROR";
                            }
                            quad.operation = q.front().tokenString; q.pop();    // "PROCEDURE"
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 20: // READ
                        {
                            if(q.front().tokenClass == VARNAME){
                                quad.operand1 = q.front().tokenString; q.pop();
                            }
                            else{
                                cout << "Missing variable operand after 'READ'." << endl;
                                quad.operand1 = "ERROR";
                                noErrors = false;
                            }
                            
                            quad.operation = q.front().tokenString; q.pop();
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 21: // WRITE
                        {
                            if(q.front().tokenClass == VARNAME || q.front().tokenClass == INTEGER){
                                quad.operand1 = q.front().tokenString; q.pop();
                            }
                            else{
                                cout << "Missing variable or integer operand after 'WRITE'." << endl;
                                quad.operand1 = "ERROR";
                                noErrors = false;
                            }
                            
                            quad.operation = q.front().tokenString; q.pop();
                            quad.operand2 = "?";
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 22: // ADDASSIGN                                   
                        {
                            errorCheck("ADDASSIGN", quad, q);
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }
                        case 23: // MOPASSIGN                                   
                        {
                            errorCheck("MOPASSIGN", quad, q);
                            quad.operand3 = "?";
                            
                            quadFile << quad.printQuad();
                            addQuad(quad);
                            break;
                        }

                    } // end switch
                    
                    // Finished reduction, set new top operator of stack
                    topColNum = opNum;   
                    
                }
                else{
                    // ERROR: Operators have no precedence relationship ("X" in table).
                    string topString = numericToClass(topColNum);
                    cout << "Error: After " << topString << " found " << token.tokenString << ", expected ";
                    string operatorString;
                    // loop through acceptable tokens to appear after "token"
                    for(int i = 0; i < numOperators; i++){
                        if(precedenceTable[topColNum][i] != 'X'){
                            operatorString = numericToClass(i);
                            cout << "'" << operatorString << "', ";
                        }
                    } // end for
                    
                    token = errorRecovery(token, topColNum, choice);
                    
                } // end if

            } // end if

        } // end while
    
    } // end while
    
    if(bracketStack.size() > 0){    // Check if any leftover braces in stack
        noErrors = false;
        cout << "No matching '}' for '{'." << endl;
    }
    
    if(noErrors){
        scanner->buildSymbolTable();
        Quad eofQuad = {"EOF" , "?", "?", "?"};
        mainQuads.push_back(eofQuad);
        
        if(procedureQuads.size() > 0){  // Append any procedure quads
            mainQuads.splice(mainQuads.end(), procedureQuads);
        }
        
        if(variableExistenceCheck(mainQuads, scanner->symbolTable)){
            if(assignmentCheck(mainQuads, scanner->symbolTable)){
                Optimizer opt(mainQuads);
                mainQuads = opt.optimizeAssignment();
                
                CodeGenerator cg(mainQuads, scanner->symbolTable);
                cg.generateCode();  
            }
            else{ // assignmentCheck fails
                cout << "CONST reassignment attempt(s) must be resolved before code generation." << endl;
            }
        }
        else{ // variableExistenceCheck fails
            cout << "Undeclared variable(s) must be resolved before code generation." << endl;
        } 
    }
    else{ // Syntax analysis found errors
        cout << "Completed parse with errors." << endl;
    }
       
} // end parseSource

/*
 * For use with operators that come in between two variables/integers. Checks that
 * both sides are present and are of valid classification. 
 */
void Parser::errorCheck(string oper, Quad &quad, queue <Token> &q){
    
    if(q.front().tokenClass == INTEGER || q.front().tokenClass == VARNAME || q.front().tokenClass == TEMP){
        quad.operand2 = q.front().tokenString; q.pop();
    }
    else{
        noErrors = false;
        cout << "Missing operand after '"<< oper << "'" << endl;
        quad.operand2 = "ERROR";
    }

    quad.operation = q.front().tokenString; q.pop();

    if(q.size() > 0){
       if(q.front().tokenClass == INTEGER || q.front().tokenClass == VARNAME || q.front().tokenClass == TEMP){
            quad.operand1 = q.front().tokenString; q.pop();
       }
    }
    else{
        noErrors = false; 
        cout << "Missing operand before '" << oper << "'"<< endl;
        quad.operand1 = "ERROR";
    } 
} // end errorCheck

/*
 * Skips to next line of code by calling for tokens from scanner until a delimiter or EOF is reached.
 * The invocation of this procedure implies an error has been discovered, so code generation is
 * prevented.
 */
Token Parser::errorRecovery(Token currentInput, int &topColNum, int &choice){   
    noErrors = false; // prevent code generation
    cout << "SKIPPING TO NEXT LINE..." << endl;
    
    while(currentInput.tokenClass != SEMICOLON && currentInput.tokenClass != END_OF_FILE 
            && currentInput.tokenClass != RBRACE && currentInput.tokenClass != LBRACE
            && currentInput.tokenClass != PROC_LBRACE){  
        currentInput = scanner->buildToken();
    }
    
    while(s.top().tokenClass != SEMICOLON && s.size() != 1){
        if(s.top().tokenClass == LBRACE || s.top().tokenClass == PROC_LBRACE) break;
        s.pop();
    }
   
    topColNum = classToNumeric(s.top().tokenClass);     // Set new top of stack
    choice = classToNumeric(currentInput.tokenClass);   // Set next token for input
    return currentInput;
} // end errorRecovery

/*
 * Accepts the quad to add to the appropriate quad container (either
 * a quad belonging to a procedure or the main program).
 */
void Parser::addQuad(Quad quad){
    if(isProcedureQuad){
        procedureQuads.push_back(quad);
    }
    else{
        mainQuads.push_back(quad);
    }
} // end addQuad

/*
 * Checks that all variables and literals that appear in the quads are present in
 * the symbol table. Symbols that appear in the quads but were not declared are 
 * displayed if found.
 */
bool Parser::variableExistenceCheck(list <Quad> quads, LinkedList* table){
    bool noUndeclaredVars = true;
    
    while(!quads.empty()){
        if(quads.front().operation != "WHILE" && quads.front().operation != "DO"
                && quads.front().operation.at(0) != 'L' && quads.front().operation != "THEN"){  
            if(quads.front().operand1 != "?"){                  
                if(!table->isInList(quads.front().operand1)){
                    noUndeclaredVars = false;
                    cout << "Found undeclared variable " << quads.front().operand1 << endl;
                }
            }

            if(quads.front().operand2 != "?"){
                if(!table->isInList(quads.front().operand2)){
                    noUndeclaredVars = false;
                    cout << "Found undeclared variable " << quads.front().operand2 << endl;
                } 
            }

            if(quads.front().operand3 != "?"){
                if(!table->isInList(quads.front().operand3)){
                    noUndeclaredVars = false;
                    cout << "Found undeclared variable " << quads.front().operand3 << endl;
                } 
            }
        } // end if
        
        quads.pop_front();
    } // end while
    
    return noUndeclaredVars;
} // end variableExistenceCheck

/*
 * Looks at all quads with the assignment ("=") operation and determines if the
 * statement illegally attempts to assign a value to a constant.
 */
bool Parser::assignmentCheck(list<Quad> quads, LinkedList* table){
    
    string oper;
    bool assignCheck = true;
    while(!quads.empty()){
        
        oper = quads.front().operation;
        
        if(oper == "=" || oper == "+=" || oper == "-=" || oper == "*=" || oper == "/="){
            
            if(table->getSymbolClass(quads.front().operand1) == CONST){
                cout << "CONST variable " << quads.front().operand1 << " cannot be on the left side of "
                        << "an assignment statement." << endl;
                assignCheck = false;
            }
      
        }
        quads.pop_front();
    }
    return assignCheck;
} // end assignmentCheck