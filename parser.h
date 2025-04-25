/*
Group 10
Akshay Shukla 2022A7PS0087P
Gobind Singh 2022A7PS0083P
Siddhartha Gotur 2022A7PS0070P
Sriram Sudheer Hebbale 2022A7PS0147P
Granth Jain 2022A7PS0172P
*/


#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"

// ========================== Enum for Non-Terminals ==========================
// Each non-terminal represents a grammar rule in the parser.
typedef enum {
    PROGRAM, MAIN_FUNCTION, OTHER_FUNCTIONS, FUNCTION, INPUT_PAR, OUTPUT_PAR, PARAMETER_LIST, 
    DATA_TYPE, PRIMITIVE_DATATYPE, CONSTRUCTED_DATATYPE, REMAINING_LIST, STMTS, TYPE_DEFINITIONS, 
    ACTUAL_OR_REDEFINED, TYPE_DEFINITION, FIELD_DEFINITIONS, FIELD_DEFINITION, FIELD_TYPE, 
    MORE_FIELDS, DECLARATIONS, DECLARATION, GLOBAL_OR_NOT, OTHER_STMTS, STMT, ASSIGNMENT_STMT, 
    SINGLE_OR_REC_ID, OPTION_SINGLE_CONSTRUCTED, ONE_EXPANSION, MORE_EXPANSIONS, FUN_CALL_STMT, 
    OUTPUT_PARAMETERS, INPUT_PARAMETERS, ITERATIVE_STMT, CONDITIONAL_STMT, ELSE_PART, IO_STMT, 
    ARITHMETIC_EXPRESSION, EXP_PRIME, TERM, TERM_PRIME, FACTOR, HIGH_PRECEDENCE_OPERATOR, 
    LOW_PRECEDENCE_OPERATORS, BOOLEAN_EXPRESSION, VAR, LOGICAL_OP, RELATIONAL_OP, RETURN_STMT, 
    OPTIONAL_RETURN, ID_LIST, MORE_IDS, DEFINETYPE_STMT, A, NON_TERMINAL_COUNT
} NonTerminal;

// ========================== Hash Set for First and Follow Sets ==========================
// A boolean array used to store FIRST and FOLLOW sets for grammar rules.
typedef struct HashSet {
    bool elements[TERMINAL_COUNT]; // Boolean flags indicating which terminals are in the set
} HashSet;

// ========================== RHS Linked List Node ==========================
// Represents a single symbol (terminal or non-terminal) in a production rule.
typedef struct RHSNode {
    int isTerminal;  // 1 if terminal, 0 if non-terminal
    int value;       // Token index (if terminal) or non-terminal index
    struct RHSNode* next; // Pointer to the next symbol in the RHS
} RHSNode;

// ========================== Production Rule Structure ==========================
// Each production consists of a linked list of RHS nodes.
typedef struct ProductionRule {
    RHSNode* head; // Points to the first RHS node in the rule
    struct ProductionRule* next_rule; // Pointer to alternative productions (if any)
} ProductionRule;

// ========================== Grammar Representation ==========================
// Stores production rules for each non-terminal.
typedef struct {
    ProductionRule* rules[NON_TERMINAL_COUNT]; // Array of production rules per non-terminal
} Grammar;

// ========================== Stack for Non-Recursive Parsing ==========================
// Used for parsing input without recursion.
typedef struct stackNode {
    int isTerminal;  // 1 if terminal, 0 if non-terminal
    int value;       // Token or non-terminal index
    struct stackNode *next; // Pointer to the next node in the stack
} stackNode;

// Stack structure to store parsing elements
typedef struct stack {
    stackNode *top; // Pointer to the top of the stack
} stack;

// ========================== Parse Tree Node ==========================
// Represents a node in the parse tree.
typedef struct parseTreeNode {
    char *lexeme;  // The lexeme associated with this node (for terminals)
    int lineno;    // Line number where the lexeme appears
    int ival;      // Integer value (for numerical tokens)
    float fval;    // Floating-point value (for real numbers)
    int isTerminal;// 1 if terminal, 0 if non-terminal
    Terminal token;// Token type (if terminal)
    int val;       // Non-terminal index (if non-terminal)
    struct parseTreeNode* children; // Pointer to the first child node
    struct parseTreeNode* right;    // Pointer to the next sibling node
    struct parseTreeNode* parent;   // Pointer to the parent node
} parseTreeNode;

// ========================== Parse Tree Structure ==========================
// Stores the root of the parse tree.
typedef struct parseTree {
    parseTreeNode* root; // Pointer to the root node
} parseTree;

// ========================== Stack for Parse Tree Construction ==========================
// Used for managing tree nodes during parsing.
typedef struct treeStackNode {
    parseTreeNode* pt; // Pointer to a parse tree node
    struct treeStackNode *next; // Pointer to the next stack node
} treeStackNode;

// Stack for managing parse tree nodes
typedef struct parseTreeStack {
    treeStackNode *top; // Pointer to the top of the tree stack
} parseTreeStack;

#endif
