/* -*- tab-width: 8; c-basic-offset: 4 -*- */

/***********************************************************************
 * Wine specific - Win32
 */
typedef struct _WINE_ACMDRIVERID *PWINE_ACMDRIVERID;

typedef struct _WINE_ACMOBJ
{
  PWINE_ACMDRIVERID	pACMDriverID;
} WINE_ACMOBJ, *PWINE_ACMOBJ;

typedef struct _WINE_ACMDRIVER
{
    WINE_ACMOBJ		obj;
    HDRVR      		hDrvr;
    DRIVERPROC		pfnDriverProc;
} WINE_ACMDRIVER, *PWINE_ACMDRIVER;

typedef struct _WINE_ACMSTREAM
{
    WINE_ACMOBJ		obj;
    PWINE_ACMDRIVER	pDrv;
    ACMDRVSTREAMINSTANCE drvInst;
} WINE_ACMSTREAM, *PWINE_ACMSTREAM;

typedef struct _WINE_ACMDRIVERID
{
    LPSTR               pszDriverAlias;
    LPSTR               pszFileName;
    HINSTANCE		hInstModule;          /* NULL if global */
    DWORD		dwProcessID;	      /* ID of process which installed a local driver */
    BOOL                bEnabled;
    PWINE_ACMDRIVER     pACMDriver;           /* NULL if not open; shouldn't this be a list ? */
    PWINE_ACMDRIVERID   pNextACMDriverID;
    PWINE_ACMDRIVERID	pPreviousACMDriverID;
} WINE_ACMDRIVERID;

/* From internal.c */
extern HANDLE MSACM_hHeap;
extern PWINE_ACMDRIVERID MSACM_pFirstACMDriverID;
extern PWINE_ACMDRIVERID MSACM_pLastACMDriverID;
PWINE_ACMDRIVERID MSACM_RegisterDriver(
  LPSTR pszDriverAlias, LPSTR pszFileName,
  HINSTANCE hinstModule);
void MSACM_RegisterAllDrivers(void);
PWINE_ACMDRIVERID MSACM_UnregisterDriver(PWINE_ACMDRIVERID p);
void MSACM_UnregisterAllDrivers(void);
PWINE_ACMDRIVERID MSACM_GetDriverID(HACMDRIVERID hDriverID);
PWINE_ACMDRIVER MSACM_GetDriver(HACMDRIVER hDriver);
PWINE_ACMOBJ MSACM_GetObj(HACMOBJ hObj);

