#ifndef _OBJECT_CLASS_H
#define _OBJECT_CLASS_H
#include "common.h"
#include "utils.h"
#include "header_obj.h"
#include "obj_string.h"
#include "obj_fn.h"
#include "obj_list.h"
#include "obj_range.h"

typedef enum
{
    MT_NONE,            //空方法类
    MT_PRIMITIVE,       //用c实现的原生方法
    MT_SCRIPT,          //在脚本语言中实现的自定义方法
    MT_FN_CALL,         //函数对象的调用方法，用于实现自定义函数
} MethodType;
//将类型转换为Value数据结构
#define VT_TO_VALUE(vt) ((Value){vt,{0}})

//将bool类型转换为Value数据结构
#define BOOL_TO_VALUE(boolean) (boolean ? VT_TO_VALUE(VT_TRUE) : VT_TO_VALUE(VT_FALSE))
#define VALUE_TO_BOOL(value) ((value).type == VT_TRUE ? true : false)

//将num类型转换为Value数据结构
#define NUM_TO_VALUE(num) ((Value){VT_NUM, {num}})
#define VALUE_TO_NUM(value) value.num

//将对象指针转换为Value数据结构
#define OBJ_TO_VALUE(objPtr) ({ \
        Value value;\
        value.type = VT_OBJ;\
        value.objHeader = (ObjHeader*) (objPtr);\
        value;\
        })

//将value转化为obj结构
#define VALUE_TO_OBJ(value) (value.objHeader)

#define VALUE_TO_OBJSTR(value) ((ObjString*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJFN(value) ((ObjFn*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJCLOSURE(value) ((ObjClosure*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJRANGE(value) ((ObjRange*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJLIST(value) ((ObjList*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMAP(value) ((ObjMap*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJTHREAD(value) ((ObjThread*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMODULE(value) ((ObjModule*)VALUE_TO_OBJ(value))
#define VALUE_TO_CLASS(value) ((Class*)VALUE_TO_OBJ(value))



//判断普通类型
#define VALUE_IS_UNDEFINED(value) ((value).type == VT_UNDEFINED)
#define VALUE_IS_NULL(value) ((value).type == VT_NULL)
#define VALUE_IS_TRUE(value) ((value).type == VT_TRUE) 
#define VALUE_IS_FALSE(value) ((value).type == VT_FALSE)
#define VALUE_IS_NUM(value) ((value).type == VT_NUM)

//判断是否为对象类型
#define VALUE_IS_OBJ(value) ((value).type == VT_OBJ)
//判断是否为某个对象类型
//先判断是不是对象，再转换为对象查看指针所指的对象头类型是不是对应的类型
#define VALUE_IS_CERTAIN_OBJ(value, objType)  \
    (VALUE_IS_OBJ(value) && VALUE_TO_OBJ(value)->type == objType)

//接下来的方法都是调用上面函数的
#define VALUE_IS_OBJSTR(value) (VALUE_IS_CERTAIN_OBJ(value, OT_STRING))
#define VALUE_IS_OBJINSTANCE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_INSTANCE))
#define VALUE_IS_OBJCLOSURE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_CLOSURE))
#define VALUE_IS_RANGE(value) (VALUE_IS_CERTAIN_OBJ(value, OT_RANGE))
#define VALUE_IS_CLASS(value) (VALUE_IS_CERTAIN_OBJ(value, OT_CLASS))

#define VALUE_IS_0(value) (VALUE_IS_NUM(value) && (value).num == 0)

//原生方法指针，指向c完成的方法
typedef bool (*Primitive)(VM* vm, Value* args);

//通用调用方法，type的内容决定了union的储存内容和调用方式
typedef struct
{
    MethodType type;
    union
    {
        Primitive PrimFn;       //指向原生方法
        ObjClosure* obj;        //指向脚本自定义实现的方法
    };
} Method;

DECLARE_BUFFER_TYPE(Method)

/* 定义class*/
struct class
{
    ObjHeader objHeader;        //对象头
    struct class* superClass;   //父类
    uint32_t fieldNum;          //字段数，或者说属性数，会继承父类的属性
    MethodBuffer methods;       //本类的方法
    ObjString* name;            //类名
};

typedef union
{
    uint64_t bits64;            //一个无符号64位整数
    uint32_t bits32[2];         //两个无符号32位整数
    double num;                 //一个双精度浮点数
}Bits64;
//64位的可变储存单元

#define CAPACITY_GROW_FACTOR 4  
//map 和 list扩容的系数

#define MIN_CAPACITY 64         
//map 扩容相关

bool valueIsEqual(Value a, Value b);

#endif