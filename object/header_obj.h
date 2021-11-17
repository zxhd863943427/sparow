#ifndef _INCLUDE_HEADER_OBJ_H
#define _INCLUDE_HEADER_OBJ_H
#include "utils.h"

//对象类型
typedef enum
{
    OT_CLASS,           //类
    OT_LIST,            //list类型
    OT_MAP,             //map类型
    OT_MODULE,          //模块类型
    OT_RANGE,           //迭代序列类型
    OT_STRING,          //string类型
    OT_UPVALUE,         //自由变量类型
    OT_FUNCTION,        //fnc类型
    OT_CLOSURE,         //闭包类型
    OT_INSTANCE,        //对象实例类型
    OT_THREAD,          //对象线程类型
}ObjType;

typedef struct objHeader
{
    ObjType type;
    bool isDark;
    Class* class;
    struct objHeader* next;
}ObjHeader;
//对象的对象头

typedef enum
{
    VT_UNDEFINED,       //未定义
    VT_NULL,            //null值
    VT_FALSE,           //bool中的False
    VT_TRUE,            //bool中的True
    VT_NUM,             //数字类型
    VT_OBJ,             //对象类型，class也是对象
} ValueType;
//储存在Value结构中的数据类型

//通用的数据结构
typedef struct
{
    ValueType type;             //数值类型，决定union中储存内容
    union
    {
        double num;             //数值
        ObjHeader* objHeader;   //指向对象头的指针
    };
}Value;
DECLARE_BUFFER_TYPE(Value);
void initObjHeader(VM* vm, ObjHeader* objHeader, ObjType objType, Class* class);

#endif