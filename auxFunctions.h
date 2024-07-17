#ifndef HW3COMP_HELPERFUNCTIONS_H
#define HW3COMP_HELPERFUNCTIONS_H

#include "parser.hpp"

void check_valid_assignment(const char* type1, const char* type2, int lineno);
void check_defined_variable(const char* id, int lineno);
void check_defined_function(const char* id, int lineno);
void check_matching_types(const char* type1, const char* type2, int lineno);
void check_numeric_type(const char* type, int lineno);
void check_bool_type(const char* type, int lineno);
void check_byte_range(int value, int lineno);
void check_while_scope(int lineno);
const char* get_larger_type(const char* type1, const char* type2);
const char* get_variable_type(const char* id, int lineno);
const char* get_function_return_type(const char* id, int lineno);
void add_variable(const char* id, const char* type, int lineno);
void add_function(const char* id, const char* returnType, const char* paramType, int lineno);
void enter_scope();
void exit_scope();
void enter_while_loop();
void exit_while_loop();

#endif //HW3COMP_HELPERFUNCTIONS_H