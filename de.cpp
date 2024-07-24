#include <iostream>
#include <fstream>
#include <sstream>
#include "parser.tab.hpp"
#include "source.hpp"
#include "hw3_output.hpp"

// Extern declarations to make them accessible in the main file
extern int yyparse();
extern FILE* yyin;
extern int yylineno;

// Include necessary header files for your symbol table and function declarations
#include "CheckFunctions.h"
#include "SymbolTable.hpp"

using namespace std;

// Global instances




int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input file>" << endl;
        return 1;
    }

    // Open the input file
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    // Run the parser
    yyparse();

    // Close the input file
    fclose(yyin);
 cout << "Finished parsing" << endl; // Debug print statement
    return 0;
}
