#include "core.h"
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "vm.h"

char rootDir = NULL;
char* readFile(const char* path)
{
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        IO_ERROR("couldn`t open file in %s .\n",path);
    }
}