#ifndef __THUNK_H__
#define __THUNK_H__

typedef struct
{
    char    magic[4];
    DWORD   length;
    DWORD   ptr;
    DWORD   x0C;

    DWORD   x10;
    DWORD   x14;
    DWORD   x18;
    DWORD   x1C;
    DWORD   x20;
} thunkstruct;

#endif