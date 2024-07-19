#ifndef CheckFunctions_H
#define CheckFunctions_H
#include "source.hpp"
#include "SymbolTable.hpp"


// scope and symbol table management functions 
void createNewScope(TablesStack& tableStack);
void exitScope(TablesStack& tableStack);
void addSymbolToCurrentScope(TablesStack& tableStack, const string& name, const string& type, int offset);
void addFunctionToGlobalScope(TablesStack& tableStack, const string& name, const string& returnType, const vector<string>& paramTypes);

// type checking functions 
string getExpressionType(const ExpNode* exp);
bool isTypeCompatible(const string& type1, const string& type2);
bool isNumericType(const string& type);
bool isBooleanType(const string& type);
void checkTypeMismatch(const string& expected, const string& actual, const string& name, int lineno);
// semantic eroro functions 
void checkVariableDeclaration(TablesStack& tableStack, const string& name, int lineno);
void checkFunctionDeclaration(TablesStack& tableStack, const string& name, int lineno);
void checkAssignment(const string& lhsType, const string& rhsType, int lineno);
void checkFunctionCall(TablesStack& tableStack, const string& funcName, const vector<string>& argTypes, int lineno);
void checkReturnStatement(TablesStack& tableStack, const string& returnType, int lineno);

// control flow checking functions
void checkBreakStatement(int loopDepth, int lineno);
void checkContinueStatement(int loopDepth, int lineno);

// aux functions 
string boolToString(bool value);
string intToString(int value);
bool isLegalByteValue(int value);
bool isVariableDefined(TablesStack& tables, const string& name);
string getVariableType(TablesStack& tables, const string& name);
bool areTypesCompatible(const string& type1, const string& type2);
void addVariableToScope(TablesStack& tables, const string& name, const string& type, int offset);
string getSymbolType(TablesStack& tableStack, const string& name);
bool isSymbolDefinedInCurrentScope(TablesStack& tableStack, const string& name);

// main function checker
void checkMainFunction(TablesStack& tableStack);

#endif 