#include "obj_range.h"
#include "utils.h"
#include "class.h"
#include "vm.h"

ObjRange* newObjRange(VM* vm, int from, int to)
{
    ObjRange* objRange = ALLOCATE(vm, ObjRange);
    initObjHeader(vm, &(objRange->objHeader), OT_RANGE, vm->rangeClass);
    objRange->from = from;
    objRange->to = to;
    return objRange;
}
