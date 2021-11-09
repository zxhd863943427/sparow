#ifndef _INCLUDE_VM_H
#define _INCLUDE_VM_H
#include "common.h"
/* 定义虚拟机 */
struct vm
{
    /* 已分配储存空间 */
    uint32_t allocatedBytes;
    /* 指向当前词法分析器 */
    Parser* curParser;
};

void initVM(VM* vm);
VM* newVM(void);
#endif