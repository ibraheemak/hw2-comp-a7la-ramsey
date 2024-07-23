#include "CheckFunctions.h"
#include "source.hpp"
#include "SymbolTable.hpp"
#include "hw3_output.hpp"
#include <string>
#include <sstream>


using namespace output;

extern TablesStack tableStack;
extern int yylineno;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// type checking functions  ////////////////////////////////////////////

string getExpressionType(const TNode* exp, TablesStack& tables) {
    if (const Num* num = dynamic_cast<const Num*>(exp)) {
        return "INT"; // Assuming Num is always INT in your implementation
    } else if (const BoolLex* boolExp = dynamic_cast<const BoolLex*>(exp)) {
        return "BOOL";
    } else if (const StringLex* strExp = dynamic_cast<const StringLex*>(exp)) {
        return "STRING";
    } else if (const IdentifierStr* id = dynamic_cast<const IdentifierStr*>(exp)) {
        return getSymbolType(tables, id->id);
    } else if (const Binop* binop = dynamic_cast<const Binop*>(exp)) {
        // For binary operations, you might need to check the operands
        // This is a simplification and might need to be expanded
        return "INT"; // Assuming binary operations result in INT
    } else if (const Relop* relop = dynamic_cast<const Relop*>(exp)) {
        return "BOOL"; // Relational operations always return BOOL
    } else if (const ExpNode* expNode = dynamic_cast<const ExpNode*>(exp)) {
        // This might be a logical operation (AND, OR, NOT)
        return "BOOL";
    }
    
    // If we reach here, it's an unknown expression type
    output::errorMismatch(yylineno); // Using yylineno as it's available globally
    exit(0);
}

bool isTypeCompatible(const string& type1, const string& type2) {
    if (type1 == type2) {
        return true;
    }
    if (type1 == "INT" && type2 == "BYTE") {
        return true;
    }
    // BYTE can be assigned to INT, but not vice versa
    if (type1 == "BYTE" && type2 == "INT") {
        return false;
    }
    // Handle explicit casting
    if ((type1 == "INT" || type1 == "BYTE") && (type2 == "INT" || type2 == "BYTE")) {
        return true;
    }
    // All other cases are incompatible
    return false;
}


bool isNumericType(const string& type) {
    return (type == "INT" || type == "BYTE");
}

bool isBooleanType(const string& type) {
    return (type == "BOOL");
}

void checkTypeMismatch(const string& expected, const string& actual, const string& name, int lineno) {
    if (!isTypeCompatible(expected, actual)) {
        output::errorMismatch(lineno);
        exit(0);
    }
}

void checkBooleanExpression(const TNode* exp, TablesStack& tables) {
    string type = getExpressionType(exp, tables);
    if (type != "BOOL") {
        output::errorMismatch(yylineno);  
        exit(0);
    }
}
void checkNumericExpression(const TNode* exp, TablesStack& tables) {
    string type = getExpressionType(exp, tables);
    if (type != "INT" && type != "BYTE") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///////////////////// main function checker ////////////////////////////////////////
void checkMainFunction(TablesStack& tables) {
    ScopeBlock* globalScope = tables.ParentScope;
    bool mainFound = false;

    for (const auto& entry : globalScope->scope) {
        if (entry->name == "main" && entry->type == "function") {
            functions* mainFunc = static_cast<functions*>(entry);
            if (mainFunc->ret_type == "VOID" && mainFunc->numofarg == 0) {
                mainFound = true;
                break;
            }
        }
    }

    if (!mainFound) {
        output::errorMismatch(0);  // Using a general error, with line number 0 for global scope
        exit(0);
    }
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// aux funcions //////////////////////////////////////
string boolToString(bool value) {
    return value ? "true" : "false";
}

string intToString(int value) {
    return std::to_string(value);
}


bool isLegalByteValue(int value) {
    return value >= 0 && value <= 255;
}

bool isVariableDefined(TablesStack& tables, const string& name) {
    for (auto it = tables.stackTable.rbegin(); it != tables.stackTable.rend(); ++it) {
        for (const auto& entry : (*it)->scope) {
            if (entry->name == name) {
                return true;
            }
        }
    }
    return false;
}

string getVariableType(TablesStack& tables, const string& name) {
    for (auto it = tables.stackTable.rbegin(); it != tables.stackTable.rend(); ++it) {
        for (const auto& entry : (*it)->scope) {
            if (entry->name == name) {
                return entry->type;
            }
        }
    }
    return ""; // Variable not found
}

bool areTypesCompatible(const string& type1, const string& type2) {
    if (type1 == type2) return true;
    if (type1 == "INT" && type2 == "BYTE") return true;
    return false;
}

void addVariableToScope(TablesStack& tables, const string& name, const string& type, int offset) {
    ScopeBlock* currentScope = tables.stackTable.back();
    tableEntry* newEntry = new tableEntry(name, type, offset);
    currentScope->scope.push_back(newEntry);
}

bool isSymbolDefinedInCurrentScope(TablesStack& tableStack, const string& name) {
    if (!tableStack.stackTable.empty()) {
        ScopeBlock* currentScope = tableStack.stackTable.back();
        for (const auto& entry : currentScope->scope) {
            if (entry->name == name) {
                return true;
            }
        }
    }
    return false;
}

string getSymbolType(TablesStack& tableStack, const string& name) {
    for (auto it = tableStack.stackTable.rbegin(); it != tableStack.stackTable.rend(); ++it) {
        for (const auto& entry : (*it)->scope) {
            if (entry->name == name) {
                return entry->type;
            }
        }
    }
    return "";  // Symbol not found
}
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// control flow checking functions //////////////////////////////////////
void checkBreakStatement(int loopDepth, int lineno) {
    if (loopDepth == 0) {
        output::errorUnexpectedBreak(lineno);
        exit(0);
    }
}

void checkContinueStatement(int loopDepth, int lineno) {
    if (loopDepth == 0) {
        output::errorUnexpectedContinue(lineno);
        exit(0);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// scope and symbol table management functions /////////////////////////////
void createNewScope(TablesStack& tableStack) {
    ScopeBlock* newScope = tableStack.InsertTable(tableStack.stackTable.empty() ? nullptr : tableStack.stackTable.back());
    // No need to push to stackTable as InsertTable already does this
}
void addSymbolToCurrentScope(TablesStack& tableStack, const string& name, const string& type, int offset) {
    if (!tableStack.stackTable.empty()) {
        ScopeBlock* currentScope = tableStack.stackTable.back();
        tableStack.insert(currentScope, name, type, offset);
    }
}

void exitScope(TablesStack& tableStack) {
    if (!tableStack.stackTable.empty()) {
        ScopeBlock* scopeToDelete = tableStack.stackTable.back();
        tableStack.stackTable.pop_back();

        // Print the symbols in the scope that's being exited
        output::endScope();
        for (const auto& entry : scopeToDelete->scope) {
            if (entry->type == "function") {
                functions* func = static_cast<functions*>(entry);
                std::ostringstream argTypes;
                for (size_t i = 0; i < func->all_arg.size(); ++i) {
                    if (i > 0) argTypes << ",";
                    argTypes << func->all_arg[i];
                }
                string functionType = output::makeFunctionType(argTypes.str(), func->ret_type);
                output::printID(func->name, func->offset, functionType);
            } else {
                output::printID(entry->name, entry->offset, entry->type);
            }
        }

        delete scopeToDelete;
    }
}

void addFunctionToGlobalScope(TablesStack& tableStack, const string& name, const string& returnType, const vector<string>& paramTypes) {
    if (!tableStack.stackTable.empty()) {
        ScopeBlock* globalScope = tableStack.ParentScope;
        int numArgs = paramTypes.size();
        tableStack.insertFunction(globalScope, name, "function", 0, numArgs, paramTypes, returnType);
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// // semantic error functions  /////////////////////////////
void checkVariableDeclaration(TablesStack& tableStack, const string& name, int lineno) {
    if (isSymbolDefinedInCurrentScope(tableStack, name)) {
        output::errorDef(lineno, name);
        exit(0);
    }
    // If we reach here, the variable declaration is valid
}
void checkFunctionDeclaration(TablesStack& tableStack, const string& name, int lineno) {
    if (isSymbolDefinedInCurrentScope(tableStack, name)) {
        output::errorDef(lineno, name);
        exit(0);
    }
    // If we reach here, the function declaration is valid
}
void checkAssignment(const string& lhsType, const string& rhsType, int lineno) {
    if (!isTypeCompatible(lhsType, rhsType)) {
        output::errorMismatch(lineno);
        exit(0);
    }
    // If we reach here, the assignment is valid
}
void checkFunctionCall(TablesStack& tableStack, const string& funcName, const vector<string>& argTypes, int lineno) {
    string funcType = getSymbolType(tableStack, funcName);
    if (funcType.empty()) {
        output::errorUndefFunc(lineno, funcName);
        exit(0);
    }
    
    // Parse the function type to get expected argument types
    size_t argsStart = funcType.find('(');
    size_t argsEnd = funcType.find(')');
    string argsStr = funcType.substr(argsStart + 1, argsEnd - argsStart - 1);
    vector<string> expectedArgTypes;
    
    size_t pos = 0;
    while ((pos = argsStr.find(',')) != string::npos) {
        expectedArgTypes.push_back(argsStr.substr(0, pos));
        argsStr.erase(0, pos + 1);
    }
    if (!argsStr.empty()) {
        expectedArgTypes.push_back(argsStr);
    }

    if (argTypes.size() != expectedArgTypes.size()) {
        output::errorPrototypeMismatch(lineno, funcName, funcType);
        exit(0);
    }
    
    for (size_t i = 0; i < argTypes.size(); ++i) {
        if (!isTypeCompatible(expectedArgTypes[i], argTypes[i])) {
            output::errorPrototypeMismatch(lineno, funcName, funcType);
            exit(0);
        }
    }
    // If we reach here, the function call is valid
}
void checkReturnStatement(TablesStack& tableStack, const string& returnType, int lineno) {
    // Get the expected return type from the current function
    string expectedReturnType = getCurrentFunctionReturnType(tableStack);
    
    if (expectedReturnType.empty()) {
        // We're not inside a function
        output::errorMismatch(lineno);
        exit(0);
    }
    
    if (!isTypeCompatible(expectedReturnType, returnType)) {
        output::errorMismatch(lineno);
        exit(0);
    }
    // If we reach here, the return statement is valid
}
string getCurrentFunctionReturnType(TablesStack& tableStack) {
    // Traverse the scope stack from top to bottom
    for (auto it = tableStack.stackTable.rbegin(); it != tableStack.stackTable.rend(); ++it) {
        for (const auto& entry : (*it)->scope) {
            if (entry->type == "function") {
                // Found the current function
                functions* func = static_cast<functions*>(entry);
                return func->ret_type;
            }
        }
    }
    // Not inside a function
    return "";
}