#include <string.h>
#include "obj_fn.h"
#include "vm.h"
#include "class.h"
#include "meta_obj.h"

//建立一个新module对象
ObjModule* newObjModule(VM* vm, const char* modName)
{
    ObjModule* objModule = ALLOCATE(vm, ObjModule); 
    if (objModule == NULL)
    {
        MEM_ERROR("allocate ObjModule failed!");
    }
    //因为为元信息对象，所以父类为NULL
    initObjHeader(vm, &objModule->objHeader, OT_MODULE, NULL);
    //初始化moduleVarName，moduleVarValue
    StringBufferInit(&(objModule->moduleVarName));
    ValueBufferInit(&(objModule->moduleVarValue));

    //若modName为NULL，则为核心类，直接将objModule->name置为NULL，若不是，则申请一个
    //String对象用于储存
    if (modName != NULL)
    {
        objModule->name = newObjString(vm, modName, strlen(modName));
    }
    else
    {
        objModule->name = NULL;
    }
    return objModule;
}
//创建类 class 的实例
//参数 class 主要作用是提供类中 field 的数目
ObjInstance* newObjInstance(VM* vm, Class* class)
{
    ObjInstance* objInstance = ALLOCATE_EXTRA(vm, ObjInstance, sizeof(Value) * class->fieldNum);
    initObjHeader(vm, &objInstance->objHeader, OT_INSTANCE, class);
    uint32_t idx = 0;
    while (idx < class->fieldNum)
    {
        objInstance->fields[idx] = VT_TO_VALUE(VT_NULL);
        idx++;
    }
    return objInstance;
}
