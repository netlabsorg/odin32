/* -*- tab-width: 8; c-basic-offset: 4 -*- */
/*
 * MCI CDAUDIO Driver for OS/2
 *
 * Copyright 1994 Martin Ayotte
 * Copyright 1999 Eric Pouech
 * Copyright 2000 Chris Wohlgemuth
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __WINE_CDROM_H__
#define __WINE_CDROM_H__

#include <stdlib.h>
// #include <unistd.h>
//#include "windef.h"

#ifdef HAVE_LINUX_CDROM_H
# include <linux/cdrom.h>
#endif
#ifdef HAVE_LINUX_UCDROM_H
# include <linux/ucdrom.h>
#endif
#ifdef HAVE_SYS_CDIO_H
# include <sys/cdio.h>
#endif

typedef struct {
    int				unixdev;
#if defined(linux)
    struct cdrom_subchnl	sc;
#elif defined(__FreeBSD__) || defined(__NetBSD__)
    struct cd_sub_channel_info	sc;
#endif
    char                        chrDrive[3];
    char                        cPad;
    char                        chrDeviceType[128];
    ULONG                       hfOS2Handle;
    ULONG                       ulLeadOut;
    /* those data reflect the cdaudio structure and
     * don't change while playing
     */
    UINT16			nTracks;
    UINT16			nFirstTrack;
    UINT16			nLastTrack;
    LPDWORD			lpdwTrackLen;
    LPDWORD			lpdwTrackPos;
    LPBYTE			lpbTrackFlags;
    DWORD			dwFirstFrame;
    DWORD			dwLastFrame;
    /* those data change while playing */
    ULONG                       ulCDROMStatus;
    USHORT                       usCDAudioStatus;
    int				cdaMode;
    UINT			nCurTrack;
    DWORD			dwCurFrame;
} WINE_CDAUDIO;

typedef struct {
    UINT		wDevID;
    int     		nUseCount;          /* Incremented for each shared open */
    BOOL  		fShareable;         /* TRUE if first open was shareable */
    WORD    		wNotifyDeviceID;    /* MCI device ID with a pending notification */
    HANDLE 		hCallback;          /* Callback handle for pending notification */
    DWORD		dwTimeFormat;
    WINE_CDAUDIO	wcda;
    int			mciMode;
} WINE_MCICDAUDIO;


#define	WINE_CDA_DONTKNOW		0x00
#define	WINE_CDA_NOTREADY		0x01
#define	WINE_CDA_OPEN			0x02
#define	WINE_CDA_PLAY			0x03
#define	WINE_CDA_STOP			0x04
#define	WINE_CDA_PAUSE			0x05

#ifndef MCI_CONFIGURE
#define MCI_CONFIGURE                  0x087A
#endif

int	CDAUDIO_Open(WINE_CDAUDIO* wcda);
int	CDAUDIO_Close(WINE_CDAUDIO* wcda);
int	CDAUDIO_Reset(WINE_CDAUDIO* wcda);
int	CDAUDIO_Play(WINE_CDAUDIO* wcda, DWORD start, DWORD stop);
int	CDAUDIO_Stop(WINE_CDAUDIO* wcda);
int	CDAUDIO_Pause(WINE_CDAUDIO* wcda, int pauseOn);
int	CDAUDIO_Seek(WINE_CDAUDIO* wcda, DWORD at);
int	CDAUDIO_SetDoor(WINE_CDAUDIO* wcda, int open);
int 	CDAUDIO_GetNumberOfTracks(WINE_CDAUDIO* wcda);
BOOL 	CDAUDIO_GetTracksInfo(WINE_CDAUDIO* wcda);
BOOL	CDAUDIO_GetCDStatus(WINE_CDAUDIO* wcda);

HFILE os2CDOpen(char *drive);
ULONG os2CDClose(ULONG hfOS2Handle);
int os2GetNumTracks(HFILE hfOS2Handle,ULONG *ulLeadOut);
BOOL os2GetCDStatus(HFILE hfOS2Handle, ULONG  *ulStatus);
BOOL os2GetCDAudioStatus(HFILE hfOS2Handle, USHORT  *usStatus);
ULONG  os2CDQueryTrackStartSector( HFILE hfDrive, ULONG numTrack, BOOL *flAudio);
BOOL os2CDEject(HFILE hfDrive);
BOOL os2CDCloseTray(HFILE hfDrive);
BOOL os2CDStop(HFILE hfDrive);
BOOL os2CDPlayRange(HFILE hfDrive ,ULONG ulFrom, ULONG ulTo);
BOOL  os2CDResume(HFILE hfDrive);
BOOL os2CDGetHeadLocation(HFILE hfOS2Handle, ULONG  *ulHeadLocation);
BOOL os2CDSeek(HFILE hfOS2Handle, ULONG  ulTo);
BOOL os2CDQueryCurTrack(HFILE hfOS2Handle, UINT  * uiCurTrack);
BOOL os2CDQueryCDDrives(int *iNumCD, char * cFirstDrive);

#define CDFRAMES_PERSEC 		75
#define SECONDS_PERMIN	 		60
#define CDFRAMES_PERMIN 		((CDFRAMES_PERSEC) * (SECONDS_PERMIN))

#ifndef CDROM_DATA_TRACK
#define CDROM_DATA_TRACK 0x04
#endif

#endif

