/* $Id: unknown.cpp,v 1.1 1999-12-07 20:19:57 achimha Exp $ */

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
#include <odinwrap.h>


ODINDEBUGCHANNEL(WSOCK32-UNKNOWN)

/*****************************************************************************
 * Prototypes & Types & Defines                                              *
 *****************************************************************************/

int _System rexec(char **, int, char *, char *, char *, int *);
int _System rcmd (char **, int, const char *, const char *, const char *, int *);


/*****************************************************************************
 *
 * Purpose,  :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : WSOCK32.1102
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

ODINFUNCTION6(int, OS2rexec, LPSTR*, lppArg1,
                             int,    iArg2,
                             LPSTR,  lpArg3,
                             LPSTR,  lpArg4,
                             LPSTR,  lpArg5,
                             int*,   lpiArg6)
{
  dprintf(("WSOCK32: rexec not implemented correctly.\n"));

  return rexec(lppArg1,
               iArg2,
               lpArg3,
               lpArg4,
               lpArg5,
               lpiArg6);
}


/*****************************************************************************
 * ODINFUNCTION6( :, int, WIN32API, OS2rcmd,
 * Purpose, :,
 * Parameters:,
 * Variables, :,
 * Result    :
 * Remark    : WSOCK32.1103
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

ODINFUNCTION6(int, OS2rcmd, LPSTR*,     lppArg1,
                            int,        iArg2,
                            const char*,lpArg3,
                            const char*,lpArg4,
                            const char*,lpArg5,
                            int       *,lpiArg6)
{
  dprintf(("WSOCK32: rcmd not implemented.\n"));

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


