/* $Id: minihll.c,v 1.1.2.5 2001-08-20 18:47:54 bird Exp $
 *
 * Minimal 'High Level Language' executable.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 * Build this in a WATCOM Odin32 environment using wmake.
 * wcc386, and wlink is required.
 *      SET DEBUG=
 *      mkdir bin
 *      mkdir bin\release.wac36
 *
 *  For 276 bytes result which uses LIBCN:
 *      wmake -f minihll.mak -u -a LIBC=1

 *  For 286 bytes result which uses MSG.DosPutMessage:
 *      wmake -f minihll.mak -u -a
 *
 * Any attempt to compile in a VACxx environment will produce a exefile with a
 * bad stack. Causing SYS0189.
 */

/*
 * Tell the IBM C compiler where to put strings.
 */
#ifdef __IBMC__
#pragma strings(writeable)
#endif


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
#ifndef LIBC
unsigned long _System DosPutMessage(unsigned long hFile,
                                    unsigned long cchMsg,
                                    char *  pszMsg);
#else
#ifdef __IBMC__
extern int _Optlink _printf_ansi(register char * pszMsg);
#else
//Watcom
extern int _printf_ansi(register char * pszMsg);
#endif
#endif

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * This is now (due to object ordering) placed in minihll2.c.
 */
extern char szMsg[19];


/*
 * The IBM compiler wanna know where to start.
 */
#ifdef __IBMC__
#pragma entry(minihll)
#endif


/*
 * Main entry potin etc.
 */
#ifndef LIBC
void  _Optlink minihll(void)
#else
void  minihll(register char *psz)
#endif
{
    #ifndef LIBC
    DosPutMessage(0, 18, szMsg);
    #else
    #ifdef __IBMC__
    _printf_ansi(psz = szMsg);
    #else
    _printf_ansi(szMsg);
    #endif
    #endif
}

