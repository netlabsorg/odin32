/* $Id: unknown.cpp,v 1.4 1999-12-07 20:25:48 achimha Exp $ */

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

typedef int SOCKET;

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


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION4(int, OS2WSARecvEx, SOCKET,    s,
                                 char FAR*, buf,
                                 int,       len,
                                 int FAR *,flags)
{
  dprintf(("WSOCK32: WSARecvEx not implemented.\n"));

//    return WSARecvEx(s,buf,len,flags);
  return 0;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINPROCEDURE2(OS2s_perror, char*, pszMessage,
                            void*, pUnknown)
{
  dprintf(("WSOCK32: s_perror not implemented.\n"));

  //perror(pszMessage);
}


