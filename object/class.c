#include "class.h"
#include "common.h"
#include "string.h"
#include "obj_range.h"
#include "core.h"
#include "vm.h"

DEFINE_BUFFER_METHOD(Method);

//判断两个对象是否相等的方法
bool valueIsEqual(Value a, Value b)
{
    //判断类型是否相等，如果否，则返回false
    if (a.type != b.type)
    {
        return false;
    }

    //如果是数字类型，比较数字
    if (a.type == VT_NUM)
    {
        return a.num == b.num;
    }
    //否则，则为对象类型
    //开始判断是不是同一个对象
    if (a.objHeader == b.objHeader )
    {
        return true;
    }
    //在不是同一个对象的情况下，判断类型，类型不同不能比较
    if (a.objHeader->type != b.objHeader->type)
    {
        return false;
    }
    //如果是同个类型
    //如果同样是str类型，比较字符串
    if (a.objHeader->type == OT_STRING)
    {
        ObjString* strA = VALUE_TO_OBJSTR(a);
        ObjString* strB = VALUE_TO_OBJSTR(b);
        //先比较长度，在比较字符串内容
        return (strA->value.length == strB->value.length && memcmp(strA->value.start, strB->value.start, strA->value.length));
    }
    //如果同样是range类型，比较from和to
    if (a.objHeader->type == OT_RANGE)
    {
        ObjRange* ranA = VALUE_TO_OBJRANGE(a);
        ObjRange* ranB = VALUE_TO_OBJRANGE(b);
        //比较from和to
        return (ranA->from == ranB ->from && ranA->to == ranB->to);
    }
    //其他相同类型在对象头不一样的情况下也无法比较，直接返回false
    return false;
}