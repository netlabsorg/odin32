/* $Id: shlstub.cpp,v 1.5 2005-06-19 19:00:41 sao2l02 Exp $ */

/*
 * Win32 stub shlwapi APIs for OS/2
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#define HAVE_WCTYPE_H
#include <odin.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "winnls.h"
#include "winversion.h"
#include "winreg.h"

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>


ODINDEBUGCHANNEL(SHLWAPI-SHLSTUB)


/*****************************************************************************
 * Name      : StrFromTimeIntervalA
 * Purpose   : 
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

LPSTR WIN32API StrFromTimeIntervalA(LPVOID lp1, LPVOID lp2, LPVOID lp3, LPVOID lp4)
{
  dprintf(("not yet implemented"));
  
  return NULL;
}


/*****************************************************************************
 * Name      : StrFromTimeIntervalW
 * Purpose   : 
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : 
 * Status    : UNTESTED
 *
 * Author    :
 *****************************************************************************/

LPWSTR WIN32API StrFromTimeIntervalW(LPVOID lp1, LPVOID lp2, LPVOID lp3, LPVOID lp4)
{
  dprintf(("not yet implemented"));
  
  return NULL;
}
