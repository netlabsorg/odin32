/* -*- tab-width: 8; c-basic-offset: 4 -*- */
/*
 * MCI CDAUDIO Driver for OS/2
 *
 * Copyright 1994    Martin Ayotte
 * Copyright 1998-99 Eric Pouech
 * Copyright 2000    Chris Wohlgemuth
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <string.h>
#include "winuser.h"
////#include "driver.h"
#include "mmddk.h"
#include "cdrom.h"
#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(cdaudio)


/*-----------------------------------------------------------------------*/

/**************************************************************************
 * 				CDAUDIO_drvOpen			[internal]	
 */
static	DWORD	CDAUDIO_drvOpen(LPSTR str, LPMCI_OPEN_DRIVER_PARMSA modp)
{
    WINE_MCICDAUDIO*	wmcda = (WINE_MCICDAUDIO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,  sizeof(WINE_MCICDAUDIO));

    if (!wmcda)
	return 0;

    wmcda->wDevID = modp->wDeviceID;
    mciSetDriverData(wmcda->wDevID, (DWORD)wmcda);
    modp->wCustomCommandTable = MCI_NO_COMMAND_TABLE;
    modp->wType = MCI_DEVTYPE_CD_AUDIO;
    return modp->wDeviceID;
}

/**************************************************************************
 * 				CDAUDIO_drvClose		[internal]	
 */
static	DWORD	CDAUDIO_drvClose(DWORD dwDevID)
{
    WINE_MCICDAUDIO*  wmcda = (WINE_MCICDAUDIO*)mciGetDriverData(dwDevID);

    if (wmcda) {
	HeapFree(GetProcessHeap(), 0, wmcda);
	mciSetDriverData(dwDevID, 0);
    }
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciGetOpenDrv		[internal]	
 */
static WINE_MCICDAUDIO*  CDAUDIO_mciGetOpenDrv(UINT wDevID)
{
    WINE_MCICDAUDIO*	wmcda = (WINE_MCICDAUDIO*)mciGetDriverData(wDevID);
    
    if (wmcda == NULL || wmcda->nUseCount == 0 /*|| wmcda->wcda.unixdev <= 0*/ ) {
	WARN("CDAUDIO_mciGetOpenDrv(UINT wDevID): Invalid wDevID=%u\n", wDevID);
	return 0;
    }
    return wmcda;
}

/**************************************************************************
 * 				CDAUDIO_mciMode			[internal]
 */
static	int	CDAUDIO_mciMode(int wcdaMode)
{
    switch (wcdaMode) {
    case WINE_CDA_DONTKNOW:	return MCI_MODE_STOP;/* 0x20d */
    case WINE_CDA_NOTREADY:	return MCI_MODE_STOP;/* 0x20d */
    case WINE_CDA_OPEN:		return MCI_MODE_OPEN;
    case WINE_CDA_PLAY:		return MCI_MODE_PLAY;
    case WINE_CDA_STOP:		return MCI_MODE_STOP;/* 0x20d */
    case WINE_CDA_PAUSE:	return MCI_MODE_PAUSE;
    default:
	FIXME("Unknown mode %04x\n", wcdaMode);
    }
    return MCI_MODE_STOP;
}

/**************************************************************************
 * 				CDAUDIO_mciGetError		[internal]
 */
static	int	CDAUDIO_mciGetError(WINE_MCICDAUDIO* wmcda)
{
    switch (wmcda->wcda.cdaMode) {
    case WINE_CDA_DONTKNOW:
    case WINE_CDA_NOTREADY:	return MCIERR_DEVICE_NOT_READY;
    case WINE_CDA_OPEN:		return MCIERR_DEVICE_OPEN;
    case WINE_CDA_PLAY:		
    case WINE_CDA_STOP:		
    case WINE_CDA_PAUSE:	break;
    default:
	FIXME("Unknown mode %04x\n", wmcda->wcda.cdaMode);
    }
    return MCIERR_DRIVER_INTERNAL;
}

/**************************************************************************
 * 			CDAUDIO_CalcFrame			[internal]
 */
static DWORD CDAUDIO_CalcFrame(WINE_MCICDAUDIO* wmcda, DWORD dwTime)
{
    DWORD	dwFrame = 0;
    UINT	wTrack;
    
    TRACE("(%p, %08lX, %lu);\n", wmcda, wmcda->dwTimeFormat, dwTime);
    
    switch (wmcda->dwTimeFormat) {
    case MCI_FORMAT_MILLISECONDS:
	dwFrame = ((dwTime - 1) * CDFRAMES_PERSEC + 500) / 1000;
	TRACE("MILLISECONDS %lu\n", dwFrame);
	break;
    case MCI_FORMAT_MSF:
	TRACE("MSF %02u:%02u:%02u\n",
	      MCI_MSF_MINUTE(dwTime), MCI_MSF_SECOND(dwTime), MCI_MSF_FRAME(dwTime));
	dwFrame += CDFRAMES_PERMIN * MCI_MSF_MINUTE(dwTime);
	dwFrame += CDFRAMES_PERSEC * MCI_MSF_SECOND(dwTime);
	dwFrame += MCI_MSF_FRAME(dwTime);
	break;
    case MCI_FORMAT_TMSF:
    default: /* unknown format ! force TMSF ! ... */
	wTrack = MCI_TMSF_TRACK(dwTime);
	TRACE("MSF %02u-%02u:%02u:%02u\n",
	      MCI_TMSF_TRACK(dwTime), MCI_TMSF_MINUTE(dwTime), 
	      MCI_TMSF_SECOND(dwTime), MCI_TMSF_FRAME(dwTime));
	TRACE("TMSF trackpos[%u]=%lu\n",
	      wTrack, wmcda->wcda.lpdwTrackPos[wTrack - 1]);
	dwFrame = wmcda->wcda.lpdwTrackPos[wTrack - 1];
	dwFrame += CDFRAMES_PERMIN * MCI_TMSF_MINUTE(dwTime);
	dwFrame += CDFRAMES_PERSEC * MCI_TMSF_SECOND(dwTime);
	dwFrame += MCI_TMSF_FRAME(dwTime);
	break;
    }
    return dwFrame;
}

/**************************************************************************
 * 			CDAUDIO_CalcTime			[internal]
 */
static DWORD CDAUDIO_CalcTime(WINE_MCICDAUDIO* wmcda, DWORD tf, DWORD dwFrame, 
			      LPDWORD lpRet)
{
    DWORD	dwTime = 0;
    UINT	wTrack;
    UINT	wMinutes;
    UINT	wSeconds;
    UINT	wFrames;
    
    TRACE("(%p, %08lX, %lu);\n", wmcda, tf, dwFrame);
    
    switch (tf) {
    case MCI_FORMAT_MILLISECONDS:
	dwTime = (dwFrame * 1000) / CDFRAMES_PERSEC + 1;
	TRACE("MILLISECONDS %lu\n", dwTime);
	*lpRet = 0;
	break;
    case MCI_FORMAT_MSF:
	wMinutes = dwFrame / CDFRAMES_PERMIN;
	wSeconds = (dwFrame - CDFRAMES_PERMIN * wMinutes) / CDFRAMES_PERSEC;
	wFrames = dwFrame - CDFRAMES_PERMIN * wMinutes - CDFRAMES_PERSEC * wSeconds;
	dwTime = MCI_MAKE_MSF(wMinutes, wSeconds, wFrames);
	TRACE("MSF %02u:%02u:%02u -> dwTime=%lu\n",
	      wMinutes, wSeconds, wFrames, dwTime);
	*lpRet = MCI_COLONIZED3_RETURN;
	break;
    case MCI_FORMAT_TMSF:
    default:	/* unknown format ! force TMSF ! ... */
	if (dwFrame < wmcda->wcda.dwFirstFrame || dwFrame > wmcda->wcda.dwLastFrame) {
	    ERR("Out of range value %lu [%lu,%lu]\n", 
		dwFrame, wmcda->wcda.dwFirstFrame, wmcda->wcda.dwLastFrame);
	    *lpRet = 0;
	    return 0;
	}
	for (wTrack = 1; wTrack < wmcda->wcda.nTracks; wTrack++) {
	    if (wmcda->wcda.lpdwTrackPos[wTrack] > dwFrame)
		break;
	}
	dwFrame -= wmcda->wcda.lpdwTrackPos[wTrack - 1];
	wMinutes = dwFrame / CDFRAMES_PERMIN;
	wSeconds = (dwFrame - CDFRAMES_PERMIN * wMinutes) / CDFRAMES_PERSEC;
	wFrames = dwFrame - CDFRAMES_PERMIN * wMinutes - CDFRAMES_PERSEC * wSeconds;
	dwTime = MCI_MAKE_TMSF(wTrack, wMinutes, wSeconds, wFrames);
	TRACE("%02u-%02u:%02u:%02u\n", wTrack, wMinutes, wSeconds, wFrames);
	*lpRet = MCI_COLONIZED4_RETURN;
	break;
    }
    return dwTime;
}

static DWORD CDAUDIO_mciSeek(UINT wDevID, DWORD dwFlags, LPMCI_SEEK_PARMS lpParms);
static DWORD CDAUDIO_mciStop(UINT wDevID, DWORD dwFlags, LPMCI_GENERIC_PARMS lpParms);

/**************************************************************************
 * 				CDAUDIO_mciOpen			[internal]
 */
static DWORD CDAUDIO_mciOpen(UINT wDevID, DWORD dwFlags, LPMCI_OPEN_PARMSA lpOpenParms)
{
    DWORD		dwDeviceID;
    WINE_MCICDAUDIO* 	wmcda = (WINE_MCICDAUDIO*)mciGetDriverData(wDevID);
    MCI_SEEK_PARMS 	seekParms;

    dprintf(("CDAUDIO_mciOpen: (wDevID: %04X, dwFlags: %08lX, %p);\n", wDevID, dwFlags, lpOpenParms));
    
    
    if (lpOpenParms == NULL) 		return MCIERR_NULL_PARAMETER_BLOCK;
    if (wmcda == NULL)			return MCIERR_INVALID_DEVICE_ID;

    dwDeviceID = lpOpenParms->wDeviceID;

    if (wmcda->nUseCount > 0) {
	/* The driver is already open on this channel */
	/* If the driver was opened shareable before and this open specifies */
	/* shareable then increment the use count */
	if (wmcda->fShareable && (dwFlags & MCI_OPEN_SHAREABLE))
	    ++wmcda->nUseCount;
	else
	    return MCIERR_MUST_USE_SHAREABLE;
    } else {
	wmcda->nUseCount = 1;
	wmcda->fShareable = dwFlags & MCI_OPEN_SHAREABLE;
    }
    if (dwFlags & MCI_OPEN_ELEMENT) {
        if (dwFlags & MCI_OPEN_ELEMENT_ID) {
            WARN("MCI_OPEN_ELEMENT_ID %8lx ! Abort", (DWORD)lpOpenParms->lpstrElementName);
            return MCIERR_NO_ELEMENT_ALLOWED;
        }
        /* Cdplayer.exe puts the driveletter into lpOpenParms->lpstrElementName */
        TRACE("CDAUDIO_mciOpen(): MCI_OPEN_ELEMENT: %s\n",lpOpenParms->lpstrElementName);

        if(strlen(lpOpenParms->lpstrElementName)==2)
            strcpy(wmcda->wcda.chrDrive,lpOpenParms->lpstrElementName);/* Save */
        /*return MCIERR_NO_ELEMENT_ALLOWED; 
          bon 19991106 allows cdplayer.exe to run*/
    }
    else if (dwFlags & MCI_OPEN_TYPE) {/*MCI_OPEN_TYPE  0x00002000L*/
        TRACE("CDAUDIO_mciOpen(): MCI_OPEN_TYPE: %s\n",lpOpenParms->lpstrDeviceType);
        wmcda->wcda.chrDrive[0]=0; /* Mark that we don't have a drive */
        if(lpOpenParms->lpstrDeviceType)
            strncpy(wmcda->wcda.chrDeviceType,lpOpenParms->lpstrDeviceType,sizeof(wmcda->wcda.chrDeviceType)-1);
        else
            wmcda->wcda.chrDeviceType[0]=0;
    }

    wmcda->wNotifyDeviceID = dwDeviceID;


    if (!CDAUDIO_Open(&wmcda->wcda)) {
#if 0
        /* We're always successful because we open the driver not the disk */
	--wmcda->nUseCount;
	return MCIERR_HARDWARE;
#endif
    }

    wmcda->mciMode = MCI_MODE_STOP;
    wmcda->dwTimeFormat = MCI_FORMAT_MSF;
    if (!CDAUDIO_GetTracksInfo(&wmcda->wcda)) {
	WARN("error reading TracksInfo !\n");
        //CDAUDIO_Close(&wmcda->wcda);
	//return MCIERR_INTERNAL;
    }
    
    //CDAUDIO_mciSeek(wDevID, MCI_SEEK_TO_START, &seekParms);

    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciClose		[internal]
 */
static DWORD CDAUDIO_mciClose(UINT wDevID, DWORD dwParam, LPMCI_GENERIC_PARMS lpParms)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);

    TRACE("Entering CDAUDIO_mciClose... (%04X, %08lX, %p) wmcd: %X;\n", wDevID, dwParam, lpParms, wmcda);

    if (wmcda == NULL) 	return MCIERR_INVALID_DEVICE_ID;
    
    if (wmcda->nUseCount == 1) {
	CDAUDIO_Close(&wmcda->wcda);
    }
    wmcda->nUseCount--;
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciGetDevCaps		[internal]
 */
static DWORD CDAUDIO_mciGetDevCaps(UINT wDevID, DWORD dwFlags, 
				   LPMCI_GETDEVCAPS_PARMS lpParms)
{
    DWORD	ret = 0;

    TRACE("CDAUDIO_mciGetDevCaps: (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);

    if (lpParms == NULL) return MCIERR_NULL_PARAMETER_BLOCK;

    if (dwFlags & MCI_GETDEVCAPS_ITEM) {
	TRACE("MCI_GETDEVCAPS_ITEM dwItem=%08lX;\n", lpParms->dwItem);

	switch (lpParms->dwItem) {
	case MCI_GETDEVCAPS_CAN_RECORD: /* 0x00000001L */
	    lpParms->dwReturn = FALSE;
            break;
	case MCI_GETDEVCAPS_HAS_AUDIO: /* 0x00000002L */
            lpParms->dwReturn = TRUE;
	    break;
	case MCI_GETDEVCAPS_HAS_VIDEO: /* 0x00000003L */
	    lpParms->dwReturn = FALSE;
            break;
	case MCI_GETDEVCAPS_DEVICE_TYPE: /* 0x00000004L */
	    lpParms->dwReturn = MCI_DEVTYPE_CD_AUDIO;
	    break;
	case MCI_GETDEVCAPS_USES_FILES: /* 0x00000005L */
	    lpParms->dwReturn = FALSE;
	    break;
	case MCI_GETDEVCAPS_COMPOUND_DEVICE: /* 0x00000006L */
	    lpParms->dwReturn = FALSE;
	    break;
	case MCI_GETDEVCAPS_CAN_EJECT: /* 0x00000007L*/
            lpParms->dwReturn = TRUE;
	    break;
	case MCI_GETDEVCAPS_CAN_PLAY: /* 0x00000008L*/
            lpParms->dwReturn = TRUE;
	    break;
	case MCI_GETDEVCAPS_CAN_SAVE: /* 0x00000009L */
	    lpParms->dwReturn = FALSE;
	    break;
	default:
	    ERR("Unsupported %lx devCaps item\n", lpParms->dwItem);
	    return MCIERR_UNRECOGNIZED_COMMAND;
	}
    } else {
	TRACE("No GetDevCaps-Item !\n");
	return MCIERR_UNRECOGNIZED_COMMAND;
    }
    TRACE("lpParms->dwReturn=%08lX;\n", lpParms->dwReturn);
    return ret;

#if 0
    /* This is Wine stuff */
    if (dwFlags & MCI_GETDEVCAPS_ITEM) {
	TRACE("MCI_GETDEVCAPS_ITEM dwItem=%08lX;\n", lpParms->dwItem);

	switch (lpParms->dwItem) {
	case MCI_GETDEVCAPS_CAN_RECORD: /* 0x00000001L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_HAS_AUDIO: /* 0x00000002L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_HAS_VIDEO: /* 0x00000003L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_DEVICE_TYPE: /* 0x00000004L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(MCI_DEVTYPE_CD_AUDIO, MCI_DEVTYPE_CD_AUDIO);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_USES_FILES: /* 0x00000005L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_COMPOUND_DEVICE: /* 0x00000006L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_CAN_EJECT: /* 0x00000007L*/
	    lpParms->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_CAN_PLAY: /* 0x00000008L*/
	    lpParms->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	case MCI_GETDEVCAPS_CAN_SAVE: /* 0x00000009L */
	    lpParms->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
	    ret = MCI_RESOURCE_RETURNED;
	    break;
	default:
	    ERR("Unsupported %lx devCaps item\n", lpParms->dwItem);
	    return MCIERR_UNRECOGNIZED_COMMAND;
	}
    } else {
	TRACE("No GetDevCaps-Item !\n");
	return MCIERR_UNRECOGNIZED_COMMAND;
    }
    TRACE("lpParms->dwReturn=%08lX;\n", lpParms->dwReturn);
    return ret;
#endif
}

/**************************************************************************
 * 				CDAUDIO_mciInfo			[internal]
 */
static DWORD CDAUDIO_mciInfo(UINT wDevID, DWORD dwFlags, LPMCI_INFO_PARMSA lpParms)
{
    LPSTR		str = NULL;
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    DWORD		ret = 0;
    char		buffer[16];

    TRACE("Entering CDAUDIO_mciInfo... (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (lpParms == NULL || lpParms->lpstrReturn == NULL)
	return MCIERR_NULL_PARAMETER_BLOCK;
    if (wmcda == NULL) return MCIERR_INVALID_DEVICE_ID;

    TRACE("buf=%p, len=%lu\n", lpParms->lpstrReturn, lpParms->dwRetSize);
    
    if (dwFlags & MCI_INFO_PRODUCT) {/* 0x00000100L */
	str = "Odins audio CD";
    } else if (dwFlags & MCI_INFO_MEDIA_UPC) {   /* 0x00000400L */
        //	ret = MCIERR_NO_IDENTITY;
	ret = 0; // No Info about MCIERR_NO_IDENTITY
    } else if (dwFlags & MCI_INFO_MEDIA_IDENTITY) { /* 0x00000800L */
	DWORD	msf, res = 0, dummy;
	int	i;

	if (!CDAUDIO_GetCDStatus(&wmcda->wcda)) {
	    return CDAUDIO_mciGetError(wmcda);
	}

	for (i = 0; i < wmcda->wcda.nTracks; i++) {
	    msf = CDAUDIO_CalcTime(wmcda, MCI_FORMAT_MSF, wmcda->wcda.lpdwTrackPos[i], &dummy);
	    res += (MCI_MSF_MINUTE(msf) << 16) +
                (MCI_MSF_SECOND(msf) << 8) +
                (MCI_MSF_FRAME(msf));
	}
	if (wmcda->wcda.nTracks <= 2) {
	    /* there are some other values added when # of tracks < 3
	     * for most Audio CD it will do without
 	     */
	    FIXME("Value is not correct !! "
                  "Please report will full audio CD information (-debugmsg +cdaudio)\n");
	}
	sprintf(buffer, "%lu", res);
	str = buffer;
    } else {
	WARN("Don't know this info command (%lu)\n", dwFlags);
	ret = MCIERR_UNRECOGNIZED_COMMAND;
    }
    if (str) {
	if (lpParms->dwRetSize <= strlen(str)) {
	    lstrcpynA(lpParms->lpstrReturn, str, lpParms->dwRetSize - 1);
	    ret = MCIERR_PARAM_OVERFLOW;
	} else {
	    strcpy(lpParms->lpstrReturn, str);
	}	
    } else {
	*lpParms->lpstrReturn = 0;
    }
    TRACE("CDAUDIO_mciInfo returns => %s (ret=%ld)\n", lpParms->lpstrReturn, ret);
    return ret;
}

/**************************************************************************
 * 				CDAUDIO_mciStatus		[internal]
 */
static DWORD CDAUDIO_mciStatus(UINT wDevID, DWORD dwFlags, LPMCI_STATUS_PARMS lpParms)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    DWORD	        ret = 0;

    TRACE("Entering CDAUDIO_mciStatus (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (lpParms == NULL) return MCIERR_NULL_PARAMETER_BLOCK;
    if (wmcda == NULL) return MCIERR_INVALID_DEVICE_ID;

    if (dwFlags & MCI_NOTIFY) {
	TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", lpParms->dwCallback);
	mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
			wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    if (dwFlags & MCI_STATUS_ITEM) { /* MCI_STATUS_ITEM 0x00000100L */
        TRACE("item: %lx!\n", lpParms->dwItem);
	switch (lpParms->dwItem) {
	case MCI_STATUS_CURRENT_TRACK: /* 0x00000008L */
	    if (!CDAUDIO_GetCDStatus(&wmcda->wcda)) {
		return CDAUDIO_mciGetError(wmcda);
	    }
	    lpParms->dwReturn = wmcda->wcda.nCurTrack;
	    TRACE("CURRENT_TRACK=%lu!\n", lpParms->dwReturn);
	    break;
	case MCI_STATUS_LENGTH: /* 0x00000001L */
	    if (wmcda->wcda.nTracks == 0) {
		if (!CDAUDIO_GetTracksInfo(&wmcda->wcda)) {
		    WARN("error reading TracksInfo !\n");
		    return CDAUDIO_mciGetError(wmcda);
		}
	    }
	    if (dwFlags & MCI_TRACK) {
		TRACE("MCI_TRACK #%lu LENGTH=??? !\n", lpParms->dwTrack);
		if (lpParms->dwTrack > wmcda->wcda.nTracks || lpParms->dwTrack == 0)
		    return MCIERR_OUTOFRANGE;
		lpParms->dwReturn = wmcda->wcda.lpdwTrackLen[lpParms->dwTrack - 1];
	    } else {
		lpParms->dwReturn = wmcda->wcda.dwLastFrame;
	    }
	    lpParms->dwReturn = CDAUDIO_CalcTime(wmcda, 
						 (wmcda->dwTimeFormat == MCI_FORMAT_TMSF) 
						    ? MCI_FORMAT_MSF : wmcda->dwTimeFormat,
						 lpParms->dwReturn,
						 &ret);
	    TRACE("LENGTH=%lu !\n", lpParms->dwReturn);
	    break;
	case MCI_STATUS_MODE: /* 0x00000004L */
	    if (!CDAUDIO_GetCDStatus(&wmcda->wcda)) 
		return CDAUDIO_mciGetError(wmcda);
	    lpParms->dwReturn = CDAUDIO_mciMode(wmcda->wcda.cdaMode);
	    if (!lpParms->dwReturn) 
                lpParms->dwReturn = wmcda->mciMode;
	    TRACE("MCI_STATUS_MODE=%08lX !\n", lpParms->dwReturn);
#if 0
            /* What's Wine doing here??? */
	    lpParms->dwReturn = MAKEMCIRESOURCE(lpParms->dwReturn, lpParms->dwReturn);
	    ret = MCI_RESOURCE_RETURNED;
#endif
	    break;
	case MCI_STATUS_MEDIA_PRESENT: /* 0x00000005L */
	    if (!CDAUDIO_GetCDStatus(&wmcda->wcda)) 
		return CDAUDIO_mciGetError(wmcda);
	    lpParms->dwReturn = (wmcda->wcda.nTracks == 0) ? FALSE : TRUE;
	    TRACE("MCI_STATUS_MEDIA_PRESENT =%c!\n", LOWORD(lpParms->dwReturn) ? 'Y' : 'N');
#if 0
            /* What's Wine doing here??? */
	    lpParms->dwReturn = (wmcda->wcda.nTracks == 0) ? 
		MAKEMCIRESOURCE(FALSE, MCI_FALSE) : MAKEMCIRESOURCE(TRUE, MCI_TRUE);
	    TRACE("MCI_STATUS_MEDIA_PRESENT =%c!\n", LOWORD(lpParms->dwReturn) ? 'Y' : 'N');
	    ret = MCI_RESOURCE_RETURNED;
#endif
	    break;
	case MCI_STATUS_NUMBER_OF_TRACKS: /* 0x00000003L */
	    lpParms->dwReturn = CDAUDIO_GetNumberOfTracks(&wmcda->wcda);
	    TRACE("MCI_STATUS_NUMBER_OF_TRACKS = %lu !\n", lpParms->dwReturn);
	    if (lpParms->dwReturn == (WORD)-1) 
		return CDAUDIO_mciGetError(wmcda);
	    break;
	case MCI_STATUS_POSITION: /* 0x00000002L */
	    if (!CDAUDIO_GetCDStatus(&wmcda->wcda)) 
		return CDAUDIO_mciGetError(wmcda);
	    lpParms->dwReturn = wmcda->wcda.dwCurFrame;
	    if (dwFlags & MCI_STATUS_START) {
                lpParms->dwReturn = wmcda->wcda.dwFirstFrame;
		TRACE("get MCI_STATUS_START !\n");
	    }
	    if (dwFlags & MCI_TRACK) {
		if (lpParms->dwTrack > wmcda->wcda.nTracks || lpParms->dwTrack == 0)
		    return MCIERR_OUTOFRANGE;
#ifndef __WIN32OS2__
                if(wmcda->wcda.nCurTrack)
#endif
                    lpParms->dwReturn = wmcda->wcda.lpdwTrackPos[lpParms->dwTrack - 1];
		//TRACE("get MCI_TRACK #%lu, track position: %lu !\n", lpParms->dwTrack, lpParms->dwReturn);
            }
	    lpParms->dwReturn = CDAUDIO_CalcTime(wmcda, wmcda->dwTimeFormat, lpParms->dwReturn, &ret);
	    TRACE("MCI_STATUS_POSITION=%08lX, track %d !\n", lpParms->dwReturn, lpParms->dwTrack);
	    break;
	case MCI_STATUS_READY: /* 0x00000007L */
	    TRACE("MCI_STATUS_READY !\n");
            lpParms->dwReturn = (wmcda->wcda.cdaMode == WINE_CDA_DONTKNOW ||
				 wmcda->wcda.cdaMode == WINE_CDA_NOTREADY) ? FALSE : TRUE;
	    TRACE("MCI_STATUS_READY=%u!\n", LOWORD(lpParms->dwReturn));
#if 0
            /* What's Wine doing here??? */
	    lpParms->dwReturn = (wmcda->wcda.cdaMode == WINE_CDA_DONTKNOW ||
				 wmcda->wcda.cdaMode == WINE_CDA_NOTREADY) ?
		MAKEMCIRESOURCE(FALSE, MCI_FALSE) : MAKEMCIRESOURCE(TRUE, MCI_TRUE);
	    TRACE("MCI_STATUS_READY=%u!\n", LOWORD(lpParms->dwReturn));
	    ret = MCI_RESOURCE_RETURNED;
#endif
	    break;
	case MCI_STATUS_TIME_FORMAT: /* 0x00000006L */
#if 0
            /* What's Wine doing here??? */
	    lpParms->dwReturn = MAKEMCIRESOURCE(wmcda->dwTimeFormat, wmcda->dwTimeFormat);
	    ret = MCI_RESOURCE_RETURNED;
#endif
            lpParms->dwReturn = wmcda->dwTimeFormat;
	    TRACE("MCI_STATUS_TIME_FORMAT=%08x!\n", LOWORD(lpParms->dwReturn));
	    break;
	case 4001: /* FIXME: for boggus FullCD */
	case MCI_CDA_STATUS_TYPE_TRACK:
	    if (!(dwFlags & MCI_TRACK)) 
		ret = MCIERR_MISSING_PARAMETER;
	    else if (lpParms->dwTrack > wmcda->wcda.nTracks || lpParms->dwTrack == 0)
		ret = MCIERR_OUTOFRANGE;
	    else
		lpParms->dwReturn = (wmcda->wcda.lpbTrackFlags[lpParms->dwTrack - 1] & 
				     CDROM_DATA_TRACK) ? MCI_CDA_TRACK_OTHER : MCI_CDA_TRACK_AUDIO;
	    TRACE("MCI_CDA_STATUS_TYPE_TRACK[%ld]=%08lx\n", lpParms->dwTrack, lpParms->dwReturn);
	    break;
	default:
	    FIXME("unknown command %08lX !\n", lpParms->dwItem);
	    return MCIERR_UNRECOGNIZED_COMMAND;
	}
    } else {
	WARN("not MCI_STATUS_ITEM !\n");
    }
    return ret;
}

/**************************************************************************
 * 				CDAUDIO_mciPlay			[internal]
 */
static DWORD CDAUDIO_mciPlay(UINT wDevID, DWORD dwFlags, LPMCI_PLAY_PARMS lpParms)
{
    int 		start, end;
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    DWORD		ret = 0;
    
    TRACE("Entering CDAUDIO_mciPlay (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (lpParms == NULL) {
	ret = MCIERR_NULL_PARAMETER_BLOCK;
    } else if (wmcda == NULL) {
	ret = MCIERR_INVALID_DEVICE_ID;
    } else {
	if (wmcda->wcda.nTracks == 0) {
	    if (!CDAUDIO_GetTracksInfo(&wmcda->wcda)) {
		WARN("error reading TracksInfo !\n");
		return MCIERR_DRIVER_INTERNAL;
	    }
	}
	wmcda->wcda.nCurTrack = 1;
	if (dwFlags & MCI_FROM) {
	    start = CDAUDIO_CalcFrame(wmcda, lpParms->dwFrom);
	    TRACE("MCI_FROM=%08lX -> %u \n", lpParms->dwFrom, start);
	} else {
	    if (!CDAUDIO_GetCDStatus(&wmcda->wcda)) return MCIERR_DRIVER_INTERNAL;
	    start = wmcda->wcda.dwCurFrame;
	}
	if (dwFlags & MCI_TO) {
	    end = CDAUDIO_CalcFrame(wmcda, lpParms->dwTo);
	    TRACE("MCI_TO=%08lX -> %u \n", lpParms->dwTo, end);
	} else {
	    end = wmcda->wcda.dwLastFrame;
	}
	
	if (CDAUDIO_Play(&wmcda->wcda, start, end) == -1)
	    return MCIERR_HARDWARE;
	wmcda->mciMode = MCI_MODE_PLAY;
	if (dwFlags & MCI_NOTIFY) {
	    TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", lpParms->dwCallback);
	    /*
	      mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
	      wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
	    */
	}
    }
    return ret;
}

/**************************************************************************
 * 				CDAUDIO_mciStop			[internal]
 */
static DWORD CDAUDIO_mciStop(UINT wDevID, DWORD dwFlags, LPMCI_GENERIC_PARMS lpParms)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    
    TRACE("Entering CDAUDIO_mciStop (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (wmcda == NULL)	return MCIERR_INVALID_DEVICE_ID;
    
    if (CDAUDIO_Stop(&wmcda->wcda) == -1)
	return MCIERR_HARDWARE;

    wmcda->mciMode = MCI_MODE_STOP;
    if (lpParms && (dwFlags & MCI_NOTIFY)) {
	TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", lpParms->dwCallback);
	mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
			wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciPause		[internal]
 */
static DWORD CDAUDIO_mciPause(UINT wDevID, DWORD dwFlags, LPMCI_GENERIC_PARMS lpParms)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    
    TRACE("Entering CDAUDIO_mciPause (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (wmcda == NULL)	return MCIERR_INVALID_DEVICE_ID;
    
    if (CDAUDIO_Pause(&wmcda->wcda, 1) == -1)
	return MCIERR_HARDWARE;
    wmcda->mciMode = MCI_MODE_PAUSE;
    if (lpParms && (dwFlags & MCI_NOTIFY)) {
        TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", lpParms->dwCallback);
	mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
			wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciResume		[internal]
 */
static DWORD CDAUDIO_mciResume(UINT wDevID, DWORD dwFlags, LPMCI_GENERIC_PARMS lpParms)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    
    TRACE("(%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (wmcda == NULL)	return MCIERR_INVALID_DEVICE_ID;
    
    if (CDAUDIO_Pause(&wmcda->wcda, 0) == -1)
	return MCIERR_HARDWARE;
    wmcda->mciMode = MCI_MODE_STOP;
    if (lpParms && (dwFlags & MCI_NOTIFY)) {
	TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", lpParms->dwCallback);
	mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
			wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciSeek			[internal]
 */
static DWORD CDAUDIO_mciSeek(UINT wDevID, DWORD dwFlags, LPMCI_SEEK_PARMS lpParms)
{
    DWORD		at;
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    
    TRACE("Entering CDAUDIO_mciSeek (%04X, %08lX, %p);\n", wDevID, dwFlags, lpParms);
    
    if (wmcda == NULL)	return MCIERR_INVALID_DEVICE_ID;
    if (lpParms == NULL) return MCIERR_NULL_PARAMETER_BLOCK;
    
    wmcda->mciMode = MCI_MODE_SEEK;
    switch (dwFlags & ~(MCI_NOTIFY|MCI_WAIT)) {
    case MCI_SEEK_TO_START:
	TRACE("Seeking to start\n");
	at = wmcda->wcda.dwFirstFrame;
	break;
    case MCI_SEEK_TO_END:
	TRACE("Seeking to end\n");
	at = wmcda->wcda.dwLastFrame;
	break;
    case MCI_TO: /* 0x00000008L */
	TRACE("Seeking to %lu\n", lpParms->dwTo);
	at = lpParms->dwTo;
	break;
    default:
	TRACE("Seeking to ??=%lu\n", dwFlags);
	return MCIERR_UNSUPPORTED_FUNCTION;
    }
    if (CDAUDIO_Seek(&wmcda->wcda, at) == -1) {
	return MCIERR_HARDWARE;
    }
    if (dwFlags & MCI_NOTIFY) {
	TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", lpParms->dwCallback);
	mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
			  wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciSetDoor		[internal]
 */
static DWORD	CDAUDIO_mciSetDoor(UINT wDevID, int open)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    
    TRACE("(%04x, %s) !\n", wDevID, (open) ? "OPEN" : "CLOSE");
    
    if (wmcda == NULL) return MCIERR_INVALID_DEVICE_ID;
    
    if (CDAUDIO_SetDoor(&wmcda->wcda, open) == -1)
	return MCIERR_HARDWARE;
    wmcda->mciMode = (open) ? MCI_MODE_OPEN : MCI_MODE_STOP;
    return 0;
}

/**************************************************************************
 * 				CDAUDIO_mciSet			[internal]
 */
static DWORD CDAUDIO_mciSet(UINT wDevID, DWORD dwFlags, LPMCI_SET_PARMS lpParms)
{
    WINE_MCICDAUDIO*	wmcda = CDAUDIO_mciGetOpenDrv(wDevID);
    
    TRACE("Entering CDAUDIO_mciSet (%04X, %08lX, %p, %08lX);\n", wDevID, dwFlags, lpParms, wmcda);
    
    if (wmcda == NULL)	return MCIERR_INVALID_DEVICE_ID;
    if (lpParms == NULL) return MCIERR_NULL_PARAMETER_BLOCK;;
    
      TRACE("dwTimeFormat=%08lX\n", lpParms->dwTimeFormat);
      TRACE("dwAudio=%08lX\n", lpParms->dwAudio);
    
    if (dwFlags & MCI_SET_TIME_FORMAT) {
	switch (lpParms->dwTimeFormat) {
	case MCI_FORMAT_MILLISECONDS: /* 0 */
	    TRACE("MCI_FORMAT_MILLISECONDS !\n"); 
	    break;
	case MCI_FORMAT_MSF: /* 2 */
	    TRACE("MCI_FORMAT_MSF !\n");
	    break;
#if 0
        case MCI_FORMAT_FRAMES:  /* 3 */
	    TRACE("MCI_FORMAT_FRAMES !\n");
	    break;
        case MCI_FORMAT_SAMPLES: /*  9 */
	    TRACE("MCI_FORMAT_SAMPLES !\n");
	    break;
#endif
	case MCI_FORMAT_TMSF: /* 10 */
	    TRACE("MCI_FORMAT_TMSF !\n");
	    break;
	default:
	    WARN("bad time format !\n");
	    return MCIERR_BAD_TIME_FORMAT;
	}
	wmcda->dwTimeFormat = lpParms->dwTimeFormat;
    } /* end of if (dwFlags & MCI_SET_TIME_FORMAT) */

    if (dwFlags & MCI_SET_DOOR_OPEN) {
	CDAUDIO_mciSetDoor(wDevID, TRUE);
    }
    if (dwFlags & MCI_SET_DOOR_CLOSED) {
	CDAUDIO_mciSetDoor(wDevID, FALSE);
    }

    if (dwFlags & MCI_SET_ON){
        if(dwFlags & MCI_SET_AUDIO) {
	    TRACE("MCI_SET_AUDIO -> %x !\n",lpParms->dwAudio);
            FIXME("FIXME: MCI_SET_ON for cdaudio not implemented (is it necessary?)\n");
            return 0;
        }
    }

    if (dwFlags & MCI_SET_VIDEO) return MCIERR_UNSUPPORTED_FUNCTION;
    if (dwFlags & MCI_SET_OFF) return MCIERR_UNSUPPORTED_FUNCTION;
    if (dwFlags & MCI_NOTIFY) {
	TRACE("MCI_NOTIFY_SUCCESSFUL %08lX !\n", 
	      lpParms->dwCallback);
	mciDriverNotify((HWND)LOWORD(lpParms->dwCallback), 
			wmcda->wNotifyDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    return 0;
}

/**************************************************************************
 * 			MCICDAUDIO_DriverProc			[sample driver]
 */
LONG CALLBACK	MCICDAUDIO_DriverProc(DWORD dwDevID, HDRVR hDriv, DWORD wMsg, 
				      DWORD dwParam1, DWORD dwParam2)
{

    switch(wMsg) {
    case DRV_LOAD:		return 1;
    case DRV_FREE:		return 1;
    case DRV_OPEN:		return CDAUDIO_drvOpen((LPSTR)dwParam1, (LPMCI_OPEN_DRIVER_PARMSA)dwParam2);
    case DRV_CLOSE:		return CDAUDIO_drvClose(dwDevID);
    case DRV_ENABLE:		return 1;	
    case DRV_DISABLE:		return 1;
    case DRV_QUERYCONFIGURE:	
      	TRACE("Received DRV_QUERYCONFIGURE command [%lu] \n", wMsg);
        return 0; /* We can't configure */
    case DRV_CONFIGURE:
      	TRACE("Received DRV_CONFIGURE command [%lu] \n", wMsg);
        //MessageBoxA(0, "CD audio Driver !", "Odin Driver", MB_OK);
        return 1;
    case DRV_INSTALL:		return DRVCNF_RESTART;
    case DRV_REMOVE:		return DRVCNF_RESTART;
	
    case MCI_OPEN_DRIVER:	return CDAUDIO_mciOpen(dwDevID, dwParam1, (LPMCI_OPEN_PARMSA)dwParam2);
    case MCI_CLOSE_DRIVER:	return CDAUDIO_mciClose(dwDevID, dwParam1, (LPMCI_GENERIC_PARMS)dwParam2);
    case MCI_GETDEVCAPS:	return CDAUDIO_mciGetDevCaps(dwDevID, dwParam1, (LPMCI_GETDEVCAPS_PARMS)dwParam2);
    case MCI_INFO:		return CDAUDIO_mciInfo(dwDevID, dwParam1, (LPMCI_INFO_PARMSA)dwParam2);
    case MCI_STATUS:		return CDAUDIO_mciStatus(dwDevID, dwParam1, (LPMCI_STATUS_PARMS)dwParam2);
    case MCI_SET:		return CDAUDIO_mciSet(dwDevID, dwParam1, (LPMCI_SET_PARMS)dwParam2);
    case MCI_PLAY:		return CDAUDIO_mciPlay(dwDevID, dwParam1, (LPMCI_PLAY_PARMS)dwParam2);
    case MCI_STOP:		return CDAUDIO_mciStop(dwDevID, dwParam1, (LPMCI_GENERIC_PARMS)dwParam2);
    case MCI_PAUSE:		return CDAUDIO_mciPause(dwDevID, dwParam1, (LPMCI_GENERIC_PARMS)dwParam2);
    case MCI_RESUME:		return CDAUDIO_mciResume(dwDevID, dwParam1, (LPMCI_GENERIC_PARMS)dwParam2);
    case MCI_SEEK:		return CDAUDIO_mciSeek(dwDevID, dwParam1, (LPMCI_SEEK_PARMS)dwParam2);
    /* FIXME: I wonder if those two next items are really called ? */
    case MCI_SET_DOOR_OPEN:	return CDAUDIO_mciSetDoor(dwDevID, TRUE);
    case MCI_SET_DOOR_CLOSED:	return CDAUDIO_mciSetDoor(dwDevID, FALSE);
    /* commands that should be supported */
    case MCI_LOAD:		
    case MCI_SAVE:		
    case MCI_FREEZE:		
    case MCI_PUT:		
    case MCI_REALIZE:		
    case MCI_UNFREEZE:		
    case MCI_UPDATE:		
    case MCI_WHERE:		
    case MCI_STEP:		
    case MCI_SPIN:		
    case MCI_ESCAPE:		
    case MCI_COPY:		
    case MCI_CUT:		
    case MCI_DELETE:		
    case MCI_PASTE:		
	FIXME("Unsupported yet command [%lu]\n", wMsg);
	break;
    /* commands that should report an error */
    case MCI_WINDOW:		
	TRACE("Received command [%lu] (MCI_WINDOW). Why do we get video messages???\n", wMsg);
	break;
    case MCI_CONFIGURE:		
	TRACE("Received Command [%lu] (MCI_CONFIGURE). Why do we get video messages???\n", wMsg);
	break;


    case MCI_OPEN:
    case MCI_CLOSE:
	ERR("Shouldn't receive a MCI_OPEN or CLOSE message\n");
	break;
    default:
	TRACE("Sending msg [%lXh] to default driver proc\n", wMsg);
	return DefDriverProc(dwDevID, hDriv, wMsg, dwParam1, dwParam2);
    }
    return MCIERR_UNRECOGNIZED_COMMAND;
}

/*-----------------------------------------------------------------------*/
