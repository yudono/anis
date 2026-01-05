#ifndef ANIS_MAP_LIB_H
#define ANIS_MAP_LIB_H

#include "../../core/lang/interpreter.h"
#include <map>
#include <string>

// Map/Object manipulation functions
Value map_keys(std::vector<Value> args);
Value map_values(std::vector<Value> args);
Value map_entries(std::vector<Value> args);
Value map_has(std::vector<Value> args);
Value map_merge(std::vector<Value> args);
Value map_clone(std::vector<Value> args);

// Registration function
void register_map_lib(Interpreter& interp);

#endif
