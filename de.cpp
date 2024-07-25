// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include "parser.tab.hpp"
// #include "source.hpp"
// #include "hw3_output.hpp"

// // Extern declarations to make them accessible in the main file
// extern int yyparse();
// extern FILE* yyin;
// extern int yylineno;

// // Include necessary header files for your symbol table and function declarations
// #include "CheckFunctions.h"
// #include "SymbolTable.hpp"

// using namespace std;

// // Global instances




// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include "parser.tab.hpp"
// #include "source.hpp"
// #include "hw3_output.hpp"

// extern int yyparse();
// extern FILE* yyin;

// int main(int argc, char* argv[]) {
//     //for tests: std::cout << "Debug: Starting program execution" << std::endl;

//     if (argc == 2) {
//         // Input from file specified as command-line argument
//         yyin = fopen(argv[1], "r");
//         if (!yyin) {
//             std::cerr << "Error opening file: " << argv[1] << std::endl;
//             return 1;
//         }
//     } else if (argc == 1) {
//         // No arguments, use stdin
//         yyin = stdin;
//     } else {
//         std::cerr << "Usage: " << argv[0] << " [<input file>]" << std::endl;
//         return 1;
//     }

//     // Run the parser
//     yyparse();

//     // Close the input file if we opened one
//     if (argc == 2) {
//         fclose(yyin);
//     }

//     //for tests: std::cout << "Debug: Ending program execution" << std::endl;
//     return 0;
// }