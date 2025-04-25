/*
Group 10
Akshay Shukla 2022A7PS0087P
Gobind Singh 2022A7PS0083P
Siddhartha Gotur 2022A7PS0070P
Sriram Sudheer Hebbale 2022A7PS0147P
Granth Jain 2022A7PS0172P
*/


#ifndef PARSERDEF
#define PARSERDEF

#include <stdbool.h>
#include "parser.h"

// ========================== HashSet Operations ==========================
// Used for managing FIRST and FOLLOW sets in parsing.

// Initializes a HashSet by setting all elements to false.
// Input: HashSet *set - pointer to the set to initialize
// Output: None
void initHashSet(HashSet *set);

// Adds a terminal to a HashSet.
// Input: HashSet *set - pointer to the set, Terminal t - terminal to add
// Output: None
void addToSet(HashSet *set, Terminal t);

// Removes a terminal from a HashSet.
// Input: HashSet *set - pointer to the set, Terminal t - terminal to remove
// Output: None
void removeFromSet(HashSet *set, Terminal t);

// Checks if a terminal is present in a HashSet.
// Input: HashSet *set - pointer to the set, Terminal t - terminal to check
// Output: bool - true if present, false otherwise
bool contains(HashSet *set, Terminal t);

// Performs a union operation between two HashSets (dest = dest ∪ src).
// Input: HashSet *dest - destination set, HashSet *src - source set
// Output: None
void unionSets(HashSet *dest, HashSet *src);

// ========================== Stack Operations (Non-Recursive Parsing) ==========================
// Used for managing parsing elements without recursion.

// Creates an empty stack.
// Input: None
// Output: stack* - pointer to the created stack
stack *createStack();

// Pushes a stack node onto the stack.
// Input: stack *stk - pointer to the stack, stackNode *sn - node to push
// Output: None
void push(stack *stk, stackNode *sn);

// Pops a stack node from the stack.
// Input: stack *stk - pointer to the stack
// Output: stackNode* - pointer to the popped node
stackNode *pop(stack *stk);

// Creates a stack node from an RHS node.
// Input: RHSNode *node - pointer to an RHS node
// Output: stackNode* - pointer to the created stack node
stackNode *createStackNode(RHSNode *node);

// ========================== Parse Tree Operations ==========================
// Functions for managing the parse tree structure.

// Initializes a new parse tree.
// Input: None
// Output: parseTree* - pointer to the newly created parse tree
parseTree *initparseTree();

// Creates a new parse tree node from a token and a stack node.
// Input: TokenNode *tkn - pointer to a token, stackNode *stk - pointer to a stack node
// Output: parseTreeNode* - pointer to the created parse tree node
parseTreeNode *createParseTreeNode(TokenNode *tkn, stackNode *stk);

// Inserts a parse tree node as a child of the given parent node.
// Input: parseTreeNode *tn - node to insert, parseTreeNode *parent - parent node
// Output: None
void insertTree(parseTreeNode *tn, parseTreeNode *parent);

// ========================== Parse Tree Stack Operations ==========================
// Used for managing parse tree nodes during parsing.

// Creates an empty parse tree stack.
// Input: None
// Output: parseTreeStack* - pointer to the created tree stack
parseTreeStack *createTreeStack();

// Pushes a tree node onto the parse tree stack.
// Input: parseTreeStack *stk - pointer to the stack, treeStackNode *sn - node to push
// Output: None
void pushTreeStack(parseTreeStack *stk, treeStackNode *sn);

// Pops a tree node from the parse tree stack.
// Input: parseTreeStack *stk - pointer to the stack
// Output: treeStackNode* - pointer to the popped node
treeStackNode *popTreeStack(parseTreeStack *stk);

// Creates a parse tree stack node from a parse tree node.
// Input: parseTreeNode *node - pointer to a parse tree node
// Output: treeStackNode* - pointer to the created tree stack node
treeStackNode *createTreeStackNode(parseTreeNode *node);

// ========================== Grammar and Parsing Operations ==========================
// Functions for initializing and processing the grammar.

// Initializes the grammar with empty production rules.
// Input: Grammar *G - pointer to the grammar
// Output: None
void initializeGrammar(Grammar *G);

// Adds a production rule to the grammar.
// Input: Grammar *G - pointer to the grammar, NonTerminal lhs - left-hand side non-terminal,
//        int *rhsArray - array of RHS symbols, int rhsSize - number of symbols in RHS,
//        int *isTerminalArray - array indicating which symbols are terminals
// Output: None
void addProduction(Grammar *G, NonTerminal lhs, int *rhsArray, int rhsSize, int *isTerminalArray);

// Computes FIRST sets for all non-terminals in the grammar.
// Input: Grammar *G - pointer to the grammar
// Output: None
void computeAllFirstSets(Grammar *G);

// Computes the FOLLOW set for all non-terminals.
// Input: Grammar *G - pointer to the grammar
// Output: None
void computeFollowSet(Grammar *G);

// Constructs the parse table based on FIRST and FOLLOW sets.
// Input: Grammar *G - pointer to the grammar, 
//        RHSNode *parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT] - parse table
// Output: None
void constructParseTable(Grammar *G, RHSNode *parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT]);

// Parses the input token list using the given parse table and constructs the parse tree.
// Input: TokenList *tk_list - pointer to the token list,
//        RHSNode *parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT] - parse table,
//        stack *mainStack - main parsing stack,
//        parseTreeStack *ptS - parse tree stack,
//        parseTree *pt - pointer to the parse tree
// Output: bool - true if parsing is successful, false if an error occurs
bool parse(TokenList *tk_list, RHSNode *parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT], 
           stack *mainStack, parseTreeStack *ptS, parseTree *pt);

// Performs an in-order traversal of the parse tree and prints it to a file.
// Input: parseTreeNode *node - pointer to the root of the subtree,
//        FILE *parseTreeFile - file pointer for writing the parse tree
// Output: None
void inorderTraversal(parseTreeNode *node, FILE *parseTreeFile);

// Parses the input and prints syntax errors, if any.
// Input: TokenList *tkns - pointer to the token list,
//        FILE *parseTreeOutFile - file pointer for storing the parse tree
// Output: None
void parseAndPrintErrors(TokenList *tkns, FILE *parseTreeOutFile);

#endif // PARSERDEF
