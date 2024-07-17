#include "HelperFunctions.h"
#include "output.hpp"
#include <string>

extern SymbolTable symbolTable;

void check_valid_assignment(const char* type1, const char* type2, int lineno) {
    if (!symbolTable.isValidAssignment(type1, type2)) {
        output::errorMismatch(lineno);
        exit(0);
    }
}

void check_defined_variable(const char* id, int lineno) {
    if (!symbolTable.isSymbolDefined(id)) {
        output::errorUndef(lineno, id);
        exit(0);
    }
}

void check_defined_function(const char* id, int lineno) {
    if (!symbolTable.isFunction(id)) {
        output::errorUndefFunc(lineno, id);
        exit(0);
    }
}

void check_matching_types(const char* type1, const char* type2, int lineno) {
    if (strcmp(type1, type2) != 0) {
        output::errorMismatch(lineno);
        exit(0);
    }
}

void check_numeric_type(const char* type, int lineno) {
    if (!symbolTable.isNumericType(type)) {
        output::errorMismatch(lineno);
        exit(0);
    }
}

void check_bool_type(const char* type, int lineno) {
    if (!symbolTable.isBoolType(type)) {
        output::errorMismatch(lineno);
        exit(0);
    }
}

void check_byte_range(int value, int lineno) {
    if (value > 255) {
        output::errorByteTooLarge(lineno, std::to_string(value));
        exit(0);
    }
}

void check_while_scope(int lineno) {
    if (!symbolTable.isInsideWhileLoop()) {
        output::errorUnexpectedBreak(lineno);
        exit(0);
    }
}

const char* get_larger_type(const char* type1, const char* type2) {
    return symbolTable.getLargerType(type1, type2).c_str();
}

const char* get_variable_type(const char* id, int lineno) {
    check_defined_variable(id, lineno);
    return symbolTable.getSymbolType(id).c_str();
}

const char* get_function_return_type(const char* id, int lineno) {
    check_defined_function(id, lineno);
    return symbolTable.getFunctionReturnType(id).c_str();
}

void add_variable(const char* id, const char* type, int lineno) {
    if (!symbolTable.addSymbol(id, type)) {
        output::errorDef(lineno, id);
        exit(0);
    }
}

void add_function(const char* id, const char* returnType, const char* paramType, int lineno) {
    std::vector<std::string> params;
    if (paramType) {
        params.push_back(paramType);
    }
    if (!symbolTable.addFunction(id, returnType, params)) {
        output::errorDef(lineno, id);
        exit(0);
    }
}

void enter_scope() {
    symbolTable.enterScope();
}

void exit_scope() {
    symbolTable.exitScope();
}

void enter_while_loop() {
    symbolTable.enterWhileLoop();
}

void exit_while_loop() {
    symbolTable.exitWhileLoop();
}