/* $Id: mixer.cpp,v 1.13 2002-05-23 13:50:15 sandervl Exp $ */

/*
 * Mixer functions
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * 
 * TODO: Mixer notification
 * 
 * NOTE: Not really flexible (capabilities, > 1 audio card)
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

static BOOL mixerAddControl(DWORD dwControl);
static BOOL mixerAddSource(DWORD dwDest);
static BOOL mixerAddDestination(DWORD dwSource);

static MIXERLINEA    mixerDest[MAX_MIXER_DESTINATIONS] = {0};
static MIXERLINEA    mixerSource[MAX_MIXER_SOURCES]    = {0};
static MIXERCONTROLA mixerControls[MAX_MIXER_CONTROLS] = {0};
static int           nrDestinations                    = 0;
static int           nrSources                         = 0;
static int           nrControls                        = 0;
static int           nrLineOutInputs                   = 0;
static int           nrLineOutControls                 = 0;
static int           nrWaveInInputs                    = 0;
static int           nrWaveInControls                  = 0;
static int           nrSPDIFInputs                     = 0;
static int           nrSPDIFControls                   = 0;

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
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
    return MMSYSERR_NOERROR;
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
MMRESULT WINAPI mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA lpMlc, DWORD fdwControls)
{
    DWORD dwRet = MMSYSERR_NOERROR;
    DWORD lineID, controlType;
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if(!pMixInfo) {
        return MMSYSERR_INVALHANDLE;
    }

    if (lpMlc == NULL) return MMSYSERR_INVALPARAM;

    if (lpMlc->cbStruct < sizeof(*lpMlc) || lpMlc->cbmxctrl < sizeof(MIXERCONTROLA))
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
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if(!pMixInfo) {
        return MMSYSERR_INVALHANDLE;
    }

    dprintf(("WINMM:mixerGetGetLineControlsW - stub\n" ));
    return MIXERR_INVALLINE;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA lpMl, DWORD fdwInfo)
{
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if(!pMixInfo) {
        return MMSYSERR_INVALHANDLE;
    }

    if (lpMl == NULL || lpMl->cbStruct != sizeof(*lpMl)) {
        dprintf(("ERROR: mixerGetLineInfoA: invalid paramter!!"));
	    return MMSYSERR_INVALPARAM;
    }
    
    switch (fdwInfo & MIXER_GETLINEINFOF_QUERYMASK) 
    {
    case MIXER_GETLINEINFOF_DESTINATION:
        dprintf(("MIXER_GETLINEINFOF_DESTINATION"));
        if(lpMl->dwDestination >= nrDestinations) {
            dprintf(("ERROR: Invalid desitnation %d", lpMl->dwDestination));
            return MMSYSERR_INVALPARAM;
        }
        memcpy(lpMl, &mixerDest[lpMl->dwDestination], sizeof(MIXERLINEA));
    	break;

    case MIXER_GETLINEINFOF_LINEID:
        dprintf(("MIXER_GETLINEINFOF_LINEID"));
    	break;

    case MIXER_GETLINEINFOF_SOURCE:
        dprintf(("MIXER_GETLINEINFOF_SOURCE"));
        if(lpMl->dwDestination >= nrDestinations) {
            dprintf(("ERROR: Invalid desitnation %d", lpMl->dwDestination));
            return MMSYSERR_INVALPARAM;
        }
    	break;

    case MIXER_GETLINEINFOF_COMPONENTTYPE:
        dprintf(("MIXER_GETLINEINFOF_COMPONENTTYPE"));
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
	    break;

    case MIXER_GETLINEINFOF_TARGETTYPE:
	    dprintf(("_TARGETTYPE not implemented yet.\n"));
	    break;
    default:
	    dprintf(("Unknown flag (%08lx)\n", fdwInfo & MIXER_GETLINEINFOF_QUERYMASK));
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

    if(!pMixInfo) {
        return MMSYSERR_INVALHANDLE;
    }
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
    if(!fMMPMAvailable) return TRUE;

    if(OSLibMixerOpen() == FALSE) {
        //Line out destination
        mixerAddControl(MIX_CTRL_VOL_OUT_LINE);
        mixerAddControl(MIX_CTRL_MUTE_OUT_LINE);
        //muast add after all controls (for counting controls & inputs)
        mixerAddDestination(MIXER_DEST_LINEOUT);

        //WaveIn destination
        mixerAddControl(MIX_CTRL_VOL_IN_W_MIC);
        mixerAddControl(MIX_CTRL_VOL_IN_W_LINE);
        //must add after all recording inputs (to count them)
        mixerAddControl(MIX_CTRL_MUX_IN_W_SRC);
        //muast add after all wave in controls (for counting controls & inputs)
        mixerAddDestination(MIXER_DEST_WAVEIN);
        return TRUE;
    }
    //Line out destination
    mixerAddControl(MIX_CTRL_VOL_OUT_LINE);
    mixerAddControl(MIX_CTRL_MUTE_OUT_LINE);

    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_SPDIF)) {
        mixerAddSource(MIXER_SRC_SPDIF);
        mixerAddControl(MIX_CTRL_VOL_IN_L_SPDIF);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_SPDIF);
    }

    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MONO)) {
        mixerAddSource(MIXER_SRC_MONOIN);
        mixerAddControl(MIX_CTRL_VOL_IN_L_MONO);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_MONO);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_PHONE)) {
        mixerAddSource(MIXER_SRC_PHONE);
        mixerAddControl(MIX_CTRL_VOL_IN_L_PHONE);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_PHONE);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MIC)) {
        mixerAddSource(MIXER_SRC_MIC);
        mixerAddControl(MIX_CTRL_VOL_IN_L_MIC);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_MIC);
        if(OSLibMixIsControlPresent(MIX_CTRL_BOOST_IN_L_MIC)) {
            mixerAddControl(MIX_CTRL_BOOST_IN_L_MIC);
        }
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_LINE)) {
        mixerAddSource(MIXER_SRC_LINE);
        mixerAddControl(MIX_CTRL_VOL_IN_L_LINE);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_LINE);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_CD)) {
        mixerAddSource(MIXER_SRC_CD);
        mixerAddControl(MIX_CTRL_VOL_IN_L_CD);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_CD);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_VIDEO)) {
        mixerAddSource(MIXER_SRC_VIDEO);
        mixerAddControl(MIX_CTRL_VOL_IN_L_VIDEO);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_VIDEO);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_AUX)) {
        mixerAddSource(MIXER_SRC_AUX);
        mixerAddControl(MIX_CTRL_VOL_IN_L_AUX);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_AUX);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_PCM)) {
        mixerAddSource(MIXER_SRC_PCM);
        mixerAddControl(MIX_CTRL_VOL_IN_L_PCM);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_PCM);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_WAVETABLE)) {
        mixerAddSource(MIXER_SRC_WAVETABLE);
        mixerAddControl(MIX_CTRL_VOL_IN_L_WAVETABLE);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_WAVETABLE);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MIDI)) {
        mixerAddSource(MIXER_SRC_MIDI);
        mixerAddControl(MIX_CTRL_VOL_IN_L_MIDI);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_MIDI);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_3DCENTER)) {
        mixerAddControl(MIX_CTRL_OUT_L_3DCENTER);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_3DDEPTH)) {
        mixerAddControl(MIX_CTRL_OUT_L_3DDEPTH);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_TREBLE)) {
        mixerAddControl(MIX_CTRL_OUT_L_TREBLE);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_BASS)) {
        mixerAddControl(MIX_CTRL_OUT_L_BASS);
    }
    //muast add after all controls (for counting controls & inputs)
    mixerAddDestination(MIXER_DEST_LINEOUT);

    //Wave In Destination
    if(OSLibMixIsControlPresent(MIX_CTRL_MUX_IN_W_SRC)) {
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_MONO)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_MONO);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_PHONE)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_PHONE);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_MIC)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_MIC);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_LINE)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_LINE);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_CD)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_CD);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_SPDIF)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_SPDIF);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_VIDEO)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_VIDEO);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_AUX)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_AUX);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_PCM)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_PCM);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_WAVETABLE)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_WAVETABLE);
        }
        if(OSLibMixIsRecSourcePresent(MIX_CTRL_VOL_IN_W_MIDI)) {
            mixerAddControl(MIX_CTRL_VOL_IN_W_MIDI);
        }
        //must add after all recording inputs (to count them)
        mixerAddControl(MIX_CTRL_MUX_IN_W_SRC);
        //muast add after all wave in controls (for counting controls & inputs)
        mixerAddDestination(MIXER_DEST_WAVEIN);
    }

    //SPDIF destination
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_OUT_SPDIF)) {
        mixerAddControl(MIX_CTRL_VOL_OUT_SPDIF);
        mixerAddControl(MIX_CTRL_MUTE_OUT_SPDIF);
        mixerAddDestination(MIXER_DEST_SPDIFOUT);
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
static BOOL mixerAddSource(DWORD dwSource)
{
    MIXERLINEA *pline  = &mixerSource[nrSources];

    if(nrSources >= MAX_MIXER_SOURCES) {
        dprintf(("ERROR: mixerAddSource: out of room!!!"));
        DebugInt3();
        return FALSE;
    }

    pline->cbStruct = sizeof(MIXERLINEA);
    memset(pline, 0, sizeof(MIXERLINEA));

    switch(dwSource) {
    case MIXER_SRC_MONOIN:
    case MIXER_SRC_PHONE:
        pline->dwSource        = nrSources;
        pline->cChannels       = 1;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_MIC:
        pline->dwSource        = nrSources;
        pline->cChannels       = 1;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_LINE:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_LINE;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_CD:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_SPDIF:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_DIGITAL;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_VIDEO:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_AUX:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_AUX;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_PCM:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_WAVEOUT;
        nrLineOutInputs++;
        break;

    case MIXER_SRC_WAVETABLE:
    case MIXER_SRC_MIDI:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = 0;
        pline->cControls       = 0;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_MIDIOUT;
        nrLineOutInputs++;
        break;

    default:
        DebugInt3();
        return FALSE;
    }
    strncpy(pline->szShortName, szSourceName[dwSource][0], sizeof(pline->szShortName));
    strncpy(pline->szName,      szSourceName[dwSource][1], sizeof(pline->szName));

    dprintf(("Adding Source %s (%s)", pline->szName, pline->szShortName));

    nrSources++;
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
static BOOL mixerAddDestination(DWORD dwDest)
{
    MIXERLINEA *pline  = &mixerDest[nrDestinations];

    if(nrDestinations >= MAX_MIXER_DESTINATIONS) {
        dprintf(("ERROR: mixerAddDestination: out of room!!!"));
        DebugInt3();
        return FALSE;
    }
    pline->cbStruct = sizeof(MIXERLINEA);
    memset(pline, 0, sizeof(MIXERLINEA));

    switch(dwDest) {
    case MIXER_DEST_LINEOUT:
        pline->dwDestination   = nrDestinations;
        pline->cChannels       = 2;
        pline->cConnections    = nrLineOutInputs;
        pline->cControls       = nrLineOutControls;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
        pline->dwSource        = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_ACTIVE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_DEST_SPDIFOUT:
        pline->dwDestination   = nrDestinations;
        pline->cChannels       = 2;
        pline->cConnections    = nrSPDIFInputs;
        pline->cControls       = nrSPDIFControls;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_DIGITAL;
        pline->dwSource        = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = 0;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_DEST_WAVEIN:
        pline->dwSource        = nrSources;
        pline->cChannels       = 2;
        pline->cConnections    = nrWaveInInputs;
        pline->cControls       = nrWaveInControls;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        pline->dwDestination   = 0;
        pline->dwLineID        = (DWORD)-1;
        pline->fdwLine         = MIXERLINE_LINEF_SOURCE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;
        
    default:
        DebugInt3();
        return FALSE;
    }
    strncpy(pline->szShortName, szDestName[dwDest][0], sizeof(pline->szShortName));
    strncpy(pline->szName,      szDestName[dwDest][1], sizeof(pline->szName));
    dprintf(("Adding destination %s (%s)", pline->szName, pline->szShortName));
    nrDestinations++;
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
static BOOL mixerAddControl(DWORD dwControl)
{
    MIXERCONTROLA *pctrl  = &mixerControls[nrControls];

    if(nrControls >= MAX_MIXER_CONTROLS) {
        dprintf(("ERROR: mixerAddControl: out of room!!!"));
        DebugInt3();
        return FALSE;
    }

    pctrl->cbStruct = sizeof(MIXERCONTROLA);
    memset(pctrl, 0, sizeof(MIXERCONTROLA));

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
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = 0;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        strncpy(pctrl->szShortName, szCtrlName[dwControl][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[dwControl][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrLineOutControls++;
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
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = 0;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        strncpy(pctrl->szShortName, szCtrlName[dwControl][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[dwControl][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrWaveInControls++;
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
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUTE;
        strncpy(pctrl->szShortName, szCtrlName[dwControl][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[dwControl][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 0;
        nrControls++;
        nrLineOutControls++;
        break;

    case MIX_CTRL_VOL_OUT_SPDIF:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = 0;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_VOL_OUT_SPDIF][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_VOL_OUT_SPDIF][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrSPDIFControls++;
        break;

    case MIX_CTRL_MUTE_OUT_SPDIF:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUTE;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_MUTE_OUT_SPDIF][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_MUTE_OUT_SPDIF][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 0;
        nrControls++;
        nrSPDIFControls++;
        break;

    case MIX_CTRL_BOOST_IN_L_MIC:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_ONOFF;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_BOOST_IN_L_MIC][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_BOOST_IN_L_MIC][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 0;
        nrControls++;
        nrLineOutControls++;
        break;

    case MIX_CTRL_OUT_L_3DDEPTH:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_FADER;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_OUT_L_3DDEPTH][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_OUT_L_3DDEPTH][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrLineOutControls++;
        break;

    case MIX_CTRL_OUT_L_3DCENTER:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_FADER;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_OUT_L_3DCENTER][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_OUT_L_3DCENTER][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrLineOutControls++;
        break;

    case MIX_CTRL_MUX_IN_W_SRC:
        pctrl->cMultipleItems   = nrWaveInControls;
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_MULTIPLE | MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUX;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_MUX_IN_W_SRC][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_MUX_IN_W_SRC][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrWaveInControls++;
        break;

    case MIX_CTRL_OUT_L_TREBLE:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = 0;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_TREBLE;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_OUT_L_TREBLE][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_OUT_L_TREBLE][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrLineOutControls++;
        break;

    case MIX_CTRL_OUT_L_BASS:
        pctrl->cMultipleItems   = 0;
        pctrl->fdwControl       = 0;
        pctrl->dwControlID      = nrControls;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_BASS;
        strncpy(pctrl->szShortName, szCtrlName[MIX_CTRL_OUT_L_BASS][0], sizeof(pctrl->szShortName));
        strncpy(pctrl->szName,      szCtrlName[MIX_CTRL_OUT_L_BASS][1], sizeof(pctrl->szName));
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 100;
        pctrl->Metrics.cSteps    = 1;
        nrControls++;
        nrLineOutControls++;
        break;

    default:
        DebugInt3();
        return FALSE;
    }
    dprintf(("Adding control %s (%s)", pctrl->szName, pctrl->szShortName));
    return TRUE;
}
/******************************************************************************/
/******************************************************************************/

