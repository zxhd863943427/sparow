#include "compiler.h"
#include "parser.h"
#include "core.h"
#include <string.h>
#if DEBUG
    #include "debug.h"
#endif

//编译单元
struct compileUnit
{
    ObjFn* fn;                                  //
    LocalVar localVars[MAX_LOCAL_VAR_NUM];
    uint32_t localVarNum;
    Upvalue upvalues[MAX_UPVALUE_NUM];
    int scopeDepth;                             //
    uint32_t stackSlotNum;
    Loop* curLoop;                              //
    ClassBookKeep* enclosingClassBK;
    struct compileUnit* enclosingUnit;
    Parser* curParser;
};