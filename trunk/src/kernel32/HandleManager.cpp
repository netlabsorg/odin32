/* $Id: HandleManager.cpp,v 1.3 1999-06-17 18:21:35 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Unified Handle Manager for OS/2
 *
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) HandleManager.Cpp       1.0.0   1998/02/11 PH start
 */

#undef DEBUG_LOCAL
//#define DEBUG_LOCAL


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 1998/02/11 PH Even overlapped I/O could be simulated by another subsystem
               thread with a request queue. We'll see if required ...


               Flush (flush handle buffer)
               WaitForSingleObject
               WaitForMultipleObjects (?)

 1998/02/12 PH IBM and Microsoft disagree about the definition of FILE_TYPE_xxx
               Interesting, Open32 returns Microsoft's values ...

 1998/02/12 PH Handles should be equipped with a locking mechanism, in particular
               as we publish a pointer into the handle table via HMHandleQueryHandleData

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "unicode.h"
#include "misc.h"

#include "HandleManager.H"
#include "HMDevice.h"
#include "HMOpen32.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                    /* this is the size of our currently static handle table */
#define MAX_OS2_HMHANDLES 256

        /* this is for the handle translation table, could by dynamic though */
#define MAX_TRANSLATION_HANDLES 8192



/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


typedef struct _HMHANDLE
{
  HMDeviceHandler *pDeviceHandler;         /* handler for this pseudo-device */
  HMHANDLEDATA    hmHandleData;                  /* attributes of the handle */
} HMHANDLE, *PHMHANDLE;


typedef struct _HMDEVICE
{
  struct _HMDEVICE *pNext;                /* pointer to next device in chain */

  PSZ             pszDeviceName;       /* name or alias of the pseudo-device */
  HMDeviceHandler *pDeviceHandler;         /* handler for this pseudo-device */
} HMDEVICE, *PHMDEVICE;


typedef struct _HMTRANSHANDLE
{
/* @@@PH
   UCHAR ucSubsystemID; to determine "lost" handles
 */
  /* the index position in the array is the 16bit windows part of the handle */
  ULONG hHandle32;                         /* 32-bit OS/2 part of the handle */
} HMTRANSHANDLE, *PHMTRANSHANDLE;


/*****************************************************************************
 * This pseudo-device logs all device requests to the logfile and returns    *
 * ERROR_INVALID_FUNCTION to virtually all requests -> debugging             *
 *****************************************************************************/
class HMDeviceDebugClass : public HMDeviceHandler
{
  public:
    HMDeviceDebugClass(LPCSTR lpDeviceName) : HMDeviceHandler(lpDeviceName) {}
};


/*****************************************************************************
 * Process Global Structures                                                 *
 *****************************************************************************/


            /* the device name is repeated here to enable device alias names */
static PHMDEVICE TabWin32Devices = NULL;
static HMHANDLE TabWin32Handles[MAX_OS2_HMHANDLES];   /* static handle table */


struct _HMGlobals
{
  HANDLE hStandardIn;                              /* stdin handle to CONIN$ */
  HANDLE hStandardOut;                           /* stdout handle to CONOUT$ */
  HANDLE hStandardError;                         /* stderr handle to CONOUT$ */

  BOOL   fIsInitialized;                   /* if HM is initialized already ? */
                                         /* this MUST !!! be false initially */

  HMDeviceHandler *pHMOpen32;             /* default handle manager instance */


  ULONG         ulHandleLast;                   /* index of last used handle */
  HMTRANSHANDLE TabTranslationHandles[MAX_TRANSLATION_HANDLES];
} HMGlobals;


/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/

                        /* get appropriate device handler by the device name */
static HMDeviceHandler *_HMDeviceFind(PSZ pszDeviceName);

                               /* get next free handle from the handle table */
static int              _HMHandleGetFree(void);

                                       /* get handle table entry from handle */
static int              _HMHandleQuery(HANDLE hHandle);



/*****************************************************************************
 * Name      : static HMDeviceHandler * _HMDeviceFind
 * Purpose   : obtain appropriate device handler from the table by searching
 *             for a device name or alias
 * Parameters: PSZ pszDeviceName
 * Variables :
 * Result    : HMDeviceHandler * - pointer to the handler object
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:42]
 *****************************************************************************/

static HMDeviceHandler *_HMDeviceFind (PSZ pszDeviceName)
{
  PHMDEVICE pHMDevice;                     /* iterator over the device table */

  if (pszDeviceName != NULL)
    for (pHMDevice = TabWin32Devices;  /* loop over all devices in the table */
         pHMDevice != NULL;
         pHMDevice = pHMDevice->pNext)
    {
      if (stricmp(pHMDevice->pszDeviceName,       /* case-insensitive search */
                  pszDeviceName) == 0)
        return (pHMDevice->pDeviceHandler);    /* OK, we've found our device */
    }

  return (HMGlobals.pHMOpen32);    /* haven't found anything, return default */
}


/*****************************************************************************
 * Name      : static int _HMHandleGetFree
 * Purpose   : get index to first free handle in the handle table
 * Parameters:
 * Variables :
 * Result    : int iIndex - index to the table or -1 in case of error
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:43]
 *****************************************************************************/

static int _HMHandleGetFree(void)
{
  int iLoop;

  for (iLoop = 0;
       iLoop < MAX_OS2_HMHANDLES;
       iLoop++)
  {
                                                      /* free handle found ? */
    if (0 == TabWin32Handles[iLoop].hmHandleData.hHMHandle)
      return (iLoop);                    /* OK, then return it to the caller */
  }

  return (-1);                              /* haven't found any free handle */
}


/*****************************************************************************
 * Name      : static int _HMHandleQuery
 * Purpose   : gets the index of handle table entry as fast as possible from
 *             the specified handle
 * Parameters: HANDLE hHandle
 * Variables :
 * Result    : index or -1 in case of error
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

static int _HMHandleQuery(HANDLE hHandle)
{
  ULONG ulIndex;                              /* index into the handle table */

                                                         /* check the handle */
  ulIndex = hHandle & ~HM_HANDLE_MASK;        /* mask out the signature bits */
  if (ulIndex != HM_HANDLE_ID)                       /* one of our handles ? */
    return (-1);                               /* nope, ERROR_INVALID_HANDLE */

  ulIndex = hHandle & HM_HANDLE_MASK;               /* get the relevant bits */
  if (ulIndex > MAX_OS2_HMHANDLES)                  /* check the table range */
    return (-1);                               /* nope, ERROR_INVALID_HANDLE */

                                                   /* Oops, invalid handle ! */
  if (TabWin32Handles[ulIndex].hmHandleData.hHMHandle != hHandle)
    return (-1);                               /* nope, ERROR_INVALID_HANDLE */

  return ( (int) ulIndex);                 /* OK, we've got our handle index */
}


/*****************************************************************************
 * Name      : DWORD  HMDeviceRegister
 * Purpose   : register a device with the handle manager
 * Parameters: PSZ             pszDeviceName
 *             HMDeviceHandler *pDeviceHandler
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/12 20:44]
 *****************************************************************************/

DWORD   HMDeviceRegister(PSZ             pszDeviceName,
                                 HMDeviceHandler *pDeviceHandler)
{
  PHMDEVICE pHMDevice;                     /* our new device to be allocated */

  if  ( (pszDeviceName  == NULL) ||                      /* check parameters */
        (pDeviceHandler == NULL) )
    return (ERROR_INVALID_PARAMETER);                /* raise error conditon */


  pHMDevice = (PHMDEVICE) malloc (sizeof (HMDEVICE) );    /* allocate memory */
  if (pHMDevice == NULL)                          /* check proper allocation */
    return (ERROR_NOT_ENOUGH_MEMORY);                        /* signal error */

  pHMDevice->pszDeviceName = strdup(pszDeviceName);             /* copy name */
  if (pHMDevice->pszDeviceName == NULL)           /* check proper allocation */
  {
    free (pHMDevice);                    /* free previously allocated memory */
    return (ERROR_NOT_ENOUGH_MEMORY);                        /* signal error */
  }

  pHMDevice->pDeviceHandler = pDeviceHandler;     /* store pointer to device */
  pHMDevice->pNext = TabWin32Devices;                   /* establish linkage */

  TabWin32Devices = pHMDevice;        /* insert new node as root in the list */

  return (NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD HMInitialize
 * Purpose   : Initialize the handlemanager
 * Parameters: -
 * Variables : -
 * Result    : always NO_ERROR
 * Remark    : this routine just stores the standard handles in the
 *             internal table within the HandleManager
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/12 20:44]
 *****************************************************************************/

DWORD HMInitialize(void)
{
  if (HMGlobals.fIsInitialized != TRUE)
  {
    HMGlobals.fIsInitialized = TRUE;                             /* OK, done */

    dprintf(("KERNEL32:HandleManager:HMInitialize() storing handles.\n"));

    memset(&HMGlobals,                       /* zero out the structure first */
           0,
           sizeof(HMGlobals));

                       /* copy standard handles from OS/2's Open32 Subsystem */
    HMSetStdHandle(STD_INPUT_HANDLE,  GetStdHandle(STD_INPUT_HANDLE));
    HMSetStdHandle(STD_OUTPUT_HANDLE, GetStdHandle(STD_OUTPUT_HANDLE));
    HMSetStdHandle(STD_ERROR_HANDLE,  GetStdHandle(STD_ERROR_HANDLE));

                        /* create handle manager instance for Open32 handles */
    HMGlobals.pHMOpen32 = new HMDeviceOpen32Class("\\\\.\\");
  }
  return (NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD  HMTerminate
 * Purpose   : Terminate the handlemanager
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/12 20:44]
 *****************************************************************************/

DWORD HMTerminate(void)
{
  /* @@@PH we could deallocate the device list here */

  return (NO_ERROR);
}


/*****************************************************************************/
/* handle translation buffer management                                      */
/*                                                                           */
/* Since some Win32 applications rely (!) on 16-bit handles, we've got to do */
/* 32-bit to 16-bit and vs vsa translation here.                             */
/* Filehandle-based functions should be routed via the handlemanager instead */
/* of going to Open32 directly.                                              */
/*****************************************************************************/


/*****************************************************************************
 * Name      : DWORD HMHandleAllocate
 * Purpose   : allocate a handle in the translation table
 * Parameters: PULONG pHandle16 - to return the allocated handle
 *             ULONG  hHandle32 - the associated OS/2 handle
 * Variables :
 * Result    : API returncode
 * Remark    : no parameter checking is done, phHandle may not be invalid
 *             hHandle32 shouldn't be 0
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleAllocate (PULONG phHandle16,
                                 ULONG  hHandle32)
{
  register ULONG ulHandle;

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleAllocate (%08xh,%08xh)\n",
           phHandle16,
           hHandle32));
#endif

  ulHandle = HMGlobals.ulHandleLast;                      /* get free handle */

  do
  {
                                                  /* check if handle is free */
    if (HMGlobals.TabTranslationHandles[ulHandle].hHandle32 == 0)
    {
      *phHandle16 = ulHandle;
      HMGlobals.TabTranslationHandles[ulHandle].hHandle32 = hHandle32;
      HMGlobals.ulHandleLast = ulHandle;          /* to shorten search times */

      return (NO_ERROR);                                               /* OK */
    }

    ulHandle++;                                        /* skip to next entry */

    if (ulHandle >= MAX_TRANSLATION_HANDLES)               /* check boundary */
      ulHandle = 0;
  }
  while (ulHandle != HMGlobals.ulHandleLast);

  return (ERROR_TOO_MANY_OPEN_FILES);                      /* OK, we're done */
}


/*****************************************************************************
 * Name      : DWORD HMHandleFree
 * Purpose   : free a handle from the translation table
 * Parameters: ULONG hHandle16 - the handle to be freed
 * Variables :
 * Result    : API returncode
 * Remark    : no parameter checking is done, hHandle16 MAY NEVER exceed
 *             the MAX_TRANSLATION_HANDLES boundary
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleFree (ULONG hHandle16)
{
  ULONG rc;                                                /* API returncode */

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleFree (%08xh)\n",
           hHandle16));
#endif

  rc = HMHandleValidate(hHandle16);                         /* verify handle */
  if (rc != NO_ERROR)                                        /* check errors */
    return (rc);                                    /* raise error condition */

  HMGlobals.TabTranslationHandles[hHandle16].hHandle32 = 0;      /* OK, done */

  return (NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD HMHandleValidate
 * Purpose   : validate a handle through the translation table
 * Parameters: ULONG hHandle16 - the handle to be verified
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleValidate (ULONG hHandle16)
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleValidate (%08xh)\n",
           hHandle16));
#endif

  if (hHandle16 >= MAX_TRANSLATION_HANDLES)                /* check boundary */
    return (ERROR_INVALID_HANDLE);                  /* raise error condition */

  if (HMGlobals.TabTranslationHandles[hHandle16].hHandle32 == 0)  /* valid ? */
    return (ERROR_INVALID_HANDLE);                  /* raise error condition */

  return (NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD HMHandleTranslateToWin
 * Purpose   : translate a 32-bit OS/2 handle to the associated windows handle
 * Parameters: ULONG  hHandle32  - the OS/2 handle
 *             PULONG phHandle16 - the associated windows handle
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleTranslateToWin (ULONG  hHandle32,
                                       PULONG phHandle16)
{
           ULONG rc;                                       /* API returncode */
  register ULONG ulIndex;                    /* index counter over the table */

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleTranslateToWin (%08xh, %08xh)\n",
           hHandle32,
           phHandle16));
#endif

  for (ulIndex = 0;
       ulIndex < MAX_TRANSLATION_HANDLES;
       ulIndex++)
  {
                                                      /* look for the handle */
    if (HMGlobals.TabTranslationHandles[ulIndex].hHandle32 == hHandle32)
    {
      *phHandle16 = ulIndex;                               /* deliver result */
      return (NO_ERROR);                                               /* OK */
    }
  }

  return (ERROR_INVALID_HANDLE);                    /* raise error condition */
}


/*****************************************************************************
 * Name      : DWORD HMHandleTranslateToOS2
 * Purpose   : translate a 16-bit Win32 handle to the associated OS/2 handle
 * Parameters: ULONG  hHandle16  - the windows handle
 *             PULONG phHandle32 - the associated OS/2 handle
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleTranslateToOS2 (ULONG  hHandle16,
                                       PULONG phHandle32)
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleTranslateToOS2 (%08xh, %08xh)\n",
           hHandle16,
           phHandle32));
#endif

  if (HMHandleValidate(hHandle16) == NO_ERROR)              /* verify handle */
  {
    *phHandle32 = HMGlobals.TabTranslationHandles[hHandle16].hHandle32;
    return (NO_ERROR);
  }

  return (ERROR_INVALID_HANDLE);                    /* raise error condition */
}


/*****************************************************************************
 * Name      : DWORD HMHandleTranslateToOS2i
 * Purpose   : translate a 16-bit Win32 handle to the associated OS/2 handle
 * Parameters: ULONG  hHandle16  - the windows handle
 * Variables :
 * Result    : OS/2 handle
 * Remark    : no checkinf
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleTranslateToOS2i (ULONG  hHandle16)
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleTranslateToOS2i (%08xh)\n",
           hHandle16));
#endif

  return(HMGlobals.TabTranslationHandles[hHandle16].hHandle32);
}



/*****************************************************************************
 * Name      : HANDLE  _HMGetStdHandle
 * Purpose   : replacement for Open32's GetStdHandle function
 * Parameters: DWORD nStdHandle
 * Variables :
 * Result    : HANDLE to standard device
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/12 20:44]
 *****************************************************************************/

HANDLE  HMGetStdHandle(DWORD nStdHandle)
{
  switch (nStdHandle)
  {
    case STD_INPUT_HANDLE:  return (HMGlobals.hStandardIn);
    case STD_OUTPUT_HANDLE: return (HMGlobals.hStandardOut);
    case STD_ERROR_HANDLE:  return (HMGlobals.hStandardError);

    default:
    {
      SetLastError(ERROR_INVALID_PARAMETER);        /* set error information */
      return (INVALID_HANDLE_VALUE);                /* raise error condition */
    }
  }
}


/*****************************************************************************
 * Name      : HANDLE  _HMSetStdHandle
 * Purpose   : replacement for Open32's SetStdHandle function
 * Parameters: DWORD  nStdHandle
 *             HANDLE hHandle
 * Variables :
 * Result    : BOOL fSuccess
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/12 20:44]
 *****************************************************************************/

BOOL    HMSetStdHandle(DWORD  nStdHandle,
                               HANDLE hHandle)
{
  switch (nStdHandle)
  {
    case STD_INPUT_HANDLE:  HMGlobals.hStandardIn    = hHandle; return TRUE;
    case STD_OUTPUT_HANDLE: HMGlobals.hStandardOut   = hHandle; return TRUE;
    case STD_ERROR_HANDLE:  HMGlobals.hStandardError = hHandle; return TRUE;

    default:
    {
      SetLastError(ERROR_INVALID_PARAMETER);        /* set error information */
      return (FALSE);                               /* raise error condition */
    }
  }
}


/*****************************************************************************
 * Name      : HANDLE  HMCreateFile
 * Purpose   : Wrapper for the CreateFile() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Fix parameters passed to the HMDeviceManager::CreateFile
 *             Supply access mode and share mode validation routines
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HFILE HMCreateFile(LPCSTR lpFileName,
                   DWORD  dwDesiredAccess,
                   DWORD  dwShareMode,
                   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                   DWORD  dwCreationDisposition,
                   DWORD  dwFlagsAndAttributes,
                   HANDLE hTemplateFile)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  HANDLE          hResult;
  DWORD           rc;                                     /* API return code */
  PHMHANDLEDATA   pHMHandleData;
  HMHANDLEDATA    HMHandleTemp;          /* temporary buffer for handle data */

  /* create new handle by either lpFileName or hTemplateFile */
  if (lpFileName == NULL)           /* this indicates creation from template */
  {
    iIndex = _HMHandleQuery(hTemplateFile);      /* query table for template */
    if (-1 == iIndex)                        /* this device is unknown to us */
    {
      SetLastError (ERROR_INVALID_HANDLE);
      return INVALID_HANDLE_VALUE;
    }
    else
    {
                                                       /* to pass to handler */
      pHMHandleData = &TabWin32Handles[iIndex].hmHandleData;
      pDeviceHandler = TabWin32Handles[iIndex].pDeviceHandler;
    }
  }
  else
  {
    pDeviceHandler = _HMDeviceFind((PSZ)lpFileName);          /* find device */

    if (NULL == pDeviceHandler)                /* this name is unknown to us */
    {
      SetLastError(ERROR_FILE_NOT_FOUND);
      return (INVALID_HANDLE_VALUE);                         /* signal error */
    }
    else
      pHMHandleData  = NULL;
  }


  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  if (lpFileName == NULL)                            /* create from template */
    memcpy (&TabWin32Handles[iIndexNew].hmHandleData,
            &TabWin32Handles[iIndex].hmHandleData,
            sizeof(HMHANDLEDATA));
  else
  {
    HMHandleTemp.dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
    HMHandleTemp.dwAccess   = dwDesiredAccess;
    HMHandleTemp.dwShare    = dwShareMode;
    HMHandleTemp.dwCreation = dwCreationDisposition;
    HMHandleTemp.dwFlags    = dwFlagsAndAttributes;
    HMHandleTemp.lpHandlerData = NULL;
  }

      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  hResult = (ULONG)iIndexNew | HM_HANDLE_ID;
  HMHandleTemp.hHMHandle                      = hResult;
  TabWin32Handles[iIndexNew].pDeviceHandler = pDeviceHandler;

                                  /* now copy back our temporary handle data */
  memcpy(&TabWin32Handles[iIndexNew].hmHandleData,
         &HMHandleTemp,
         sizeof(HMHANDLEDATA));

  rc = pDeviceHandler->CreateFile(lpFileName,     /* call the device handler */
                                  &HMHandleTemp,
                                  lpSecurityAttributes,
                                  pHMHandleData);

#ifdef DEBUG_LOCAL
    dprintf(("KERNEL32/HandleManager:CheckPoint2: %s lpHandlerData=%08xh\n",
             lpFileName,
             HMHandleTemp.lpHandlerData));
#endif

  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }
  else
  {
             /* copy data fields that might have been modified by CreateFile */
    memcpy(&TabWin32Handles[iIndexNew].hmHandleData,
           &HMHandleTemp,
           sizeof(HMHANDLEDATA));
  }


#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32/HandleManager: CreateFile(%s)=%08xh\n",
           lpFileName,
           hResult));
#endif

  return hResult;                                     /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMOpenFile
 * Purpose   : Wrapper for the OpenFile() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Fix parameters passed to the HMDeviceManager::OpenFile
 *             Supply access mode and share mode validation routines
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/


/***********************************************************************
 *              FILE_ConvertOFMode
 *
 * Convert OF_* mode into flags for CreateFile.
 */
static void FILE_ConvertOFMode( INT mode, DWORD *access, DWORD *sharing )
{
    switch(mode & 0x03)
    {
      case OF_READ:      *access = GENERIC_READ; break;
      case OF_WRITE:     *access = GENERIC_WRITE; break;
      case OF_READWRITE: *access = GENERIC_READ | GENERIC_WRITE; break;
      default:           *access = 0; break;
    }
    switch(mode & 0x70)
    {
      case OF_SHARE_EXCLUSIVE:  *sharing = 0; break;
      case OF_SHARE_DENY_WRITE: *sharing = FILE_SHARE_READ; break;
      case OF_SHARE_DENY_READ:  *sharing = FILE_SHARE_WRITE; break;
       case OF_SHARE_DENY_NONE:
      case OF_SHARE_COMPAT:
      default:                  *sharing = FILE_SHARE_READ | FILE_SHARE_WRITE; break;
    }
}

HANDLE HMOpenFile(LPCSTR    lpFileName,
                  OFSTRUCT* pOFStruct,
                  UINT      fuMode)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  HANDLE          hResult;
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  pDeviceHandler = _HMDeviceFind((PSZ)lpFileName);            /* find device */
  if (NULL == pDeviceHandler)                  /* this name is unknown to us */
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }
  else
    pHMHandleData  = NULL;


  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */

  FILE_ConvertOFMode(fuMode,                                 /* map OF_flags */
                     &pHMHandleData->dwAccess,
                     &pHMHandleData->dwShare);

  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  hResult = (ULONG)iIndexNew | HM_HANDLE_ID;
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = hResult;
  TabWin32Handles[iIndexNew].pDeviceHandler       = pDeviceHandler;

  rc = pDeviceHandler->OpenFile  (lpFileName,     /* call the device handler */
                                  &TabWin32Handles[iIndexNew].hmHandleData,
                                  pOFStruct,
                                  fuMode);

#ifdef DEBUG_LOCAL
    dprintf(("KERNEL32/HandleManager:CheckPoint3: %s lpHandlerData=%08xh\n",
             lpFileName,
             HMHandleTemp.lpHandlerData));
#endif

  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32/HandleManager: OpenFile(%s)=%08xh\n",
           lpFileName,
           hResult));
#endif

  return hResult;                                     /* return valid handle */
}



/*****************************************************************************
 * Name      : HANDLE  HMCloseFile
 * Purpose   : Wrapper for the CloseHandle() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMCloseHandle(HANDLE hObject)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      fResult;       /* result from the device handler's CloseHandle() */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hObject);                         /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (FALSE);                                        /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  fResult = pHMHandle->pDeviceHandler->CloseHandle(&pHMHandle->hmHandleData);

  if (fResult == TRUE)                   /* remove handle if close succeeded */
    pHMHandle->hmHandleData.hHMHandle = 0;            /* mark handle as free */

  return (fResult);                                   /* deliver return code */
}


/*****************************************************************************
 * Name      : HANDLE  HMReadFile
 * Purpose   : Wrapper for the ReadHandle() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
BOOL HMReadFile(HANDLE       hFile,
                LPVOID       lpBuffer,
                DWORD        nNumberOfBytesToRead,
                LPDWORD      lpNumberOfBytesRead,
                LPOVERLAPPED lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      fResult;       /* result from the device handler's CloseHandle() */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (FALSE);                                        /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  fResult = pHMHandle->pDeviceHandler->ReadFile(&pHMHandle->hmHandleData,
                                                lpBuffer,
                                                nNumberOfBytesToRead,
                                                lpNumberOfBytesRead,
                                                lpOverlapped);

  return (fResult);                                   /* deliver return code */
}


/*****************************************************************************
 * Name      : HANDLE  HMWriteFile
 * Purpose   : Wrapper for the WriteHandle() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

BOOL HMWriteFile(HANDLE       hFile,
                        LPCVOID      lpBuffer,
                        DWORD        nNumberOfBytesToWrite,
                        LPDWORD      lpNumberOfBytesWritten,
                        LPOVERLAPPED lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      fResult;       /* result from the device handler's CloseHandle() */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (FALSE);                                        /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  fResult = pHMHandle->pDeviceHandler->WriteFile(&pHMHandle->hmHandleData,
                                                 lpBuffer,
                                                 nNumberOfBytesToWrite,
                                                 lpNumberOfBytesWritten,
                                                 lpOverlapped);

  return (fResult);                                   /* deliver return code */
}


/*****************************************************************************
 * Name      : HANDLE  HMGetFileType
 * Purpose   : Wrapper for the GetFileType() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/12 13:37]
 *****************************************************************************/

DWORD HMGetFileType(HANDLE hFile)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->GetFileType(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::_DeviceReuqest
 * Purpose   : entry method for special request functions
 * Parameters: ULONG ulRequestCode
 *             various parameters as required
 * Variables :
 * Result    :
 * Remark    : the standard behaviour is to return an error code for non-
 *             existant request codes
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/
DWORD   HMDeviceRequest (HANDLE hFile,
                         ULONG  ulRequestCode,
                         ULONG  arg1,
                         ULONG  arg2,
                         ULONG  arg3,
                         ULONG  arg4)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->_DeviceRequest(&pHMHandle->hmHandleData,
                                                       ulRequestCode,
                                                       arg1,
                                                       arg2,
                                                       arg3,
                                                       arg4);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::GetFileInformationByHandle
 * Purpose   : router function for GetFileInformationByHandle
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
DWORD HMGetFileInformationByHandle (HANDLE                     hFile,
                                    BY_HANDLE_FILE_INFORMATION *pHFI)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->GetFileInformationByHandle(&pHMHandle->hmHandleData,
                                                       pHFI);

  return (dwResult);                                  /* deliver return code */
}



/*****************************************************************************
 * Name      : HMDeviceHandler::SetEndOfFile
 * Purpose   : router function for SetEndOfFile
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
BOOL HMSetEndOfFile (HANDLE hFile)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                 /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetEndOfFile(&pHMHandle->hmHandleData);

  return (bResult);                                   /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::SetFileTime
 * Purpose   : router function for SetFileTime
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
BOOL HMSetFileTime (HANDLE         hFile,
                    const FILETIME *pFT1,
                    const FILETIME *pFT2,
                    const FILETIME *pFT3)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                 /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetFileTime(&pHMHandle->hmHandleData,
                                                   (LPFILETIME)pFT1,
                                                   (LPFILETIME)pFT2,
                                                   (LPFILETIME)pFT3);

  return (bResult);                                   /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::GetFileSize
 * Purpose   : router function for GetFileSize
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
DWORD HMGetFileSize (HANDLE hFile,
                     PDWORD pSize)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->GetFileSize(&pHMHandle->hmHandleData,
                                                    pSize);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::SetFilePointer
 * Purpose   : router function for SetFilePointer
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
DWORD HMSetFilePointer (HANDLE hFile,
                        LONG   lDistanceToMove,
                        PLONG  lpDistanceToMoveHigh,
                        DWORD  dwMoveMethod)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->SetFilePointer(&pHMHandle->hmHandleData,
                                                       lDistanceToMove,
                                                       lpDistanceToMoveHigh,
                                                       dwMoveMethod);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::LockFile
 * Purpose   : router function for LockFile
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
BOOL HMLockFile (HFILE         hFile,
                 DWORD         arg2,
                 DWORD         arg3,
                 DWORD         arg4,
                 DWORD         arg5)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->LockFile(&pHMHandle->hmHandleData,
                                                 arg2,
                                                 arg3,
                                                 arg4,
                                                 arg5);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::LockFileEx
 * Purpose   : router function for LockFileEx
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
DWORD HMLockFileEx(HANDLE        hFile,
                   DWORD         dwFlags,
                   DWORD         dwReserved,
                   DWORD         nNumberOfBytesToLockLow,
                   DWORD         nNumberOfBytesToLockHigh,
                   LPOVERLAPPED  lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->LockFileEx(&pHMHandle->hmHandleData,
                                                   dwFlags,
                                                   dwReserved,
                                                   nNumberOfBytesToLockLow,
                                                   nNumberOfBytesToLockHigh,
                                                   lpOverlapped);

  return (dwResult);                                  /* deliver return code */
}



/*****************************************************************************
 * Name      : HMDeviceHandler::UnlockFile
 * Purpose   : router function for UnlockFile
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
BOOL HMUnlockFile (HFILE         hFile,
                   DWORD         arg2,
                   DWORD         arg3,
                   DWORD         arg4,
                   DWORD         arg5)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->UnlockFile(&pHMHandle->hmHandleData,
                                                   arg2,
                                                   arg3,
                                                   arg4,
                                                   arg5);

  return (dwResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMDeviceHandler::UnlockFileEx
 * Purpose   : router function for UnlockFileEx
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/
BOOL HMUnlockFileEx(HANDLE        hFile,
                    DWORD         dwFlags,
                    DWORD         dwReserved,
                    DWORD         nNumberOfBytesToLockLow,
                    DWORD         nNumberOfBytesToLockHigh,
                    LPOVERLAPPED  lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (INVALID_HANDLE_ERROR);                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->UnlockFileEx(&pHMHandle->hmHandleData,
                                                     dwFlags,
                                                     dwReserved,
                                                     nNumberOfBytesToLockLow,
                                                     nNumberOfBytesToLockHigh,
                                                     lpOverlapped);

  return (dwResult);                                  /* deliver return code */
}

