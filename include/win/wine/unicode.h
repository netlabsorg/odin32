/* $Id: unicode.h,v 1.1 2000-08-24 09:29:45 sandervl Exp $ */
/*
 * Wine internal Unicode definitions
 *
 * Copyright 2000 Alexandre Julliard
 */

#ifndef __WINE_UNICODE_H
#define __WINE_UNICODE_H

#include "windef.h"
#include <heapstring.h>

static WCHAR *strchrW( const WCHAR *str, WCHAR ch )
{
    for ( ; *str; str++) if (*str == ch) return (WCHAR *)str;
    return NULL;
}
 
static WCHAR *strrchrW( const WCHAR *str, WCHAR ch )
{
    WCHAR *ret = NULL;
    for ( ; *str; str++) if (*str == ch) ret = (WCHAR *)str;
    return ret;
}               


extern WCHAR *strstrW( const WCHAR *str, const WCHAR *sub );

#endif  /* __WINE_UNICODE_H */
