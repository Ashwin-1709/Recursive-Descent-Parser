#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS (1000000)
#define MAX_LINE_LENGTH (1000000)

char tokens[100000][50];
int cur_pos = 0, token_pos = 0, end_pos = 0;

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

Node *parseExpression();

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

void error() {
    perror("Error occurred\n");
    exit(EXIT_FAILURE);
}

Node *createNode() { return (Node *)(malloc(sizeof(Node))); }

void printTree(Node *root) {
    printf("%s ", root->val);
    int c = root->child_cnt;

    if (c == 0)
        return;

    printf("[");
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

bool isVariable() {
    char *v = tokens[cur_pos];

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
    Node *t = (Node *)(malloc(sizeof(Node)));
    strcpy(t->val, tokens[cur_pos++]);
    return t;
}

Node *parseVariable(int spos) {
    char *s = tokens[cur_pos];
    int l = strlen(s);

    if (spos == l) {
        return NULL;
    }

    Node *I = (Node *)(malloc(sizeof(Node)));
    strcpy(I->val, "I");

    Node *C = (Node *)(malloc(sizeof(Node)));
    strcpy(C->val, "C");

    Node *t = createNode();
    t->val[0] = s[spos];

    addChild(C, t);
    addChild(I, C);
    addChild(I, parseVariable(spos + 1));
    return I;
}

Node *parseVariableList() {
    Node *L = (Node *)(malloc(sizeof(Node)));
    strcpy(L->val, "L");

    if (isVariable()) {
        addChild(L, parseVariable(0));
        cur_pos++;
    } else {
        error();
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
        error();
        return NULL;
    }
}

Node *parseDeclaration() {
    Node *D = (Node *)(malloc(sizeof(Node)));
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

    Node *P = (Node *)(malloc(sizeof(Node)));
    strcpy(P->val, "P");

    if (isDeclaration()) {
        addChild(P, parseDeclaration());
    }

    if (strcmp(tokens[cur_pos], ";") == 0)
        addChild(P, parseTerminal());
    else {
        error();
        return NULL;
    }

    addChild(P, parseProgram());
    return P;
}

Node *parseT3() {
    Node *T3 = (Node *)(malloc(sizeof(Node)));
    strcpy(T3->val, "T3");
    if (strcmp(tokens[cur_pos], "(") == 0) {
        addChild(T3, parseTerminal());
        addChild(T3, parseExpression());
        addChild(T3, parseTerminal());
    } else if (isVariable()) {
        addChild(T3, parseVariable(0));
    } else if (isConstant()) {         // parse number condition
        addChild(T3, parseTerminal()); // Parse number here
    } else {
        error();
        return NULL;
    }
    return T3;
}

Node *parseT2() {
    Node *T2 = (Node *)(malloc(sizeof(Node)));
    strcpy(T2->val, "T2");
    if ((strcmp(tokens[cur_pos + 1], "*") == 0) ||
        (strcmp(tokens[cur_pos + 1], "/") == 0)) {
        addChild(T2, parseT3());
        addChild(T2, parseTerminal());
        addChild(T2, parseT2());
    } else {
        addChild(T2, parseT3());
    }
    return T2;
}

Node *parseT1() {
    // Parse T3 will handle offset to next expression or terminal
    Node *T1 = (Node *)(malloc(sizeof(Node)));
    strcpy(T1->val, "T1");
    if ((strcmp(tokens[cur_pos + 1], "+") == 0) ||
        (strcmp(tokens[cur_pos + 1], "-") == 0)) {
        addChild(T1, parseT2());
        addChild(T1, parseTerminal());
        addChild(T1, parseT1());
    } else {
        addChild(T1, parseT2());
    }
    return T1;
}

Node *parseExpression() {
    Node *E = (Node *)(malloc(sizeof(Node)));
    strcpy(E->val, "E");
    if ((strcmp(tokens[cur_pos + 1], ">") == 0) ||
        (strcmp(tokens[cur_pos + 1], "==") == 0)) {
        addChild(E, parseT1());
        addChild(E, parseTerminal());
        addChild(E, parseExpression());
    } else {
        addChild(E, parseT1());
    }
    return E;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        perror("Pass the name of the input file as the first parameter. e.g.: "
               "./simulator input.txt");
        exit(EXIT_FAILURE);
    }

    end_pos = tokenize(argv[1]);

    for (int i = 0; i < end_pos; i++)
        printf("%s\n", tokens[i]);

    cur_pos = 0;

    Node *root = parseProgram();
    printTree(root);
    printf("\n");

    return EXIT_SUCCESS;
}
