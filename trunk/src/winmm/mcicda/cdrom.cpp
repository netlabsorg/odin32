/* -*- tab-width: 8; c-basic-offset: 4 -*- */
/*
 * MCI CDAUDIO Driver for OS/2. Interface to OS/2 DosIOCtrl()
 *
 * Copyright 2000    Chris Wohlgemuth
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#include <os2win.h>
#include "cdrom.h"
#include <debugtools.h>
#include <string.h>

/* 
 * FIXME: Should use the right errorcodes for SetLastError if a function fails.
*/

int	CDAUDIO_Open(WINE_CDAUDIO* wcda)
{
  ULONG yyrc;
  USHORT sel;
  HFILE hFile;
  int iNumCD;
  char chrFirstCD[3]={0};
  int i;

  dprintf(("MCICDA-CDROM: Trying to open drive %s...\n",wcda->chrDrive));

  if(strlen(wcda->chrDrive)==2) {
      if((wcda->hfOS2Handle=os2CDOpen(wcda->chrDrive))==NULL) {
          SetLastError(ERROR_WRONG_DISK);  
          /* We always return TRUE because we want to open the driver not the disk */
      }
      return TRUE;
  }

  if(!strcmpi(wcda->chrDeviceType,"cdaudio")) {
      /* We try any CDROM in the system until one can be opened */
      if(!os2CDQueryCDDrives(&iNumCD,  chrFirstCD)) {
          SetLastError(ERROR_WRONG_DISK);  
          return FALSE;/* Can't get drives in system */
      }
      
      chrFirstCD[1]=':';
      for(i=0;i<iNumCD;i++) {
          chrFirstCD[0]++;
          if((wcda->hfOS2Handle=os2CDOpen(chrFirstCD))!=NULL) {     
              return TRUE;
          }
      }
      SetLastError(ERROR_WRONG_DISK);
      /* We always return TRUE because we want to open the driver not the disk */  
      /* Can't open CD */     
  }
  /* We always return TRUE because we want to open the driver not the disk */
  return TRUE;
}

int CDAUDIO_Close(WINE_CDAUDIO* wcda)
{
    ULONG rc;
    
    dprintf(("MCICDA-CDROM: CDAUDIO_Close: Closing drive: %s...\n",wcda->chrDrive));
    
    if(wcda->hfOS2Handle==NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    
  if((rc=os2CDClose(wcda->hfOS2Handle))!=0) {
      dprintf(("MCICDA-CDROM: CDAUDIO_Close:rc=%d \n",rc));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
  }
  wcda->hfOS2Handle=NULL;
  dprintf(("MCICDA-CDROM: CDAUDIO_Close: Drive %s closed\n",wcda->chrDrive));
  return TRUE;
}

int	CDAUDIO_Reset(WINE_CDAUDIO* wcda)
{
  dprintf(("MCICDA-CDROM: CDAUDIO_Reset not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

int	CDAUDIO_Play(WINE_CDAUDIO* wcda, DWORD start, DWORD stop)
{
    ULONG rc;

    /* FIXME: error handling is missing */
    //SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return 1;
        }
    }

    CDAUDIO_GetCDStatus(wcda);
    if(wcda->cdaMode==WINE_CDA_PLAY||wcda->cdaMode==WINE_CDA_PAUSE)
        CDAUDIO_Stop(wcda);
    rc=os2CDPlayRange(wcda->hfOS2Handle , start, stop);
    return 0;
}

int	CDAUDIO_Stop(WINE_CDAUDIO* wcda)
{
    //  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    /* FIXME: error handling is missing */
    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return 1;
        }
    }

    os2CDStop(wcda->hfOS2Handle);
    return 0;
}

int	CDAUDIO_Pause(WINE_CDAUDIO* wcda, int pauseOn)
{

    //SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    /* FIXME: error handling is missing */
    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return 1;
        }
    }

    if(pauseOn)
        os2CDStop(wcda->hfOS2Handle);
    else
        os2CDResume(wcda->hfOS2Handle);
    return 0;
}

int	CDAUDIO_Seek(WINE_CDAUDIO* wcda, DWORD at)
{
    /* FIXME: error handling is missing */
  //SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return 1;
        }
    }
    
    os2CDSeek(wcda->hfOS2Handle, at);
    return 0;
}

int	CDAUDIO_SetDoor(WINE_CDAUDIO* wcda, int open)
{

    /* FIXME: error handling is missing */
    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return 1;
        }
    }
     
    CDAUDIO_Stop(wcda);
    if(open) {
        os2CDEject(wcda->hfOS2Handle);
    }
    else
        os2CDCloseTray(wcda->hfOS2Handle);

    return 0;
}


/******************************************/
/* Result:  
	 0:   Error
	 -1: CD is Data Disk
	 other: # Audio tracks */
/******************************************/
int 	CDAUDIO_GetNumberOfTracks(WINE_CDAUDIO* wcda)
{
    int rcOS2;

    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }
    }

    rcOS2=os2GetNumTracks(wcda->hfOS2Handle,&wcda->ulLeadOut);
    switch(rcOS2)
        {
        case -1:
            SetLastError(ERROR_INVALID_PARAMETER);
            return -1;
        case 0:
            SetLastError(ERROR_WRONG_DISK);  
            return -1;
        default:
            break;
        }

    wcda->nTracks=rcOS2;
    wcda->nFirstTrack=1;
    wcda->nLastTrack=rcOS2;
    return rcOS2;
}

BOOL 	CDAUDIO_GetTracksInfo(WINE_CDAUDIO* wcda)
{

    int		i, length;
    ULONG		start, last_start = 0;
    int		total_length = 0;

    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }
    
    if (wcda->nTracks == 0) {
	if (CDAUDIO_GetNumberOfTracks(wcda) <= 0)
            return FALSE;
    }
    
    TRACE("nTracks=%u\n", wcda->nTracks);
    
    SetLastError(ERROR_INVALID_PARAMETER);
    
    if (wcda->lpdwTrackLen != NULL) 
	free(wcda->lpdwTrackLen);
    wcda->lpdwTrackLen = (LPDWORD)malloc((wcda->nTracks + 1) * sizeof(DWORD));
    if (wcda->lpdwTrackPos != NULL) 
	free(wcda->lpdwTrackPos);
    wcda->lpdwTrackPos = (LPDWORD)malloc((wcda->nTracks + 1) * sizeof(DWORD));
    if (wcda->lpbTrackFlags != NULL)
	free(wcda->lpbTrackFlags);
    wcda->lpbTrackFlags = (LPBYTE)malloc((wcda->nTracks + 1) * sizeof(BYTE));
    
    if (wcda->lpdwTrackLen == NULL || wcda->lpdwTrackPos == NULL ||
	wcda->lpbTrackFlags == NULL) {
	WARN("error allocating track table !\n");
        /* Freeing the already allocated mem */
        if (wcda->lpdwTrackLen != NULL) 
            free(wcda->lpdwTrackLen);
        if (wcda->lpdwTrackPos != NULL) 
            free(wcda->lpdwTrackPos);
        if (wcda->lpbTrackFlags != NULL)
            free(wcda->lpbTrackFlags);
        wcda->lpbTrackFlags=NULL;
        wcda->lpdwTrackPos=NULL;
        wcda->lpdwTrackLen= NULL;
	return FALSE;
    }
    memset(wcda->lpdwTrackLen, 0, (wcda->nTracks + 1) * sizeof(DWORD));
    memset(wcda->lpdwTrackPos, 0, (wcda->nTracks + 1) * sizeof(DWORD));
    memset(wcda->lpbTrackFlags, 0, (wcda->nTracks + 1) * sizeof(BYTE));
    
    for (i = 0; i <= wcda->nTracks; i++) {
        if((start=os2CDQueryTrackStartSector(wcda->hfOS2Handle,i))==0)
            {
                WARN("error reading start sector for track %d\n", i+1);
                /* Freeing the already allocated mem */
                if (wcda->lpdwTrackLen != NULL) 
                    free(wcda->lpdwTrackLen);
                if (wcda->lpdwTrackPos != NULL) 
                    free(wcda->lpdwTrackPos);
                if (wcda->lpbTrackFlags != NULL)
                    free(wcda->lpbTrackFlags);
                wcda->lpbTrackFlags=NULL;
                wcda->lpdwTrackPos=NULL;
                wcda->lpdwTrackLen= NULL;
                return FALSE;
            }
        start-=150;
        
	if (i == 0) {
	    last_start = start;
	    wcda->dwFirstFrame = start;
	    TRACE("dwFirstOffset=%u\n", start);
	} else {
	    length = start - last_start;
	    last_start = start;
	    start = last_start - length;
	    total_length += length;
	    wcda->lpdwTrackLen[i - 1] = length;
	    wcda->lpdwTrackPos[i - 1] = start;
	    TRACE("track #%u start=%u len=%u\n", i, start, length);
	}
        //if(wcda->ulCDROMStatus & )
        wcda->lpbTrackFlags[i] = 0; 
	//TRACE("track #%u flags=%02x\n", i + 1, wcda->lpbTrackFlags[i]);
    }/* for */
    
    wcda->dwLastFrame = last_start;
    TRACE("total_len=%u Leaving CDAUDIO_GetTracksInfo...\n", total_length);
    
    return TRUE;
    
    dprintf(("MCICDA-CDROM: CDAUDIO_GetTracksInfo not implemented.\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}


BOOL	CDAUDIO_GetCDStatus(WINE_CDAUDIO* wcda)
{
    ULONG ulRet;
    int oldmode = wcda->cdaMode;

    if(!wcda->hfOS2Handle) {
        CDAUDIO_Open(wcda);/* Get new handle */
        if(!wcda->hfOS2Handle) {
            WARN("hfOS2Handle isn't valid and can't get new one\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }

    if(!os2GetCDStatus(wcda->hfOS2Handle, &wcda->ulCDROMStatus)) {
        WARN("os2GetCDStatus(wcda->hfOS2Handle, &wcda->ulCDROMStatus) returned FALSE\n");
        return FALSE;
    }
    /* Current mode */    
    //wcda->cdaMode=WINE_CDA_STOP;
    if(wcda->ulCDROMStatus & 0x1L)
        wcda->cdaMode=WINE_CDA_OPEN; /* Door open */
    else if(wcda->ulCDROMStatus & 0x1000L)
        wcda->cdaMode=WINE_CDA_PLAY; /* Playing */
    else if(wcda->ulCDROMStatus & 0x800L)
        wcda->cdaMode=WINE_CDA_NOTREADY;
    else
        wcda->cdaMode=WINE_CDA_STOP;

    if(wcda->cdaMode==WINE_CDA_OPEN && wcda->hfOS2Handle) {
        CDAUDIO_Close(wcda);
        return FALSE;
    }

    if(!os2GetCDAudioStatus(wcda->hfOS2Handle, &wcda->usCDAudioStatus)) {
        WARN("os2GetCDAudioStatus(wcda->hfOS2Handle, &wcda->usCDAudioStatus) returned FALSE rc=%x %d\n",wcda->usCDAudioStatus,wcda->usCDAudioStatus);
        return FALSE;

    }
    else {
        if(wcda->usCDAudioStatus & 1)
            wcda->cdaMode=WINE_CDA_PAUSE;
        os2CDGetHeadLocation(wcda->hfOS2Handle,&wcda->dwCurFrame);
        os2CDQueryCurTrack(wcda->hfOS2Handle, &wcda->nCurTrack);
    }

    if (oldmode != wcda->cdaMode && oldmode == WINE_CDA_OPEN) {
        TRACE("Updating TracksInfo !\n");
        if (!CDAUDIO_GetTracksInfo(wcda)) {
            WARN("error updating TracksInfo !\n");
            return FALSE;
        }
    }

    dprintf(("MCICDA-CDROM: Leaving CDAUDIO_GetCDStatus... cdaMode: %x\n",wcda->cdaMode));
    if (wcda->cdaMode != WINE_CDA_OPEN)
        return TRUE;
    else 
        return FALSE;
}











