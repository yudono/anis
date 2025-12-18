#include "map.h"

// keys(obj) -> array of keys
Value map_keys(std::vector<Value> args) {
    if (args.empty() || !args[0].isMap) return Value(std::vector<Value>{});
    
    std::vector<Value> keys;
    for (const auto& pair : *args[0].mapVal) {
        keys.push_back(Value(pair.first, 0, false));
    }
    return Value(keys);
}

// values(obj) -> array of values
Value map_values(std::vector<Value> args) {
    if (args.empty() || !args[0].isMap) return Value(std::vector<Value>{});
    
    std::vector<Value> values;
    for (const auto& pair : *args[0].mapVal) {
        values.push_back(pair.second);
    }
    return Value(values);
}

// entries(obj) -> array of [key, value] pairs
Value map_entries(std::vector<Value> args) {
    if (args.empty() || !args[0].isMap) return Value(std::vector<Value>{});
    
    std::vector<Value> entries;
    for (const auto& pair : *args[0].mapVal) {
        std::vector<Value> entry;
        entry.push_back(Value(pair.first, 0, false));
        entry.push_back(pair.second);
        entries.push_back(Value(entry));
    }
    return Value(entries);
}

// has(obj, key) -> boolean
Value map_has(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isMap) return Value("", 0, true);
    
    std::string key = args[1].toString();
    bool exists = args[0].mapVal->count(key) > 0;
    return Value("", exists ? 1 : 0, true);
}

// merge(obj1, obj2) -> new merged object
Value map_merge(std::vector<Value> args) {
    if (args.size() < 2 || !args[0].isMap || !args[1].isMap) 
        return Value(std::map<std::string, Value>{});
    
    std::map<std::string, Value> result = *args[0].mapVal;
    for (const auto& pair : *args[1].mapVal) {
        result[pair.first] = pair.second;
    }
    return Value(result);
}

// clone(obj) -> deep cloned object
Value map_clone(std::vector<Value> args) {
    if (args.empty() || !args[0].isMap) return Value(std::map<std::string, Value>{});
    
    // Shallow clone for now (deep clone would need recursive logic)
    std::map<std::string, Value> cloned = *args[0].mapVal;
    return Value(cloned);
}

// Register all map functions
void register_map_lib(Interpreter& interp) {
    interp.registerNative("obj_keys", map_keys);
    interp.registerNative("obj_values", map_values);
    interp.registerNative("obj_entries", map_entries);
    interp.registerNative("obj_has", map_has);
    interp.registerNative("obj_merge", map_merge);
    interp.registerNative("obj_clone", map_clone);
}
