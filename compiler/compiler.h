#ifndef _COMPILER_COMPILER_H
#define _COMPILER_COMPILER_H 
#include "obj_fn.h"


#define MAX_LOCAL_VAR_NUM 128
//最大本地变量数量
#define MAX_UPVALUE_NUM 128
//最大自由变量数量
#define MAX_ID_LEN 128
//变量名最大长度
#define MAX_METHOD_NAME_LEN 128
//方法名最大长度
#define MAX_ARG_NUM 16
//传递参数最大数量

#define MAX_SIGN_LEN MAX_METHOD_NAME_LEN + MAX_ARG_NUM*2 + 1
//函数签名最大长度 ，= 函数名长度 + '(' n个参数 + (n-1)个参数分隔符',' + ')'

#define MAX_FIELD_NUM 128

typedef struct
{
    bool isEnclosingLocalVar;   //如果此 upvalue 是直接外层函数的局部变量就置为 true,否则置为 false
    uint32_t index;             //外层函数中局部变量的索引或者外层函数中 upvalue 的索引,/这取决于 isEnclosingLocalVar 的值
}Upvalue;
//Upvalue的定义

typedef struct
{
    const char* name;
    uint32_t length;
    int scopeDepth;             //局部变量作用域
    bool isUpvalue;             //表示本函数中的局部变量是否是其内层函数所引用的 upvalue,如果是，由内部函数来设置此项为true
}LocalVar;
//局部变量

typedef enum
{
    SIGN_CONSTRUCT,             //构造函数
    SIGN_METHOD,                //普通方法
    SIGN_GETTER,                //getter方法
    SIGN_SETTER,                //setter方法
    SIGN_SUBSCRIPT,             //getter形式的下标,如list[1]
    SIGN_SUBSCRIPT_SETTER,      //setter形式的下标,如list[1] =9
}SignatureType;
//方法签名的类型

typedef struct
{
    SignatureType type;         //签名类型
    const char* name;           //签名
    uint32_t length;            //签名长度
    uint32_t argNum;            //参数个数
}Signature;
//方法签名数据类型

typedef struct loop
{
    int condStartIndex;         //循环中条件的地址
    int bodyStartIndex;         //循环体起始地址
    int scopeDepth;             //循环中若有 break,告诉它需要退出的作用域深度
    int exitIndex;              //循环条件不满足时跳出循环体的目标地址
    struct loop* enclosingLoop; //外层循环
}Loop;
//loop循环结构

typedef struct 
{
    ObjString* name;                //类名
    SymbolTable fields;             //参数个数
    bool inStatic;                  //若当前编译静态方法就为真
    IntBuffer instantMethods;       //实例方法
    IntBuffer staticMethods;        //实例方法
    Signature* signature;           //当前正在编译的签名
}ClassBookKeep;
//用于记录类编译时的信息

typedef struct compileUnit CompileUnit;

int defineModuleVar(VM* vm, ObjModule* objModule, const char* name, uint32_t length, Value value);

#endif