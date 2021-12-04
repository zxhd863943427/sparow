#ifndef _INCLUDE_VM_H
#define _INCLUDE_VM_H
#include "header_obj.h"
#include "common.h"
#include "obj_map.h"
#include "obj_thread.h"

typedef enum vmResult
{
    VM_RESULT_SUCCESS,
    VM_RESULT_ERROR
}VMResult;      //虚拟机执行结果
//如果执行无误,可以将字符码输出到文件缓存,避免下次重新编译

/* 定义虚拟机 */
struct vm
{
    Class* classOfClass;        //元类
    Class* objectClass;            //类
    Class* stringClass;         //string对象的父类
    Class* mapClass;            //map对象的父类
    Class* rangeClass;          //range对象的父类
    Class* listClass;           //list对象的父类
    Class* nullClass;           //null对象的父类
    Class* boolClass;           //bool对象的父类
    Class* numClass;            //num对象的父类
    Class* fnClass;             //fn对象的父类
    Class* threadClass;         //thread对象的父类

    uint32_t allocatedBytes;    /* 已分配储存空间 */
    ObjHeader* allObjects;      //所以已分配对象的链表
    SymbolTable allMethodNames; //所以类的方法名
    ObjMap* allModules;
    ObjThread* curThread;       //当前正在执行的线程
    Parser* curParser;          /* 指向当前词法分析器 */
};

void initVM(VM* vm);
VM* newVM(void);
#endif