/* $Id: unknown.cpp,v 1.1 1999-07-07 12:18:04 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 * Copyright 1998, 1999 Patrick Haller (phaller@gmx.net)
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2wrap.h>
#include <misc.h>


/*****************************************************************************
 * Prototypes & Types & Defines                                              *
 *****************************************************************************/

int _System rexec(char **, int, char *, char *, char *, int *);
int _System rcmd (char **, int, const char *, const char *, const char *, int *);


/*****************************************************************************
 * Name      : int WIN32API OS2rexec
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : WSOCK32.1102
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

int WIN32API OS2rexec(LPSTR *lppArg1,
                      int   iArg2,
                      LPSTR lpArg3,
                      LPSTR lpArg4,
                      LPSTR lpArg5,
                      int   *lpiArg6)
{
  dprintf(("WSOCK32: OS2rexec(%08xh,%08xh,%s,%s,%s,%08xh) not implemented correctly.\n",
           lppArg1,
           iArg2,
           lpArg3,
           lpArg4,
           lpArg5,
           lpiArg6));

  return rexec(lppArg1,
               iArg2,
               lpArg3,
               lpArg4,
               lpArg5,
               lpiArg6);
}


/*****************************************************************************
 * Name      : int WIN32API OS2rcmd
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : WSOCK32.1103
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

int WIN32API OS2rcmd(LPSTR      *lppArg1,
                     int        iArg2,
                     const char *lpArg3,
                     const char *lpArg4,
                     const char *lpArg5,
                     int        *lpiArg6)
{
  dprintf(("WSOCK32: OS2rcmd(%08xh,%08xh,%s,%s,%s,%08xh) not implemented.\n",
           lppArg1,
           iArg2,
           lpArg3,
           lpArg4,
           lpArg5,
           lpiArg6));

  //@@@PH actually it exists in TCP/IP 5.0 headers. Unknown which OS/2 function to link to.
  return 0;
/*
  return rcmd(lppArg1,
              iArg2,
              lpArg3,
              lpArg4,
              lpArg5,
              lpiArg6);
*/
}

