#include "CheckFunctions.h"
#include "source.hpp"
#include "hw3_output.hpp"
#include <string>
#include <sstream>


using namespace output;



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// type checking functions  ////////////////////////////////////////////

string getExpressionType(const ExpNode* exp) {
    if (dynamic_cast<const Num*>(exp)) {
        return "INT";
    } else if (dynamic_cast<const BoolLex*>(exp)) {
        return "BOOL";
    } else if (dynamic_cast<const StringLex*>(exp)) {
        return "STRING";
    } else if (const IdentifierStr* id = dynamic_cast<const IdentifierStr*>(exp)) {
        // !!
        // Here you would look up the type in your symbol table
        // For now, we'll return a placeholder
        return "PLACEHOLDER";
    }
    // Add more cases as needed
    return "UNKNOWN";
}

bool isTypeCompatible(const string& type1, const string& type2) {
    if (type1 == type2) {
        return true;
    }
    if (type1 == "INT" && type2 == "BYTE") {
        return true;
    }
    // !! Add more compatibility rules as needed
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

void checkBooleanExpression(const ExpNode* exp, int lineno) {
    string type = getExpressionType(exp);
    if (!isBooleanType(type)) {
        output::errorMismatch(lineno);  
        exit(0);
    }
}


void checkNumericExpression(const ExpNode* exp, int lineno) {
    string type = getExpressionType(exp);
    if (!isNumericType(type)) {
        output::errorMismatch(lineno);
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