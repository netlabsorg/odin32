/* $Id: HandleManager.cpp,v 1.104 2003-06-02 16:25:15 sandervl Exp $ */

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

#include <unicode.h>
#include <dbglog.h>

#include <HandleManager.H>
#include "handlenames.h"
#include "HMDevice.h"
#include "HMDisk.h"
#include "HMOpen32.h"
#include "HMEvent.h"
#include "HMFile.h"
#include "HMMutex.h"
#include "HMSemaphore.h"
#include "HMMMap.h"
#include "HMComm.h"
#include "HMParPort.h"
#include "HMNul.h"
#include "HMToken.h"
#include "HMThread.h"
#include "HMNPipe.h"
#include "HMStd.h"
#include "HMMailslot.h"

#include "hmhandle.h"

#include <vmutex.h>
#include <win\thread.h>

#include <odinapi.h>

#include <_ras.h>

#define DBG_LOCALLOG  DBG_handlemanager
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

// this is the size of our currently static handle table
#define MAX_OS2_HMHANDLES 	(16*1024)
#define ERROR_SYS_INTERNAL      328

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/


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
static int       lastIndex       = 1;
static HMHANDLE *TabWin32Handles = NULL;   /* static handle table */
VMutex           handleMutex;

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
  HMDeviceHandler        *pHMInfoFile;
  HMDeviceHandler        *pHMDisk;
  HMDeviceHandler        *pHMMutex;
  HMDeviceHandler        *pHMSemaphore;
  HMDeviceHandler        *pHMFileMapping;  /* static instances of subsystems */
  HMDeviceHandler        *pHMComm;                   /* serial communication */
  HMDeviceHandler        *pHMToken;         /* security tokens */
  HMDeviceHandler        *pHMThread;
  HMDeviceHandler        *pHMNamedPipe;
  HMDeviceHandler        *pHMMailslot;
  HMDeviceHandler        *pHMParPort;              /* parallel communication */
  HMDeviceHandler        *pHMNul;                              /* nul device */

  ULONG         ulHandleLast;                   /* index of last used handle */
} HMGlobals;


#ifdef RAS

RAS_TRACK_HANDLE rthHandles = 0;

ULONG WIN32API LogObjectContent_Handle (ULONG objident, ULONG objhandle, void *objdata, ULONG cbobjdata, FNRASLOG_EXTERNAL *pRasLog)
{
    pRasLog ("    %8.8x: data=%p, type=%x, internal type=%x", objhandle, TabWin32Handles[objhandle].hmHandleData.hHMHandle, GetFileType(objhandle), TabWin32Handles[objhandle].hmHandleData.dwInternalType);
    return 0;
}

#endif

#define FREEHANDLE(a) do {                                            \
    TabWin32Handles[a].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE; \
    RasRemoveObject(rthHandles, a);                                   \
  } while (0)

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

  //find next free handle; do not reuse handles until we have no choice
  if(lastIndex >= MAX_OS2_HMHANDLES) {
        lastIndex = 1;
  }

  for (ulLoop = lastIndex;ulLoop < MAX_OS2_HMHANDLES; ulLoop++)
  {
                                                       /* free handle found ? */
    if (INVALID_HANDLE_VALUE == TabWin32Handles[ulLoop].hmHandleData.hHMHandle)
    {
        //Mark handle as allocated here.
        memset(&TabWin32Handles[ulLoop].hmHandleData, 0, sizeof(TabWin32Handles[ulLoop].hmHandleData));
        TabWin32Handles[ulLoop].hmHandleData.hHMHandle      = ulLoop;
        TabWin32Handles[ulLoop].hmHandleData.dwInternalType = HMTYPE_UNKNOWN;
        TabWin32Handles[ulLoop].hmHandleData.hWin32Handle   = (HANDLE)ulLoop;
        lastIndex = ulLoop+1;
        handleMutex.leave();
        RasAddObject (rthHandles, ulLoop, NULL, 0);
        return (ulLoop);                    /* OK, then return it to the caller */
    }
  }

  handleMutex.leave();
  dprintf(("KERNEL32:HandleManager:_HMHandleGetFree() no free handle (%d already allocated)\n", ulLoop));
#ifdef RAS
  RasLog ("KERNEL32:HandleManager:_HMHandleGetFree() no free handle");
  RasLogObjects (rthHandles, RAS_FLAG_LOG_OBJECTS);
#endif
  return (INVALID_HANDLE_VALUE);             /* haven't found any free handle */
}

/*****************************************************************************
 * Name      : static int _HMHandleGetFree
 * Purpose   : get pointer to first free handle in the handle table
 * Parameters:
 * Variables :
 * Result    : pointer to the table or NULL in case of error
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
PHMHANDLE HMHandleGetFreePtr(ULONG dwType)
{
  ULONG handle;
  PHMHANDLE pHandle;

  handle = _HMHandleGetFree();
  if(handle == INVALID_HANDLE_VALUE) {
      return NULL;
  }
  pHandle = &TabWin32Handles[handle];
  switch(dwType) {
  case HMTYPE_MEMMAP:
        pHandle->pDeviceHandler = HMGlobals.pHMFileMapping;
        break;
  case HMTYPE_DEVICE:
        pHandle->pDeviceHandler = HMGlobals.pHMFile;
        break;
  case HMTYPE_PROCESSTOKEN:
  case HMTYPE_THREADTOKEN:
        pHandle->pDeviceHandler = HMGlobals.pHMToken;
        break;
  case HMTYPE_THREAD:
        pHandle->pDeviceHandler = HMGlobals.pHMThread;
        break;
  case HMTYPE_PIPE:
        pHandle->pDeviceHandler = HMGlobals.pHMNamedPipe;
        break;
  case HMTYPE_EVENTSEM:
        pHandle->pDeviceHandler = HMGlobals.pHMEvent;
        break;
  case HMTYPE_MUTEXSEM:
        pHandle->pDeviceHandler = HMGlobals.pHMMutex;
        break;
  case HMTYPE_SEMAPHORE:
        pHandle->pDeviceHandler = HMGlobals.pHMSemaphore;
        break;
  case HMTYPE_COMPORT:
        pHandle->pDeviceHandler = HMGlobals.pHMComm;
        break;
  case HMTYPE_PARPORT:
        pHandle->pDeviceHandler = HMGlobals.pHMParPort;
        break;
  case HMTYPE_MAILSLOT:
        pHandle->pDeviceHandler = HMGlobals.pHMMailslot;
        break;

  case HMTYPE_UNKNOWN:
  default:
        DebugInt3();
        HMHandleFree(handle);
        return NULL;
  }

  pHandle->hmHandleData.dwInternalType = dwType;
  return pHandle;
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
    if (hHandle >= MAX_OS2_HMHANDLES)                 /* check the table range */
    {
        /* Standard handle? */
        switch (hHandle)
        {
            case STD_INPUT_HANDLE:  hHandle = HMGlobals.hStandardIn; break;
            case STD_OUTPUT_HANDLE: hHandle = HMGlobals.hStandardOut; break;
            case STD_ERROR_HANDLE:  hHandle = HMGlobals.hStandardError; break;
            default:
                return(-1);
        }
        if (hHandle >= MAX_OS2_HMHANDLES)
            return(-1);
    }
                                                   /* Oops, invalid handle ! */
  if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
        return(-1);              /* nope, ERROR_INVALID_HANDLE */

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
    if (hHandle >= MAX_OS2_HMHANDLES)                  /* check the table range */
    {
        /* Standard handle? */
        switch (hHandle)
        {
            case STD_INPUT_HANDLE:  hHandle = HMGlobals.hStandardIn; break;
            case STD_OUTPUT_HANDLE: hHandle = HMGlobals.hStandardOut; break;
            case STD_ERROR_HANDLE:  hHandle = HMGlobals.hStandardError; break;
            default:
                return(-1);
        }
        if (hHandle >= MAX_OS2_HMHANDLES)
            return(-1);
    }

                                                   /* Oops, invalid handle ! */
  if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
        return(-1);              /* nope, ERROR_INVALID_HANDLE */

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

static ULONG INLINE _HMHandleQuery(HANDLE hHandle)
{
    if (hHandle >= MAX_OS2_HMHANDLES)        /* check the table range */
    {
        /* Standard handle? */
        switch (hHandle)
        {
            case STD_INPUT_HANDLE:  hHandle = HMGlobals.hStandardIn; break;
            case STD_OUTPUT_HANDLE: hHandle = HMGlobals.hStandardOut; break;
            case STD_ERROR_HANDLE:  hHandle = HMGlobals.hStandardError; break;
            default:
                return(INVALID_HANDLE_VALUE); /* nope, ERROR_INVALID_HANDLE */
        }
        if (hHandle >= MAX_OS2_HMHANDLES)    /* check the table range */
            return(INVALID_HANDLE_VALUE);   /* nope, ERROR_INVALID_HANDLE */
    }

                                                   /* Oops, invalid handle ! */
  if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
        return(INVALID_HANDLE_VALUE);       /* nope, ERROR_INVALID_HANDLE */

    return( hHandle);                       /* OK, we've got our handle index */
}

/*****************************************************************************
 * Name      : HMHandleQueryPtr
 * Purpose   : gets the pointer of handle table entry as fast as possible from
 *             the specified handle
 * Parameters: HANDLE hHandle
 * Variables :
 * Result    : pointer or NULL in case of error
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/

PHMHANDLE HMHandleQueryPtr(HANDLE hHandle)
{
    if (hHandle >= MAX_OS2_HMHANDLES)
    {
        /* Standard handle? */
        switch (hHandle)
        {
            case STD_INPUT_HANDLE:  hHandle = HMGlobals.hStandardIn; break;
            case STD_OUTPUT_HANDLE: hHandle = HMGlobals.hStandardOut; break;
            case STD_ERROR_HANDLE:  hHandle = HMGlobals.hStandardError; break;
            default:
                SetLastError(ERROR_INVALID_HANDLE);
                return(NULL);
        }
        if (hHandle >= MAX_OS2_HMHANDLES)
        {
            SetLastError(ERROR_INVALID_HANDLE);
            return(NULL);
        }
    }
    if (INVALID_HANDLE_VALUE == TabWin32Handles[hHandle].hmHandleData.hHMHandle)
    {
        SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
        return(NULL);              /* nope, ERROR_INVALID_HANDLE */
    }
    return( &TabWin32Handles[hHandle]);                       /* OK, we've got our handle index */
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

DWORD   HMDeviceRegisterEx(LPCSTR          pszDeviceName,
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

DWORD   HMDeviceRegister(LPCSTR          pszDeviceName,
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

#ifdef RAS
    RasRegisterObjectTracking(&rthHandles, "KERNEL32 handles",
                              0, RAS_TRACK_FLAG_LOGOBJECTCONTENT,
                              LogObjectContent_Handle, NULL);
#endif

    handleMutex.enter();

    TabWin32Handles = (HMHANDLE *)VirtualAlloc(NULL, MAX_OS2_HMHANDLES*sizeof(HMHANDLE), MEM_COMMIT, PAGE_READWRITE);
    if(TabWin32Handles == NULL) {
        DebugInt3();
        return ERROR_NOT_ENOUGH_MEMORY;
    }

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

#if 1
    //This is a very bad idea. \\\\.\\NTICE -> NTICE, if the file exits, then
    //it will open the file instead of the device driver
    /* add standard symbolic links first, so local symbolic links in the
     * device handlers get precedence over the default here.
     *
     * Device handlers are supposed to place the appropriate
     * symbolic links such as "\\.\\COM1", "COM1"
     *
     * - "\\.\f:\temp\readme.txt" is a valid file
     * - "com1" is a valid device in case serial port 1 exists,
     *   otherwise it'd be a valid file.
     * - "\\.\filename" is the only misleading case (to be verified)
     *
     * Note:
     * the Open32 "device" definately MUST be the last device to be
     * asked to accept the specified name.
     */
    {
      // strings are placed in read-only segment
      PSZ pszDrive  = strdup("\\\\.\\x:");
      PSZ pszDrive2 = strdup("\\\\.\\x:");
      for (char ch = 'A'; ch <= 'Z'; ch++)
      {
        pszDrive[4] = ch;
        pszDrive2[4] = ch;
        HandleNamesAddSymbolicLink(pszDrive, pszDrive+4);
        HandleNamesAddSymbolicLink(pszDrive2, pszDrive2+4);
      }
      free(pszDrive);
      free(pszDrive2);
      HandleNamesAddSymbolicLink("\\\\?\\UNC\\", "\\\\");
      //SvL: Can be used in Windows 2000 to open device drivers
      HandleNamesAddSymbolicLink("\\\\.\\Global", "\\\\.");
    }
#endif

    /* copy standard handles from OS/2's Open32 Subsystem */
    HMGlobals.pHMStandard   = new HMDeviceStandardClass("\\\\STANDARD_HANDLE\\");
    HMSetStdHandle(STD_INPUT_HANDLE,  O32_GetStdHandle(STD_INPUT_HANDLE));
    HMSetStdHandle(STD_OUTPUT_HANDLE, O32_GetStdHandle(STD_OUTPUT_HANDLE));
    HMSetStdHandle(STD_ERROR_HANDLE,  O32_GetStdHandle(STD_ERROR_HANDLE));

    /* create handle manager instance for Open32 handles */
    HMGlobals.pHMOpen32     = new HMDeviceOpen32Class("\\\\.\\");
    HMGlobals.pHMEvent      = new HMDeviceEventClass("\\\\EVENT\\");
    HMGlobals.pHMFile       = new HMDeviceFileClass("\\\\FILE\\");
    HMGlobals.pHMInfoFile   = new HMDeviceInfoFileClass("\\\\INFOFILE\\");
    HMGlobals.pHMDisk       = new HMDeviceDiskClass("\\\\DISK\\");
    HMGlobals.pHMMutex      = new HMDeviceMutexClass("\\\\MUTEX\\");
    HMGlobals.pHMSemaphore  = new HMDeviceSemaphoreClass("\\\\SEM\\");
    HMGlobals.pHMFileMapping= new HMDeviceMemMapClass("\\\\MEMMAP\\");
    HMGlobals.pHMComm       = new HMDeviceCommClass("\\\\COM\\");
    HMGlobals.pHMToken      = new HMDeviceTokenClass("\\\\TOKEN\\");
    HMGlobals.pHMThread     = new HMDeviceThreadClass("\\\\THREAD\\");
    HMGlobals.pHMNamedPipe  = new HMDeviceNamedPipeClass("\\\\PIPE\\");
    HMGlobals.pHMMailslot   = new HMMailslotClass("\\MAILSLOT\\");
    HMGlobals.pHMParPort    = new HMDeviceParPortClass("\\\\LPT\\");
    HMGlobals.pHMNul        = new HMDeviceNulClass("\\\\NUL\\");
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
#ifdef DEBUG
  dprintf(("List of leaked handles"));
  for(int i = 0; i < MAX_OS2_HMHANDLES; i++)
  {
    /* check if handle is free */
    if (TabWin32Handles[i].hmHandleData.hHMHandle != INVALID_HANDLE_VALUE)
    {
        dprintf(("Handle %x(%p) type %x internal type %x", i, TabWin32Handles[i].hmHandleData.hHMHandle, GetFileType(i), TabWin32Handles[i].hmHandleData.dwInternalType));
    }
  }
#endif
  RasLogObjects (rthHandles, RAS_FLAG_LOG_OBJECTS);

  if(HMGlobals.pHMOpen32)
    delete HMGlobals.pHMOpen32;
  if(HMGlobals.pHMEvent)
    delete HMGlobals.pHMEvent;
  if(HMGlobals.pHMFile)
    delete HMGlobals.pHMFile;
  if(HMGlobals.pHMInfoFile)
    delete HMGlobals.pHMInfoFile;
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
  if(HMGlobals.pHMParPort)
    delete HMGlobals.pHMParPort;
  if(HMGlobals.pHMNul)
    delete HMGlobals.pHMNul;

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

  if (!phHandle16)
      return (ERROR_TOO_MANY_OPEN_FILES);
  // @@@PH 2001-09-27
  // prevent too quick re-use of last handle
  ulHandle = HMGlobals.ulHandleLast + 1;                  /* get free handle */

  handleMutex.enter();

  if(ulHandle == 0 || ulHandle >= MAX_OS2_HMHANDLES) {
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
        RasAddObject (rthHandles, ulHandle, NULL, 0);
        return (NO_ERROR);                                               /* OK */
    }

    ulHandle++;                                        /* skip to next entry */

    if (ulHandle >= MAX_OS2_HMHANDLES)                     /* check boundary */
        ulHandle = 1;
  }
  while (ulHandle != HMGlobals.ulHandleLast);

  handleMutex.leave();

#ifdef DEBUG
  dprintf(("ERROR: Out of handles!!!!"));
  for (int i = 0; i < MAX_OS2_HMHANDLES; i++)
  {
    /* check if handle is free */
    if (TabWin32Handles[i].hmHandleData.hHMHandle != INVALID_HANDLE_VALUE)
    {
        dprintf(("Handle %d type %d internal type %d", i, GetFileType(i), TabWin32Handles[i].hmHandleData.dwInternalType));
    }
  }
#endif
#ifdef RAS
  RasLog ("KERNEL32:HandleManager:HMHandleAllocate() no free handle");
  RasLogObjects (rthHandles, RAS_FLAG_LOG_OBJECTS);
#endif
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

  FREEHANDLE(hHandle16);
//  TabWin32Handles[hHandle16].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
//  RasRemoveObjects(rthHandles, hHandle16);
                                                                 /* OK, done */

  return (NO_ERROR);
}


/*****************************************************************************
 * Name      : DWORD HMHandleValidate
 * Purpose   : validate a handle through the translation table
 * Parameters: ULONG hHandle - the handle to be verified
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Wed, 1998/02/11 20:44]
 *****************************************************************************/

DWORD  HMHandleValidate (ULONG hHandle)
{
#ifdef DEBUG_LOCAL
    dprintf(("KERNEL32: HMHandleValidate (%08xh)\n", hHandle));
#endif

    if (hHandle >= MAX_OS2_HMHANDLES)                      /* check boundary */
    {
        /* Standard handle? */
        switch (hHandle)
        {
            case STD_INPUT_HANDLE:  hHandle = HMGlobals.hStandardIn; break;
            case STD_OUTPUT_HANDLE: hHandle = HMGlobals.hStandardOut; break;
            case STD_ERROR_HANDLE:  hHandle = HMGlobals.hStandardError; break;
            default:
                return(ERROR_INVALID_HANDLE);
        }
        if (hHandle >= MAX_OS2_HMHANDLES)
            return(ERROR_INVALID_HANDLE);
    }

    if (TabWin32Handles[hHandle].hmHandleData.hHMHandle == INVALID_HANDLE_VALUE)
                                                                  /* valid ? */
        return(ERROR_INVALID_HANDLE);                  /* raise error condition */

    return(NO_ERROR);
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
 * Name      : HANDLE  _HMUdptStdHandle
 * Purpose   :
 * Parameters: DWORD  nStdHandle
 *             HANDLE hHandle
 * Variables :
 * Result    : BOOL fSuccess
 * Remark    :
 * Status    :
 *
 * Author    : Dmitry Froloff [Thu, 2003/03/03 17:44]
 *****************************************************************************/

BOOL HMUpdtStdHandle(DWORD  nStdHandle, HANDLE hHandle)
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
                       DWORD   fdwOptions)
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
  if (fdwOptions & DUPLICATE_SAME_ACCESS)
      pHMHandleData->dwAccess    = TabWin32Handles[srchandle].hmHandleData.dwAccess;
  else
      pHMHandleData->dwAccess    = fdwAccess;

  pHMHandleData->dwShare       = TabWin32Handles[srchandle].hmHandleData.dwShare;
  pHMHandleData->dwCreation    = TabWin32Handles[srchandle].hmHandleData.dwCreation;
  pHMHandleData->dwFlags       = TabWin32Handles[srchandle].hmHandleData.dwFlags;
  pHMHandleData->lpHandlerData = TabWin32Handles[srchandle].hmHandleData.lpHandlerData;
  pHMHandleData->dwInternalType = TabWin32Handles[srchandle].hmHandleData.dwInternalType;


  /* we've got to mark the handle as occupied here, since another device */
  /* could be created within the device handler -> deadlock */

  /* write appropriate entry into the handle table if open succeeded */
  TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = iIndexNew;
  TabWin32Handles[iIndexNew].pDeviceHandler         = pDeviceHandler;
                                                  /* call the device handler */
  rc = pDeviceHandler->DuplicateHandle(srchandle,
                                       &TabWin32Handles[iIndexNew].hmHandleData,
                                       srcprocess,
                                       &TabWin32Handles[srchandle].hmHandleData,
                                       destprocess,
                                       fdwAccess,
                                       fInherit,
                                       fdwOptions & ~DUPLICATE_CLOSE_SOURCE, 0);

  //Don't let Open32 close it for us, but do it manually (regardless of error; see SDK docs))
  if (fdwOptions & DUPLICATE_CLOSE_SOURCE)
    HMCloseHandle(srchandle);

  if(rc == FALSE)     /* oops, creation failed within the device handler */
  {
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    FREEHANDLE(iIndexNew);
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

HANDLE HMCreateFile(LPCSTR lpFileName,
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
    // name resolving
    CHAR szFilename[260];
    if (TRUE == HandleNamesResolveName((PSZ)lpFileName,
                            szFilename,
                            sizeof(szFilename),
                                       TRUE))
    {
      // use the resolved name
      lpFileName = szFilename;
    }


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
        /* PF When create flag is set we do not care about zero in desired access
              verified in Win2k */
        if(dwDesiredAccess == 0 && dwCreationDisposition != CREATE_NEW) {
             dprintf(("File information access!"));
             pDeviceHandler = HMGlobals.pHMInfoFile;
        }
        else pDeviceHandler = HMGlobals.pHMFile;
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
    memset(&HMHandleTemp, 0, sizeof(HMHandleTemp));
    HMHandleTemp.dwAccess   = dwDesiredAccess;
    HMHandleTemp.dwShare    = dwShareMode;
    HMHandleTemp.dwCreation = dwCreationDisposition;
    HMHandleTemp.dwFlags    = dwFlagsAndAttributes;
    HMHandleTemp.hWin32Handle  = iIndexNew;
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
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;

    // Note:
    // device handlers have to return an Win32-style error code
    // from CreateFile() !
    SetLastError(rc);
    return (INVALID_HANDLE_VALUE);                           /* signal error */
  }
  else
  {
    /* copy data fields that might have been modified by CreateFile */
    memcpy(&TabWin32Handles[iIndexNew].hmHandleData,
           &HMHandleTemp,
           sizeof(HMHANDLEDATA));

    if(lpSecurityAttributes && lpSecurityAttributes->bInheritHandle) {
        dprintf(("Set inheritance for child processes"));
        HMSetHandleInformation(iIndexNew, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    }

    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?
  }

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

HFILE WIN32API OpenFile(LPCSTR lpFileName, OFSTRUCT* pOFStruct,
                  UINT      fuMode)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  VOID            *pDevData;
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  dprintf(("KERNEL32: OpenFile(%s, %08xh, %08xh)\n",
           lpFileName, pOFStruct, fuMode));

  // name resolving
  CHAR szFilename[260];
  if (TRUE == HandleNamesResolveName((PSZ)lpFileName,
                          szFilename,
                          sizeof(szFilename),
                                     TRUE))
  {
    // use the resolved name
    lpFileName = szFilename;
  }


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

  rc = pDeviceHandler->OpenFile(lpFileName,     /* call the device handler */
                                &TabWin32Handles[iIndexNew].hmHandleData,
                                pOFStruct,
                                fuMode);

#ifdef DEBUG_LOCAL
    dprintf(("KERNEL32/HandleManager:CheckPoint3: %s lpHandlerData=%08xh rc=%08xh\n",
             lpFileName,
             &TabWin32Handles[iIndexNew].hmHandleData.lpHandlerData,
             rc));
#endif

  /*
   * Based on testcase (5) and MSDN:
   *      "OF_PARSE   Fills the OFSTRUCT structure but carries out no other action."
   */
  if (fuMode & OF_PARSE) {
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
    return 0;
  }

  if(rc != NO_ERROR)     /* oops, creation failed within the device handler */
  {
    FREEHANDLE(iIndexNew);
//      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
      SetLastError(pOFStruct->nErrCode);
      return (INVALID_HANDLE_VALUE);                           /* signal error */
  }
  else {
      if(fuMode & (OF_DELETE|OF_EXIST)) {
          //file handle already closed
          FREEHANDLE(iIndexNew);
//          TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
          return TRUE; //TODO: correct?
      }

      if(fuMode & OF_VERIFY) {
          FREEHANDLE(iIndexNew);
//          TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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
 * Name      : DWORD HMGetHandleInformation
 * Purpose   : The GetHandleInformation function obtains information about certain
 *             properties of an object handle. The information is obtained as a set of bit flags.
 * Parameters: HANDLE  hObject
 *             LPDWORD lpdwFlags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMGetHandleInformation(HANDLE hObject, LPDWORD lpdwFlags)
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

  //Handle information is stored in the handle structure; return it here
  if(lpdwFlags) {
      *lpdwFlags = pHMHandle->hmHandleData.dwHandleInformation;
  }

  SetLastError(ERROR_SUCCESS);
  return TRUE;                                   /* deliver return code */
}

/*****************************************************************************
 * Name      : BOOL HMSetHandleInformation
 * Purpose   : The SetHandleInformation function sets certain properties of an
 *             object handle. The information is specified as a set of bit flags.
 * Parameters: HANDLE hObject  handle to an object
 *             DWORD  dwMask   specifies flags to change
 *             DWORD  dwFlags  specifies new values for flags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL   HMSetHandleInformation       (HANDLE hObject,
                                     DWORD  dwMask,
                                     DWORD  dwFlags)
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
  //SvL: Check if pDeviceHandler is set
  if (pHMHandle->pDeviceHandler)
  {
    fResult = pHMHandle->pDeviceHandler->SetHandleInformation(&pHMHandle->hmHandleData,
                                                              dwMask, dwFlags);
  }
  else
  {
    dprintf(("HMSetHandleInformation(%08xh): pDeviceHandler not set", hObject));
    fResult = TRUE;
  }

  if(fResult == TRUE) {
      SetLastError(ERROR_SUCCESS);
  }
  return (fResult);                                   /* deliver return code */
}

/*****************************************************************************
 * Name      : HANDLE  HMGetFileNameFromHandle
 * Purpose   : Query the name of the file associated with the system handle (if any)
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : SvL
 *****************************************************************************/
BOOL HMGetFileNameFromHandle(HANDLE hObject, LPSTR lpszFileName, DWORD cbFileName)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      fResult;       /* result from the device handler's CloseHandle() */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */


  if(lpszFileName == NULL) {
      DebugInt3();
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }
                                                          /* validate handle */
  iIndex = _HMHandleQuery(hObject);                         /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return (FALSE);                                        /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */
  //SvL: Check if pDeviceHandler is set
  if (pHMHandle->pDeviceHandler)
  {
    fResult = pHMHandle->pDeviceHandler->GetFileNameFromHandle(&pHMHandle->hmHandleData,
                                                               lpszFileName, cbFileName);
  }
  else
  {
    dprintf(("HMGetFileNameFromHandle(%08xh): pDeviceHandler not set", hObject));
    fResult = FALSE;
  }

  if(fResult == TRUE) {
      SetLastError(ERROR_SUCCESS);
  }
  return (fResult);                                   /* deliver return code */
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

  if(pHMHandle->hmHandleData.dwHandleInformation & HANDLE_FLAG_PROTECT_FROM_CLOSE) {
      dprintf(("Handle not closed because of HANDLE_FLAG_PROTECT_FROM_CLOSE"));
      SetLastError(ERROR_SUCCESS);
      return TRUE;
  }

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
    FREEHANDLE(iIndex);
//    pHMHandle->hmHandleData.hHMHandle = INVALID_HANDLE_VALUE; /* mark handle as free */
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
                LPOVERLAPPED lpOverlapped,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  fResult = pHMHandle->pDeviceHandler->ReadFile(&pHMHandle->hmHandleData,
                                                lpBuffer,
                                                nNumberOfBytesToRead,
                                                lpNumberOfBytesRead,
                                                lpOverlapped, lpCompletionRoutine);

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
                 LPOVERLAPPED lpOverlapped,
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

  /* watcom hack:
   * watcom may write -1 bytes to a device during flushall(), we should return
   * ERROR_NOT_ENOUGH_MEMORY or ERROR_NO_ACCESS
   */
  if (nNumberOfBytesToWrite == 0xffffffff)
  {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return FALSE;
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  fResult = pHMHandle->pDeviceHandler->WriteFile(&pHMHandle->hmHandleData,
                                                 lpBuffer,
                                                 nNumberOfBytesToWrite,
                                                 lpNumberOfBytesWritten,
                                                 lpOverlapped, lpCompletionRoutine);

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

DWORD WIN32API GetFileType(HANDLE hFile)
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API GetFileInformationByHandle (HANDLE                     hFile,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API SetEndOfFile (HANDLE hFile)
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API SetFileTime (HANDLE         hFile,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API GetFileTime (HANDLE hFile, FILETIME *pFT1,
                           FILETIME *pFT2, FILETIME *pFT3)
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

DWORD WIN32API GetFileSize (HANDLE hFile, PDWORD pSize)
{
  int       iIndex;                           /* index into the handle table */
  DWORD     dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return -1; //INVALID_SET_FILE_POINTER
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  dwResult = pHMHandle->pDeviceHandler->GetFileSize(&pHMHandle->hmHandleData,
                                                    pSize);

  return (dwResult);                                  /* deliver return code */
}

/***********************************************************************
 *           GetFileSizeEx   (KERNEL32.@)
 */
#define INVALID_FILE_SIZE        ((DWORD)~0UL)
BOOL WINAPI GetFileSizeEx( HANDLE hFile, PLARGE_INTEGER lpFileSize )
{
    lpFileSize->LowPart = GetFileSize(hFile, ((PDWORD)&lpFileSize->HighPart));
    return (lpFileSize->LowPart != INVALID_FILE_SIZE);
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

DWORD WIN32API SetFilePointer (HANDLE hFile,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  dwResult = pHMHandle->pDeviceHandler->SetFilePointer(&pHMHandle->hmHandleData,
                                                       lDistanceToMove,
                                                       lpDistanceToMoveHigh,
                                                       dwMoveMethod);

  return (dwResult);                                  /* deliver return code */
}

/***********************************************************************
 *           SetFilePointerEx   (KERNEL32.@)
 */
BOOL WINAPI SetFilePointerEx( HANDLE hFile, LARGE_INTEGER distance,
                              LARGE_INTEGER *newpos, DWORD method )
{
    LONG pos;
    LONG newp;
    BOOL res;

    dprintf(("KERNEL32::SetFilePointerEx"));

    pos = (LONG)distance.LowPart;
    res = SetFilePointer(hFile, pos, &newp, method);
    if (res != -1)
    {
        if (newpos) newpos->LowPart = (LONG)res;
        return TRUE;
    }
    return FALSE;
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

BOOL WIN32API LockFile (HANDLE        hFile,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  dwResult = pHMHandle->pDeviceHandler->LockFile(&pHMHandle->hmHandleData,
                                                 arg2,
                                                 arg3,
                                                 arg4,
                                                 arg5);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : BOOL LockFileEx
 * Purpose   : The LockFileEx function locks a byte range within an open file for shared or exclusive access.
 * Parameters: HANDLE hFile                     handle of file to lock
 *             DWORD  dwFlags                   functional behavior modification flags
 *             DWORD  dwReserved                reserved, must be set to zero
 *             DWORD  nNumberOfBytesToLockLow   low-order 32 bits of length to lock
 *             DWORD  nNumberOfBytesToLockHigh  high-order 32 bits of length to lock
 *             LPOVERLAPPED  LPOVERLAPPED       addr. of structure with lock region start offset
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API LockFileEx(HANDLE        hFile,
                  DWORD         dwFlags,
                  DWORD         dwReserved,
                  DWORD         nNumberOfBytesToLockLow,
                  DWORD         nNumberOfBytesToLockHigh,
                  LPOVERLAPPED  lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API UnlockFile (HANDLE        hFile,
                   DWORD         arg2,
                   DWORD         arg3,
                   DWORD         arg4,
                   DWORD         arg5)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  dwResult = pHMHandle->pDeviceHandler->UnlockFile(&pHMHandle->hmHandleData,
                                                   arg2,
                                                   arg3,
                                                   arg4,
                                                   arg5);

  return (dwResult);                                  /* deliver return code */
}


/*****************************************************************************
 * Name      : BOOL UnlockFileEx
 * Purpose   : The UnlockFileEx function unlocks a previously locked byte range in an open file.
 * Parameters: HANDLE hFile                     handle of file to lock
 *             DWORD  dwReserved                reserved, must be set to zero
 *             DWORD  nNumberOfBytesToLockLow   low-order 32 bits of length to lock
 *             DWORD  nNumberOfBytesToLockHigh  high-order 32 bits of length to lock
 *             LPOVERLAPPED  LPOVERLAPPED       addr. of structure with lock region start offset
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UnlockFileEx(HANDLE        hFile,
                    DWORD         dwReserved,
                    DWORD         nNumberOfBytesToLockLow,
                    DWORD         nNumberOfBytesToLockHigh,
                    LPOVERLAPPED  lpOverlapped)
{
  int       iIndex;                           /* index into the handle table */
  BOOL      dwResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  iIndex = _HMHandleQuery(hFile);                           /* get the index */
  if (-1 == iIndex)                                               /* error ? */
  {
    SetLastError(ERROR_INVALID_HANDLE);       /* set win32 error information */
    return FALSE;                         /* signal failure */
  }

  pHMHandle = &TabWin32Handles[iIndex];               /* call device handler */

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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
  BOOL      dwResult;                /* result from the device handler's API */
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API FlushFileBuffers(HANDLE hFile)
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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

BOOL WIN32API ReleaseMutex(HANDLE hObject)
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  dwResult = pHMHandle->pDeviceHandler->ReleaseMutex(&pHMHandle->hmHandleData);

  return (dwResult);                                  /* deliver return code */
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

HANDLE WIN32API CreateMutexA(LPSECURITY_ATTRIBUTES lpsa,
                             BOOL                  bInitialOwner,
                     LPCTSTR               lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  if(lpName) { //check if shared mutex semaphore already exists
      dprintf(("Event semaphore name %s", lpName));

      //TODO: No inheritance??
      HANDLE handle = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, lpName);
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
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_MUTEXSEM;


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
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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

HANDLE WIN32API OpenMutexA(DWORD   fdwAccess,
                           BOOL    fInherit,
                     LPCTSTR               lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  if(lpName) {
      dprintf(("Mutex semaphore name %s", lpName));
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
  pHMHandleData->dwAccess   = fdwAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_MUTEXSEM;


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
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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

HANDLE WIN32API CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpsa,
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
      dprintf(("Semaphore name %s", lpName));

      HANDLE handle = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, lpName);
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
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_SEMAPHORE;


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
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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

HANDLE WIN32API OpenSemaphoreA(DWORD   fdwAccess,
                       BOOL    fInherit,
                       LPCTSTR lpName)
{
  int             iIndex;                     /* index into the handle table */
  int             iIndexNew;                  /* index into the handle table */
  HMDeviceHandler *pDeviceHandler;         /* device handler for this handle */
  PHMHANDLEDATA   pHMHandleData;
  DWORD           rc;                                     /* API return code */


  if(lpName) {
      dprintf(("Semaphore name %s", lpName));
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
  pHMHandleData->dwAccess   = fdwAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_SEMAPHORE;


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
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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

BOOL WIN32API ReleaseSemaphore(HANDLE hEvent,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

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
  pHMHandleData->dwAccess   = 0;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_MEMMAP;

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
          FREEHANDLE(iIndexNew);
//          TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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
    FREEHANDLE(iIndexNew);
//    TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return NULL;

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
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
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
////      SetLastError(ERROR_INVALID_HANDLE);
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
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
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
////      SetLastError(ERROR_INVALID_HANDLE);
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

BOOL WIN32API DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode,
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  fResult = pHMHandle->pDeviceHandler->DeviceIoControl(&pHMHandle->hmHandleData,
                                                dwIoControlCode,
                                                lpInBuffer, nInBufferSize,
                                                lpOutBuffer, nOutBufferSize,
                                                lpBytesReturned, lpOverlapped);

  return (fResult);                                   /* deliver return code */
}
/*****************************************************************************
 * Name      : BOOL WIN32API CancelIo
 * Purpose   : The CancelIO function cancels all pending input and output
 *             (I/O) operations that were issued by the calling thread for
 *             the specified file handle. The function does not cancel
 *             I/O operations issued for the file handle by other threads.
 * Parameters: HANDLE hFile   file handle for which to cancel I/O
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero All pending
 *             I/O operations issued by the calling thread for the file handle
 *             were successfully canceled.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : If there are any I/O operations in progress for the specified
 *             file HANDLE and they were issued by the calling thread, the
 *             CancelIO function cancels them.
 *             Note that the I/O operations must have been issued as
 *             overlapped I/O. If they were not, the I/O operations would not
 *             have returned to allow the thread to call the CancelIO function.
 *             Calling the CancelIO function with a file handle that was not
 *             opened with FILE_FLAG_OVERLAPPED does nothing.
 *             All I/O operations that are canceled will complete with the
 *             error ERROR_OPERATION_ABORTED. All completion notifications
 *             for the I/O operations will occur normally.
 * Status    :
 *
 * Author    : Sander van Leeuwen
 *****************************************************************************/
BOOL WIN32API CancelIo(HANDLE hDevice)
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

  if (!pHMHandle || !pHMHandle->pDeviceHandler)
      return ERROR_SYS_INTERNAL;

  fResult = pHMHandle->pDeviceHandler->CancelIo(&pHMHandle->hmHandleData);

  return (fResult);                                   /* deliver return code */
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
  int       iIndex;                           /* index into the handle table */
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
  pHMHandleData->dwAccess   = DesiredAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_THREADTOKEN;


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
    FREEHANDLE(iIndexNew);
//      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
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
  int       iIndex;                           /* index into the handle table */
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
  pHMHandleData->dwAccess   = DesiredAccess;
  pHMHandleData->dwShare    = 0;
  pHMHandleData->dwCreation = 0;
  pHMHandleData->dwFlags    = 0;
  pHMHandleData->lpHandlerData = NULL;
  pHMHandleData->dwInternalType = HMTYPE_PROCESSTOKEN;


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
    FREEHANDLE(iIndexNew);
//      TabWin32Handles[iIndexNew].hmHandleData.hHMHandle = INVALID_HANDLE_VALUE;
  return (rc);                                           /* signal error */
  }
  else
    SetLastError(ERROR_SUCCESS); //@@@PH 1999/10/27 rc5desg requires this?

  *TokenHandle = iIndexNew;                                   /* return valid handle */
  return STATUS_SUCCESS;
}


/**
 * Gets the type of an object.
 *
 * @returns the value of one of the HMMTYPE_ defines.
 * @returns HMTYPE_BAD_HANDLE if hObject isn't a valid handle.
 * @param   hObject     Handle to object.
 */
unsigned WIN32API HMQueryObjectType(HANDLE hObject)
{
    PHMHANDLEDATA   pData = HMQueryHandleData(hObject);
    if (pData)
        return (unsigned)pData->dwInternalType;
    return HMTYPE_BAD_HANDLE;
}

