#ifndef _OBJECT_MAP_H
#define _OBJECT_MAP_H
#include "header_obj.h"

#define MAP_LOAD_PERCENT 0.8
//map扩容的判断标准，利用率大于这个就扩容

typedef struct
{
    Value key;
    Value value;
}Entry;
//map对象中储存的建-值对

typedef struct
{
    ObjHeader objHeader;
    uint32_t capacity;      //Entry的总容量
    uint32_t count;         //Entry的使用量
    Entry* entries;         //指向Entry数组
}ObjMap;
//map对象

ObjMap* newObjMap(VM* vm);
void mapSet(VM* vm, ObjMap* objMap, Value key, Value value);
Value mapGet(ObjMap* objMap, Value key);
void clearMap(VM* vm, ObjMap* objMap);
Value removeKey(VM* vm, ObjMap* objMap, Value key);

#endif