/*
Group 10
Akshay Shukla 2022A7PS0087P
Gobind Singh 2022A7PS0083P
Siddhartha Gotur 2022A7PS0070P
Sriram Sudheer Hebbale 2022A7PS0147P
Granth Jain 2022A7PS0172P
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexerDef.h"
#include <math.h>

char* terms[]={
    "TK_MAIN", "TK_END", "TK_FUNID", "TK_INPUT", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", 
    "TK_OUTPUT", "TK_ID", "TK_INT", "TK_REAL", "TK_RECORD", "TK_RUID", "TK_UNION", "TK_COMMA", 
    "TK_SEM", "TK_COLON", "TK_GLOBAL", "TK_ASSIGNOP", "TK_DOT", "TK_CALL", "TK_WITH", "TK_PARAMETERS", 
    "TK_WHILE", "TK_OP", "TK_CL", "TK_ENDWHILE", "TK_IF", "TK_THEN", "TK_ELSE", "TK_ENDIF", "TK_READ", 
    "TK_WRITE", "TK_MUL", "TK_DIV", "TK_PLUS", "TK_MINUS", "TK_NOT", "TK_AND", "TK_OR", "TK_LT", "TK_LE", 
    "TK_EQ", "TK_GT", "TK_GE", "TK_NE", "TK_NUM", "TK_RNUM", "TK_RETURN", "TK_DEFINETYPE", "TK_AS", 
    "TK_ENDRECORD", "TK_ENDUNION", "TK_FIELDID", "TK_TYPE", "EPS", "END_OF_INPUT", "DOLLAR", "TK_COMMENT"
};

// Global variables
keyword * keywordTable[KC];
Symboltable * table;
twinBuffer tb;
int line = 1;
bool bufferOneLoad = true;
bool bufferTwoLoad = false;
bool isFileEnd = false;

// Lexeme pointers
char * beginPtr;
char * forwardPtr;

/*---------------------------------------------------------------- Function Prototypes ----------------------------------------------------------------------------------*/

char * cleanFile(FILE * fptr, int count){
    // Removes all whitespaces and "\t"s.
    char cur_character;
    char * prelex = (char *)malloc(count * sizeof(char));
    while ((cur_character = fgetc(fptr)) != EOF){
        if(cur_character != ' ' && cur_character != '\t'){
            strncat(prelex, &cur_character, 1);
        }
    }
    return prelex;
}

void neatifyFile(FILE * fptr, int count, char * neatFile){
    // Removes extra whitespaces keeping single whitespaces only
    char space = ' ';
    char t_space = '\t';
    char newline = '\n';
    char cur_character;
    char prev_character;

    FILE * neat = fopen(neatFile, "w");

    if(!neat){
        printf("Error while opening neat file !\n");
        return;
    }

    while((cur_character = fgetc(fptr)) != EOF){
        int newline_count = 0;
        prev_character = '?';
        while(cur_character == ' ' || cur_character == '\t' || cur_character == '\n'){
            // Skip all whitespaces and newlines
            prev_character = cur_character;
            if(cur_character == '\n') newline_count++;
            cur_character = fgetc(fptr);
            if (cur_character == EOF){
                break;
            }
        }
        if(newline_count > 0){ 
            // If there are multiple newlines, print only one
            for(int i = 0; i < newline_count; i++){
                fputc(newline, neat);
            }
        }
        else if(prev_character == ' ' || prev_character == '\t'){ 
            // If there are multiple spaces, print only one
            fputc(space, neat);
        }
        if(cur_character != ' ' && cur_character != '\t'){ 
            // Print the character if it is not a space or tab
            fputc(cur_character, neat);
        }
    }

    fclose(neat);
}

void initializeKeywords()
{
    // Initialize the keywords
    keyword key[] = {
        {"_main", TK_MAIN},
        {"as", TK_AS},
        {"call", TK_CALL},
        {"definetype", TK_DEFINETYPE},
        {"else", TK_ELSE},
        {"end", TK_END},
        {"endif", TK_ENDIF},
        {"endrecord", TK_ENDRECORD},
        {"endunion", TK_ENDUNION},
        {"endwhile", TK_ENDWHILE},
        {"global", TK_GLOBAL},
        {"if", TK_IF},
        {"input", TK_INPUT},
        {"int", TK_INT},
        {"list", TK_LIST},
        {"output", TK_OUTPUT},
        {"parameters", TK_PARAMETERS},
        {"parameter", TK_PARAMETER},
        {"real", TK_REAL},
        {"record", TK_RECORD},
        {"return", TK_RETURN},
        {"then", TK_THEN},
        {"type", TK_TYPE},
        {"union", TK_UNION},
        {"while", TK_WHILE},
        {"with", TK_WITH},
        {"write", TK_WRITE},
        {"read", TK_READ}
    };

    // Allocate memory for the keywordTable and copy the keywords
    for(int i =0 ; i<KC ; i++){
            keywordTable[i]= (keyword*)malloc(sizeof(keyword)); 
            keywordTable[i] -> key = (char *)malloc(20 * sizeof(char));
            if(keywordTable[i]==NULL){
                fprintf(stderr , "Memory allocation failed for keywords %d\n" , i); 
                exit(EXIT_FAILURE);
            }

            keywordTable[i] -> key = strdup(key[i].key);
            keywordTable[i] -> token = key[i].token; 
        
    }
}

int hash(char *lexeme)
{
    // Hash function to generate hash value for the lexeme
    unsigned long hash = 5381;
    int c;
    while ((c = *lexeme++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % tablesize;
}

int lookup(char *lexeme)
{
    int index = hash(lexeme);

    // Traverse the linked list at index to find the lexeme
    SymbolItem *current = table->data[index];
    while (current != NULL)
    {
        if (strcmp(current->lexeme, lexeme) == 0)
        {
            // Lexeme found in the symbol table
            return true;
        }
        current = current->next;
    }
    // Lexeme not found in the symbol table
    return false;
}

void insert(char *lexeme, Terminal token)
{
    int index = hash(lexeme);

    // Check if the lexeme already exists
    if (!lookup(lexeme))
    {
        // Lexeme doesn't exist, create a new SymbolItem
        SymbolItem *newItem = (SymbolItem *)malloc(sizeof(SymbolItem));
        if (newItem == NULL)
        {
            fprintf(stderr, "Memory allocation failed for SymbolItem.\n");
            exit(EXIT_FAILURE);
        }
        newItem->lexeme = strdup(lexeme); // Make a copy of lexeme
        if (newItem->lexeme == NULL)
        {
            fprintf(stderr, "Memory allocation failed for lexeme.\n");
            exit(EXIT_FAILURE);
        }
        newItem->token = token;
        newItem->next = NULL; // Initialize next pointer

        // Insert newItem into the symbol table
        if (table->data[index] == NULL)
        {
            // No collision, insert newItem directly
            table->data[index] = newItem;
        }
        else
        {
            // Collision occurred, append newItem to the existing linked list
            SymbolItem *current = table->data[index];
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = newItem;
        }
        table->currentsize++;
    }
    else
    {
        // Lexeme already exists, handle as needed
        //printf("Lexeme %s already exists in the symbol table.\n", lexeme);
    }
}

void initializeSymbolTable()
{
    // Allocate memory for the symbol table
    table = (Symboltable *)malloc(sizeof(Symboltable));
    if (table == NULL)
    {
        fprintf(stderr, "Memory allocation failed for SymbolTable.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < tablesize; i++)
    {
        table->data[i] = NULL;
    }
    table->currentsize = 0;
    initializeKeywords();

    // Insert keywords into the symbol table
    for(int i = 0; i < KC; i++)
    {
        insert(keywordTable[i]->key, keywordTable[i]->token);
    }
    return;
}

char * getLexeme(){
    int lex_len;
    char * lex;

    // If both pointers are in bufferOne
    if((beginPtr >= tb.bufferOne && beginPtr < tb.bufferOne + T_BUFFER_SIZE) && (forwardPtr >= tb.bufferOne && forwardPtr < tb.bufferOne + T_BUFFER_SIZE)){
        lex_len = forwardPtr - beginPtr;
        lex = (char *)malloc((lex_len + 1) * sizeof(char));

        if(!lex){
            fprintf(stderr, "Mem allocation failed 0");
            exit(EXIT_FAILURE);
        }

        strncpy(lex, beginPtr, lex_len);
    }
    // If both pointers are in bufferTwo
    else if((beginPtr >= tb.bufferTwo && beginPtr < tb.bufferTwo + T_BUFFER_SIZE) && (forwardPtr >= tb.bufferTwo && forwardPtr < tb.bufferTwo + T_BUFFER_SIZE)){
        lex_len = forwardPtr - beginPtr;
        lex = (char *)malloc((lex_len + 1) * sizeof(char));

        if(!lex){
            printf("%x %x %x %x %x %x\n", tb.bufferOne,tb.bufferOne + T_BUFFER_SIZE, tb.bufferTwo,tb.bufferTwo + T_BUFFER_SIZE, beginPtr, forwardPtr);
            printf("%d lex_len\n", lex_len);
            fprintf(stderr, "Mem allocation failed 1");
            exit(EXIT_FAILURE);
        }
        else{
            strncpy(lex, beginPtr, lex_len);
        }
    }
    // If beginPtr is in bufferOne and forwardPtr is in bufferTwo
    else if((beginPtr >= tb.bufferOne && beginPtr < tb.bufferOne + T_BUFFER_SIZE) && (forwardPtr >= tb.bufferTwo && forwardPtr < tb.bufferTwo + T_BUFFER_SIZE)){
        lex_len = tb.bufferOne + T_BUFFER_SIZE - beginPtr + forwardPtr - tb.bufferTwo;
        lex = (char *)malloc((lex_len + 1) * sizeof(char));

        if(!lex){
            fprintf(stderr, "Mem allocation failed 2");
            exit(EXIT_FAILURE);
        }

        int partOneLen = tb.bufferOne + T_BUFFER_SIZE - beginPtr;
        strncpy(lex, beginPtr, partOneLen);
        strncpy(lex + partOneLen, tb.bufferTwo, lex_len - partOneLen);
    }
    // If beginPtr is in bufferTwo and forwardPtr is in bufferOne
    else{
        lex_len = tb.bufferTwo + T_BUFFER_SIZE - beginPtr + forwardPtr - tb.bufferOne;
        lex = (char *)malloc((lex_len + 1) * sizeof(char));

        if(!lex){
            fprintf(stderr, "Mem allocation failed 3");
            exit(EXIT_FAILURE);
        }

        int partOneLen = tb.bufferTwo + T_BUFFER_SIZE - beginPtr;
        strncpy(lex, beginPtr, partOneLen);
        strncpy(lex + partOneLen, tb.bufferOne, lex_len - partOneLen);
    }
    lex[lex_len] = '\0';

    return lex;
}

FILE * initialise(char * inputFile, long long int buff_size){

    // Initialize the twin buffer
    memset(tb.bufferOne, 0, sizeof(tb.bufferOne));
    memset(tb.bufferTwo, 0, sizeof(tb.bufferTwo));
    
    FILE * fptr = fopen(inputFile, "r");

    if(fptr == NULL){
        printf("Error opening prelex file !\n");
        return NULL;
    }

    // Read the first buffer and fill it
    size_t size = fread(tb.bufferOne, sizeof(char), T_BUFFER_SIZE, fptr);
    if (size < T_BUFFER_SIZE){
        tb.bufferOne[size] = EOF;
    }

    // Updating the global variables
    bufferOneLoad = false;
    bufferTwoLoad = true;
    line = 1;
    isFileEnd = false;
    beginPtr = tb.bufferOne;
    forwardPtr = tb.bufferOne;
    initializeSymbolTable();
    return fptr;
}

FILE * getstream(FILE * fp){

    // Check if the forwardPtr is at the end of bufferOne
    if (forwardPtr == tb.bufferOne + T_BUFFER_SIZE - 1){
        if(bufferTwoLoad){
            size_t size = fread(tb.bufferTwo, sizeof(char), T_BUFFER_SIZE, fp);
            if(size < T_BUFFER_SIZE){
                tb.bufferTwo[size] = EOF;
            }
        }
        bufferOneLoad = true;
        bufferTwoLoad = false;
        forwardPtr = tb.bufferTwo;
    }
    // Check if the forwardPtr is at the end of bufferTwo
    else if(forwardPtr == tb.bufferTwo + T_BUFFER_SIZE - 1){
        if(bufferOneLoad){
            size_t size = fread(tb.bufferOne, sizeof(char), T_BUFFER_SIZE, fp);
            if (size < T_BUFFER_SIZE){
                tb.bufferOne[size] = EOF;
            }
        }
        bufferOneLoad = false;
        bufferTwoLoad = true;
        forwardPtr = tb.bufferOne;
    }
    else{
        // Move the forwardPtr to the next character
        forwardPtr++;
    }

    return fp;
}

void dec_forwardPtr(){
    // Decrement the forwardPtr based on the buffer it is in
    if(forwardPtr == tb.bufferOne){
        forwardPtr = tb.bufferTwo + T_BUFFER_SIZE - 1;
    }
    else if(forwardPtr == tb.bufferTwo){
        forwardPtr = tb.bufferOne + T_BUFFER_SIZE - 1;
    }
    else{
        forwardPtr--;
    }
}

char getNextChar(FILE * fp){
    // Get the next character from the file using getstream()
    char ch = * forwardPtr;
    if (ch == EOF){
        isFileEnd = true;
    }

    getstream(fp);
    return ch;
}

Terminal findKeyword(char * lex){
    // Find the keyword in the keywordTable
    for(int i = 0; i < KC; i++){
        if (strcmp(lex, keywordTable[i] -> key) == 0){
            return keywordTable[i] -> token;
        }
    }
    return -1;
}

double compute(const char *lexeme) {
    // Compute the RNUM value of the lexeme 
    // general form: [0-9][0-9]* [.][0-9][0-9] [E][+|-|∈] [0-9][0-9]
    char *end;
    double value = strtod(lexeme, &end);
    //printf("value: %lf\n",value);

    if (*end == 'E' || *end == 'e') {
        int exponent = strtol(end + 1, NULL, 10);
        value *= pow(10, exponent);
    }

    return value;
}


SymbolItem tokenize(char * lex, Terminal g, int lineNumber){

    // Tokenize the lexeme based on the terminal
    SymbolItem newSymbolItem;
    newSymbolItem.lexeme = lex;
    newSymbolItem.lineCount = lineNumber;
    newSymbolItem.token = g;
    newSymbolItem.iVal = 0;
    newSymbolItem.fVal = 0.0;

    switch(g){
        case TK_NUM:
            newSymbolItem.iVal = (int)compute(lex); 
            break;
        case TK_RNUM:
            newSymbolItem.fVal = compute(lex); 
            break;
        case TK_FIELDID:
            // use findKeyword to find the token
            // insert the lexeme into the symbol table
            newSymbolItem.token = findKeyword(newSymbolItem.lexeme);
            if (newSymbolItem.token == -1)
            {
                newSymbolItem.token = TK_FIELDID;
            }
            else insert(lex, TK_FIELDID);
            break;
        case TK_ID:
            // insert the lexeme into the symbol table
            if(strlen(newSymbolItem.lexeme) > 20){
                error_handler(&newSymbolItem,lex, lineNumber, -3);
            }
            else insert(lex, TK_ID);
            break;
        case TK_FUNID:
            // insert the lexeme into the symbol table
            if(strlen(newSymbolItem.lexeme) > 30){
                error_handler(&newSymbolItem,lex, lineNumber, -4);
            }
            else insert(lex, TK_FUNID);
            break;
        }

    // No switch case for other Terminal as they don't need any processing
    beginPtr = forwardPtr;
    return newSymbolItem;
}

SymbolItem check_main(char * str, int lno){
    // checks if the lexeme is TK_MAIN or TK_FUNID
    if(strncmp(str, "_main", 5) == 0){
        return tokenize("_main", TK_MAIN, lno);
    }
    else{
        return tokenize(str, TK_FUNID, lno);
    }
}

SymbolItem getToken(FILE * fp){

    // Use DFA to identify meaningful lexemes and get their tokens
    beginPtr = forwardPtr;
    char ch = getNextChar(fp);
    int dfa = 0; 
    SymbolItem newSymbolItem;

    if(isFileEnd){
        newSymbolItem.eof = true;
        newSymbolItem.lexeme = NULL;
        newSymbolItem.lineCount = line;
        return newSymbolItem;
    }

    while(dfa >= 0){
        switch(dfa){
            case 0:
                if(ch >= 'b' && ch <= 'd'){
                    dfa = 1;
                }
                else if(ch == 'a' || (ch >= 'e' && ch <= 'z')){
                    dfa = 15;
                }
                else if(ch >= '0' && ch <= '9'){
                    dfa = 13;
                }
                else if(ch == '<'){
                    dfa = 17;
                }
                else if(ch == '_'){
                    dfa = 25;
                }
                else if(ch == '>'){
                    dfa = 30;
                }
                else if(ch == '&'){
                    dfa = 33;
                }
                else if(ch == '@'){
                    dfa = 36;
                }
                else if(ch == '#'){
                    dfa = 39;
                }
                else if(ch == '!'){
                    dfa = 42;
                }
                else if(ch == '%'){
                    dfa = 44;
                }
                else if(ch == '='){
                    dfa = 48;
                }
                else if(ch == ']'){
                    dfa = 50;
                }
                else if(ch == '['){
                    dfa = 52;
                }
                else if(ch == '.'){
                    dfa = 54;
                }
                else if(ch == ':'){
                    dfa = 56;
                }
                else if(ch == ';'){
                    dfa = 58;
                }
                else if(ch == '~'){
                    dfa = 60;
                }
                else if(ch == ','){
                    dfa = 62;
                }
                else if(ch == '('){
                    dfa = 51;
                }
                else if(ch == ')'){
                    dfa = 53;
                }
                else if(ch == '+'){
                    dfa = 55;
                }
                else if(ch == '-'){
                    dfa = 57;
                }
                else if(ch == '*'){
                    dfa = 59;
                }
                else if(ch == '/'){
                    dfa = 61;
                }
                else if(ch == '\n'){
                    dfa = 63;
                }
                else if(ch == ' '){
                    dfa = 46;
                }
                // else if(ch==EOF){
                //     break;
                // }
                else{
                    // Error code
                    dfa = -5;
                }
            break;
            case 1:
                ch = getNextChar(fp);
                if(ch >= '2' && ch <= '7'){
                    dfa = 2;
                }
                else if(ch >= 'a' && ch <= 'z'){
                    dfa = 15;
                }
                else{
                    // Error code
                    dfa = -2; 
                }
            break;
            case 2:
                ch = getNextChar(fp);
                if(ch >= 'b' && ch <= 'd'){
                    dfa = 2;
                }
                else if(ch >= '2' && ch <= '7'){
                    dfa = 3;
                }
                else{
                    dfa = 4;
                }
            break;
            case 3:
                ch = getNextChar(fp);
                if(ch >= '2' && ch <= '7'){
                    dfa = 3;
                }
                else{
                    dfa = 4;
                }
            break;
            case 4:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_ID, line);
            break;
            case 5:
                return tokenize(getLexeme(), TK_RNUM, line);
            break;
            case 6:
                ch = getNextChar(fp);
                if(ch >= '0' && ch <= '9'){
                    dfa = 5;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 7:
                ch = getNextChar(fp);
                if(ch >= '0' && ch <= '9'){
                    dfa = 6;
                }
                else{
                    // Error code
                    dfa = -2;
                }               
            break;
            case 8:
                ch = getNextChar(fp);
                if(ch == '+' || ch == '-'){
                    dfa = 7;
                }
                else if(ch >= '0' && ch <= '9'){
                    dfa = 6;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 9:
                ch = getNextChar(fp);
                if(ch == 'E'){
                    dfa = 8;
                }
                else{
                    dfa = 10;
                }
            break;
            case 10:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_RNUM, line);
            break;
            case 11:
                ch = getNextChar(fp);
                if(ch >= '0' && ch <= '9'){
                    dfa = 9;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 12:
                ch = getNextChar(fp);
                if(ch >= '0' && ch <= '9'){
                    dfa = 11;
                }
                else{
                    dfa = 21;
                }
            break;
            case 13:
                ch = getNextChar(fp);
                if(ch <= '9' && ch >= '0'){
                    dfa = 13;
                }
                else if(ch == '.'){
                    dfa = 12;
                }
                else{
                    dfa = 14;
                }
            break;
            case 14:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_NUM, line);
            break;
            case 15:
                ch = getNextChar(fp);
                if(ch >= 'a' && ch <= 'z'){
                    dfa = 15;
                }
                else{
                    dfa = 16;
                }
            break;
            case 16:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_FIELDID, line);
            break;
            case 17:
                ch = getNextChar(fp);
                if(ch == '='){
                    dfa = 23;
                }
                else if(ch == '-'){
                    dfa = 18;
                }
                else{
                    dfa = 22;
                }
            break;
            case 18:
                ch = getNextChar(fp);
                if(ch == '-'){
                    dfa = 19;
                }
                else{
                    dfa = 24;
                }
            break;
            case 19:
                ch = getNextChar(fp);
                if(ch == '-'){
                    dfa = 20;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 20:
                return tokenize(getLexeme(), TK_ASSIGNOP, line);
            break;
            case 21:
                forwardPtr -= 2;
                return tokenize(getLexeme(), TK_NUM, line);
            break;
            case 22:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_LT, line);
            break;
            case 23:
                return tokenize(getLexeme(), TK_LE, line);
            break;
            case 24:
                forwardPtr -= 2;
                return tokenize(getLexeme(), TK_LT, line);
            break;
            case 25:
                ch = getNextChar(fp);
                if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')){
                    dfa = 26;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 26:
                ch = getNextChar(fp);
                if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')){
                    dfa = 26;
                }
                else if(ch >= '0' && ch <= '9'){
                    dfa = 27;
                }
                else{
                    dfa = 28;
                }
            break;
            case 27:
                ch = getNextChar(fp);
                if(ch >= '0' && ch <= '9'){
                    dfa = 27;
                }
                else{
                    dfa = 29;
                }
            break;
            case 28:
                dec_forwardPtr();
                return check_main(getLexeme(), line);
            break;
            case 29:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_FUNID, line);
            break;
            case 30:
                ch = getNextChar(fp);
                if(ch == '='){
                    dfa = 31;
                }
                else{
                    dfa = 32;
                }
            break;
            case 31:
                return tokenize(getLexeme(), TK_GE, line);
            break;
            case 32:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_GT, line);
            break;
            case 33:
                ch = getNextChar(fp);
                if(ch == '&'){
                    dfa = 34;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 34:
                ch = getNextChar(fp);
                if(ch == '&'){
                    dfa = 35;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 35:
                return tokenize(getLexeme(), TK_AND, line);
            break;
            case 36:
                ch = getNextChar(fp);
                if(ch == '@'){
                    dfa = 37;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 37:
                ch = getNextChar(fp);
                if(ch == '@'){
                    dfa = 38;
                }
                else{
                    // Error code
                    dfa = -2;
                }               
            break;
            case 38:
                return tokenize(getLexeme(), TK_OR, line);
            break;
            case 39:
                ch = getNextChar(fp);
                if(ch >= 'a' && ch <= 'z'){
                    dfa = 40;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 40:
                ch = getNextChar(fp);
                if(ch >= 'a' && ch <= 'z'){
                    dfa = 40;
                }
                else{
                    dfa = 41;
                }     
            break;
            case 41:
                dec_forwardPtr();
                return tokenize(getLexeme(), TK_RUID, line);
            break;
            case 42:
                ch = getNextChar(fp);
                if(ch == '='){
                    dfa = 43;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 43:
                return tokenize(getLexeme(), TK_NE, line);
            break;
            case 44:
                while((ch != '\n')){
                    ch = getNextChar(fp);
                }
                dfa = 45;
            break;
            case 45:
                inc_line_number();
                return tokenize("%", TK_COMMENT, line - 1);
            break;
            case 46:
                beginPtr = forwardPtr;
                ch = getNextChar(fp);
                dfa = 0;
            break;
            case 47:
            break;
            case 48:
                ch = getNextChar(fp);
                if(ch == '='){
                    dfa = 49;
                }
                else{
                    // Error code
                    dfa = -2;
                }
            break;
            case 49:
                return tokenize(getLexeme(), TK_EQ, line);
            break;
            case 50:
                return tokenize(getLexeme(), TK_SQR, line);
            break;
            case 51:
                return tokenize(getLexeme(), TK_OP, line);
            break;
            case 52:
                return tokenize(getLexeme(), TK_SQL, line);
            break;
            case 53:
                return tokenize(getLexeme(), TK_CL, line);
            break;
            case 54:
                return tokenize(getLexeme(), TK_DOT, line);
            break;
            case 55:
                return tokenize(getLexeme(), TK_PLUS, line);
            break;
            case 56:
                return tokenize(getLexeme(), TK_COLON, line);
            break;
            case 57:
                return tokenize(getLexeme(), TK_MINUS, line);
            break;
            case 58:
                return tokenize(getLexeme(), TK_SEM, line);
            break;
            case 59:
                return tokenize(getLexeme(), TK_MUL, line);
            break;
            case 60:
                return tokenize(getLexeme(), TK_NOT, line);
            break;
            case 61:
                return tokenize(getLexeme(), TK_DIV, line);
            break;
            case 62:
                return tokenize(getLexeme(), TK_COMMA, line);
            break;
            case 63:
                inc_line_number();
                beginPtr = forwardPtr;
                ch = getNextChar(fp);
                if(isFileEnd){
                    newSymbolItem.eof = true;
                    newSymbolItem.lexeme = NULL;
                    newSymbolItem.lineCount = line;
                    return newSymbolItem;
                }
                dfa = 0;
            break;
        }
    }

    // Error handling
    if(dfa < 0){
        if(dfa == -5){
            error_handler(&newSymbolItem,getLexeme(), line, dfa);   
        }
        else{
            dec_forwardPtr();
            error_handler(&newSymbolItem,getLexeme(), line, dfa);
        }
    }

    return newSymbolItem;
}

void error_handler(SymbolItem* newSymbolItem,char * lex, int lno, int error_code){
    if(error_code == -2){ // Invalid pattern
        printf("Line No. %d : Error: Unknown pattern <%s>\n", lno, lex);
    }
    else if(error_code == -3){ // Length of variable identifier is greater than 20
        printf("Line No. %d : Error: Variable Identifier is longer than the prescribed length of 20 characters.\n", lno);
    }
    else if(error_code == -4){ // Length of function identifier is greater than 30
        printf("Line No. %d : Error: Function Identifier is longer than the prescribed length of 30 characters.\n", lno);
    }
    else if(error_code = -5){ // Invalid symbol
        printf("Line No. %d : Error: Unknown Symbol <%s>\n", lno, lex);
    }

    // Initialize the newSymbolItem with error values
    newSymbolItem->lexeme = NULL;
    newSymbolItem->lineCount = -1;
    newSymbolItem->token = -1;
    newSymbolItem->iVal = 0;
    newSymbolItem->fVal = 0.0;
    beginPtr = forwardPtr;
}

void inc_line_number(){
    // Increment the line number
    line++;
}

TokenList * createTokenList(){
    // Create a new TokenList
    TokenList * tklist = (TokenList *)malloc(sizeof(TokenList));
    tklist -> count = 0;
    tklist -> head = NULL;
    return tklist;
}

void appendTokenList(TokenList * tklist, Terminal token, char * lexeme, int lno, int iVal, float fVal){
    // Append a new TokenNode to the TokenList
    TokenNode * tknode = (TokenNode *)malloc(sizeof(TokenNode));
    tknode -> token = token;
    tknode -> lexeme = lexeme;
    tknode -> lineNumber = lno;
    tknode -> next = NULL;
    tknode -> ival = iVal;
    tknode -> fval = fVal;

    if(tklist -> head == NULL){
        tklist -> head = tknode;
        tklist -> count++;
        return;
    }

    TokenNode * temp = tklist -> head;
    while(temp -> next != NULL){
        temp = temp -> next;
    }
    tklist -> count++;
    temp -> next = tknode;
}

void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *input = fopen(testcaseFile, "r");
    FILE *output = fopen(cleanFile, "w");

    if (!input || !output) {
        printf("Error: Unable to open file(s)!\n");
        return;
    }

    char ch;
    bool inComment = false;
    bool isEmptyLine = true;  // Track if the line is empty after comment removal

    while ((ch = fgetc(input)) != EOF) {
        if (ch == '%') {
            // Start of a single-line comment, skip until newline
            inComment = true;
        }

        if (ch == '\n') {
            // End of the comment
            inComment = false;
            if (!isEmptyLine) {
                fputc(ch, output); // Write only if the line had content
            }
            isEmptyLine = true; // Reset for the next line
        } else if (!inComment) {
            // Copy non-comment characters
            if (ch != ' ' && ch != '\t') {
                isEmptyLine = false;  // Line has non-whitespace content
            }
            fputc(ch, output);
        }
    }

    fclose(input);
    fclose(output);
    printf("Comments and blank lines removed. Clean file saved as: %s\n", cleanFile);
    printf("\n");
    FILE *fptr = fopen(cleanFile, "r");  
    if (fptr == NULL) {  
        printf("Error in printing");
    }

    // Read and print each line from the file
    char readbuff[100];
    while (fgets(readbuff, sizeof(readbuff), fptr) != NULL) {
        printf("%s", readbuff);
    }
    printf("\n");
    fclose(fptr);
}

/*-------------------------------------------------------------Main Function-------------------------------------------------------------------------------*/
TokenList *returnTokenList(char *inputFile,int choice) {
    // Initialize the lexer with the cleaned file
    bool prelex = false;
    char* prelex_file_name = "preprocessedFileForLexer.txt";
    // Neatifying the original txt file and copying into prelex_file
    FILE * fp = NULL;
    fp = fopen(inputFile, "r");
    char cur_character;
    if (fp == NULL){
        printf("Error while opening source text in returnTokenList!\n");
        return NULL;
    }
    int count= 0;
    while ((cur_character = fgetc(fp)) != EOF){
        count++;
    }
    rewind(fp);
    neatifyFile(fp, count, prelex_file_name);
    fclose(fp);
    
    // Initialize the lexer with the cleaned file
    fp = initialise(prelex_file_name, T_BUFFER_SIZE);
    if (!fp) {
        fprintf(stderr, "Failed to initialize lexer with file: %s\n", prelex_file_name);
        return NULL;
    }

    // Fetch and print tokens until the end of the file
    TokenList * tklist = createTokenList();
    tklist -> count = 0;
    SymbolItem currToken;
    int tokenCount = 0;
    line = 1; 

    while (!isFileEnd) {
        currToken = getToken(fp);
        // Check if a valid token is fetched
        if (currToken.lexeme != NULL) {
            tokenCount++;
            if(choice ==2){
                printf("Line No. %d\t\tLexeme %s\t\tToken %s \n", currToken.lineCount, currToken.lexeme,terms[currToken.token]);
            }
            if(currToken.token != TK_COMMENT) appendTokenList(tklist, currToken.token, currToken.lexeme, currToken.lineCount, currToken.iVal, currToken.fVal);
            // Assuming dynamic memory allocation for lexeme, it should be freed after use
            // free(currToken.lexeme);
        }
    }
    //Close the file and clean up if necessary
    fclose(fp);

    return tklist;
}
