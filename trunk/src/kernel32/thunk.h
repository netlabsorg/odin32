/* $Id: thunk.h,v 1.1 1999-05-24 20:19:49 ktk Exp $ */

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