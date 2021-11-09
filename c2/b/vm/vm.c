#include <stdlib.h>
#include "vm.h"
#include "utils.h"

void initVM(VM* vm)
{
    vm ->allocatedBytes=0;
    vm->curParser=NULL;
}
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