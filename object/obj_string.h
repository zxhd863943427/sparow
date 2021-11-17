#ifndef _OBJECT_STRING_H
#define _OBJECT_STRING_H
#include "header_obj.h"

typedef struct
{
    ObjHeader objHeader;        //对象头
    uint32_t hashCode;          //哈希值
    charValue value;            //文本长度+可变数组的数据结构
}ObjString;

uint32_t hashString(const char* str, uint32_t length);
void hashObjString(ObjString* objString);
ObjString* newObjString(VM* vm, const char* str, uint32_t length);

#endif