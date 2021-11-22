#include "obj_map.h"
#include "class.h"

ObjMap* newObjMap(VM* vm)
{
    ObjMap* objMap = ALLOCATE(vm,ObjMap);
    initObjHeader(vm, &(objMap->objHeader), OT_MAP, vm->mapClass);
    objMap->capacity = objMap->count = 0;
    objMap->entries = NULL;
    return objMap;
}

static uint32_t hashNum(double num)
{
    Bits64 bits64;
    bits64.num = num;
    return bits64.bits32[0] ^ bits64.bits32[1];
}

static uint32_t hashObj(ObjHeader* objHeader)
{
    switch (objHeader->type)
    {
        case OT_CLASS:
        {
            Class* class = (Class*)objHeader;
            //由于目前还没有创建类的函数，所以只能先用原始方法获取class->name的hash值
            return hashString(class->name->value.start, class->name->value.length);
        }
        case OT_RANGE:
        {
            ObjRange* objRange = (ObjRange*)objHeader;
            return hashNum(objRange->from) ^ hashNum(objRange->to);
        }
        case OT_STRING:
        {
            //因为string对象里已经有hash值了，直接调用返回
            ObjString* objString = (ObjString*)objHeader;
            return objString->hashCode;
        }
        default:
            RUN_ERROR("the hashable are objstring, objrange and class.\n 这是一个非objstring, objrange 或class的对象,它不能作为可哈希对象的键");
            NOT_REACHED();
    }
}

static uint32_t hashValue(Value value)
{
    switch (value.type)
    {
        case VT_FALSE:
            return 0;
        case VT_TRUE:
            return 1;
        case VT_NULL:
            return 2;
        case VT_NUM:
            return hashNum(value.num);
        case VT_OBJ:
            return hashObj(value.objHeader);
        default:
            RUN_ERROR("unsupport type hashed!");
    }
    return 0;
}

//在Entry数组中添加键值对
static bool addEntry(Entry* entries, uint32_t capacity, Value key, Value value)
{
    //对容量取余获得地址
    uint32_t index = hashValue(key) % capacity;
    //开发检测法搜索下一个可用slot
    while (true)
    {
        //若key未被占用，直接赋值并返回true
        if (entries[index].key.type == VT_UNDEFINED)
        {
            entries[index].key = key;
            entries[index].value = value;
            return true;
        }
        //若key已被占用，判断是否为相同的key，若是，更新值并返回false
        else if (valueIsEqual(entries[index].key, key))
        {
            entries[index].value = value;
            return false;
        }
        //获取下一个尝试的slot，用求余运算防止超过容量
        index = (index + 1) % capacity;
    }
}

//调整objMap 的容量为新的newCapacity
static void resizeMap(VM* vm, ObjMap* objMap, uint32_t newCapacity)
{
    //建立新的entries
    Entry* newEntries = ALLOCATE_ARRAY(vm, Entry, newCapacity);
    uint32_t index = 0;
    while (index < newCapacity)
    {
        newEntries[index].key = VT_TO_VALUE(VT_UNDEFINED);
        newEntries[index].value = VT_TO_VALUE(VT_FALSE);
        index++;
    }
    //转移entries
    index = 0;
    while (index < objMap->capacity)
    {
        Entry* entryArr = objMap->entries;
        //判断是否为空闲slot，如果不是，将对应的slot加入到新的newEntries中
        if (entryArr[index].key.type != VT_UNDEFINED)
        {
            addEntry(newEntries, newCapacity, entryArr[index].key, entryArr[index].value);
        }
        index++;
    }
    //释放旧的entries的空间
    DEALLOCATE_ARRAY(vm, objMap->entries, objMap->capacity);
    objMap->entries = newEntries;
    objMap->capacity = newCapacity;
}

//objMap 中查找对应的entry，返回entry位置的指针
static Entry* findEntry(ObjMap* objMap, Value key)
{
    //如果map的容量为0，不需要查找，直接返回NULL
    if (objMap->capacity == 0)
    {
        return NULL;
    }
    //否则开始用开放检测法查找slot
    //先哈希，再取模，获得index
    uint32_t index = hashValue(key) % objMap->capacity;
    Entry* entry;
    while (true)
    {
        entry = &(objMap->entries[index]);
        //比较key是否相等，是则返回entry
        if (valueIsEqual(entry->key, key))
        {
            return entry;
        }
        //如果不是，判断判断检测链是否断裂，是则返回NULL，表示未找到
        else if (VALUE_IS_UNDEFINED(entry->key) && VALUE_IS_FALSE(entry->value))
        {
            return NULL;
        }
        //查找下一个slot
        index = (index + 1) %objMap->capacity;
    }
}

// objMap 中实现 key 与 value 的关联:objMap[key]=value
void mapSet(VM* vm, ObjMap* objMap, Value key, Value value)
{
    //如果利用率大于80%，就扩容
    if (objMap->count + 1 > objMap->capacity * MAP_LOAD_PERCENT)
    {
        uint32_t newCapacity = objMap->capacity * CAPACITY_GROW_FACTOR;
        if (newCapacity < MIN_CAPACITY)
        {
            newCapacity = MIN_CAPACITY;
        }
        resizeMap(vm, objMap, newCapacity);
    }
    //如果添加键成功，而不是更新键，就增加count的值
    if (addEntry(objMap->entries, objMap->capacity, key, value))
    {
        objMap->count++;
    }
}

//从 map 中查找 key 对应的 value: map[key] 
Value mapGet(ObjMap* objMap, Value key)
{
    Entry* entry = findEntry(objMap, key);
    if (entry == NULL)
    {
        return VT_TO_VALUE(VT_UNDEFINED);
    }
    return entry->value;
}

//回收 objMap.entries 占用的空间
void clearMap(VM* vm, ObjMap* objMap)
{
    DEALLOCATE_ARRAY(vm, objMap->entries, objMap->capacity);
    objMap->entries = NULL;
    objMap->capacity = objMap->count = 0;
}

//删除 objMap 中的 key 的同时，返回 map[key] 
Value removeKey(VM* vm, ObjMap* objMap, Value key)
{
    //调用findEntry()函数查找entry
    Entry* entry = findEntry(objMap, key);
    if (entry == NULL)
    {
        return VT_TO_VALUE(VT_NULL);
    }
    else 
    {
        Value value = entry->value;
        entry->key = VT_TO_VALUE(VT_UNDEFINED);
        entry->value = VT_TO_VALUE(VT_TRUE);
        
        //将count的值减小
        objMap->count--;
        //判断是否要回收或减小entries的大小
        //判断是否已经没有entry储存
        if (objMap->count == 0)
        {
            clearMap(vm, objMap);
        }
        //先判断capacity是否小于 MIN_CAPACITY ，
        //再判断 capacity 缩小后利用率会不会小于 80%
        else if (objMap->capacity > MIN_CAPACITY &&
                 objMap->count > (objMap->capacity / CAPACITY_GROW_FACTOR) * MAP_LOAD_PERCENT)
        {
            uint32_t newCapacity = objMap->capacity / CAPACITY_GROW_FACTOR;
            //判断缩小后capacity 会不会小于 MIN_CAPACITY
            if (newCapacity < MIN_CAPACITY)
            {
                newCapacity = MIN_CAPACITY;
            }
            resizeMap(vm, objMap, newCapacity);
        }
        return value;
    } 
}