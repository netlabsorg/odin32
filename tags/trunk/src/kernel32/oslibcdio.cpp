/*
 * OS2CDROM.DMD interface for sending SCSI commands
 *
 * Based on example code by:
 *	(c) 2001 S&T Systemtechnik GmbH					                    
 *									                                    
 *	erzeugt:	15.02.01	R.Ihle				                        
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define	INCL_NOPMAPI
#define	INCL_DOSDEVICES
#define	INCL_DOSDEVIOCTL
#define	INCL_DOSFILEMGR
#define	INCL_DOSMISC
#include <os2wrap.h>

#include "cdioctl.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "oslibCdIo.h"
#include <win32type.h>


//needed for high memory support check
DWORD SYSTEM OSLibDosDevIOCtl( DWORD hFile, DWORD dwCat, DWORD dwFunc,
                               PVOID pParm, DWORD dwParmMaxLen, DWORD *pdwParmLen,
                               PVOID pData, DWORD dwDataMaxLen, DWORD *pdwDataLen);

static BOOL fDisableCDIo = FALSE;

//----------------------------------------------------------------------//
//	Open drive							//
//----------------------------------------------------------------------//
void WIN32API DisableCDIo()
{
    fDisableCDIo = TRUE;
}

//----------------------------------------------------------------------//
//	Initialize CD/DVD drive access					//
//----------------------------------------------------------------------//

BOOL OSLibCdIoInitialize(void)
{
    ULONG ulAction, ulFeatures;
    int	  fResult = 0;
    HFILE hDev;

    if(fDisableCDIo) {
        return FALSE;
    }

    if( DosOpen("CD-ROM2$", &hDev, &ulAction, 0, FILE_NORMAL,
	            OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
	            OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE |
	            OPEN_FLAGS_FAIL_ON_ERROR, NULL) == 0 )
    {
        fResult = DosDevIOCtl(hDev, IOCTL_CDROMDISK2, CDROMDISK2_FEATURES,
			                  NULL, 0, NULL, &ulFeatures, sizeof(ulFeatures), NULL) == 0 &&
	              (ulFeatures & FEATURE_EXECMD_SUPPORT);

        DosClose(hDev);
    }

    return fResult;
}


//----------------------------------------------------------------------//
//	Unload ASPI manager						//
//----------------------------------------------------------------------//

BOOL OSLibCdIoTerminate(void)
{
    return 1;
}


//----------------------------------------------------------------------//
//	Open drive							//
//----------------------------------------------------------------------//

BOOL OSLibCdIoIsSupported(HFILE hDisk)
{
    ULONG ulAction, ulCDSig;

    ulCDSig = ('C') | ('D' << 8) | ('9' << 16) | ('9' << 24);

    if(OSLibCdIoInitialize() == FALSE) {
        return FALSE;
    }

    if(hDisk == 0) {//no media present; return success here as the DosDevIOCtl will fail
        return TRUE;
    }

    if(DosDevIOCtl(hDisk, IOCTL_CDROMDISK, CDROMDISK_GETDRIVER, &ulCDSig, sizeof(ulCDSig), NULL,
		           &ulCDSig, sizeof(ulCDSig), NULL) == 0 &&
	    ulCDSig == (('C') | ('D' << 8) | ('0' << 16) | ('1' << 24)) )
    {
        return TRUE;
    }
    return FALSE;
}


//----------------------------------------------------------------------//
//	Send command to drive						//
//----------------------------------------------------------------------//

BOOL OSLibCdIoSendCommand(HCDIO hCdIo, CDIO_CMD_BUFFER *pCmdBuf, void *pData, unsigned cbData)
{
    struct ExecCmd	Param;
    ULONG			ulDummy, ParmLen, DataLen;

    if( !pData || !cbData )
    {
        ulDummy = 0;  pData = &ulDummy;  cbData = sizeof(ulDummy);
    }

    Param.ID_code     = 'C' | ('D'<<8) | ('0'<<16) | ('1'<<24);
    Param.data_length = (USHORT)cbData;
    Param.cmd_length  = pCmdBuf->cbCDB;
    Param.flags       = (USHORT)
		      (( pCmdBuf->flDirection & 1 ) ? EX_DIRECTION_IN : 0);
    memcpy(Param.cmd_buffer, pCmdBuf->arCDB, sizeof(Param.cmd_buffer));


    ParmLen = sizeof(Param);
    DataLen = cbData;
    return OSLibDosDevIOCtl((HFILE)hCdIo, IOCTL_CDROMDISK, CDROMDISK_EXECMD,
		                     &Param, ParmLen, &ParmLen, pData, cbData, &DataLen) == 0;
}


//----------------------------------------------------------------------//
//	Load / Eject disk						//
//----------------------------------------------------------------------//

BOOL OSLibCdIoLoadEjectDisk(HCDIO hCdIo, int fLoad)
{
    CDIO_CMD_BUFFER	CmdBuf;

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_OUTPUT;
    CmdBuf.cbCDB       = 6;
    CmdBuf.arCDB[0]    = 0x1b;		// START STOP UNIT command
    CmdBuf.arCDB[4]    = (unsigned char)(( fLoad ) ?  0x03 : 0x02);
					    // Bit 1: 0:Start/Stop, 1:Load/Eject
					    // Bit 0: 0:Stop/Eject, 1:Start/Load
    return OSLibCdIoSendCommand(hCdIo, &CmdBuf, NULL, 0);
}


//----------------------------------------------------------------------//
//	Test drive readyness						//
//----------------------------------------------------------------------//

BOOL OSLibCdIoUnitReady(HCDIO hCdIo)
{
    CDIO_CMD_BUFFER	CmdBuf;

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_OUTPUT;
    CmdBuf.cbCDB       = 6;
//  CmdBuf.arCDB[0]    = 0x00;		// TEST UNIT READY command

    return OSLibCdIoSendCommand(hCdIo, &CmdBuf, NULL, 0);
}


//----------------------------------------------------------------------//
//	Reset drive							//
//----------------------------------------------------------------------//

BOOL OSLibCdIoResetUnit(HCDIO hCdIo)
{
    CDIO_CMD_BUFFER	CmdBuf;

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_OUTPUT;
    CmdBuf.cbCDB       = 6;
    CmdBuf.arCDB[0]    = 0x01;		// REZERO command

    return OSLibCdIoSendCommand(hCdIo, &CmdBuf, NULL, 0);
}


//----------------------------------------------------------------------//
//	Query information about drive					//
//----------------------------------------------------------------------//

BOOL OSLibCdIoInquiry(HCDIO hCdIo, unsigned uPageCode, void *pData, unsigned cbData)
{
    CDIO_CMD_BUFFER	CmdBuf;

    if( cbData > 255 )	cbData = 255;

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_INPUT;
    CmdBuf.cbCDB       = 6;
    CmdBuf.arCDB[0]    = 0x12;		// INQUIRY CAPACITY command
    CmdBuf.arCDB[4]    = (unsigned char)cbData;

    if( uPageCode )
    {
        CmdBuf.arCDB[1]  = 1;		// enable vital product data
        CmdBuf.arCDB[2]  = (unsigned char)uPageCode;
    }

    return OSLibCdIoSendCommand(hCdIo, &CmdBuf, pData, cbData);
}


//----------------------------------------------------------------------//
//	Query sense information						//
//----------------------------------------------------------------------//

BOOL OSLibCdIoRequestSense(HCDIO hCdIo, void *pData, unsigned cbData)
{
    CDIO_CMD_BUFFER	CmdBuf;

    if( cbData > 255 )	cbData = 255;

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_INPUT;
    CmdBuf.cbCDB       = 6;
    CmdBuf.arCDB[0]    = 0x03;		// REQUEST SENSE command
    CmdBuf.arCDB[4]    = (unsigned char)cbData;

    return OSLibCdIoSendCommand(hCdIo, &CmdBuf, pData, cbData);
}


//----------------------------------------------------------------------//
//	Query disc capacity						//
//----------------------------------------------------------------------//

BOOL OSLibCdIoQueryCapacity(HCDIO hCdIo, unsigned *pctBlocks, unsigned *pcbBlock)
{
    CDIO_CMD_BUFFER	CmdBuf;
    unsigned		arResult[2];

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_INPUT;
    CmdBuf.cbCDB       = 10;
    CmdBuf.arCDB[0]    = 0x25;		// READ CAPACITY command

    if( OSLibCdIoSendCommand(hCdIo, &CmdBuf, arResult, sizeof(arResult)) )
    {
        *pctBlocks = (arResult[0] << 24) |
		    ((arResult[0] & 0x0000ff00) << 8) |
		    ((arResult[0] & 0x00ff0000) >> 8) |
		    (arResult[0] >> 24);
    
        *pcbBlock =  (arResult[1] << 24) |
		    ((arResult[1] & 0x0000ff00) << 8) |
		    ((arResult[1] & 0x00ff0000) >> 8) |
		    (arResult[1] >> 24);

        return 1;
    }

    return 0;
}


//----------------------------------------------------------------------//
//	Read data							//
//----------------------------------------------------------------------//

BOOL OSLibCdIoReadBlock(HCDIO hCdIo, unsigned uLBA, void *pData, unsigned cbData)
{
    CDIO_CMD_BUFFER	CmdBuf;

    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.flDirection = CMDDIR_INPUT;
    CmdBuf.cbCDB       = 10;
//  CmdBuf.arCDB[0]    = 0x3e;		// READ LONG command MODE SELECT ???
//  CmdBuf.arCDB[1]    = 0x02;		// Bit 1: perform ECC

    CmdBuf.arCDB[0]    = 0x28;		// READ command
    CmdBuf.arCDB[2]    = (unsigned char)(uLBA >> 24);
    CmdBuf.arCDB[3]    = (unsigned char)(uLBA >> 16);
    CmdBuf.arCDB[4]    = (unsigned char)(uLBA >> 8);
    CmdBuf.arCDB[5]    = (unsigned char)(uLBA);
    CmdBuf.arCDB[7]    = 0;
    CmdBuf.arCDB[8]    = 1;

    return ( cbData > 2047 ) ? OSLibCdIoSendCommand(hCdIo, &CmdBuf, pData, cbData) : 0;
}


//----------------------------------------------------------------------//
//	Query bit mask of all CD drives					//
//----------------------------------------------------------------------//

ULONG OSLibCdIoQueryDriveList(void)
{
    BIOSPARAMETERBLOCK    BPB;
    HFILE			hDev;
    struct DrvInfo	CDInfo;
    ULONG			ulAction, ulParam, uDriveMap = 0;

    if( DosOpen("CD-ROM2$", &hDev, &ulAction, 0, FILE_NORMAL,
	        OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
	        OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE |
	        OPEN_FLAGS_FAIL_ON_ERROR, NULL) == 0 )
    {
        if( DosDevIOCtl(hDev, IOCTL_CDROMDISK2, CDROMDISK2_DRVINFO,
		        NULL, 0, NULL, &CDInfo, sizeof(CDInfo), NULL) == 0 )
        {
        // Workaround for Warp4/FP14 kernel bug ...
        ulParam = CDInfo.usFirstDrvNo << 8;
        while( CDInfo.usFirstDrvNo <= ('Z' - 'A') &&
	        DosDevIOCtl((HFILE)-1, IOCTL_DISK, DSK_GETDEVICEPARAMS,
			            &ulParam, 2, NULL, &BPB, sizeof(BPB), NULL) == 0 &&
			            BPB.bDeviceType != DEVTYPE_UNKNOWN )
        {
	            ulParam = ++CDInfo.usFirstDrvNo;
        }

        if( CDInfo.usFirstDrvNo > ('Z' - 'A') )	CDInfo.usDrvCount = 0;

        while( CDInfo.usDrvCount-- )
	            uDriveMap |= 1 << (CDInfo.usFirstDrvNo + CDInfo.usDrvCount);
        }

        DosClose(hDev);
    }
    
    return uDriveMap;
}


//----------------------------------------------------------------------//
//	Query fully qualified path name of a given file			//
//----------------------------------------------------------------------//

BOOL OSLibCdIoQueryFullName(char *pchPartName, char *pchFullName, unsigned cbFullName)
{
    APIRET	rc;

    DosError(FERR_DISABLEHARDERR);
    rc = DosQueryPathInfo(pchPartName, FIL_QUERYFULLNAME, pchFullName, cbFullName);
    DosError(FERR_ENABLEHARDERR | FERR_ENABLEEXCEPTION);

    return rc == 0;
}


//----------------------------------------------------------------------//
//	Read volume label		 				//
//----------------------------------------------------------------------//
BOOL OSLibCdIoQueryVolumeLabel(char chDrive, char *pchLabel, unsigned cbLabel)
{
    FSINFO	FsInfo;

    memset(pchLabel, '\0', cbLabel);
    if( cbLabel && DosQueryFSInfo((chDrive & 0x5f) - 'A' + 1,
				    FSIL_VOLSER, &FsInfo, sizeof(FsInfo)) == 0 )
    {
        strncpy(pchLabel, FsInfo.vol.szVolLabel, cbLabel);
        pchLabel[cbLabel-1] = '\0';
        return 1;
    }

    return 0;
}


