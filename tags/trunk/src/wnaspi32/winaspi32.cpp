#include <os2win.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
// #include <unistd.h>

#include "aspi.h"
#include "wnaspi32.h"
#include <winreg.h>
#include "options.h"
#include "debugtools.h"

#include "srbos2.h"
#include "odinaspi.h"
//#include "callback.h"

DEFAULT_DEBUG_CHANNEL(aspi)


/* FIXME!
 * 1) Residual byte length reporting not handled
 * 2) Make this code re-entrant for multithreading
 * 3) Only linux supported so far
 */

#ifdef linux

static ASPI_DEVICE_INFO *ASPI_open_devices = NULL;

static int
ASPI_OpenDevice(SRB_ExecSCSICmd *prb)
{
    int fd;
    char        idstr[20];
    char        device_str[50];
    ASPI_DEVICE_INFO *curr;

    /* search list of devices to see if we've opened it already.
     * There is not an explicit open/close in ASPI land, so hopefully
     * keeping a device open won't be a problem.
     */

    for (curr = ASPI_open_devices; curr; curr = curr->next) {
        if (curr->hostId == prb->SRB_HaId &&
            curr->target == prb->SRB_Target &&
            curr->lun == prb->SRB_Lun) {
            return curr->fd;
        }
    }

    /* device wasn't cached, go ahead and open it */
    sprintf(idstr, "scsi c%1dt%1dd%1d", prb->SRB_HaId, prb->SRB_Target, prb->SRB_Lun);

    if (!PROFILE_GetWineIniString(idstr, "Device", "", device_str, sizeof(device_str))) {
        TRACE("Trying to open unlisted scsi device %s\n", idstr);
        return -1;
    }

    TRACE("Opening device %s=%s\n", idstr, device_str);

    fd = open(device_str, O_RDWR);
    if (fd == -1) {
        int save_error = errno;
#ifdef HAVE_STRERROR
    ERR("Error opening device %s, error '%s'\n", device_str, strerror(save_error));
#else
    ERR("Error opening device %s, error %d\n", device_str, save_error);
#endif
        return -1;
    }

    /* device is now open */
    curr = HeapAlloc( SystemHeap, 0, sizeof(ASPI_DEVICE_INFO) );
    curr->fd = fd;
    curr->hostId = prb->SRB_HaId;
    curr->target = prb->SRB_Target;
    curr->lun = prb->SRB_Lun;

    /* insert new record at beginning of open device list */
    curr->next = ASPI_open_devices;
    ASPI_open_devices = curr;
    return fd;
}


static void
ASPI_DebugPrintCmd(SRB_ExecSCSICmd *prb)
{
  BYTE  cmd;
  int   i;
  BYTE *cdb;

  switch (prb->CDBByte[0]) {
  case CMD_INQUIRY:
    TRACE("{\n");
    TRACE("\tEVPD: %d\n", prb->CDBByte[1] & 1);
    TRACE("\tLUN: %d\n", (prb->CDBByte[1] & 0xc) >> 1);
    TRACE("\tPAGE CODE: %d\n", prb->CDBByte[2]);
    TRACE("\tALLOCATION LENGTH: %d\n", prb->CDBByte[4]);
    TRACE("\tCONTROL: %d\n", prb->CDBByte[5]);
    TRACE("}\n");
    break;
  case CMD_SCAN_SCAN:
    TRACE("Transfer Length: %d\n", prb->CDBByte[4]);
    break;
  }

  TRACE("Host Adapter: %d\n", prb->SRB_HaId);
  TRACE("Flags: %d\n", prb->SRB_Flags);
  if (TARGET_TO_HOST(prb)) {
    TRACE("\tData transfer: Target to host. Length checked.\n");
  }
  else if (HOST_TO_TARGET(prb)) {
    TRACE("\tData transfer: Host to target. Length checked.\n");
  }
  else if (NO_DATA_TRANSFERED(prb)) {
    TRACE("\tData transfer: none\n");
  }
  else {
    WARN("\tTransfer by scsi cmd. Length not checked.\n");
  }

  TRACE("\tResidual byte length reporting %s\n", prb->SRB_Flags & 0x4 ? "enabled" : "disabled");
  TRACE("\tLinking %s\n", prb->SRB_Flags & 0x2 ? "enabled" : "disabled");
  TRACE("\tPosting %s\n", prb->SRB_Flags & 0x1 ? "enabled" : "disabled");
  TRACE("Target: %d\n", prb->SRB_Target);
  TRACE("Lun: %d\n", prb->SRB_Lun);
  TRACE("BufLen: %ld\n", prb->SRB_BufLen);
  TRACE("SenseLen: %d\n", prb->SRB_SenseLen);
  TRACE("BufPtr: %p\n", prb->SRB_BufPointer);
  TRACE("CDB Length: %d\n", prb->SRB_CDBLen);
  TRACE("POST Proc: %lx\n", (DWORD) prb->SRB_PostProc);
  cdb = &prb->CDBByte[0];
  cmd = prb->CDBByte[0];
  if (TRACE_ON(aspi))
  {
      DPRINTF("CDB buffer[");
      for (i = 0; i < prb->SRB_CDBLen; i++) {
          if (i != 0) DPRINTF(",");
          DPRINTF("%02x", *cdb++);
      }
      DPRINTF("]\n");
  }
}

static void
ASPI_PrintSenseArea(SRB_ExecSCSICmd *prb)
{
  int   i;
  BYTE *cdb;

  if (TRACE_ON(aspi))
  {
      cdb = &prb->CDBByte[0];
      DPRINTF("SenseArea[");
      for (i = 0; i < prb->SRB_SenseLen; i++) {
          if (i) DPRINTF(",");
          DPRINTF("%02x", *cdb++);
      }
      DPRINTF("]\n");
  }
}

static void
ASPI_DebugPrintResult(SRB_ExecSCSICmd *prb)
{

  switch (prb->CDBByte[0]) {
  case CMD_INQUIRY:
    TRACE("Vendor: '%s'\n", prb->SRB_BufPointer + INQUIRY_VENDOR);
    break;
  case CMD_TEST_UNIT_READY:
    ASPI_PrintSenseArea(prb);
    break;
  }
}

static WORD
ASPI_ExecScsiCmd(SRB_ExecSCSICmd *lpPRB)
{
  struct sg_header *sg_hd, *sg_reply_hdr;
  int   status;
  int   in_len, out_len;
  int   error_code = 0;
  int   fd;

  ASPI_DebugPrintCmd(lpPRB);

  fd = ASPI_OpenDevice(lpPRB);
  if (fd == -1) {
      ERR("Failed: could not open device c%01dt%01dd%01d. Device permissions !?\n",
          lpPRB->SRB_HaId,lpPRB->SRB_Target,lpPRB->SRB_Lun);
      lpPRB->SRB_Status = SS_NO_DEVICE;
      return SS_NO_DEVICE;
  }

  sg_hd = NULL;
  sg_reply_hdr = NULL;

  lpPRB->SRB_Status = SS_PENDING;

  if (!lpPRB->SRB_CDBLen) {
      WARN("Failed: lpPRB->SRB_CDBLen = 0.\n");
      lpPRB->SRB_Status = SS_ERR;
      return SS_ERR;
  }

  /* build up sg_header + scsi cmd */
  if (HOST_TO_TARGET(lpPRB)) {
    /* send header, command, and then data */
    in_len = SCSI_OFF + lpPRB->SRB_CDBLen + lpPRB->SRB_BufLen;
    sg_hd = (struct sg_header *) malloc(in_len);
    memset(sg_hd, 0, SCSI_OFF);
    memcpy(sg_hd + 1, &lpPRB->CDBByte[0], lpPRB->SRB_CDBLen);
    if (lpPRB->SRB_BufLen) {
      memcpy(((BYTE *) sg_hd) + SCSI_OFF + lpPRB->SRB_CDBLen, lpPRB->SRB_BufPointer, lpPRB->SRB_BufLen);
    }
  }
  else {
    /* send header and command - no data */
    in_len = SCSI_OFF + lpPRB->SRB_CDBLen;
    sg_hd = (struct sg_header *) malloc(in_len);
    memset(sg_hd, 0, SCSI_OFF);
    memcpy(sg_hd + 1, &lpPRB->CDBByte[0], lpPRB->SRB_CDBLen);
  }

  if (TARGET_TO_HOST(lpPRB)) {
    out_len = SCSI_OFF + lpPRB->SRB_BufLen;
    sg_reply_hdr = (struct sg_header *) malloc(out_len);
    memset(sg_reply_hdr, 0, SCSI_OFF);
    sg_hd->reply_len = out_len;
  }
  else {
    out_len = SCSI_OFF;
    sg_reply_hdr = (struct sg_header *) malloc(out_len);
    memset(sg_reply_hdr, 0, SCSI_OFF);
    sg_hd->reply_len = out_len;
  }

  status = write(fd, sg_hd, in_len);
  if (status < 0 || status != in_len) {
      int save_error = errno;

    WARN("Not enough bytes written to scsi device bytes=%d .. %d\n", in_len, status);
    if (status < 0) {
                if (save_error == ENOMEM) {
            MESSAGE("ASPI: Linux generic scsi driver\n  You probably need to re-compile your kernel with a larger SG_BIG_BUFF value (sg.h)\n  Suggest 130560\n");
        }
#ifdef HAVE_STRERROR
                WARN("error:= '%s'\n", strerror(save_error));
#else
                WARN("error:= %d\n", save_error);
#endif
    }
    goto error_exit;
  }

  status = read(fd, sg_reply_hdr, out_len);
  if (status < 0 || status != out_len) {
    WARN("not enough bytes read from scsi device%d\n", status);
    goto error_exit;
  }

  if (sg_reply_hdr->result != 0) {
    error_code = sg_reply_hdr->result;
    WARN("reply header error (%d)\n", sg_reply_hdr->result);
    goto error_exit;
  }

  if (TARGET_TO_HOST(lpPRB) && lpPRB->SRB_BufLen) {
    memcpy(lpPRB->SRB_BufPointer, sg_reply_hdr + 1, lpPRB->SRB_BufLen);
  }

  /* copy in sense buffer to amount that is available in client */
  if (lpPRB->SRB_SenseLen) {
    int sense_len = lpPRB->SRB_SenseLen;
    if (lpPRB->SRB_SenseLen > 16)
      sense_len = 16;
    memcpy(SENSE_BUFFER(lpPRB), &sg_reply_hdr->sense_buffer[0], sense_len);
  }


  lpPRB->SRB_Status = SS_COMP;
  lpPRB->SRB_HaStat = HASTAT_OK;
  lpPRB->SRB_TargStat = STATUS_GOOD;

  /* now do posting */

  if (lpPRB->SRB_PostProc) {
    if (ASPI_POSTING(lpPRB)) {
      TRACE("Post Routine (%lx) called\n", (DWORD) lpPRB->SRB_PostProc);
      (*lpPRB->SRB_PostProc)(lpPRB);
    }
    else
    if (lpPRB->SRB_Flags & SRB_EVENT_NOTIFY) {
      TRACE("Setting event %04x\n", (HANDLE)lpPRB->SRB_PostProc);
      SetEvent((HANDLE)lpPRB->SRB_PostProc); /* FIXME: correct ? */
    }
  }
  free(sg_reply_hdr);
  free(sg_hd);
  ASPI_DebugPrintResult(lpPRB);
  return SS_COMP;

error_exit:
  if (error_code == EBUSY) {
      lpPRB->SRB_Status = SS_ASPI_IS_BUSY;
      TRACE("Device busy\n");
  }
  else {
      WARN("Failed\n");
      lpPRB->SRB_Status = SS_ERR;
  }

  /* I'm not sure exactly error codes work here
   * We probably should set lpPRB->SRB_TargStat, SRB_HaStat ?
   */
  WARN("error_exit\n");
  free(sg_reply_hdr);
  free(sg_hd);
  return lpPRB->SRB_Status;
}
#endif

// OS/2 ODIN implemetation code
#ifdef _ODIN_H_

static void
ASPI_DebugPrintCmd(SRB_ExecSCSICmd *prb)
{
  BYTE  cmd;
  int   i;
  BYTE *cdb;

  switch (prb->CDBByte[0]) {
  case CMD_INQUIRY:
    TRACE("{\n");
    TRACE("\tEVPD: %d\n", prb->CDBByte[1] & 1);
    TRACE("\tLUN: %d\n", (prb->CDBByte[1] & 0xc) >> 1);
    TRACE("\tPAGE CODE: %d\n", prb->CDBByte[2]);
    TRACE("\tALLOCATION LENGTH: %d\n", prb->CDBByte[4]);
    TRACE("\tCONTROL: %d\n", prb->CDBByte[5]);
    TRACE("}\n");
    break;
  case CMD_SCAN_SCAN:
    TRACE("Transfer Length: %d\n", prb->CDBByte[4]);
    break;
  }

  TRACE("Host Adapter: %d\n", prb->SRB_HaId);
  TRACE("Flags: %d\n", prb->SRB_Flags);
  if (TARGET_TO_HOST(prb)) {
    TRACE("\tData transfer: Target to host. Length checked.\n");
  }
  else if (HOST_TO_TARGET(prb)) {
    TRACE("\tData transfer: Host to target. Length checked.\n");
  }
  else if (NO_DATA_TRANSFERED(prb)) {
    TRACE("\tData transfer: none\n");
  }
  else {
    WARN("\tTransfer by scsi cmd. Length not checked.\n");
  }

  TRACE("\tResidual byte length reporting %s\n", prb->SRB_Flags & 0x4 ? "enabled" : "disabled");
  TRACE("\tLinking %s\n", prb->SRB_Flags & 0x2 ? "enabled" : "disabled");
  TRACE("\tPosting %s\n", prb->SRB_Flags & 0x1 ? "enabled" : "disabled");
  TRACE("Target: %d\n", prb->SRB_Target);
  TRACE("Lun: %d\n", prb->SRB_Lun);
  TRACE("BufLen: %ld\n", prb->SRB_BufLen);
  TRACE("SenseLen: %d\n", prb->SRB_SenseLen);
  TRACE("BufPtr: %p\n", prb->SRB_BufPointer);
  TRACE("CDB Length: %d\n", prb->SRB_CDBLen);
  TRACE("POST Proc: %lx\n", (DWORD) prb->SRB_PostProc);
  cdb = &prb->CDBByte[0];
  cmd = prb->CDBByte[0];
  if (TRACE_ON(aspi))
  {
      DPRINTF("CDB buffer[");
      for (i = 0; i < prb->SRB_CDBLen; i++) {
          if (i != 0) DPRINTF(",");
          DPRINTF("%02x", *cdb++);
      }
      DPRINTF("]\n");
  }
}

static void
ASPI_PrintSenseArea(SRB_ExecSCSICmd *prb)
{
  int   i;
  BYTE *cdb;

  if (TRACE_ON(aspi))
  {
      cdb = &prb->CDBByte[0];
      DPRINTF("SenseArea[");
      for (i = 0; i < prb->SRB_SenseLen; i++) {
          if (i) DPRINTF(",");
          DPRINTF("%02x", *cdb++);
      }
      DPRINTF("]\n");
  }
}

static void
ASPI_DebugPrintResult(SRB_ExecSCSICmd *prb)
{

  switch (prb->CDBByte[0]) {
  case CMD_INQUIRY:
    TRACE("Vendor: '%s'\n", prb->SRB_BufPointer + INQUIRY_VENDOR);
    break;
  case CMD_TEST_UNIT_READY:
    ASPI_PrintSenseArea(prb);
    break;
  }
}


static WORD
ASPI_ExecScsiCmd( PODINASPIDATA pData,
                  SRB_ExecSCSICmd *lpPRB)
{
  int   status;
  int   error_code = 0;
  SRBOS2 SRBlock;
  LONG rc;

  ASPI_DebugPrintCmd(lpPRB);

  lpPRB->SRB_Status = SS_PENDING;

  if (!lpPRB->SRB_CDBLen)
  {
      WARN("Failed: lpPRB->SRB_CDBLen = 0.\n");
      lpPRB->SRB_Status = SS_ERR;
      return SS_ERR;
  }

  if(MaxCDBStatus<lpPRB->SRB_CDBLen)
  {
    WARN("Failed: lpPRB->SRB_CDBLen > 64.\n");
    lpPRB->SRB_Status = SS_ERR;
    return SS_ERR;
  }

  if(lpPRB->SRB_BufLen>65536)    // Check Max 64k!!
  {
    WARN("Failed: lpPRB->SRB_BufLen > 65536.\n");
    lpPRB->SRB_Status = SS_BUFFER_TO_BIG;
    return SS_BUFFER_TO_BIG;
  }

  // copy up to LUN SRBOS2 has no WORD for padding like in WINE
  memcpy( &SRBlock,
          lpPRB,
          6* sizeof(BYTE) + sizeof(DWORD));
  SRBlock.flags |= SRB_Post;
  SRBlock.u.cmd.sense_len = 32;                    // length of sense buffer
  SRBlock.u.cmd.data_ptr  = NULL;                  // pointer to data buffer
  SRBlock.u.cmd.link_ptr  = NULL;                  // pointer to next SRB
  SRBlock.u.cmd.cdb_len   = lpPRB->SRB_CDBLen;     // SCSI command length
  memcpy( &SRBlock.u.cmd.cdb_st[0],
          &lpPRB->CDBByte[0],
          lpPRB->SRB_CDBLen);

  if (HOST_TO_TARGET(lpPRB))
  {
    // Write: Copy all Data to Communication Buffer
    if (lpPRB->SRB_BufLen)
    {
      memcpy( pData->pvBuffer,
              lpPRB->SRB_BufPointer,
              lpPRB->SRB_BufLen);
    }
  }

  rc = lSendSRBlock( pData->hfDriver,
                     &SRBlock,
                     &SRBlock);

  if(!rc)
  {
    if( fWaitPost(pData->hevPost))
    {
      if (TARGET_TO_HOST(lpPRB))
      {
        // Was Read : Copy all Data from Communication Buffer
        if (lpPRB->SRB_BufLen)
        {
          memcpy( lpPRB->SRB_BufPointer,
                  pData->pvBuffer,
                  lpPRB->SRB_BufLen);
        }
      }

      if (lpPRB->SRB_SenseLen)
      {
        int sense_len = lpPRB->SRB_SenseLen;
        if (lpPRB->SRB_SenseLen > 32)
          sense_len = 32;
        memcpy( SENSE_BUFFER(lpPRB),
                &SRBlock.u.cmd.cdb_st[SRBlock.u.cmd.cdb_len],
                sense_len);
      }

      /* now do posting */

      if (lpPRB->SRB_PostProc)
      {
        if (ASPI_POSTING(lpPRB))
        {
          TRACE("Post Routine (%lx) called\n", (DWORD) lpPRB->SRB_PostProc);
          (*lpPRB->SRB_PostProc)();
        }
        else
        if (lpPRB->SRB_Flags & SRB_EVENT_NOTIFY)
        {
          TRACE("Setting event %04x\n", (HANDLE)lpPRB->SRB_PostProc);
          SetEvent((HANDLE)lpPRB->SRB_PostProc); /* FIXME: correct ? */
        }
      }
    }
    lpPRB->SRB_Status = SRBlock.status;
    lpPRB->SRB_HaStat = SRBlock.u.cmd.ha_status;
    lpPRB->SRB_TargStat = SRBlock.u.cmd.target_status;
  }
  else
    lpPRB->SRB_Status = SS_ERR;


  ASPI_DebugPrintResult(lpPRB);

  return lpPRB->SRB_Status;

}
#endif

/*******************************************************************
 *     GetASPI32SupportInfo32           [WNASPI32.0]
 *
 * Checks if the ASPI subsystem is initialized correctly.
 *
 * RETURNS
 *    HIWORD: 0.
 *    HIBYTE of LOWORD: status (SS_COMP or SS_FAILED_INIT)
 *    LOBYTE of LOWORD: # of host adapters.
 */
#ifdef _ODIN_H_
DWORD WINAPI GetASPI32SupportInfo()
{
  LONG rc;
  ODINASPIDATA aspi;
  ULONG hmtxDriver;
  BYTE bNumDrv;
  HKEY hkeyDrvInfo;
  DWORD dwType;
  DWORD dwData;
  DWORD dwSize;
  SRBOS2 SRBlock;
  ULONG ulParam, ulReturn;
  BYTE brc;

  bNumDrv = 0;

  if( fGainDrvAccess( FALSE, &hmtxDriver) ) // Do nonblocking call for info
  {
    brc = bOpenASPI(&aspi);
    if(SS_COMP==brc  )
    {
      SRBlock.cmd=SRB_Inquiry;                      // host adapter inquiry
      SRBlock.ha_num=0;                             // host adapter number
      SRBlock.flags=0;                              // no flags set

      rc = lSendSRBlock( aspi.hfDriver,
                         &SRBlock,
                         &SRBlock);

      if (!rc)
      {
        bNumDrv = SRBlock.u.inq.num_ha;

        rc = RegOpenKeyA ( HKEY_LOCAL_MACHINE,
                           "Software\\ODIN\\ASPIROUT",
                           &hkeyDrvInfo);
        if(ERROR_SUCCESS==rc)
        {
          dwData = bNumDrv;
          RegSetValueExA( hkeyDrvInfo,
                          "NumAdapers",
                          NULL,
                          REG_DWORD,
                          (LPBYTE)&dwData,
                          sizeof(DWORD));
          RegCloseKey( hkeyDrvInfo);
        }
      }

      fCloseASPI(&aspi);
    }
    else
      brc = SS_FAILED_INIT;

    fReleaseDrvAccess( hmtxDriver);
  }
  else
  {
    // Driver is used by other process/thread
    // so try get value form registry

    brc = SS_FAILED_INIT;
    rc = RegOpenKeyA ( HKEY_LOCAL_MACHINE,
                       "Software\\ODIN\\ASPIROUT",
                       &hkeyDrvInfo);

    if(ERROR_SUCCESS!=rc)
      return ((SS_FAILED_INIT << 8) | bNumDrv);

    dwSize = sizeof(DWORD);
    rc = RegQueryValueExA( hkeyDrvInfo,
                           "NumAdapers",
                           NULL,
                           &dwType,
                           (BYTE*)&dwData,
                           &dwSize);

    RegCloseKey( hkeyDrvInfo);

    if( (ERROR_SUCCESS==rc) &&
        (REG_DWORD != dwType) )
    {
      bNumDrv = 0xFF & dwData;
      brc = SS_COMP;
    }
  }

  return ((brc << 8) | bNumDrv); /* FIXME: get # of host adapters installed */
}
#else
DWORD WINAPI GetASPI32SupportInfo()
{
  return ((SS_COMP << 8) | 1); /* FIXME: get # of host adapters installed */
}
#endif

/***********************************************************************
 *             SendASPI32Command32 (WNASPI32.1)
 */
DWORD __cdecl SendASPI32Command(LPSRB lpSRB)
{
#ifdef linux
  switch (lpSRB->common.SRB_Cmd) {
  case SC_HA_INQUIRY:
    lpSRB->inquiry.SRB_Status = SS_COMP;       /* completed successfully */
    lpSRB->inquiry.HA_Count = 1;               /* not always */
    lpSRB->inquiry.HA_SCSI_ID = 7;             /* not always ID 7 */
    strcat(lpSRB->inquiry.HA_ManagerId, "ASPI for WIN32"); /* max 15 chars, don't change */
    strcat(lpSRB->inquiry.HA_Identifier, "Wine host"); /* FIXME: return host adapter name */
    memset(lpSRB->inquiry.HA_Unique, 0, 16); /* default HA_Unique content */
    lpSRB->inquiry.HA_Unique[6] = 0x02; /* Maximum Transfer Length (128K, Byte> 4-7) */
    FIXME("ASPI: Partially implemented SC_HA_INQUIRY for adapter %d.\n", lpSRB->inquiry.SRB_HaId);
    return SS_COMP;
  case SC_GET_DEV_TYPE:
    FIXME("Not implemented SC_GET_DEV_TYPE\n");
    break;
  case SC_EXEC_SCSI_CMD:
    return ASPI_ExecScsiCmd(&lpSRB->cmd);
    break;
  case SC_RESET_DEV:
    FIXME("Not implemented SC_RESET_DEV\n");
    break;
  default:
    WARN("Unknown command %d\n", lpSRB->common.SRB_Cmd);
  }
  return SS_INVALID_SRB;
#else
  #ifdef _ODIN_H_

    SRBOS2 SRBlock;
    DWORD dwRC;
    ULONG ulParam, ulReturn;
    BYTE  bRC;
    ODINASPIDATA aspi;
    ULONG hmtxDriver;
    LONG rc;

    if(NULL==lpSRB)
      return SS_INVALID_SRB;  // Not sure what to return here but that is an error

    // test first for a valid command
    if( (SC_HA_INQUIRY!=lpSRB->common.SRB_Cmd) &&
        (SC_GET_DEV_TYPE!=lpSRB->common.SRB_Cmd) &&
        (SC_EXEC_SCSI_CMD!=lpSRB->common.SRB_Cmd) &&
        (SC_ABORT_SRB!=lpSRB->common.SRB_Cmd) &&
        (SC_RESET_DEV!=lpSRB->common.SRB_Cmd) )
      return SS_INVALID_SRB; // shoud be invalid command

    dwRC = SS_ERR;

    if( fGainDrvAccess( TRUE, &hmtxDriver) ) // Block if a SRB is pending
    {
      bRC = bOpenASPI( &aspi);
      if(SS_COMP==bRC)
      {
        switch (lpSRB->common.SRB_Cmd)
        {
          case SC_HA_INQUIRY:
            SRBlock.cmd=SRB_Inquiry;                      // host adapter inquiry
            SRBlock.ha_num=lpSRB->inquiry.SRB_HaId;           // host adapter number
            SRBlock.flags=0;                              // no flags set

            rc = lSendSRBlock( aspi.hfDriver,
                               &SRBlock,
                               &SRBlock);
            if (!rc)
            {
              memcpy( lpSRB,
                      &SRBlock,
                      sizeof(SRB_HaInquiry) );
              dwRC = SRBlock.status;
            }
            break;
          case SC_GET_DEV_TYPE:
            memcpy( &SRBlock,
                    lpSRB,
                    sizeof(SRB_GDEVBlock));
            SRBlock.flags = 0;

            rc = lSendSRBlock( aspi.hfDriver,
                               &SRBlock,
                               &SRBlock);

            if (!rc)
            {
              memcpy( lpSRB,
                      &SRBlock,
                      sizeof(SRB_GDEVBlock) );
              lpSRB->devtype.SRB_Rsvd1 = 0x00;
              dwRC = SRBlock.status;
            }

            break;
          case SC_EXEC_SCSI_CMD:
            dwRC = ASPI_ExecScsiCmd( &aspi,
                                     &lpSRB->cmd);
            break;
          case SC_ABORT_SRB:
            dwRC = SS_INVALID_SRB; // We don't do async ASPI so no way to abort
            break;
          case SC_RESET_DEV:
            memset( &SRBlock,
                    0,
                    sizeof(SRBOS2) );
            SRBlock.cmd = lpSRB->reset.SRB_Cmd;       /* ASPI command code = SC_RESET_DEV */
            SRBlock.status = lpSRB->reset.SRB_Status; /* ASPI command status byte */
            SRBlock.ha_num = lpSRB->reset.SRB_HaId;   /* ASPI host adapter number */
            SRBlock.flags =  SRB_Post;
            SRBlock.u.res.target = lpSRB->reset.SRB_Target; /* Target's SCSI ID */
            SRBlock.u.res.lun    = lpSRB->reset.SRB_Lun;    /* Target's LUN number */

            rc = lSendSRBlock( aspi.hfDriver,
                               &SRBlock,
                               &SRBlock);

            if (!rc)
            {
              fWaitPost( aspi.hevPost );
              lpSRB->reset.SRB_Status   = SRBlock.status;
              lpSRB->reset.SRB_Flags    = SRBlock.flags;
              lpSRB->reset.SRB_Hdr_Rsvd = 0;
              memset( lpSRB->reset.SRB_Rsvd1,
                      0,
                      12 );
              lpSRB->reset.SRB_HaStat   = SRBlock.u.res.ha_status;     /* Host Adapter Status */
              lpSRB->reset.SRB_TargStat = SRBlock.u.res.target_status; /* Target Status */
              lpSRB->reset.SRB_PostProc = NULL;                  /* Post routine */
              lpSRB->reset.SRB_Rsvd2    = NULL;                  /* Reserved */
              memset( lpSRB->reset.SRB_Rsvd3,
                      0,
                      32);            /* Reserved */
              dwRC = SRBlock.status;
            }
            break;
          } // end switch (lpSRB->common.SRB_Cmd)

        fCloseASPI(&aspi);
      }
      else
      {
        dwRC = bRC;
      }

      fReleaseDrvAccess( hmtxDriver);
    }
    else
    {
      dwRC = SS_NO_ASPI;
    }

    return dwRC;

  #else
    return SS_INVALID_SRB;
  #endif
#endif
}


/***********************************************************************
 *             GetASPI32DLLVersion32   (WNASPI32.3)
 */

DWORD WINAPI GetASPI32DLLVersion()
{
#ifdef linux
  return (DWORD)1;
#else
  #ifdef _ODIN_H_
    return (DWORD)1;
  #else
    return (DWORD)0;
  #endif
#endif
}

