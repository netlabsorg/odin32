/* -*- tab-width: 8; c-basic-offset: 4 -*- */
/*
 * MCI CDAUDIO Driver for OS/2. Interface to OS/2 DosIOCtrl()
 *
 * Copyright 2000    Chris Wohlgemuth
 *           2001    Przemyslaw Dobrowolski
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOS

#include <os2wrap.h>
//#include <cdrom.h>

#pragma pack(1)

typedef struct{
	UCHAR   ucFirstTrack;
	UCHAR 	ucLastTrack;
	ULONG   ulLeadOut;
}CDINFO;

typedef struct
{
    int ADR_data_for_track:4;	// ADR data for track
    int preemphasis:1;		// 0 = Audio without preemphasis
                                // 1 = Audio with preemphasis
    int cdda_permitted:1;	// 0 = Digital copy prohibited
                                // 1 = Digital copy permitted
    int type_of_track:1;	// 0 = Audio track
                                // 1 = Data track
    int channels:1;		// 0 = Two-channel audio
                               // 1 = Four-channel audio
}TRACKTYPE;

typedef struct
{
	ULONG ulTrackAddress;
	TRACKTYPE track_type;
}CDTRACKINFO;

typedef struct
{
	UCHAR signature[4];
	UCHAR  ucTrackNum;
}TINFOPARAM;

typedef struct
{
	UCHAR ucFrames;
	UCHAR ucSeconds;
	UCHAR ucMinutes;
	UCHAR ucNotUsed;
}MSF;

typedef struct
{
  USHORT usStatusBits;
  ULONG ulStarting;
  ULONG ulEnding;
}AUDIOSTATUS;

typedef struct
{
  UCHAR signature[4];
  UCHAR  ucAddressMode;
  ULONG ulStartSector;
  ULONG ulEndSector;
}PLAYPARAM2;

typedef struct
{
  UCHAR signature[4];
  UCHAR  ucAddressMode;
  ULONG ulTo;
}SEEKPARAM;

typedef struct
{
  UCHAR ucCtrlAdr;
  UCHAR ucTrack;
  UCHAR ucIndex;
  UCHAR ucTrackMins;
  UCHAR ucTrackSecs;
  UCHAR UCTrackFrames;
  UCHAR ucNull;
  UCHAR ucDiskMins;
  UCHAR ucDiskSecs;
  UCHAR ucDiskFrames;
}SUBCHANNELQ;

#pragma pack()


LONG static _CDCalculateSector(MSF* msf1)
{
 	return ((msf1->ucMinutes*60+msf1->ucSeconds)*75+msf1->ucFrames);	
}


HFILE os2CDOpen(char *drive)
{
    HFILE hfDrive = 0;
    ULONG ulAction;
    ULONG rc;
    
    rc = DosOpen(drive, &hfDrive, &ulAction, 0,
                 FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY |
                 OPEN_FLAGS_DASD|OPEN_FLAGS_FAIL_ON_ERROR, NULL);	
    
    if(rc)
        return 0;//Error
    
    return hfDrive;
}

ULONG os2CDClose(ULONG hfOS2Handle)
{
    return DosClose(hfOS2Handle);	
}

/******************************************/
/* Result:  
	 0:   Error
	 -1: CD is Data Disk
	 other: # Audio tracks */
/******************************************/
int os2GetNumTracks(HFILE hfOS2Handle, ULONG *ulLeadOut)
{
    ULONG ulParamLen;
    ULONG ulDataLen;
    ULONG rc;
    CDINFO cdInfo;
    TINFOPARAM tip;
    CDTRACKINFO trackInfo;
	
    ulDataLen=sizeof(cdInfo);
    ulParamLen=4;
    
    if(!hfOS2Handle)
        return 0;
	
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
                     "CD01", 4, &ulParamLen, &cdInfo,
                     sizeof(cdInfo), &ulDataLen);

    if(rc) {
        return 0;//Error
    }

    ulDataLen=sizeof(trackInfo);
    ulParamLen=sizeof(TINFOPARAM);
    tip.signature[0]='C';
    tip.signature[1]='D';	
    tip.signature[2]='0';
    tip.signature[3]='1';
    tip.ucTrackNum=1;
	
    /* We have a disc. Check if it's audio */
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
                     &tip, sizeof(tip), &ulParamLen, &trackInfo,
                     sizeof(trackInfo), &ulDataLen);

    if(rc) {
        return 0;//Error
    }

// PD: Don't return error when data track is found.
//  if(trackInfo.ucTCInfo & 0x40) {
//      return -1;//It's a data track
//  }

    *ulLeadOut=cdInfo.ulLeadOut;
    return cdInfo.ucLastTrack-cdInfo.ucFirstTrack+1;
}	

BOOL os2GetCDStatus(HFILE hfOS2Handle, ULONG  *ulStatus)
{
    ULONG ulParamLen;
    ULONG ulDeviceStatus;
    ULONG ulDataLen;
    ULONG rc;

    ulDeviceStatus=0;

    /* Get status */
    ulDataLen=sizeof(ulDeviceStatus);
    ulParamLen=4;
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMDISK, CDROMDISK_DEVICESTATUS,
                     "CD01", 4, &ulParamLen, &ulDeviceStatus,
                     sizeof(ulDeviceStatus), &ulDataLen);		
    if(rc) {
      return FALSE;//Error
    }

    *ulStatus=ulDeviceStatus;
    return TRUE;
}

BOOL os2GetCDAudioStatus(HFILE hfOS2Handle, USHORT  *usStatus)
{
    AUDIOSTATUS asStatus;
    ULONG ulParamLen;
    ULONG ulDataLen;
    ULONG rc;

    /* Get status */

    ulDataLen=sizeof(asStatus);
    ulParamLen=4;
    
    /* Get information */
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOSTATUS,
                     "CD01", 4, &ulParamLen, &asStatus,
                     sizeof(asStatus), &ulDataLen);
    if(rc) {
        *usStatus=rc;
        return FALSE;//Error
    }
    *usStatus=asStatus.usStatusBits;
    return TRUE;
}

/* Returns sector info of track #numTrack */
/* Starting with track 0 */
ULONG  os2CDQueryTrackStartSector( HFILE hfDrive, ULONG numTrack, BOOL *pflAudio)
{	
    ULONG ulParamLen;
    ULONG ulDataLen;
    ULONG rc;
    CDINFO cdInfo;
    TINFOPARAM tip;
    CDTRACKINFO trackInfo[2];

    do {
        if(!hfDrive)
            break;

        /* Get cd info */
        ulDataLen=sizeof(cdInfo);
        ulParamLen=4;
        rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
                         "CD01", 4, &ulParamLen, &cdInfo,
                         sizeof(cdInfo), &ulDataLen);		
        if(rc)
            break;//Error
        
        ulDataLen=sizeof(trackInfo);
        ulParamLen=sizeof(TINFOPARAM);
        tip.signature[0]='C';
        tip.signature[1]='D';	
        tip.signature[2]='0';
        tip.signature[3]='1';
        /* Get information about our track */
        tip.ucTrackNum=numTrack+1;
        if(tip.ucTrackNum<=cdInfo.ucLastTrack) {
            rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
                             &tip, sizeof(tip), &ulParamLen, &trackInfo[0],
                             sizeof(trackInfo[0]), &ulDataLen);
            if(rc) 
                break;//Error

           *pflAudio=!trackInfo[0].track_type.type_of_track;

        }else 
            return _CDCalculateSector((MSF*)&cdInfo.ulLeadOut);
        
        
        return _CDCalculateSector((MSF*)&trackInfo[0].ulTrackAddress);
        
    }while(TRUE);
    
    /* error */
    return 0;
}


BOOL os2CDEject(HFILE hfDrive)
{
    ULONG ulParamLen;
    ULONG rc;
 
    if(!hfDrive)
        return FALSE;

    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMDISK, CDROMDISK_EJECTDISK,
                     "CD01", 4, &ulParamLen,0,
                     0, 0);		
    if(rc)
        return FALSE;//Error

    return TRUE;
}

#ifndef CDROMDISK_CLOSETRAY
#define CDROMDISK_CLOSETRAY 0x0045
#endif

BOOL os2CDCloseTray(HFILE hfDrive)
{
    ULONG ulParamLen;
    ULONG rc;

    if(!hfDrive)
        return FALSE;
    
    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMDISK, CDROMDISK_CLOSETRAY,
                     "CD01", 4, &ulParamLen,0,
                     0, 0);		
    if(rc)
        return FALSE;//Error
    
    return TRUE;
}

BOOL os2CDStop(HFILE hfDrive)
{
    ULONG ulParamLen;
    ULONG rc;

    if(!hfDrive)
        return FALSE;
    
	    

      /* Stop CD */
    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_STOPAUDIO,
                     "CD01", 4, &ulParamLen,0,
                     0, 0);		
    if(rc)
        return FALSE;//Error
    
    return TRUE;
}


/*****************************************/
/*                                       */
/* Plays frame range                     */
/*                                       */
/* Returns TRUE if successful            */
/*                                       */
/*****************************************/
BOOL os2CDPlayRange(HFILE hfDrive ,ULONG ulFrom, ULONG ulTo)
{
    ULONG ulParamLen;
    ULONG ulDataLen;
    ULONG rc;
    CDINFO cdInfo;
    TINFOPARAM tip;
    CDTRACKINFO trackInfo[2];
    PLAYPARAM2 playParam={0};
    
    if(!hfDrive)
        return FALSE;
    
    //memcpy(&playParam,PlayParam, sizeof(playParam));
    /* Play the Track... */
    ulParamLen=sizeof(PLAYPARAM2);
    playParam.signature[0]='C';
    playParam.signature[1]='D';	
    playParam.signature[2]='0';
    playParam.signature[3]='1';
    //playParam.ucAddressMode=01;
    playParam.ulStartSector=ulFrom;
    playParam.ulEndSector=ulTo;

    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_PLAYAUDIO,
                     &playParam, sizeof(playParam), &ulParamLen,0,
                     0, 0);
        return rc;
    if(rc) 
        return FALSE;
    
    return TRUE;    
}

BOOL  os2CDResume(HFILE hfDrive)
{
    ULONG ulParamLen;
    ULONG rc;
    
    if(!hfDrive)
        return FALSE;

    /* Stop CD */
    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_RESUMEAUDIO,
                     "CD01", 4, &ulParamLen,0,
                     0, 0);		
    if(rc)
        return FALSE;//Error
    
    return TRUE;
}

BOOL os2CDGetHeadLocation(HFILE hfOS2Handle, ULONG  *ulHeadLocation)
{
    UCHAR ucParam[5]={0};
    ULONG ulParamLen;
    ULONG ulDeviceStatus;
    ULONG ulDataLen;
    ULONG rc;

    ulDeviceStatus=0;

    if(!hfOS2Handle)
        return FALSE;

    /* Get Location */
    ulDataLen=sizeof(ulDeviceStatus);
    ulParamLen=5;
    ucParam[0]='C';
    ucParam[1]='D';
    ucParam[2]='0';
    ucParam[3]='1';
    ucParam[4]=0;
 
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMDISK, CDROMDISK_GETHEADLOC,
                     &ucParam, 5, &ulParamLen, &ulDeviceStatus,
                     sizeof(ulDeviceStatus), &ulDataLen);		
    if(rc) {
      return FALSE;//Error
    }

    *ulHeadLocation=ulDeviceStatus;
    return TRUE;

}

BOOL os2CDSeek(HFILE hfOS2Handle, ULONG  ulTo)
{
    SEEKPARAM sp={0};
    ULONG ulParamLen;
    ULONG rc;

    if(!hfOS2Handle)
        return FALSE;

    sp.signature[0]='C';
    sp.signature[1]='D';	
    sp.signature[2]='0';
    sp.signature[3]='1';
    sp.ulTo=ulTo;

    ulParamLen=sizeof(sp);
	
    /* We have a disc. Check if it's audio */
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMDISK, CDROMDISK_SEEK,
                     &sp, sizeof(sp), &ulParamLen, 0,
                     0, 0);

    if(rc) {
        return FALSE;//Error
    }

    return TRUE;
}

BOOL os2CDQueryCurTrack(HFILE hfOS2Handle, UINT  * uiCurTrack)
{
    ULONG ulParamLen;
    ULONG ulDeviceStatus;
    ULONG ulDataLen;
    ULONG rc;
    SUBCHANNELQ scq;


    if(!hfOS2Handle) 
        return FALSE;


	/* Get status */
    ulDataLen=sizeof(scq);
    ulParamLen=4;
    rc = DosDevIOCtl(hfOS2Handle, IOCTL_CDROMAUDIO, CDROMAUDIO_GETSUBCHANNELQ,
                     "CD01", 4, &ulParamLen, &scq,
                     sizeof(scq), &ulDataLen);		
    if(rc) {
        return FALSE;//Error
    }
    *uiCurTrack=(scq.ucTrack & 0xF)+(scq.ucTrack & 0xF0)*10;
    return TRUE;
}

/**************************************************************/
/*                                                            */
/* This funktion returns the CD-Drives in the system          */
/*                                                            */
/* iNumCD (output): # of CD-Drives                            */
/* cFirstDrive (output): first cd-Drive letter                */
/* returns TRUE: We have cd-drives                            */
/*                                                            */
/**************************************************************/
BOOL os2CDQueryCDDrives(int *iNumCD, char * cFirstDrive)
{
  HFILE hfDevice;
  ULONG ulAction;
  ULONG ulLen;
  static char cFirst=0;
  static int iNumCDLocal=0;
  static BOOL haveCD=FALSE;
  static BOOL done=FALSE;			

  struct
  {
    USHORT usCountCD;
    USHORT usFirstCD;
  } CDInfo;
  
  if(!done){
    ulLen = sizeof(CDInfo);
    if(!DosOpen("\\DEV\\CD-ROM2$", &hfDevice, &ulAction, 0,
                FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY, NULL))
      {
        if(!DosDevIOCtl(hfDevice, 0x82, 0x60, NULL, 0, NULL, &CDInfo, ulLen, &ulLen))
          {
            if(CDInfo.usCountCD) {
              haveCD=TRUE;
              iNumCDLocal=CDInfo.usCountCD;
              cFirst='A'+ CDInfo.usFirstCD;
            }										 
          }
        DosClose(hfDevice);
      }
    done=TRUE;
  }
  *iNumCD=iNumCDLocal;
  *cFirstDrive=cFirst;
  return haveCD;				
}










