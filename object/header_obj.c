#include "header_obj.h"
#include "class.h"
#include "vm.h"

DEFINE_BUFFER_METHOD(Value)
//初始化对象头，并更新虚拟机中的链表节点
void initObjHeader(VM* vm, ObjHeader* objHeader, ObjType objType, Class* class)
{
    objHeader->type = objType;              //初始化对象的类型
    objHeader->isDark = false;              //初始化对象是否可达
    objHeader->class = class;               //初始化对象的父类
    objHeader->next = vm->allObjects;       //下面是更新链表
    vm->allObjects = objHeader;
}