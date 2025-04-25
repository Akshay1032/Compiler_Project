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
#include <stdbool.h>
#include <string.h>
#include "parserDef.h"

const char* terminalNames[] = {
    "TK_MAIN", "TK_END", "TK_FUNID", "TK_INPUT", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", 
    "TK_OUTPUT", "TK_ID", "TK_INT", "TK_REAL", "TK_RECORD", "TK_RUID", "TK_UNION", "TK_COMMA", 
    "TK_SEM", "TK_COLON", "TK_GLOBAL", "TK_ASSIGNOP", "TK_DOT", "TK_CALL", "TK_WITH", "TK_PARAMETERS", 
    "TK_WHILE", "TK_OP", "TK_CL", "TK_ENDWHILE", "TK_IF", "TK_THEN", "TK_ELSE", "TK_ENDIF", "TK_READ", 
    "TK_WRITE", "TK_MUL", "TK_DIV", "TK_PLUS", "TK_MINUS", "TK_NOT", "TK_AND", "TK_OR", "TK_LT", "TK_LE", 
    "TK_EQ", "TK_GT", "TK_GE", "TK_NE", "TK_NUM", "TK_RNUM", "TK_RETURN", "TK_DEFINETYPE", "TK_AS", 
    "TK_ENDRECORD", "TK_ENDUNION", "TK_FIELDID", "TK_TYPE", "EPS", "END_OF_INPUT", "DOLLAR", "TK_COMMENT"
};

const char* nonTerminalNames[] = {
    "<program>", "<mainFunction>", "<otherFunctions>", "<function>", "<input_par>", "<output_par>", "<parameter_list>", "<dataType>", "<primitiveDatatype>",
     "<constructedDatatype>", "<remaining_list>", "<stmts>", "<typeDefinitions>", "<actualOrRedefined>", "<typeDefinition>", "<fieldDefinitions>", "<fieldDefinition>", 
     "<fieldtype>", "<moreFields>", "<declarations>", "<declaration>", "<global_or_not>", "<otherStmts>", "<stmt>", "<assignmentStmt>", "<singleOrRecId>",
      "<option_single_constructed>", "<oneExpansion>", "<moreExpansions>", "<funCallStmt>", "<outputParameters>", "<inputParameters>", "<iterativeStmt>",
       "<conditionalStmt>", "<elsePart>", "<ioStmt>", "<arithmeticExpression>", "<expPrime>", "<term>", "<termPrime>", "<factor>", "<highPrecedenceOperator>",
     "<lowPrecedenceOperators>", "<booleanExpression>", "<var>", "<logicalOp>", "<relationalOp>", "<returnStmt>", "<optionalReturn>", "<idList>", "<more_ids>", "<definetypestmt>", "<A>"
};


void initHashSet(HashSet *set) {
    memset(set->elements, 0, sizeof(set->elements));
}

void addToSet(HashSet *set, Terminal t) {
    set->elements[t] = true;
}

void removeFromSet(HashSet * set, Terminal t){
    set -> elements[t] = false;
}

bool contains(HashSet *set, Terminal t) {
    return set->elements[t];
}

void unionSets(HashSet *dest, HashSet *src) {
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        if (src->elements[i] && i != EPS) {
            dest->elements[i] = true;
        }
    }
}

HashSet firstSet[NON_TERMINAL_COUNT];
HashSet followSet[NON_TERMINAL_COUNT];
bool computedFirst[NON_TERMINAL_COUNT]; // To track if a FIRST set is computed

TokenNode * lookAheadPtr;

RHSNode * parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT];
stack * createStack(){
    stack * stk = (stack *)malloc(sizeof(stack));
    return stk;
}


void top(stack * stk){
    if(stk -> top){
        printf("%s\n", stk -> top -> isTerminal ? terminalNames[stk -> top -> value]:nonTerminalNames[stk -> top -> value]);
    }
    return;
}


stackNode * createStackNode(RHSNode * node){
    stackNode * sn = (stackNode *)malloc(sizeof(stackNode));
    sn -> isTerminal = node -> isTerminal;
    sn -> value = node -> value;
    sn -> next = NULL;
    return sn;
}

void push(stack * stk, stackNode * sn){
    sn -> next = stk -> top;
    stk -> top = sn;
}


stack * initMainStack(){
    stack * stk = (stack *)malloc(sizeof(stack));
    stk -> top = NULL;
    stackNode * sn = (stackNode *)malloc(sizeof(stackNode));
    sn -> isTerminal = 1;
    sn -> value = DOLLAR;
    sn -> next = NULL;
    push(stk, sn);
    return stk;
}


stackNode * pop(stack * stk){
    if(!stk -> top){
        return NULL;
    }
    stackNode * sn = stk -> top;
    stk -> top = sn -> next;
    return sn;
}

void addToStack(stack * stk, RHSNode * prod_rule){
    stack * temp_stk = (stack *)malloc(sizeof(stk));
    temp_stk -> top = NULL;

    RHSNode * cur = prod_rule;
    while(cur){
        stackNode * sn = createStackNode(cur);
        push(temp_stk, sn);
        cur = cur -> next;
    }
    stackNode * sn;
    while(sn = pop(temp_stk)){
        push(stk, sn);
    }
    free(temp_stk);
}

// Function to create a new RHS node
RHSNode* createRHSNode(int isTerminal, int value) {
    RHSNode* newNode = (RHSNode*)malloc(sizeof(RHSNode));
    newNode->isTerminal = isTerminal;
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

// Function to create a new production rule
ProductionRule* createProduction() {
    ProductionRule* newProd = (ProductionRule*)malloc(sizeof(ProductionRule));
    newProd->head = NULL;
    newProd->next_rule = NULL;
    return newProd;
}

// Function to add a production rule to a Non-Terminal
void addProduction(Grammar* G, NonTerminal lhs, int* rhsArray, int rhsSize, int* isTerminalArray) {
    ProductionRule* newRule = createProduction();
    RHSNode* prev = NULL;

    for (int i = 0; i < rhsSize; i++) {
        RHSNode* newNode = createRHSNode(isTerminalArray[i], rhsArray[i]);
        if (!newRule->head) {
            newRule->head = newNode;
        } else {
            prev->next = newNode;
        }
        prev = newNode;
    }

    if (!G->rules[lhs]) {
        G->rules[lhs] = newRule;
    } else {
        ProductionRule* temp = G->rules[lhs];
        while (temp->next_rule) temp = temp->next_rule;
        temp->next_rule = newRule;
    }
}
// lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol

parseTreeNode* createParseTreeNode(TokenNode* tkn, stackNode* stk)
{
    parseTreeNode* treeNode = (parseTreeNode*) malloc(sizeof(parseTreeNode));
    treeNode->lineno = tkn->lineNumber;
    treeNode->isTerminal = stk->isTerminal;
    treeNode->lexeme = tkn->lexeme;
    treeNode->token = tkn->token;
    treeNode->ival = -1;
    treeNode->fval = -1;
    treeNode->children = NULL;
    treeNode->right = NULL;
    if(strcmp(terminalNames[stk->value], "TK_NUM") == 0)
    {
        treeNode->ival = tkn->ival;
    }
    if(strcmp(terminalNames[stk->value], "TK_RNUM") == 0)
    {
        treeNode->fval = tkn->fval;
    }
    return treeNode;
}


parseTree* initparseTree()
{
    parseTree* pt = (parseTree*) malloc(sizeof(parseTree));
    parseTreeNode* root = (parseTreeNode *) malloc(sizeof(parseTreeNode));
    root->lexeme = "---";
    root->children = NULL;
    root->isTerminal = 0;
    root->lineno = -1;
    root->right = NULL;
    root->token = PROGRAM;
    root->val = PROGRAM;
    root->ival = -1;
    root->fval = -1;
    root->parent = NULL;
    pt->root = root;
    return pt;
}

void insertTree(parseTreeNode* tn, parseTreeNode* parent)
{
    if(parent->children == NULL)
    {
        parent->children = tn;
    }
    else{
        parseTreeNode* temp = parent->children;
        while(temp->right != NULL)
        {
            temp = temp->right;
        }
        temp->right = tn;
        tn->parent = parent;
    }
}

parseTreeStack * createTreeStack(){
    parseTreeStack * stk = (parseTreeStack *)malloc(sizeof(parseTreeStack));
    stk->top = NULL;
    return stk;
}

void pushTreeStack(parseTreeStack * stk, treeStackNode * sn){
    sn -> next = stk -> top;
    stk -> top = sn;
}

treeStackNode * createTreeStackNode(parseTreeNode * node){
    treeStackNode * sn = (treeStackNode *)malloc(sizeof(treeStackNode));
    sn -> pt = node;
    sn -> next = NULL;
    return sn;
}

treeStackNode * popTreeStack(parseTreeStack * stk){
    if(!stk ->top){
        return NULL;
    }
    treeStackNode * sn = stk -> top;
    stk -> top = sn -> next;
    return sn;
}

// Function to initialize grammar rules
void initializeGrammar(Grammar* G) {
    for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
        G->rules[i] = NULL;
    }

    // Example production rule: <program> → <otherFunctions> <mainFunction>
    int program_rhs[] = {OTHER_FUNCTIONS, MAIN_FUNCTION};
    int program_term[] = {0, 0}; // 0 means non-terminal
    addProduction(G, PROGRAM, program_rhs, 2, program_term);

    // Example production rule: <mainFunction> → TK_MAIN <stmts> TK_END
    int mainFunction_rhs[] = {TK_MAIN, STMTS, TK_END};
    int mainFunction_term[] = {1, 0, 1}; // 1 means terminal, 0 means non-terminal
    addProduction(G, MAIN_FUNCTION, mainFunction_rhs, 3, mainFunction_term);

    // <otherFunctions> → <function> <otherFunctions>
    int otherFunctions_rhs1[] = {FUNCTION, OTHER_FUNCTIONS};
    int otherFunctions_term1[] = {0, 0};
    addProduction(G, OTHER_FUNCTIONS, otherFunctions_rhs1, 2, otherFunctions_term1);

    // <otherFunctions> → eps
    int otherFunctions_rhs2[] = {EPS};
    int otherFunctions_term2[] = {1};
    addProduction(G, OTHER_FUNCTIONS, otherFunctions_rhs2, 1, otherFunctions_term2);

    // <function> → TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END
    int function_rhs[] = {TK_FUNID, INPUT_PAR, OUTPUT_PAR, TK_SEM, STMTS, TK_END};
    int function_term[] = {1, 0, 0, 1, 0, 1};
    addProduction(G, FUNCTION, function_rhs, 6, function_term);

    // <input_par> → TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
    int input_par_rhs[] = {TK_INPUT, TK_PARAMETER, TK_LIST, TK_SQL, PARAMETER_LIST, TK_SQR};
    int input_par_term[] = {1, 1, 1, 1, 0, 1};
    addProduction(G, INPUT_PAR, input_par_rhs, 6, input_par_term);

    // <output_par> → TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR
    int output_par_rhs1[] = {TK_OUTPUT, TK_PARAMETER, TK_LIST, TK_SQL, PARAMETER_LIST, TK_SQR};
    int output_par_term1[] = {1, 1, 1, 1, 0, 1};
    addProduction(G, OUTPUT_PAR, output_par_rhs1, 6, output_par_term1);

    // <output_par> → eps
    int output_par_rhs2[] = {EPS};
    int output_par_term2[] = {1};
    addProduction(G, OUTPUT_PAR, output_par_rhs2, 1, output_par_term2);

    // <parameter_list> → <dataType> TK_ID <remaining_list>
    int parameter_list_rhs[] = {DATA_TYPE, TK_ID, REMAINING_LIST};
    int parameter_list_term[] = {0, 1, 0};
    addProduction(G, PARAMETER_LIST, parameter_list_rhs, 3, parameter_list_term);

    // <dataType> → <primitiveDatatype>
    int dataType_rhs1[] = {PRIMITIVE_DATATYPE};
    int dataType_term1[] = {0};
    addProduction(G, DATA_TYPE, dataType_rhs1, 1, dataType_term1);

    // <dataType> → <constructedDatatype>
    int dataType_rhs2[] = {CONSTRUCTED_DATATYPE};
    int dataType_term2[] = {0};
    addProduction(G, DATA_TYPE, dataType_rhs2, 1, dataType_term2);

    // <primitiveDatatype> → TK_INT
    int primitiveDatatype_rhs1[] = {TK_INT};
    int primitiveDatatype_term1[] = {1};
    addProduction(G, PRIMITIVE_DATATYPE, primitiveDatatype_rhs1, 1, primitiveDatatype_term1);

    // <primitiveDatatype> → TK_REAL
    int primitiveDatatype_rhs2[] = {TK_REAL};
    int primitiveDatatype_term2[] = {1};
    addProduction(G, PRIMITIVE_DATATYPE, primitiveDatatype_rhs2, 1, primitiveDatatype_term2);

    // <constructedDatatype> → TK_RECORD TK_RUID
    int constructedDatatype_rhs1[] = {TK_RECORD, TK_RUID};
    int constructedDatatype_term1[] = {1, 1};
    addProduction(G, CONSTRUCTED_DATATYPE, constructedDatatype_rhs1, 2, constructedDatatype_term1);

    // <constructedDatatype> → TK_UNION TK_RUID
    int constructedDatatype_rhs2[] = {TK_UNION, TK_RUID};
    int constructedDatatype_term2[] = {1, 1};
    addProduction(G, CONSTRUCTED_DATATYPE, constructedDatatype_rhs2, 2, constructedDatatype_term2);

    // <constructedDatatype> → TK_RUID
    int constructedDatatype_rhs3[] = {TK_RUID};
    int constructedDatatype_term3[] = {1};
    addProduction(G, CONSTRUCTED_DATATYPE, constructedDatatype_rhs3, 1, constructedDatatype_term3);

    // <remaining_list> → TK_COMMA <parameter_list>
    int remaining_list_rhs1[] = {TK_COMMA, PARAMETER_LIST};
    int remaining_list_term1[] = {1, 0};
    addProduction(G, REMAINING_LIST, remaining_list_rhs1, 2, remaining_list_term1);

    // <remaining_list> → eps
    int remaining_list_rhs2[] = {EPS};
    int remaining_list_term2[] = {1};
    addProduction(G, REMAINING_LIST, remaining_list_rhs2, 1, remaining_list_term2);

    // <stmts> → <typeDefinitions> <declarations> <otherStmts> <returnStmt>
    int stmts_rhs[] = {TYPE_DEFINITIONS, DECLARATIONS, OTHER_STMTS, RETURN_STMT};
    int stmts_term[] = {0, 0, 0, 0};
    addProduction(G, STMTS, stmts_rhs, 4, stmts_term);

    // <typeDefinitions> → <actualOrRedefined> <typeDefinitions>
    int typeDefinitions_rhs1[] = {ACTUAL_OR_REDEFINED, TYPE_DEFINITIONS};
    int typeDefinitions_term1[] = {0, 0};
    addProduction(G, TYPE_DEFINITIONS, typeDefinitions_rhs1, 2, typeDefinitions_term1);

    // <typeDefinitions> → eps
    int typeDefinitions_rhs2[] = {EPS};
    int typeDefinitions_term2[] = {1};
    addProduction(G, TYPE_DEFINITIONS, typeDefinitions_rhs2, 1, typeDefinitions_term2);

    // <actualOrRedefined> → <typeDefinition>
    int actualOrRedefined_rhs1[] = {TYPE_DEFINITION};
    int actualOrRedefined_term1[] = {0};
    addProduction(G, ACTUAL_OR_REDEFINED, actualOrRedefined_rhs1, 1, actualOrRedefined_term1);

    // <actualOrRedefined> → <definetypestmt>
    int actualOrRedefined_rhs2[] = {DEFINETYPE_STMT};
    int actualOrRedefined_term2[] = {0};
    addProduction(G, ACTUAL_OR_REDEFINED, actualOrRedefined_rhs2, 1, actualOrRedefined_term2);

    // <typeDefinition> → TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD
    int typeDefinition_rhs1[] = {TK_RECORD, TK_RUID, FIELD_DEFINITIONS, TK_ENDRECORD};
    int typeDefinition_term1[] = {1, 1, 0, 1};
    addProduction(G, TYPE_DEFINITION, typeDefinition_rhs1, 4, typeDefinition_term1);

    // <typeDefinition> → TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION
    int typeDefinition_rhs2[] = {TK_UNION, TK_RUID, FIELD_DEFINITIONS, TK_ENDUNION};
    int typeDefinition_term2[] = {1, 1, 0, 1};
    addProduction(G, TYPE_DEFINITION, typeDefinition_rhs2, 4, typeDefinition_term2);

    // <fieldDefinitions> → <fieldDefinition> <fieldDefinition> <moreFields>
    int fieldDefinitions_rhs[] = {FIELD_DEFINITION, FIELD_DEFINITION, MORE_FIELDS};
    int fieldDefinitions_term[] = {0, 0, 0};
    addProduction(G, FIELD_DEFINITIONS, fieldDefinitions_rhs, 3, fieldDefinitions_term);

    // <fieldDefinition> → TK_TYPE <fieldType> TK_COLON TK_FIELDID TK_SEM
    int fieldDefinition_rhs[] = {TK_TYPE, FIELD_TYPE, TK_COLON, TK_FIELDID, TK_SEM};
    int fieldDefinition_term[] = {1, 0, 1, 1, 1};
    addProduction(G, FIELD_DEFINITION, fieldDefinition_rhs, 5, fieldDefinition_term);

    // <fieldtype> → <primitiveDatatype>
    int fieldType_rhs1[] = {PRIMITIVE_DATATYPE};
    int fieldType_term1[] = {0};
    addProduction(G, FIELD_TYPE, fieldType_rhs1, 1, fieldType_term1);

    // <fieldtype> → <constructedDatatype>
    int fieldType_rhs2[] = {CONSTRUCTED_DATATYPE};
    int fieldType_term2[] = {0};
    addProduction(G, FIELD_TYPE, fieldType_rhs2, 1, fieldType_term2);

    // <moreFields> → <fieldDefinition> <moreFields>
    int moreFields_rhs1[] = {FIELD_DEFINITION, MORE_FIELDS};
    int moreFields_term1[] = {0, 0};
    addProduction(G, MORE_FIELDS, moreFields_rhs1, 2, moreFields_term1);

    // <moreFields> → eps
    int moreFields_rhs2[] = {EPS};
    int moreFields_term2[] = {1};
    addProduction(G, MORE_FIELDS, moreFields_rhs2, 1, moreFields_term2);

    // <declarations> → <declaration> <declarations>
    int declarations_rhs1[] = {DECLARATION, DECLARATIONS};
    int declarations_term1[] = {0, 0};
    addProduction(G, DECLARATIONS, declarations_rhs1, 2, declarations_term1);

    // <declarations> → eps
    int declarations_rhs2[] = {EPS};
    int declarations_term2[] = {1};
    addProduction(G, DECLARATIONS, declarations_rhs2, 1, declarations_term2);

    // <declaration> → TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM
    int declaration_rhs[] = {TK_TYPE, DATA_TYPE, TK_COLON, TK_ID, GLOBAL_OR_NOT, TK_SEM};
    int declaration_term[] = {1, 0, 1, 1, 0, 1};
    addProduction(G, DECLARATION, declaration_rhs, 6, declaration_term);

    // <global_or_not> → TK_COLON TK_GLOBAL
    int globalOrNot_rhs1[] = {TK_COLON, TK_GLOBAL};
    int globalOrNot_term1[] = {1, 1};
    addProduction(G, GLOBAL_OR_NOT, globalOrNot_rhs1, 2, globalOrNot_term1);

    // <global_or_not> → eps
    int globalOrNot_rhs2[] = {EPS};
    int globalOrNot_term2[] = {1};
    addProduction(G, GLOBAL_OR_NOT, globalOrNot_rhs2, 1, globalOrNot_term2);

    // <otherStmts> → <stmt> <otherStmts>
    int otherStmts_rhs1[] = {STMT, OTHER_STMTS};
    int otherStmts_term1[] = {0, 0};
    addProduction(G, OTHER_STMTS, otherStmts_rhs1, 2, otherStmts_term1);

    // <otherStmts> → eps
    int otherStmts_rhs2[] = {EPS};
    int otherStmts_term2[] = {1};
    addProduction(G, OTHER_STMTS, otherStmts_rhs2, 1, otherStmts_term2);

    // <stmt> → <assignmentStmt>
    int stmt_rhs1[] = {ASSIGNMENT_STMT};
    int stmt_term1[] = {0};
    addProduction(G, STMT, stmt_rhs1, 1, stmt_term1);

    // <stmt> → <iterativeStmt>
    int stmt_rhs2[] = {ITERATIVE_STMT};
    int stmt_term2[] = {0};
    addProduction(G, STMT, stmt_rhs2, 1, stmt_term2);

    // <stmt> → <conditionalStmt>
    int stmt_rhs3[] = {CONDITIONAL_STMT};
    int stmt_term3[] = {0};
    addProduction(G, STMT, stmt_rhs3, 1, stmt_term3);

    // <stmt> → <ioStmt>
    int stmt_rhs4[] = {IO_STMT};
    int stmt_term4[] = {0};
    addProduction(G, STMT, stmt_rhs4, 1, stmt_term4);

    // <stmt> → <funCallStmt>
    int stmt_rhs5[] = {FUN_CALL_STMT};
    int stmt_term5[] = {0};
    addProduction(G, STMT, stmt_rhs5, 1, stmt_term5);

    // <assignmentStmt> → <SingleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM
    int assignmentStmt_rhs[] = {SINGLE_OR_REC_ID, TK_ASSIGNOP, ARITHMETIC_EXPRESSION, TK_SEM};
    int assignmentStmt_term[] = {0, 1, 0, 1};
    addProduction(G, ASSIGNMENT_STMT, assignmentStmt_rhs, 4, assignmentStmt_term);

            // <singleOrRecId> → TK_ID <option_single_constructed>
    int singleOrRecId_rhs[] = {TK_ID, OPTION_SINGLE_CONSTRUCTED};
    int singleOrRecId_term[] = {1, 0};
    addProduction(G, SINGLE_OR_REC_ID, singleOrRecId_rhs, 2, singleOrRecId_term);

    // <option_single_constructed> → <oneExpansion> <moreExpansions>
    int optionSingleConstructed_rhs1[] = {ONE_EXPANSION, MORE_EXPANSIONS};
    int optionSingleConstructed_term1[] = {0, 0};
    addProduction(G, OPTION_SINGLE_CONSTRUCTED, optionSingleConstructed_rhs1, 2, optionSingleConstructed_term1);

    // <option_single_constructed> → eps
    int optionSingleConstructed_rhs2[] = {EPS};
    int optionSingleConstructed_term2[] = {1};
    addProduction(G, OPTION_SINGLE_CONSTRUCTED, optionSingleConstructed_rhs2, 1, optionSingleConstructed_term2);

    // <oneExpansion> → TK_DOT TK_FIELDID
    int oneExpansion_rhs[] = {TK_DOT, TK_FIELDID};
    int oneExpansion_term[] = {1, 1};
    addProduction(G, ONE_EXPANSION, oneExpansion_rhs, 2, oneExpansion_term);

    // <moreExpansions> → <oneExpansion> <moreExpansions>
    int moreExpansions_rhs1[] = {ONE_EXPANSION, MORE_EXPANSIONS};
    int moreExpansions_term1[] = {0, 0};
    addProduction(G, MORE_EXPANSIONS, moreExpansions_rhs1, 2, moreExpansions_term1);

    // <moreExpansions> → eps
    int moreExpansions_rhs2[] = {EPS};
    int moreExpansions_term2[] = {1};
    addProduction(G, MORE_EXPANSIONS, moreExpansions_rhs2, 1, moreExpansions_term2);

    // <funCallStmt> → <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM
    int funCallStmt_rhs[] = {OUTPUT_PARAMETERS, TK_CALL, TK_FUNID, TK_WITH, TK_PARAMETERS, INPUT_PARAMETERS, TK_SEM};
    int funCallStmt_term[] = {0, 1, 1, 1, 1, 0, 1};
    addProduction(G, FUN_CALL_STMT, funCallStmt_rhs, 7, funCallStmt_term);

    // <outputParameters> → TK_SQL <idList> TK_SQR TK_ASSIGNOP
    int outputParameters_rhs1[] = {TK_SQL, ID_LIST, TK_SQR, TK_ASSIGNOP};
    int outputParameters_term1[] = {1, 0, 1, 1};
    addProduction(G, OUTPUT_PARAMETERS, outputParameters_rhs1, 4, outputParameters_term1);

    // <outputParameters> → eps
    int outputParameters_rhs2[] = {EPS};
    int outputParameters_term2[] = {1};
    addProduction(G, OUTPUT_PARAMETERS, outputParameters_rhs2, 1, outputParameters_term2);

    // <inputParameters> → TK_SQL <idList> TK_SQR
    int inputParameters_rhs[] = {TK_SQL, ID_LIST, TK_SQR};
    int inputParameters_term[] = {1, 0, 1};
    addProduction(G, INPUT_PARAMETERS, inputParameters_rhs, 3, inputParameters_term);

    // <iterativeStmt> → TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE
    int iterativeStmt_rhs[] = {TK_WHILE, TK_OP, BOOLEAN_EXPRESSION, TK_CL, STMT, OTHER_STMTS, TK_ENDWHILE};
    int iterativeStmt_term[] = {1, 1, 0, 1, 0, 0, 1};
    addProduction(G, ITERATIVE_STMT, iterativeStmt_rhs, 7, iterativeStmt_term);

    // <conditionalStmt> → TK_IF TK_OP <booleanExpression> TK_CL TK_THEN <stmt> <otherStmts> <elsePart>
    int conditionalStmt_rhs[] = {TK_IF, TK_OP, BOOLEAN_EXPRESSION, TK_CL, TK_THEN, STMT, OTHER_STMTS, ELSE_PART};
    int conditionalStmt_term[] = {1, 1, 0, 1, 1, 0, 0, 0};
    addProduction(G, CONDITIONAL_STMT, conditionalStmt_rhs, 8, conditionalStmt_term);

    // <elsePart> → TK_ELSE <stmt> <otherStmts> TK_ENDIF
    int elsePart_rhs1[] = {TK_ELSE, STMT, OTHER_STMTS, TK_ENDIF};
    int elsePart_term1[] = {1, 0, 0, 1};
    addProduction(G, ELSE_PART, elsePart_rhs1, 4, elsePart_term1);

    // <elsePart> → TK_ENDIF
    int elsePart_rhs2[] = {TK_ENDIF};
    int elsePart_term2[] = {1};
    addProduction(G, ELSE_PART, elsePart_rhs2, 1, elsePart_term2);

    // <ioStmt> → TK_READ TK_OP <var> TK_CL TK_SEM
    int ioStmt_rhs1[] = {TK_READ, TK_OP, VAR, TK_CL, TK_SEM};
    int ioStmt_term1[] = {1, 1, 0, 1, 1};
    addProduction(G, IO_STMT, ioStmt_rhs1, 5, ioStmt_term1);

    // <ioStmt> → TK_WRITE TK_OP <var> TK_CL TK_SEM
    int ioStmt_rhs2[] = {TK_WRITE, TK_OP, VAR, TK_CL, TK_SEM};
    int ioStmt_term2[] = {1, 1, 0, 1, 1};
    addProduction(G, IO_STMT, ioStmt_rhs2, 5, ioStmt_term2);

    // <arithmeticExpression> → <term> <expPrime>
    int arithmeticExpression_rhs[] = {TERM, EXP_PRIME};
    int arithmeticExpression_term[] = {0, 0};
    addProduction(G, ARITHMETIC_EXPRESSION, arithmeticExpression_rhs, 2, arithmeticExpression_term);

    // <expPrime> → <lowPrecedenceOperators> <term> <expPrime>
    int expPrime_rhs1[] = {LOW_PRECEDENCE_OPERATORS, TERM, EXP_PRIME};
    int expPrime_term1[] = {0, 0, 0};
    addProduction(G, EXP_PRIME, expPrime_rhs1, 3, expPrime_term1);

    // <expPrime> → eps
    int expPrime_rhs2[] = {EPS};
    int expPrime_term2[] = {1};
    addProduction(G, EXP_PRIME, expPrime_rhs2, 1, expPrime_term2);

    // <term> → <factor> <termPrime>
    int term_rhs[] = {FACTOR, TERM_PRIME};
    int term_term[] = {0, 0};
    addProduction(G, TERM, term_rhs, 2, term_term);

    // <termPrime> → <highPrecedenceOperators> <factor> <termPrime>
    int termPrime_rhs1[] = {HIGH_PRECEDENCE_OPERATOR, FACTOR, TERM_PRIME};
    int termPrime_term1[] = {0, 0, 0};
    addProduction(G, TERM_PRIME, termPrime_rhs1, 3, termPrime_term1);

    // <termPrime> → eps
    int termPrime_rhs2[] = {EPS};
    int termPrime_term2[] = {1};
    addProduction(G, TERM_PRIME, termPrime_rhs2, 1, termPrime_term2);

    // <factor> → TK_OP <arithmeticExpression> TK_CL
    int factor_rhs1[] = {TK_OP, ARITHMETIC_EXPRESSION, TK_CL};
    int factor_term1[] = {1, 0, 1};
    addProduction(G, FACTOR, factor_rhs1, 3, factor_term1);

    // <factor> → <var>
    int factor_rhs2[] = {VAR};
    int factor_term2[] = {0};
    addProduction(G, FACTOR, factor_rhs2, 1, factor_term2);

    // <highPrecedenceOperator> → TK_MUL
    int highPrecedenceOperator_rhs1[] = {TK_MUL};
    int highPrecedenceOperator_term1[] = {1};
    addProduction(G, HIGH_PRECEDENCE_OPERATOR, highPrecedenceOperator_rhs1, 1, highPrecedenceOperator_term1);

    // <highPrecedenceOperator> → TK_DIV
    int highPrecedenceOperator_rhs2[] = {TK_DIV};
    int highPrecedenceOperator_term2[] = {1};
    addProduction(G, HIGH_PRECEDENCE_OPERATOR, highPrecedenceOperator_rhs2, 1, highPrecedenceOperator_term2);

    // <lowPrecedenceOperators> → TK_PLUS
    int lowPrecedenceOperators_rhs1[] = {TK_PLUS};
    int lowPrecedenceOperators_term1[] = {1};
    addProduction(G, LOW_PRECEDENCE_OPERATORS, lowPrecedenceOperators_rhs1, 1, lowPrecedenceOperators_term1);

    // <lowPrecedenceOperators> → TK_MINUS
    int lowPrecedenceOperators_rhs2[] = {TK_MINUS};
    int lowPrecedenceOperators_term2[] = {1};
    addProduction(G, LOW_PRECEDENCE_OPERATORS, lowPrecedenceOperators_rhs2, 1, lowPrecedenceOperators_term2);

    // <booleanExpression> → TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL
    int booleanExpression_rhs1[] = {TK_OP, BOOLEAN_EXPRESSION, TK_CL, LOGICAL_OP, TK_OP, BOOLEAN_EXPRESSION, TK_CL};
    int booleanExpression_term1[] = {1, 0, 1, 0, 1, 0, 1};
    addProduction(G, BOOLEAN_EXPRESSION, booleanExpression_rhs1, 7, booleanExpression_term1);

    // <booleanExpression> → <var> <relationalOp> <var>
    int booleanExpression_rhs2[] = {VAR, RELATIONAL_OP, VAR};
    int booleanExpression_term2[] = {0, 0, 0};
    addProduction(G, BOOLEAN_EXPRESSION, booleanExpression_rhs2, 3, booleanExpression_term2);

    // <booleanExpression> → TK_NOT TK_OP <booleanExpression> TK_CL
    int booleanExpression_rhs3[] = {TK_NOT, TK_OP, BOOLEAN_EXPRESSION, TK_CL};
    int booleanExpression_term3[] = {1, 1, 0, 1};
    addProduction(G, BOOLEAN_EXPRESSION, booleanExpression_rhs3, 4, booleanExpression_term3);

    // <var> → <singleOrRecId>
    int var_rhs1[] = {SINGLE_OR_REC_ID};
    int var_term1[] = {0};
    addProduction(G, VAR, var_rhs1, 1, var_term1);

    // <var> → TK_NUM
    int var_rhs2[] = {TK_NUM};
    int var_term2[] = {1};
    addProduction(G, VAR, var_rhs2, 1, var_term2);

    // <var> → TK_RNUM
    int var_rhs3[] = {TK_RNUM};
    int var_term3[] = {1};
    addProduction(G, VAR, var_rhs3, 1, var_term3);

    // <logicalOp> → TK_AND
    int logicalOp_rhs1[] = {TK_AND};
    int logicalOp_term1[] = {1};
    addProduction(G, LOGICAL_OP, logicalOp_rhs1, 1, logicalOp_term1);

    // <logicalOp> → TK_OR
    int logicalOp_rhs2[] = {TK_OR};
    int logicalOp_term2[] = {1};
    addProduction(G, LOGICAL_OP, logicalOp_rhs2, 1, logicalOp_term2);

    // <relationalOp> → TK_LT
    int relationalOp_rhs1[] = {TK_LT};
    int relationalOp_term1[] = {1};
    addProduction(G, RELATIONAL_OP, relationalOp_rhs1, 1, relationalOp_term1);

    // <relationalOp> → TK_LE
    int relationalOp_rhs2[] = {TK_LE};
    int relationalOp_term2[] = {1};
    addProduction(G, RELATIONAL_OP, relationalOp_rhs2, 1, relationalOp_term2);

    // <relationalOp> → TK_EQ
    int relationalOp_rhs3[] = {TK_EQ};
    int relationalOp_term3[] = {1};
    addProduction(G, RELATIONAL_OP, relationalOp_rhs3, 1, relationalOp_term3);

    // <relationalOp> → TK_GT
    int relationalOp_rhs4[] = {TK_GT};
    int relationalOp_term4[] = {1};
    addProduction(G, RELATIONAL_OP, relationalOp_rhs4, 1, relationalOp_term4);

    // <relationalOp> → TK_GE
    int relationalOp_rhs5[] = {TK_GE};
    int relationalOp_term5[] = {1};
    addProduction(G, RELATIONAL_OP, relationalOp_rhs5, 1, relationalOp_term5);

    // <relationalOp> → TK_NE
    int relationalOp_rhs6[] = {TK_NE};
    int relationalOp_term6[] = {1};
    addProduction(G, RELATIONAL_OP, relationalOp_rhs6, 1, relationalOp_term6);

    // <returnStmt> → TK_RETURN <optionalReturn> TK_SEM
    int returnStmt_rhs[] = {TK_RETURN, OPTIONAL_RETURN, TK_SEM};
    int returnStmt_term[] = {1, 0, 1};
    addProduction(G, RETURN_STMT, returnStmt_rhs, 3, returnStmt_term);

    // <optionalReturn> → TK_SQL <idList> TK_SQR
    int optionalReturn_rhs1[] = {TK_SQL, ID_LIST, TK_SQR};
    int optionalReturn_term1[] = {1, 0, 1};
    addProduction(G, OPTIONAL_RETURN, optionalReturn_rhs1, 3, optionalReturn_term1);

    // <optionalReturn> → eps
    int optionalReturn_rhs2[] = {EPS};
    int optionalReturn_term2[] = {1};
    addProduction(G, OPTIONAL_RETURN, optionalReturn_rhs2, 1, optionalReturn_term2);

    // <idList>===> TK_ID <more_ids>
    // <more_ids>===> TK_COMMA <idList> | eps
    // <definetypestmnt> ===> TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
    // <A> ==> TK_RECORD | TK_UNION

    // <idList> → TK_ID <more_ids>
    int idList_rhs[] = {TK_ID, MORE_IDS};
    int idList_term[] = {1, 0};
    addProduction(G, ID_LIST, idList_rhs, 2, idList_term);

    // <more_ids> → TK_COMMA <idList>
    int moreIds_rhs1[] = {TK_COMMA, ID_LIST};
    int moreIds_term1[] = {1, 0};
    addProduction(G, MORE_IDS, moreIds_rhs1, 2, moreIds_term1);

    // <more_ids> → eps
    int moreIds_rhs2[] = {EPS};
    int moreIds_term2[] = {1};
    addProduction(G, MORE_IDS, moreIds_rhs2, 1, moreIds_term2);

    // <definetypestmnt> → TK_DEFINETYPE <A> TK_RUID TK_AS TK_RUID
    int definetypeStmt_rhs[] = {TK_DEFINETYPE, A, TK_RUID, TK_AS, TK_RUID};
    int definetypeStmt_term[] = {1, 0, 1, 1, 1};
    addProduction(G, DEFINETYPE_STMT, definetypeStmt_rhs, 5, definetypeStmt_term);

     // <A> → TK_RECORD
    int A_rhs1[] = {TK_RECORD};
    int A_term1[] = {1};
    addProduction(G, A, A_rhs1, 1, A_term1);

    // <A> → TK_UNION
    int A_rhs2[] = {TK_UNION};
    int A_term2[] = {1};
    addProduction(G, A, A_rhs2, 1, A_term2);


}

// Function to print the grammar rules in a readable format
void printGrammar(Grammar* G) {
    for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
        if (!G->rules[i]) continue;

        printf("%s -> ", nonTerminalNames[i]);
        ProductionRule* rule = G->rules[i];

        while (rule) {
            RHSNode* node = rule->head;
            while (node) {
                printf("%s ", node->isTerminal ? terminalNames[node->value] : nonTerminalNames[node->value]);
                node = node->next;
            }
            if (rule->next_rule) printf("| "); // Only print '|' if more rules exist
            rule = rule->next_rule;
        }
        printf("\n");
    }
}

// Recursive Function to Compute First Set
void computeFirst(int nt, Grammar* G) {
    if (computedFirst[nt]) return; // Avoid recomputation
    computedFirst[nt] = true;

    ProductionRule* rule = G->rules[nt];
    while (rule) {
        RHSNode* node = rule->head;
        bool allNullable = true; // Check if all symbols in RHS derive epsilon
        while (node) {
            if (node->isTerminal) {
                addToSet(&firstSet[nt], node->value);
                allNullable = false;
                break;
            } else {
                int nextNT = node->value;
                computeFirst(nextNT, G); // Recursive call
                unionSets(&firstSet[nt], &firstSet[nextNT]);
                if (!contains(&firstSet[nextNT], EPS)){
                    allNullable = false;
                    break;
                }
            }
            node = node->next;
        }
        if (allNullable) {
            addToSet(&firstSet[nt], EPS);
        }
        rule = rule->next_rule;
    }
}

void computeAllFirstSets(Grammar* G) {
    memset(computedFirst, 0, sizeof(computedFirst));
    for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
        computeFirst(i, G);
    }
}

void computeFollowSet(Grammar* G) {
    addToSet(&followSet[PROGRAM], DOLLAR); // Rule 1: Add $ to FOLLOW(start symbol)

    bool changed = true;
    while (changed) {
        changed = false;
        
        for (int nt = 0; nt < NON_TERMINAL_COUNT; nt++) {
            ProductionRule* rule = G->rules[nt];
            
            while (rule) {
                RHSNode* node = rule->head;

                while (node) {
                    if (!node->isTerminal) {  // Only process non-terminals
                        RHSNode* nextNode = node->next;
                        bool allEpsilon = true;  // Track if all following symbols derive ε

                        while (nextNode) {
                            if (nextNode->isTerminal) {
                                // Directly add terminal to FOLLOW(B)
                                if (!contains(&followSet[node->value], nextNode->value)) {
                                    addToSet(&followSet[node->value], nextNode->value);
                                    changed = true;
                                }
                                allEpsilon = false; // Terminals do not derive ε
                                break;
                            } else {
                                // Add FIRST(nextNode) - {ε} to FOLLOW(B)
                                HashSet prevSet = followSet[node->value];
                                unionSets(&followSet[node->value], &firstSet[nextNode->value]);
                                removeFromSet(&followSet[node->value], EPS); // Exclude ε

                                if (!contains(&firstSet[nextNode->value], EPS)) {
                                    allEpsilon = false; // Stop if this does not derive ε
                                    break;
                                }

                                // If set changed, mark for another iteration
                                if (memcmp(&prevSet, &followSet[node->value], sizeof(HashSet)) != 0) {
                                    changed = true;
                                }
                            }
                            nextNode = nextNode->next;
                        }

                        // If all following symbols derive ε, add FOLLOW(A) to FOLLOW(B)
                        if (allEpsilon) {
                            HashSet prevSet = followSet[node->value];
                            unionSets(&followSet[node->value], &followSet[nt]);
                            if (memcmp(&prevSet, &followSet[node->value], sizeof(HashSet)) != 0) {
                                changed = true;
                            }
                        }
                    }
                    node = node->next;
                }
                rule = rule->next_rule;
            }
        }
    }
}



// Function to print First and Follow Sets
void printSet(HashSet *set, const char* name) {
    printf("%s: { ", name);
    for (int i = 0; i < TERMINAL_COUNT; i++) {
        if (set->elements[i]) {
            printf("%s ", terminalNames[i]);
        }
    }
    printf("}\n");
}

void printFirstFollow() {
    for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
        printf("First(%s) ", nonTerminalNames[i]);
        printSet(&firstSet[i], "");
        printf("Follow(%s) ", nonTerminalNames[i]);
        printSet(&followSet[i], "");
    }
}

void computeFirstSetForRHS(RHSNode* node, HashSet* firstSetRHS) {
    initHashSet(firstSetRHS);  // Assuming there's a function to initialize the set

    while (node) {
        if (node->isTerminal) {
            addToSet(firstSetRHS, node->value);
            return;  // Stop processing since terminal means no further derivation
        } else {
            unionSets(firstSetRHS, &firstSet[node->value]);
            if (!contains(&firstSet[node->value], EPS)) {
                return;  // Stop if EPS is not in FIRST(node)
            }
        }
        node = node->next;
    }
    
    addToSet(firstSetRHS, EPS);  // If all symbols in RHS can derive EPS, add EPS
}


void constructParseTable(Grammar* G, RHSNode* parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT]) {
    memset(parse_table, 0, sizeof(RHSNode*) * NON_TERMINAL_COUNT * TERMINAL_COUNT);

    for (int nt = 0; nt < NON_TERMINAL_COUNT; nt++) {
        ProductionRule* rule = G->rules[nt];
        while (rule) {
            RHSNode* node = rule->head;
            HashSet firstAlpha;
            computeFirstSetForRHS(node, &firstAlpha);  // Compute FIRST set for RHS

            // Step 1: Add production for each terminal in FIRST(α)
            for (int t = 0; t < TERMINAL_COUNT; t++) {
                if (contains(&firstAlpha, t)) {
                    parse_table[nt][t] = node;
                }
            }

            // Step 2: Handle epsilon case
            if (contains(&firstAlpha, EPS)) {
                for (int t = 0; t < TERMINAL_COUNT; t++) {
                    if (contains(&followSet[nt], t)) {
                        parse_table[nt][t] = node;
                    }
                }
            }
            rule = rule->next_rule;
        }
        for(int i = 0;i<TERMINAL_COUNT;i++)
        {
            if(!parse_table[nt][i])
            {
                RHSNode * synchnode = (RHSNode *) malloc(sizeof(RHSNode));
                synchnode -> isTerminal = 0;
                synchnode -> value = -1;
                synchnode -> next = NULL;
                parse_table[nt][i] = synchnode;
            }
        }
    }

}

// lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol

// The lexeme of the current node is printed when it is the leaf node else a dummy string of characters "‐‐‐‐" is printed. 
// The line number is one of the information collected by the lexical analyzer during single pass of the source code. 
// The token name corresponding to the current node is printed third.
//  If the lexeme is an integer or real number, then its value computed by the lexical analyzer should be printed at the fourth place. 
//  Print the grammar symbol (non-terminal symbol) of the parent node of the currently visited node appropriately at fifth place 
//  (for the root node print ROOT for parent symbol) . The sixth column is for printing yes or no appropriately. 
//  Print the non-terminal symbol of the node being currently visited at the 7th place, 
//  if the node is not the leaf node [Print the actual non-terminal symbol and not the enumerated values for the non-terminal]. 
//  Ensure appropriate justification so that the columns appear neat and straight.

bool parse(TokenList * tk_list, RHSNode* parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT], stack * mainStack, parseTreeStack* ptS, parseTree * pt){
    lookAheadPtr = tk_list -> head;
    stackNode * sn = (stackNode *)malloc(sizeof(stackNode));
    sn -> isTerminal = 0;
    sn -> value = PROGRAM;
    sn -> next = NULL;
    push(mainStack, sn);
    treeStackNode * tsN = createTreeStackNode(pt->root);
    pushTreeStack(ptS, tsN);

    while(lookAheadPtr){
        stackNode * topSymbol = pop(mainStack);
        parseTreeNode * parent = popTreeStack(ptS)->pt;
        // printf("Top Symbol value = %s, Lookahead pointer value = %s\n", topSymbol -> isTerminal? terminalNames[topSymbol -> value]:nonTerminalNames[topSymbol-> value], terminalNames[lookAheadPtr -> token]);
        if(topSymbol -> value == DOLLAR){
            return false;
        }
        if(topSymbol -> isTerminal){
            int T = topSymbol -> value;
            if(T == lookAheadPtr -> token){
                parent->lexeme = lookAheadPtr->lexeme;
                parent->lineno = lookAheadPtr->lineNumber;
                if(lookAheadPtr->token == TK_NUM)
                {
                    parent->ival = lookAheadPtr->ival;
                }
                else if(lookAheadPtr->token == TK_RNUM)
                {
                    parent->fval = lookAheadPtr->fval;
                }
                lookAheadPtr = lookAheadPtr -> next;
            }
            else{
                // REPORT THE error condition
                printf("ERROR FOUND on line number %d: %s EXPECTED, GOT %s. MOVING AHEAD...\n", lookAheadPtr -> lineNumber, terminalNames[T], terminalNames[ lookAheadPtr -> token]);
                //As the input symbol and the top of the stack do not match and the top of the stack is terminal, we just pop the stack.
                continue;
            }
        }
        else{
            int NT = topSymbol -> value;
            int T = lookAheadPtr -> token;
            if(!parse_table[NT][T]){
                // add error condition with line number
                printf("ERROR FOUND on line number %d: Cannot match %s, Token Discarded. Moving ahead...\n", lookAheadPtr -> lineNumber, terminalNames[T]);

                //as the parse table entry is empty, we just skip the input symbol and move ahead.
                lookAheadPtr = lookAheadPtr -> next;
                treeStackNode * k = createTreeStackNode(parent);
                pushTreeStack(ptS, k);
                push(mainStack, topSymbol);
            }
            else if(parse_table[NT][T]->value == -1){
                // the value of the parse table entry is 'synch' or -1. According to panic mode error recovery we pop the stack
                printf("ERROR FOUND on line number %d: Recovering from error, Popping stack and moving ahead \n", lookAheadPtr -> lineNumber);
                continue;
            }
            else{
                if(parse_table[NT][T] -> value != EPS) 
                {
                    addToStack(mainStack, parse_table[NT][T]);
                    RHSNode* rhs = parse_table[NT][T];
                    parseTreeStack * temp_stack = (parseTreeStack*) malloc(sizeof(parseTreeStack));
                    temp_stack->top = NULL;
                    while(rhs != NULL)
                    {
                        parseTreeNode * ptn = (parseTreeNode*) malloc(sizeof(parseTreeNode));
                        ptn->isTerminal = rhs->isTerminal;
                        ptn->val = rhs->value;
                        ptn->children = NULL;
                        ptn->right = NULL;
                        ptn -> parent = parent;
                        treeStackNode* temp_node = createTreeStackNode(ptn);
                        pushTreeStack(temp_stack, temp_node);
                        insertTree(ptn, parent);
                        rhs = rhs->next;
                    }
                    treeStackNode* t;
                    while(t = popTreeStack(temp_stack))
                    {
                        pushTreeStack(ptS, t);
                    }
                }
            }
        }
        //printf("Parent node value: %s Parent node lexeme: %s\n", parent -> isTerminal? terminalNames[parent -> val]:nonTerminalNames[parent -> val], parent->lexeme);
    }

    while(mainStack -> top -> value != DOLLAR){
        if(parse_table[mainStack -> top -> value][DOLLAR] -> value == EPS){
            printf("Popping : %s\n", terminalNames[pop(mainStack) -> value]);
        }
        else{
            printf("Error : Stack still contains NTs\n");
            return false;
        }
    }

    return true;
}

void printParseTable(RHSNode * parse_table[NON_TERMINAL_COUNT][TERMINAL_COUNT]){
    for(int i = 0; i < NON_TERMINAL_COUNT; i++){
        for(int j = 0; j < TERMINAL_COUNT; j++){
            RHSNode * prod_rule = parse_table[i][j];
            printf("M[%s][%s] :", nonTerminalNames[i], terminalNames[j]);
            while (prod_rule) {
                printf("%s ", prod_rule -> isTerminal ? terminalNames[prod_rule->value] : nonTerminalNames[prod_rule->value]);
                prod_rule = prod_rule -> next;
            }
            printf("\n");
        }
    }
}

TokenNode *createTokenNode(Terminal token, const char *lexeme, int lineNumber) {
    TokenNode *newNode = (TokenNode *)malloc(sizeof(TokenNode));
    if (!newNode) {
        printf("Memory allocation failed for TokenNode\n");
        exit(EXIT_FAILURE);
    }
    newNode->token = token;
    newNode->lexeme = strdup(lexeme);
    newNode->lineNumber = lineNumber;
    newNode->next = NULL;
    return newNode;
}

// Function to add a token to the TokenList
void addToken(TokenList *list, int lineNumber, const char *lexeme, int token) {
    TokenNode *newNode = createTokenNode(token, lexeme, lineNumber);
    if (!list->head) {
        list->head = newNode;
    } else {
        TokenNode *temp = list->head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    list->count++;
}
// lexeme CurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
void inorderTraversal(parseTreeNode* node,FILE* parseTreeFile) {
    if (node == NULL) return;
    // Print current node details
    if(node->isTerminal == 1)
    {
        fprintf(parseTreeFile,"lexeme: %s CurrentNode line_number: %d tokenName: %s",
           node->lexeme,
           node->lineno,
           terminalNames[node-> val]);
        if (node->val == TK_NUM) {
        fprintf(parseTreeFile," ValueIfNumber: %d ", node->ival);
        } 
        else if (node->val == TK_RNUM) {
        fprintf(parseTreeFile," ValueIfNumber: %.2f ", node->fval);
        } 
        else {
        fprintf(parseTreeFile," -- ");
        }   
        fprintf(parseTreeFile," parentNodeSymbol: %s isLeafNode: yes",
           node->parent ? nonTerminalNames[node->parent->val]: "ROOT");
        fprintf(parseTreeFile, " nodeSymbol: ---\n" );
    }
    else{
        inorderTraversal(node->children,parseTreeFile);
        fprintf(parseTreeFile,"lexeme: --- CurrentNode tokenName: --- ");
        
        fprintf(parseTreeFile," ValueIfNumber: -- ");
        
        fprintf(parseTreeFile," parentNodeSymbol: %s isLeafNode: no",
           node->parent ? nonTerminalNames[node->parent->val]: "ROOT");
        fprintf(parseTreeFile, " nodeSymbol: %s\n", nonTerminalNames[node->val]);
    }

    // Visit the remaining children
    parseTreeNode* sibling = node->children ? node->children->right : NULL;
    while (sibling) {
        inorderTraversal(sibling,parseTreeFile);
        sibling = sibling->right;
    }
}

void parseAndPrintErrors(TokenList * tkns,FILE* parseTreeOutFile)
{
    TokenNode * h = tkns->head;
    while(!h)
    {
        printf("%s  ", h->lexeme);
        h = h->next;
    }
    Grammar G;
    initializeGrammar(&G);
    // printGrammar(&G);

    for (int i = 0; i < NON_TERMINAL_COUNT; i++) {
        initHashSet(&firstSet[i]);
        initHashSet(&followSet[i]);
    }    

    computeAllFirstSets(&G);
    computeFollowSet(&G);
    // printFirstFollow();

    constructParseTable(&G, parse_table);
    // printParseTable(parse_table);

    stack * mainStack = initMainStack();
    parseTreeStack* TreeStack = createTreeStack();
    parseTree * parsetree = initparseTree();
    FILE* parseTreeFile = fopen(parseTreeOutFile,"w");
    parse(tkns, parse_table, mainStack,  TreeStack, parsetree);
    inorderTraversal(parsetree->root,parseTreeFile);
    fclose(parseTreeFile);
}