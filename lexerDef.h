/*
Group 10
Akshay Shukla 2022A7PS0087P
Gobind Singh 2022A7PS0083P
Siddhartha Gotur 2022A7PS0070P
Sriram Sudheer Hebbale 2022A7PS0147P
Granth Jain 2022A7PS0172P
*/


#ifndef LEXERDEF
#define LEXERDEF

#include "lexer.h"

// ========================== Function Prototypes ==========================

// Initializes the keyword table with predefined keywords and their corresponding tokens.
// Input: None
// Output: None
void initializeKeywords();

// Computes a hash value for a given lexeme using the DJB2 hash function.
// Input: char* lexeme - the string to hash
// Output: int - the computed hash value
int hash(char *lexeme);

// Checks if a lexeme exists in the symbol table.
// Input: char* lexeme - the string to check
// Output: int - returns 1 if found, 0 otherwise
int lookup(char *lexeme);

// Inserts a new lexeme-token pair into the symbol table if it does not already exist.
// Input: char* lexeme - identifier name, Terminal token - associated token type
// Output: None
void insert(char *lexeme, Terminal token);

// Initializes the symbol table and inserts predefined keywords.
// Input: None
// Output: None
void initializeSymbolTable();

// Extracts and returns the current lexeme from the twin buffer.
// Input: None
// Output: char* - dynamically allocated string containing the lexeme
char *getLexeme();

// Initializes the lexer by setting up buffers, loading input, and preparing the symbol table.
// Input: char* inputFile - file path, long long int buff_size - buffer size
// Output: FILE* - file pointer for reading input
FILE *initialise(char *inputFile, long long int buff_size);

// Reads the next stream of characters into the buffer when needed.
// Input: FILE* fp - file pointer to the source file
// Output: FILE* - updated file pointer
FILE *getstream(FILE *fp);

// Moves the forward pointer one step back, handling buffer transitions if necessary.
// Input: None
// Output: None
void dec_forwardPtr();

// Retrieves the next character from the input stream while managing buffer transitions.
// Input: FILE* fp - file pointer to the source file
// Output: char - next character from the stream
char getNextChar(FILE *fp);

// Checks if a given lexeme is a keyword and returns its corresponding token if found.
// Input: char* lex - the lexeme to check
// Output: Terminal - the token corresponding to the keyword, or -1 if not a keyword
Terminal findKeyword(char *lex);

// Converts a lexeme representing a real number into its floating-point value.
// Input: const char* lexeme - string representation of a number
// Output: double - the computed floating-point value
double compute(const char *lexeme);

// Converts a lexeme into a SymbolItem structure containing token information.
// Input: char* lex - lexeme string, Terminal g - token type, int lineNumber - line number in source code
// Output: SymbolItem - a structure containing lexeme details
SymbolItem tokenize(char *lex, Terminal g, int lineNumber);

// Checks if a lexeme corresponds to `_main` or a function identifier.
// Input: char* str - lexeme to check, int lno - line number in source code
// Output: SymbolItem - a structure representing `_main` or a function ID
SymbolItem check_main(char *str, int lno);

// Uses a DFA to recognize lexemes and return their corresponding tokens.
// Input: FILE* fp - file pointer to the source file
// Output: SymbolItem - structure containing tokenized information
SymbolItem getToken(FILE *fp);

// Handles lexical errors by printing appropriate error messages.
// Input: SymbolItem* newSymbolItem - pointer to the token structure,
//        char* lex - erroneous lexeme, int lno - line number, int error_code - error type
// Output: None
void error_handler(SymbolItem* newSymbolItem, char *lex, int lno, int error_code);

// Increments the global line number counter.
// Input: None
// Output: None
void inc_line_number();

// Creates and initializes an empty token list.
// Input: None
// Output: TokenList* - pointer to a newly allocated token list
TokenList *createTokenList();

// Appends a new token to the token list with its attributes.
// Input: TokenList* tklist - pointer to token list,
//        Terminal token - token type, char* lexeme - lexeme string,
//        int lno - line number, int iVal - integer value, float fVal - floating-point value
// Output: None
void appendTokenList(TokenList *tklist, Terminal token, char *lexeme, int lno, int iVal, float fVal);

// Removes comments from a source file and writes a cleaned version to a new file.
// Input: char* testcaseFile - path to input file, char* cleanFile - path to output file
// Output: None
void removeComments(char *testcaseFile, char *cleanFile);

// Processes an input file to return a list of tokens, optionally printing them.
// Input: char* inputFile - file path, int choice - 1 for silent processing, 2 for printing tokens
// Output: TokenList* - pointer to the list of extracted tokens
TokenList *returnTokenList(char *inputFile, int choice);

#endif
