//***************************************************************************
//*                                                                         *
//*  ASPI Router Library                                                    *
//*                                                                         *
//*  This is a sample library which shows how to send SRB's to the          *
//*  ASPI Router device driver. USE AT YOUR OWN RISK!!                      *
//*                                                                         *
//*  Version 1.01 - June 1997                                               *
//*                                                                         *
//*  Changes since 1.00:                                                    *
//*  abort(), AbortSRB added                                                *
//*                                                                         *
//***************************************************************************
#ifndef _ASPILIB_H_
#define _ASPILIB_H_

//#include <os2wrap.h>
#include "srbos2.h"

class scsiObj
{
  private:
    ULONG       postSema;               // Event Semaphore for posting SRB completion
    ULONG       driver_handle;          // file handle for device driver
    BOOL        initSemaphore();
    BOOL        closeSemaphore();
    BOOL        openDriver();
    BOOL        closeDriver();
    BOOL        initBuffer();
    ULONG       hmtxDriver;

  public:
    scsiObj();
    ~scsiObj();
    BOOL        init(ULONG bufsize);
    BOOL        close();
    ULONG       rewind(UCHAR id, UCHAR lun);
    ULONG       read(UCHAR id, UCHAR lun, ULONG transfer);
    ULONG       locate(UCHAR id, UCHAR lun, ULONG block);
    ULONG       unload(UCHAR id, UCHAR lun);
    ULONG       write(UCHAR id, UCHAR lun, ULONG transfer);
    ULONG       write_filemarks(UCHAR id, UCHAR lun, BOOL setmark, ULONG count);
    ULONG       space(UCHAR id, UCHAR lun, UCHAR code, ULONG count);
    ULONG       read_position(UCHAR id, UCHAR lun, ULONG* pos, ULONG* partition, BOOL* BOP, BOOL* EOP);
    ULONG       HA_inquiry(UCHAR ha);
    ULONG       getDeviceType(UCHAR ha_num, UCHAR id, UCHAR lun);
    ULONG       testUnitReady(UCHAR id, UCHAR lun);
    ULONG       resetDevice(UCHAR ha_num,UCHAR id, UCHAR lun);
    ULONG       abort();
    BOOL        waitPost();
    ULONG       SendSRBlock(VOID);
    ULONG       getNumHosts();
    BOOL        access(BOOL fWait);
    BOOL        release();


    SRBOS2      SRBlock;                // SCSI Request Block
    SRBOS2      AbortSRB;               // Abort SRB
    PVOID       buffer;                 // Our data buffer
};

extern scsiObj *aspi;

#endif // _ASPILIB_H_