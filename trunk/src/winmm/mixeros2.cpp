/* $Id: mixeros2.cpp,v 1.1 2002-05-22 20:19:11 sandervl Exp $ */

/*
 * OS/2 Mixer multimedia
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_BASE
#define INCL_DOSERRORS
#define INCL_DOSDEVIOCTL
#define INCL_DOSFILEMGR
#define INCL_OS2MM
#include <os2wrap.h>
#include <os2mewrap.h>
#include <stdlib.h>
#include <string.h>
#include <dbglog.h>
#include <ioctl90.h>

#include "initwinmm.h"
#include "mixeros2.h"

static BOOL  GetAudioPDDName (char *pszPDDName);
static HFILE DevOpen (char *ddName);
static BOOL  mixerapiIOCTL90 (HFILE hPdd, ULONG ulFunc, PVOID pData, ULONG cbDataSize);

static HFILE hPDDMix = 0;
static char  mixerApiMap[256] = {0};

/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixerOpen()
{
    char szPDDName[128] = "\\DEV\\";

    if(GetAudioPDDName(&szPDDName[5]) == FALSE) {
        return FALSE;
    }

    dprintf(("OSLibMixerOpen: PDD name %s", szPDDName));

    hPDDMix = DevOpen(szPDDName);
    if(hPDDMix == 0) {
        dprintf(("Unable to open PDD %s", szPDDName));
        return FALSE;
    }
    if(mixerapiIOCTL90(hPDDMix, GETAPIMAP, mixerApiMap, sizeof(mixerApiMap)) == FALSE) {
        dprintf(("GETAPIMAP failed!!"));
        DosClose(hPDDMix);
        return FALSE;
    }

    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
void OSLibMixerClose()
{
    if(hPDDMix) {
        DosClose(hPDDMix);
    }
}
/******************************************************************************/
/******************************************************************************/
static DWORD OSLibMixGetIndex(DWORD dwControl) 
{
    DWORD idx;

    switch(dwControl) {
    case MIXER_CTRL_MONOIN:
        idx = MONOINSET;
        break;
    case MIXER_CTRL_PHONE:
        idx = PHONESET;
        break;
    case MIXER_CTRL_MIC:
        idx = MICSET;
        break;
    case MIXER_CTRL_LINE:
        idx = LINESET;
        break;
    case MIXER_CTRL_CD:
        idx = CDSET;
        break;
    case MIXER_CTRL_VIDEO:
        idx = VIDEOSET;
        break;
    case MIXER_CTRL_AUX:
        idx = AUXSET;
        break;
    case MIXER_CTRL_BASS:
        idx = BASSTREBLESET;
        break;
    case MIXER_CTRL_TREBLE:
        idx = BASSTREBLESET;
        break;
    case MIXER_CTRL_3D:
        idx = THREEDSET;
        break;
    case MIXER_CTRL_STREAMVOL:
        idx = STREAMVOLSET;
        break;
    case MIXER_CTRL_RECORDSRC:
        idx = RECORDSRCSET;
        break;
    case MIXER_CTRL_RECORDGAIN:
        idx = RECORDGAINSET;
        break;
    default:
        DebugInt3();
        return -1;
    }
    return idx & 0xF;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixIsControlPresent(DWORD dwControl)
{
    DWORD idx;

    idx = OSLibMixGetIndex(dwControl);
    if(idx == -1) {
        return FALSE;
    }
    idx += MONOINSET;
    return mixerApiMap[idx] != 0;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixSetVolume(DWORD dwControl, BOOL fMute, DWORD dwVolLeft, DWORD dwVolRight)
{
    DWORD     dwFunc;
    MIXSTRUCT mixstruct;

    dwFunc = OSLibMixGetIndex(dwControl);
    if(dwFunc == -1) {
        return FALSE;
    }
    dwFunc += MONOINSET;

    if(dwVolLeft > MIXER_MAX_VOLUME || dwVolRight > MIXER_MAX_VOLUME) {
        dprintf(("OSLibMixSetVolume: Volume (%d,%d) out of RANGE!!", dwVolLeft, dwVolRight));
        return FALSE;
    }
    mixstruct.Mute    = fMute;
    mixstruct.VolumeL = dwVolLeft;
    mixstruct.VolumeR = dwVolRight;

    if(mixerapiIOCTL90(hPDDMix, dwFunc, &mixstruct, sizeof(mixstruct)) == TRUE) {
        return TRUE;
    }
    dprintf(("OSLibMixSetVolume: mixerapiIOCTL90 %d failed!!", dwFunc));
    return FALSE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixGetVolume(DWORD dwControl, BOOL *pfMute, DWORD *pdwVolLeft, DWORD *pdwVolRight)
{
    DWORD     dwFunc;
    MIXSTRUCT mixstruct;

    dwFunc = OSLibMixGetIndex(dwControl);
    if(dwFunc == -1) {
        return FALSE;
    }
    dwFunc += MONOINQUERY;

    if(mixerapiIOCTL90(hPDDMix, dwFunc, &mixstruct, sizeof(mixstruct)) == TRUE) {
        return TRUE;
    }
    if(pfMute)      *pfMute      = mixstruct.Mute;
    if(pdwVolLeft)  *pdwVolLeft  = mixstruct.VolumeL;
    if(pdwVolRight) *pdwVolRight = mixstruct.VolumeR;

    if(mixstruct.VolumeL > MIXER_MAX_VOLUME || mixstruct.VolumeR > MIXER_MAX_VOLUME) {
        dprintf(("OSLibMixGetVolume: Volume (%d,%d) out of RANGE!!", mixstruct.VolumeL, mixstruct.VolumeR));
        return FALSE;
    }

    dprintf(("OSLibMixGetVolume: mixerapiIOCTL90 %d failed!!", dwFunc));
    return FALSE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixSetRecSource(DWORD dwRecSrc)
{
    DWORD idx;
    MIXSTRUCT mixstruct;

    switch(dwRecSrc) {
    case MIXER_RECSRC_MIC:
        idx = I90SRC_MIC;
        break;
    case MIXER_RECSRC_CD:
        idx = I90SRC_CD;
        break;
    case MIXER_RECSRC_VIDEO:
        idx = I90SRC_VIDEO;
        break;
    case MIXER_RECSRC_AUX:
        idx = I90SRC_AUX;
        break;
    case MIXER_RECSRC_LINE:
        idx = I90SRC_LINE;
        break;
    case MIXER_RECSRC_RES5:
        idx = I90SRC_RES5;
        break;
    case MIXER_RECSRC_RES6:
        idx = I90SRC_RES6;
        break;
    case MIXER_RECSRC_PHONE:
        idx = I90SRC_PHONE;
        break;
    default:
        DebugInt3();
        return FALSE;
    }
    mixstruct.Mute    = 0;
    mixstruct.VolumeL = idx;

    if(mixerapiIOCTL90(hPDDMix, RECORDSRCSET, &mixstruct, sizeof(mixstruct)) == TRUE) {
        return TRUE;
    }
    dprintf(("OSLibMixSetRecSource: mixerapiIOCTL90 RECORDSRCSET failed!!"));
    return FALSE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixGetRecSource(DWORD *pdwRecSrc)
{
    DWORD idx;
    MIXSTRUCT mixstruct;

    if(mixerapiIOCTL90(hPDDMix, RECORDSRCQUERY, &mixstruct, sizeof(mixstruct)) == FALSE) {
        dprintf(("OSLibMixSetRecSource: mixerapiIOCTL90 RECORDSRCGET failed!!"));
        return FALSE;
    }
    switch(mixstruct.VolumeL) {
    case I90SRC_MIC:
        idx = MIXER_RECSRC_MIC;
        break;
    case I90SRC_CD:
        idx = MIXER_RECSRC_CD;
        break;
    case I90SRC_VIDEO:
        idx = MIXER_RECSRC_VIDEO;
        break;
    case I90SRC_AUX:
        idx = MIXER_RECSRC_AUX;
        break;
    case I90SRC_LINE:
        idx = MIXER_RECSRC_LINE;
        break;
    case I90SRC_RES5:
        idx = MIXER_RECSRC_RES5;
        break;
    case I90SRC_RES6:
        idx = MIXER_RECSRC_RES6;
        break;
    case I90SRC_PHONE:
        idx = MIXER_RECSRC_PHONE;
        break;
    default:
        DebugInt3();
        return FALSE;
    }
    if(pdwRecSrc) {
        *pdwRecSrc = idx;
    }
    return TRUE;
}
/******************************************************************************/
// OS/2 32-bit program to query the Physical Device Driver name
// for the default MMPM/2 WaveAudio device.  Joe Nord 10-Mar-1999
/******************************************************************************/
static HFILE DevOpen (char *ddName)
{
   ULONG ulRC;
   ULONG OpenFlags;
   ULONG OpenMode;
   ULONG ulFileSize      = 0;
   ULONG ulFileAttribute = 0;
   ULONG ulActionTaken   = 0;
   HFILE hPdd            = NULL;

   OpenFlags = OPEN_ACTION_OPEN_IF_EXISTS;   // Do not create file

   OpenMode  = OPEN_ACCESS_READWRITE +       // Read/Write file
               OPEN_SHARE_DENYNONE +         // Non-exclusive access
               OPEN_FLAGS_FAIL_ON_ERROR;     // No system popups on errors

   ulRC = DosOpen (ddName,          // in
                   &hPdd,           //    out (handle)
                   &ulActionTaken,  //    out
                   ulFileSize,      // in
                   ulFileAttribute, // in
                   OpenFlags,       // in
                   OpenMode,        // in
                   NULL);           // in

   if (ulRC != 0)
      hPdd = NULL;

   return (hPdd);
} 
/******************************************************************************/
/******************************************************************************/
static BOOL mixerapiIOCTL90 (HFILE hPdd, ULONG ulFunc, PVOID pData, ULONG cbDataSize)
{
   ULONG     ulRC;

   ulRC = DosDevIOCtl
      (hPdd,               // Device Handle
       0x90,               // Category (user defined >= 0x80)
       ulFunc,             // Function Use defines in .H file
       NULL,               // in      Address of parm data (not used)
       0,                  // in      Max size of parm data structure
       NULL,               // in out  Actual size of parm data structure
       pData,              // in      Address of command data
       cbDataSize,         // in      Maximum size of command data
       &cbDataSize);       // in out  Size of command data

   return ulRC == NO_ERROR;
}
/******************************************************************************/
// OS/2 32-bit program to query the Physical Device Driver name
// for the default MMPM/2 WaveAudio device.  Joe Nord 10-Mar-1999
/******************************************************************************/
static BOOL GetAudioPDDName (char *pszPDDName)
{
   ULONG                   ulRC;
   char                    szAmpMix[9] = "AMPMIX01";

   MCI_SYSINFO_PARMS       SysInfo;
   MCI_SYSINFO_LOGDEVICE   SysInfoParm;
   MCI_SYSINFO_QUERY_NAME  QueryNameParm;

   memset (&SysInfo, '\0', sizeof(SysInfo));
   memset (&SysInfoParm, '\0', sizeof(SysInfoParm));
   memset (&QueryNameParm, '\0', sizeof(QueryNameParm));

   SysInfo.ulItem       = MCI_SYSINFO_QUERY_NAMES;
   SysInfo.usDeviceType  = MCI_DEVTYPE_WAVEFORM_AUDIO;
   SysInfo.pSysInfoParm = &QueryNameParm;

   strcpy (QueryNameParm.szLogicalName, szAmpMix);

   ulRC = mymciSendCommand (0,
                          MCI_SYSINFO,
                          MCI_SYSINFO_ITEM | MCI_WAIT,
                          (PVOID) &SysInfo,
                          0);
   if (ulRC != 0) return FALSE;

   // Get PDD associated with our AmpMixer
   // Device name is in pSysInfoParm->szPDDName

   SysInfo.ulItem       = MCI_SYSINFO_QUERY_DRIVER;
   SysInfo.usDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
   SysInfo.pSysInfoParm = &SysInfoParm;

   strcpy (SysInfoParm.szInstallName, QueryNameParm.szInstallName);

   ulRC = mymciSendCommand (0,
                          MCI_SYSINFO,
                          MCI_SYSINFO_ITEM | MCI_WAIT,
                          (PVOID) &SysInfo,
                          0);
   if (ulRC != 0) return FALSE;

   strcpy (pszPDDName, SysInfoParm.szPDDName);

   return TRUE;
}
/******************************************************************************/
/******************************************************************************/

