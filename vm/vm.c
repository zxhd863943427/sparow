#include <stdlib.h>
#include "vm.h"
#include "utils.h"

void initVM(VM* vm)
{
    vm ->allocatedBytes=0;
    vm->allObjects = NULL;
    vm->curParser=NULL;
}
//创建虚拟机并初始化，返回一个虚拟机的指针
VM* newVM(void)
{
    VM* vm = (VM*)malloc(sizeof(VM));
    if (vm==NULL)
    {
        MEM_ERROR("allcoate VM memory Failed!");
    }
    initVM(vm);
    return vm;
}