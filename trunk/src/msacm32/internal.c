/* -*- tab-width: 8; c-basic-offset: 4 -*- */

/*
 *      MSACM32 library
 *
 *      Copyright 1998  Patrik Stridvall
 *		  1999	Eric Pouech
 */

#include <string.h>

#include "winbase.h"
#include "windef.h"
#include "wingdi.h"
#include "winuser.h"
#include "winerror.h"
#include "mmsystem.h"
#include "msacm.h"
#include "msacmdrv.h"
#include "wineacm.h"
#include "debugtools.h"

#ifdef __WIN32OS2__
#include <heapstring.h>
#endif

DEFAULT_DEBUG_CHANNEL(msacm);

/**********************************************************************/

HANDLE MSACM_hHeap = (HANDLE) NULL;
PWINE_ACMDRIVERID MSACM_pFirstACMDriverID = NULL;
PWINE_ACMDRIVERID MSACM_pLastACMDriverID = NULL;

/***********************************************************************
 *           MSACM_RegisterDriver() 
 */
PWINE_ACMDRIVERID MSACM_RegisterDriver(LPSTR pszDriverAlias, LPSTR pszFileName,
				       HINSTANCE hinstModule)
{ 
    PWINE_ACMDRIVERID padid;

    TRACE("('%s', '%s', 0x%08x)\n", pszDriverAlias, pszFileName, hinstModule);

    padid = (PWINE_ACMDRIVERID) HeapAlloc(MSACM_hHeap, 0, sizeof(WINE_ACMDRIVERID));
    padid->obj.dwType = WINE_ACMOBJ_DRIVERID;
    padid->obj.pACMDriverID = padid;
    padid->pszDriverAlias = NULL;
    if (pszDriverAlias)
    {
        padid->pszDriverAlias = HeapAlloc( MSACM_hHeap, 0, strlen(pszDriverAlias)+1 );
        strcpy( padid->pszDriverAlias, pszDriverAlias );
    }
    padid->pszFileName = NULL;
    if (pszFileName)
    {
        padid->pszFileName = HeapAlloc( MSACM_hHeap, 0, strlen(pszFileName)+1 );
        strcpy( padid->pszFileName, pszFileName );
    }
    padid->hInstModule = hinstModule;
    padid->bEnabled = TRUE;
    padid->pACMDriverList = NULL;
    padid->pNextACMDriverID = NULL;
    padid->pPrevACMDriverID = MSACM_pLastACMDriverID;
    if (MSACM_pLastACMDriverID)
	MSACM_pLastACMDriverID->pNextACMDriverID = padid;
    MSACM_pLastACMDriverID = padid;
    if (!MSACM_pFirstACMDriverID)
	MSACM_pFirstACMDriverID = padid;
    
    return padid;
}

/***********************************************************************
 *           MSACM_RegisterAllDrivers() 
 */
void MSACM_RegisterAllDrivers(void)
{
    LPSTR pszBuffer;
    DWORD dwBufferLength;

    /* FIXME 
     *  What if the user edits system.ini while the program is running?
     *  Does Windows handle that?
     */
    if (MSACM_pFirstACMDriverID)
	return;
    
    /* FIXME: Do not work! How do I determine the section length? */
    dwBufferLength = 1024;
/* EPP 	GetPrivateProfileSectionA("drivers32", NULL, 0, "system.ini"); */
    
    pszBuffer = (LPSTR) HeapAlloc(MSACM_hHeap, 0, dwBufferLength);
    if (GetPrivateProfileSectionA("drivers32", pszBuffer, dwBufferLength, "system.ini")) {
	char* s = pszBuffer;
	while (*s) {
	    if (!strncasecmp("MSACM.", s, 6)) {
		char *s2 = s;
		while (*s2 != '\0' && *s2 != '=') s2++;
		if (*s2) {
		    *s2 = '\0';
		    MSACM_RegisterDriver(s, s2 + 1, 0);
		    *s2 = '=';
		}
	    }  
	    s += strlen(s) + 1; /* Either next char or \0 */
	}
    }
    
    HeapFree(MSACM_hHeap, 0, pszBuffer);

    MSACM_RegisterDriver("msacm32.dll", "msacm32.dll", 0);
}

/***********************************************************************
 *           MSACM_UnregisterDriver()
 */
PWINE_ACMDRIVERID MSACM_UnregisterDriver(PWINE_ACMDRIVERID p)
{
    PWINE_ACMDRIVERID pNextACMDriverID;
    
    while (p->pACMDriverList)
	acmDriverClose((HACMDRIVER) p->pACMDriverList, 0);
    
    if (p->pszDriverAlias)
	HeapFree(MSACM_hHeap, 0, p->pszDriverAlias);
    if (p->pszFileName)
	HeapFree(MSACM_hHeap, 0, p->pszFileName);
    
    if (p == MSACM_pFirstACMDriverID)
	MSACM_pFirstACMDriverID = p->pNextACMDriverID;
    if (p == MSACM_pLastACMDriverID)
	MSACM_pLastACMDriverID = p->pPrevACMDriverID;

    if (p->pPrevACMDriverID)
	p->pPrevACMDriverID->pNextACMDriverID = p->pNextACMDriverID;
    if (p->pNextACMDriverID)
	p->pNextACMDriverID->pPrevACMDriverID = p->pPrevACMDriverID;
    
    pNextACMDriverID = p->pNextACMDriverID;
    
    HeapFree(MSACM_hHeap, 0, p);
    
    return pNextACMDriverID;
}

/***********************************************************************
 *           MSACM_UnregisterAllDrivers()
 * FIXME
 *   Where should this function be called?
 */
void MSACM_UnregisterAllDrivers(void)
{
    PWINE_ACMDRIVERID p;

    for (p = MSACM_pFirstACMDriverID; p; p = MSACM_UnregisterDriver(p));
}

/***********************************************************************
 *           MSACM_GetObj()
 */
PWINE_ACMOBJ MSACM_GetObj(HACMOBJ hObj, DWORD type)
{
    PWINE_ACMOBJ	pao = (PWINE_ACMOBJ)hObj;

    if (pao == NULL || IsBadReadPtr(pao, sizeof(WINE_ACMOBJ)) ||
	((type != WINE_ACMOBJ_DONTCARE) && (type != pao->dwType)))
	return NULL;
    return pao;
}

/***********************************************************************
 *           MSACM_GetDriverID() 
 */
PWINE_ACMDRIVERID MSACM_GetDriverID(HACMDRIVERID hDriverID)
{
    return (PWINE_ACMDRIVERID)MSACM_GetObj((HACMOBJ)hDriverID, WINE_ACMOBJ_DRIVERID);
}

/***********************************************************************
 *           MSACM_GetDriver()
 */
PWINE_ACMDRIVER MSACM_GetDriver(HACMDRIVER hDriver)
{
    return (PWINE_ACMDRIVER)MSACM_GetObj((HACMOBJ)hDriver, WINE_ACMOBJ_DRIVER);
}

/***********************************************************************
 *           MSACM_Message()
 */
MMRESULT MSACM_Message(HACMDRIVER had, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
    PWINE_ACMDRIVER	pad = MSACM_GetDriver(had);

    return pad ? SendDriverMessage(pad->hDrvr, uMsg, lParam1, lParam2) : MMSYSERR_INVALHANDLE;
}
