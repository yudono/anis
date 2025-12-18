#ifndef SUNDA_ARRAY_LIB_H
#define SUNDA_ARRAY_LIB_H

#include "../../core/lang/interpreter.h"
#include <vector>

// Array manipulation functions
Value array_push(std::vector<Value> args);
Value array_pop(std::vector<Value> args);
Value array_shift(std::vector<Value> args);
Value array_unshift(std::vector<Value> args);
Value array_slice(std::vector<Value> args);
Value array_concat(std::vector<Value> args);
Value array_reverse(std::vector<Value> args);
Value array_sort(std::vector<Value> args);
Value array_includes(std::vector<Value> args);
Value array_indexOf(std::vector<Value> args);
Value array_join(std::vector<Value> args);

// Registration function
void register_array_lib(Interpreter& interp);

#endif
