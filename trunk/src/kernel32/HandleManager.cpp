/* $Id: HandleManager.cpp,v 1.71 2001-10-03 13:48:56 sandervl Exp $ */

/*
 * Win32 Unified Handle Manager for OS/2
 *
 * 1998/02/11 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) HandleManager.Cpp       1.0.0   1998/02/11 PH start
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

//#undef DEBUG_LOCAL
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
#include "HMDisk.h"
#include "HMOpen32.h"
#include "HMEvent.h"
#include "HMFile.h"
#include "HMMutex.h"
#include "HMSemaphore.h"
#include "HMMMap.h"
#include "HMComm.h"
#include "HMToken.h"
#include "HMThread.h"
#include "HMNPipe.h"
#include "HMStd.h"
#include "HMMailslot.h"

#include <vmutex.h>
#include <win\thread.h>

#define DBG_LOCALLOG  DBG_handlemanager
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

                    /* this is the size of our currently static handle table */
#define MAX_OS2_HMHANDLES 4096


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct _HMDEVICE;

typedef struct _HMHANDLE
{
  HMDeviceHandler *pDeviceHandler;         /* handler for this pseudo-device */
  HMHANDLEDATA    hmHandleData;                  /* attributes of the handle */
} HMHANDLE, *PHMHANDLE;


typedef struct _HMDEVICE
{
  struct _HMDEVICE *pNext;                /* pointer to next device in chain */

  LPSTR           pszDeviceName;       /* name or alias of the pseudo-device */
  HMDeviceHandler *pDeviceHandler;         /* handler for this pseudo-device */
  VOID            *pDevData;         /* Pointer To Device data */
} HMDEVICE, *PHMDEVICE;


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
VMutex          handleMutex;

struct _HMGlobals
{
  HANDLE hStandardIn;                              /* stdin handle to CONIN$ */
  HANDLE hStandardOut;                           /* stdout handle to CONOUT$ */
  HANDLE hStandardError;                         /* stderr handle to CONOUT$ */

  BOOL   fIsInitialized;                   /* if HM is initialized already ? */
                                         /* this MUST !!! be false initially */

  HMDeviceHandler        *pHMStandard;     /* default handle manager instance */
  HMDeviceHandler        *pHMOpen32;       /* default handle manager instance */
  HMDeviceHandler        *pHMEvent;        /* static instances of subsystems */
  HMDeviceHandler        *pHMFile;
  HMDeviceHandler        *pHMDisk;
  HMDeviceHandler        *pHMMutex;
  HMDeviceHandler        *pHMSemaphore;
  HMDeviceHandler        *pHMFileMapping;  /* static instances of subsystems */
  HMDeviceHandler        *pHMComm;                   /* serial communication */
  HMDeviceHandler        *pHMToken;         /* security tokens */
  HMDeviceHandler        *pHMThread;
  HMDeviceHandler        *pHMNamedPipe;
  HMDeviceHandler        *pHMMailslot;

  ULONG         ulHandleLast;                   /* index of last used handle */
} HMGlobals;


/*****************************************************************************
 * Local Prototypes                                                          *
 *****************************************************************************/

                        /* get appropriate device handler by the device name */
static HMDeviceHandler*  _Optlink _HMDeviceFind(LPSTR pszDeviceName);

                               /* get next free handle from the handle table */
static ULONG            _Optlink _HMHandleGetFree(void);

                                       /* get handle table entry from handle */
static ULONG            _Optlink _HMHandleQuery(HANDLE hHandle);

// Get GlobalDeviceData
static VOID *_HMDeviceGetData (LPSTR pszDeviceName);


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

static HMDeviceHandler *_HMDeviceFind (LPSTR pszDeviceName)
{
  PHMDEVICE pHMDevice;                     /* iterator over the device table */
  int namelength = strlen(pszDeviceName);

  if (pszDeviceName != NULL)
  {
    for (pHMDevice = TabWin32Devices;  /* loop over all devices in the table */
         pHMDevice != NULL;
         pHMDevice = pHMDevice->pNext)
    {
        if(pHMDevice->pDeviceHandler->FindDevice(pHMDevice->pszDeviceName, pszDeviceName, namelength) == TRUE)
        {
            return pHMDevice->pDeviceHandler;
        }
    }
  }
  return (HMGlobals.pHMOpen32);    /* haven't found anything, return default */
}
/*****************************************************************************
 * Name      : static VOID *_HMDeviceGetData
 * Purpose   : obtain pointer to device data from the table by searching
 *             for a device name or alias
 * Parameters: PSZ pszDeviceName
 * Variables :
 * Result    : VOID * - pointer to the handlers device data
 * Remark    :
 * Status    :
 *
 * Author    : Markus Montkowski
 *****************************************************************************/

static VOID *_HMDeviceGetData (LPSTR pszDeviceName)
{
  PHMDEVICE pHMDevice;                     /* iterator over the device table */
  int namelength = strlen(pszDeviceName);

  if (pszDeviceName != NULL)
  {
    for (pHMDevice = TabWin32Devices;  /* loop over all devices in the table */
         pHMDevice != NULL;
         pHMDevice = pHMDevice->pNext)
    {
        if(pHMDevice->pDeviceHandler->FindDevice(pHMDevice->pszDeviceName, pszDeviceName, namelength) == TRUE)
        {
            return (pHMDevice->pDevData);    /* OK, we've found our device */
        }
    }
  }
  return (NULL);    /* haven't found anything, return NULL */
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

static ULONG _HMHandleGetFree(void)
{
  register ULONG ulLoop;

  handleMutex.enter();

  for (ulLoop = 1; // @@@PH Note, this is an experimental change
                   // 0L as hHandle is sometimes considered invalid!
                   // this will never return 0l as free handle now.
       ulLoop < MAX_OS2_HMHANDLES;
       ulLoop++)
  {
                                                       /* free handle found ? */
    if (INVALID_HANDLE_VALUE == TabWin32Handles[ulLoop].hmHandleData.hHMHandle) {
        //SvL: Mark handle as allocated here. Doing it outside of this function
        //     isn't thread safe. (and not very smart)
        TabWin32Handles[ulLoop].hmHandleData.hHMHandle      = ulLoop; 
        TabWin32Handles[ulLoop].hmHandleData.dwUserData     = 0;
        TabWin32Handles[ulLoop].hmHandleData.dwInternalType = HMTYPE_UNKNOWN;
        TabWin32Handles[ulLoop].hmHandleData.lpDeviceData   = NULL;
        handleMutex.leave();
        return (ulLoop);                    /* OK, then return it to the caller */
    }
  }

  handleMutex.leave();
  return (INVALID_HANDLE_VALUE);             /* haven't found any free handle */
}


/*****************************************************************************
 * Name      : HMHandleGetUserData
 * Purpose   : Get the dwUserData dword for a specific handle
 * Parameters: HANDLE hHandle
 * Variables :
 * Result    : -1 or dwUserData
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMHandleGetUserData(ULONG  hHandle)
{
  if (hHandle > MAX_OS2_HMHANDLES)                  /* check the table range */
    return (-1);
                                                   /* Oops, invalid handle ! */
  if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
    return (-1);              /* nope, ERROR_INVALID_HANDLE */

  return TabWin32Handles[hHandle].hmHandleData.dwUserData;
}

/*****************************************************************************
 * Name      : HMHandleGetUserData
 * Purpose   : Get the dwUserData dword for a specific handle
 * Parameters: HANDLE hHandle
 * Variables :
 * Result    : -1 or dwUserData
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMHandleSetUserData(ULONG  hHandle, ULONG dwUserData)
{
  if (hHandle > MAX_OS2_HMHANDLES)                  /* check the table range */
    return (-1);
                                                   /* Oops, invalid handle ! */
  if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
    return (-1);              /* nope, ERROR_INVALID_HANDLE */

  TabWin32Handles[hHandle].hmHandleData.dwUserData = dwUserData;
  return NO_ERROR;
}

/*****************************************************************************
 * Name      : static int _HMHandleQuery
 * Purpose   : gets the index of handle table entry as fast as possible from
 *             the specified handle
 * Parameters: HANDLE hHandle
 * Variables :
 * Result    : index or -1 in case of error
 * Remark    : Should fail for standard handles (in/out/err)!!!!!!!!!!
 *             HMGetFileType depends on this!!!
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

static ULONG _HMHandleQuery(HANDLE hHandle)
{
  if (hHandle > MAX_OS2_HMHANDLES)                  /* check the table range */
    return (INVALID_HANDLE_VALUE);             /* nope, ERROR_INVALID_HANDLE */

                                                   /* Oops, invalid handle ! */
  if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
    return (INVALID_HANDLE_VALUE);              /* nope, ERROR_INVALID_HANDLE */

  return ( hHandle);                       /* OK, we've got our handle index */
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

DWORD   HMDeviceRegisterEx(LPSTR           pszDeviceName,
                           HMDeviceHandler *pDeviceHandler,
                           VOID            *pDevData)
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
  pHMDevice->pDevData = pDevData;

  TabWin32Devices = pHMDevice;        /* insert new node as root in the list */

  return (NO_ERROR);
}

DWORD   HMDeviceRegister(LPSTR           pszDeviceName,
                         HMDeviceHandler *pDeviceHandler)
{
  return HMDeviceRegisterEx(pszDeviceName, pDeviceHandler, NULL);
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
  ULONG ulIndex;

  if (HMGlobals.fIsInitialized != TRUE)
  {
    handleMutex.enter();
    // fill handle table
    for(ulIndex = 0; ulIndex < MAX_OS2_HMHANDLES; ulIndex++) {
        TabWin32Handles[ulIndex].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    }
    handleMutex.leave();

    dprintf(("KERNEL32:HandleManager:HMInitialize() storing handles.\n"));

    memset(&HMGlobals,                       /* zero out the structure first */
           0,
           sizeof(HMGlobals));

    HMGlobals.fIsInitialized = TRUE;                             /* OK, done */

    /* copy standard handles from OS/2's Open32 Subsystem */
    HMGlobals.pHMStandard   = new HMDeviceStandardClass("\\\\STANDARD_HANDLE\\");
    HMSetStdHandle(STD_INPUT_HANDLE,  O32_GetStdHandle(STD_INPUT_HANDLE));
    HMSetStdHandle(STD_OUTPUT_HANDLE, O32_GetStdHandle(STD_OUTPUT_HANDLE));
    HMSetStdHandle(STD_ERROR_HANDLE,  O32_GetStdHandle(STD_ERROR_HANDLE));

    /* create handle manager instance for Open32 handles */
    HMGlobals.pHMOpen32     = new HMDeviceOpen32Class("\\\\.\\");
    HMGlobals.pHMEvent      = new HMDeviceEventClass("\\\\EVENT\\");
    HMGlobals.pHMFile       = new HMDeviceFileClass("\\\\FILE\\");
    HMGlobals.pHMDisk       = new HMDeviceDiskClass("\\\\DISK\\");
    HMGlobals.pHMMutex      = new HMDeviceMutexClass("\\\\MUTEX\\");
    HMGlobals.pHMSemaphore  = new HMDeviceSemaphoreClass("\\\\SEM\\");
    HMGlobals.pHMFileMapping= new HMDeviceMemMapClass("\\\\MEMMAP\\");
    HMGlobals.pHMComm       = new HMDeviceCommClass("\\\\COM\\");
    HMGlobals.pHMToken      = new HMDeviceTokenClass("\\\\TOKEN\\");
    HMGlobals.pHMThread     = new HMDeviceThreadClass("\\\\THREAD\\");
    HMGlobals.pHMNamedPipe  = new HMDeviceNamedPipeClass("\\\\PIPE\\");
    HMGlobals.pHMMailslot   = new HMMailslotClass("\\MAILSLOT\\");
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

  if(HMGlobals.pHMOpen32)
    delete HMGlobals.pHMOpen32;
  if(HMGlobals.pHMEvent)
    delete HMGlobals.pHMEvent;
  if(HMGlobals.pHMFile)
    delete HMGlobals.pHMFile;
  if(HMGlobals.pHMMutex)
    delete HMGlobals.pHMMutex;
  if(HMGlobals.pHMSemaphore)
    delete HMGlobals.pHMSemaphore;
  if(HMGlobals.pHMFileMapping)
    delete HMGlobals.pHMFileMapping;
  if(HMGlobals.pHMComm)
    delete HMGlobals.pHMComm;
  if(HMGlobals.pHMToken)
    delete HMGlobals.pHMToken;
  if(HMGlobals.pHMThread)
    delete HMGlobals.pHMThread;
  if(HMGlobals.pHMNamedPipe)
    delete HMGlobals.pHMNamedPipe;
  if(HMGlobals.pHMMailslot)
    delete HMGlobals.pHMMailslot;
  if(HMGlobals.pHMDisk)
    delete HMGlobals.pHMDisk;
  if(HMGlobals.pHMStandard);
    delete HMGlobals.pHMStandard;

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
 *             Should be protected with a HM-Mutex !
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD HMHandleAllocate (PULONG phHandle16,
                        ULONG  hHandleOS2)
{
  register ULONG ulHandle;

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleAllocate (%08xh,%08xh)\n",
           phHandle16,
           hHandleOS2));
#endif

  ulHandle = HMGlobals.ulHandleLast;                      /* get free handle */

  handleMutex.enter();

  if(ulHandle == 0) {
        ulHandle = 1; //SvL: Start searching from index 1
  }
  do
  {
    /* check if handle is free */
    if (TabWin32Handles[ulHandle].hmHandleData.hHMHandle == INVALID_HANDLE_VALUE)
    {
        *phHandle16 = ulHandle;
        TabWin32Handles[ulHandle].hmHandleData.hHMHandle = hHandleOS2;
        TabWin32Handles[ulHandle].hmHandleData.lpDeviceData = NULL;
        HMGlobals.ulHandleLast = ulHandle;          /* to shorten search times */

        handleMutex.leave();
        return (NO_ERROR);                                               /* OK */
    }

    ulHandle++;                                        /* skip to next entry */

    if (ulHandle >= MAX_OS2_HMHANDLES)                     /* check boundary */
        ulHandle = 1;
  }
  while (ulHandle != HMGlobals.ulHandleLast);

  handleMutex.leave();

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

  TabWin32Handles[hHandle16].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
                                                                 /* OK, done */

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

  if (hHandle16 >= MAX_OS2_HMHANDLES)                      /* check boundary */
    return (ERROR_INVALID_HANDLE);                  /* raise error condition */

  if (TabWin32Handles[hHandle16].hmHandleData.hHMHandle == INVALID_HANDLE_VALUE)
                                                                  /* valid ? */
    return (ERROR_INVALID_HANDLE);                  /* raise error condition */

  return (NO_ERROR);
}
//*****************************************************************************
//*****************************************************************************
PHMHANDLEDATA HMQueryHandleData(HANDLE handle)
{
  int iIndex;

  iIndex = _HMHandleQuery(handle);                   /* get the index */
  if (-1 == iIndex)                                  /* error ? */
  {
      return NULL;
  }
  return &TabWin32Handles[iIndex].hmHandleData;      /* call device handler */
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

DWORD  HMHandleTranslateToWin (ULONG  hHandleOS2,
                               PULONG phHandle16)
{
           ULONG rc;                                       /* API returncode */
  register ULONG ulIndex;                    /* index counter over the table */

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleTranslateToWin (%08xh, %08xh)\n",
           hHandleOS2,
           phHandle16));
#endif

  for (ulIndex = 1;
       ulIndex < MAX_OS2_HMHANDLES;
       ulIndex++)
  {
                                                      /* look for the handle */
    if (TabWin32Handles[ulIndex].hmHandleData.hHMHandle == hHandleOS2)
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
                               PULONG phHandleOS2)
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleTranslateToOS2 (%08xh, %08xh)\n",
           hHandle16,
           phHandleOS2));
#endif

  if (HMHandleValidate(hHandle16) == NO_ERROR)              /* verify handle */
  {
    *phHandleOS2 = TabWin32Handles[hHandle16].hmHandleData.hHMHandle;
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

DWORD HMHandleTranslateToOS2i (ULONG  hHandle16)
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: HMHandleTranslateToOS2i (%08xh)\n",
           hHandle16));
#endif

  return(TabWin32Handles[hHandle16].hmHandleData.hHMHandle);
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

HANDLE HMGetStdHandle(DWORD nStdHandle)
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

BOOL HMSetStdHandle(DWORD  nStdHandle, HANDLE hHandleOpen32)
{
 PHMHANDLEDATA pHMHandleData;
 HANDLE        hHandle;

  hHandle = _HMHandleGetFree();              /* get free handle */
  if (hHandle == -1)                         /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[hHandle].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_CHAR;
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->dwUserData = nStdHandle;
  pHMHandleData->hHMHandle  = hHandleOpen32;
  pHMHandleData->lpHandlerData = NULL;

  TabWin32Handles[hHandle].pDeviceHandler = HMGlobals.pHMStandard;

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
 * Name      : HANDLE  HMDuplicateHandle
 * Purpose   : replacement for Open32's HMDuplicateHandle function
 * Parameters:
 *
 * Variables :
 * Result    : BOOL fSuccess
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen [Wed, 1999/08/25 15:44]
 *****************************************************************************/

BOOL HMDuplicateHandle(HANDLE  srcprocess,
                       HANDLE  srchandle,
                       HANDLE  destprocess,
                       PHANDLE desthandle,
                       DWORD   fdwAccess,
                       BOOL    fInherit,
                       DWORD   fdwOptions,
                       DWORD   fdwOdinOptions)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  BOOL            rc;                                     /* API return code */

  if(HMHandleValidate(srchandle) != NO_ERROR)
  {
    dprintf(("KERNEL32: HMDuplicateHandle: invalid handle %x", srchandle));
    SetLastError(ERROR_INVALID_HANDLE);      /* use this as error message */
    return FALSE;
  }

  pDeviceHandler = TabWin32Handles[srchandle].pDeviceHandler;  /* device is predefined */
  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return FALSE;                           /* signal error */
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData                = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType        = TabWin32Handles[srchandle].hmHandleData.dwType;
  if (fdwOptions & DUPLICATE_SAME_ACCESS)
    pHMHandleData->dwAccess    = TabWin32Handles[srchandle].hmHandleData.dwAccess;
  else
    pHMHandleData->dwAccess    = fdwAccess;

  if((fdwOdinOptions & DUPLICATE_ACCESS_READWRITE) == DUPLICATE_ACCESS_READWRITE) {
  pHMHandleData->dwAccess = GENERIC_READ | GENERIC_WRITE;
  }
  else
  if(fdwOdinOptions & DUPLICATE_ACCESS_READ) {
  pHMHandleData->dwAccess = GENERIC_READ;
  }

  if(fdwOdinOptions & DUPLICATE_SHARE_READ) {
  pHMHandleData->dwShare = FILE_SHARE_READ;
  }
  else
  if(fdwOdinOptions & DUPLICATE_SHARE_DENYNONE) {
  pHMHandleData->dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
  }
  else  pHMHandleData->dwShare = TabWin32Handles[srchandle].hmHandleData.dwShare;

  pHMHandleData->dwCreation    = TabWin32Handles[srchandle].hmHandleData.dwCreation;
  pHMHandleData->dwFlags       = TabWin32Handles[srchandle].hmHandleData.dwFlags;
  pHMHandleData->lpHandlerData = TabWin32Handles[srchandle].hmHandleData.lpHandlerData;


  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;
                                                  /* call the device handler */
  rc = pDeviceHandler->DuplicateHandle(&TabWin32Handles[iIndexNew].hmHandleData,
                                       srcprocess,
                                       &TabWin32Handles[srchandle].hmHandleData,
                                       destprocess,
                                       desthandle,
                                       fdwAccess,
                                       fInherit,
                                       fdwOptions & ~DUPLICATE_CLOSE_SOURCE,
                                       fdwOdinOptions);

  //Don't let Open32 close it for us, but do it manually (regardless of error; see SDK docs))
  if (fdwOptions & DUPLICATE_CLOSE_SOURCE)
    HMCloseHandle(srchandle);

  if(rc == FALSE)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    return FALSE;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  *desthandle = iIndexNew;
  return TRUE;                                   /* return valid handle */
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
  VOID            *pDevData;

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
    pDeviceHandler = _HMDeviceFind((LPSTR)lpFileName);        /* find device */
    if (NULL == pDeviceHandler)                /* this name is unknown to us */
    {
      SetLastError(ERROR_FILE_NOT_FOUND);
      return (INVALID_HANDLE_VALUE);                         /* signal error */
    }
    else
      pHMHandleData  = NULL;

    pDevData       = _HMDeviceGetData((LPSTR)lpFileName);

    if(pDeviceHandler == HMGlobals.pHMOpen32) {
        pDeviceHandler = HMGlobals.pHMFile;
    }
  }


  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  if (lpFileName == NULL)                            /* create from template */
    memcpy (&HMHandleTemp,
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
    HMHandleTemp.lpDeviceData  = pDevData;
  }

      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  HMHandleTemp.hHMHandle                    = iIndexNew;
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
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

             /* copy data fields that might have been modified by CreateFile */
    memcpy(&TabWin32Handles[iIndexNew].hmHandleData,
           &HMHandleTemp,
           sizeof(HMHANDLEDATA));
  }


#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32/HandleManager: CreateFile(%s)=%08xh\n",
           lpFileName,
           iIndexNew));
#endif

  return (HFILE)iIndexNew;                             /* return valid handle */
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
  VOID            *pDevData;
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */

  if(fuMode & OF_REOPEN) {
       pDeviceHandler = _HMDeviceFind((LPSTR)pOFStruct->szPathName);          /* find device */
  }
  else pDeviceHandler = _HMDeviceFind((LPSTR)lpFileName);          /* find device */
  if (NULL == pDeviceHandler)                  /* this name is unknown to us */
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }
  else
    pHMHandleData  = NULL;

  if(fuMode & OF_REOPEN) {
       pDevData       = _HMDeviceGetData((LPSTR)pOFStruct->szPathName);
  }
  else pDevData       = _HMDeviceGetData((LPSTR)lpFileName);
  

  if(pDeviceHandler == HMGlobals.pHMOpen32) {
    pDeviceHandler = HMGlobals.pHMFile;
  }

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

  //SvL; Must be OPEN_EXISTING because mmaps depend on it (to duplicate
  //     the file handle when this handle is a parameter for CreateFileMappingA/W
  pHMHandleData->dwCreation = OPEN_EXISTING;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->lpDeviceData  = pDevData;

      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

  rc = pDeviceHandler->OpenFile  (lpFileName,     /* call the device handler */
                                  &TabWin32Handles[iIndexNew].hmHandleData,
                                  pOFStruct,
                                  fuMode);

#ifdef DEBUG_LOCAL
    dprintf(("KERNEL32/HandleManager:CheckPoint3: %s lpHandlerData=%08xh rc=%08xh\n",
             lpFileName,
             &TabWin32Handles[iIndexNew].hmHandleData.lpHandlerData,
             rc));
#endif

  if(rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
      SetLastError(pOFStruct->nErrCode);
      return (INVALID_HANDLE_VALUE);                           /* signal error */
  }
  else {
    if(fuMode & (OF_DELETE|OF_EXIST)) {
    //file handle already closed
        TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    return TRUE; //TODO: correct?
  }
  if(fuMode & OF_PARSE) {
        TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    return 0;
  }
  if(fuMode & OF_VERIFY) {
        TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    return 1; //TODO: correct?
  }
  }

#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32/HandleManager: OpenFile(%s)=%08xh\n",
           lpFileName,
           iIndexNew));
#endif

  return iIndexNew;                                   /* return valid handle */
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
    //@@@PH it may occur someone closes e.g. a semaphore handle
    // which is not registered through the HandleManager yet.
    // so we try to pass on to Open32 instead.
    dprintf(("KERNEL32: HandleManager:HMCloseHandle(%08xh) passed on to Open32.\n",
             hObject));

    fResult = O32_CloseHandle(hObject);
    return (fResult);

    //SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    //return (FALSE);                                        /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  //SvL: Check if pDeviceHandler is set
  if (pHMHandle->pDeviceHandler)
  {
    fResult = pHMHandle->pDeviceHandler->CloseHandle(&pHMHandle->hmHandleData);
  }
  else
  {
    dprintf(("HMCloseHAndle(%08xh): pDeviceHandler not set", hObject));
    fResult = TRUE;
  }

  if (fResult == TRUE)                   /* remove handle if close succeeded */
  {
    pHMHandle->hmHandleData.hHMHandle = INVALID_HANDLE_VALUE; /* mark handle as free */
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?
  }

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
 * Name      : HANDLE  HMReadFileEx
 * Purpose   : Wrapper for the ReadFileEx() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMReadFileEx(HANDLE                     hFile,
                  LPVOID                     lpBuffer,
                  DWORD                      nNumberOfBytesToRead,
                  LPOVERLAPPED               lpOverlapped,
                  LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
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
  fResult = pHMHandle->pDeviceHandler->ReadFileEx(&pHMHandle->hmHandleData,
                                                  lpBuffer,
                                                  nNumberOfBytesToRead,
                                                  lpOverlapped,
                                                  lpCompletionRoutine);

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
 * Name      : HANDLE  HMWriteFileEx
 * Purpose   : Wrapper for the WriteFileEx() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMWriteFileEx(HANDLE                     hFile,
                   LPVOID                     lpBuffer,
                   DWORD                      nNumberOfBytesToWrite,
                   LPOVERLAPPED               lpOverlapped,
                   LPOVERLAPPED_COMPLETION_ROUTINE  lpCompletionRoutine)
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
  fResult = pHMHandle->pDeviceHandler->WriteFileEx(&pHMHandle->hmHandleData,
                                                   lpBuffer,
                                                   nNumberOfBytesToWrite,
                                                   lpOverlapped,
                                                   lpCompletionRoutine);

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
      SetLastError(ERROR_INVALID_HANDLE);     /* set win32 error information */
      return FILE_TYPE_UNKNOWN;                            /* signal failure */
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

BOOL HMGetFileInformationByHandle (HANDLE                     hFile,
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
    return FALSE;                         /* signal failure */
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
    return FALSE;                         /* signal failure */
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
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetFileTime(&pHMHandle->hmHandleData,
                                                   (LPFILETIME)pFT1,
                                                   (LPFILETIME)pFT2,
                                                   (LPFILETIME)pFT3);

  return (bResult);                                   /* deliver return code */
}

/*****************************************************************************
 * Name      : HMDeviceHandler::GetFileTime
 * Purpose   : router function for SetFileTime
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

BOOL HMGetFileTime (HANDLE         hFile,
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
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetFileTime(&pHMHandle->hmHandleData,
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
    return FALSE;                         /* signal failure */
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

BOOL HMLockFileEx(HANDLE        hFile,
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
    return FALSE;                         /* signal failure */
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
    return FALSE;                         /* signal failure */
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
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->UnlockFileEx(&pHMHandle->hmHandleData,
                                                     dwReserved,
                                                     nNumberOfBytesToLockLow,
                                                     nNumberOfBytesToLockHigh,
                                                     lpOverlapped);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::WaitForSingleObject
 * Purpose   : router function for WaitForSingleObject
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMWaitForSingleObject(HANDLE hObject,
                            DWORD  dwTimeout)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hObject);                         /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
       dprintf(("KERNEL32: HandleManager:HMWaitForSingleObject(%08xh) passed on to Open32.\n",
                 hObject));

       if(dwTimeout == INFINITE) {
          //Workaround for applications that block the PM input queue
          //while waiting for a child process to terminate.
          //(WaitSingleObject now calls MsgWaitMultipleObjects and
          // processes messages while waiting for the process to die)
          //(Napster install now doesn't block PM anymore (forcing a reboot))
 
          HMODULE hUser32 = LoadLibraryA("USER32.DLL");

          BOOL (* WINAPI pfnPeekMessageA)(LPMSG,HWND,UINT,UINT,UINT);
          LONG (* WINAPI pfnDispatchMessageA)(const MSG*);

          *(FARPROC *)&pfnPeekMessageA = GetProcAddress(hUser32,"PeekMessageA");
          *(FARPROC *)&pfnDispatchMessageA = GetProcAddress(hUser32,"DispatchMessageA");

          TEB *teb = GetThreadTEB();

          if(!teb || !pfnPeekMessageA || !pfnDispatchMessageA) {
              dprintf(("ERROR: !teb || !pfnPeekMessageA || !pfnDispatchMessageA"));
              DebugInt3();
              return WAIT_FAILED;
          }

          //TODO: Ignoring all messages could be dangerous. But processing them,
          //while the app doesn't expect any, isn't safe either.
//-> must active check in pmwindow.cpp if this is enabled again!
//          teb->o.odin.fIgnoreMsgs = TRUE;

          while(TRUE) {
              dwResult = HMMsgWaitForMultipleObjects(1, &hObject, FALSE, 
                                                     INFINITE, QS_ALLINPUT);
              if(dwResult == WAIT_OBJECT_0 + 1) {
                  MSG msg ;
   
                  while (pfnPeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
                  {
                     if (msg.message == WM_QUIT) {
                         dprintf(("ERROR: WaitForSingleObject call abandoned because WM_QUIT msg was received!!"));
//                       teb->o.odin.fIgnoreMsgs = FALSE;
                         FreeLibrary(hUser32);
                         return WAIT_ABANDONED;
                     }
   
                     /* otherwise dispatch it */
                     pfnDispatchMessageA(&msg);
   
                  } // end of PeekMessage while loop
              }
              else {
                  dprintf(("WaitForSingleObject: Process %x terminated", hObject));
                  break;
              }
          }
//          teb->o.odin.fIgnoreMsgs = FALSE;
          FreeLibrary(hUser32);
          return dwResult;
       }
       else {
          // maybe handles from CreateProcess() ...
          dwResult = O32_WaitForSingleObject(hObject, dwTimeout);
          return (dwResult);
       }
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->WaitForSingleObject(&pHMHandle->hmHandleData,
                                                            dwTimeout);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::WaitForSingleObjectEx
 * Purpose   : router function for WaitForSingleObjectEx
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMWaitForSingleObjectEx(HANDLE hObject,
                              DWORD  dwTimeout,
                              BOOL   fAlertable)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hObject);                         /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return WAIT_FAILED;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->WaitForSingleObjectEx(&pHMHandle->hmHandleData,
                                                              dwTimeout,
                                                              fAlertable);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::FlushFileBuffers
 * Purpose   : router function for FlushFileBuffers
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMFlushFileBuffers(HANDLE hFile)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->FlushFileBuffers(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMDeviceHandler::GetOverlappedResult
 * Purpose   : router function for GetOverlappedResult
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMGetOverlappedResult(HANDLE       hObject,
                           LPOVERLAPPED lpOverlapped,
                           LPDWORD      arg3,
                           BOOL         arg4)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hObject);                         /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->GetOverlappedResult(&pHMHandle->hmHandleData,
                                                            lpOverlapped,
                                                            arg3,
                                                            arg4);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMReleaseMutex
 * Purpose   : router function for ReleaseMutex
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMReleaseMutex(HANDLE hObject)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hObject);                         /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->ReleaseMutex(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMSetEvent
 * Purpose   : router function for SetEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMSetEvent(HANDLE hEvent)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hEvent);                          /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->SetEvent(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMPulseEvent
 * Purpose   : router function for PulseEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMPulseEvent(HANDLE hEvent)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hEvent);                          /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->PulseEvent(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HMResetEvent
 * Purpose   : router function for ResetEvent
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMResetEvent(HANDLE hEvent)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hEvent);                          /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->ResetEvent(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HANDLE  HMCreateEvent
 * Purpose   : Wrapper for the CreateEvent() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMCreateEvent(LPSECURITY_ATTRIBUTES lpsa,
                     BOOL                  bManualReset,
                     BOOL                  bInitialState,
                     LPCTSTR               lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  if(lpName) { //check if shared event semaphore already exists
      //TODO: No inheritance??
      HANDLE handle = HMOpenEvent(EVENT_ALL_ACCESS, FALSE, lpName);
      if(handle) {
          dprintf(("CreateEvent: return handle of existing event semaphore %x", handle));
          SetLastError(ERROR_ALREADY_EXISTS);
          return handle;
      }
  }

  pDeviceHandler = HMGlobals.pHMEvent;               /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }

  /* Initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;    
  pHMHandleData->lpHandlerData = NULL;

  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

  /* call the device handler */
  rc = pDeviceHandler->CreateEvent(&TabWin32Handles[iIndexNew].hmHandleData,
                                   lpsa,
                                   bManualReset,
                                   bInitialState,
                                   lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return 0;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMCreateMutex
 * Purpose   : Wrapper for the CreateMutex() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMCreateMutex(LPSECURITY_ATTRIBUTES lpsa,
                     BOOL                  bInitialOwner,
                     LPCTSTR               lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  if(lpName) { //check if shared mutex semaphore already exists
      //TODO: No inheritance??
      HANDLE handle = HMOpenMutex(MUTEX_ALL_ACCESS, FALSE, lpName);
      if(handle) {
          dprintf(("CreateMutex: return handle of existing mutex semaphore %x", handle));
          SetLastError(ERROR_ALREADY_EXISTS);
          return handle;
      }
  }

  pDeviceHandler = HMGlobals.pHMMutex;               /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */
  rc = pDeviceHandler->CreateMutex(&TabWin32Handles[iIndexNew].hmHandleData,
                                   lpsa,
                                   bInitialOwner,
                                   lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return 0;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMOpenEvent
 * Purpose   : Wrapper for the OpenEvent() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMOpenEvent(DWORD   fdwAccess,
                   BOOL    fInherit,
                   LPCTSTR lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  pDeviceHandler = HMGlobals.pHMEvent;               /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = fdwAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */
  rc = pDeviceHandler->OpenEvent(&TabWin32Handles[iIndexNew].hmHandleData,
                                 fInherit,
                                 lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return 0;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMOpenMutex
 * Purpose   : Wrapper for the OpenMutex() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMOpenMutex(DWORD   fdwAccess,
                   BOOL    fInherit,
                   LPCTSTR lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  pDeviceHandler = HMGlobals.pHMMutex;               /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = fdwAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */
  rc = pDeviceHandler->OpenMutex(&TabWin32Handles[iIndexNew].hmHandleData,
                                 fInherit,
                                 lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return 0;                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMCreateSemaphore
 * Purpose   : Wrapper for the CreateSemaphore() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMCreateSemaphore(LPSECURITY_ATTRIBUTES lpsa,
                         LONG                  lInitialCount,
                         LONG                  lMaximumCount,
                         LPCTSTR               lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */

  if(lpName) { //check if shared event semaphore already exists
      //TODO: No inheritance??
      HANDLE handle = HMOpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, lpName);
      if(handle) {
          dprintf(("CreateSemaphore: return handle of existing semaphore %x", handle));
          SetLastError(ERROR_ALREADY_EXISTS);
          return handle;
      }
  }
  
  pDeviceHandler = HMGlobals.pHMSemaphore;               /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */
  rc = pDeviceHandler->CreateSemaphore(&TabWin32Handles[iIndexNew].hmHandleData,
                                       lpsa,
                                       lInitialCount,
                                       lMaximumCount,
                                       lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return 0;                         /* signal failure */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMOpenSemaphore
 * Purpose   : Wrapper for the OpenSemaphore() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMOpenSemaphore(DWORD   fdwAccess,
                       BOOL    fInherit,
                       LPCTSTR lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  pDeviceHandler = HMGlobals.pHMSemaphore;               /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }


                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = fdwAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */
  rc = pDeviceHandler->OpenSemaphore(&TabWin32Handles[iIndexNew].hmHandleData,
                                     fInherit,
                                     lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return 0;                         /* signal failure */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HMReleaseSemaphore
 * Purpose   : router function for ReleaseSemaphore
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

BOOL HMReleaseSemaphore(HANDLE hEvent,
                        LONG   cReleaseCount,
                        LPLONG lpPreviousCount)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hEvent);                          /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return 0;                         /* signal failure */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->ReleaseSemaphore(&pHMHandle->hmHandleData,
                                                         cReleaseCount,
                                                         lpPreviousCount);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : HANDLE  HMCreateFileMapping
 * Purpose   : Wrapper for the CreateFileMapping() API
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMCreateFileMapping(HANDLE                hFile,
                           LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                           DWORD                 flProtect,
                           DWORD                 dwMaximumSizeHigh,
                           DWORD                 dwMaximumSizeLow,
                           LPCTSTR               lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */

  pDeviceHandler = HMGlobals.pHMFileMapping;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;                           /* signal error */
  }

                           /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;

      /* we've got to mark the handle as occupied here, since another device */
                   /* could be created within the device handler -> deadlock */

          /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */

  // @@@PH: Note: hFile is a Win32-style handle, it's not (yet) converted to
  //              a valid HandleManager-internal handle!
  rc = pDeviceHandler->CreateFileMapping(&TabWin32Handles[iIndexNew].hmHandleData,
                                         hFile,
                                         lpFileMappingAttributes,
                                         flProtect,
                                         dwMaximumSizeHigh,
                                         dwMaximumSizeLow,
                                         lpName);

  if(rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
      if(rc != ERROR_ALREADY_EXISTS) {
          TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
          SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
          return (NULL);                                           /* signal error */
      }
      SetLastError(ERROR_ALREADY_EXISTS);
      return iIndexNew;                                   /* return valid handle */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HANDLE  HMOpenFileMapping
 * Purpose   : Wrapper for the OpenFileMapping() API
 * Parameters:
 * Variables :
 * Result    : HANDLE if succeeded,
 *             NULL   if failed.
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

HANDLE HMOpenFileMapping(DWORD   fdwAccess,
                         BOOL    fInherit,
                         LPCTSTR lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  pDeviceHandler = HMGlobals.pHMFileMapping;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return (NULL);                                           /* signal error */
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = fdwAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */
  rc = pDeviceHandler->OpenFileMapping(&TabWin32Handles[iIndexNew].hmHandleData,
                                       fdwAccess,
                                       fInherit,
                                       lpName);
  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    SetLastError(rc);          /* Hehe, OS/2 and NT are pretty compatible :) */
    return (NULL);                                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  return iIndexNew;                                   /* return valid handle */
}


/*****************************************************************************
 * Name      : HMMapViewOfFileEx
 * Purpose   : router function for MapViewOfFileEx
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

LPVOID HMMapViewOfFileEx(HANDLE hFileMappingObject,
                         DWORD  dwDesiredAccess,
                         DWORD  dwFileOffsetHigh,
                         DWORD  dwFileOffsetLow,
                         DWORD  dwNumberOfBytesToMap,
                         LPVOID lpBaseAddress)
{
  int       iIndex;                           /* index into the handle table */
  LPVOID    lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFileMappingObject);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->MapViewOfFileEx(&pHMHandle->hmHandleData,
                                                      dwDesiredAccess,
                                                      dwFileOffsetHigh,
                                                      dwFileOffsetLow,
                                                      dwNumberOfBytesToMap,
                                                      lpBaseAddress);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMWaitForMultipleObjects
 * Purpose   : router function for WaitForMultipleObjects
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMWaitForMultipleObjects (DWORD   cObjects,
                                PHANDLE lphObjects,
                                BOOL    fWaitAll,
                                DWORD   dwTimeout)
{
#ifdef USE_OS2SEMAPHORES
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  if(cObjects == 1) {
      return HMWaitForSingleObject(*lphObjects, dwTimeout);
  }

  if(cObjects > MAXIMUM_WAIT_OBJECTS) {
      dprintf(("KERNEL32: HMWaitForMultipleObjects: Too many objects (%d)", cObjects));
      SetLastError(ERROR_INVALID_PARAMETER);
      return WAIT_FAILED;
  }

                                                          /* validate handle */
  iIndex = _HMHandleQuery(*lphObjects);                   /* get the index */
  if (-1 == iIndex)                                       /* error ? */
  {//oh, oh. possible problem here
   //TODO: rewrite handling of other handles; don't forward to open32
      dprintf(("WANRING: HMWaitForMultipleObjects: unknown handle passed on to Open32 -> will not work if other handles are semaphores"));
      return O32_WaitForMultipleObjects(cObjects, lphObjects, fWaitAll, dwTimeout);
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->WaitForMultipleObjects(&pHMHandle->hmHandleData,
                                                               cObjects, lphObjects, fWaitAll,
                                                               dwTimeout);

  return (dwResult);                                  /* deliver return code */
#else
  ULONG   ulIndex;
  PHANDLE pArrayOfHandles;
  PHANDLE pLoop1 = lphObjects;
  PHANDLE pLoop2;
  DWORD   rc;

  // allocate array for handle table
  pArrayOfHandles = (PHANDLE)alloca(cObjects * sizeof(HANDLE));
  if (pArrayOfHandles == NULL)
  {
    dprintf(("ERROR: HMWaitForMultipleObjects: alloca failed to allocate %d handles", cObjects));
    O32_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return WAIT_FAILED;
  }
  else
    pLoop2 = pArrayOfHandles;

  // convert array to odin handles
  for (ulIndex = 0;

       ulIndex < cObjects;

       ulIndex++,
       pLoop1++,
       pLoop2++)
  {
    rc = HMHandleTranslateToOS2 (*pLoop1, // translate handle
                                 pLoop2);

    dprintf(("KERNEL32: HMWaitForMultipleObjects: handle %3i: ODIN-%08xh, Open32-%08xh\n",
             ulIndex,
             *pLoop1,
             *pLoop2));

    //  @@@PH to imlpement: check handle type!
    // SvL: We still use Open32 handles for threads & processes -> don't fail here!
    if (rc != NO_ERROR)
    {
      dprintf(("KERNEL32: HMWaitForMultipleObjects - WARNING: handle %08xh is NOT an Odin handle (probably Open32 thread or process)\n",
               *pLoop1));

    *pLoop2 = *pLoop1;
////      O32_SetLastError(ERROR_INVALID_HANDLE);
////      return (WAIT_FAILED);
    }
  }

  // OK, now forward to Open32.
  // @@@PH: Note this will fail on handles that do NOT belong to Open32
  //        but to i.e. the console subsystem!
  rc = O32_WaitForMultipleObjects(cObjects,
                                  pArrayOfHandles,
                                  fWaitAll,
                                  dwTimeout);

  return (rc);                            // OK, done
#endif
}


/*****************************************************************************
 * Name      : HMWaitForMultipleObjectsEx
 * Purpose   : router function for WaitForMultipleObjectsEx
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD HMWaitForMultipleObjectsEx (DWORD   cObjects,
                                  PHANDLE lphObjects,
                                  BOOL    fWaitAll,
                                  DWORD   dwTimeout,
                                  BOOL    fAlertable)
{
  // @@@PH: Note: fAlertable is ignored !
  return (HMWaitForMultipleObjects(cObjects,
                                   lphObjects,
                                   fWaitAll,
                                   dwTimeout));
}

/*****************************************************************************
 * Name      : HMMsgWaitForMultipleObjects
 * Purpose   : router function for MsgWaitForMultipleObjects
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : Open32 doesn't implement this properly! (doesn't check dwWakeMask)
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1999/06/17 20:44]
 *****************************************************************************/

DWORD  HMMsgWaitForMultipleObjects  (DWORD      cObjects, 
                                     LPHANDLE   lphObjects,
                                     BOOL       fWaitAll,
                                     DWORD      dwTimeout,
                                     DWORD      dwWakeMask)
{
#ifdef USE_OS2SEMAPHORES
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */


  if(dwWakeMask == 0) {
      dprintf(("WARNING: wakemask == 0 -> calling WaitForMultipleObjects"));
      return HMWaitForMultipleObjects(cObjects, lphObjects, fWaitAll, dwTimeout);
  }
                                                          /* validate handle */
  iIndex = _HMHandleQuery(*lphObjects);                   /* get the index */
  if (-1 == iIndex)                                       /* error ? */
  {//oh, oh. possible problem here
   //TODO: rewrite handling of other handles; don't forward to open32
      dprintf(("WANRING: HMWaitForMultipleObjects: unknown handle passed on to Open32 -> will not work if other handles are semaphores"));
      return O32_MsgWaitForMultipleObjects(cObjects, lphObjects, fWaitAll, dwTimeout, dwWakeMask);
  }

  if(cObjects > MAXIMUM_WAIT_OBJECTS) {
      dprintf(("KERNEL32: HMMsgWaitForMultipleObjects: Too many objects (%d)", cObjects));
      SetLastError(ERROR_INVALID_PARAMETER);
      return WAIT_FAILED;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  dwResult = pHMHandle->pDeviceHandler->MsgWaitForMultipleObjects(&pHMHandle->hmHandleData,
                                                                  cObjects, lphObjects, fWaitAll,
                                                                  dwTimeout, dwWakeMask);

  return (dwResult);                                  /* deliver return code */
#else
  ULONG   ulIndex;
  PHANDLE pArrayOfHandles;
  PHANDLE pLoop1 = lphObjects;
  PHANDLE pLoop2;
  DWORD   rc;

  // allocate array for handle table
  pArrayOfHandles = (PHANDLE)alloca(cObjects * sizeof(HANDLE));
  if (pArrayOfHandles == NULL)
  {
      dprintf(("ERROR: HMMsgWaitForMultipleObjects: alloca failed to allocate %d handles", cObjects));
      O32_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return WAIT_FAILED;
  }
  else
    pLoop2 = pArrayOfHandles;

  // convert array to odin handles
  for (ulIndex = 0;

       ulIndex < cObjects;

       ulIndex++,
       pLoop1++,
       pLoop2++)
  {
    rc = HMHandleTranslateToOS2 (*pLoop1, // translate handle
                                 pLoop2);

    dprintf2(("MsgWaitForMultipleObjects handle %x->%x", *pLoop1, *pLoop2));
    // SvL: We still use Open32 handles for threads & processes -> don't fail here!
    if (rc != NO_ERROR)
    {
      dprintf(("KERNEL32: HMMsgWaitForMultipleObjects - WARNING: handle %08xh is NOT an Odin handle (probably Open32 thread or process)\n",
               *pLoop1));

    *pLoop2 = *pLoop1;
////      O32_SetLastError(ERROR_INVALID_HANDLE);
////      return (WAIT_FAILED);
    }
  }

  // OK, now forward to Open32.
  // @@@PH: Note this will fail on handles that do NOT belong to Open32
  //        but to i.e. the console subsystem!
  rc = O32_MsgWaitForMultipleObjects(cObjects,
                                     pArrayOfHandles,
                                     fWaitAll, dwTimeout,
                                     dwWakeMask);

  dprintf2(("MsgWaitForMultipleObjects returned %d", rc));
  return (rc);                            // OK, done
#endif
}
/*****************************************************************************
 * Name      : HMDeviceIoControl
 * Purpose   : router function for DeviceIoControl
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Sander van Leeuwen
 *****************************************************************************/

BOOL HMDeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode,
                       LPVOID lpInBuffer, DWORD nInBufferSize,
                       LPVOID lpOutBuffer, DWORD nOutBufferSize,
                       LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      fResult;       /* result from the device handler's CloseHandle() */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hDevice);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (FALSE);                                        /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  fResult = pHMHandle->pDeviceHandler->DeviceIoControl(&pHMHandle->hmHandleData,
                                                dwIoControlCode,
                                                lpInBuffer, nInBufferSize,
                                                lpOutBuffer, nOutBufferSize,
                                                lpBytesReturned, lpOverlapped);

  return (fResult);                                   /* deliver return code */
}



/*****************************************************************************
 * Name      : HMCOMGetCommState
 * Purpose   : router function for GetCommState
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Achim Hasenmueller [Sat, 1999/11/27 13:40]
 *****************************************************************************/

BOOL HMCommGetCommState(HANDLE hCommDev, LPDCB lpdcb)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpdcb,sizeof(DCB)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetCommState(&pHMHandle->hmHandleData,
                                                    lpdcb);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommWaitCommEvent( HANDLE hCommDev,
                          LPDWORD lpfdwEvtMask,
                          LPOVERLAPPED lpo)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  if(NULL!=lpo && IsBadReadPtr(lpo,sizeof(OVERLAPPED)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->WaitCommEvent( &pHMHandle->hmHandleData,
                                                      lpfdwEvtMask,
                                                      lpo);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommGetCommProperties( HANDLE hCommDev,
                              LPCOMMPROP lpcmmp)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpcmmp,sizeof(COMMPROP)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetCommProperties( &pHMHandle->hmHandleData,
                                                          lpcmmp);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommGetCommMask( HANDLE hCommDev,
                        LPDWORD lpfdwEvtMask)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpfdwEvtMask,sizeof(DWORD)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetCommMask( &pHMHandle->hmHandleData,
                                                    lpfdwEvtMask);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetCommMask( HANDLE hCommDev,
                        DWORD fdwEvtMask)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetCommMask( &pHMHandle->hmHandleData,
                                                    fdwEvtMask);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommPurgeComm( HANDLE hCommDev,
                      DWORD fdwAction)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }


  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->PurgeComm( &pHMHandle->hmHandleData,
                                                  fdwAction);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommClearCommError( HANDLE hCommDev,
                           LPDWORD lpdwErrors,
                           LPCOMSTAT lpcst)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpdwErrors,sizeof(DWORD)) ||
     (NULL!=lpcst && IsBadWritePtr(lpcst,sizeof(COMSTAT)) ) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->ClearCommError(&pHMHandle->hmHandleData,
                                                      lpdwErrors,
                                                      lpcst);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetCommState( HANDLE hCommDev,
                         LPDCB lpdcb)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadReadPtr(lpdcb,sizeof(DCB)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }


  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetCommState(&pHMHandle->hmHandleData,
                                                    lpdcb);

  return (bResult);                                  /* deliver return code */
}


BOOL HMCommGetCommTimeouts( HANDLE hCommDev,
                            LPCOMMTIMEOUTS lpctmo)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpctmo,sizeof(COMMTIMEOUTS)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetCommTimeouts( &pHMHandle->hmHandleData,
                                                        lpctmo);

  return (bResult);                                  /* deliver return code */
}
BOOL HMCommGetCommModemStatus( HANDLE hCommDev,
                               LPDWORD lpModemStat )
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpModemStat,sizeof(DWORD)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetCommModemStatus( &pHMHandle->hmHandleData,
                                                           lpModemStat);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetCommTimeouts( HANDLE hCommDev,
                            LPCOMMTIMEOUTS lpctmo)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadReadPtr(lpctmo,sizeof(COMMTIMEOUTS)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetCommTimeouts( &pHMHandle->hmHandleData,
                                                        lpctmo);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommTransmitCommChar( HANDLE hCommDev,
                             CHAR cChar )
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->TransmitCommChar( &pHMHandle->hmHandleData,
                                                         cChar);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetCommConfig( HANDLE hCommDev,
                          LPCOMMCONFIG lpCC,
                          DWORD dwSize )
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if( IsBadReadPtr(lpCC,sizeof(COMMCONFIG)) ||
      dwSize < sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetCommConfig( &pHMHandle->hmHandleData,
                                                      lpCC,
                                                      dwSize);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetCommBreak( HANDLE hCommDev )
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetCommBreak( &pHMHandle->hmHandleData);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommGetCommConfig( HANDLE hCommDev,
                          LPCOMMCONFIG lpCC,
                          LPDWORD lpdwSize )
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpdwSize,sizeof(DWORD)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  if( IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetCommConfig( &pHMHandle->hmHandleData,
                                                      lpCC,
                                                      lpdwSize);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommEscapeCommFunction( HANDLE hCommDev,
                               UINT dwFunc )
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */

  switch(dwFunc)
  {
    case CLRDTR:
    case CLRRTS:
    case SETDTR:
    case SETRTS:
    case SETXOFF:
    case SETXON:
      bResult = pHMHandle->pDeviceHandler->EscapeCommFunction( &pHMHandle->hmHandleData,
                                                               dwFunc);
      break;
    case SETBREAK:
      bResult = pHMHandle->pDeviceHandler->SetCommBreak(&pHMHandle->hmHandleData);
      break;
    case CLRBREAK:
      bResult = pHMHandle->pDeviceHandler->ClearCommBreak(&pHMHandle->hmHandleData);
      break;
    default:
      SetLastError(ERROR_INVALID_PARAMETER);
      bResult = FALSE;
  }


  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetupComm( HANDLE hCommDev,
                      DWORD dwInQueue,
                      DWORD dwOutQueue)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetupComm(&pHMHandle->hmHandleData,
                                                 dwInQueue,
                                                 dwOutQueue);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommClearCommBreak(HANDLE hCommDev)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->ClearCommBreak(&pHMHandle->hmHandleData);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommSetDefaultCommConfig( HANDLE hCommDev,
                                 LPCOMMCONFIG lpCC,
                                 DWORD dwSize)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if( (lpCC!=NULL) &&
      ( IsBadReadPtr(lpCC,sizeof(COMMCONFIG)) ||
        dwSize != sizeof(COMMCONFIG) ) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->SetDefaultCommConfig(&pHMHandle->hmHandleData,
                                                            lpCC,
                                                            dwSize);

  return (bResult);                                  /* deliver return code */
}

BOOL HMCommGetDefaultCommConfig( HANDLE hCommDev,
                                 LPCOMMCONFIG lpCC,
                                 LPDWORD lpdwSize)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hCommDev);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (NULL);                                         /* signal failure */
  }

  if(IsBadWritePtr(lpdwSize,sizeof(DWORD)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  bResult = pHMHandle->pDeviceHandler->GetDefaultCommConfig( &pHMHandle->hmHandleData,
                                                             lpCC,
                                                             lpdwSize);

  return (bResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMOpenThreadToken
 * Purpose   : router function for NtOpenThreadToken
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

DWORD HMOpenThreadToken(HANDLE  ThreadHandle,
                        DWORD   DesiredAccess,
                        DWORD   OpenAsSelf,
                        HANDLE *TokenHandle)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */

  pDeviceHandler = HMGlobals.pHMToken;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return ERROR_NOT_ENOUGH_MEMORY;
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = DesiredAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */

  // @@@PH: Note: hFile is a Win32-style handle, it's not (yet) converted to
  //              a valid HandleManager-internal handle!
  rc = pDeviceHandler->OpenThreadToken(&TabWin32Handles[iIndexNew].hmHandleData,
                                       ThreadHandle,
                                       OpenAsSelf);

  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
  return (rc);                                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  *TokenHandle = iIndexNew;                                   /* return valid handle */
  return STATUS_SUCCESS;
}

/*****************************************************************************
 * Name      : HMOpenProcessToken
 * Purpose   : router function for NtOpenProcessToken
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMOpenProcessToken(HANDLE  ProcessHandle,
                         DWORD   DesiredAccess,
                         DWORD   dwUserData,
                         HANDLE *TokenHandle)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */

  pDeviceHandler = HMGlobals.pHMToken;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return ERROR_NOT_ENOUGH_MEMORY;
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = DesiredAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */

  // @@@PH: Note: hFile is a Win32-style handle, it's not (yet) converted to
  //              a valid HandleManager-internal handle!
  rc = pDeviceHandler->OpenProcessToken(&TabWin32Handles[iIndexNew].hmHandleData,
                                        dwUserData,
                                        ProcessHandle);

  if (rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
  return (rc);                                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  *TokenHandle = iIndexNew;                                   /* return valid handle */
  return STATUS_SUCCESS;
}
/*****************************************************************************
 * Name      : HMCreateThread
 * Purpose   : router function for CreateThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
HANDLE HMCreateThread(LPSECURITY_ATTRIBUTES  lpsa,
                      DWORD                  cbStack,
                      LPTHREAD_START_ROUTINE lpStartAddr,
                      LPVOID                 lpvThreadParm,
                      DWORD                  fdwCreate,
                      LPDWORD                lpIDThread,
                      BOOL                   fFirstThread)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  HANDLE          rc;                                     /* API return code */

  SetLastError(ERROR_SUCCESS);

  pDeviceHandler = HMGlobals.pHMThread;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;      /* unknown handle type */
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;


  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

                                                  /* call the device handler */

  // @@@PH: Note: hFile is a Win32-style handle, it's not (yet) converted to
  //              a valid HandleManager-internal handle!
  rc = pDeviceHandler->CreateThread(&TabWin32Handles[iIndexNew].hmHandleData,
                                    lpsa,  cbStack, lpStartAddr,
                                    lpvThreadParm, fdwCreate, lpIDThread, fFirstThread);

  if (rc == 0)     /* oops, creation failed within the device handler */
  {
      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
  return 0;                                           /* signal error */
  }

  return iIndexNew;
}
/*****************************************************************************
 * Name      : HMGetThreadPriority
 * Purpose   : router function for GetThreadPriority
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
INT HMGetThreadPriority(HANDLE hThread)
{
  int       iIndex;                           /* index into the handle table */
  INT       lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (-1);                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->GetThreadPriority(&TabWin32Handles[iIndex].hmHandleData);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSuspendThread
 * Purpose   : router function for SuspendThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMSuspendThread(HANDLE hThread)
{
  int       iIndex;                           /* index into the handle table */
  HANDLE    lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                         /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return -1;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->SuspendThread(&TabWin32Handles[iIndex].hmHandleData);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetThreadPriority
 * Purpose   : router function for SetThreadPriority
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMSetThreadPriority(HANDLE hThread, int priority)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                   /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->SetThreadPriority(&TabWin32Handles[iIndex].hmHandleData, priority);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMGetThreadContext
 * Purpose   : router function for GetThreadContext
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMGetThreadContext(HANDLE hThread, CONTEXT *lpContext)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->GetThreadContext(&TabWin32Handles[iIndex].hmHandleData, lpContext);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetThreadContext
 * Purpose   : router function for SetThreadContext
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMSetThreadContext(HANDLE hThread, const CONTEXT *lpContext)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->SetThreadContext(&TabWin32Handles[iIndex].hmHandleData, lpContext);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMTerminateThread
 * Purpose   : router function for TerminateThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMTerminateThread(HANDLE hThread, DWORD exitcode)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->TerminateThread(&TabWin32Handles[iIndex].hmHandleData, exitcode);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMResumeThread
 * Purpose   : router function for ResumeThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
DWORD HMResumeThread(HANDLE hThread)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                 /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return -1;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->ResumeThread(&TabWin32Handles[iIndex].hmHandleData);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMGetExitCodeThread
 * Purpose   : router function for GetExitCodeThread
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMGetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->GetExitCodeThread(&TabWin32Handles[iIndex].hmHandleData, lpExitCode);

  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetThreadTerminated
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMSetThreadTerminated(HANDLE hThread)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hThread);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->SetThreadTerminated(&TabWin32Handles[iIndex].hmHandleData);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMPeekNamedPipe
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL   HMPeekNamedPipe(HANDLE hPipe,
                       LPVOID  lpvBuffer,
                       DWORD   cbBuffer,
                       LPDWORD lpcbRead,
                       LPDWORD lpcbAvail,
                       LPDWORD lpcbMessage)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->PeekNamedPipe(&TabWin32Handles[iIndex].hmHandleData,
                                                      lpvBuffer,
                                                      cbBuffer,
                                                      lpcbRead,
                                                      lpcbAvail,
                                                      lpcbMessage);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMCreateNamedPipe
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
DWORD HMCreateNamedPipe(LPCTSTR lpName,
                      DWORD   dwOpenMode,
                      DWORD   dwPipeMode,
                      DWORD   nMaxInstances,
                      DWORD   nOutBufferSize,
                      DWORD   nInBufferSize,
                      DWORD   nDefaultTimeOut,
                      LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  HANDLE          rc;                                     /* API return code */

  SetLastError(ERROR_SUCCESS);

  pDeviceHandler = HMGlobals.pHMNamedPipe;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_PIPE;
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;

  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

  /* call the device handler */

  rc = pDeviceHandler->CreateNamedPipe(&TabWin32Handles[iIndexNew].hmHandleData,
                                       lpName,dwOpenMode,
                                       dwPipeMode,nMaxInstances,
                                       nOutBufferSize,nInBufferSize,
                                       nDefaultTimeOut,lpSecurityAttributes);

  if (rc == -1)     /* oops, creation failed within the device handler */
  {
      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
      return 0;                                           /* signal error */
  }

  dprintf(("Win32 Handle -> %08x",iIndexNew));

  return iIndexNew;
}

/*****************************************************************************
 * Name      : HMConnectNamedPipe
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMConnectNamedPipe(HANDLE hPipe, LPOVERLAPPED lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->ConnectNamedPipe(&TabWin32Handles[iIndex].hmHandleData,
                                                         lpOverlapped);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMDisconnectNamedPipe
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMDisconnectNamedPipe(HANDLE hPipe)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->DisconnectNamedPipe(&TabWin32Handles[iIndex].hmHandleData);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMGetNamedPipeHandleState
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMGetNamedPipeHandleState(HANDLE hPipe,
                               LPDWORD lpState,
                               LPDWORD lpCurInstances,
                               LPDWORD lpMaxCollectionCount,
                               LPDWORD lpCollectDataTimeout,
                               LPTSTR  lpUserName,
                               DWORD   nMaxUserNameSize)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->GetNamedPipeHandleState(&TabWin32Handles[iIndex].hmHandleData,
                                                                lpState,
                                                                lpCurInstances,
                                                                lpMaxCollectionCount,
                                                                lpCollectDataTimeout,
                                                                lpUserName,
                                                                nMaxUserNameSize);


  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMGetNamedPipeInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMGetNamedPipeInfo(HANDLE hPipe,
                        LPDWORD lpFlags,
                        LPDWORD lpOutBufferSize,
                        LPDWORD lpInBufferSize,
                        LPDWORD lpMaxInstances)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->GetNamedPipeInfo(&TabWin32Handles[iIndex].hmHandleData,
                                                         lpFlags,
                                                         lpOutBufferSize,
                                                         lpInBufferSize,
                                                         lpMaxInstances);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMTransactNamedPipe
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
DWORD HMTransactNamedPipe(HANDLE hPipe,
                          LPVOID       lpvWriteBuf,
                          DWORD        cbWriteBuf,
                          LPVOID       lpvReadBuf,
                          DWORD        cbReadBuf,
                          LPDWORD      lpcbRead,
                          LPOVERLAPPED lpo)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->TransactNamedPipe(&TabWin32Handles[iIndex].hmHandleData,
                                                          lpvWriteBuf,
                                                          cbWriteBuf,
                                                          lpvReadBuf,
                                                          cbReadBuf,
                                                          lpcbRead,
                                                          lpo);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMSetNamedPipeHandleState
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMSetNamedPipeHandleState(HANDLE  hPipe,
                               LPDWORD lpdwMode,
                               LPDWORD lpcbMaxCollect,
                               LPDWORD lpdwCollectDataTimeout)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hPipe);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->SetNamedPipeHandleState(&TabWin32Handles[iIndex].hmHandleData,
                                                                lpdwMode,
                                                                lpcbMaxCollect,
                                                                lpdwCollectDataTimeout);

  return (lpResult);                                  /* deliver return code */
}

/*****************************************************************************
 * Name      : HMCreatePipe
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : NOT TESTED!
 *
 * Author    : Przemyslaw Dobrowolski
 *****************************************************************************/
BOOL HMCreatePipe(PHANDLE phRead,
                PHANDLE phWrite,
                LPSECURITY_ATTRIBUTES lpsa,
                DWORD                 cbPipe)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNewRead;              /* index into the handle table */
  int             iIndexNewWrite;             /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;            /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  HANDLE          rc;                                     /* API return code */

  SetLastError(ERROR_SUCCESS);

  pDeviceHandler = HMGlobals.pHMNamedPipe;         /* device is predefined */

  iIndexNewRead = _HMHandleGetFree();              /* get free handle */
  if (-1 == iIndexNewRead)                         /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }

  iIndexNewWrite = _HMHandleGetFree();              /* get free handle */
  if (-1 == iIndexNewWrite)                         /* oops, no free handles ! */
  {
    HMHandleFree(iIndexNewRead);
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }


  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNewRead].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_PIPE;
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;

  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNewRead].hmHandleData.hHMHandle = iIndexNewRead;
  TabWin32Handles[iIndexNewRead].pDeviceHandler         = pDeviceHandler;

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNewWrite].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_PIPE;
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;

  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNewWrite].hmHandleData.hHMHandle = iIndexNewWrite;
  TabWin32Handles[iIndexNewWrite].pDeviceHandler         = pDeviceHandler;
  /* call the device handler */

  rc = pDeviceHandler->CreatePipe(&TabWin32Handles[iIndexNewRead].hmHandleData,
                                  &TabWin32Handles[iIndexNewWrite].hmHandleData,
                                  lpsa,
                                  cbPipe);

  if (rc == 0)     /* oops, creation failed within the device handler */
  {
      HMHandleFree(iIndexNewRead);
      HMHandleFree(iIndexNewWrite);
      return FALSE;                                           /* signal error */
  }

  *phRead  = iIndexNewRead;
  *phWrite = iIndexNewWrite;

  return TRUE;
}

/*****************************************************************************
 * Name      : HMCreateMailslotA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/
HANDLE HMCreateMailslotA(LPCSTR lpName, DWORD nMaxMessageSize,
                         DWORD lReadTimeout,
                         LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMMailslotClass *pDeviceHandler;            /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  BOOL            rc;                                     /* API return code */

  SetLastError(ERROR_SUCCESS);

  pDeviceHandler = (HMMailslotClass *)HMGlobals.pHMMailslot;         /* device is predefined */

  iIndexNew = _HMHandleGetFree();                         /* get free handle */
  if (-1 == iIndexNew)                            /* oops, no free handles ! */
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);      /* use this as error message */
    return 0;
  }

  /* initialize the complete HMHANDLEDATA structure */
  pHMHandleData = &TabWin32Handles[iIndexNew].hmHandleData;
  pHMHandleData->dwType     = FILE_TYPE_UNKNOWN;
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;

  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;

  /* call the device handler */

  rc = pDeviceHandler->CreateMailslotA(&TabWin32Handles[iIndexNew].hmHandleData,
                                       lpName, nMaxMessageSize,
                                       lReadTimeout, lpSecurityAttributes);

  if (rc == FALSE)     /* oops, creation failed within the device handler */
  {
      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
      return 0;                                           /* signal error */
  }

  return iIndexNew;
}
/*****************************************************************************
 * Name      : HMGetMailslotInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMGetMailslotInfo(HANDLE  hMailslot,
                       LPDWORD lpMaxMessageSize,
                       LPDWORD lpNextSize,
                       LPDWORD lpMessageCount,
                       LPDWORD lpReadTimeout)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hMailslot);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->GetMailslotInfo(&TabWin32Handles[iIndex].hmHandleData,
                                                        lpMaxMessageSize,
                                                        lpNextSize,
                                                        lpMessageCount,
                                                        lpReadTimeout);
  return (lpResult);                                  /* deliver return code */
}
/*****************************************************************************
 * Name      : HMSetMailslotInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMSetMailslotInfo(HANDLE hMailslot,
                       DWORD  dwReadTimeout)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      lpResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

  SetLastError(ERROR_SUCCESS);
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hMailslot);              /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  lpResult = pHMHandle->pDeviceHandler->SetMailslotInfo(&TabWin32Handles[iIndex].hmHandleData,
                                                       dwReadTimeout);

  return (lpResult);                                  /* deliver return code */
}