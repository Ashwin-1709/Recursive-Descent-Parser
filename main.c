#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKENS (1000000)
#define MAX_LINE_LENGTH (1000000)
#define EXIT_SUCCESS 0

char tokens[100000][50];
int cur_pos = 0, token_pos = 0, end_pos=0;

struct Node{

    char val[50];
    int child_cnt;
    struct Node* child[15];

};

/**
Reads the passed input file line by line.
Removes all space type characters from the lines, including \n (Carriage Return). 
Modifies the global tokens and num_tokens variables. 
*/

// We need to split on delimiters , while pushing certain delimiters as tokens
// , ( ) { } ; -> to be pushed
// \n whitespace \t \0 -> not be pushed

int charCheck(char c) {
    if(c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}')
        return 1;
    else if (c == '\n' || c == '\t' || c == '\0' || c == ' ' || c == 0 || c == 10 || c == 13) 
        return 2;
    return 0;
}

char operators[6] = {'+' , '-' , '*' , '/' , '>' , '='};

int tokenize(char *filename) {
    FILE* in_file = fopen(filename , "r");
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, in_file)) {
        for (int i = 0 ; i < strlen(line) ; i++) {
            bool isoperator = false;
            for (int j = 0 ; j < 6 ; j++) {
                if (operators[j] == line[i]) {
                    if (operators[j] == '=' && cur_pos > 0 && tokens[cur_pos-1][0] == '=' && line[i-1] == '=') {
                        tokens[cur_pos - 1][1] = '=';
                        isoperator = true;
                        break;
                    }
                    if (token_pos > 0) {
                        tokens[++cur_pos][0] = operators[j];
                        token_pos = 0;
                        cur_pos++;
                    } 
                    else {
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

    printf("Tokens: \n");
    for (int i = 0 ; i < cur_pos ; i++)
        printf("%s " , tokens[i]);   

    return cur_pos;  
}

void addChild(struct Node* par, struct Node* ch)
{
    if (ch==NULL)
        return;

    par->child[par->child_cnt++] = ch; 
}

void error()
{
    perror("Error occurred\n");
    exit(EXIT_FAILURE);
}

struct Node* createNode()
{
    return (struct Node*)(malloc(sizeof(struct Node)));
}

void printTree(struct Node* root)
{
    printf("%s ", root->val);
    int c = root->child_cnt;

    if (c == 0)
        return;

    printf("[ ");
    for (int i=0;i<c;i++)
        printTree(root->child[i]);
    printf("] ");
}

// all identifier functions are defined here

bool isDeclaration()
{
    if (strcmp("int", tokens[cur_pos])==0)
        return true;
    
    return false;
}

bool isVariable()
{
    char* v = tokens[cur_pos];

    if (strcmp(v,"for")==0)
        return false;

    if (strcmp(v,"int")==0)
        return false;

    if (strcmp(v,"read")==0)
        return false;

    if (strcmp(v,"write")==0)
        return false;

    return isalpha(v);
}

// all parser functions are defined here

struct Node* parseTerminal()
{
    struct Node* t = (struct Node*)(malloc(sizeof(struct Node)));
    strcpy(t->val,tokens[cur_pos++]);
    return t;
}

struct Node* parseVariable(int spos)
{
    char* s = tokens[cur_pos];
    int l = strlen(s);

    if (spos==l)
        return NULL;

    struct Node* I = (struct Node*)(malloc(sizeof(struct Node)));
    struct Node* C = (struct Node*)(malloc(sizeof(struct Node)));
    strcpy(C->val, "C");

    struct Node* t = createNode();
    t->val[0] = s[spos];

    addChild(C, t);
    addChild(I, C);
    addChild(I, parseVariable(spos+1));
    return I;
}

struct Node* parseVariableList()
{
    struct Node* L = (struct Node*)(malloc(sizeof(struct Node)));

    if (isVariable())
        addChild(L, parseVariable(0));
    else {
        error();
        return NULL;
    }

    if (strcmp(tokens[cur_pos],",")==0)
    {
        addChild(L, parseTerminal());
        struct Node* x = parseVariableList();
        addChild(L, x);
        return L;
    }
    else if (strcmp(tokens[cur_pos],";")==0)
        return NULL;
    else
    {
        error();
        return NULL;
    }
}

struct Node* parseDeclaration()
{
    struct Node* D= (struct Node*)(malloc(sizeof(struct Node)));
    addChild(D, parseTerminal());
    addChild(D, parseVariableList());
    return D;
}

struct Node* parseProgram()
{
    // end of program

    if (cur_pos == end_pos)
        return NULL;

    // ignore empty statements

    if (strcmp(tokens[cur_pos], ";")==0)
    {
        cur_pos++;
        parseProgram();
    }

    struct Node* P = (struct Node*)(malloc(sizeof(struct Node)));
    strcpy(P->val, "P");

    if (isDeclaration())
        addChild(P, parseDeclaration());

    if (strcmp(tokens[cur_pos],";")==0)
        addChild(P, parseTerminal());
    else
    {
        error();
        return NULL;
    }

    addChild(P, parseProgram());
    return P;
}


int main(int argc, char** argv) {

    if (argc < 2) {
        perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
        exit(EXIT_FAILURE);
    }

    printf("%d" , tokenize(argv[1]));
    end_pos = tokenize(argv[1]);
    cur_pos = 0;

    struct Node* root = parseProgram();
    printTree(root);
    return EXIT_SUCCESS;
}
