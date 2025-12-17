#include "core/lang/interpreter.h"
#include "lib/gui/minigui.h"
#include <iostream>

struct GuiLib {
    static void register_gui(Interpreter& interpreter) {
         interpreter.registerNative("render_gui", [&](std::vector<Value> args) {
             std::cout << "Starting GUI from Sunda..." << std::endl;
             
             // Bridge declarations
             // Bind Click (Generic)
             interpreter.registerNative("bind_native_click", [&](std::vector<Value> args) {
                  if (args.size() >= 2 && args[1].isClosure) {
                      std::string id = args[0].strVal;
                      // bind_click is from minigui.h
                      Value v = args[1]; // Capture Value
                      bind_click(id, [id, v, &interpreter]() { 
                          interpreter.executeClosure(v); 
                          request_rerender(); 
                      });
                  }
                  return Value("", 0, true);
             });
             
             // Update Hook Native
             interpreter.registerNative("updateHook", [&](std::vector<Value> args) {
                 if (args.size() >= 2) {
                     int idx = args[0].intVal;
                     Value newVal = args[1];
                     if (idx >= 0 && idx < interpreter.hooks.size()) {
                         interpreter.hooks[idx] = newVal;
                         request_rerender();
                     }
                 }
                 return Value("", 0, true);
             });
             
             // Bridge setState (Hooks Style)
             interpreter.registerNative("setState", [&](std::vector<Value> args) {
                  int idx = interpreter.hookIndex++;
                  
                  if (idx >= interpreter.hooks.size()) {
                       Value init = args.empty() ? Value("", 0, true) : args[0];
                       interpreter.hooks.push_back(init);
                  }
                  
                  Value currentVal = interpreter.hooks[idx];
                  
                  // Synthesize Setter Closure: (val) => updateHook(idx, val)
                  // AST: CallExpr("updateHook", [Literal(idx), Var("val")])
                  auto callExpr = std::make_shared<CallExpr>("updateHook", std::vector<std::shared_ptr<Expr>>{
                      std::make_shared<LiteralExpr>(std::to_string(idx), false),
                      std::make_shared<VarExpr>("val")
                  });
                  
                  auto block = std::make_shared<BlockStmt>();
                  block->statements.push_back(std::make_shared<ExprStmt>(callExpr));
                  
                  Value setterClosure(block, interpreter.environment, std::vector<std::string>{"val"});
                  
                  // Return List: [currentVal, setterClosure]
                  std::vector<Value> retList;
                  retList.push_back(currentVal);
                  retList.push_back(setterClosure);
                  return Value(retList);
             });
             
             interpreter.registerNative("print", [&](std::vector<Value> args) {
                 for (auto& a : args) std::cout << a.toString();
                 return Value("", 0, true);
             });
             interpreter.registerNative("println", [&](std::vector<Value> args) {
                 for (auto& a : args) std::cout << a.toString();
                 std::cout << std::endl;
                 return Value("", 0, true);
             });

             // Start Main Loop via minigui
             render_gui([&](){
                  interpreter.resetHooks(); // Reset index each render
                  Value v = interpreter.getGlobal("App");
                  if (v.isClosure) {
                      Value ret = interpreter.callClosure(v);
                      return ret.toString();
                  }
                  return std::string("<Page><Text>Error: App not found</Text></Page>");
             });
             
             return Value("", 0, true);
         });
    }
};
