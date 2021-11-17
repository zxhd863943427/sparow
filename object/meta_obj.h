#ifndef _OBJECT_METAOBJ_H
#define _OBJECT_METAOBJ_H
#include "obj_string.h"


typedef struct
{
    ObjHeader objHeader;            //对象头
    SymbolTable moduleVarName;      //模块中变量的名
    ValueBuffer moduleVarValue;     //模块这变量的值
    ObjString* name;                //模块名
}ObjModule;
//模块对象

typedef struct
{
    ObjHeader objHeader;            //对象头
    Value fields[0];                 //具体字段，及对象实例的属性数组
}ObjInstance;
//对象的实例结构，包含一个通用数值结构Value的可变数组以及对象头

ObjModule* newObjModule(VM* vm, const char* modName);
ObjInstance* newObjInstance(VM* vm, Class* class);

#endif