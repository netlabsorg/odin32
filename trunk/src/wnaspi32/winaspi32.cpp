/* $Id: winaspi32.cpp,v 1.12 2000-12-16 15:42:42 sandervl Exp $ */
/*
 * WNASPI routines
 *
 * Copyright 1999 Markus Montkowski
 * Copyright 2000 Przemyslaw Dobrowolski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>

#include "aspi.h"
#include "wnaspi32.h"
#include <winreg.h>
#include "options.h"
#include "debugtools.h"

//#include "srbos2.h"
//#include "odinaspi.h"
#include "aspilib.h"
//#include "callback.h"

#ifdef DEBUG
#define DEBUG_BUFFER
#endif

DEFAULT_DEBUG_CHANNEL(aspi)
ODINDEBUGCHANNEL(WNASPI32)

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
#ifdef DEBUG_BUFFER
      dprintfNoEOL(("CDB buffer["));
      for (i = 0; i < prb->SRB_CDBLen; i++) {
          if (i != 0) dprintfNoEOL((",0x%02x", *cdb++));
	  else        dprintfNoEOL(("0x%02x", *cdb++));
      }
      dprintfNoEOL(("]\n"));
#endif
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
  dprintf(("Result code: %x", prb->SRB_Status));
}


static WORD ASPI_ExecScsiCmd( scsiObj *aspi, SRB_ExecSCSICmd *lpPRB)
{
  int   status;
  int   error_code = 0;
  LONG rc;

  /* FIXME: hackmode */
#define MAKE_TARGET_TO_HOST(lpPRB) \
  	if (!TARGET_TO_HOST(lpPRB)) { \
	    WARN("program was not sending target_to_host for cmd %x (flags=%x),correcting.\n",lpPRB->CDBByte[0],lpPRB->SRB_Flags); \
	    lpPRB->SRB_Flags |= 8; \
	}
#define MAKE_HOST_TO_TARGET(lpPRB) \
  	if (!HOST_TO_TARGET(lpPRB)) { \
	    WARN("program was not sending host_to_target for cmd %x (flags=%x),correcting.\n",lpPRB->CDBByte[0],lpPRB->SRB_Flags); \
	    lpPRB->SRB_Flags |= 0x10; \
	}
  switch (lpPRB->CDBByte[0]) {
  case 0x12: /* INQUIRY */
  case 0x5a: /* MODE_SENSE_10 */
  case 0xa4: /* REPORT_KEY (DVD) MMC-2 */
  case 0xad: /* READ DVD STRUCTURE MMC-2 */
        MAKE_TARGET_TO_HOST(lpPRB)
	break;
  case 0xa3: /* SEND KEY (DVD) MMC-2 */
        MAKE_HOST_TO_TARGET(lpPRB)
	break;
  default:
	if ((((lpPRB->SRB_Flags & 0x18) == 0x00) ||
	     ((lpPRB->SRB_Flags & 0x18) == 0x18)
	    ) && lpPRB->SRB_BufLen
	) {
	    FIXME("command 0x%02x, no data transfer specified, but buflen is %ld!!!\n",lpPRB->CDBByte[0],lpPRB->SRB_BufLen); 
	}
	break;
  }

  ASPI_DebugPrintCmd(lpPRB);

  lpPRB->SRB_Status = SS_PENDING;

  if (!lpPRB->SRB_CDBLen)
  {
      dprintf(("Failed: lpPRB->SRB_CDBLen = 0."));
      lpPRB->SRB_Status = SS_ERR;
      return SS_ERR;
  }

  if(MaxCDBStatus<lpPRB->SRB_CDBLen)
  {
    dprintf(("Failed: lpPRB->SRB_CDBLen > 64."));
    lpPRB->SRB_Status = SS_ERR;
    return SS_ERR;
  }

  if(lpPRB->SRB_BufLen>65536)    // Check Max 64k!!
  {
    dprintf(("Failed: lpPRB->SRB_BufLen > 65536."));
    lpPRB->SRB_Status = SS_BUFFER_TO_BIG;
    return SS_BUFFER_TO_BIG;
  }

  // copy up to LUN SRBOS2 has no WORD for padding like in WINE
  memcpy( &aspi->SRBlock,
          lpPRB,
          6* sizeof(BYTE) + sizeof(DWORD));
  aspi->SRBlock.flags |= SRB_Post;
  aspi->SRBlock.u.cmd.sense_len = lpPRB->SRB_SenseLen;;   // length of sense buffer
  aspi->SRBlock.u.cmd.data_ptr  = NULL;                   // pointer to data buffer
  aspi->SRBlock.u.cmd.link_ptr  = NULL;                   // pointer to next SRB
  aspi->SRBlock.u.cmd.data_len  = lpPRB->SRB_BufLen;
  aspi->SRBlock.u.cmd.cdb_len   = lpPRB->SRB_CDBLen;     // SCSI command length
  memcpy( &aspi->SRBlock.u.cmd.cdb_st[0],
          &lpPRB->CDBByte[0],
          lpPRB->SRB_CDBLen);

  if (HOST_TO_TARGET(lpPRB))
  {
    // Write: Copy all Data to Communication Buffer
    if (lpPRB->SRB_BufLen)
    {
      memcpy( aspi->buffer,
              lpPRB->SRB_BufPointer,
              lpPRB->SRB_BufLen);

#ifdef DEBUG_BUFFER
      char *cdb = (char *)lpPRB->SRB_BufPointer;
      dprintfNoEOL(("Write SRB buffer["));
      for (int i = 0; i < lpPRB->SRB_BufLen; i++) {
          if (i != 0) dprintfNoEOL((",0x%02x", *cdb++));
	  else        dprintfNoEOL(("0x%02x", *cdb++));
      }
      dprintfNoEOL(("]\n"));
#endif
    }
  }

  rc = aspi->SendSRBlock();

  if(!rc)
  {
    if( aspi->waitPost())
    {
      if (TARGET_TO_HOST(lpPRB))
      {
        // Was Read : Copy all Data from Communication Buffer
        if (lpPRB->SRB_BufLen)
        {
          memcpy( lpPRB->SRB_BufPointer,
                  aspi->buffer,
                  lpPRB->SRB_BufLen);
#ifdef DEBUG_BUFFER
          char *cdb = (char *)lpPRB->SRB_BufPointer;
          dprintfNoEOL(("Read SRB buffer["));
          for (int i = 0; i < lpPRB->SRB_BufLen; i++) {
             if (i != 0) dprintfNoEOL((",0x%02x", *cdb++));
             else        dprintfNoEOL(("0x%02x", *cdb++));
          }
          dprintfNoEOL(("]\n"));
#endif
        }
      }

      if (lpPRB->SRB_SenseLen)
      {
        int sense_len = lpPRB->SRB_SenseLen;
        if (lpPRB->SRB_SenseLen > 32)
          sense_len = 32;
        memcpy( SENSE_BUFFER(lpPRB),
                &aspi->SRBlock.u.cmd.cdb_st[aspi->SRBlock.u.cmd.cdb_len],
                sense_len);
      }

#if 0
      /* FIXME: Should this be != 0 maybe? */
      if(aspi->SRBlock.u.cmd.target_status == 2 ) {
      	aspi->SRBlock.status = SS_ERR;
    	switch (lpPRB->CDBByte[0]) {
    		case 0xa4: /* REPORT_KEY (DVD) MMC-2 */
    		case 0xa3: /* SEND KEY (DVD) MMC-2 */
          		aspi->SRBlock.status              = SS_COMP;
	  		aspi->SRBlock.u.cmd.target_status = 0;
	  		FIXME("Program wants to do DVD Region switching, but fails (non compliant DVD drive). Ignoring....\n");
	  		break;
    	}
      }
#endif
      /* now do posting */
      if (lpPRB->SRB_PostProc)
      {
        if(ASPI_POSTING(lpPRB))
        {
          dprintf(("Post Routine (%lx) called\n", (DWORD) lpPRB->SRB_PostProc));
          (*lpPRB->SRB_PostProc)();
        }
        else
        if (lpPRB->SRB_Flags & SRB_EVENT_NOTIFY)
        {
          dprintf(("Setting event %04x\n", (HANDLE)lpPRB->SRB_PostProc));
          SetEvent((HANDLE)lpPRB->SRB_PostProc); /* FIXME: correct ? */
        }
      }
    }
  }
  else
    lpPRB->SRB_Status = SS_ERR;

  //SvL: Shouldn't this be set before posting?
  lpPRB->SRB_Status = aspi->SRBlock.status;
  lpPRB->SRB_HaStat = aspi->SRBlock.u.cmd.ha_status;
  lpPRB->SRB_TargStat = aspi->SRBlock.u.cmd.target_status;

  ASPI_DebugPrintResult(lpPRB);

  return lpPRB->SRB_Status;
}

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
ODINFUNCTION0(DWORD, GetASPI32SupportInfo)
{
  LONG rc;
  BYTE bNumDrv;
  HKEY hkeyDrvInfo;
  DWORD dwType;
  DWORD dwData;
  DWORD dwSize;
  ULONG ulParam, ulReturn;
  BYTE brc;

  bNumDrv = 0;

  if(aspi->access(FALSE)) //'non-blocking' call
  {
        bNumDrv = aspi->getNumHosts();
        brc = SS_COMP;
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
        aspi->release();
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

/***********************************************************************
 *             SendASPI32Command32 (WNASPI32.1)
 */
DWORD CDECL SendASPI32Command(LPSRB lpSRB)
{
    DWORD dwRC;
    ULONG ulParam, ulReturn;
    BYTE  bRC;
    LONG rc;

    if(NULL==lpSRB)
      return SS_INVALID_SRB;  // Not sure what to return here but that is an error

    dprintf(("SendASPI32Command %x %d", lpSRB, lpSRB->common.SRB_Cmd));

    // test first for a valid command
    if( (SC_HA_INQUIRY!=lpSRB->common.SRB_Cmd) &&
        (SC_GET_DEV_TYPE!=lpSRB->common.SRB_Cmd) &&
        (SC_EXEC_SCSI_CMD!=lpSRB->common.SRB_Cmd) &&
        (SC_ABORT_SRB!=lpSRB->common.SRB_Cmd) &&
        (SC_RESET_DEV!=lpSRB->common.SRB_Cmd) ) 
    {
	dprintf(("Invalid command!"));
     	return SS_INVALID_SRB; // shoud be invalid command
    }

    dwRC = SS_ERR;

    if(aspi->access(TRUE)) // Block if a SRB is pending
    {
        switch (lpSRB->common.SRB_Cmd)
        {
          case SC_HA_INQUIRY:
            aspi->HA_inquiry(lpSRB->inquiry.SRB_HaId);
            memset(lpSRB,0,sizeof(SRB_HaInquiry));
            memcpy( lpSRB,
                    &aspi->SRBlock,
                    sizeof(SRB_HaInquiry)-4 );
            // FIXME: I'dont know why in OS/2->ha_unique are filled with 0
            // Hackmode ON ======
            lpSRB->inquiry.HA_Unique[6]  = 0x01; // transfer only 64KB
            lpSRB->inquiry.HA_Unique[3]  = 0x10; // 16 devices.
            // HackMode OFF =====
            dwRC = aspi->SRBlock.status;
            break;

          case SC_GET_DEV_TYPE:
            rc=aspi->getDeviceType(lpSRB->devtype.SRB_HaId,lpSRB->devtype.SRB_Target, lpSRB->devtype.SRB_Lun);
            dwRC = lpSRB->devtype.SRB_Status = aspi->SRBlock.status;
            lpSRB->devtype.SRB_DeviceType = aspi->SRBlock.u.dev.devtype;
            break;

          case SC_EXEC_SCSI_CMD:
            dwRC = ASPI_ExecScsiCmd( aspi, &lpSRB->cmd);
            break;

          case SC_ABORT_SRB:
            dwRC = SS_INVALID_SRB; // We don't do async ASPI so no way to abort
            break;

          case SC_RESET_DEV:
            aspi->resetDevice(lpSRB->reset.SRB_HaId,lpSRB->reset.SRB_Target, lpSRB->reset.SRB_Lun);
            lpSRB->reset.SRB_Status   = aspi->SRBlock.status;
            lpSRB->reset.SRB_Flags    = aspi->SRBlock.flags;
            lpSRB->reset.SRB_Hdr_Rsvd = 0;
            memset( lpSRB->reset.SRB_Rsvd1,
                    0,
                    12 );
            lpSRB->reset.SRB_HaStat   = aspi->SRBlock.u.res.ha_status;     /* Host Adapter Status */
            lpSRB->reset.SRB_TargStat = aspi->SRBlock.u.res.target_status; /* Target Status */
            lpSRB->reset.SRB_PostProc = NULL;                  /* Post routine */
            lpSRB->reset.SRB_Rsvd2    = NULL;                  /* Reserved */
            memset( lpSRB->reset.SRB_Rsvd3,
                    0,
                    32);            /* Reserved */
             dwRC = aspi->SRBlock.status;
             break;

        } // end switch (lpSRB->common.SRB_Cmd)

      	aspi->release();
    }
    else
    {
        dprintf(("Unable to access aspi!!"));
        dwRC = SS_NO_ASPI;
    }

    return dwRC;
}

/***********************************************************************
 *             GetASPI32DLLVersion32   (WNASPI32.3)
 */
DWORD WINAPI GetASPI32DLLVersion()
{
  return (DWORD)1;
}

