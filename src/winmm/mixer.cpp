/* $Id: mixer.cpp,v 1.14 2002-05-23 20:42:06 sandervl Exp $ */

/*
 * Mixer functions
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * 
 * TODO: Mixer notification
 * 
 * NOTE: Not really flexible (capabilities, > 1 audio card)
 *
 * Some portions borrowed from Wine (X11): (dll\winmm\mmsystem.c)
 * (mixerGetLineControlsW/mixerGetControlDetailsW)
 * Copyright 1993 Martin Ayotte
 *                Eric POUECH 
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
#include <dbglog.h>
#include <mmsystem.h>
#include <winnls.h>

#include "waveoutdart.h"
#include "winmm.h"
#include "initwinmm.h"
#include "mixer.h"
#include "mixeros2.h"

#define DBG_LOCALLOG	DBG_mixer
#include "dbglocal.h"

static BOOL        mixerAddControl(MIXERLINEA *pDestLine, DWORD dwControl, MIXERLINEA *pSrcLine);
static MIXERLINEA *mixerAddSource(MIXERLINEA *pDestLine, DWORD dwSource);
static MIXERLINEA *mixerAddDestination(DWORD dwDest);

//array of destination mixer lines
static MIXERLINEA    mixerDest[MAX_MIXER_DESTINATIONS]                          = {0};
//array of connected sources to destinations
static int           mixerDestInputs[MAX_MIXER_DESTINATIONS][MAX_MIXER_SOURCES] = {-1};
//array of source mixer lines
static MIXERLINEA    mixerSource[MAX_MIXER_SOURCES]                             = {0};
//array of controls connected to mixer lines
static int           mixerLineControls[MAX_MIXER_LINES][MAX_SOURCE_CONTROLS]    = {-1};
//array of all mixer lines
static MIXERLINEA   *pmixerLines[MAX_MIXER_LINES]                               = {NULL};
//mapping from src to line id
static int           mixerSourceLineId[MAX_MIXER_SOURCES]                       = {-1};
//array of all mixer controls
static MIXERCONTROLA mixerControls[MAX_MIXER_CONTROLS]                          = {0};

static int           nrDestinations    = 0;
static int           nrSources         = 0;
static int           nrControls        = 0;
static int           nrLines           = 0;
static int           nrLineOutInputs   = 0;
static int           nrWaveInInputs    = 0;

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
    DWORD	lineID, controlType;
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;
   
    if(fdwDetails == MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if(fdwDetails == MIXER_OBJECTF_MIXER) {
        if(hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }

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
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
    DWORD ret = MMSYSERR_NOTENABLED;

    if (lpmcd == NULL || lpmcd->cbStruct != sizeof(*lpmcd))
	    return MMSYSERR_INVALPARAM;

    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
	    /* can safely use W structure as it is, no string inside */
	    ret = mixerGetControlDetailsA(hmxobj, lpmcd, fdwDetails);
	    break;

    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
	{
	    LPVOID	paDetailsW = lpmcd->paDetails;
	    int		size = max(1, lpmcd->cChannels) * sizeof(MIXERCONTROLDETAILS_LISTTEXTA);

	    if (lpmcd->u.cMultipleItems != 0 && lpmcd->u.cMultipleItems != lpmcd->u.hwndOwner) {
		    size *= lpmcd->u.cMultipleItems;
	    }
	    lpmcd->paDetails = HeapAlloc(GetProcessHeap(), 0, size);
	    /* set up lpmcd->paDetails */
	    ret = mixerGetControlDetailsA(hmxobj, lpmcd, fdwDetails);
	    /* copy from lpmcd->paDetails back to paDetailsW; */
	    HeapFree(GetProcessHeap(), 0, lpmcd->paDetails);
	    lpmcd->paDetails = paDetailsW;
	    break;
	}
    default:
    	dprintf(("Unsupported fdwDetails=0x%08lx\n", fdwDetails));
        break;
    }

    return ret;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
    DWORD	ret = MMSYSERR_NOTSUPPORTED;
    DWORD	lineID, controlType;
    int		val;
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;
     
    if(fdwDetails == MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if(fdwDetails == MIXER_OBJECTF_MIXER) {
        if(hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }
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
MMRESULT WINAPI mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA lpMlc, DWORD fdwControls)
{
    DWORD dwRet = MMSYSERR_NOERROR;
    DWORD lineID, controlType;
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if(fdwControls == MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if(fdwControls == MIXER_OBJECTF_MIXER) {
        if(hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }

    if (lpMlc == NULL) return MMSYSERR_INVALPARAM;

    if (lpMlc->cbStruct < sizeof(*lpMlc) || lpMlc->cbmxctrl < sizeof(MIXERCONTROLA))
	    return MMSYSERR_INVALPARAM;

    switch(fdwControls & MIXER_GETLINECONTROLSF_QUERYMASK) 
    {
    case MIXER_GETLINECONTROLSF_ALL:
        dprintf(("MIXER_GETLINECONTROLSF_ALL"));
        return MIXERR_INVALLINE;
	    break;

    case MIXER_GETLINECONTROLSF_ONEBYID:
        dprintf(("MIXER_GETLINECONTROLSF_ONEBYID %x", lpMlc->u.dwControlID));
        if(lpMlc->cControls != 1 || lpMlc->cbmxctrl != sizeof(MIXERCONTROLA)) {
            dprintf(("invalid parameters"));
            return MMSYSERR_INVALPARAM;
        }
        if(lpMlc->u.dwControlID >= nrControls) {
            dprintf(("invalid control"));
            return MIXERR_INVALCONTROL;
        }
        memcpy(lpMlc->pamxctrl, &mixerControls[lpMlc->u.dwControlID], sizeof(MIXERCONTROLA));
        dprintf(("found control %s (%s)", lpMlc->pamxctrl->szName, lpMlc->pamxctrl->szShortName));
	    break;

    case MIXER_GETLINECONTROLSF_ONEBYTYPE:
    {
        dprintf(("MIXER_GETLINECONTROLSF_ONEBYTYPE %x %d", lpMlc->u.dwControlType, lpMlc->dwLineID));
        if(lpMlc->dwLineID >= nrLines) {
            dprintf(("ERROR: Invalid line %d", lpMlc->dwLineID));
            return MIXERR_INVALLINE;
        }
#ifdef DEBUG
	    switch (lpMlc->u.dwControlType) {
        case MIXERCONTROL_CONTROLTYPE_CUSTOM:
            dprintf(("MIXERCONTROL_CONTROLTYPE_CUSTOM"));
            break;
        case MIXERCONTROL_CONTROLTYPE_BOOLEANMETER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_BOOLEANMETER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_SIGNEDMETER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_SIGNEDMETER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_PEAKMETER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_PEAKMETER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_BOOLEAN:
            dprintf(("MIXERCONTROL_CONTROLTYPE_BOOLEAN"));
            break;
        case MIXERCONTROL_CONTROLTYPE_ONOFF:
            dprintf(("MIXERCONTROL_CONTROLTYPE_ONOFF"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MUTE:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MUTE"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MONO:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MONO"));
            break;
        case MIXERCONTROL_CONTROLTYPE_LOUDNESS:
            dprintf(("MIXERCONTROL_CONTROLTYPE_LOUDNESS"));
            break;
        case MIXERCONTROL_CONTROLTYPE_STEREOENH:
            dprintf(("MIXERCONTROL_CONTROLTYPE_STEREOENH"));
            break;
        case MIXERCONTROL_CONTROLTYPE_BUTTON:
            dprintf(("MIXERCONTROL_CONTROLTYPE_BUTTON"));
            break;
        case MIXERCONTROL_CONTROLTYPE_DECIBELS:
            dprintf(("MIXERCONTROL_CONTROLTYPE_DECIBELS"));
            break;
        case MIXERCONTROL_CONTROLTYPE_SIGNED:
            dprintf(("MIXERCONTROL_CONTROLTYPE_SIGNED"));
            break;
        case MIXERCONTROL_CONTROLTYPE_UNSIGNED:
            dprintf(("MIXERCONTROL_CONTROLTYPE_UNSIGNED"));
            break;
        case MIXERCONTROL_CONTROLTYPE_PERCENT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_PERCENT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_SLIDER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_SLIDER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_PAN:
            dprintf(("MIXERCONTROL_CONTROLTYPE_PAN"));
            break;
        case MIXERCONTROL_CONTROLTYPE_QSOUNDPAN:
            dprintf(("MIXERCONTROL_CONTROLTYPE_QSOUNDPAN"));
            break;
        case MIXERCONTROL_CONTROLTYPE_FADER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_FADER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_VOLUME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_VOLUME"));
            break;
        case MIXERCONTROL_CONTROLTYPE_BASS:
            dprintf(("MIXERCONTROL_CONTROLTYPE_BASS"));
            break;
        case MIXERCONTROL_CONTROLTYPE_TREBLE:
            dprintf(("MIXERCONTROL_CONTROLTYPE_TREBLE"));
            break;
        case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_EQUALIZER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_SINGLESELECT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MUX:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MUX"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MIXER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MIXER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MICROTIME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MICROTIME"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MILLITIME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MILLITIME"));
            break;
	    default:
    	    return MMSYSERR_INVALPARAM;
    	}
#endif
        int idx;

        for(int i=0;i<MAX_SOURCE_CONTROLS;i++) {
            idx = mixerLineControls[lpMlc->dwLineID][i];
            if(idx == -1) break;
            if(mixerControls[idx].dwControlType == lpMlc->u.dwControlType) {
                memcpy(lpMlc->pamxctrl, &mixerControls[idx], sizeof(MIXERCONTROLA));
                dprintf(("found control %s (%s)", lpMlc->pamxctrl->szName, lpMlc->pamxctrl->szShortName));
                return MMSYSERR_NOERROR;
            }

        }
        return MIXERR_INVALLINE;
    }
    default:
	    dprintf(("Unknown flag %08lx\n", fdwControls & MIXER_GETLINECONTROLSF_QUERYMASK));
	    dwRet = MMSYSERR_INVALPARAM;
        break;
    }

    return dwRet;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW lpmlcW, DWORD fdwControls)
{
    MIXERLINECONTROLSA	mlcA;
    DWORD		ret;
    int			i;

    if (lpmlcW == NULL || lpmlcW->cbStruct != sizeof(*lpmlcW) || lpmlcW->cbmxctrl != sizeof(MIXERCONTROLW))
	    return MMSYSERR_INVALPARAM;

    mlcA.cbStruct = sizeof(mlcA);
    mlcA.dwLineID = lpmlcW->dwLineID;
    mlcA.u.dwControlID = lpmlcW->u.dwControlID;
    mlcA.u.dwControlType = lpmlcW->u.dwControlType;
    mlcA.cControls = lpmlcW->cControls;
    mlcA.cbmxctrl = sizeof(MIXERCONTROLA);
    mlcA.pamxctrl = (MIXERCONTROLA *)HeapAlloc(GetProcessHeap(), 0, mlcA.cControls * mlcA.cbmxctrl);

    ret = mixerGetLineControlsA(hmxobj, &mlcA, fdwControls);

    if (ret == MMSYSERR_NOERROR) {
	    lpmlcW->dwLineID = mlcA.dwLineID;
	    lpmlcW->u.dwControlID = mlcA.u.dwControlID;
	    lpmlcW->u.dwControlType = mlcA.u.dwControlType;
	    lpmlcW->cControls = mlcA.cControls;
	
    	for (i = 0; i < mlcA.cControls; i++) {
	        lpmlcW->pamxctrl[i].cbStruct = sizeof(MIXERCONTROLW);
	        lpmlcW->pamxctrl[i].dwControlID = mlcA.pamxctrl[i].dwControlID;
	        lpmlcW->pamxctrl[i].dwControlType = mlcA.pamxctrl[i].dwControlType;
	        lpmlcW->pamxctrl[i].fdwControl = mlcA.pamxctrl[i].fdwControl;
	        lpmlcW->pamxctrl[i].cMultipleItems = mlcA.pamxctrl[i].cMultipleItems;
            MultiByteToWideChar( CP_ACP, 0, mlcA.pamxctrl[i].szShortName, -1,
                                 lpmlcW->pamxctrl[i].szShortName,
                                 sizeof(lpmlcW->pamxctrl[i].szShortName)/sizeof(WCHAR) );
            MultiByteToWideChar( CP_ACP, 0, mlcA.pamxctrl[i].szName, -1,
                                 lpmlcW->pamxctrl[i].szName,
                                 sizeof(lpmlcW->pamxctrl[i].szName)/sizeof(WCHAR) );
	        /* sizeof(lpmlcW->pamxctrl[i].Bounds) == 
	         * sizeof(mlcA.pamxctrl[i].Bounds) */
	        memcpy(&lpmlcW->pamxctrl[i].Bounds, &mlcA.pamxctrl[i].Bounds, 
		           sizeof(mlcA.pamxctrl[i].Bounds));
	        /* sizeof(lpmlcW->pamxctrl[i].Metrics) == 
	         * sizeof(mlcA.pamxctrl[i].Metrics) */
	        memcpy(&lpmlcW->pamxctrl[i].Metrics, &mlcA.pamxctrl[i].Metrics, 
		           sizeof(mlcA.pamxctrl[i].Metrics));
	    }
    }

    HeapFree(GetProcessHeap(), 0, mlcA.pamxctrl);
    return ret;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA lpMl, DWORD fdwInfo)
{
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if(fdwInfo & MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if((fdwInfo & MIXER_OBJECTF_MIXER) == MIXER_OBJECTF_MIXER) {
        if(hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }

    if (lpMl == NULL || lpMl->cbStruct != sizeof(*lpMl)) {
        dprintf(("ERROR: mixerGetLineInfoA: invalid paramter!!"));
	    return MMSYSERR_INVALPARAM;
    }
    
    switch (fdwInfo & MIXER_GETLINEINFOF_QUERYMASK) 
    {
    case MIXER_GETLINEINFOF_DESTINATION:
        dprintf(("MIXER_GETLINEINFOF_DESTINATION %d", lpMl->dwDestination));
        if(lpMl->dwDestination >= nrDestinations) {
            dprintf(("ERROR: Invalid destination %d", lpMl->dwDestination));
            return MMSYSERR_INVALPARAM;
        }
        memcpy(lpMl, &mixerDest[lpMl->dwDestination], sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->cConnections, lpMl->cControls));
    	break;

    case MIXER_GETLINEINFOF_LINEID:
        dprintf(("MIXER_GETLINEINFOF_LINEID %d", lpMl->dwLineID));
        if(lpMl->dwLineID >= nrLines) {
            dprintf(("ERROR: Invalid destination %d", lpMl->dwLineID));
            return MIXERR_INVALLINE;
        }
        memcpy(lpMl, pmixerLines[lpMl->dwLineID], sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->cConnections, lpMl->cControls));
    	break;

    case MIXER_GETLINEINFOF_SOURCE:
        dprintf(("MIXER_GETLINEINFOF_SOURCE %d %d", lpMl->dwDestination, lpMl->dwSource));
        if(lpMl->dwDestination >= nrDestinations) {
            dprintf(("ERROR: Invalid destination %d", lpMl->dwDestination));
            return MIXERR_INVALLINE;
        }
        if(lpMl->dwSource >= MIXER_SRC_MAX) {
            dprintf(("ERROR: Invalid destination %d", lpMl->dwDestination));
            return MIXERR_INVALLINE;
        }
        if(mixerDestInputs[lpMl->dwDestination][lpMl->dwSource] == -1) {
            dprintf(("ERROR: Invalid destination %d", lpMl->dwDestination));
            return MIXERR_INVALLINE;
        }
        memcpy(lpMl, &mixerSource[mixerDestInputs[lpMl->dwDestination][lpMl->dwSource]], sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->cConnections, lpMl->cControls));
    	break;

    case MIXER_GETLINEINFOF_COMPONENTTYPE:
        dprintf(("MIXER_GETLINEINFOF_COMPONENTTYPE"));
	    switch (lpMl->dwComponentType) 
	    {
        case MIXERLINE_COMPONENTTYPE_DST_UNDEFINED:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_UNDEFINED"));
            break;
    	case MIXERLINE_COMPONENTTYPE_DST_DIGITAL:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_DIGITAL"));
            break;
    	case MIXERLINE_COMPONENTTYPE_DST_LINE:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_LINE"));
            break;
    	case MIXERLINE_COMPONENTTYPE_DST_MONITOR:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_MONITOR"));
            break;
    	case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_SPEAKERS"));
            break;
    	case MIXERLINE_COMPONENTTYPE_DST_HEADPHONES:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_HEADPHONES"));
            break;
        case MIXERLINE_COMPONENTTYPE_DST_TELEPHONE:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_TELEPHONE"));
            break;
        case MIXERLINE_COMPONENTTYPE_DST_WAVEIN:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_WAVEIN"));
            break;
        case MIXERLINE_COMPONENTTYPE_DST_VOICEIN:
            dprintf(("MIXERLINE_COMPONENTTYPE_DST_VOICEIN"));
            break;
    	default:
	    	dprintf(("Unhandled component type (%08lx)\n", lpMl->dwComponentType));
	    	return MMSYSERR_INVALPARAM;
	    }
        return MIXERR_INVALLINE;
	    break;

    case MIXER_GETLINEINFOF_TARGETTYPE:
	    dprintf(("_TARGETTYPE not implemented yet.\n"));
        return MIXERR_INVALLINE;
	    break;
    default:
	    dprintf(("Unknown flag (%08lx)\n", fdwInfo & MIXER_GETLINEINFOF_QUERYMASK));
	    return MMSYSERR_INVALPARAM;
	    break;
    }   
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo)
{
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;
    MIXERLINEA     line;
    MMRESULT       result;

    line.cbStruct              = sizeof(MIXERLINEA);
    line.cChannels             = pmxl->cChannels;
    line.cConnections          = pmxl->cConnections;
    line.dwComponentType       = pmxl->dwComponentType;
    line.dwDestination         = pmxl->dwDestination;
    line.dwLineID              = pmxl->dwLineID;
    line.dwSource              = pmxl->dwSource;
    line.dwUser                = pmxl->dwUser;
    line.fdwLine               = pmxl->fdwLine;
    line.szName[0]             = 0;
    line.szShortName[0]        = 0;
    line.Target.dwDeviceID     = pmxl->Target.dwDeviceID;
    line.Target.dwType         = pmxl->Target.dwType;
    line.Target.vDriverVersion = pmxl->Target.vDriverVersion;
    line.Target.wMid           = pmxl->Target.wMid;
    line.Target.wPid           = pmxl->Target.wPid;
    line.Target.szPname[0]     = 0;

    result = mixerGetLineInfoA(hmxobj, &line, fdwInfo);
    if(result != MMSYSERR_NOERROR) {
        return result;
    }
    pmxl->cbStruct              = sizeof(MIXERLINEA);
    pmxl->cChannels             = line.cChannels;
    pmxl->cConnections          = line.cConnections;
    pmxl->dwComponentType       = line.dwComponentType;
    pmxl->dwDestination         = line.dwDestination;
    pmxl->dwLineID              = line.dwLineID;
    pmxl->dwSource              = line.dwSource;
    pmxl->dwUser                = line.dwUser;
    pmxl->fdwLine               = line.fdwLine;
    MultiByteToWideChar(CP_ACP, 0, line.szName, -1, pmxl->szName, sizeof(line.szName));
    MultiByteToWideChar(CP_ACP, 0, line.szShortName, -1, pmxl->szShortName, sizeof(line.szShortName));
    pmxl->Target.dwDeviceID     = line.Target.dwDeviceID;
    pmxl->Target.dwType         = line.Target.dwType;
    pmxl->Target.vDriverVersion = line.Target.vDriverVersion;
    pmxl->Target.wMid           = line.Target.wMid;
    pmxl->Target.wPid           = line.Target.wPid;
    pmxl->Target.szPname[0]     = 0;
    MultiByteToWideChar(CP_ACP, 0, line.Target.szPname, -1, pmxl->Target.szPname, sizeof(line.Target.szPname));
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerMessage(HMIXER hmx, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
    dprintf(("WINMM:mixerMessage - stub\n" ));
    return MMSYSERR_INVALPARAM;
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
        pmxcaps->cDestinations = nrDestinations;
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
        pmxcaps->cDestinations = nrDestinations;
    }

    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetID(HMIXEROBJ hmxobj, UINT * puMxId, DWORD fdwId)
{
    switch(fdwId) {
    case MIXER_OBJECTF_MIXER:
    {
        DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;
        if(!pMixInfo) {
            if(puMxId) {
                *puMxId = -1;
            }
            return MMSYSERR_INVALHANDLE;
        }
        if(puMxId) {
            *puMxId = pMixInfo->uDeviceID;
        }
        break;
    }

    case MIXER_OBJECTF_HMIXER:
    case MIXER_OBJECTF_WAVEOUT:
    case MIXER_OBJECTF_HWAVEOUT:
    case MIXER_OBJECTF_WAVEIN:
    case MIXER_OBJECTF_HWAVEIN:
    case MIXER_OBJECTF_MIDIOUT:
    case MIXER_OBJECTF_HMIDIOUT:
    case MIXER_OBJECTF_MIDIIN:
    case MIXER_OBJECTF_HMIDIIN:
    case MIXER_OBJECTF_AUX:
        //TODO: assume default mixer
        if(puMxId) {
            *puMxId = 0;
        }
        break;
    default:
        if(puMxId) {
            *puMxId = -1;
        }
        return MMSYSERR_INVALPARAM;
    }
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
    MIXERLINEA *pDestLine;

    if(!fMMPMAvailable) return TRUE;

    memset(mixerSourceLineId, -1, sizeof(mixerSourceLineId));
    memset(mixerLineControls, -1, sizeof(mixerLineControls));
    memset(mixerDestInputs, -1, sizeof(mixerDestInputs));

    if(OSLibMixerOpen() == FALSE) {
        //Line out destination
        pDestLine = mixerAddDestination(MIXER_DEST_LINEOUT);

        mixerAddControl(pDestLine, MIX_CTRL_VOL_OUT_LINE, pDestLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_OUT_LINE, pDestLine);

        //WaveIn destination
        pDestLine = mixerAddDestination(MIXER_DEST_WAVEIN);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_MIC, pDestLine);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_LINE, pDestLine);
        //must add after all recording inputs (to count them)
        mixerAddControl(pDestLine, MIX_CTRL_MUX_IN_W_SRC, pDestLine);
        return TRUE;
    }
    //Line out destination
    pDestLine = mixerAddDestination(MIXER_DEST_LINEOUT);

    mixerAddControl(pDestLine, MIX_CTRL_VOL_OUT_LINE, pDestLine);
    mixerAddControl(pDestLine, MIX_CTRL_MUTE_OUT_LINE, pDestLine);
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_3DCENTER)) {
        mixerAddControl(pDestLine, MIX_CTRL_OUT_L_3DCENTER, pDestLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_3DDEPTH)) {
        mixerAddControl(pDestLine, MIX_CTRL_OUT_L_3DDEPTH, pDestLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_TREBLE)) {
        mixerAddControl(pDestLine, MIX_CTRL_OUT_L_TREBLE, pDestLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_BASS)) {
        mixerAddControl(pDestLine, MIX_CTRL_OUT_L_BASS, pDestLine);
    }

    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_SPDIF)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_SPDIF);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_SPDIF, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_SPDIF, pLine);
    }

    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MONO)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_MONOIN);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_MONO, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_MONO, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_PHONE)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_PHONE);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_PHONE, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_PHONE, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MIC)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_MIC);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_MIC, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_MIC, pLine);
        if(OSLibMixIsControlPresent(MIX_CTRL_BOOST_IN_L_MIC)) {
            mixerAddControl(pDestLine, MIX_CTRL_BOOST_IN_L_MIC, pLine);
        }
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_LINE)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_LINE);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_LINE, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_LINE, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_CD)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_CD);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_CD, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_CD, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_VIDEO)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_VIDEO);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_VIDEO, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_VIDEO, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_AUX)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_AUX);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_AUX, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_AUX, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_PCM)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_PCM);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_PCM, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_PCM, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_WAVETABLE)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_WAVETABLE);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_WAVETABLE, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_WAVETABLE, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MIDI)) {
        MIXERLINEA *pLine = mixerAddSource(pDestLine, MIXER_SRC_MIDI);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_L_MIDI, pLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_IN_L_MIDI, pLine);
    }

    //Wave In Destination
    if(OSLibMixIsControlPresent(MIX_CTRL_MUX_IN_W_SRC)) {
        pDestLine = mixerAddDestination(MIXER_DEST_WAVEIN);
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_MONO)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_MONO, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_PHONE)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_PHONE, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_MIC)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_MIC, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_LINE)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_LINE, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_CD)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_CD, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_SPDIF)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_SPDIF, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_VIDEO)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_VIDEO, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_AUX)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_AUX, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_PCM)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_PCM, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_WAVETABLE)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_WAVETABLE, pDestLine);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_MIDI)) {
            mixerAddControl(pDestLine, MIX_CTRL_VOL_IN_W_MIDI, pDestLine);
        }
        //must add after all recording inputs (to count them)
        mixerAddControl(pDestLine, MIX_CTRL_MUX_IN_W_SRC, pDestLine);
    }

    //SPDIF destination
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_OUT_SPDIF)) {
        pDestLine = mixerAddDestination(MIXER_DEST_SPDIFOUT);
        mixerAddControl(pDestLine, MIX_CTRL_VOL_OUT_SPDIF, pDestLine);
        mixerAddControl(pDestLine, MIX_CTRL_MUTE_OUT_SPDIF, pDestLine);
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
static MIXERLINEA *mixerAddSource(MIXERLINEA *pDestLine, DWORD dwSource)
{
    MIXERLINEA *pline  = &mixerSource[nrSources];

    if(nrSources >= MAX_MIXER_SOURCES) {
        dprintf(("ERROR: mixerAddSource: out of room!!!"));
        DebugInt3();
        return NULL;
    }

    pline->cbStruct = sizeof(MIXERLINEA);
    memset(pline, 0, sizeof(MIXERLINEA));

    pline->cConnections    = 0;
    pline->cControls       = 0;
    pline->dwDestination   = 0;
    pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
    pline->dwSource        = nrSources;

    switch(dwSource) {
    case MIXER_SRC_MONOIN:
    case MIXER_SRC_PHONE:
        pline->cChannels       = 1;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_MIC:
        pline->cChannels       = 1;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_LINE:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_LINE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_CD:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_SPDIF:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_DIGITAL;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_VIDEO:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_AUX:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_AUX;
        break;

    case MIXER_SRC_PCM:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_WAVEOUT;
        break;

    case MIXER_SRC_WAVETABLE:
    case MIXER_SRC_MIDI:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_MIDIOUT;
        break;

    default:
        DebugInt3();
        return NULL;
    }
    strncpy(pline->szShortName, szSourceName[dwSource][0], sizeof(pline->szShortName));
    strncpy(pline->szName,      szSourceName[dwSource][1], sizeof(pline->szName));

    dprintf(("Adding Source %s (%s)", pline->szName, pline->szShortName));
    nrSources++;

    mixerDestInputs[MIXER_DEST_LINEOUT][nrLineOutInputs] = pline->dwSource;
    nrLineOutInputs++;

    pline->dwLineID      = nrLines;
    pmixerLines[nrLines] = pline;
    nrLines++;

    //increase nr of inputs at destinateion line
    pDestLine->cConnections++;

    //store line id in source to line mapping array
    mixerSourceLineId[dwSource] = pline->dwLineID;

    return pline;
}
/******************************************************************************/
/******************************************************************************/
static MIXERLINEA *mixerAddDestination(DWORD dwDest)
{
    MIXERLINEA *pline  = &mixerDest[nrDestinations];

    if(nrDestinations >= MAX_MIXER_DESTINATIONS) {
        dprintf(("ERROR: mixerAddDestination: out of room!!!"));
        DebugInt3();
        return NULL;
    }
    pline->cbStruct = sizeof(MIXERLINEA);
    memset(pline, 0, sizeof(MIXERLINEA));

    switch(dwDest) {
    case MIXER_DEST_LINEOUT:
        pline->dwDestination   = nrDestinations;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
        pline->dwSource        = 0;
        pline->fdwLine         = MIXERLINE_LINEF_ACTIVE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_DEST_SPDIFOUT:
        pline->dwDestination   = nrDestinations;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_DIGITAL;
        pline->dwSource        = 0;
        pline->fdwLine         = MIXERLINE_LINEF_ACTIVE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_DEST_WAVEIN:
        pline->dwSource        = nrDestinations;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        pline->dwDestination   = 0;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;
        
    default:
        DebugInt3();
        return NULL;
    }
    strncpy(pline->szShortName, szDestName[dwDest][0], sizeof(pline->szShortName));
    strncpy(pline->szName,      szDestName[dwDest][1], sizeof(pline->szName));
    dprintf(("Adding destination %s (%s) connections %d controls %d", pline->szName, pline->szShortName, pline->cConnections, pline->cControls));
    nrDestinations++;

    pline->dwLineID      = nrLines;
    pmixerLines[nrLines] = pline;
    nrLines++;
    return pline;
}
/******************************************************************************/
/******************************************************************************/
static int mixerWaveInVolToSource(DWORD dwControl) 
{
    switch(dwControl) {
    case MIX_CTRL_VOL_IN_W_MONO:
        return MIXER_SRC_MONOIN;
    case MIX_CTRL_VOL_IN_W_PHONE:
        return MIXER_SRC_PHONE;
    case MIX_CTRL_VOL_IN_W_MIC:
        return MIXER_SRC_MIC;
    case MIX_CTRL_VOL_IN_W_LINE:
        return MIXER_SRC_LINE;
    case MIX_CTRL_VOL_IN_W_CD:
        return MIXER_SRC_CD;
    case MIX_CTRL_VOL_IN_W_SPDIF:
        return MIXER_SRC_SPDIF;
    case MIX_CTRL_VOL_IN_W_VIDEO:
        return MIXER_SRC_VIDEO;
    case MIX_CTRL_VOL_IN_W_AUX:
        return MIXER_SRC_AUX;
    case MIX_CTRL_VOL_IN_W_PCM:
        return MIXER_SRC_PCM;
    case MIX_CTRL_VOL_IN_W_WAVETABLE:
        return MIXER_SRC_WAVETABLE;
    case MIX_CTRL_VOL_IN_W_MIDI:
        return MIXER_SRC_MIDI;
    default:
        DebugInt3();
        return 0;
    }
}
/******************************************************************************/
/******************************************************************************/
static BOOL mixerAddControl(MIXERLINEA *pDestLine, DWORD dwControl, MIXERLINEA *pSrcLine)
{
    MIXERCONTROLA *pctrl  = &mixerControls[nrControls];

    if(nrControls >= MAX_MIXER_CONTROLS) {
        dprintf(("ERROR: mixerAddControl: out of room!!!"));
        DebugInt3();
        return FALSE;
    }

    pctrl->cbStruct = sizeof(MIXERCONTROLA);
    memset(pctrl, 0, sizeof(MIXERCONTROLA));

    pctrl->cMultipleItems   = 0;
    pctrl->dwControlID      = nrControls;
    pctrl->fdwControl       = 0;
    switch(dwControl) {
    case MIX_CTRL_VOL_OUT_LINE:
    case MIX_CTRL_VOL_IN_L_MONO:
    case MIX_CTRL_VOL_IN_L_PHONE:
    case MIX_CTRL_VOL_IN_L_MIC:
    case MIX_CTRL_VOL_IN_L_LINE:
    case MIX_CTRL_VOL_IN_L_CD:
    case MIX_CTRL_VOL_IN_L_SPDIF:
    case MIX_CTRL_VOL_IN_L_VIDEO:
    case MIX_CTRL_VOL_IN_L_AUX:
    case MIX_CTRL_VOL_IN_L_PCM:
    case MIX_CTRL_VOL_IN_L_WAVETABLE:
    case MIX_CTRL_VOL_IN_L_MIDI:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        break;

    case MIX_CTRL_VOL_IN_W_MONO:
    case MIX_CTRL_VOL_IN_W_PHONE:
    case MIX_CTRL_VOL_IN_W_MIC:
    case MIX_CTRL_VOL_IN_W_LINE:
    case MIX_CTRL_VOL_IN_W_CD:
    case MIX_CTRL_VOL_IN_W_SPDIF:
    case MIX_CTRL_VOL_IN_W_VIDEO:
    case MIX_CTRL_VOL_IN_W_AUX:
    case MIX_CTRL_VOL_IN_W_PCM:
    case MIX_CTRL_VOL_IN_W_WAVETABLE:
    case MIX_CTRL_VOL_IN_W_MIDI:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        mixerDestInputs[MIXER_DEST_WAVEIN][nrWaveInInputs] = mixerSourceLineId[mixerWaveInVolToSource(dwControl)];
        nrWaveInInputs++;
        break;

    case MIX_CTRL_MUTE_OUT_LINE:
    case MIX_CTRL_MUTE_IN_L_MONO:
    case MIX_CTRL_MUTE_IN_L_PHONE:
    case MIX_CTRL_MUTE_IN_L_MIC:
    case MIX_CTRL_MUTE_IN_L_LINE:
    case MIX_CTRL_MUTE_IN_L_CD:
    case MIX_CTRL_MUTE_IN_L_SPDIF:
    case MIX_CTRL_MUTE_IN_L_VIDEO:
    case MIX_CTRL_MUTE_IN_L_AUX:
    case MIX_CTRL_MUTE_IN_L_PCM:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUTE;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 0;
        break;

    case MIX_CTRL_VOL_OUT_SPDIF:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        break;

    case MIX_CTRL_MUTE_OUT_SPDIF:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUTE;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 0;
        break;

    case MIX_CTRL_BOOST_IN_L_MIC:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_ONOFF;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 0;
        break;

    case MIX_CTRL_OUT_L_3DDEPTH:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_FADER;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        break;

    case MIX_CTRL_OUT_L_3DCENTER:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_FADER;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        break;

    case MIX_CTRL_MUX_IN_W_SRC:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_MULTIPLE | MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUX;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 1;
        break;

    case MIX_CTRL_OUT_L_TREBLE:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_TREBLE;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        break;

    case MIX_CTRL_OUT_L_BASS:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_BASS;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        break;

    default:
        DebugInt3();
        return FALSE;
    }
    //increase nr of controls of corresponding destinateion line
    pDestLine->cControls++;

    //add control to list of controls associated with source line
    for(int i=0;i<MAX_SOURCE_CONTROLS;i++) {
        if(mixerLineControls[pSrcLine->dwLineID][i] == -1) {
            mixerLineControls[pSrcLine->dwLineID][i] = pctrl->dwControlID;
            break;
        }
    }
    if(i == MAX_SOURCE_CONTROLS) {
        DebugInt3();
    }

    strncpy(pctrl->szShortName, szCtrlName[dwControl][0], sizeof(pctrl->szShortName));
    strncpy(pctrl->szName,      szCtrlName[dwControl][1], sizeof(pctrl->szName));
    nrControls++;
    dprintf(("Adding control %s (%s)", pctrl->szName, pctrl->szShortName));
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/

