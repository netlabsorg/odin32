/* $Id: security_odin.cpp,v 1.1 2002-02-21 22:52:01 sandervl Exp $ */
/*
 * Win32 security API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen (OS/2 Port)
 *
 * Based on Wine code (dlls\advapi32\security.c)
 *
 * Copyright original Wine author(s) (????)
 *
 * dlls/advapi32/security.c
 *  FIXME: for all functions thunking down to Rtl* functions:  implement SetLastError()
 */

#include <string.h>

#ifdef __WIN32OS2__
#include <os2win.h>
#include <heapstring.h>
#endif

#include "windef.h"
#include "winerror.h"
#include "heap.h"
#include "ntddk.h"
#include "ntsecapi.h"
#include "debugtools.h"

DECLARE_DEBUG_CHANNEL(advapi-security)

static BOOL fInitSecurity = FALSE;
static BOOL fHasSecurity = FALSE;

BOOL Wine_HasSecurity(void)
{
    //SvL: Let's not check this for every single call please...
    if(!fInitSecurity) 
    {
    	OSVERSIONINFOA osi;
    	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA); 
    	GetVersionExA(&osi);
    	if (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) { 
		fHasSecurity = TRUE;
	}
        fInitSecurity = TRUE;
    }
    if(!fHasSecurity) {
       	SetLastError(ERROR_CALL_NOT_IMPLEMENTED); 
        return FALSE;
    }
    return TRUE;
}  


#define CallWin32ToNt(func) \
	{ NTSTATUS ret; \
	  ret = (func); \
	  if (ret !=STATUS_SUCCESS) \
	  { SetLastError (RtlNtStatusToDosError(ret)); return FALSE; } \
	  return TRUE; \
	}

/*	##############################
	######	TOKEN FUNCTIONS ######
	##############################
*/



/*****************************************************************************
 * Name      : AddAce
 * Purpose   : The AddAce function adds one or more ACEs to a specified ACL.
 *             An ACE is an access-control entry. An ACL is an access-control list.
 * Parameters: PACL   pAcl               address of access-control list
 *             DWORD  dwAceRevision      ACL revision level
 *             DWORD  dwStartingAceIndex index of ACE position in ACL
 *             LPVOID pAceList           address of one or more ACEs
 *             DWORD  nAceListLength     size of buffer for ACEs
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WIN32API AddAce(PACL   pAcl,
                        DWORD  dwAceRevision,
                        DWORD  dwStartingAceIndex,
                        LPVOID pAceList,
                        DWORD  nAceListLength)
{
  CallWin32ToNt (RtlAddAce(pAcl, dwAceRevision, dwStartingAceIndex, (PACE_HEADER)pAceList, nAceListLength));
}


