/* $Id: stricmp.c,v 1.3 1999-11-10 01:45:37 bird Exp $
 *
 * stricmp - Case insensitive string compare.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
#pragma info(nogen, noext, nouni)

/**
 * stricmp - case insensitive string compare.
 * Not i subsys library.
 * @param   psz1   String 1
 * @param   psz2   String 2
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



/**
 * strnicmp - case insensitive string compare for up to cch chars of the strings.
 * Not i subsys library.
 * @param   psz1   String 1
 * @param   psz2   String 2
 * @return  0 if equal
 *          != 0 if not equal
 */
int strnicmp(const char *psz1, const char *psz2, size_t cch)
{
    int iRet;

    while (cch > 0 && (iRet = upcase(*psz1) - upcase(*psz2)) == 0 && *psz1 != '\0')
        psz1++, psz2++, cch--;

    return iRet;
}

