/* $Id: winaspi32.cpp,v 1.15 2002-07-28 15:38:09 sandervl Exp $ */
/*
 * WNASPI routines
 *
 * Copyright 1999 Markus Montkowski
 * Copyright 2000 Przemyslaw Dobrowolski
 * Copyright 2002 Sander van Leeuwen
 *
 * Partly based on WINE code (dlls\winaspi\winaspi32.c)
 *
 * Copyright 1997 Bruce Milner
 * Copyright 1998 Andreas Mohr
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>

#include "aspi.h"
#include "wnaspi32.h"
#include "options.h"
#include "debugtools.h"
#include "cdio.h"

#ifdef DEBUG
#define DEBUG_BUFFER
#endif


#ifdef DEBUG
//*****************************************************************************
//*****************************************************************************
static void ASPI_DebugPrintCmd(SRB_ExecSCSICmd *prb)
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
//*****************************************************************************
//*****************************************************************************
static void ASPI_PrintSenseArea(SRB_ExecSCSICmd *prb)
{
  int   i;
  BYTE *cdb;

      cdb = &prb->CDBByte[0];
      DPRINTF("SenseArea[");
      for (i = 0; i < prb->SRB_SenseLen; i++) {
          if (i) DPRINTF(",");
          DPRINTF("%02x", *cdb++);
      }
      DPRINTF("]\n");
}
//*****************************************************************************
//*****************************************************************************
static void ASPI_DebugPrintResult(SRB_ExecSCSICmd *prb)
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
#else
#define ASPI_DebugPrintResult(a)
#define ASPI_DebugPrintCmd(a)
#endif

//*****************************************************************************
// Posting must be done in such a way that as soon as the SRB_Status is set
// we don't touch the SRB anymore because it could possibly be freed
// if the app is doing ASPI polling
//*****************************************************************************
static DWORD WNASPI32_DoPosting( SRB_ExecSCSICmd *lpPRB, DWORD status )
{
	void (* CDECL SRB_PostProc)(SRB_ExecSCSICmd *);
	BYTE SRB_Flags = lpPRB->SRB_Flags;
	if( status == SS_PENDING )
	{
		dprintf(("Tried posting SS_PENDING\n"));
		return SS_PENDING;
	}
    *((DWORD *)&SRB_PostProc) = (DWORD)lpPRB->SRB_PostProc;

	lpPRB->SRB_Status = status;
	/* lpPRB is NOT safe, it could be freed in another thread */

	if (SRB_PostProc)
	{
		if (SRB_Flags & 0x1)
		{
			dprintf(("Post Routine (%lx) called\n", (DWORD) SRB_PostProc));
			/* Even though lpPRB could have been freed by
			 * the program.. that's unlikely if it planned
			 * to use it in the PostProc
			 */
			(*SRB_PostProc)(lpPRB);
		}
		else if (SRB_Flags & SRB_EVENT_NOTIFY) {
			dprintf(("Setting event %04x\n", (HANDLE)SRB_PostProc));
			SetEvent((HANDLE)SRB_PostProc);
		}
	}
	return lpPRB->SRB_Status;
}
//*****************************************************************************
//*****************************************************************************
static WORD ASPI_ExecScsiCmd(SRB_ExecSCSICmd *lpPRB)
{
    int   status;
    int   error_code = 0;
    LONG  rc;
    CDIO_CMD_BUFFER cdiocmd;

    HCDIO hCDIO = OSLibCdIoGetDevice(lpPRB->SRB_HaId, lpPRB->SRB_Target, lpPRB->SRB_Lun);
    if(hCDIO == -1) {
        dprintf(("Failed: invalid device %d %d %d", lpPRB->SRB_HaId, lpPRB->SRB_Target, lpPRB->SRB_Lun));
        ASPI_DebugPrintCmd(lpPRB);
        return WNASPI32_DoPosting( lpPRB, SS_NO_DEVICE );
    }

  /* FIXME: hackmode */
#define MAKE_TARGET_TO_HOST(lpPRB) \
  	if (!TARGET_TO_HOST(lpPRB)) { \
	    dprintf(("program was not sending target_to_host for cmd %x (flags=%x),correcting.\n",lpPRB->CDBByte[0],lpPRB->SRB_Flags)); \
	    lpPRB->SRB_Flags |= SRB_DIR_IN; \
	}
#define MAKE_HOST_TO_TARGET(lpPRB) \
  	if (!HOST_TO_TARGET(lpPRB)) { \
	    dprintf(("program was not sending host_to_target for cmd %x (flags=%x),correcting.\n",lpPRB->CDBByte[0],lpPRB->SRB_Flags)); \
	    lpPRB->SRB_Flags |= SRB_DIR_OUT; \
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
	        dprintf(("command 0x%02x, no data transfer specified, but buflen is %ld!!!\n",lpPRB->CDBByte[0],lpPRB->SRB_BufLen)); 
	    }
	    break;
    }

    ASPI_DebugPrintCmd(lpPRB);

    lpPRB->SRB_Status = SS_PENDING;

    if (!lpPRB->SRB_CDBLen)
    {
        dprintf(("Failed: lpPRB->SRB_CDBLen = 0."));
        return WNASPI32_DoPosting( lpPRB, SS_INVALID_SRB );
    }

    if(MAX_CDBLEN < lpPRB->SRB_CDBLen)
    {
        dprintf(("Failed: lpPRB->SRB_CDBLen > 64."));
        return WNASPI32_DoPosting( lpPRB, SS_INVALID_SRB );
    }

    if(lpPRB->SRB_BufLen > 65536)    // Check Max 64k!!
    {
        dprintf(("Failed: lpPRB->SRB_BufLen > 65536."));
        return WNASPI32_DoPosting( lpPRB, SS_BUFFER_TO_BIG );
    }

    memset(&cdiocmd, 0, sizeof(cdiocmd));

    if(lpPRB->SRB_Flags & SRB_DIR_IN) {
        cdiocmd.flDirection |= CMDDIR_INPUT;
    }
    if(lpPRB->SRB_Flags & SRB_DIR_OUT) {
        cdiocmd.flDirection |= CMDDIR_OUTPUT;
    }

    cdiocmd.cbCDB = lpPRB->SRB_CDBLen;
    memcpy(cdiocmd.arCDB, lpPRB->CDBByte, lpPRB->SRB_CDBLen);

#ifdef DEBUG_BUFFER
    if(lpPRB->SRB_Flags & SRB_DIR_OUT) {
        char *cdb = (char *)lpPRB->SRB_BufPointer;
        dprintfNoEOL(("Write SRB buffer["));
        for (int i = 0; i < lpPRB->SRB_BufLen; i++) {
            if (i != 0) dprintfNoEOL((",0x%02x", *cdb++));
            else        dprintfNoEOL(("0x%02x", *cdb++));
        }
        dprintfNoEOL(("]\n"));
    }
#endif

    lpPRB->SRB_HaStat   = HASTAT_OK;
    lpPRB->SRB_TargStat = HASTAT_OK;

    if(OSLibCdIoSendCommand(hCDIO, &cdiocmd, lpPRB->SRB_BufPointer, lpPRB->SRB_BufLen) == FALSE) {
        dprintf(("OSLibCdIoSendCommand failed!!"));
        return WNASPI32_DoPosting( lpPRB, SS_ERR );
    }

    if(lpPRB->SRB_SenseLen) {
        if(OSLibCdIoRequestSense(hCDIO, SENSE_BUFFER(lpPRB), lpPRB->SRB_SenseLen) == FALSE) {
            dprintf(("OSLibCdIoRequestSense failed!!"));
            return WNASPI32_DoPosting( lpPRB, SS_ERR );
        }
    }
#ifdef DEBUG_BUFFER
    if(lpPRB->SRB_Flags & SRB_DIR_IN) {
        char *cdb = (char *)lpPRB->SRB_BufPointer;
        dprintfNoEOL(("Write SRB buffer["));
        for (int i = 0; i < lpPRB->SRB_BufLen; i++) {
            if (i != 0) dprintfNoEOL((",0x%02x", *cdb++));
            else        dprintfNoEOL(("0x%02x", *cdb++));
        }
        dprintfNoEOL(("]\n"));
    }
#endif
    ASPI_DebugPrintResult(lpPRB);
    return WNASPI32_DoPosting( lpPRB, SS_COMP );
}
//*****************************************************************************
//     GetASPI32SupportInfo32           [WNASPI32.0]
//
// Checks if the ASPI subsystem is initialized correctly.
//
// RETURNS
//    HIWORD: 0.
//    HIBYTE of LOWORD: status (SS_COMP or SS_FAILED_INIT)
//    LOBYTE of LOWORD: # of host adapters.
//*****************************************************************************
DWORD CDECL GetASPI32SupportInfo()
{
    BYTE bNumDrv;
    BYTE brc;

    bNumDrv = 0;

    // first thing we do is check whether we have a successful ASPI setup
    if(fASPIAvailable == FALSE)
    {
        dprintf(("ASPI was not initialized successfully, return error"));
        return (SS_FAILED_INIT << 8);
    }

    bNumDrv = OSLibCdIoGetNumDrives();
    if (!bNumDrv)
	    return SS_NO_ADAPTERS << 8;

    brc = SS_COMP;

    //pretend there's only one controller
    return ((brc << 8) | 1);
}
//*****************************************************************************
//    SendASPI32Command32 (WNASPI32.1)
//*****************************************************************************
DWORD CDECL SendASPI32Command(LPSRB lpSRB)
{
    DWORD dwRC;
    ULONG ulParam, ulReturn;
    BYTE  bRC;
    LONG  rc;

    // first thing we do is check whether we have a successful ASPI setup
    if(fASPIAvailable == FALSE)
    {
        dprintf(("SendASPI32Command: ASPI was not initialized successfully, return error"));
        return SS_NO_ASPI;
    }

    if(NULL==lpSRB)
        return SS_INVALID_SRB;  // Not sure what to return here but that is an error

    dprintf(("SendASPI32Command %x %d", lpSRB, lpSRB->common.SRB_Cmd));

    dwRC = SS_ERR;

    switch (lpSRB->common.SRB_Cmd)
    {
    case SC_HA_INQUIRY:
        lpSRB->inquiry.SRB_Status   = SS_COMP;       /* completed successfully */
        lpSRB->inquiry.HA_Count     = 1;
        lpSRB->inquiry.HA_SCSI_ID   = 7;             /* not always ID 7 */
        strcpy((char *)lpSRB->inquiry.HA_ManagerId, "ASPI for WIN32"); /* max 15 chars, don't change */
        strcpy((char *)lpSRB->inquiry.HA_Identifier, "Odin host"); /* FIXME: return host adapter name */
        memset(lpSRB->inquiry.HA_Unique, 0, 16); /* default HA_Unique content */
        lpSRB->inquiry.HA_Unique[6] = 0x02; /* Maximum Transfer Length (128K, Byte> 4-7) */
        lpSRB->inquiry.HA_Unique[3] = 0x08; /* Maximum number of SCSI targets */
        dprintf(("ASPI: Partially implemented SC_HA_INQUIRY for adapter %d.\n", lpSRB->inquiry.SRB_HaId));
        
        //OS/2 override
        lpSRB->inquiry.HA_Unique[6] = 0x01; // transfer only 64KB
        lpSRB->inquiry.HA_Unique[3] = 0x10; // 16 devices.

        dwRC = SS_COMP;
        break;

    case SC_GET_DEV_TYPE:
    {
        dprintf(("SC_GET_DEV_TYPE %d %d %d", lpSRB->devtype.SRB_HaId, lpSRB->devtype.SRB_Target, lpSRB->devtype.SRB_Lun));

        /* FIXME: We should return SS_NO_DEVICE if the device is not configured */
        /* FIXME: We should return SS_INVALID_HA if HostAdapter!=0 */
        SRB		tmpsrb;
        char	inqbuf[200];
        DWORD	ret;

        memset(&tmpsrb,0,sizeof(tmpsrb));

        /* Copy header */
        memcpy(&tmpsrb.common,&(lpSRB->common),sizeof(tmpsrb.common));

        tmpsrb.cmd.SRB_Flags	|= 8; /* target to host */
        tmpsrb.cmd.SRB_Cmd 		= SC_EXEC_SCSI_CMD;
        tmpsrb.cmd.SRB_Target	= lpSRB->devtype.SRB_Target;
        tmpsrb.cmd.SRB_Lun		= lpSRB->devtype.SRB_Lun;
        tmpsrb.cmd.SRB_BufLen	= sizeof(inqbuf);
        tmpsrb.cmd.SRB_BufPointer	= (BYTE *)inqbuf;
        tmpsrb.cmd.CDBByte[0]	= 0x12; /* INQUIRY  */
                          /* FIXME: handle lun */
        tmpsrb.cmd.CDBByte[4]	= sizeof(inqbuf);
        tmpsrb.cmd.SRB_CDBLen	= 6;

        ret = ASPI_ExecScsiCmd(&tmpsrb.cmd);

        lpSRB->devtype.SRB_Status	  = tmpsrb.cmd.SRB_Status;
        lpSRB->devtype.SRB_DeviceType = inqbuf[0] & 0x1f;

        TRACE("returning devicetype %d for target %d\n", inqbuf[0] & 0x1f, tmpsrb.cmd.SRB_Target);

        if (ret!=SS_PENDING) /* Any error is passed down directly */
            return ret;
        /* FIXME: knows that the command is finished already, pass final Status */
        return tmpsrb.cmd.SRB_Status;
    }

    case SC_EXEC_SCSI_CMD:
        dwRC = ASPI_ExecScsiCmd(&lpSRB->cmd);
        break;

    case SC_ABORT_SRB:
        dprintf(("SC_ABORT_SRB %d", lpSRB->abort.SRB_HaId));
        dwRC = SS_INVALID_SRB; // We don't do async ASPI so no way to abort
        break;

    case SC_RESET_DEV:
    {
        HCDIO hCDIO = OSLibCdIoGetDevice(lpSRB->reset.SRB_HaId, lpSRB->reset.SRB_Target, lpSRB->reset.SRB_Lun);
        if(hCDIO == -1) {
            return SS_NO_DEVICE;
        }
        dprintf(("SC_RESET_DEV %d %d %d", lpSRB->reset.SRB_HaId, lpSRB->reset.SRB_Target, lpSRB->reset.SRB_Lun));
        OSLibCdIoResetUnit(hCDIO); 
        dwRC = SS_COMP;
        break;
    }

    case SC_GET_DISK_INFO:
        /* here we have to find out the int13 / bios association.
         * We just say we do not have any.
         */
        FIXME("SC_GET_DISK_INFO always return 'int13 unassociated disk'.\n");
        lpSRB->diskinfo.SRB_DriveFlags = 0; /* disk is not int13 served */
        return SS_COMP;

    default:
	    dprintf(("Invalid command!"));
     	return SS_INVALID_SRB; // shoud be invalid command

    } // end switch (lpSRB->common.SRB_Cmd)

    dprintf(("SendASPI32Command %x %d done", lpSRB, lpSRB->common.SRB_Cmd));
    return dwRC;
}
//*****************************************************************************
//   GetASPI32DLLVersion32   (WNASPI32.3)
//*****************************************************************************
DWORD CDECL GetASPI32DLLVersion()
{
    return (DWORD)1;
}
//*****************************************************************************
//*****************************************************************************


