#include "CheckFunctions.h"
#include "source.hpp"
#include "SymbolTable.hpp"
#include "hw3_output.hpp"
#include <string>
#include <sstream>
#include <iostream>

using namespace output;

extern TablesStack tableStack;
extern int yylineno;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// type checking functions  ////////////////////////////////////////////

string getExpressionType(const TNode* exp, TablesStack& tables) {
    if (const Num* num = dynamic_cast<const Num*>(exp)) {
        return "int"; // Assuming Num is always int in your implementation
    } else if (const BoolLex* boolExp = dynamic_cast<const BoolLex*>(exp)) {
        return "bool";
    } else if (const StringLex* strExp = dynamic_cast<const StringLex*>(exp)) {
        return "string";
    } else if (const IdentifierStr* id = dynamic_cast<const IdentifierStr*>(exp)) {
        return getSymbolType(tables, id->id);
    } else if (const Binop* binop = dynamic_cast<const Binop*>(exp)) {
        // For binary operations, you might need to check the operands
        // This is a simplification and might need to be expanded
        return "int"; // Assuming binary operations result in int
    } else if (const Relop* relop = dynamic_cast<const Relop*>(exp)) {
        return "bool"; // Relational operations always return bool
    } else if (const ExpNode* expNode = dynamic_cast<const ExpNode*>(exp)) {
        // This might be a logical operation (AND, OR, NOT)
        return expNode->type;
    }
    
    // If we reach here, it's an unknown expression type
    output::errorMismatch(yylineno); // Using yylineno as it's available globally
    exit(0);
}

bool isTypeCompatible(const string& type1, const string& type2,bool explicitCast) {
    if (type1 == type2) {
        return true;
    }
    if(explicitCast){
            if (type1 == "int" && type2 == "byte") 
        return true;

            if (type1 == "byte" && type2 == "int")
        return true;
    
    }
    
    if (type1 == "int" && type2 == "byte") {
        return true;
    }
    // byte can be assigned to int, but not vice versa
    if (type1 == "byte" && type2 == "int") {
        return false;
    }
    // Handle explicit casting
    if ((type1 == "int" || type1 == "byte") && (type2 == "int" || type2 == "byte")) {
        return true;
    }
    // All other cases are incompatible
    return false;
}


bool isNumericType(const string& type) {
    return (type == "int" || type == "byte");
}

bool isBooleanType(const string& type) {
    return (type == "bool");
}

void checkTypeMismatch(const string& expected, const string& actual, const string& name, int lineno) {
    if (!isTypeCompatible(expected, actual)) {
        //for tests: cout << expected << ", " << actual << ", " << name << endl;
        output::errorMismatch(lineno);
        exit(0);
    }
}

/*void checkNumericExpression(const TNode* exp, TablesStack& tables) {
    string type = getExpressionType(exp, tables);
    if (!isNumericType(type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}*/
void checkNumericExpression(const TNode* exp, TablesStack& tables) {
    string type = getExpressionType(exp, tables);
    //for tests: std::cout << "Expression type: " << type << endl;
    if (!isNumericType(type)) {
    //for tests: std::cout << "Not a numeric type!" << endl;
        output::errorMismatch(yylineno);
        exit(0);
    }
}

void checkBooleanExpression(const TNode* exp, TablesStack& tables) {
    string type = getExpressionType(exp, tables);
    if (!isBooleanType(type)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

bool isExplicitCastingValid(const string& targetType, const string& sourceType) {
    if (targetType == sourceType) return true;
    if ((targetType == "int" && sourceType == "byte") ||
        (targetType == "byte" && sourceType == "int")) {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///////////////////// main function checker ////////////////////////////////////////
void checkMainFunction(TablesStack& tables) {
    ScopeBlock* globalScope = tables.ParentScope;
    bool mainFound = false;

    for (const auto& entry : globalScope->scope) {
        //for tests: cout<< entry->name << ", " << entry->type << endl;
        if (entry->name == "main" && entry->type == "function") {
            functions* mainFunc = static_cast<functions*>(entry);
            //for tests: cout << entry->name << ", " << entry->type << mainFunc->ret_type << endl;
            if (mainFunc->ret_type == "void") {
                mainFound = true;
                //for tests: cout << "'main' function found in global scope" << endl;
                break;
            }
        }
    }

    if (!mainFound) {
        //for tests: cout << "Error: No 'main' function found in global scope" << endl;

        // output::errorMismatch(0);  // Using a general error, with line number 0 for global scope
        // exit(0);
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
    if (type1 == "int" && type2 == "byte") return true;
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
        while(currentScope!=nullptr){
        for (const auto& entry : currentScope->scope) {
            if (entry->name == name) {
                return true;
            }
        }
        currentScope=currentScope->parent;
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
                std::string argTypes;
                for (size_t i = 0; i < func->all_arg.size(); ++i) {
                    if (i > 0) argTypes += ",";
                    argTypes += toUpper(func->all_arg[i]);
                }
                string functionType = output::makeFunctionType(argTypes, toUpper(func->ret_type));
                output::printID(func->name, func->offset, functionType);
            } else {
                output::printID(entry->name, entry->offset, toUpper(entry->type));
            }
        }

        delete scopeToDelete;
    }
}
void addFunctionToGlobalScope(TablesStack& tableStack, const string& name, const string& returnType, const vector<string>& paramTypes) {
    if (!tableStack.stackTable.empty()) {
        ScopeBlock* globalScope = tableStack.ParentScope;
        tableStack.insertFunction(globalScope, name, "function", 0, paramTypes, returnType);
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
    //for tests: cout << "Debug: Checking assignment at line " << lineno << " with lhsType: " << lhsType << " and rhsType: " << rhsType << endl;
    if (!isTypeCompatible(lhsType, rhsType)) {
        //for tests: cout << "checkAssignment:" << lhsType << ", " << rhsType << endl;
        output::errorMismatch(lineno);
        exit(0);
    }
    // If we reach here, the assignment is valid
}

string formatArgTypes(const vector<string>& argTypes) {
    string result;
    for (size_t i = 0; i < argTypes.size(); ++i) {
        if (i > 0) result += ",";
        result += toUpper(argTypes[i]);
    }
    return result;
}
ExpNode* checkFunctionCall(TablesStack& tableStack, const string& funcName, const vector<string>& argTypes, int lineno) {
    string funcType = getSymbolType(tableStack, funcName);
    if (funcType.empty()) {
        output::errorUndefFunc(lineno, funcName);
        exit(0);
    }

    // Find the function entry
    tableEntry* entry = nullptr;
    for (const auto& e : tableStack.ParentScope->scope) {
        if (e->name == funcName) {
            entry = e;
            break;
        }
    }

    if (!entry || entry->type != "function") {
        output::errorUndefFunc(lineno, funcName);
        exit(0);
    }

    functions* func = static_cast<functions*>(entry);

    // Check if the argument types match
    bool matchFound = false;
    for (const auto& paramType : func->all_arg) {
        if (isTypeCompatible(paramType, argTypes[0])) {
            matchFound = true;
            break;
        }
    }

    if (!matchFound) {
        string expectedArgType = toUpper(func->all_arg[0]);  // Use the expected type from the function definition
        output::errorPrototypeMismatch(lineno, funcName, expectedArgType);
        exit(0);
    }

    // If we get here, the call is valid
    return new ExpNode(func->ret_type);
}

void checkReturnStatement(TablesStack& tableStack, const string& returnType, int lineno) {
    string expectedReturnType = getCurrentFunctionReturnType(tableStack);
    
    if (expectedReturnType.empty()) {
        // We're not inside a function
        output::errorMismatch(lineno);
        exit(0);
    }
    
    if (expectedReturnType == "void" && returnType != "void") {
        output::errorMismatch(lineno);
        exit(0);
    }
    
    if (expectedReturnType != "void" && returnType == "void") {
        output::errorMismatch(lineno);
        exit(0);
    }
    
    if (!isTypeCompatible(expectedReturnType, returnType)) {
        output::errorMismatch(lineno);
        exit(0);
    }
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