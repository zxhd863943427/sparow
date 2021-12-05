#include "core.h"


char *rootDir = NULL;                       //根目录

#define CORE_MODULE VT_TO_VALUE(VT_NULL)

//栈指令，将返回值放置于args[0]的位置
//返回值类型是 Value 类型,且是放在 args[0], args 是 Value 数组
//RET_VALUE 的参数就是 Value 类型,无须转换直接赋值
//它是后面"RET_其他类型 "的基础
#define RET_VALUE(value)\
    do\
    {\
        args[0] = value;\
        return true;\
    }while(0);

#define RET_OBJ(obj) RET_VALUE(OBJ_TO_VALUE(obj))
#define RET_BOOL(boolean) RET_VALUE(BOOL_TO_VALUE(boolean))
#define RET_NUM(num) RET_VALUE(NUM_TO_VALUE(num))
#define RET_NULL RET_VALUE(VT_TO_VALUE(VT_NULL))
#define RET_TRUE RET_VALUE(VT_TO_VALUE(VT_TRUE))
#define RET_FALSE RET_VALUE(VT_TO_VALUE(VT_FALSE))

//设置线程报错,写入对应线程的报错信息，并设置返回值为false
#define SET_ERROR_FALSE(vmPtr, errMsg) \
    do\
    {\
        vmPtr->curThread->errorObj =\
        OBJ_TO_VALUE(newObjString(vmPtr, errMsg, strlen(errMsg)));\
        return false;\
    }while(0);\

//绑定方法 func 到 classPtr 指向的类
#define PRIM_METHOD_BIND(classPtr, methodName, func) \
{\
    uint32_t length = strlen(methodName);\
    int globalIdx =getIndexFromSymbolTable(&(vm->allMethodNames), methodName, length);\
    if (globalIdx == -1)\
    {\
        globalIdx = addSymbol(vm, &(vm->allMethodNames), methodName, length);\
    }\
    Method method;\
    method.type = MT_PRIMITIVE;\
    method.PrimFn = func;\
    bindMethod(vm, classPtr, (uint32_t)globalIdx, method);\
}\

char* readFile(const char* path)
{
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        IO_ERROR("couldn`t open file in %s .\n",path);
    }
    struct stat fileStat;
    stat(path,&fileStat);
    size_t fileSize = fileStat.st_size;
    char* fileContent = (char*)malloc(fileSize+1);
    if(fileContent==NULL)
    {
        MEM_ERROR("could not allocate memory for reading file \"%s\" .\n",path);
    }
    size_t numRead = fread(fileContent,sizeof(char),fileSize,file);
    if (numRead<fileSize)
    {
        IO_ERROR("could not read file \"%s\".\n", path);
    }
    fileContent[fileSize]='\0';
    fclose(file);
    return fileContent;
}


//实现原生方法

//!object: object 取反,结果为 false 
static bool primObjectNot(VM* vm UNUSED, Value* args)
{
    RET_FALSE;
}

//args[0] == args[1]: 返回 object 是否相等
static bool primObjectEqual(VM* vm UNUSED, Value* args)
{
    Value boolValue = BOOL_TO_VALUE(valueIsEqual(args[0],args[1]));
    RET_VALUE(boolValue);
}

//args[0] != args[1]: 返回 object 是否不等
static bool primObjectNotEqual(VM* vm UNUSED, Value* args)
{
    Value boolValue = BOOL_TO_VALUE( !(valueIsEqual(args[0],args[1])) );
    RET_VALUE(boolValue);
}

//这个实现方式有点迷……
//args[0] is args[1]:类 args[0]是否为类 args[1]的子类
static bool primObjectIs(VM* vm, Value* args)
{
    //args[1]必须是 class 
    if (!VALUE_IS_CLASS(args[1]))
    {
        RUN_ERROR("argm=ument must be class!");
    }
    Class* thisClass = getClassOfObj(vm, args[0]); 
    Class* baseClass = (Class*)(args[1].objHeader);

    //
    while (baseClass != NULL)
    {
        //在某一级基类找到匹配，就设置返回值为 VT_TRUE 并返回
        if (thisClass == baseClass)
        {
            RET_TRUE;
        }
        baseClass = baseClass->superClass;
    }
    //若未找到基类,说明不具备 is_a 关系
    RET_FALSE;
}

//args[0].tostring: 返回 args[0]所属 class 的名字
static bool primObjectToString(VM* vm UNUSED, Value* args)
{
    Class* class = args[0].objHeader->class;
    Value nameValue = OBJ_TO_VALUE(class->name);
    RET_VALUE(nameValue);
}

//args[0].type:返回对象 args[0]的类
static bool primObjectType(VM* vm, Value* args)
{
    Class* class = getClassOfObj(vm, args[0]);
    RET_OBJ(class);
}

//args[0].name: 返回类名
static bool primClassName(VM* vm UNUSED, Value* args)
{
    RET_OBJ(VALUE_TO_CLASS(args[0])-> name);
}

//args[0].supertype: 返回 args[0]的基类
static bool primClassSupertype(VM* vm UNUSED, Value* args)
{
    Class* class = VALUE_TO_CLASS(args[0]);
    //如果基类不为NULL,则返回
    if (class->superClass != NULL)
    {
        RET_OBJ(class->superClass);
    }
    RET_VALUE(VT_TO_VALUE(VT_NULL));
}

//args[0].toString: 返回类名
static bool primClassToString(VM* vm UNUSED, Value* args)
{
    RET_OBJ(VALUE_TO_CLASS(args[0])->name);
}

//args[0].same(args[1], args[2]): 返回 args[1]和 args[2]是否相等
static bool primObjectmetaSame(VM* vm UNUSED, Value* args)
{
    Value boolValue = BOOL_TO_VALUE(valueIsEqual(args[1], args[2]));
    RET_VALUE(boolValue);
}

//从 modules 中获取名为 moduleName 的模块
static ObjModule* getModule(VM* vm, Value moduleName)
{
    Value value = mapGet(vm->allModules, moduleName);
    if (value.type == VT_UNDEFINED)
    {
        return NULL;
    }
    return VALUE_TO_OBJMODULE(value);
}


//载入模块 moduleName 并编译
static ObjThread* loadModule(VM* vm, Value moduleName, const char* moduleCode)
{
    //先查看是否已经导入了该模块,避免重新导入
    ObjModule* module = getModule(vm, moduleName);
    //若该模块名=NULL,则说明未加载,先将其载入,并继承核心模块中的变量
    if (module == NULL)
    {
        //创建模块并添加到 vm->allModules
        ObjString* modName = VALUE_TO_OBJSTR(moduleName);
        ASSERT(modName->value.start[modName->value.length]=='\0', "string.value.start is not terminated!");

        module = newObjModule(vm, modName->value.start);
        mapSet(vm, vm->allModules, moduleName, OBJ_TO_VALUE(module));

        //继承核心模块中的变量
        ObjModule* coreModule = getModule(vm, CORE_MODULE);
        uint32_t idx = 0;
        while (idx < coreModule->moduleVarName.count)
        {
            defineModuleVar(vm, module, 
                coreModule->moduleVarName.datas[idx].str, 
                coreModule->moduleVarName.datas[idx].length, 
                coreModule->moduleVarValue.datas[idx]);
            idx++;
        }
    }
    ObjFn* fn = compileModule(vm, module, moduleCode);
    ObjClosure* objClosure = newObjClosure(vm, fn);
    ObjThread* moduleThread = newObjThread(vm, objClosure);

    return moduleThread;
}

//table 中查找符号 symbol，找到后返回索引，否则返回-1 
int getIndexFromSymbolTable(SymbolTable* table, const char *symbol, uint32_t length)
{
    ASSERT(length!=0, "length of symbol is 0!");
    uint32_t index = 0;
    while (index < table->count)
    {
        //先判断符号长度是否相等，然后再调用memcmp函数比较字符串
        if(length == table->datas[index].length && 
            memcmp(table->datas[index].str, symbol, length) == 0)
        {
            return index;
        }
        index++;
    }
    return -1;
}

//往 table 中添加符号 symbol,返回其索引
int addSymbol(VM* vm, SymbolTable* table, const char* symbol, uint32_t length)
{
    ASSERT(length!=0, "length of symbol is 0!");
    String string;
    string.str = ALLOCATE_ARRAY(vm, char, length+1);
    memcpy(string.str, symbol, length);
    string.str[length]='\0';
    string.length = length;
    StringBufferAdd(vm, table, string);
    return table->count -1 ;
}

//定义类
static Class* defineClass(VM* vm, ObjModule* objModule, const char* name)
{
    //1．先创建类
    Class* class = newRawClass(vm, name, 0);
    //2．把类作为普通变量在模块中定义
    defineModuleVar(vm, objModule, name, strlen(name), OBJ_TO_VALUE(class));
    return class;
}
//使 class->methods[index]=method 
void bindMethod(VM* vm, Class* class, uint32_t index, Method method)
{
    //判断是否需要填充及扩容
    //就算有剩余空间（index <= capaity），但是没有使用的部分需要填0,所以也需要一并判断
    if (index >= class->methods.count)
    {
        //
        Method emptyPad = {MT_NONE, {0}};
        MethodBufferFillWrite(vm, &(class->methods), emptyPad, 
                            index - (class->methods.count)+1);
    }
    class->methods.datas[index] = method;
}

//
void bindSuperClass(VM* vm, Class* subClass, Class* superClass)
{
    subClass->superClass = superClass;
    //继承基类属性数
    subClass->fieldNum += superClass->fieldNum;
    //绑定基类方法
    uint32_t idx = 0;
    while (idx < superClass->methods.count)
    {
        bindMethod(vm, subClass, idx, superClass->methods.datas[idx]);
        idx++;
    }
}

//代码桩superClass->fieldNum
VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode)
{
    ObjThread* objThread = loadModule(vm, moduleName, moduleCode);
    return VM_RESULT_ERROR;
}

//编译核心模块
void buildCore(VM* vm)
{
    ObjModule* coreModule = newObjModule(vm, NULL);
    //把创建的核心模块,录入到 vm->allModules
    mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));

    //创建 object 类并绑定方法
    vm->objectClass = defineClass(vm, coreModule, "object");
    PRIM_METHOD_BIND(vm->objectClass, "!", primObjectNot);
    PRIM_METHOD_BIND(vm->objectClass, "==(_)", primObjectEqual);
    PRIM_METHOD_BIND(vm->objectClass, "!=(_)", primObjectNotEqual);
    PRIM_METHOD_BIND(vm->objectClass, "is(_)", primObjectIs);
    PRIM_METHOD_BIND(vm->objectClass, "toString", primObjectToString);
    PRIM_METHOD_BIND(vm->objectClass, "type",primObjectType);

    //定义 classOfClass 类,它是所有 meta 类的 meta 类和基类
    vm->classOfClass = defineClass(vm, coreModule, "class");

    //objectClass 是任何类的基类
    bindSuperClass(vm, vm->objectClass, vm->classOfClass);

    PRIM_METHOD_BIND(vm->classOfClass, "name", primClassName);
    PRIM_METHOD_BIND(vm->classOfClass, "toString",primClassToString);
    PRIM_METHOD_BIND(vm->classOfClass, "supertype", primClassSupertype);
    
    //定义 object 类的元信息类 objectMetaclass,它无须挂载到 vm
    Class* objectMeta = defineClass(vm, coreModule, "objectMeta");

    //classOfClass 类是所有 meta 类的 meta 类和基类
    bindSuperClass(vm, objectMeta, vm->classOfClass);

    //类型比较
    PRIM_METHOD_BIND(objectMeta, "same(_,_)",primObjectmetaSame);

    //绑定各自的 meta 类
    vm->classOfClass->objHeader.class = vm->classOfClass;
    vm->objectClass->objHeader.class = objectMeta;
    objectMeta->objHeader.class = vm->classOfClass;
}