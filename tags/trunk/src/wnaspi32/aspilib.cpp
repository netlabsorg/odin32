//***************************************************************************
//*                                                                         *
//*  ASPI Router Library                                                    *
//*  for Odin WNASPI32.DLL                                                  *
//*                                                                         *
//*  This is a sample library which shows how to send SRB's to the          *
//*  ASPI Router device driver. USE AT YOUR OWN RISK!!                      *
//*                                                                         *
//*  Copyright (C) 1997 Daniel Dorau                                        *
//*  Copyright (C) 2000 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>       *
//*                                                                         *
//***************************************************************************
#define      INCL_DOSFILEMGR
#define      INCL_DOSDEVICES
#define      INCL_DOSDEVIOCTL
#define      INCL_DOSSEMAPHORES
#define      INCL_DOSMEMMGR
#define      INCL_DOSERRORS
#include     <os2wrap.h>
#include     <string.h>
#include     <stdio.h>
#include     <misc.h>
#include "aspilib.h"

//***************************************************************************
//*                                                                         *
//*  scsiObj()                                                              *
//*                                                                         *
//*  Standard constructor                                                   *
//*                                                                         *
//***************************************************************************
scsiObj::scsiObj()
{
}


//***************************************************************************
//*                                                                         *
//*  ~scsiObj()                                                             *
//*                                                                         *
//*  Standard destructor                                                    *
//*                                                                         *
//***************************************************************************
scsiObj::~scsiObj()
{
}


//***************************************************************************
//*                                                                         *
//*  BOOL openDriver()                                                      *
//*                                                                         *
//*  Opens the ASPI Router device driver and sets device_handle.            *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful opening of device driver                        *
//*                                                                         *
//*  Preconditions: ASPI Router driver has be loaded                        *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::openDriver()
{
  ULONG rc;                                             // return value
  ULONG ActionTaken;                                    // return value

  rc = DosOpen((PSZ) "aspirou$",                        // open driver
               &driver_handle,
               &ActionTaken,
               0,
               0,
               FILE_OPEN,
               OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE,
               NULL);
  if (rc) return FALSE;                                 // opening failed -> return false
  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL closeDriver()                                                     *
//*                                                                         *
//*  Closes the device driver                                               *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful closing of device driver                        *
//*                                                                         *
//*  Preconditions: ASPI Router driver has be opened with openDriver        *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::closeDriver()
{
  ULONG rc;                                             // return value

  rc = DosClose(driver_handle);
  if (rc) return FALSE;                                 // closing failed -> return false
  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL initSemaphore()                                                   *
//*                                                                         *
//*  Creates a new Event Semaphore and passes its handle to ASPI Router.    *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful creation of event semaphore                     *
//*                                                                         *
//*  Preconditions: driver_handle has to be set with openDriver             *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::initSemaphore()
{
  ULONG  rc;                                            // return value
  USHORT openSemaReturn;                                // return value
  unsigned long cbreturn;
  unsigned long cbParam;

  rc = DosCreateEventSem(NULL, &postSema,               // create event semaphore
                         DC_SEM_SHARED, 0);
  if (rc) return FALSE;                                 // DosCreateEventSem failed
  rc = DosDevIOCtl(driver_handle, 0x92, 0x03,           // pass semaphore handle
                   (void*) &postSema, sizeof(HEV),      // to driver
                   &cbParam, (void*) &openSemaReturn,
                   sizeof(USHORT), &cbreturn);
  if (rc) return FALSE;                                 // DosDevIOCtl failed
  if (openSemaReturn) return FALSE;                     // Driver could not open semaphore

  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL closeSemaphore()                                                  *
//*                                                                         *
//*  Closes the Event Semaphore                                             *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful closing of event semaphore                      *
//*                                                                         *
//*  Preconditions: init_Semaphore has to be called successfully before     *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::closeSemaphore()
{
  ULONG  rc;                                            // return value

  rc = DosCloseEventSem(postSema);                      // close event semaphore
  if (rc) return FALSE;                                 // DosCloseEventSem failed
  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL initBuffer()                                                      *
//*                                                                         *
//*  Sends the address of the data buffer to ASPI Router so that it can     *
//*  lock down the segment.                                                 *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful locking of buffer segment                       *
//*                                                                         *
//*  Preconditions: (called from init())                                    *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::initBuffer()
{
  ULONG  rc;                                            // return value
  USHORT lockSegmentReturn;                             // return value
  unsigned long cbreturn;
  unsigned long cbParam;

  rc = DosDevIOCtl(driver_handle, 0x92, 0x04,           // pass buffer pointer
                   (void*) buffer, sizeof(PVOID),       // to driver
                   &cbParam, (void*) &lockSegmentReturn,
                   sizeof(USHORT), &cbreturn);
  if (rc) return FALSE;                                 // DosDevIOCtl failed
  if (lockSegmentReturn) return FALSE;                  // Driver could not lock segment

  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL init(ULONG bufsize)                                               *
//*                                                                         *
//*  This inits the ASPI library and ASPI router driver.                    *
//*  Allocates the data buffer and passes its address to the driver         *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful initialization of driver and library            *
//*                                                                         *
//*  Preconditions: ASPI router device driver has to be loaded              *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::init(ULONG bufsize)
{
  BOOL  success;
  ULONG rc;

  rc = DosAllocMem(&buffer, bufsize, OBJ_TILE | PAG_READ | PAG_WRITE | PAG_COMMIT);
  if (rc) return FALSE;
  success=openDriver();                         // call openDriver member function
  if (!success) return FALSE;
  success=initSemaphore();                      // call initSemaphore member function
  if (!success) return FALSE;

  success=initBuffer();

  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL close()                                                           *
//*                                                                         *
//*  This closes the ASPI library and ASPI router driver and frees          *
//*  the memory allocated for the data buffer.
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful closing of library and driver                   *
//*                                                                         *
//*  Preconditions: init() should be called successfully before             *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::close()
{
  BOOL success;
  ULONG rc;

  success=closeSemaphore();                     // call closeSemaphore member function
  if (!success)
  {
    printf("closeSemaphore() unsuccessful.\n");
    return FALSE;
  }
  success=closeDriver();                        // call closeDriver member function
  if (!success)
  {
    return FALSE;
    printf("closeDriver() unsucessful.\n");
  }
  rc = DosFreeMem(buffer);
  if (rc)
  {
    printf("DosFreeMem unsuccessful. return code: %ld\n", rc);
    return FALSE;
  }
  return TRUE;
}


//***************************************************************************
//*                                                                         *
//*  BOOL waitPost()                                                        *
//*                                                                         *
//*  Waits for postSema being posted by device driver                       *
//*  Returns:                                                               *
//*    TRUE - Success                                                       *
//*    FALSE - Unsuccessful access of event semaphore                       *
//*                                                                         *
//*  Preconditions: init() has to be called successfully before             *
//*                                                                         *
//***************************************************************************
BOOL scsiObj::waitPost()
{
  ULONG count=0;
  ULONG rc;                                             // return value

  rc = DosWaitEventSem(postSema, -1);                   // wait forever
  if (rc) return FALSE;                                 // DosWaitEventSem failed
  rc = DosResetEventSem(postSema, &count);              // reset semaphore
  if (rc) return FALSE;                                 // DosResetEventSem failed
  return TRUE;
}

//***************************************************************************
//*                                                                         *
//*  ULONG HA_inquiry(UCHAR ha)                                             *
//*                                                                         *
//*  Sends a SRB containing a Host Adapter Inquiry command                  *
//*  Returns:                                                               *
//*    0  - Success                                                         *
//*    1  - DevIOCtl failed                                                 *
//*    2  - Host Adapter not installed                                      *
//*                                                                         *
//*  Preconditions: driver has to be opened                                 *
//*                                                                         *
//***************************************************************************
ULONG scsiObj::HA_inquiry(UCHAR ha)
{
  ULONG rc;                                     // return value
  unsigned long cbreturn;
  unsigned long cbParam;

  SRBlock.cmd=SRB_Inquiry;                      // host adapter inquiry
  SRBlock.ha_num=ha;                            // host adapter number
  SRBlock.flags=0;                              // no flags set

  rc = DosDevIOCtl(driver_handle, 0x92, 0x02, (void*) &SRBlock, sizeof(SRBOS2), &cbParam,
                  (void*) &SRBlock, sizeof(SRBOS2), &cbreturn);
  if (rc)
    return 1;                                   // DosDevIOCtl failed
  if (SRBlock.status != SRB_Done) return 2;
  return 0;
}


//***************************************************************************
//*                                                                         *
//*  ULONG getDeviceType(UCHAR id, UCHAR lun)                               *
//*                                                                         *
//*  Sends a SRB containing a Get Device Type command                       *
//*  Returns:                                                               *
//*    0  - Success                                                         *
//*    1  - DevIOCtl failed                                                 *
//*    2  - Device not installed                                            *
//*                                                                         *
//*  Preconditions: driver has to be opened                                 *
//*                                                                         *
//***************************************************************************
ULONG scsiObj::getDeviceType(UCHAR ha_num, UCHAR id, UCHAR lun)
{
  ULONG rc;                                     // return value
  unsigned long cbreturn;
  unsigned long cbParam;

  SRBlock.cmd=SRB_Device;                       // get device type
  SRBlock.ha_num=ha_num;                        // host adapter number
  SRBlock.flags=0;                              // no flags set
  SRBlock.u.dev.target=id;                      // target id
  SRBlock.u.dev.lun=lun;                        // target LUN

  rc = DosDevIOCtl(driver_handle, 0x92, 0x02, (void*) &SRBlock, sizeof(SRBOS2), &cbParam,
                  (void*) &SRBlock, sizeof(SRBOS2), &cbreturn);
  if (rc)
    return 1;                                   // DosDevIOCtl failed
  if (SRBlock.status != SRB_Done) return 2;
  return 0;
}


//***************************************************************************
//*                                                                         *
//*  ULONG resetDevice(UCHAR id, UCHAR lun)                                 *
//*                                                                         *
//*  Sends a SRB containing a Reset Device command                          *
//*  Returns:                                                               *
//*    0  - Success                                                         *
//*    1  - DevIOCtl failed                                                 *
//*    2  - Semaphore access failure                                        *
//*    3  - SCSI command failed                                             *
//*                                                                         *
//*  Preconditions: init() has to be called successfully before             *
//*                                                                         *
//***************************************************************************
ULONG scsiObj::resetDevice(UCHAR ha_num,UCHAR id, UCHAR lun)
{
  ULONG rc;                                     // return value
  unsigned long cbreturn;
  unsigned long cbParam;
  BOOL  success;

  SRBlock.cmd=SRB_Reset;                        // reset device
  SRBlock.ha_num=ha_num;                        // host adapter number
  SRBlock.flags=SRB_Post;                       // posting enabled
  SRBlock.u.res.target=id;                      // target id
  SRBlock.u.res.lun=lun;                        // target LUN

  rc = DosDevIOCtl(driver_handle, 0x92, 0x02, (void*) &SRBlock, sizeof(SRBOS2), &cbParam,
                  (void*) &SRBlock, sizeof(SRBOS2), &cbreturn);
  if (rc)
    return 1;                                   // DosDevIOCtl failed
  else
  {
    success=waitPost();                         // wait for SRB being processed
    if (!success) return 2;                     // semaphore could not be accessed
  }
  if (SRBlock.status != SRB_Done) return 3;
  return 0;
}


//***************************************************************************
//*                                                                         *
//*  ULONG abort()                                                          *
//*                                                                         *
//*  Sends a SRB containing a Get Device Type command                       *
//*  Returns:                                                               *
//*    0  - Success                                                         *
//*    1  - DevIOCtl failed                                                 *
//*    2  - Abort SRB not successful                                        *
//*                                                                         *
//*  Preconditions: driver has to be opened                                 *
//*                                                                         *
//***************************************************************************
ULONG scsiObj::abort()
{
  ULONG rc;                                     // return value
  unsigned long cbreturn;
  unsigned long cbParam;

  AbortSRB.cmd=SRBOS2_Abort;                       // abort SRB
  AbortSRB.ha_num=0;                            // host adapter number
  AbortSRB.flags=0;                             // no flags set
  AbortSRB.u.abt.srb=&SRBlock;                  // SRB to abort

  rc = DosDevIOCtl(driver_handle, 0x92, 0x02, (void*) &AbortSRB, sizeof(SRBOS2), &cbParam,
                  (void*) &AbortSRB, sizeof(SRBOS2), &cbreturn);
  if (rc)
    return 1;                                   // DosDevIOCtl failed
  if (SRBlock.status != SRB_Done) return 2;
  return 0;
}
//***************************************************************************
//***************************************************************************
ULONG scsiObj::getNumHosts()
{ int i,j=0;
  ULONG rc;

  for (i=0;i<15;i++)
  {
    rc=HA_inquiry(i);
    if (rc==0) j++;
  }
  return j;
}
//***************************************************************************
//***************************************************************************
ULONG scsiObj::SendSRBlock(VOID)
{
  ULONG ulParam, ulReturn;

  return DosDevIOCtl( (HFILE) driver_handle,
                      0x92,
                      0x02,
                      (void*) &SRBlock,
                      sizeof(SRBOS2),
                      &ulParam,
                      (void*) &SRBlock,
                      sizeof(SRBOS2),
                      &ulReturn);
}
//***************************************************************************
//***************************************************************************
BOOL fGainDrvAccess( BOOL fWait,
                     ULONG *phSem)
{
  ULONG rc;
  rc = DosCreateMutexSem ( "\\SEM32\\ODIN\\ASPIROUT",
                           (HMTX*)phSem,
                           0,
                           TRUE);
  if(NO_ERROR==rc)
    return TRUE;
  if((ERROR_DUPLICATE_NAME!=rc)||(!fWait))
    return FALSE;
  rc = DosOpenMutexSem ( "\\SEM32\\ODIN\\ASPIROUT",
                         (HMTX*)phSem);
  if(NO_ERROR!=rc)
    return FALSE;

  rc = DosRequestMutexSem( (HMTX)(*phSem),
                           SEM_INDEFINITE_WAIT);

  if(NO_ERROR!=rc)
    return FALSE;

  return TRUE;
}
//***************************************************************************
//***************************************************************************
BOOL fReleaseDrvAccess(ULONG hSem)
{
  ULONG rc;
  BOOL frc = TRUE;

  rc = DosReleaseMutexSem((HMTX)hSem);
  if(NO_ERROR!=rc)
    frc=FALSE;

  rc = DosCloseMutexSem((HMTX)hSem);
  if(NO_ERROR!=rc)
    frc=FALSE;

  return frc;
}
