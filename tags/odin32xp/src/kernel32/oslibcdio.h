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

#ifndef	__CDIO_H
#define	__CDIO_H

#ifdef	__cplusplus
  extern "C" {
#endif

typedef ULONG HCDIO;

typedef struct
{
  unsigned	flDirection;		// direction flag
  unsigned	cbCDB;			    // command length
  unsigned char	arCDB[16];		// actual CdIo command
} CDIO_CMD_BUFFER;

#define	CMDDIR_OUTPUT		0
#define	CMDDIR_INPUT		1

BOOL    OSLibCdIoInitialize(void);
BOOL    OSLibCdIoTerminate(void);

BOOL    OSLibCdIoIsSupported(HFILE hDisk);
BOOL 	OSLibCdIoSendCommand(HCDIO hCdIo, CDIO_CMD_BUFFER *pCmd, void *pData, unsigned cbData);

BOOL    OSLibCdIoLoadEjectDisk(HCDIO hCdIo, int fLoad);
BOOL    OSLibCdIoResetUnit(HCDIO hCdIo);
BOOL    OSLibCdIoUnitReady(HCDIO hCdIo);
BOOL    OSLibCdIoInquiry(HCDIO hCdIo, unsigned uPageCode, void *pData, unsigned cbData);
BOOL    OSLibCdIoRequestSense(HCDIO hCdIo, void *pData, unsigned cbData);
BOOL    OSLibCdIoQueryCapacity(HCDIO hCdIo, unsigned *pctBlocks, unsigned *pcbBlock);
BOOL    OSLibCdIoReadBlock(HCDIO hCdIo, unsigned uLBA, void *pData, unsigned cbData);

ULONG   OSLibCdIoQueryDriveList(void);
BOOL	OSLibCdIoQueryFullName(char *pchPartName, char *pchFullName, unsigned cbFullName);
BOOL	OSLibCdIoQueryVolumeLabel(char chDrive, char *pchLabel, unsigned cbLabel);

#ifdef	__cplusplus
  }
#endif

#endif


