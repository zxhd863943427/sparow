#include "obj_string.h"
#include <string.h>
#include <stdlib.h>
#include "vm.h"
#include "utils.h"
#include "common.h"

//哈希算法——fnv-la算法
uint32_t hashString(const char* str, uint32_t length)
{
    uint32_t hashCode = 2166136261, idx = 0;
    while (idx < length)
    {
        hashCode ^= str[idx];
        hashCode *= 16777629;
        idx++;
    }
    return hashCode;
}

//输入一个 ObjSrting 对象，调用 hashString() 函数计算哈希值并以此更新 ObjString 的属性
void hashObjString(ObjString* objString)
{
    objString->hashCode = hashString(objString->value.start, objString->value.length);
}

//以字符串创建新的ObjString对象
ObjString* newObjString(VM* vm,const char* str, uint32_t length)
{
    //支持创建长度为0的字符，但此时str必须为NULL，当length不为0时，str不能为NULL
    ASSERT(length == 0 || str != NULL, "str length don`t match length ! ");
    //为对象申请空间，长度加一是为了末尾的'\0'
    ObjString* objString = ALLOCATE_EXTRA(vm, ObjString, length + 1);

    if (objString != NULL)
    {
        initObjHeader(vm, &objString->objHeader, OT_STRING, vm->stringClass);
        objString->value.length = length;
        if (length > 0)                         //若长度不为零，则将字符串复制到对象中
        {
            memcpy(objString->value.start, str, length);
        }
        objString->value.start[length] = '\0';  //在字符串末尾填'\0'
        hashObjString(objString);               //初始化哈希值
    }
    else
    {
        MEM_ERROR("Allocating ObjString failed!");
    }
    return objString;
}