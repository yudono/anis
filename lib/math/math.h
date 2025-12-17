#include "core/lang/interpreter.h"
#include <cstdlib>
#include <ctime>

// Math Library
struct MathLib {
    static void register_math(Interpreter& interpreter) {
         interpreter.registerNative("random", [](std::vector<Value> args) {
             // random(min, max)
             int min = 0;
             int max = 100;
             if (args.size() >= 1) min = args[0].intVal;
             if (args.size() >= 2) max = args[1].intVal;
             if (args.size() >= 2) max = args[1].intVal;
             
             static bool seeded = false;
             if (!seeded) {
                 std::srand(std::time(nullptr));
                 seeded = true;
             }
             
             int r = min + (std::rand() % (max - min + 1));
             return Value("", r, true);
         });
    }
};
