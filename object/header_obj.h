#ifndef _INCLUDE_HEADER_OBJ_H
#define _INCLUDE_HEADER_OBJ_H
#include "utils.h"

typedef enum
{
    OT_CLASS,
    OT_LIST,
    OT_MAP,
    OT_MODULC,
    OT_RANGE,
    OT_STRING,
    OT_UPVALUE,
    OT_FUNCTION,
    OT_CLOSURE,
    OT_INSTANCE,
    OT_THREAD,
}ObjType;

typedef struct objHeader
{
    ObjType type;
    bool isDark;
    Class* class;
    struct objHeader* next;
}ObjHeader;

typedef enum
{
    VT_UNDEFINED,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ,
} ValueType;

typedef struct

#endif