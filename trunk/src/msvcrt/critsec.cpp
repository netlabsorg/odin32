/* $Id: critsec.cpp,v 1.1 2000-08-22 08:09:20 phaller Exp $ */

/*
 * The Visual C RunTime DLL (MSVCRT/MSVCRT20/MSVCRT40)
 * 
 * Implements Visual C run-time functionality
 *
 * Copyright 2000 Patrick Haller
 */


#include <os2win.h>
#include <odinwrap.h>
#include <wchar.h>
#include <math.h>
#include <string.h>

#include <heapstring.h>
#include <crtdll.h>

#include <win/winbase.h>
#include <win/winnt.h>
#include "msvcrt.h"

ODINDEBUGCHANNEL(msvcrt-critsec)

/*****************************************************************************
 * local variables                                                           *
 *****************************************************************************/

#define CRITSEC_TABLE_LOCK 0x11
#define CRITSEC_MAX_ENTRIES 0x30
 
static CRITICAL_SECTION* arrpCriticalSections[CRITSEC_MAX_ENTRIES] = {0};
static CRITICAL_SECTION* pCriticalSection_01 = arrpCriticalSections[0x01];
static CRITICAL_SECTION* pCriticalSection_09 = arrpCriticalSections[0x09];
static CRITICAL_SECTION* pCriticalSection_0D = arrpCriticalSections[0x0D];
static CRITICAL_SECTION* pCriticalSection_11 = arrpCriticalSections[CRITSEC_TABLE_LOCK];


/*********************************************************************
 *                  INTERNAL startup_critsec_table
 */

#if NOT_YET_COMPLETE
void startup_critsec_table(void)
{
  CRITICAL_SECTION *pCS;
  
  for (int iIndex = 0;
       iIndex < CRITSEC_MAX_ENTRIES;
       iIndex++)
  {
    // delete all "non-critical" critical sections
    pCS = arrpCriticalSections[iIndex];
    if ( (pCS != pCriticalSection_01) &&
         (pCS != pCriticalSection_09) &&
         (pCS != pCriticalSection_0D) &&
         (pCS != pCriticalSection_11) )
    {
      DeleteCriticalSection(pCS);
      free(pCS);
    }
  }
  
  // delete important critical sections
  DeleteCriticalSection(pCriticalSection_01);
  DeleteCriticalSection(pCriticalSection_09);
  DeleteCriticalSection(pCriticalSection_0D);
  DeleteCriticalSection(pCriticalSection_11);
}

#endif

/*********************************************************************
 *                  _unlock    (MSVCRT.480)
 */
 
VOID CDECL MSVCRT__unlock(unsigned long ulIndex)
{
  dprintf(("MSVCRT: _unlock %08xh\n",
          ulIndex));
  
  CRITICAL_SECTION *pCS = arrpCriticalSections[ulIndex];
  LeaveCriticalSection(pCS);
}


/*********************************************************************
 *                  _lock    (MSVCRT.318)
 */
// Prototype from CRTDLL.CPP
VOID CDECL amsg_exit(int errnum);

// @@@PH Note: subsystem startup & termination
// is absent. That's a show stopper of course!
// See MSVCRT.start() code.

VOID CDECL MSVCRT__lock(unsigned long ulIndex)
{
  dprintf(("MSVCRT: _lock %08xh\n",
          ulIndex));
  
  if (arrpCriticalSections[ulIndex] == NULL)
  {
    CRITICAL_SECTION *pCSNew;
    
    pCSNew = (CRITICAL_SECTION*)malloc( sizeof(CRITICAL_SECTION) );
    if (pCSNew == NULL)
      amsg_exit(0x11); // yield error message
    
    MSVCRT__lock(CRITSEC_TABLE_LOCK); // lock table
     // has been modified meanwhile by other thread ?
    if (arrpCriticalSections[ulIndex] != NULL)
      free(pCSNew);
    else
    {
      InitializeCriticalSection(pCSNew);
      arrpCriticalSections[ulIndex] = pCSNew;
    }
      
    MSVCRT__unlock(CRITSEC_TABLE_LOCK); // unlock table
  }
    
  EnterCriticalSection(arrpCriticalSections[ulIndex]);
}
