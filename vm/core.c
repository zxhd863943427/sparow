#include "core.h"
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "vm.h"
#include "class.h"

char *rootDir = NULL;
#define CORE_MODULE VT_TO_VALUE(VT_NULL)

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