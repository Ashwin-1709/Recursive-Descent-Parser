### Theory of Computation - Assignment 2

### Problem Statement : 
( Refer to ```problem_statement.pdf``` ) </br>
For a given hypothetical language 'Basic C' with given specifications : 
1. Write an unambiguous grammar G for basic C with the above features.
2. Build a parser for the language and generates the syntax tree for a given program P in basic C.
3. Simulate program P on the input given by the user (if any) at the run-time.

Parsing is the process to determine whether the start symbol can derive the program or not. If the Parsing is successful then the program is a valid program otherwise the program is invalid. We have used Recursive Descent Parser for parsing which is a Top-down parsing technique. A top-down parser builds the parse tree from the top to down, starting with the start non-terminal. We have used a predictive parser which eliminates the need of backtracking. Simulation of the program P is done by traversing the generated syntax tree.

### How to run : 
1. Compile the main file ```gcc main.c -o main.out```
2. Run the executable with input file as argument ```./main.out input.txt```
3. To visualise the syntax tree, use the python script with syntax tree output as the argument ```python tree.py "<syntax tree output>"``` </br>
Note : ```tree.py``` requires nltk library.

**Contributors**
- [Arkishman Ghosh](https://github.com/ArkiGhosh)
- [Ashwin Arun](https://github.com/CodeFreak2002)
- [Ashwin Pugalia](https://github.com/Ashwin-1709)
- [Chinmay Dalal](https://github.com/p00f)
- [Sriram Balasubramanian](https://github.com/SriramB2002)

