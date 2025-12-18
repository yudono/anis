#include "array.h"
#include <algorithm>

// push(arr, item) -> modified array
Value array_push(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isList) return Value(std::vector<Value>{});
    
    args[0].listVal->push_back(args[1]);
    return args[0];
}

// pop(arr) -> removed item
Value array_pop(std::vector<Value> args) {
    if (args.empty() || !args[0].isList || args[0].listVal->empty()) 
        return Value("", 0, false);
    
    Value last = args[0].listVal->back();
    args[0].listVal->pop_back();
    return last;
}

// shift(arr) -> removed item
Value array_shift(std::vector<Value> args) {
    if (args.empty() || !args[0].isList || args[0].listVal->empty()) 
        return Value("", 0, false);
    
    Value first = args[0].listVal->front();
    args[0].listVal->erase(args[0].listVal->begin());
    return first;
}

// unshift(arr, item) -> modified array
Value array_unshift(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isList) return Value(std::vector<Value>{});
    
    args[0].listVal->insert(args[0].listVal->begin(), args[1]);
    return args[0];
}

// slice(arr, start, end) -> new array
Value array_slice(std::vector<Value> args) {
    if (args.empty() || !args[0].isList) return Value(std::vector<Value>{});
    
    auto& list = *args[0].listVal;
    int start = (args.size() > 1 && args[1].isInt) ? args[1].intVal : 0;
    int end = (args.size() > 2 && args[2].isInt) ? args[2].intVal : list.size();
    
    if (start < 0) start = 0;
    if (end > (int)list.size()) end = list.size();
    if (start >= end) return Value(std::vector<Value>{});
    
    std::vector<Value> result(list.begin() + start, list.begin() + end);
    return Value(result);
}

// concat(arr1, arr2) -> new array
Value array_concat(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isList || !args[1].isList) 
        return Value(std::vector<Value>{});
    
    std::vector<Value> result = *args[0].listVal;
    result.insert(result.end(), args[1].listVal->begin(), args[1].listVal->end());
    return Value(result);
}

// reverse(arr) -> modified array
Value array_reverse(std::vector<Value> args) {
    if (args.empty() || !args[0].isList) return Value(std::vector<Value>{});
    
    std::reverse(args[0].listVal->begin(), args[0].listVal->end());
    return args[0];
}

// sort(arr) -> modified array
Value array_sort(std::vector<Value> args) {
    if (args.empty() || !args[0].isList) return Value(std::vector<Value>{});
    
    std::sort(args[0].listVal->begin(), args[0].listVal->end(), 
        [](const Value& a, const Value& b) {
            if (a.isInt && b.isInt) return a.intVal < b.intVal;
            return a.toString() < b.toString();
        });
    return args[0];
}

// includes(arr, item) -> boolean
Value array_includes(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isList) return Value("", 0, true);
    
    for (const auto& item : *args[0].listVal) {
        if (item.isInt && args[1].isInt && item.intVal == args[1].intVal) 
            return Value("", 1, true);
        if (item.toString() == args[1].toString()) 
            return Value("", 1, true);
    }
    return Value("", 0, true);
}

// indexOf(arr, item) -> int
Value array_indexOf(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isList) return Value("", -1, true);
    
    auto& list = *args[0].listVal;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i].isInt && args[1].isInt && list[i].intVal == args[1].intVal) 
            return Value("", (int)i, true);
        if (list[i].toString() == args[1].toString()) 
            return Value("", (int)i, true);
    }
    return Value("", -1, true);
}

// join(arr, separator) -> string (alias for string.join)
Value array_join(std::vector<Value> args) {
    if (args.empty() || !args[0].isList) return Value("", 0, false);
    
    std::string separator = (args.size() > 1) ? args[1].toString() : ",";
    std::string result;
    
    auto& list = *args[0].listVal;
    for (size_t i = 0; i < list.size(); i++) {
        result += list[i].toString();
        if (i < list.size() - 1) result += separator;
    }
    
    return Value(result, 0, false);
}

// Register all array functions
void register_array_lib(Interpreter& interp) {
    interp.registerNative("arr_push", array_push);
    interp.registerNative("arr_pop", array_pop);
    interp.registerNative("arr_shift", array_shift);
    interp.registerNative("arr_unshift", array_unshift);
    interp.registerNative("arr_slice", array_slice);
    interp.registerNative("arr_concat", array_concat);
    interp.registerNative("arr_reverse", array_reverse);
    interp.registerNative("arr_sort", array_sort);
    interp.registerNative("arr_includes", array_includes);
    interp.registerNative("arr_indexOf", array_indexOf);
    interp.registerNative("arr_join", array_join);
}
