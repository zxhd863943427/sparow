#ifndef _OBJECT_FN_H
#define _OBJECT_FN_H
#include "utils.h"
#include "meta_obj.h"

typedef struct
{
    char* fnName;       //函数名
    IntBuffer lineNo;   //行号
}FnDebug;
//在函数中的调试结构

typedef struct
{
    ObjHeader objHeader;
    ByteBuffer instrStream;     //编译后的指令流
    ValueBuffer constants;      //函数中的常量表，如模块对象中的模块变量、类的类名

    ObjModule* module;          //

    //本函数的最大所需栈空间，在编译时统计
    uint32_t maxStackSlotUsedNum;
    //本函数所涵盖的upvalue数量
    uint32_t upvalueNum;
    //本函数期待的参数个数
    uint8_t argNum;

//设立调试模式开关
#ifdef DEBUG
    FnDebug* debug;
#endif

}ObjFn;     
//函数对象


typedef struct upvalue
{
    ObjHeader objHeader;
    Value* localVarPtr;         //指向目标值的指针，当函数栈被回收后，会指向closedUpvalue
    Value closedUpvalue;        //已被关闭后被保存的upvalue
    struct upvalue* next;       //用以链接openUpvalue链表
}ObjUpvalue;
//upvalue对象

typedef struct
{
    ObjHeader objHeader;        
    ObjFn* fn;                  //闭包对象中所引用的函数
    ObjUpvalue* upvalues[0];    //柔性数组，用于储存该函数的closed upvalue的指针
}ObjClosure;
//闭包对象，既一个函数与他的自由变量的集合结构

typedef struct
{
    uint8_t* ip;                //程序计数器，用于指向下一个被执行的指令
    ObjClosure* closure;        //在本frame中被执行的闭包函数
    Value* stackStart;          //frame是共享thread.stack的，故需要此项指向本thread运行时stack的起始位置
}Frame;
//调用框架，不是对象

#define INITIAL_FRAME_NUM 4
//获得一个新的空upvalue对象
ObjUpvalue* newObjUpvalue(VM* vm, Value* localVarPtr);
//获得一个新的空Closure闭包对象
ObjClosure* newObjClosure(VM* vm, ObjFn* objFn);
//获得一个新的空Fn函数对象
ObjFn* newObjFn(VM* vm, ObjModule* objModule, uint32_t maxStackSlotUsedNum);

#endif