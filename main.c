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
int cur_pos = 0, token_pos = 0;

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

int main(int argc, char** argv) {

    if (argc < 2) {
        perror("Pass the name of the input file as the first parameter. e.g.: ./simulator input.txt");
        exit(EXIT_FAILURE);
    }

    printf("%d" , tokenize(argv[1]));

    return EXIT_SUCCESS;
}