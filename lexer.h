/*
Akshay Shukla 2022A7PS0087P
Gobind Singh 2022A7PS0083P
Siddhartha Gotur 2022A7PS0070P
Sriram Sudheer Hebbale 2022A7PS0147P
Granth Jain 2022A7PS0172P
*/

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"

// Constants defining table sizes
#define tablesize 10000000  // Size of the symbol table
#define T_BUFFER_SIZE 64    // Size of the twin buffer for lexical analysis
#define KC 28               // Number of keywords in the language

// External array of terminal names (used for printing tokens)
extern char* terms[];

// Structure representing an entry in the symbol table
typedef struct sitem {
    char *lexeme;      // The string representation of the lexeme
    Terminal token;    // The corresponding token type
    char *type;        // Type information (if applicable)
    int iVal;          // Integer value (for numeric literals)
    int lineCount;     // Line number where the lexeme appears
    int eof;           // Flag indicating end-of-file
    float fVal;        // Floating-point value (for real numbers)
    struct sitem *next; // Pointer to the next item in case of hash collision
} SymbolItem;

// Structure representing the symbol table
typedef struct {
    SymbolItem *data[tablesize]; // Array of pointers to symbol table entries
    int currentsize;             // Current number of entries in the symbol table
} Symboltable;

// Structure for implementing twin-buffering (used in lexical analysis)
typedef struct {
    char bufferOne[T_BUFFER_SIZE]; // First buffer
    char bufferTwo[T_BUFFER_SIZE]; // Second buffer
} twinBuffer;

// Structure representing a keyword entry
typedef struct keyword {
    char* key;       // The keyword string (e.g., "while", "if")
    Terminal token;  // The corresponding token type
} keyword;

#endif
