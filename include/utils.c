#include "utils.h"
#include "vm.h"
#include "parser.h"
#include <stdlib.h>
#include <stdarg.h>

//这里是一些通用函数，比如内存的管理、缓冲区的清理、报错函数

void * memManager(VM* vm,void* ptr,uint32_t oldSize,uint32_t newSize){
    vm->allocatedBytes +=newSize-oldSize;

    if(newSize==0)
    {
        free(ptr);
        return NULL;
    }

    return realloc(ptr,newSize);
}

uint32_t ceilToPowerOf2(uint32_t v)
{
    //v=0时，将v修复为1,这样就能避免溢出
    v += (v==0);
    v--;
    //以下步骤实际上是将v的首位后填一处理
    v|=v>>1;
    v|=v>>2;
    v|=v>>4;
    v|=v>>8;
    v|=v>>16;
    v++;
    return v;
}

DEFINE_BUFFER_METHOD(String)
DEFINE_BUFFER_METHOD(Int)
DEFINE_BUFFER_METHOD(Char)
DEFINE_BUFFER_METHOD(Byte)

void symbolTableClear(VM* vm,SymbolTable* buffer)
{
    uint32_t idx = 0;
    while(idx<buffer->count)
    {
        memManager(vm,buffer->datas[idx++].str,0,0);
    }
    StringBufferClear(vm,buffer);
}

//通用报错函数
void errorReport(void* parser,ErrorType errorType,const char* fmt,...)
{
    char buffer[DEFAULT_BUFFER_SIZE]={'\0'};
    va_list ap;
    va_start(ap,fmt);
    vsnprintf(buffer,DEFAULT_BUFFER_SIZE,fmt,ap);
    va_end(ap);
    switch (errorType)
    {
    case ERROR_IO:
    case ERROR_MEM:
        fprintf(stderr,"%s:%d in function %s():%s\n",
                __FILE__, __LINE__, __func__, buffer);
        break;
    case ERROR_LEX:
    case ERROR_COMPILE:
        ASSERT(parser !=NULL,"parser is null!");
        fprintf(stderr,"%s:%d \"%s \"\n",
            ((Parser*)parser)->file,((Parser*)parser)->preToken.lineNo, buffer);
        break;
    case ERROR_RUNTIME:
        fprintf(stderr,"%s \n",buffer);
        break;    
    default:
        NOT_REACHED();
    }
    exit(1);
}