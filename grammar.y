// This is just a specification, the parser was handwritten.
%{
}%
%token <operator> PLUS "+"
%token <operator> MINUS "-"
%token <operator> MULTIPLY "*"
%token <operator> DIVIDE "/"
%token <operator> ASSIGNMENT "="
%token <operator> GREATER_THAN ">"
%token <operator> EQUALS "=="
%token <operator> OPENING_BRACKET "("
%token <operator> CLOSING_BRACKET ")"
%token <delimiter> SEMICOLON ";"
%token <delimiter> COMMA ","
%token <delimiter> OPENING_BRACE "{"
%token <delimiter> CLOSING_BRACE "}"
%token <keyword> FOR "for"
%token <keyword> WRITE "write"
%token <keyword> READ "read"
%token <keyword> INT "int"
%token <variable> [a-z]+
%token <constant> [0-9]+

%%
program:
  declaration SEMICOLON
| declaration SEMICOLON statement
| statement
;

statement:
  assignment SEMICOLON
| read SEMICOLON
| write SEMICOLON
| loop SEMICOLON
| assignment SEMICOLON statement
| read SEMICOLON statement
| write SEMICOLON statement
| loop SEMICOLON statement
;

loop:
  FOR OPENING_BRACKET assignment SEMICOLON expression SEMICOLON assignment CLOSING_BRACKET OPENING_BRACE statement CLOSING_BRACE
;

assignment:
  variable EQUALS expression
;

expression:
  expression GREATER_THAN T1
| expression EQUALS T1
| T1
;

T1:
  T1 PLUS T2
| T1 MINUS T2
| T2
;

T2:
  T2 MULTIPLY T3
| T2 DIVIDE T3
| T3
;

T3:
  OPENING_BRACKET expression CLOSING_BRACKET
| constant
| variable
;

write:
  WRITE constant
| WRITE variable
;

read:
  READ variable
;

declaration:
  INT variable_list
;

variable_list:
  variable
| variable COMMA variable_list
;

variable:
  <variable>
;

constant:
  <constant>
;
