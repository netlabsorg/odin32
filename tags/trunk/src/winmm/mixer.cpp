/* $Id: mixer.cpp,v 1.8 2001-02-11 10:33:29 sandervl Exp $ */

/*
 * Mixer stubs
 *
 * Copyright 1998 Joel Troster
 * 
 * TODO: This is just a hack to fool apps into thinking a mixer exists
 *       Must be redesigned
 *
 * Based on Wine code (dlls\winmm\wineoss\mixer.c)
 * Copyright 	1997 Marcus Meissner
 * 		1999 Eric Pouech
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#define NONAMELESSUNION

#include <os2win.h>
#include <string.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <misc.h>

#include "dwaveout.h"
#include "winmm.h"

#define DBG_LOCALLOG	DBG_mixer
#include "dbglocal.h"


ODINDEBUGCHANNEL(WINMM-MIXER)

#define WINMM_MIXERSTRING_A	"OS/2 WINMM Mixer"
#define WINMM_MIXERSTRING_W	(LPWSTR)L"OS/2 WINMM Mixer"

#define WINMM_SHORTNAME_WAVEOUT_A	"Wave Out"
#define WINMM_SHORTNAME_WAVEOUT_W	(LPWSTR)L"Wave Out"
#define WINMM_SHORTNAME_LINEOUT_A	"Line Out"
#define WINMM_SHORTNAME_LINEOUT_W	(LPWSTR)L"Line Out"

#define SOUND_MIXER_LINEOUT	0
#define SOUND_MIXER_WAVEOUT	1
#define SOUND_MIXER_MAXID	2

DWORD MIX_Volume[SOUND_MIXER_MAXID] = { 0x80008000, 0x80008000};
DWORD MIX_Mute[SOUND_MIXER_MAXID]   = { 0, 0};

//
// All Stubs
//

/**************************************************************************
 * 				MIX_SplitControlID		[internal]
 */
static BOOL MIX_SplitControlID(DWORD controlID, LPDWORD lineID, LPDWORD controlType)
{
    *lineID = controlID / 2;
    *controlType = (controlID & 1) ? 
	MIXERCONTROL_CONTROLTYPE_MUTE : MIXERCONTROL_CONTROLTYPE_VOLUME;

    return *lineID < SOUND_MIXER_MAXID;
}
/**************************************************************************
 * 				MIX_MakeControlID		[internal]
 */
static DWORD MIX_MakeControlID(DWORD lineID, DWORD controlType)
{
    switch (controlType) {
    case MIXERCONTROL_CONTROLTYPE_VOLUME:
	return 2 * lineID + 0;
    case MIXERCONTROL_CONTROLTYPE_MUTE:
	return 2 * lineID + 1;
    }
    dprintf(("MIX_MakeControlID: Internal error"));
    return 0x00FADE00;
}
/**************************************************************************
 * 				MIX_DoGetLineControls		[internal]
 */
static void MIX_DoGetLineControls(LPMIXERCONTROLA mc, DWORD lineID, DWORD dwType)
{
    mc->cbStruct = sizeof(MIXERCONTROLA);
    
    switch (dwType) {
    case MIXERCONTROL_CONTROLTYPE_VOLUME:
	mc->dwControlID = MIX_MakeControlID(lineID, dwType);
	mc->dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mc->fdwControl = 0;
	mc->cMultipleItems = 0;
	lstrcpynA(mc->szShortName, "Vol", MIXER_SHORT_NAME_CHARS);
	lstrcpynA(mc->szName, "Volume", MIXER_LONG_NAME_CHARS);
	memset(&mc->Bounds, 0, sizeof(mc->Bounds));
	mc->Bounds.s1.dwMinimum = 0;
	mc->Bounds.s1.dwMaximum = 65535;
	memset(&mc->Metrics, 0, sizeof(mc->Metrics));
	break;
    case MIXERCONTROL_CONTROLTYPE_MUTE:
	mc->dwControlID = MIX_MakeControlID(lineID, dwType);
	mc->dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mc->fdwControl = 0;
	mc->cMultipleItems = 0;
	lstrcpynA(mc->szShortName, "Mute", MIXER_SHORT_NAME_CHARS);
	lstrcpynA(mc->szName, "Mute", MIXER_LONG_NAME_CHARS);
	memset(&mc->Bounds, 0, sizeof(mc->Bounds));
	memset(&mc->Metrics, 0, sizeof(mc->Metrics));
	break;
    default:
	dprintf(("MIX_DoGetLineControls: Internal error: unknown type: %08lx\n", dwType));
    }
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetControlDetailsA,
              HMIXEROBJ, hmxobj,
              LPMIXERCONTROLDETAILS, lpmcd,
              DWORD, fdwDetails)
{
    DWORD	ret = MMSYSERR_NOTSUPPORTED;
    DWORD	lineID, controlType;
   
    if (lpmcd == NULL) return MMSYSERR_INVALPARAM;

    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
	if(MIX_SplitControlID(lpmcd->dwControlID, &lineID, &controlType)) {
	    switch (controlType) 
	    {
	    case MIXERCONTROL_CONTROLTYPE_VOLUME:
		{
		    LPMIXERCONTROLDETAILS_UNSIGNED	mcdu;
		    int					val;

		    /* return value is 00RL (4 bytes)... */
		    val = MIX_Volume[lineID];
	    
		    switch (lpmcd->cChannels) {
		    case 1:
			/* mono... so R = L */
			mcdu = (LPMIXERCONTROLDETAILS_UNSIGNED)lpmcd->paDetails;
			mcdu->dwValue = LOWORD(val);
			break;
		    case 2:
			/* stereo, left is paDetails[0] */
			mcdu = (LPMIXERCONTROLDETAILS_UNSIGNED)((char*)lpmcd->paDetails + 0 * lpmcd->cbDetails);
			mcdu->dwValue = LOWORD(val);
			mcdu = (LPMIXERCONTROLDETAILS_UNSIGNED)((char*)lpmcd->paDetails + 1 * lpmcd->cbDetails);
			mcdu->dwValue = HIWORD(val);
			break;
		    default:
			dprintf(("Unknown cChannels (%ld)\n", lpmcd->cChannels));
			return MMSYSERR_INVALPARAM;
		    }
		}
		break;
	    case MIXERCONTROL_CONTROLTYPE_MUTE:
		{
		    LPMIXERCONTROLDETAILS_BOOLEAN	mcdb;
	    
		    /* we mute both channels at the same time */
		    mcdb = (LPMIXERCONTROLDETAILS_BOOLEAN)lpmcd->paDetails;
		    mcdb->fValue = MIX_Mute[lineID];
		}
		break;
	    }
	    ret = MMSYSERR_NOERROR;
	} else {
	    ret = MMSYSERR_INVALPARAM;
	}
	break;
    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
	dprintf(("mixerGetControlDetailsA: NIY\n"));
	break;
    default:
	dprintf(("Unknown flag (%08lx)\n", fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK));
    }
    return ret;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetControlDetailsW,
              HMIXEROBJ, hmxobj,
              LPMIXERCONTROLDETAILS, pmxcd,
              DWORD, fdwDetails)
{
  dprintf(("WINMM:mixerGetControlDetailsW - stub\n" ));
  return MIXERR_INVALCONTROL;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerSetControlDetails,
              HMIXEROBJ, hmxobj,
              LPMIXERCONTROLDETAILS, lpmcd,
              DWORD, fdwDetails)
{
    DWORD	ret = MMSYSERR_NOTSUPPORTED;
    DWORD	lineID, controlType;
    int		val;
     
    if (lpmcd == NULL) return MMSYSERR_INVALPARAM;
    
    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
	if (MIX_SplitControlID(lpmcd->dwControlID, &lineID, &controlType)) {
	    switch (controlType) {
	    case MIXERCONTROL_CONTROLTYPE_VOLUME:
		{
		    LPMIXERCONTROLDETAILS_UNSIGNED	mcdu;
		    
		    /* val should contain 00RL */
		    switch (lpmcd->cChannels) {
		    case 1:
			/* mono... so R = L */
			mcdu = (LPMIXERCONTROLDETAILS_UNSIGNED)lpmcd->paDetails;
			val = mcdu->dwValue;
			break;
		    case 2:
			/* stereo, left is paDetails[0] */
			mcdu = (LPMIXERCONTROLDETAILS_UNSIGNED)((char*)lpmcd->paDetails + 0 * lpmcd->cbDetails);
			val = mcdu->dwValue;
			mcdu = (LPMIXERCONTROLDETAILS_UNSIGNED)((char*)lpmcd->paDetails + 1 * lpmcd->cbDetails);
			val += (mcdu->dwValue) << 16;
			break;
		    default:
			dprintf(("Unknown cChannels (%ld)\n", lpmcd->cChannels));
			return MMSYSERR_INVALPARAM;
		    }
		    
  	 	    MIX_Volume[lineID] = val;
		}
		ret = MMSYSERR_NOERROR;
		break;
	    case MIXERCONTROL_CONTROLTYPE_MUTE:
		{
		    LPMIXERCONTROLDETAILS_BOOLEAN	mcdb;
		    
		    mcdb = (LPMIXERCONTROLDETAILS_BOOLEAN)lpmcd->paDetails;
		    MIX_Mute[lineID] = mcdb->fValue;
		}
		ret = MMSYSERR_NOERROR;
		break;
	    }
	}
	break;
    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
	dprintf(("mixerSetControlDetails: NIY\n"));
	break;
    default:
	dprintf(("Unknown flag (%08lx)\n", fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK));
    }
    return MMSYSERR_NOTSUPPORTED;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetLineControlsA,
              HMIXEROBJ, hmxobj,
              LPMIXERLINECONTROLSA, lpMlc,
              DWORD, fdwControls)
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
	    MIX_DoGetLineControls(&lpMlc->pamxctrl[0], lpMlc->dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
	    MIX_DoGetLineControls(&lpMlc->pamxctrl[1], lpMlc->dwLineID, MIXERCONTROL_CONTROLTYPE_MUTE);
	}
	break;
    case MIXER_GETLINECONTROLSF_ONEBYID:
	if (MIX_SplitControlID(lpMlc->u.dwControlID, &lineID, &controlType))
	    MIX_DoGetLineControls(&lpMlc->pamxctrl[0], lineID, controlType);
	else 
	    dwRet = MMSYSERR_INVALPARAM;
	break;
    case MIXER_GETLINECONTROLSF_ONEBYTYPE:
	switch (lpMlc->u.dwControlType & MIXERCONTROL_CT_CLASS_MASK) {
	case MIXERCONTROL_CT_CLASS_FADER:
	    MIX_DoGetLineControls(&lpMlc->pamxctrl[0], lpMlc->dwLineID, MIXERCONTROL_CONTROLTYPE_VOLUME);
	    break;
	case MIXERCONTROL_CT_CLASS_SWITCH:
	    MIX_DoGetLineControls(&lpMlc->pamxctrl[0], lpMlc->dwLineID, MIXERCONTROL_CONTROLTYPE_MUTE);
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
ODINFUNCTION3(MMRESULT, mixerGetLineControlsW,
              HMIXEROBJ, hmxobj,
              LPMIXERLINECONTROLSW, pmxlc,
              DWORD, fdwControls)
{
  dprintf(("WINMM:mixerGetGetLineControlsW - stub\n" ));
  return MIXERR_INVALLINE;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetDevCapsA,
              UINT, uMxId,
              LPMIXERCAPSA, pmxcaps,
              UINT, cbmxcaps)
{
  if(DartWaveOut::getNumDevices() == 0) {
        memset(pmxcaps, 0, sizeof(*pmxcaps));
        return MMSYSERR_NODRIVER;
  }

  // we have to fill in this thing
  pmxcaps->wMid = 0;                  /* manufacturer ID */
  pmxcaps->wPid = 0;                  /* product ID */
  pmxcaps->vDriverVersion = 0x0001;        /* version of the driver */
  strcpy( pmxcaps->szPname, WINMM_MIXERSTRING_A); /* product name */

  pmxcaps->fdwSupport = 0;
  pmxcaps->cDestinations = 1;

  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetDevCapsW,
              UINT, uMxId,
              LPMIXERCAPSW, pmxcaps,
              UINT, cbmxcaps)
{
  if(DartWaveOut::getNumDevices() == 0) {
        memset(pmxcaps, 0, sizeof(*pmxcaps));
        return MMSYSERR_NODRIVER;
  }

  // we have to fill in this thing
  pmxcaps->wMid = 0;                  /* manufacturer ID */
  pmxcaps->wPid = 0;                  /* product ID */
  pmxcaps->vDriverVersion = 0x0001;        /* version of the driver */
  lstrcpyW( pmxcaps->szPname, WINMM_MIXERSTRING_W ); /* product name */

  pmxcaps->fdwSupport = 0;
  pmxcaps->cDestinations = 1;

  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetID,
              HMIXEROBJ, hmxobj,
              UINT *, puMxId,
              DWORD, fdwId)
{
 DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

  if(pMixInfo && puMxId) {
       *puMxId = pMixInfo->uDeviceID;
  }
  else *puMxId = 0;
  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION3(MMRESULT, mixerGetLineInfoA,
              HMIXEROBJ, hmxobj,
              LPMIXERLINEA, lpMl,
              DWORD, fdwInfo)
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
	/* FIXME: Linux doesn't seem to support multiple outputs? 
	 * So we have only one output type: Speaker.
	 */
	lpMl->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	lpMl->dwSource = 0xFFFFFFFF;
	lpMl->dwLineID = SOUND_MIXER_LINEOUT;
	lstrcpynA(lpMl->szShortName, WINMM_SHORTNAME_LINEOUT_A, MIXER_SHORT_NAME_CHARS);
	lstrcpynA(lpMl->szName, WINMM_SHORTNAME_LINEOUT_A, MIXER_LONG_NAME_CHARS);
	
	/* we have all connections found in the MIX_DevMask */
	lpMl->cConnections = 1; //waveout
       	lpMl->cControls    = 1;
        lpMl->cChannels    = 2;
	break;

    case MIXER_GETLINEINFOF_LINEID:
	if(lpMl->dwLineID >= SOUND_MIXER_MAXID)
	    return MIXERR_INVALLINE;

        lpMl->cChannels     = 2;
        lpMl->dwDestination = 0; /* index for speakers */
	if(lpMl->dwLineID == SOUND_MIXER_WAVEOUT) {
        	lpMl->dwLineID      = SOUND_MIXER_WAVEOUT;
		lstrcpynA(lpMl->szShortName, WINMM_SHORTNAME_WAVEOUT_A, MIXER_SHORT_NAME_CHARS);
		lstrcpynA(lpMl->szName, WINMM_SHORTNAME_WAVEOUT_A, MIXER_LONG_NAME_CHARS);
        	lpMl->cConnections  = 0;
        	lpMl->cControls     = 1;
		lpMl->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
		lpMl->fdwLine	     |= MIXERLINE_LINEF_SOURCE;
	}
	else {
		lpMl->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		lpMl->dwSource = 0xFFFFFFFF;
        	lpMl->dwLineID = SOUND_MIXER_LINEOUT;
		lstrcpynA(lpMl->szShortName, WINMM_SHORTNAME_LINEOUT_A, MIXER_SHORT_NAME_CHARS);
		lstrcpynA(lpMl->szName, WINMM_SHORTNAME_LINEOUT_A, MIXER_LONG_NAME_CHARS);
        	lpMl->cConnections  = 1;
        	lpMl->cControls     = 1;
	}
	break;

    case MIXER_GETLINEINFOF_SOURCE:
	if(lpMl->dwSource != SOUND_MIXER_WAVEOUT)
	    return MIXERR_INVALLINE;

       	lpMl->dwLineID      = SOUND_MIXER_WAVEOUT;
	lstrcpynA(lpMl->szShortName, WINMM_SHORTNAME_WAVEOUT_A, MIXER_SHORT_NAME_CHARS);
	lstrcpynA(lpMl->szName, WINMM_SHORTNAME_WAVEOUT_A, MIXER_LONG_NAME_CHARS);
       	lpMl->cConnections  = 0;
       	lpMl->cControls     = 1;
	lpMl->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	lpMl->fdwLine	     |= MIXERLINE_LINEF_SOURCE;
        lpMl->cChannels     = 2;
        lpMl->dwLineID      = SOUND_MIXER_WAVEOUT;
        lpMl->dwDestination = 0; /* index for speakers */
	break;

    case MIXER_GETLINEINFOF_COMPONENTTYPE:
	switch (lpMl->dwComponentType) 
	{
	case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:
	    	lpMl->dwDestination = 0;
		lpMl->dwSource = 0xFFFFFFFF;
        	lpMl->dwLineID = SOUND_MIXER_LINEOUT;
   	    	lstrcpynA(lpMl->szShortName, WINMM_SHORTNAME_LINEOUT_A, MIXER_SHORT_NAME_CHARS);
	    	lstrcpynA(lpMl->szName, WINMM_SHORTNAME_LINEOUT_A, MIXER_LONG_NAME_CHARS);
	    	break;
	case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT:
       	 	lpMl->dwLineID = SOUND_MIXER_WAVEOUT;
	    	lpMl->fdwLine |= MIXERLINE_LINEF_SOURCE;
	    	lstrcpynA(lpMl->szShortName, WINMM_SHORTNAME_WAVEOUT_A, MIXER_SHORT_NAME_CHARS);
            	lstrcpynA(lpMl->szName, WINMM_SHORTNAME_WAVEOUT_A, MIXER_LONG_NAME_CHARS);
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
ODINFUNCTION3(MMRESULT, mixerGetLineInfoW,
              HMIXEROBJ, hmxobj,
              LPMIXERLINEW, pmxl,
              DWORD, fdwInfo)
{
  dprintf(("WINMM:mixerGetLineInfoW - stub\n" ));
  return MIXERR_INVALLINE;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION4(MMRESULT, mixerMessage,
              HMIXER, hmx,
              UINT, uMsg,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM:mixerMessage - stub\n" ));
  return 0;
}

/******************************************************************************/
ODINFUNCTION0(UINT, mixerGetNumDevs)
{
  if(DartWaveOut::getNumDevices() == 0) {
        return 0;
  }
  return 1;
}
/******************************************************************************/
/******************************************************************************/
ODINFUNCTION5(MMRESULT, mixerOpen,
              LPHMIXER, phmx,
              UINT, uMxId,
              DWORD, dwCallback,
              DWORD, dwInstance,
              DWORD, fdwOpen)
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
ODINFUNCTION1(MMRESULT, mixerClose,
              HMIXER, hmx)
{
  if(hmx) {
	free((void *)hmx);
  }
  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/


