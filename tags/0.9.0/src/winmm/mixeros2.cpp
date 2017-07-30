/* $Id: mixeros2.cpp,v 1.10 2002-07-12 09:21:27 sandervl Exp $ */

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

#include "initterm.h"
#include "mixeros2.h"

#ifndef LOWORD
#define LOWORD(a)   (a & 0xffff)
#endif

static BOOL  GetAudioPDDName (char *pszPDDName);
static HFILE DevOpen (char *ddName);
static BOOL  mixerapiIOCTL90 (HFILE hPdd, ULONG ulFunc, PVOID pData, ULONG cbDataSize);

static HFILE hPDDMix = 0;
static char  mixerApiMap[256] = {0};
static int   szVolumeLevels[MIX_CTRL_MAX][2] = {0};

/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixerOpen()
{
    char szPDDName[128] = "\\DEV\\";

    if(OSLibGetAudioPDDName(&szPDDName[5]) == FALSE) {
        return FALSE;
    }

    dprintf(("OSLibMixerOpen: PDD name %s", szPDDName));

    hPDDMix = DevOpen(szPDDName);
    if(hPDDMix == 0) {
        dprintf(("Unable to open PDD %s", szPDDName));
        return FALSE;
    }
    //Touch array to force OS/2 to make it valid; some braindead audio drivers
    //(Crystal) do not check pointers.
    mixerApiMap[0] = 0;
    if(mixerapiIOCTL90(hPDDMix, GETAPIMAP, mixerApiMap, sizeof(mixerApiMap)) == FALSE) {
        dprintf(("GETAPIMAP failed!!"));
        DosClose(hPDDMix);
        return FALSE;
    }

    memset(szVolumeLevels, -1, sizeof(szVolumeLevels));

    //query current master volume
    MCI_MASTERAUDIO_PARMS parms = {0};
    APIRET                rc;

    rc = mymciSendCommand(0, MCI_MASTERAUDIO, MCI_MASTERVOL | MCI_WAIT | MCI_QUERYCURRENTSETTING, &parms, 0);
    if(LOWORD(rc) == MCIERR_SUCCESS) 
    {
        dprintf(("Current master volume %d -> %d", parms.ulReturn, MMPM2MASTER_TO_WIN32_VOLUME(parms.ulReturn)));
        szVolumeLevels[MIX_CTRL_VOL_OUT_LINE][0]  = MMPM2MASTER_TO_WIN32_VOLUME(parms.ulReturn);
        szVolumeLevels[MIX_CTRL_VOL_OUT_LINE][1]  = szVolumeLevels[MIX_CTRL_VOL_OUT_LINE][0];
        szVolumeLevels[MIX_CTRL_MUTE_OUT_LINE][0] = (szVolumeLevels[MIX_CTRL_MUTE_OUT_LINE][0] == 0);
    }
    else {
        char szError[256] = "";

        mymciGetErrorString(rc, szError, sizeof(szError));
        dprintf(("mciSendCommand returned error %x = %s", rc, szError));
    }

    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
void OSLibMixerClose()
{
    if(hPDDMix) {
        MIXSTRUCT mixstruct;
        
        //unlock recording source
        if(mixerapiIOCTL90(hPDDMix, RECORDSRCQUERY, &mixstruct, sizeof(mixstruct)) == TRUE) {
            mixstruct.Mute = 2;
            if(mixerapiIOCTL90(hPDDMix, RECORDSRCSET, &mixstruct, sizeof(mixstruct)) == FALSE) {
                dprintf(("OSLibMixerClose: mixerapiIOCTL90 RECORDSRCSET failed!!"));
            }
        }
        else dprintf(("OSLibMixerClose: mixerapiIOCTL90 RECORDSRCQUERY failed!!"));

        //unlock recording gain
        if(mixerapiIOCTL90(hPDDMix, RECORDGAINQUERY, &mixstruct, sizeof(mixstruct)) == TRUE) {
            mixstruct.Mute = 2;
            if(mixerapiIOCTL90(hPDDMix, RECORDGAINSET, &mixstruct, sizeof(mixstruct)) == FALSE) {
                dprintf(("OSLibMixerClose: mixerapiIOCTL90 RECORDGAINSET failed!!"));
            }
        }
        else dprintf(("OSLibMixerClose: mixerapiIOCTL90 RECORDGAINQUERY failed!!"));

        //unlock PCM volume
        if(mixerapiIOCTL90(hPDDMix, STREAMVOLQUERY, &mixstruct, sizeof(mixstruct)) == TRUE) {
            mixstruct.Mute = 2;
            if(mixerapiIOCTL90(hPDDMix, STREAMVOLSET, &mixstruct, sizeof(mixstruct)) == FALSE) {
                dprintf(("OSLibMixerClose: mixerapiIOCTL90 STREAMVOLSET failed!!"));
            }
        }
        else dprintf(("OSLibMixerClose: mixerapiIOCTL90 STREAMVOLQUERY failed!!"));

        DosClose(hPDDMix);
    }
}
/******************************************************************************/
/******************************************************************************/
static DWORD OSLibMixGetIndex(DWORD dwControl) 
{
    DWORD idx;

    switch(dwControl) {
    case MIX_CTRL_VOL_IN_L_MONO:
    case MIX_CTRL_MUTE_IN_L_MONO:
        idx = MONOINSET;
        break;
    case MIX_CTRL_VOL_IN_L_PHONE:
    case MIX_CTRL_MUTE_IN_L_PHONE:
        idx = PHONESET;
        break;
    case MIX_CTRL_VOL_IN_L_MIC:
    case MIX_CTRL_MUTE_IN_L_MIC:
        idx = MICSET;
        break;
    case MIX_CTRL_VOL_IN_L_LINE:
    case MIX_CTRL_MUTE_IN_L_LINE:
        idx = LINESET;
        break;
    case MIX_CTRL_VOL_IN_L_CD:
    case MIX_CTRL_MUTE_IN_L_CD:
        idx = CDSET;
        break;
    case MIX_CTRL_VOL_IN_L_VIDEO:
    case MIX_CTRL_MUTE_IN_L_VIDEO:
        idx = VIDEOSET;
        break;
    case MIX_CTRL_VOL_IN_L_AUX:
    case MIX_CTRL_MUTE_IN_L_AUX:
        idx = AUXSET;
        break;
    case MIX_CTRL_OUT_L_BASS:
    case MIX_CTRL_OUT_L_TREBLE:
        idx = BASSTREBLESET;
        break;
    case MIX_CTRL_OUT_L_3DCENTER:
    case MIX_CTRL_OUT_L_3DDEPTH:
        idx = THREEDSET;
        break;
    case MIX_CTRL_VOL_IN_L_PCM:
    case MIX_CTRL_MUTE_IN_L_PCM:
        idx = STREAMVOLSET;
        break;
    case MIX_CTRL_MUX_IN_W_SRC:
        idx = RECORDSRCSET;
        break;
    case MIX_CTRL_VOL_IN_W_MONO:
    case MIX_CTRL_VOL_IN_W_PHONE:
    case MIX_CTRL_VOL_IN_W_MIC:
    case MIX_CTRL_VOL_IN_W_LINE:
    case MIX_CTRL_VOL_IN_W_CD:
    case MIX_CTRL_VOL_IN_W_VIDEO:
    case MIX_CTRL_VOL_IN_W_AUX:
    case MIX_CTRL_VOL_IN_W_PCM:
        idx = RECORDGAINSET;
        break;
    default:
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
BOOL OSLibMixSetVolume(DWORD dwControl, DWORD dwVolLeft, DWORD dwVolRight)
{
    DWORD     dwFunc, dwIOCT90VolLeft, dwIOCT90VolRight;
    MIXSTRUCT mixstruct;

    if(dwControl > MIX_CTRL_MAX) {
        DebugInt3();
        return FALSE;
    }
    if(dwVolLeft > MIXER_WIN32_MAX_VOLUME || dwVolRight > MIXER_WIN32_MAX_VOLUME) {
        dprintf(("OSLibMixSetVolume: Volume (%d,%d) out of RANGE!!", dwVolLeft, dwVolRight));
        return FALSE;
    }

    szVolumeLevels[dwControl][0] = dwVolLeft;
    szVolumeLevels[dwControl][1] = dwVolRight;
    
    //Master volume/mute can't be controlled with ioctl90
    if(dwControl == MIX_CTRL_VOL_OUT_LINE) 
    {
        MCI_MASTERAUDIO_PARMS parms;

        if(szVolumeLevels[MIX_CTRL_MUTE_OUT_LINE][0] == TRUE) {
            //muted, ignore
            return TRUE;
        }
        memset(&parms, 0, sizeof(parms));
        parms.ulMasterVolume = WIN32_TO_MMPM2MASTER_VOLUME(szVolumeLevels[dwControl][0]);

        mymciSendCommand(0, MCI_MASTERAUDIO, MCI_MASTERVOL | MCI_WAIT, &parms, 0);

        return TRUE;
    }

    dwFunc = OSLibMixGetIndex(dwControl);
    if(dwFunc == -1) {
        return FALSE;
    }
    dwFunc += MONOINSET;

    //first get current mute value
    if(mixerapiIOCTL90(hPDDMix, dwFunc-MONOINSET+MONOINQUERY, &mixstruct, sizeof(mixstruct)) == FALSE) {
        return FALSE;
    }

    if(dwControl == MIX_CTRL_OUT_L_TREBLE) {
        //get bass value (right = treble, left = bass)
        OSLibMixGetVolume(MIX_CTRL_OUT_L_BASS, &dwVolLeft, NULL);
    }
    else 
    if(dwControl == MIX_CTRL_OUT_L_BASS) {
        //get treble value (right = treble, left = bass)
        OSLibMixGetVolume(MIX_CTRL_OUT_L_TREBLE, &dwVolRight, NULL);
    }

    dwIOCT90VolLeft  = WIN32_TO_IOCTL90_VOLUME(dwVolLeft);    
    dwIOCT90VolRight = WIN32_TO_IOCTL90_VOLUME(dwVolRight);

    if(mixstruct.VolumeL == dwIOCT90VolLeft && 
       mixstruct.VolumeR == dwIOCT90VolRight) 
    {
        return TRUE; //nothing to do
    }
    //change volume levels
    mixstruct.VolumeL = dwIOCT90VolLeft;
    mixstruct.VolumeR = dwIOCT90VolRight;

    if(mixerapiIOCTL90(hPDDMix, dwFunc, &mixstruct, sizeof(mixstruct)) == TRUE) {
        return TRUE;
    }
    dprintf(("OSLibMixSetVolume: mixerapiIOCTL90 %d failed!!", dwFunc));
    return FALSE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixSetMute(DWORD dwControl, BOOL fMute)
{
    DWORD     dwFunc;
    MIXSTRUCT mixstruct;

    if(dwControl > MIX_CTRL_MAX) {
        DebugInt3();
        return FALSE;
    }
    szVolumeLevels[dwControl][0] = fMute;

    //Master volume/mute can't be controlled with ioctl90
    if(dwControl == MIX_CTRL_MUTE_OUT_LINE) {
        MCI_MASTERAUDIO_PARMS parms;

        memset(&parms, 0, sizeof(parms));
        parms.ulMasterVolume = (fMute) ? 0 : MMPM2MASTER_TO_WIN32_VOLUME(szVolumeLevels[MIX_CTRL_VOL_OUT_LINE][0]);

        mymciSendCommand(0, MCI_MASTERAUDIO, MCI_MASTERVOL | MCI_WAIT, &parms, 0);

        return TRUE;
    }

    dwFunc = OSLibMixGetIndex(dwControl);
    if(dwFunc == -1) {
        return FALSE;
    }
    dwFunc += MONOINSET;

    //first get current volume levels
    if(mixerapiIOCTL90(hPDDMix, dwFunc-MONOINSET+MONOINQUERY, &mixstruct, sizeof(mixstruct)) == FALSE) {
        return FALSE;
    }

    if(mixstruct.Mute == fMute) {
        return TRUE; //nothing to do
    }
    
    //change mute
    mixstruct.Mute = fMute;

    dprintf(("OSLibMixSetMute (%d,%d) %d", mixstruct.VolumeL, mixstruct.VolumeR, mixstruct.Mute));
    if(mixerapiIOCTL90(hPDDMix, dwFunc, &mixstruct, sizeof(mixstruct)) == TRUE) {
        return TRUE;
    }
    dprintf(("OSLibMixSetMute: mixerapiIOCTL90 %d failed!!", dwFunc));
    return FALSE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixGetVolume(DWORD dwControl, DWORD *pdwVolLeft, DWORD *pdwVolRight)
{
    DWORD     dwFunc;
    MIXSTRUCT mixstruct;

    //Master volume/mute can't be controlled with ioctl90
    if(dwControl == MIX_CTRL_VOL_OUT_LINE) 
    {
        if(pdwVolLeft)  *pdwVolLeft  = szVolumeLevels[MIX_CTRL_VOL_OUT_LINE][0];
        if(pdwVolRight) *pdwVolRight = szVolumeLevels[MIX_CTRL_VOL_OUT_LINE][1];

        return TRUE;
    }

    //wave in recording levels are virtual controls as there is only
    //one control for recording gain
    switch(dwControl) {
    case MIX_CTRL_VOL_IN_W_MONO:
    case MIX_CTRL_VOL_IN_W_PHONE:
    case MIX_CTRL_VOL_IN_W_MIC:
    case MIX_CTRL_VOL_IN_W_LINE:
    case MIX_CTRL_VOL_IN_W_CD:
    case MIX_CTRL_VOL_IN_W_VIDEO:
    case MIX_CTRL_VOL_IN_W_AUX:
    case MIX_CTRL_VOL_IN_W_PCM:
        if(szVolumeLevels[dwControl][0] != -1) {
            if(pdwVolLeft)  *pdwVolLeft  = szVolumeLevels[dwControl][0];
            if(pdwVolRight) *pdwVolRight = szVolumeLevels[dwControl][1];
            return TRUE;
        }
        break;
    }

    dwFunc = OSLibMixGetIndex(dwControl);
    if(dwFunc == -1) {
        return FALSE;
    }
    dwFunc += MONOINQUERY;

    if(mixerapiIOCTL90(hPDDMix, dwFunc, &mixstruct, sizeof(mixstruct)) == FALSE) {
        return FALSE;
    }
    if(mixstruct.VolumeL > MIXER_IOCTL90_MAX_VOLUME || mixstruct.VolumeR > MIXER_IOCTL90_MAX_VOLUME) {
        dprintf(("OSLibMixGetVolume: Volume (%d,%d) out of RANGE!!", mixstruct.VolumeL, mixstruct.VolumeR));
    }
    mixstruct.VolumeL = min(MIXER_IOCTL90_MAX_VOLUME, mixstruct.VolumeL);
    if(dwFunc == RECORDGAINSET) {
         mixstruct.VolumeR = mixstruct.VolumeL; //only left is valid
    }
    else mixstruct.VolumeR = min(MIXER_IOCTL90_MAX_VOLUME, mixstruct.VolumeR);

    if(dwControl == MIX_CTRL_OUT_L_TREBLE) {
        mixstruct.VolumeL = mixstruct.VolumeR;  //right = treble, left = bass 
    }
    else 
    if(dwControl == MIX_CTRL_OUT_L_BASS) {
        mixstruct.VolumeR = mixstruct.VolumeL;  //right = treble, left = bass 
    }

    //save volume levels
    szVolumeLevels[dwControl][0] = IOCTL90_TO_WIN32_VOLUME(mixstruct.VolumeL);
    szVolumeLevels[dwControl][1] = IOCTL90_TO_WIN32_VOLUME(mixstruct.VolumeR);

    if(pdwVolLeft)  *pdwVolLeft  = szVolumeLevels[dwControl][0];
    if(pdwVolRight) *pdwVolRight = szVolumeLevels[dwControl][1];

    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixGetMute(DWORD dwControl, BOOL *pfMute)
{
    DWORD     dwFunc;
    MIXSTRUCT mixstruct;

    //Master volume/mute can't be controlled with ioctl90
    if(dwControl == MIX_CTRL_MUTE_OUT_LINE) 
    {
        if(pfMute) *pfMute = szVolumeLevels[MIX_CTRL_MUTE_OUT_LINE][0];

        return TRUE;
    }

    dwFunc = OSLibMixGetIndex(dwControl);
    if(dwFunc == -1) {
        return FALSE;
    }
    dwFunc += MONOINQUERY;

    if(mixerapiIOCTL90(hPDDMix, dwFunc, &mixstruct, sizeof(mixstruct)) == FALSE) {
        return FALSE;
    }
    if(pfMute) *pfMute = mixstruct.Mute;
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixGetLineCaps(DWORD dwLine, DWORD *pcChannels)
{
    switch(dwLine) {
    case MIXER_SRC_IN_L_MONOIN:
    case MIXER_SRC_IN_W_MONOIN:
    case MIXER_SRC_IN_L_PHONE:
    case MIXER_SRC_IN_W_PHONE:
    case MIXER_SRC_IN_L_MIC:
    case MIXER_SRC_IN_W_MIC:
        *pcChannels = 1;
        break;

    case MIXER_SRC_IN_W_LINE:
    case MIXER_SRC_IN_W_CD:
    case MIXER_SRC_IN_W_SPDIF:
    case MIXER_SRC_IN_W_VIDEO:
    case MIXER_SRC_IN_W_AUX:
    case MIXER_SRC_IN_W_PCM:
    case MIXER_SRC_IN_W_WAVETABLE:
    case MIXER_SRC_IN_W_MIDI:
        *pcChannels = 2;
        break;

    case MIXER_SRC_IN_L_LINE:
    case MIXER_SRC_IN_L_CD:
    case MIXER_SRC_IN_L_SPDIF:
    case MIXER_SRC_IN_L_VIDEO:
    case MIXER_SRC_IN_L_AUX:
    case MIXER_SRC_IN_L_PCM:
    case MIXER_SRC_IN_L_WAVETABLE:
    case MIXER_SRC_IN_L_MIDI:
        *pcChannels = 2;
        break;
    default:
        DebugInt3();
        return FALSE;
    }

    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixGetCtrlCaps(DWORD dwControl, LONG *plMinimum, LONG *plMaximum, DWORD *pcSteps)
{
    switch(dwControl) {
    case MIX_CTRL_VOL_OUT_LINE:
        *plMinimum = MIXER_WIN32_MIN_VOLUME;
        *plMaximum = MIXER_WIN32_MAX_VOLUME;
        *pcSteps   = MIXER_WIN32_CSTEP_VOLUME;
        break;

    case MIX_CTRL_VOL_IN_L_MONO:
    case MIX_CTRL_VOL_IN_L_PHONE:
    case MIX_CTRL_VOL_IN_L_MIC:
    case MIX_CTRL_VOL_IN_L_LINE:
    case MIX_CTRL_VOL_IN_L_CD:
    case MIX_CTRL_VOL_IN_L_VIDEO:
    case MIX_CTRL_VOL_IN_L_AUX:
    case MIX_CTRL_VOL_IN_L_PCM:
        *plMinimum = MIXER_WIN32_MIN_VOLUME;
        *plMaximum = MIXER_WIN32_MAX_VOLUME;
        *pcSteps   = MIXER_WIN32_CSTEP_VOLUME;
        break;

    case MIX_CTRL_VOL_IN_W_MONO:
    case MIX_CTRL_VOL_IN_W_PHONE:
    case MIX_CTRL_VOL_IN_W_MIC:
    case MIX_CTRL_VOL_IN_W_LINE:
    case MIX_CTRL_VOL_IN_W_CD:
    case MIX_CTRL_VOL_IN_W_VIDEO:
    case MIX_CTRL_VOL_IN_W_AUX:
    case MIX_CTRL_VOL_IN_W_PCM:
        *plMinimum = MIXER_WIN32_MIN_VOLUME;
        *plMaximum = MIXER_WIN32_MAX_VOLUME;
        *pcSteps   = MIXER_WIN32_CSTEP_VOLUME;
        break;

    case MIX_CTRL_OUT_L_BASS:
    case MIX_CTRL_OUT_L_TREBLE:
        *plMinimum = MIXER_WIN32_MIN_VOLUME;
        *plMaximum = MIXER_WIN32_MAX_VOLUME;
        *pcSteps   = MIXER_WIN32_CSTEP_VOLUME;
        break;

    case MIX_CTRL_OUT_L_3DCENTER:
    case MIX_CTRL_OUT_L_3DDEPTH:
        *plMinimum = MIXER_WIN32_MIN_VOLUME;
        *plMaximum = MIXER_WIN32_MAX_VOLUME;
        *pcSteps   = MIXER_WIN32_CSTEP_VOLUME;
        break;
    default:
        DebugInt3();
        return FALSE;
    }
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixIsRecSourcePresent(DWORD dwRecSrc)
{
    DWORD oldRecSrc;
    BOOL  ret = TRUE;

    OSLibMixGetRecSource(&oldRecSrc);

    if(OSLibMixSetRecSource(dwRecSrc) == FALSE) {
        ret = FALSE;
    }
    OSLibMixSetRecSource(oldRecSrc);
    return ret;
}
/******************************************************************************/
/******************************************************************************/
BOOL OSLibMixSetRecSource(DWORD dwRecSrc)
{
    DWORD     idx, volidx;
    MIXSTRUCT mixstruct;
    DWORD     dwVolL, dwVolR;

    switch(dwRecSrc) {
    case MIXER_SRC_IN_W_MIC:
        idx = I90SRC_MIC;
        volidx = MIX_CTRL_VOL_IN_W_MIC;
        break;
    case MIXER_SRC_IN_W_CD:
        idx = I90SRC_CD;
        volidx = MIX_CTRL_VOL_IN_W_CD;
        break;
    case MIXER_SRC_IN_W_VIDEO:
        idx = I90SRC_VIDEO;
        volidx = MIX_CTRL_VOL_IN_W_VIDEO;
        break;
    case MIXER_SRC_IN_W_AUX:
        idx = I90SRC_AUX;
        volidx = MIX_CTRL_VOL_IN_W_AUX;
        break;
    case MIXER_SRC_IN_W_LINE:
        idx = I90SRC_LINE;
        volidx = MIX_CTRL_VOL_IN_W_LINE;
        break;
    case MIXER_SRC_IN_W_PHONE:
        idx = I90SRC_PHONE;
        volidx = MIX_CTRL_VOL_IN_W_PHONE;
        break;
    default:
        return FALSE;
    }
    mixstruct.Mute    = 0;
    mixstruct.VolumeL = idx;

    //select recording source
    if(mixerapiIOCTL90(hPDDMix, RECORDSRCSET, &mixstruct, sizeof(mixstruct)) == FALSE) {
        dprintf(("OSLibMixSetRecSource: mixerapiIOCTL90 RECORDSRCSET failed!!"));
        return FALSE;
    }

    if(szVolumeLevels[volidx][0] != -1) 
    {//if changed, override recording gain
        dwVolL = szVolumeLevels[volidx][0];
        dwVolR = szVolumeLevels[volidx][1];
        mixstruct.VolumeL = WIN32_TO_IOCTL90_VOLUME(dwVolL);
        mixstruct.VolumeR = WIN32_TO_IOCTL90_VOLUME(dwVolR);
        mixstruct.Mute    = 0;

        //set recording gain to that of the selected source
        dprintf(("set recording gain to (%d,%d)", mixstruct.VolumeL, mixstruct.VolumeR));
        if(mixerapiIOCTL90(hPDDMix, RECORDGAINSET, &mixstruct, sizeof(mixstruct)) == FALSE) {
            dprintf(("OSLibMixSetRecSource: mixerapiIOCTL90 RECORDGAINSET failed!!"));
            return FALSE;
        }
    }
    return TRUE;
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
        idx = MIXER_SRC_IN_W_MIC;
        break;
    case I90SRC_CD:
        idx = MIXER_SRC_IN_W_CD;
        break;
    case I90SRC_VIDEO:
        idx = MIXER_SRC_IN_W_VIDEO;
        break;
    case I90SRC_AUX:
        idx = MIXER_SRC_IN_W_AUX;
        break;
    case I90SRC_LINE:
        idx = MIXER_SRC_IN_W_LINE;
        break;
    case I90SRC_PHONE:
        idx = MIXER_SRC_IN_W_PHONE;
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
BOOL OSLibGetAudioPDDName (char *pszPDDName)
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

