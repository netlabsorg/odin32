#define INCL_DOSSEMAPHORES
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS

#include <os2wrap.h>
#include "srbos2.h"
#include <odinaspi.h>
#include "aspi.h"

BYTE bOpenASPI(PODINASPIDATA pData)
{
  BOOL  success;
  ULONG rc;
  ULONG  ulParam, ulReturn;                          // return value
  USHORT usDrvReturn;                                // return value

  rc = DosAllocMem( &pData->pvBuffer,
                   65536,
                   OBJ_TILE | PAG_READ | PAG_WRITE | PAG_COMMIT);
  if(rc)
    return FALSE;
  rc = DosOpen( (PSZ) "aspirou$",                    // open driver
                (HFILE*)&pData->hfDriver,
                &ulReturn,
                0,
                0,
                FILE_OPEN,
                OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE,
                NULL);
  if(rc)
  {
    DosFreeMem( pData->pvBuffer);
    return SS_NO_ASPI;    // opening failed -> return false
  }

  rc = DosCreateEventSem( NULL,
                          (HEV*)&pData->hevPost,     // create event semaphore
                          DC_SEM_SHARED,
                          0);
  if(rc)
  {
    DosFreeMem( pData->pvBuffer); // Free Mem
    DosClose((HFILE)pData->hfDriver);    // Close the driver
    return SS_ERR;                 // DosCreateEventSem failed
  }
  rc = DosDevIOCtl( (HFILE) pData->hfDriver,
                    0x92,
                    0x03,                     // pass semaphore handle
                    (void*) &pData->hevPost,
                    sizeof(HEV),              // to driver
                    &ulParam,
                    (void*) &usDrvReturn,
                    sizeof(USHORT),
                    &ulReturn);

  if(rc)
  {
    DosFreeMem( pData->pvBuffer);     // Free Mem
    DosCloseEventSem((HEV)pData->hevPost); // close event semaphore
    DosClose((HFILE)pData->hfDriver);        // Close the driver

    return SS_ERR;                     // Error passing Sem to driver
  }

  rc = DosDevIOCtl( (HFILE)pData->hfDriver,
                    0x92,
                    0x04,                    // pass buffer pointer
                    (void*) pData->pvBuffer,
                    sizeof(PVOID),           // to driver
                    &ulParam,
                    (void*) &usDrvReturn,
                    sizeof(USHORT),
                    &ulReturn);

  if(rc)
  {
    DosFreeMem( pData->pvBuffer);     // Free Mem
    DosCloseEventSem((HEV)pData->hevPost); // close event semaphore
    DosClose((HFILE)pData->hfDriver);        // Close the driver

    return SS_ERR;                     // Error passing Sem to driver
  }

  return SS_COMP;
}

BOOL fCloseASPI( PODINASPIDATA pData)
{
  ULONG rc;
  BOOL frc = TRUE;

  rc = DosCloseEventSem((HEV)pData->hevPost); // close event semaphore
  if(NO_ERROR!=rc)
    frc=FALSE;

  rc = DosClose((HFILE)pData->hfDriver);        // Close the driver
  if(NO_ERROR!=rc)
    frc=FALSE;

  rc = DosFreeMem( pData->pvBuffer);     // Free Mem
  if(NO_ERROR!=rc)
    frc=FALSE;

  return frc;
}

BOOL fWaitPost(ULONG postSema)
{
  ULONG count=0;
  ULONG rc;                                   // return value

  rc = DosWaitEventSem( (HEV)postSema,
                        SEM_INDEFINITE_WAIT); // wait forever
  if(rc)
    return FALSE;                             // DosWaitEventSem failed

  rc = DosResetEventSem( (HEV)postSema,
                         &count);             // reset semaphore
  if(rc)
    return FALSE;                             // DosResetEventSem failed

  return TRUE;
}

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

LONG lSendSRBlock( ULONG ulFile,
                   SRBOS2 *pSRBIn,
                   SRBOS2 *pSRBOut)
{
  ULONG ulParam, ulReturn;
  return DosDevIOCtl( (HFILE) ulFile,
                      0x92,
                      0x02,
                      (void*) &pSRBIn,
                      sizeof(SRBOS2),
                      &ulParam,
                      (void*) pSRBOut,
                      sizeof(SRBOS2),
                      &ulReturn);

}

