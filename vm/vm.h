#ifndef _INCLUDE_VM_H
#define _INCLUDE_VM_H
#include "header_obj.h"
#include "common.h"
/* 定义虚拟机 */
struct vm
{
    Class* stringClass;
    uint32_t allocatedBytes;    /* 已分配储存空间 */
    ObjHeader* allObjects;      //所以已分配对象的链表
    Parser* curParser;          /* 指向当前词法分析器 */
};

void initVM(VM* vm);
VM* newVM(void);
#endif