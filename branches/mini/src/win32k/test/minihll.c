#ifdef __IBMC__
#pragma strings(readonly)
#endif

//#define LIBC

#ifndef LIBC

#define INCL_DOSMISC
#include <os2.h>
/*unsigned long _System
DosPutMessage(
    unsigned long hFile,
    unsigned long cchMsg,
    const char *  pszMsg);
*/
#else
#include <stdio.h>
#endif

#ifdef __IBMC__
#pragma entry(entry)
#endif



/*
 * Main entry potin etc.
 */
void entry(void)
{
    DosPutMessage(0, 18, "I'm really small!\r");
}
