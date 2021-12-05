#ifndef _INCLUDE_CORE_H
#define _INCLUDE_CORE_H
#include"vm.h"
extern char *rootDir;
char* readFile(const char* sourceFile);
int getIndexFromSymbolTable(SymbolTable* table, const char *symbol, uint32_t length);
int addSymbol(VM* vm, SymbolTable* table, const char* symbol, uint32_t length);
void bindMethod(VM* vm, Class* class, uint32_t index, Method method);
void bindSuperClass(VM* vm, Class* subClass, Class* superClass);
VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode);
void buildCore(VM* vm);
#endif