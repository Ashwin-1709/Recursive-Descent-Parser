#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS (1000000)
#define MAX_LINE_LENGTH (1000000)

char tokens[100000][50];
char *variables[100000];
int variable_values[100000];
int cur_pos = 0, token_pos = 0, end_pos = 0, var_pos = 0;

typedef struct Node {
    char val[50];
    int child_cnt;
    struct Node *child[15];
} Node;

bool isConstant() {
    char *token = tokens[cur_pos];
    for (int i = 0; i < 50; i++) {
        if (token[i] == 0)
            break;
        if (!isdigit(token[i]))
            return false;
    }
    return true;
}

/*Function Signatures*/

Node *parseExpression(int start, int end);
Node *parseStatement();
Node *parseRead();
Node *parseWrite();
Node *parseAssignment();
Node *parseForLoop();
Node *parseT1(int start, int end);
Node *parseT2(int start, int end);
Node *parseT3(int start, int end);
void simulateRead(Node *root);
void simulateWrite(Node *root);
int simulateExpression(Node *root);
void simulateAssignment(Node *root);
void simulateAssignment(Node *root);
void simulateFor(Node *root);
void simulateStatement(Node *root);
void simulateProgram(Node *root);
int get_end(int pos);
void error(char *msg);

/**
Reads the passed input file line by line.
Removes all space type characters from the lines, including \n (Carriage
Return). Modifies the global tokens and num_tokens variables.
*/

// We need to split on delimiters , while pushing certain delimiters as tokens
// , ( ) { } ; -> to be pushed
// \n whitespace \t \0 -> not be pushed

int charCheck(char c) {
    if (c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}')
        return 1;
    else if (c == '\n' || c == '\t' || c == '\0' || c == ' ' || c == 0 ||
             c == 10 || c == 13)
        return 2;
    return 0;
}

char operators[6] = {'+', '-', '*', '/', '>', '='};

int tokenize(char *filename) {
    FILE *in_file = fopen(filename, "r");
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, in_file)) {
        for (int i = 0; i < strlen(line); i++) {
            bool isoperator = false;
            for (int j = 0; j < 6; j++) {
                if (operators[j] == line[i]) {
                    if (operators[j] == '=' && cur_pos > 0 &&
                        tokens[cur_pos - 1][0] == '=' && line[i - 1] == '=') {
                        tokens[cur_pos - 1][1] = '=';
                        isoperator = true;
                        break;
                    }
                    if (token_pos > 0) {
                        tokens[++cur_pos][0] = operators[j];
                        token_pos = 0;
                        cur_pos++;
                    } else {
                        tokens[cur_pos][0] = operators[j];
                        token_pos = 0;
                        cur_pos++;
                    }
                    isoperator = true;
                    break;
                }
            }

            if (isoperator)
                continue;

            int type = charCheck(line[i]);
            if (type == 2) {
                if (token_pos > 0)
                    cur_pos++;
                token_pos = 0;
            } else if (type == 1) {
                if (token_pos > 0)
                    cur_pos++;
                tokens[cur_pos][0] = line[i];
                cur_pos++;
                token_pos = 0;
            } else
                tokens[cur_pos][token_pos++] = line[i];
        }
    }

    return cur_pos;
}

void addChild(Node *par, Node *ch) {
    if (ch == NULL)
        return;

    int c = par->child_cnt++;
    Node **p = par->child;
    p[c] = ch;
}

int get_end(int pos) {
    int end = pos, bracket = 0;
    while (true) {

        if (end == end_pos) {
            error("Not a well formed expression");
        }

        if ((strcmp(tokens[end], "(")) == 0)
            bracket++;
        else if ((strcmp(tokens[end], ")")) == 0)
            bracket--;
        if ((bracket < 0) || (strcmp(tokens[end], ";") == 0))
            return end - 1;
        end++;
    }
}

void error(char* msg) {
    fprintf(stderr,"Error while parsing : %s\n", msg);
    fprintf(stderr, "Error occurred\n");
    exit(EXIT_FAILURE);
}

Node *createNode() {
    Node *new_node = (Node *)(malloc(sizeof(Node)));
    new_node->child_cnt = 0;
    for (int i = 0; i < 50; i++)
        new_node->val[i] = 0;
    return new_node;
}

void printTree(struct Node *root) {
    printf("[%s ", root->val);
    int c = root->child_cnt;

    for (int i = 0; i < c; i++)
        printTree(root->child[i]);

    printf("]");
}

// all identifier functions are defined here

bool isDeclaration() {
    if (strcmp("int", tokens[cur_pos]) == 0)
        return true;

    return false;
}

bool isVariable(char *v) {

    if (strcmp(v, "for") == 0)
        return false;

    if (strcmp(v, "int") == 0)
        return false;

    if (strcmp(v, "read") == 0)
        return false;

    if (strcmp(v, "write") == 0)
        return false;

    int l = strlen(v);

    for (int i = 0; i < l; i++) {
        if (v[i] < 'a' || v[i] > 'z')
            return false;
    }

    return true;
}

// all parser functions are defined here

Node *parseTerminal() {
    Node *t = createNode();
    strcpy(t->val, tokens[cur_pos++]);
    return t;
}

Node *parseVariable() {    

    if (isVariable(tokens[cur_pos]))
    {
        Node *I = createNode();
        strcpy(I->val, "I");
        addChild(I, parseTerminal());
        return I;
    }
    else
    {
        error(strcat(tokens[cur_pos]," is not a variable."));
        return NULL;
    }
}

Node *parseVariableList() {
    Node *L = createNode();
    strcpy(L->val, "L");

    if (isVariable(tokens[cur_pos])) {
        bool exists = false;
        for (int i = 0; i < var_pos; i++) {
            if (!strcmp(variables[i], tokens[cur_pos])) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            variables[var_pos] = tokens[cur_pos];
            variable_values[var_pos] = 0;
            var_pos++;
        } 
        addChild(L, parseVariable());
    } else {
        error("Not a variable.");
        return NULL;
    }

    if (strcmp(tokens[cur_pos], ",") == 0) {
        addChild(L, parseTerminal());
        Node *x = parseVariableList();
        addChild(L, x);
        return L;
    } else if (strcmp(tokens[cur_pos], ";") == 0)
        return L;
    else {
        error("Incorrect delimiter in Variable List.");
        return NULL;
    }
}

Node *parseDeclaration() {
    Node *D = createNode();
    strcpy(D->val, "D");

    addChild(D, parseTerminal());
    addChild(D, parseVariableList());
    return D;
}

Node *parseProgram() {
    // end of program

    if (cur_pos == end_pos)
        return NULL;

    // ignore empty statements

    if (strcmp(tokens[cur_pos], ";") == 0) {
        cur_pos++;
        return parseProgram();
    }

    Node *P = createNode();
    strcpy(P->val, "P");

    if (isDeclaration()) {
        addChild(P, parseDeclaration());
	
	if (strcmp(tokens[cur_pos], ";") == 0)
            addChild(P, parseTerminal());
        else {
            error("Incorrect delimiter after declaration");
            return NULL;
        }
    }   

    addChild(P, parseStatement());
    return P;
}

Node *parseStatement() {
    // end of program

    if (cur_pos == end_pos)
        return NULL;

    // ignore empty statements

    if (strcmp(tokens[cur_pos], ";") == 0) {
        cur_pos++;
        return parseStatement();
    }

    Node *S = createNode();
    strcpy(S->val, "S");
    if (strcmp(tokens[cur_pos], "read") == 0) {
        addChild(S, parseRead());
    } else if (strcmp(tokens[cur_pos], "write") == 0) {
        addChild(S, parseWrite());
    } else if (strcmp(tokens[cur_pos + 1], "=") == 0) {
        addChild(S, parseAssignment());
    } else if (strcmp(tokens[cur_pos], "for") == 0) {
        addChild(S, parseForLoop());
    } else {
        error("Not a statement.");
        return NULL;
    }
    if (strcmp(tokens[cur_pos], ";") == 0)
        addChild(S, parseTerminal());
    else {
        error("Missing semicolon.");
        return NULL;
    }
    if (strcmp(tokens[cur_pos], "}") != 0)
        addChild(S, parseStatement());
    return S;
}

Node *parseExpression(int start, int end) {
    // printf("start = %d and end = %d\n" , start , end);
    assert(start <= end);
    Node *E = createNode();
    strcpy(E->val , "E");
    int lst = -1 , bracket = 0;
    for(int i = start ; i < end ; i++) {
        if((strcmp(tokens[i] , ">") == 0) || 
            (strcmp(tokens[i] , "==")) == 0) {
                if(bracket == 0) 
                    lst = i;
            }
        if((strcmp(tokens[i] , "(")) == 0)
            bracket++;
        else if ((strcmp(tokens[i], ")")) == 0)
            bracket--;
    }
    if (lst == -1) {
        addChild(E, parseT1(start, end));
    } else {
        addChild(E, parseExpression(start, lst - 1));
        // printf("Parsing %s\n" , tokens[cur_pos]);
        addChild(E, parseTerminal());
        addChild(E, parseT1(lst + 1, end));
    }
    return E;
}

Node* parseT1(int start , int end) {
    // printf("start = %d and end = %d\n" , start , end);
    assert(start <= end);
    Node *T1 = createNode();
    strcpy(T1->val , "T1");
    int lst = -1 , bracket = 0;
    for(int i = start ; i < end ; i++) {
        if((strcmp(tokens[i] , "+") == 0) || 
            (strcmp(tokens[i] , "-")) == 0) {
                if(bracket == 0) 
                    lst = i;
            }
        if((strcmp(tokens[i] , "(")) == 0)
            bracket++;
        else if((strcmp(tokens[i] , ")")) == 0)
            bracket--;
    }
    if (lst == -1) {
        addChild(T1, parseT2(start, end));
    } else {
        addChild(T1, parseT1(start, lst - 1));
        // printf("Parsing %s\n" , tokens[cur_pos]);
        addChild(T1, parseTerminal());
        addChild(T1, parseT2(lst + 1, end));
    }
    return T1;
}

Node *parseT2(int start, int end) {
    // printf("start = %d and end = %d\n" , start , end);
    assert(start <= end);
    Node *T2 = createNode();
    strcpy(T2->val , "T2");
    int lst = -1 , bracket = 0;
    for(int i = start ; i < end ; i++) {
        if((strcmp(tokens[i] , "*") == 0) || 
            (strcmp(tokens[i] , "/")) == 0) {
                if(bracket == 0) 
                    lst = i;
            }
        if((strcmp(tokens[i] , "(")) == 0)
            bracket++;
        else if ((strcmp(tokens[i], ")")) == 0)
            bracket--;
    }
    if (lst == -1) {
        addChild(T2, parseT3(start, end));
    } else {
        addChild(T2, parseT2(start, lst - 1));
        // printf("Parsing %s\n" , tokens[cur_pos]);
        addChild(T2, parseTerminal());
        addChild(T2, parseT3(lst + 1, end));
    }
    return T2;
}

Node *parseT3(int start, int end) {
    // printf("start = %d and end = %d\n" , start , end);
    assert(start <= end);
    Node *T3 = createNode();
    strcpy(T3->val, "T3");
    if (strcmp(tokens[cur_pos], "(") == 0) {
        // printf("Parsing %s\n" , tokens[cur_pos]);
        addChild(T3, parseTerminal());
        addChild(T3, parseExpression(start + 1, end));
        // printf("Parsing %s\n" , tokens[cur_pos]);
        addChild(T3, parseTerminal());
    } else if (isVariable(tokens[cur_pos])) {
        addChild(T3, parseVariable());
    } else if (isConstant()) {         // parse number condition
        addChild(T3, parseTerminal()); // Parse number here
    } else {
        error("Not an expression.");
        return NULL;
    }
    return T3;
}

Node *parseAssignment() {
    Node *A = createNode();
    strcpy(A->val, "A");
    addChild(A, parseVariable());
    // printf("here with %s\n" , A->child[0]->child[0]->val);
    addChild(A, parseTerminal());
    addChild(A, parseExpression(cur_pos, get_end(cur_pos)));
    return A;
}

Node *parseForLoop() {
    Node *F = createNode();
    strcpy(F->val, "F");

    addChild(F, parseTerminal());

    addChild(F, parseTerminal());

    addChild(F, parseAssignment());
    addChild(F, parseTerminal());
    addChild(F, parseExpression(cur_pos, get_end(cur_pos)));
    addChild(F, parseTerminal());
    // printf("parse for loop = %s\n" , tokens[cur_pos]);
    addChild(F, parseAssignment());
    addChild(F, parseTerminal());
    addChild(F, parseTerminal());
    addChild(F, parseStatement());
    // addChild(F, parseTerminal());
    addChild(F, parseTerminal());
    return F;
}

Node *parseRead() {
    Node *R = createNode();
    strcpy(R->val, "R");
    addChild(R,parseTerminal());
    addChild(R,parseVariable());
    return R;
}

Node *parseWrite() {
    Node *W = createNode();
    strcpy(W->val, "W");
    // if ((strcmp(tokens[cur_pos], "write") == 0)) {
    //     addChild(W, parseTerminal());
    //     if (isVariable(tokens[cur_pos])) {
    //         addChild(W, parseVariable());
    //     } else if (isConstant()) {
    //         addChild(W, parseTerminal());
    //     } else {
    //         error();
    //         return NULL;
    //     }
    // } else {
    //     error();
    //     return NULL;
    // }
    addChild(W,parseTerminal());

    if (isVariable(tokens[cur_pos])) {
            addChild(W, parseVariable());
        } else if (isConstant()) {
            addChild(W, parseTerminal());
        } else {
            error("Only a variable or a numeric constant can be written.");
            return NULL;
        }
    return W;
}

// Symbol table functions

int getVariablePosition(char *v) {
    int pos = -1;
    for (int i = 0; i < var_pos; i++) {
        if (!strcmp(variables[i], v)) {
            pos = i;
            break;
        }
    }
    return pos;
}

int getVariableValue(char *v) {
    int pos = getVariablePosition(v);
    if (pos == -1)
        return -1;
    return variable_values[pos];
}

void updateVariableValue(char *v, int new_val) {
    int pos = getVariablePosition(v);
    if (pos == -1) {
        printf("Invalid variable %s\n", v);
        return;
    }
    if(new_val < 0) {
        printf("Value of %s is negative\n" , v);
        exit(EXIT_FAILURE);
    }
    variable_values[pos] = new_val;
}

// Simulator functions

void simulateRead(Node *root) {
    char *v = root->child[1]->child[0]->val;
    int pos = getVariablePosition(v);

    if (pos == -1) {
        printf("Syntax Error : Variable '%s' not declared\n", v);
        exit(EXIT_FAILURE);
    }

    int cur;
    printf("Input for %s : ", v);
    scanf("%d", &cur);

    if (cur < 0) {
        printf("Error : Negative value entered\n");
        exit(EXIT_FAILURE);
    }

    updateVariableValue(v, cur);
}

void simulateWrite(Node *root) {
    char *v;
    if (strcmp(root->child[1]->val, "I") == 0)
        v = root->child[1]->child[0]->val;
    else
        v = root->child[1]->val;
    bool canWrite = true;
    for (int i = 0; i < 50; i++) {
        if (v[i] == 0)
            break;
        if (!isdigit(v[i])) {
            canWrite = false;
            break;
        }
    }
    if (canWrite) {
        printf("%s\n", v);
    } else {
        int pos = getVariablePosition(v);
        if (pos == -1) {
            printf("Syntax Error : Variable '%s' not declared\n", v);
            exit(EXIT_FAILURE);
        }
        printf("%d\n", getVariableValue(v));
    }
}

int simulateExpression(Node *root) {
    if ((root->child_cnt) == 0) {
        // printf("before constant : %d\n",atoi(root->val));
        return atoi(root->val);
    } else if ((root->child_cnt) == 1) {
        if (strcmp(root->val, "I") == 0)
        {
            // printf("before variable : %d\n",getVariableValue(root->child[0]->val));
            int ret = getVariableValue(root->child[0]->val);
            if(ret == -1) {
                printf("Syntax Error : Variable '%s' not declared\n", root->child[0]->val);
                exit(EXIT_FAILURE);
            }
            return ret;
        }            
        else
            return simulateExpression(root->child[0]);
    } else {
        if (strcmp(root->child[0]->val, "(") == 0)
            return simulateExpression(root->child[1]);
        else {
            int x1 = simulateExpression(root->child[0]);
            char *op = root->child[1]->val;
            int x2 = simulateExpression(root->child[2]);

            if (strcmp(op, ">") == 0) {
                // printf("Evaluating %d%s%d : %d\n", x1,op,x2,x1 > x2);
                return x1 > x2;
            } else if (strcmp(op, "==") == 0) {
                // printf("Evaluating %d%s%d : %d\n", x1,op,x2,x1 == x2);
                return x1 == x2;
            } else if (strcmp(op, "+") == 0) {
                // printf("Evaluating %d%s%d : %d\n", x1,op,x2,x1 + x2);
                return x1 + x2;
            } else if (strcmp(op, "-") == 0) {
                // printf("Evaluating %d%s%d : %d\n", x1,op,x2,x1 - x2);
                return x1 - x2;
            } else if (strcmp(op, "*") == 0) {
                // printf("Evaluating %d%s%d : %d\n", x1,op,x2,x1 * x2);
                return x1 * x2;
            } else {
                // printf("Evaluating %d%s%d : %d\n", x1,op,x2,x1 / x2);
                
                if (x2 == 0) {
                    fprintf(stderr, "Error : Divide by Zero Error\n");
                    exit(EXIT_FAILURE);
                }
                return x1 / x2;
            }
        }
    }
}

void simulateAssignment(Node *root) {
    char *v = root->child[0]->child[0]->val;
    int val = simulateExpression(root->child[2]);
    if(val < 0) {
        fprintf(stderr, "Value of %s is negative\n" , v);
        exit(EXIT_FAILURE);
    }
    updateVariableValue(v, val);
}

void simulateFor(Node *root) {
    // for(A;E;A){S;};
    simulateAssignment(root->child[2]);
    while (simulateExpression(root->child[4])) {
        simulateStatement(root->child[9]);
        simulateAssignment(root->child[6]);
    }
}

void simulateStatement(Node *root) {
    for (int i = 0; i < (root->child_cnt); i++) {
        Node *c = root->child[i];

        if (strcmp(c->val, "S") == 0)
            simulateStatement(c);
        else if (strcmp(c->val, "R") == 0)
            simulateRead(c);
        else if (strcmp(c->val, "W") == 0)
            simulateWrite(c);
        else if (strcmp(c->val, "A") == 0)
            simulateAssignment(c);
        else if (strcmp(c->val, "F") == 0) {
            simulateFor(c);
        }
    }
}

void simulateProgram(Node *root) {
    if(root->child_cnt == 1)
        simulateStatement(root->child[0]);
    else if ((root->child_cnt) == 2)
        simulateStatement(root->child[0]);
    else
        simulateStatement(root->child[2]);
}

// Main function

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    end_pos = tokenize(argv[1]);

    // for (int i = 0; i < end_pos; i++)
    //     printf("%d : %s\n", i , tokens[i]);

    cur_pos = 0;
    Node *root = parseProgram();
    printTree(root);
    printf("\n");
    simulateProgram(root);
    return EXIT_SUCCESS;
}
