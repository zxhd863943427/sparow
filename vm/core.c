#include "core.h"
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "vm.h"
#include "class.h"
#include "compiler.h"

char *rootDir = NULL;                       //根目录

#define CORE_MODULE VT_TO_VALUE(VT_NULL)

//栈指令，将返回值放置于args[0]的位置
//返回值类型是 Value 类型,且是放在 args[0], args 是 Value 数组
//RET_VALUE 的参数就是 Value 类型,无须转换直接赋值
//它是后面"RET_其他类型 "的基础
#define RET_VALUE(value)\
    do\
    {\
        arg[0] = value;\
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
\
}\

void fnPRIM_METHOD_BIND(Class* classPtr,char* methodName,ObjFn* func, VM* vm) \
{\
    uint32_t length = strlen(methodName);\
    int globalIdx =getIndexFromSymbolTable(&(vm->allMethodNames), methodName, length);\
    if (globalIdx == -1)\
    {\
        globalIdx = addSymbol(vm, &(vm->allMethodNames), methodName, length);\
    }
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

//代码桩
VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode)
{
    return VM_RESULT_ERROR;
}

//编译核心模块
void buildCore(VM* vm)
{
    ObjModule* objModule = newObjModule(vm, NULL);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(objModule));
}