/* $Id: stricmp.c,v 1.1 1999-09-06 02:20:02 bird Exp $
 *
 * stricmp - Case insensitive string compare.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define upcase(ch)   \
     (ch >= 'a' && ch <= 'z' ? ch - ('a' - 'A') : ch)


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <string.h>


/**
 * strcmpi - case insensitive string compare. Not i subsys library.
 * @param   psz1   String 1
 * @parma   psz2   String 2
 * @return  0 if equal
 *          != 0 if not equal
 */
int stricmp(const char *psz1, const char *psz2)
{
    int iRet;

    while ((iRet = upcase(*psz1) - upcase(*psz2)) == 0 && *psz1 != '\0')
        psz1++, psz2++;

    return iRet;
}

