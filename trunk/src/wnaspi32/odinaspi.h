

#ifndef __ODINASPI_H
  #define __ODINASPI_H

typedef struct
{
  ULONG       hfDriver;          // file handle for device driver
  ULONG       hevPost;               // Event Semaphore for posting SRB completion
  PVOID       pvBuffer;                 // Our data buffer
} ODINASPIDATA, *PODINASPIDATA;

extern LONG lSendSRBlock( ULONG ulFile,
                          SRBOS2 *pSRBIn,
                          SRBOS2 *pSRBOut);

extern BYTE bOpenASPI(PODINASPIDATA pData);
extern BOOL fCloseASPI( PODINASPIDATA pData);
extern BOOL fWaitPost(ULONG postSema);
extern BOOL fGainDrvAccess( BOOL fWait,
                            ULONG *phSem);
extern BOOL fReleaseDrvAccess(ULONG hSem);

#endif
