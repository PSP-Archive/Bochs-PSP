#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include <pspiofilemgr.h>

extern "C" {
    int pspstat( const char *path, struct stat *buffer);
}

int pspstat( const char *path, struct stat *buffer)
{
    SceIoStat stat;
    int retVal;

    retVal = sceIoGetstat(path, &stat);

    if( retVal < 0 )
    {
        return retVal;
    }

    buffer->st_size = stat.st_size;
    buffer->st_mode = stat.st_mode;

    return 0;
}


extern "C" {
    int _stat( const char *path, struct stat *buffer);
}

int _stat( const char *path, struct stat *buffer)
{
    return pspstat(path, buffer);
}
