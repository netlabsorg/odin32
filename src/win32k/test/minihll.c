/* $Id: minihll.c,v 1.1.2.4 2001-08-16 15:57:01 bird Exp $
 *
 * Minimal 'High Level Language' executable.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
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

