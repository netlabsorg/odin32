/* $Id: mixer.cpp,v 1.12 2002-05-22 20:19:11 sandervl Exp $ */

/*
 * Mixer functions
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#define NONAMELESSUNION
#define NONAMELESSSTRUCT

#include <os2win.h>
#include <string.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <misc.h>

#include "waveoutdart.h"
#include "winmm.h"
#include "initwinmm.h"
#include "mixer.h"
#include "mixeros2.h"

#define DBG_LOCALLOG	DBG_mixer
#include "dbglocal.h"


MIXERLINEA    mixerDest[MAX_MIXER_DESTINATIONS] = {0};
MIXERLINEA    mixerSource[MAX_MIXER_SOURCES]    = {0};
MIXERCONTROLA mixerControls[MAX_MIXER_CONTROLS] = {0};

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
    DWORD	ret = MMSYSERR_NOTSUPPORTED;
    DWORD	lineID, controlType;
   
    if (lpmcd == NULL) return MMSYSERR_INVALPARAM;

    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
        break;
    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
    	dprintf(("mixerGetControlDetailsA: NIY\n"));
	    break;
    default:
	    dprintf(("Unknown flag (%08lx)\n", fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK));
        break;
    }
    return ret;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
    dprintf(("WINMM:mixerGetControlDetailsW - stub\n" ));
    return MIXERR_INVALCONTROL;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
    DWORD	ret = MMSYSERR_NOTSUPPORTED;
    DWORD	lineID, controlType;
    int		val;
     
    if (lpmcd == NULL) return MMSYSERR_INVALPARAM;
    
    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
    	break;
    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
    	dprintf(("mixerSetControlDetails: NIY\n"));
    	break;
    default:
	    dprintf(("Unknown flag (%08lx)\n", fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK));
        break;
    }
    return MMSYSERR_NOTSUPPORTED;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineControlsA(HMIXEROBJ  hmxobj, LPMIXERLINECONTROLSA lpMlc, DWORD fdwControls)
{
 DWORD dwRet = MMSYSERR_NOERROR;
 DWORD lineID, controlType;

    if (lpMlc == NULL) return MMSYSERR_INVALPARAM;
    if (lpMlc->cbStruct < sizeof(*lpMlc) ||
	lpMlc->cbmxctrl < sizeof(MIXERCONTROLA))
	return MMSYSERR_INVALPARAM;

    switch(fdwControls & MIXER_GETLINECONTROLSF_QUERYMASK) 
    {
    case MIXER_GETLINECONTROLSF_ALL:
	    if (lpMlc->cControls != 2) {
    	    dwRet = MMSYSERR_INVALPARAM;
    	} 
        else {
	    }
	    break;
    case MIXER_GETLINECONTROLSF_ONEBYID:
	    break;
    case MIXER_GETLINECONTROLSF_ONEBYTYPE:
	    switch (lpMlc->u.dwControlType & MIXERCONTROL_CT_CLASS_MASK) {
	    case MIXERCONTROL_CT_CLASS_FADER:
	        break;
	    case MIXERCONTROL_CT_CLASS_SWITCH:
	        break;
	    default:
    	    dwRet = MMSYSERR_INVALPARAM;
    	}
    	break;
    default:
	    dprintf(("Unknown flag %08lx\n", fdwControls & MIXER_GETLINECONTROLSF_QUERYMASK));
	    dwRet = MMSYSERR_INVALPARAM;
    }

    return dwRet;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls)
{
    dprintf(("WINMM:mixerGetGetLineControlsW - stub\n" ));
    return MIXERR_INVALLINE;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA lpMl, DWORD fdwInfo)
{
    if (lpMl == NULL || lpMl->cbStruct != sizeof(*lpMl)) 
	    return MMSYSERR_INVALPARAM;
    
    /* FIXME: set all the variables correctly... the lines below
     * are very wrong...
     */
    lpMl->fdwLine	= MIXERLINE_LINEF_ACTIVE;
    lpMl->cChannels	= 1;
    lpMl->dwUser	= 0;
    lpMl->cControls	= 2;

    switch (fdwInfo & MIXER_GETLINEINFOF_QUERYMASK) 
    {
    case MIXER_GETLINEINFOF_DESTINATION:
    	break;

    case MIXER_GETLINEINFOF_LINEID:
    	break;

    case MIXER_GETLINEINFOF_SOURCE:
    	break;

    case MIXER_GETLINEINFOF_COMPONENTTYPE:
	    switch (lpMl->dwComponentType) 
	    {
	    case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:
	    	break;
    	case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT:
        	break;
    	default:
	    	dprintf(("Unhandled component type (%08lx)\n", lpMl->dwComponentType));
	    	return MMSYSERR_INVALPARAM;
	    }
       	lpMl->cConnections  = 0;
       	lpMl->cControls     = 1;
        lpMl->cChannels     = 2;
	    break;

    case MIXER_GETLINEINFOF_TARGETTYPE:
	    dprintf(("_TARGETTYPE not implemented yet.\n"));
	    break;
    default:
	    dprintf(("Unknown flag (%08lx)\n", fdwInfo & MIXER_GETLINEINFOF_QUERYMASK));
	    break;
    }
    
    lpMl->Target.dwType = MIXERLINE_TARGETTYPE_AUX;
    lpMl->Target.dwDeviceID = 0xFFFFFFFF;
    lpMl->Target.wMid = 0;
    lpMl->Target.wPid = 0;
    lpMl->Target.vDriverVersion = 1;
    lstrcpyA(lpMl->Target.szPname, WINMM_MIXERSTRING_A);
    
    return MMSYSERR_NOERROR;
}

/******************************************************************************/
MMRESULT WINAPI mixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo)
{
    dprintf(("WINMM:mixerGetLineInfoW - stub\n" ));
    return MIXERR_INVALLINE;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerMessage(HMIXER hmx, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
    dprintf(("WINMM:mixerMessage - stub\n" ));
    return 0;
}
/******************************************************************************/
/******************************************************************************/
UINT WINAPI mixerGetNumDevs()
{
    if(fMMPMAvailable == FALSE || DartWaveOut::getNumDevices() == 0) {
        return 0;
    }
    return 1;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetDevCapsA(UINT uMxId, LPMIXERCAPSA pmxcaps, UINT cbmxcaps)
{
    if(fMMPMAvailable == FALSE || DartWaveOut::getNumDevices() == 0) {
        return MMSYSERR_BADDEVICEID;
    }

    if(uMxId > 0) {
        return MMSYSERR_BADDEVICEID;
    }

    //According to MSDN, nothing is copied when cbmxcaps is zero
    if(cbmxcaps >= sizeof(MIXERCAPSA)) {
        // we have to fill in this thing
        pmxcaps->wMid = 0;                  /* manufacturer ID */
        pmxcaps->wPid = 0;                  /* product ID */
        pmxcaps->vDriverVersion = 0x0001;        /* version of the driver */
        strcpy( pmxcaps->szPname, WINMM_MIXERSTRING_A); /* product name */

        pmxcaps->fdwSupport    = 0;    //no mixer flags exist
        pmxcaps->cDestinations = 1;
    }

    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetDevCapsW(UINT uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps)
{
    if(fMMPMAvailable == FALSE || DartWaveOut::getNumDevices() == 0) {
        return MMSYSERR_BADDEVICEID;
    }

    if(uMxId > 0) {
        return MMSYSERR_BADDEVICEID;
    }

    //According to MSDN, nothing is copied when cbmxcaps is zero
    if(cbmxcaps >= sizeof(MIXERCAPSW)) {
        // we have to fill in this thing
        pmxcaps->wMid = 0;                  /* manufacturer ID */
        pmxcaps->wPid = 0;                  /* product ID */
        pmxcaps->vDriverVersion = 0x0001;        /* version of the driver */
        lstrcpyW( pmxcaps->szPname, WINMM_MIXERSTRING_W ); /* product name */

        pmxcaps->fdwSupport    = 0;    //no mixer flags exist
        pmxcaps->cDestinations = 1;
    }

    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetID(HMIXEROBJ hmxobj, UINT * puMxId, DWORD fdwId)
{
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if(!pMixInfo) {
        return MMSYSERR_INVALHANDLE;
    }

    if(puMxId) {
        *puMxId = pMixInfo->uDeviceID;
    }
    else *puMxId = 0;
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerOpen(LPHMIXER phmx, UINT uMxId, DWORD dwCallback, DWORD dwInstance,
                          DWORD fdwOpen)
{
    DEVICE_STRUCT *pMixInfo;

    if(DartWaveOut::getNumDevices() == 0) {
        if(phmx) *phmx = 0;
        return MMSYSERR_NODRIVER;
    }
    pMixInfo = (DEVICE_STRUCT *)malloc(sizeof(DEVICE_STRUCT));
    if(pMixInfo == NULL) {
        return MMSYSERR_NODRIVER;
    }
    pMixInfo->dwCallback = dwCallback;
    pMixInfo->dwDriverInstance = dwInstance;
    pMixInfo->dwFlags    = fdwOpen;
    pMixInfo->uDeviceID  = uMxId;
    pMixInfo->type       = WINMM_MIXER;
    if(phmx)
    	*phmx = (HMIXER)pMixInfo;
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerClose(HMIXER hmx)
{
    if(hmx) {
    	free((void *)hmx);
    }
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
BOOL mixerInit() 
{
    if(!fMMPMAvailable) return TRUE;

    if(OSLibMixerOpen() == FALSE) {
        return FALSE;
    }
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
void mixerExit()
{
    OSLibMixerClose();
}
/******************************************************************************/
/******************************************************************************/

