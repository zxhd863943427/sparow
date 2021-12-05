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
    ObjFn* fn;                                  //所编译的函数对象
    LocalVar localVars[MAX_LOCAL_VAR_NUM];      //作用域中允许的局部变量的数量上限
    uint32_t localVarNum;                       //已分配局部变量数
    Upvalue upvalues[MAX_UPVALUE_NUM];          //记录所引用的upvalue
    int scopeDepth;                             //记录当前编译代码的作用域
    uint32_t stackSlotNum;                      //当前使用的slot数
    Loop* curLoop;                              //当前正在编译的循环层
    ClassBookKeep* enclosingClassBK;            //当前正编译的类的编译信息
    struct compileUnit* enclosingUnit;          //指向包含此编译单元的编译单元，即直接外层
    Parser* curParser;                          //当前parser
};

//在模块 objModule 中定义名为 name,值为 value 的模块变量
int defineModuleVar(VM* vm, ObjModule* objModule, const char* name, uint32_t length, Value value)
{
    //判断命名是否长度大于最大限制长度，如果是，则复制长度限制范围内的文本
    //并根据对应情况输出报错信息
    if (length > MAX_ID_LEN)
    {
        char id[MAX_ID_LEN] = {'\0'};
        memcpy(id, name, length);
        //判断当前是否已生成词法分析器
        if (vm->curParser != NULL)
        {
            COMPILE_ERROR(vm->curParser,"length of identifier \'%s\' should be no more than %d ",id, MAX_ID_LEN);
        }
        else
        {
            MEM_ERROR("length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        }
    }
    int symbolIndex = getIndexFromSymbolTable(&(objModule->moduleVarName), name, length);
    //从模块变量名中查找变量,若不存在就添加
    if (symbolIndex == -1)
    {
        //添加变量名
        symbolIndex = addSymbol(vm, &(objModule->moduleVarName), name, length);
        ValueBufferAdd(vm, &(objModule->moduleVarValue), value);
    }
    //若遇到之前预先声明的模块变量的定义,在此为其赋予正确的值
    else if (VALUE_IS_NUM(objModule->moduleVarValue.datas[symbolIndex]))
    {
        objModule->moduleVarValue.datas[symbolIndex] = value;
    }
    else
    {
        symbolIndex = -1;       //已定义则返回-1,用于判断重定义
    }
    return symbolIndex;
}

//编译模块(目前是桩函数)
ObjFn* compileModule(VM* vm, ObjModule* objModule, const char* moduleCode)
{
    ;
}