
/*
 * Win32 KERNEL32 Subsystem for OS/2
 *
 * 2000/11/10 PH Patrick Haller (patrick.haller@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/
#include <os2win.h>
#include <winnls.h>
#include "unicode.h"
#include "handlemanager.h"
#include <string.h>

#include "stubs.h"

#define DBG_LOCALLOG    DBG_stubs
#include "dbglocal.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/



/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

DWORD WIN32API KERNEL32_99(DWORD x)
{
  dprintf(("KERNEL32: Unknown API KERNEL32.99\n"));
  return 1;
}

/**********************************************************************
 *		_KERNEL32_100
 */
BOOL WINAPI KERNEL32_100(HANDLE threadid,DWORD exitcode,DWORD x)
{
	dprintf(("_KERNEL32_100 (%d,%ld,0x%08lx): STUB",threadid,exitcode,x));
	return TRUE;
}


/*****************************************************************************
 * Name      : NullFunction
 * Purpose   : Empty function. Such exports actually exist. (.42 .47)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Wed, 2000/11/10 19:47]
 *****************************************************************************/

VOID WIN32API NullFunction(void)
{
  // it does actually NOTHING
}


/*****************************************************************************
 * Name      : KERNEL32_17
 * Purpose   : Some function dispatcher
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    : The function table has 16 32-bit function pointer entries
 *             The function is selected by the CL register.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Wed, 2000/11/10 19:47]
 *****************************************************************************/

VOID WIN32API KERNEL32_17(DWORD *pFunctionTable)
{
  // it's some function dispatcher
  SetLastError(0x78);
  
  //@@@PH we should now get the correct CL value */
  BYTE CL;
  int  ret;

  switch( (CL >> 4) & 0x0f)
  {
    case 0x0f: ret = -1; break;
    case 0x0e: ret = 0x78; break;
    case 0x0d: ret = 0x32; break;
    case 0x01: ret = 1; break;
    default: ret = 0; break;
  }
  
  // select jump function
  VOID* pfn = (VOID*)pFunctionTable[CL & 0x0F];
  //@@@PH we'd now jump ...
}


/*****************************************************************************
 * Name      : KERNEL32_16
 * Purpose   : Unknown
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    : 
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Wed, 2000/11/10 19:47]
 *****************************************************************************/

VOID WIN32API KERNEL32_16(DWORD arg1,
                          DWORD arg2,
                          DWORD arg3)
{
  dprintf(("KERNEL32: KERNEL32_16 (%08xh,%08xh,%08xh) not implemented\n",
           arg1,
           arg2,
           arg3));
}
