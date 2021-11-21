#include "obj_list.h"

//新建list对象
ObjList* newObjList(VM* vm, uint32_t elementNum)
{
    Value* elementArray =  NULL;
    //先分配内存
    if (elementNum > 0)
    {
        elementArray = ALLOCATE_ARRAY(vm, Value, elementNum);
    }
    ObjList* objList = ALLOCATE(vm,ObjList);
    objList->elements.datas = elementArray;
    objList->elements.capacity = objList->elements.count = elementNum;
    initObjHeader(vm, &(objList->objHeader), OT_LIST, vm->listClass);
    return objList;
}

//objList插入操作
void insertElement(VM* vm, ObjList* objList, uint32_t index, Value value)
{
    if (index > objList->elements.count -1)
    {
        RUN_ERROR("index out bounded! 数组插入数据时超过索引限制！");
    }
    //准备一个后移的空间
    ValueBufferAdd(vm,&objList->elements,VT_TO_VALUE(VT_NULL));
    //将index后的元素全部后移
    uint32_t idx = objList->elements.count -1 ;
    while(idx > index)
    {
        objList->elements.datas[idx] = objList->elements.datas[idx - 1];
        idx--;
    }
    //后移后插入
    objList->elements.datas[index] = value;
}

//调整list容量，为list删除部分元素调用
static void shrinkList(VM* vm, ObjList* objList, uint32_t newCapacity)
{
    uint32_t oldSize = objList->elements.capacity * sizeof(Value);
    uint32_t newSize = newCapacity * sizeof(Value);
    memManager(vm, objList->elements.datas, oldSize, newSize);
    objList->elements.capacity = newCapacity;
}

//删除元素
Value removeElement(VM* vm, ObjList* objList, uint32_t index)
{
    if (index > objList->elements.count)
    {
        RUN_ERROR("index out bounded! 正在试图删除不存在于数组中的数据！");
    }
    Value valueRemoved = objList->elements.datas[index];
    uint32_t idx = index;
    //当idx == objList->elements.count-1 时，循环会终止，刚好将数组index后元素前移一位
    while (idx < objList->elements.count)
    {
        objList->elements.datas[idx] = objList->elements.datas[idx + 1];
        idx++;
    }
    //判断数组使用率，过低则调用shrinkList()减小容量
    uint32_t _capacity = objList->elements.capacity / CAPACITY_GROW_FACTOR;
    if (_capacity > objList->elements.count)
    {
        shrinkList(vm, objList, _capacity);
    }
    objList->elements.count--;
    return valueRemoved;
}
