/*
 * File: Scanner.cpp
 */
#include "Scanner.h"

Scanner::Scanner(string sourceFileName){

    sourceFile.open(sourceFileName.c_str(), fstream::in);
    if(sourceFile.fail()){
        cerr << "Could not open file " << sourceFileName << ", terminating compilation..." << endl;
        exit(0);
    }
    
    tokenFile.open("tokenList.txt"); 

    ifstream inFile;
    inFile.open("InputFiles/scannerDFSA.txt");    // contains decision table for scanner

    if(!inFile){
        cerr << "Could not open scannerDFSA.txt." << endl;
        exit(0);
    }

    inFile >> rows;
    inFile >> cols;

    scannerTable = new int*[rows];            // build table as 2D array
    for(int i = 0; i < rows; i++){
        scannerTable[i] = new int[cols];
    }

    for(int i = 0; i < rows; i++){          // populate table
        for(int j = 0; j < cols; j++){
            inFile >> scannerTable[i][j];
        }
    }
    inFile.close();

    // CREATE 2nd 2D ARRAY FOR OTHER DEC.TABLE
    inFile.open("InputFiles/firstPassDFSA.txt");

    if(!inFile){
        cerr << "Could not open firstPassDFSA.txt." << endl;
        exit(0);
    }

    inFile >> rows;
    inFile >> cols;

    firstPassTable = new int*[rows];
    for(int i = 0; i < rows; i++){
        firstPassTable[i] = new int[cols];
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            inFile >> firstPassTable[i][j];
        }
    }
    
    inFile.close();
    
} // end Scanner constructor

/*
 * Looks up if the string associated with the passed "token" matches any of the 
 * reserved words. If there's a match, returns the index of the reserved word from 
 * the lookup table "reservedWords". If no match, return -1.
 */
int Scanner::isReservedWord(Token token){
    for(int i = 0; i < numReservedWords; i++){
        if(reservedWords[i].tokenString == token.tokenString){
            return i;
        }
    }
    return -1;
}

/*
 * Converts a recognized character to a numeric value consistent with the index
 * of the corresponding column in the scanner decision table. Unrecognized characters
 * result in the return of the index of the last column which will lead to an error
 * message. Column indeces begin with 0. 
 */
int Scanner::charToNumeric(char ch){
    if(isalpha(ch)){ return 0; }        // L
    else if(isdigit(ch)){ return 1; }   // D
    else if(ch == '*'){ return 2; }     // *
    else if(ch == '/'){ return 3; }     // /
    else if(ch == '='){ return 4; }     // =
    else if(ch == '<'){ return 5; }     // <
    else if(isspace(ch)){ return 6; }   // whitespace
    else if(ch == '{'){ return 7; }     // {
    else if(ch == '}'){ return 8; }     // }
    else if(ch == '+'){ return 9; }     // +
    else if(ch == '-'){ return 10; }    // -
    else if(ch == ','){ return 11; }    // ,
    else if(ch == ';'){ return 12; }    // ;
    else if(ch == '\0'){ return 13; }   // Designates EOF
    else if(ch == '>'){ return 14; }    // >
    else if(ch == '('){ return 15; }    // (
    else if(ch == ')'){ return 16; }    // )
    else if(ch == '!'){ return 17; }    // !
    else { return 18; }                 // other
}

/*
 * Gets the next available character from the source code file. If at the end of
 * the file, returns the value of '\0' for the character. 
 */
char Scanner::getNextChar(){
    char character;
    if(sourceFile.peek() == EOF){
        character = '\0';
    }
    else{
        sourceFile >> noskipws >> character;
    }

    return character;
}

/*
 * Converts a recognized token's class to a numeric value consistent with the index
 * of the corresponding column in the "first pass" decision table. Column indeces begin with 0.  
 */
int Scanner::tokenToNumeric(Token token){
    if(token.tokenClass == CLASS){ return 0; }
    else if(token.tokenClass == VARNAME){ return 1; }
    else if(token.tokenClass == LBRACE){ return 2; }
    else if(token.tokenClass == RBRACE){ return 3; }
    else if(token.tokenClass == SEMICOLON){ return 4; }
    else if(token.tokenClass == CONST){ return 5; }
    else if(token.tokenClass == VAR){ return 6; }
    else if(token.tokenClass == ASSIGN){ return 7; }
    else if(token.tokenClass == INTEGER){ return 8; }
    else if(token.tokenClass == COMMA){ return 9; }
    else if(token.tokenClass == END_OF_FILE){ return 10; }         
    else if(isReservedWord(token) != -1 && token.tokenClass != PROCEDURE){ return 11; } // Handles non-PROCEDURE reserved words
    else if(token.tokenClass == ADDOP || token.tokenClass == MOP){ return 12; }
    else if(token.tokenClass == PROCEDURE){ return 13; }
    else if(token.tokenClass == LPAREN){ return 14; }
    else if(token.tokenClass == RPAREN){ return 15; }
}

/*
 * Uses the scanner DFSA to build tokens one character at a time. Returns a token
 * indicating end of file when all tokens have been created. 
 */
Token Scanner::buildToken(){
    if(sourceFile.tellg() == 0){ // Set lastChar to empty if file pointer is at start of file
        lastChar = ' ';
    }
    Token newToken;
    newToken.tokenString = "";
    char character = lastChar;

    bool finished = false; 
    int nextState = 0;
    int choice;

    while(!finished){
        switch(nextState){
            case 0:
                choice = charToNumeric(character);
                nextState = scannerTable[0][choice];
                if(nextState == 0){ character = getNextChar(); }
                break;
            case 1:
                cerr << "Error, illegal character " << character << " in input!" << endl;
                exit(0);
                break;
            case 2:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[2][choice];
                break;
            case 3:
                if(isdigit(character)){
                    newToken.tokenString += character;
                }

                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[3][choice];
                break;
            case 4:
                /*<int>*/
                newToken.tokenClass = INTEGER;
                finished = true;
                break;
            case 5:
                if(isalnum(character)){
                    newToken.tokenString += character;
                }

                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[5][choice];
                break;
            case 6:
            {
                /*<var>*/
                int reserveIndex = isReservedWord(newToken);
                if(reserveIndex != -1){
                    newToken = reservedWords[reserveIndex];             
                }
                else{
                    newToken.tokenClass = VARNAME;
                }

                finished = true;
                break;
            }
            case 7:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[7][choice];
                break;
            case 8:
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[8][choice]; 
                break;
            case 9:
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[9][choice];
                if(nextState == 0) {            // Prepare to return to state 0 when comment complete
                    newToken.tokenString = "";  // Reset string
                    character = getNextChar();
                }

                break;
            case 10:
                /*<mop> (divide) */
                newToken.tokenClass = MOP;
                finished = true;
                break;
            case 11:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[11][choice];
                break;
            case 12:
                /*<assignment>*/
                newToken.tokenClass = ASSIGN;
                finished = true;
                break;
            case 13:
                /*<relop> (==)*/                        
                newToken.tokenString += character;
                newToken.tokenClass = RELOP;
                character = getNextChar();
                finished = true;
                break;
            case 14:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[14][choice];
                break;
            case 15:
                /*<relop> (<)*/                         
                newToken.tokenClass = RELOP;
                finished = true;
                break;
            case 16:
                /*<relop> (<=)*/                          
                newToken.tokenString += character;
                newToken.tokenClass = RELOP;
                character = getNextChar();
                finished = true;
                break;
            case 17:
                /*LB ({)*/
                newToken.tokenString += character;
                newToken.tokenClass = LBRACE;
                character = getNextChar();
                finished = true;
                break;
            case 18:
                /*RB (})*/
                newToken.tokenString += character;
                newToken.tokenClass = RBRACE;
                character = getNextChar();
                finished = true;
                break;
            case 19:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[19][choice];
                break;
            case 20:
                /*<addop> (+)*/
                newToken.tokenClass = ADDOP;
                finished = true;
                break;
            case 21:
                /*<addAssign> (+=)*/                        
                newToken.tokenString += character;
                newToken.tokenClass = ADDASSIGN;
                character = getNextChar();
                finished = true;
                break;
            case 22:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[22][choice];
                break;
            case 23:
                /*<addop> (-)*/
                newToken.tokenClass = ADDOP;
                finished = true;
                break;
            case 24:
                /*<addAssign> (-=)*/                
                newToken.tokenString += character;
                newToken.tokenClass = ADDASSIGN;
                character = getNextChar();
                finished = true;
                break;
            case 25:
                /*<comma> (,)*/
                newToken.tokenString += character;
                newToken.tokenClass = COMMA;
                character = getNextChar();
                finished = true;
                break;
            case 26:
                /*<semicolon>*/
                newToken.tokenString += character;
                newToken.tokenClass = SEMICOLON;
                character = getNextChar();
                finished = true;
                break;
            case 27:
                /*EOF (represented by '\0')*/
                newToken.tokenClass = END_OF_FILE;
                finished = true;
                break;
            case 28:
                /*<mop> (*)*/
                newToken.tokenClass = MOP;     
                finished = true;
                break;
            case 29:
                /*<mopAssign> (*=)*/
                newToken.tokenString += character;
                newToken.tokenClass = MOPASSIGN;
                character = getNextChar();
                finished = true;
                break;
            case 30:
                /*<mopAssign> (/=)*/
                newToken.tokenString += character;
                newToken.tokenClass = MOPASSIGN;
                character = getNextChar();
                finished = true;
                break;
            case 31:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[31][choice];
                break;
            case 32:
                /*<relop> (>)*/                         
                newToken.tokenClass = RELOP;
                finished = true;
                break;
            case 33:
                /*<relop> (>=)*/                          
                newToken.tokenString += character;
                newToken.tokenClass = RELOP;
                character = getNextChar();
                finished = true;
                break;
            case 34:
                /* LP ( */
                newToken.tokenString += character;
                newToken.tokenClass = LPAREN;
                character = getNextChar();
                finished = true;
                break; 
            case 35:
                /* RP ) */
                newToken.tokenString += character;
                newToken.tokenClass = RPAREN;
                character = getNextChar();
                finished = true;
                break;
            case 36:
                newToken.tokenString += character;
                character = getNextChar();
                choice = charToNumeric(character);
                nextState = scannerTable[36][choice];
                break;
            case 37:
                /* <NOT>, ! */
                newToken.tokenClass = NOT;
                finished = true;
                break;
            case 38:
                /* != , <relop> */
                newToken.tokenString += character;
                newToken.tokenClass = RELOP;
                character = getNextChar();
                choice = charToNumeric(character);
                finished = true;
                break;
        } // end switch

    } // end while
    
    lastChar = character;
    return newToken;
    
} // end buildToken

/*
 * Using the "first pass" DFSA, creates the symbol table using a linked list. 
 * Not all tokens are added to the table (this is instead the purpose of the 
 * token list, which is also created here).
 */
void Scanner::buildSymbolTable(){
    
    // Set file pointer to beginning of file
    resetFilePointer();
    
    symbolTable = new LinkedList("symbolTable.txt");
    Token newToken;
    int choice;
    int dataSegAddress = 0;
    int codeSegAddress = 0;
    
    bool finished = false;
    int nextState = 0;
    while(!finished){

        switch(nextState){
            case 0:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[0][choice];
                break;
            case 1:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[1][choice];
                break;
            case 2:
                newToken.tokenClass = PROGRAMNAME;
                newToken.address = codeSegAddress;
                codeSegAddress += 2;
                newToken.segment = CS;
                symbolTable->add(newToken);
                
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[2][choice];
                break;
            case 3:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[3][choice];
                break;
            case 4:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[4][choice];
                break;
            case 5:
                /* Add constant to symbol table */
                newToken.tokenClass = CONST;
                newToken.segment = DS;
                newToken.address = dataSegAddress;
                dataSegAddress += 2;
                symbolTable->add(newToken);
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[5][choice];
                break;
            case 6:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[6][choice];
                break;
            case 7:
                // Assign the value to the CONST variable
                symbolTable->last->token.value = newToken.tokenString; 
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[7][choice];
                break;
            case 8:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[8][choice];
                break;
            case 9:
                /* Add variable to symbol table */
                if(!symbolTable->isInList(newToken.tokenString)){
                    newToken.segment = DS;
                    newToken.address = dataSegAddress;
                    newToken.value = "?";
                    dataSegAddress += 2;
                    symbolTable->add(newToken);
                }
                
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[9][choice];
                break;
            case 10:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[10][choice];
                break;
            case 11:
                /* Add integer to symbol table */
                newToken.tokenString = "LIT" + newToken.tokenString;
                if(!symbolTable->isInList(newToken.tokenString)){
                    newToken.segment = DS;
                    newToken.address = dataSegAddress;
                    dataSegAddress += 2;
                    newToken.value = newToken.tokenString.substr(3);
                    symbolTable->add(newToken);
                }
                
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[11][choice];
                break;
            case 12:
                /* Reached EOF (End pass 1) */
                finished = true;                                
                break;
            case 13:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[13][choice];
                break;
            case 14:
                /* Add procedure to symbol table */
                if(!symbolTable->isInList(newToken.tokenString)){
                    newToken.tokenClass = PROCEDURENAME;
                    newToken.segment = CS;
                    newToken.address = codeSegAddress;
                    codeSegAddress += 2;
                    symbolTable->add(newToken);
                }
                
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[14][choice];
                break;
            case 15:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[15][choice];
                break;
            case 16:
                /* Add procedure parameter(s) to symbol table */
                newToken.segment = DS;
                newToken.address = dataSegAddress;
                dataSegAddress += 2;
                symbolTable->add(newToken);
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[16][choice];
                break;
            case 17:
                newToken = buildToken();
                choice = tokenToNumeric(newToken);
                nextState = firstPassTable[17][choice];
                break;
            default:                                            
                cout << "ERROR: Unknown token class!" << endl;
                exit(0);
                break;
        } // end switch
        if(newToken.tokenClass != END_OF_FILE){
            tokenFile << newToken.tokenString << "\t" << newToken.tokenClass << endl;
        }    
    } // end while

    tokenFile.close();
    
    // Add temporary variables to symbol table 
    newToken.segment = DS;
    newToken.tokenClass = "";
    newToken.value = "?";
    for(int i = 0; i < 8; i++){
        newToken.tokenString = "T" + to_string(i+1);
        newToken.tokenClass = TEMP;
        newToken.address = dataSegAddress;
        dataSegAddress += 2;
        symbolTable->add(newToken);
    }
    symbolTable->printList();

} // end buildSymbolTable

void Scanner::resetFilePointer(){
    sourceFile.clear();
    sourceFile.seekg(0, sourceFile.beg);  // Set file pointer to beginning of file
} // end resetFilePointer