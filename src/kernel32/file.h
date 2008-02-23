/* $Id: file.h,v 1.1 2005-06-26 18:14:04 sao2l02 Exp $ */
/*
 * File handling declarations
 *
 * Copyright 1996 Alexandre Julliard
 */

#ifndef __WINE_FILE_H
#define __WINE_FILE_H

#include <time.h> /* time_t */
#include "winbase.h"

#define MAX_PATHNAME_LEN   1024

/* Definition of a full DOS file name */
typedef struct
{
    char  long_name[MAX_PATHNAME_LEN];  /* Long pathname in Unix format */
    char  short_name[MAX_PATHNAME_LEN]; /* Short pathname in DOS 8.3 format */
    int   drive;
} DOS_FULL_NAME;

#define IS_END_OF_NAME(ch)  (!(ch) || ((ch) == '/') || ((ch) == '\\'))
#define INVALID_DOS_CHARS  "*?<>|\"+=,;[] \345"

/* DOS device descriptor */
typedef struct
{
    char *name;
    int flags;
} DOS_DEVICE;

#endif  /* __WINE_FILE_H */
