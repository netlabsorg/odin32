/* $Id: mixer.cpp,v 1.26 2002-07-12 08:12:29 sandervl Exp $ */

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

static MIXERCONTROLA *mixerAddControl(DWORD dwControl, MIXLINE *pSrcLine);
static MIXLINE       *mixerAddSource(MIXLINE *pDestLine, DWORD dwSource);
static MIXLINE       *mixerAddDestination(DWORD dwDest);

//array of destination mixer lines
static MIXLINE       mixerDest[MAX_MIXER_DESTINATIONS] = {0};
//array of source mixer lines
static MIXLINE       mixerSource[MAX_MIXER_SOURCES]    = {0};
//array of all mixer lines
static MIXLINE      *pmixerLines[MAX_MIXER_LINES]      = {NULL};
//array of all mixer controls                         
static MIXCONTROL    mixerControls[MAX_MIXER_CONTROLS] = {0};

static int           nrDestinations    = 0;
static int           nrSources         = 0;
static int           nrControls        = 0;
static int           nrLines           = 0;

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS lpmcd, DWORD fdwDetails)
{
    DWORD	lineID, controlType;
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;
   
    if((fdwDetails & 0xF0000000)== MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if((fdwDetails & 0xF0000000) == MIXER_OBJECTF_MIXER) {
        if(!HIWORD(hmxobj) && hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }

    if(lpmcd == NULL || lpmcd->cbStruct != sizeof(MIXERCONTROLDETAILS)) {
        dprintf(("ERROR: invalid pointer or structure size %d (%d)", (lpmcd) ? lpmcd->cbStruct : 0, sizeof(MIXERCONTROLDETAILS)));
        return MMSYSERR_INVALPARAM;
    }
    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
    	dprintf(("MIXER_GETCONTROLDETAILSF_VALUE %d (internal id %d)", lpmcd->dwControlID, mixerControls[lpmcd->dwControlID].id));
        if(lpmcd->dwControlID >= nrControls) {
            dprintf(("invalid control %d", lpmcd->dwControlID));
            return MIXERR_INVALCONTROL;
        }
        if(lpmcd->cbDetails < sizeof(MIXERCONTROLDETAILS_UNSIGNED)) {
            dprintf(("not enough room in buffer (%d)", lpmcd->cbDetails));
            return MMSYSERR_INVALPARAM;
        }
        switch(mixerControls[lpmcd->dwControlID].ctrl.dwControlType) {
        case MIXERCONTROL_CONTROLTYPE_MUX:
        {
            MIXERCONTROLDETAILS_BOOLEAN *pDetails = (MIXERCONTROLDETAILS_BOOLEAN *)lpmcd->paDetails;
            DWORD dwRecSrc;

            dprintf(("MIXERCONTROL_CONTROLTYPE_MUX"));
            if(lpmcd->cChannels != 1) {//only accepts 1 in win2k
                dprintf(("invalid number of channels %d", lpmcd->cChannels));
                return MMSYSERR_INVALPARAM;
            }
            //this is also checked in win2k
            if(lpmcd->u.cMultipleItems != mixerControls[lpmcd->dwControlID].ctrl.cMultipleItems) {
                dprintf(("invalid cMultipleItems %d", lpmcd->u.cMultipleItems));
                return MMSYSERR_INVALPARAM;
            }

            if(mixerControls[lpmcd->dwControlID].id != MIX_CTRL_MUX_IN_W_SRC) {
                dprintf(("oh, oh. not wavein mux"));
                DebugInt3();
                return MMSYSERR_INVALPARAM;
            }

            if(OSLibMixGetRecSource(&dwRecSrc) == FALSE) {
                dprintf(("OSLibMixGetRecSource failed!!"));
                return MIXERR_INVALCONTROL;
            }
            //clear the array
            memset(pDetails, 0, sizeof(MIXERCONTROLDETAILS_BOOLEAN)*lpmcd->u.cMultipleItems);
            //mark recording source
            for(int i=0;i<nrDestinations;i++) {
                if(mixerDest[i].id == MIXER_DEST_WAVEIN) {
                    for(int j=0;j<mixerDest[i].cConnections;j++) {
                        dprintf(("wavein source %s %d (id %d)", pmixerLines[mixerDest[i].Connections[j]]->line.szName, pmixerLines[mixerDest[i].Connections[j]]->id, mixerDest[i].Connections[j]));
                        if(pmixerLines[mixerDest[i].Connections[j]]->id == dwRecSrc) {
                            pDetails[j].fValue = 1;
                            return MMSYSERR_NOERROR;
                        }
                    }
                }
            }
            dprintf(("recording source %d not found!!", dwRecSrc));
            break;
        }

        case MIXERCONTROL_CONTROLTYPE_VOLUME: //unsigned
        {
            MIXERCONTROLDETAILS_UNSIGNED *pDetails = (MIXERCONTROLDETAILS_UNSIGNED *)lpmcd->paDetails;
            DWORD dwVolLeft, dwVolRight;

            dprintf(("MIXERCONTROL_CONTROLTYPE_VOLUME"));
            if(OSLibMixGetVolume(mixerControls[lpmcd->dwControlID].id, &dwVolLeft, &dwVolRight) == FALSE) {
                dprintf(("OSLibMixGetVolume failed!!"));
                return MIXERR_INVALCONTROL;
            }
            pDetails->dwValue = dwVolLeft;
            dprintf(("%s Left volume %d", mixerControls[lpmcd->dwControlID].ctrl.szName, dwVolLeft));
            if(lpmcd->cChannels == 2) {
                dprintf(("%s Right volume %d", mixerControls[lpmcd->dwControlID].ctrl.szName, dwVolRight));
                pDetails += 1;
                pDetails->dwValue = dwVolRight;
            }
            //todo > 2 channels
            return MMSYSERR_NOERROR;
        }

        case MIXERCONTROL_CONTROLTYPE_MUTE: //assuming boolean
        {
            MIXERCONTROLDETAILS_BOOLEAN *pDetails = (MIXERCONTROLDETAILS_BOOLEAN *)lpmcd->paDetails;
            BOOL  fMute;

            dprintf(("MIXERCONTROL_CONTROLTYPE_MUTE"));
            if(OSLibMixGetMute(mixerControls[lpmcd->dwControlID].id, &fMute) == FALSE) {
                dprintf(("OSLibMixGetVolume failed!!"));
                return MIXERR_INVALCONTROL;
            }
            pDetails->fValue = fMute;
            dprintf(("%s Left mute %d", mixerControls[lpmcd->dwControlID].ctrl.szName, fMute));
            if(lpmcd->cChannels == 2) {
                dprintf(("%s Right mute %d", mixerControls[lpmcd->dwControlID].ctrl.szName, fMute));
                pDetails += 1;
                pDetails->fValue = fMute;
            }
            //todo > 2 channels (usually only 1 channel is requested though)
            return MMSYSERR_NOERROR;
        }

        case MIXERCONTROL_CONTROLTYPE_FADER:
        {
            MIXERCONTROLDETAILS_UNSIGNED *pDetails = (MIXERCONTROLDETAILS_UNSIGNED *)lpmcd->paDetails;
            DWORD dwLevelL;

            dprintf(("MIXERCONTROL_CONTROLTYPE_FADER"));

            if(OSLibMixGetVolume(mixerControls[lpmcd->dwControlID].id, &dwLevelL, NULL) == FALSE) {
                dprintf(("OSLibMixGetVolume failed!!"));
                return MIXERR_INVALCONTROL;
            }
            pDetails->dwValue = dwLevelL;
#ifdef DEBUG
            if(mixerControls[lpmcd->dwControlID].id == MIX_CTRL_OUT_L_3DCENTER) {
                 dprintf(("3D center %d", dwLevelL));
            }
            else dprintf(("3D depth %d", dwLevelL));
#endif
            if(lpmcd->cChannels == 2) {
                pDetails += 1;
                pDetails->dwValue = dwLevelL;
            }
            //todo > 2 channels (??)
            return MMSYSERR_NOERROR;
        }

        case MIXERCONTROL_CONTROLTYPE_BASS:
        case MIXERCONTROL_CONTROLTYPE_TREBLE:
        {
            MIXERCONTROLDETAILS_UNSIGNED *pDetails = (MIXERCONTROLDETAILS_UNSIGNED *)lpmcd->paDetails;
            DWORD dwLevelL;

#ifdef DEBUG
            switch(fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
            case MIXERCONTROL_CONTROLTYPE_TREBLE:
                dprintf(("MIXERCONTROL_CONTROLTYPE_TREBLE"));
                break;
            case MIXERCONTROL_CONTROLTYPE_BASS:
                dprintf(("MIXERCONTROL_CONTROLTYPE_BASS"));
                break;
            }
#endif

            if(OSLibMixGetVolume(mixerControls[lpmcd->dwControlID].id, &dwLevelL, NULL) == FALSE) {
                dprintf(("OSLibMixGetVolume failed!!"));
                return MIXERR_INVALCONTROL;
            }
            pDetails->dwValue = dwLevelL;
            dprintf(("%s %d", mixerControls[lpmcd->dwControlID].ctrl.szName, dwLevelL));
            if(lpmcd->cChannels == 2) {
                pDetails += 1;
                pDetails->dwValue = dwLevelL;
            }
            return MMSYSERR_NOERROR;
        }
        
#ifdef DEBUG
        case MIXERCONTROL_CONTROLTYPE_MIXER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MIXER"));
            break;
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
        case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_EQUALIZER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_SINGLESELECT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MICROTIME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MICROTIME"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MILLITIME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MILLITIME"));
            break;
#endif
        default:
            DebugInt3();
            return MIXERR_INVALCONTROL;
        }
        return MIXERR_INVALCONTROL;

    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
    	dprintf(("MIXER_GETCONTROLDETAILSF_LISTTEXT %d", lpmcd->dwControlID));
        if(lpmcd->dwControlID >= nrControls) {
            dprintf(("invalid control %d", lpmcd->dwControlID));
            return MIXERR_INVALCONTROL;
        }
        switch(mixerControls[lpmcd->dwControlID].ctrl.dwControlType) {
        case MIXERCONTROL_CONTROLTYPE_MUX:
        {
            MIXERCONTROLDETAILS_LISTTEXTA *pDetails = (MIXERCONTROLDETAILS_LISTTEXTA *)lpmcd->paDetails;
            if(lpmcd->cChannels != 1) {//only accepts 1 in win2k
                dprintf(("invalid number of channels %d", lpmcd->cChannels));
                return MMSYSERR_INVALPARAM;
            }
            //this is also checked in win2k
            if(lpmcd->u.cMultipleItems != mixerControls[lpmcd->dwControlID].ctrl.cMultipleItems) {
                dprintf(("invalid cMultipleItems %d", lpmcd->u.cMultipleItems));
                return MMSYSERR_INVALPARAM;
            }
            //fails otherwise in win2k
            if(lpmcd->cbDetails != sizeof(MIXERCONTROLDETAILS_LISTTEXTA)) {
                dprintf(("invalid buffer size %d; should be %d", lpmcd->cbDetails, sizeof(MIXERCONTROLDETAILS_LISTTEXTA)));
                return MMSYSERR_INVALPARAM;
            }
            if(mixerControls[lpmcd->dwControlID].id != MIX_CTRL_MUX_IN_W_SRC) {
                dprintf(("oh, oh. not wavein mux"));
                DebugInt3();
                return MMSYSERR_INVALPARAM;
            }
            //clear the array
            memset(pDetails, 0, sizeof(MIXERCONTROLDETAILS_LISTTEXTA)*lpmcd->u.cMultipleItems);
            //set mux source details
            for(int i=0;i<nrDestinations;i++) {
                if(mixerDest[i].id == MIXER_DEST_WAVEIN) {
                    for(int j=0;j<mixerDest[i].cConnections;j++) {
                        dprintf(("wavein source %s %d (%s)", pmixerLines[mixerDest[i].Connections[j]]->line.szName, pmixerLines[mixerDest[i].Connections[j]]->id, pmixerLines[mixerDest[i].Connections[j]]->line.szName));
                        pDetails->dwParam1 = pmixerLines[mixerDest[i].Connections[j]]->line.dwLineID;
                        pDetails->dwParam2 = 0;
                        strncpy(pDetails->szName, pmixerLines[mixerDest[i].Connections[j]]->line.szName, sizeof(pDetails->szName));
                        pDetails++;
                    }
                    return MMSYSERR_NOERROR;
                }
            }
            DebugInt3();
            return MMSYSERR_INVALPARAM;
        }
        case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
        case MIXERCONTROL_CONTROLTYPE_MIXER:
        case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
        case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
            dprintf(("Only implemented for mux controls"));
            return MIXERR_INVALCONTROL;
            
        default:
            dprintf(("Only implemented for multiple item controls"));
            return MIXERR_INVALCONTROL;
        }

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
        MIXERCONTROLDETAILS_LISTTEXTW *pDetailsW = (MIXERCONTROLDETAILS_LISTTEXTW *)lpmcd->paDetails;
        MIXERCONTROLDETAILS_LISTTEXTA *pDetailsA;
	    int size = max(1, lpmcd->cChannels) * sizeof(MIXERCONTROLDETAILS_LISTTEXTA);
        int i;

	    if (lpmcd->u.cMultipleItems != 0) {
		    size *= lpmcd->u.cMultipleItems;
	    }
	    pDetailsA = (MIXERCONTROLDETAILS_LISTTEXTA *)HeapAlloc(GetProcessHeap(), 0, size);
        lpmcd->paDetails = pDetailsA;
        lpmcd->cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXTA);
	    /* set up lpmcd->paDetails */
	    ret = mixerGetControlDetailsA(hmxobj, lpmcd, fdwDetails);
	    /* copy from lpmcd->paDetails back to paDetailsW; */
        if(ret == MMSYSERR_NOERROR) {
            for(i=0;i<lpmcd->u.cMultipleItems*lpmcd->cChannels;i++) {
                pDetailsW->dwParam1 = pDetailsA->dwParam1;
                pDetailsW->dwParam2 = pDetailsA->dwParam2;
                MultiByteToWideChar( CP_ACP, 0, pDetailsA->szName, -1,
                                     pDetailsW->szName,
                                     sizeof(pDetailsW->szName)/sizeof(WCHAR) );
                pDetailsA++;
                pDetailsW++;
            }
            pDetailsA -= lpmcd->u.cMultipleItems*lpmcd->cChannels;
            pDetailsW -= lpmcd->u.cMultipleItems*lpmcd->cChannels;
        }
	    HeapFree(GetProcessHeap(), 0, pDetailsA);
	    lpmcd->paDetails = pDetailsW;
        lpmcd->cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXTW);
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
     
    if((fdwDetails & 0xF0000000) == MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if((fdwDetails & 0xF0000000) == MIXER_OBJECTF_MIXER) {
        if(!HIWORD(hmxobj) && hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }
    if(lpmcd == NULL || lpmcd->cbStruct != sizeof(MIXERCONTROLDETAILS)) {
        dprintf(("ERROR: invalid pointer or structure size %x %d", lpmcd, lpmcd->cbStruct));
        return MMSYSERR_INVALPARAM;
    }
    
    switch (fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
    case MIXER_GETCONTROLDETAILSF_VALUE:
    	dprintf(("MIXER_GETCONTROLDETAILSF_VALUE %d (internal id %d)", lpmcd->dwControlID, mixerControls[lpmcd->dwControlID].id));
        if(lpmcd->dwControlID >= nrControls) {
            dprintf(("invalid control %d", lpmcd->dwControlID));
            return MIXERR_INVALCONTROL;
        }
        if(lpmcd->cbDetails < sizeof(MIXERCONTROLDETAILS_UNSIGNED)) {
            dprintf(("not enough room in buffer (%d)", lpmcd->cbDetails));
            return MMSYSERR_INVALPARAM;
        }
        switch(mixerControls[lpmcd->dwControlID].ctrl.dwControlType) {
        case MIXERCONTROL_CONTROLTYPE_MUX:
        {
            MIXERCONTROLDETAILS_BOOLEAN *pDetails = (MIXERCONTROLDETAILS_BOOLEAN *)lpmcd->paDetails;
            DWORD dwRecSrc;

            dprintf(("MIXERCONTROL_CONTROLTYPE_MUX"));
            if(lpmcd->cChannels != 1) {//only accepts 1 in win2k
                dprintf(("invalid number of channels %d", lpmcd->cChannels));
                return MMSYSERR_INVALPARAM;
            }
            //this is also checked in win2k
            if(lpmcd->u.cMultipleItems != mixerControls[lpmcd->dwControlID].ctrl.cMultipleItems) {
                dprintf(("invalid cMultipleItems %d", lpmcd->u.cMultipleItems));
                return MMSYSERR_INVALPARAM;
            }

            if(mixerControls[lpmcd->dwControlID].id != MIX_CTRL_MUX_IN_W_SRC) {
                dprintf(("oh, oh. not wavein mux"));
                DebugInt3();
                return MMSYSERR_INVALPARAM;
            }
            //select recording source
            for(int i=0;i<nrDestinations;i++) {
                if(mixerDest[i].id == MIXER_DEST_WAVEIN) 
                {
                    for(int j=0;j<mixerDest[i].cConnections;j++) {
                        dprintf(("wavein source %s %d (id %d)", pmixerLines[mixerDest[i].Connections[j]]->line.szName, pmixerLines[mixerDest[i].Connections[j]]->id, mixerDest[i].Connections[j]));
                        if(pDetails[j].fValue) 
                        {
                            if(OSLibMixSetRecSource(pmixerLines[mixerDest[i].Connections[j]]->id) == FALSE) {
                                dprintf(("OSLibMixGetRecSource failed!!"));
                                return MIXERR_INVALCONTROL;
                            }
                            goto success;
                        }
                    }
                }
            }
            dprintf(("recording source %d not found!!", dwRecSrc));
            break;
        }

        case MIXERCONTROL_CONTROLTYPE_VOLUME: //unsigned
        {
            MIXERCONTROLDETAILS_UNSIGNED *pDetails = (MIXERCONTROLDETAILS_UNSIGNED *)lpmcd->paDetails;
            DWORD dwVolumeL, dwVolumeR;

            dprintf(("MIXERCONTROL_CONTROLTYPE_VOLUME"));
            dwVolumeL = pDetails->dwValue;

            dprintf(("%s Left volume %d", mixerControls[lpmcd->dwControlID].ctrl.szName, pDetails->dwValue));
            if(lpmcd->cChannels == 2) {
                pDetails += 1;
                dprintf(("%s Right volume %d", mixerControls[lpmcd->dwControlID].ctrl.szName, pDetails->dwValue));
                dwVolumeR = pDetails->dwValue;
            }
            else dwVolumeR = dwVolumeL;
            //todo > 2 channels

            if(OSLibMixSetVolume(mixerControls[lpmcd->dwControlID].id, dwVolumeL, dwVolumeR) == FALSE) {
                return MMSYSERR_INVALPARAM;
            }
            mixerControls[lpmcd->dwControlID].val[0].dwValue = dwVolumeL;
            mixerControls[lpmcd->dwControlID].val[1].dwValue = dwVolumeR;
            goto success;
        }

        case MIXERCONTROL_CONTROLTYPE_MUTE: //assuming boolean
        {
            MIXERCONTROLDETAILS_BOOLEAN *pDetails = (MIXERCONTROLDETAILS_BOOLEAN *)lpmcd->paDetails;
            BOOL fMute;

            dprintf(("MIXERCONTROL_CONTROLTYPE_MUTE"));
            dprintf(("%s Left mute %d", mixerControls[lpmcd->dwControlID].ctrl.szName, pDetails->fValue));
            fMute = pDetails->fValue;
            if(lpmcd->cChannels == 2) {
                pDetails += 1;
                dprintf(("%s Right mute %d", mixerControls[lpmcd->dwControlID].ctrl.szName, pDetails->fValue));
            }
            //todo > 2 channels (usually only 1 channel is requested though)

            if(OSLibMixSetMute(mixerControls[lpmcd->dwControlID].id, fMute) == FALSE) {
                return MMSYSERR_INVALPARAM;
            }
            mixerControls[lpmcd->dwControlID].val[0].dwValue = fMute;
            goto success;
        }

        case MIXERCONTROL_CONTROLTYPE_BASS:
        case MIXERCONTROL_CONTROLTYPE_TREBLE:
        case MIXERCONTROL_CONTROLTYPE_FADER:
        {
            MIXERCONTROLDETAILS_UNSIGNED *pDetails = (MIXERCONTROLDETAILS_UNSIGNED *)lpmcd->paDetails;
            DWORD dwLevel;

#ifdef DEBUG
            switch(fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK) {
            case MIXERCONTROL_CONTROLTYPE_TREBLE:
                dprintf(("MIXERCONTROL_CONTROLTYPE_TREBLE"));
                break;
            case MIXERCONTROL_CONTROLTYPE_BASS:
                dprintf(("MIXERCONTROL_CONTROLTYPE_BASS"));
                break;
            case MIXERCONTROL_CONTROLTYPE_FADER:
                dprintf(("MIXERCONTROL_CONTROLTYPE_FADER"));
                break;
            }
#endif
            dprintf(("%s %d", mixerControls[lpmcd->dwControlID].ctrl.szName, pDetails->dwValue));
            dwLevel = pDetails->dwValue;
            if(OSLibMixSetVolume(mixerControls[lpmcd->dwControlID].id, dwLevel, dwLevel) == FALSE) {
                return MMSYSERR_INVALPARAM;
            }
            mixerControls[lpmcd->dwControlID].val[0].dwValue = dwLevel;
            goto success;
        }
        
#ifdef DEBUG
        case MIXERCONTROL_CONTROLTYPE_MIXER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MIXER"));
            break;
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
        case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
            dprintf(("MIXERCONTROL_CONTROLTYPE_EQUALIZER"));
            break;
        case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_SINGLESELECT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MICROTIME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MICROTIME"));
            break;
        case MIXERCONTROL_CONTROLTYPE_MILLITIME:
            dprintf(("MIXERCONTROL_CONTROLTYPE_MILLITIME"));
            break;
#endif
        default:
            DebugInt3();
            return MIXERR_INVALCONTROL;
        }
        return MIXERR_INVALCONTROL;
    case MIXER_GETCONTROLDETAILSF_LISTTEXT:
    	dprintf(("MIXER_GETCONTROLDETAILSF_LISTTEXT"));
    	break;
    default:
	    dprintf(("Unknown flag (%08lx)\n", fdwDetails & MIXER_GETCONTROLDETAILSF_QUERYMASK));
        break;
    }
    return MMSYSERR_NOTSUPPORTED;

success:
    if(pMixInfo->dwFlags & CALLBACK_WINDOW && pMixInfo->dwCallback) {
        dprintf(("Notify window %x of control change", pMixInfo->dwCallback));
        PostMessageA((HWND)pMixInfo->dwCallback, MM_MIXM_CONTROL_CHANGE, (WPARAM)hmxobj, (LPARAM)lpmcd->dwControlID);
    }
    return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA lpMlc, DWORD fdwControls)
{
    DWORD dwRet = MMSYSERR_NOERROR;
    DWORD lineID, controlType;
    DEVICE_STRUCT *pMixInfo = (DEVICE_STRUCT *)hmxobj;

    if((fdwControls & 0xF0000000) == MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if((fdwControls & 0xF0000000) == MIXER_OBJECTF_MIXER) {
        if(!HIWORD(hmxobj) && hmxobj > 0) {
            return MMSYSERR_NODRIVER;
        }
    }

    if (lpMlc == NULL) return MMSYSERR_INVALPARAM;

    if (lpMlc->cbStruct < sizeof(*lpMlc) || lpMlc->cbmxctrl < sizeof(MIXERCONTROLA))
	    return MMSYSERR_INVALPARAM;

    switch(fdwControls & MIXER_GETLINECONTROLSF_QUERYMASK) 
    {
    case MIXER_GETLINECONTROLSF_ALL:
    {
        MIXERLINEA    *pLine;
        MIXERCONTROLA *pCtrl;

        dprintf(("MIXER_GETLINECONTROLSF_ALL for line %d", lpMlc->dwLineID));
        if(lpMlc->dwLineID >= nrLines) {
            dprintf(("ERROR: Invalid line %d", lpMlc->dwLineID));
            return MIXERR_INVALLINE;
        }
        pLine = &pmixerLines[lpMlc->dwLineID]->line;
        if(lpMlc->cControls != pLine->cControls) {
            dprintf(("ERROR: invalid nr of controls %d or structure size %d (%d) (ptr %x)", lpMlc->cControls, lpMlc->cbmxctrl, pLine->cControls*sizeof(MIXERCONTROLA), lpMlc->pamxctrl));
            return MMSYSERR_INVALPARAM;
        }
        for(int i=0;i<lpMlc->cControls;i++) {
            pCtrl = lpMlc->pamxctrl+i;
            memcpy(pCtrl, &mixerControls[pmixerLines[lpMlc->dwLineID]->Controls[i]].ctrl, sizeof(MIXERCONTROLA));
            dprintf(("found control %s (%s) control id %d", pCtrl->szName, pCtrl->szShortName, pCtrl->dwControlID));
        }
	    break;
    }

    case MIXER_GETLINECONTROLSF_ONEBYID:
    {
        dprintf(("MIXER_GETLINECONTROLSF_ONEBYID %x", lpMlc->u.dwControlID));
        if(lpMlc->cControls != 1 || lpMlc->cbmxctrl != sizeof(MIXERCONTROLA)) {
            dprintf(("invalid parameters"));
            return MMSYSERR_INVALPARAM;
        }
        if(lpMlc->u.dwControlID >= nrControls) {
            dprintf(("invalid control"));
            return MIXERR_INVALCONTROL;
        }
        //find line associated with this control
        lpMlc->dwLineID = -1;
        for(int i=0;i<nrLines;i++) {
            for(int j=0;j<pmixerLines[i]->cControls;j++) {
                if(pmixerLines[i]->Controls[j] == lpMlc->u.dwControlID) {
                    lpMlc->dwLineID = i;
                    break;
                }
                else 
                if(pmixerLines[i]->Controls[j] == -1) {
                    break;
                }
            }
            if(lpMlc->dwLineID != -1) {
                break;
            }
        }
        if(i == MAX_MIXER_LINES) {
            dprintf(("Associated line for control %d NOT FOUND", lpMlc->u.dwControlID));
            return MIXERR_INVALCONTROL;
        }
        memcpy(lpMlc->pamxctrl, &mixerControls[lpMlc->u.dwControlID].ctrl, sizeof(MIXERCONTROLA));

        dprintf(("found control %s (%s) associated line %d", lpMlc->pamxctrl->szName, lpMlc->pamxctrl->szShortName, lpMlc->dwLineID));
	    break;
    }

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

        for(int i=0;i<pmixerLines[lpMlc->dwLineID]->cControls;i++) {
            idx = pmixerLines[lpMlc->dwLineID]->Controls[i];
            if(mixerControls[idx].ctrl.dwControlType == lpMlc->u.dwControlType) {
                memcpy(lpMlc->pamxctrl, &mixerControls[idx].ctrl, sizeof(MIXERCONTROLA));
                dprintf(("found control %s (%s) control id %d", lpMlc->pamxctrl->szName, lpMlc->pamxctrl->szShortName, lpMlc->pamxctrl->dwControlID));
                return MMSYSERR_NOERROR;
            }

        }
        return MIXERR_INVALLINE;    //returned by win2k when type not found
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

    if((fdwInfo & 0xF0000000) == MIXER_OBJECTF_HMIXER) {
        if(!pMixInfo) {
            return MMSYSERR_INVALHANDLE;
        }
    }
    else
    if((fdwInfo & 0xF0000000) == MIXER_OBJECTF_MIXER) {
        if(!HIWORD(hmxobj) && hmxobj > 0) {
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
        memcpy(lpMl, &mixerDest[lpMl->dwDestination].line, sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) id %d connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->dwLineID, lpMl->cConnections, lpMl->cControls));
    	break;

    case MIXER_GETLINEINFOF_LINEID:
        dprintf(("MIXER_GETLINEINFOF_LINEID %d", lpMl->dwLineID));
        if(lpMl->dwLineID >= nrLines) {
            dprintf(("ERROR: Invalid line %d", lpMl->dwLineID));
            return MIXERR_INVALLINE;
        }
        memcpy(lpMl, &pmixerLines[lpMl->dwLineID]->line, sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->cConnections, lpMl->cControls));
    	break;

    case MIXER_GETLINEINFOF_SOURCE:
        dprintf(("MIXER_GETLINEINFOF_SOURCE %d %d", lpMl->dwDestination, lpMl->dwSource));
        if(lpMl->dwDestination >= nrDestinations) {
            dprintf(("ERROR: Invalid destination %d", lpMl->dwDestination));
            return MIXERR_INVALLINE;
        }
        if(lpMl->dwSource >= MIXER_SRC_MAX) {
            dprintf(("ERROR: Invalid source %d", lpMl->dwSource));
            return MIXERR_INVALLINE;
        }
        if(mixerDest[lpMl->dwDestination].Connections[lpMl->dwSource] == -1) {
            dprintf(("ERROR: Invalid destination/source combo (%d,%d)", lpMl->dwDestination, lpMl->dwSource));
            return MIXERR_INVALLINE;
        }
        memcpy(lpMl, &pmixerLines[mixerDest[lpMl->dwDestination].Connections[lpMl->dwSource]]->line, sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) id %d connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->dwLineID, lpMl->cConnections, lpMl->cControls));
    	break;

    case MIXER_GETLINEINFOF_COMPONENTTYPE:
    {
        dprintf(("MIXER_GETLINEINFOF_COMPONENTTYPE"));
#ifdef DEBUG
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
        case MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_DIGITAL:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_DIGITAL"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_LINE:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_LINE"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY"));
            break;
        case MIXERLINE_COMPONENTTYPE_SRC_ANALOG:
            dprintf(("MIXERLINE_COMPONENTTYPE_SRC_ANALOG"));
            break;
    	default:
	    	dprintf(("Unhandled component type (%08lx)\n", lpMl->dwComponentType));
	    	return MMSYSERR_INVALPARAM;
	    }
#endif
        //search all lines (src & dest) for one with specified type
        for(int i=0;i<nrLines;i++) {
            if(pmixerLines[i]->line.dwComponentType == lpMl->dwComponentType) {
                break;
            }
        }
        if(i == nrLines) {
            dprintf(("Component type %d not found!!", lpMl->dwComponentType));
            return MIXERR_INVALLINE;
        }
        memcpy(lpMl, &pmixerLines[i]->line, sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) id %d connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->dwLineID, lpMl->cConnections, lpMl->cControls));
	    break;
    }
    case MIXER_GETLINEINFOF_TARGETTYPE:
    {
	    dprintf(("MIXER_GETLINEINFOF_TARGETTYPE %x", lpMl->Target.dwType));
#ifdef DEBUG
        switch(lpMl->Target.dwType) {
        case MIXERLINE_TARGETTYPE_UNDEFINED:
            dprintf(("MIXERLINE_TARGETTYPE_UNDEFINED"));
            break;
        case MIXERLINE_TARGETTYPE_WAVEOUT:
            dprintf(("MIXERLINE_TARGETTYPE_WAVEOUT"));
            break;
        case MIXERLINE_TARGETTYPE_WAVEIN:
            dprintf(("MIXERLINE_TARGETTYPE_WAVEIN"));
            break;
        case MIXERLINE_TARGETTYPE_MIDIOUT:
            dprintf(("MIXERLINE_TARGETTYPE_MIDIOUT"));
            break;
        case MIXERLINE_TARGETTYPE_MIDIIN:
            dprintf(("MIXERLINE_TARGETTYPE_MIDIIN"));
            break;
        case MIXERLINE_TARGETTYPE_AUX:
            dprintf(("MIXERLINE_TARGETTYPE_AUX"));
            break;
        default:        
	    	dprintf(("Unhandled target type (%08lx)\n", lpMl->Target.dwType));
	    	return MMSYSERR_INVALPARAM;
        }
#endif
        //search all lines (src & dest) for one with specified type
        //TODO: Should we compare mid, pid & pddname too? (must be initialized according to MSDN)
        //      (see below; unicode version doesn't copy pddname; change if these checks are required)
        for(int i=0;i<nrLines;i++) {
            if(pmixerLines[i]->line.Target.dwType == lpMl->Target.dwType) {
                break;
            }
        }
        if(i == nrLines) {
            dprintf(("Component type %d not found!!", lpMl->Target.dwType));
            return MIXERR_INVALLINE;
        }
        memcpy(lpMl, &pmixerLines[i]->line, sizeof(MIXERLINEA));
        dprintf(("found line %s (%s) id %d connections %d controls %d", lpMl->szName, lpMl->szShortName, lpMl->dwLineID, lpMl->cConnections, lpMl->cControls));
        break;
    }
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
    line.cControls             = pmxl->cControls;
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
    //TODO: need to copy this?? (MIXER_GETLINEINFOF_TARGETTYPE)
    line.Target.szPname[0]     = 0;

    result = mixerGetLineInfoA(hmxobj, &line, fdwInfo);
    if(result != MMSYSERR_NOERROR) {
        return result;
    }
    pmxl->cbStruct              = sizeof(MIXERLINEA);
    pmxl->cChannels             = line.cChannels;
    pmxl->cConnections          = line.cConnections;
    pmxl->cControls             = line.cControls;
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
    dprintf(("WINMM:mixerMessage - NOT IMPLEMENTED" ));
    return MMSYSERR_NOTSUPPORTED;
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
        pmxcaps->wMid = WINMM_MIXER_CAPS_WMID;                  /* manufacturer ID */
        pmxcaps->wPid = WINMM_MIXER_CAPS_WPID;                  /* product ID */
        pmxcaps->vDriverVersion = WINMM_MIXER_CAPS_VERSION;     /* version of the driver */
        strncpy( pmxcaps->szPname, WINMM_MIXERSTRING_A, sizeof(pmxcaps->szPname)); /* product name */

        pmxcaps->fdwSupport    = 0;    //no mixer flags exist
        pmxcaps->cDestinations = nrDestinations;
        dprintf(("mixerGetDevCapsA: cDestinations %d", pmxcaps->cDestinations));
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
        pmxcaps->wMid = WINMM_MIXER_CAPS_WMID;                  /* manufacturer ID */
        pmxcaps->wPid = WINMM_MIXER_CAPS_WPID;                  /* product ID */
        pmxcaps->vDriverVersion = WINMM_MIXER_CAPS_VERSION;     /* version of the driver */
        lstrcpyW( pmxcaps->szPname, WINMM_MIXERSTRING_W );      /* product name */

        pmxcaps->fdwSupport    = 0;    //no mixer flags exist
        pmxcaps->cDestinations = nrDestinations;
        dprintf(("mixerGetDevCapsW: cDestinations %d", pmxcaps->cDestinations));
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
    MIXLINE *pDestLine;
    MIXERCONTROLA *pWaveInMux;

    if(!fMMPMAvailable) return TRUE;

    memset(mixerDest, 0, sizeof(mixerDest));
    for(int i=0;i<MIXER_DEST_MAX;i++) {
        memset(mixerDest[i].Connections, -1, sizeof(mixerDest[i].Connections));
        memset(mixerDest[i].Controls, -1, sizeof(mixerDest[i].Controls));
    }
    memset(mixerSource, 0, sizeof(mixerSource));
    for(i=0;i<MIXER_SRC_MAX;i++) {
        memset(mixerSource[i].Connections, -1, sizeof(mixerSource[i].Connections));
        memset(mixerSource[i].Controls, -1, sizeof(mixerSource[i].Controls));
        mixerSource[i].id = -1;
    }
    if(OSLibMixerOpen() == FALSE) {
        //Line out destination
        pDestLine = mixerAddDestination(MIXER_DEST_LINEOUT);
        mixerAddControl(MIX_CTRL_VOL_OUT_LINE, pDestLine);
        mixerAddControl(MIX_CTRL_MUTE_OUT_LINE, pDestLine);

        //WaveIn destination
        pDestLine  = mixerAddDestination(MIXER_DEST_WAVEIN);
        pWaveInMux = mixerAddControl(MIX_CTRL_MUX_IN_W_SRC, pDestLine);
        mixerAddControl(MIX_CTRL_VOL_IN_W_MIC, pDestLine);
        mixerAddControl(MIX_CTRL_VOL_IN_W_LINE, pDestLine);
        pWaveInMux->cMultipleItems = pDestLine->line.cConnections;
        return TRUE;
    }

    //Line out destination
    pDestLine = mixerAddDestination(MIXER_DEST_LINEOUT);

    mixerAddControl(MIX_CTRL_VOL_OUT_LINE, pDestLine);
    mixerAddControl(MIX_CTRL_MUTE_OUT_LINE, pDestLine);
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_3DCENTER)) {
        mixerAddControl(MIX_CTRL_OUT_L_3DCENTER, pDestLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_3DDEPTH)) {
        mixerAddControl(MIX_CTRL_OUT_L_3DDEPTH, pDestLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_TREBLE)) {
        mixerAddControl(MIX_CTRL_OUT_L_TREBLE, pDestLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_OUT_L_BASS)) {
        mixerAddControl(MIX_CTRL_OUT_L_BASS, pDestLine);
    }

    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_PCM)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_PCM);
        mixerAddControl(MIX_CTRL_VOL_IN_L_PCM, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_PCM, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_WAVETABLE)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_WAVETABLE);
        mixerAddControl(MIX_CTRL_VOL_IN_L_WAVETABLE, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_WAVETABLE, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MIDI)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_MIDI);
        mixerAddControl(MIX_CTRL_VOL_IN_L_MIDI, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_MIDI, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MIC)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_MIC);
        mixerAddControl(MIX_CTRL_VOL_IN_L_MIC, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_MIC, pLine);
        if(OSLibMixIsControlPresent(MIX_CTRL_BOOST_IN_L_MIC)) {
            mixerAddControl(MIX_CTRL_BOOST_IN_L_MIC, pLine);
        }
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_LINE)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_LINE);
        mixerAddControl(MIX_CTRL_VOL_IN_L_LINE, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_LINE, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_CD)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_CD);
        mixerAddControl(MIX_CTRL_VOL_IN_L_CD, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_CD, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_AUX)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_AUX);
        mixerAddControl(MIX_CTRL_VOL_IN_L_AUX, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_AUX, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_SPDIF)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_SPDIF);
        mixerAddControl(MIX_CTRL_VOL_IN_L_SPDIF, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_SPDIF, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_MONO)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_MONOIN);
        mixerAddControl(MIX_CTRL_VOL_IN_L_MONO, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_MONO, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_PHONE)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_PHONE);
        mixerAddControl(MIX_CTRL_VOL_IN_L_PHONE, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_PHONE, pLine);
    }
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_IN_L_VIDEO)) {
        MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_L_VIDEO);
        mixerAddControl(MIX_CTRL_VOL_IN_L_VIDEO, pLine);
        mixerAddControl(MIX_CTRL_MUTE_IN_L_VIDEO, pLine);
    }

    //Wave In Destination
    if(OSLibMixIsControlPresent(MIX_CTRL_MUX_IN_W_SRC)) {
        pDestLine = mixerAddDestination(MIXER_DEST_WAVEIN);
        pWaveInMux = mixerAddControl(MIX_CTRL_MUX_IN_W_SRC, pDestLine);

        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_MIC)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_MIC);
            mixerAddControl(MIX_CTRL_VOL_IN_W_MIC, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_LINE)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_LINE);
            mixerAddControl(MIX_CTRL_VOL_IN_W_LINE, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_CD)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_CD);
            mixerAddControl(MIX_CTRL_VOL_IN_W_CD, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_AUX)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_AUX);
            mixerAddControl(MIX_CTRL_VOL_IN_W_AUX, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_PCM)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_PCM);
            mixerAddControl(MIX_CTRL_VOL_IN_W_PCM, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_WAVETABLE)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_WAVETABLE);
            mixerAddControl(MIX_CTRL_VOL_IN_W_WAVETABLE, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_MIDI)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_MIDI);
            mixerAddControl(MIX_CTRL_VOL_IN_W_MIDI, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_MONOIN)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_MONOIN);
            mixerAddControl(MIX_CTRL_VOL_IN_W_MONO, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_PHONE)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_PHONE);
            mixerAddControl(MIX_CTRL_VOL_IN_W_PHONE, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_SPDIF)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_SPDIF);
            mixerAddControl(MIX_CTRL_VOL_IN_W_SPDIF, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_VIDEO)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_VIDEO);
            mixerAddControl(MIX_CTRL_VOL_IN_W_VIDEO, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_STEREOMIX)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_STEREOMIX);
            mixerAddControl(MIX_CTRL_VOL_IN_W_STEREOMIX, pLine);
        }
        if(OSLibMixIsRecSourcePresent(MIXER_SRC_IN_W_MONOMIX)) {
            MIXLINE *pLine = mixerAddSource(pDestLine, MIXER_SRC_IN_W_MONOMIX);
            mixerAddControl(MIX_CTRL_VOL_IN_W_MONOMIX, pLine);
        }
        pWaveInMux->cMultipleItems    = pDestLine->line.cConnections;
        pWaveInMux->Bounds.s.lMinimum = 0;
        pWaveInMux->Bounds.s.lMaximum = pDestLine->line.cConnections;
        pWaveInMux->Metrics.cSteps    = pDestLine->line.cConnections;
    }

    //SPDIF destination
    if(OSLibMixIsControlPresent(MIX_CTRL_VOL_OUT_SPDIF)) {
        pDestLine = mixerAddDestination(MIXER_DEST_SPDIFOUT);
        mixerAddControl(MIX_CTRL_VOL_OUT_SPDIF, pDestLine);
        mixerAddControl(MIX_CTRL_MUTE_OUT_SPDIF, pDestLine);
    }

    return TRUE;
}
/******************************************************************************/
/******************************************************************************/
void mixerExit()
{ 
    if(fMMPMAvailable == FALSE) return;

    OSLibMixerClose();
}
/******************************************************************************/
/******************************************************************************/
static MIXLINE *mixerAddSource(MIXLINE *pDestLine, DWORD dwSource)
{
    MIXLINE    *pSource = &mixerSource[nrSources]; 
    MIXERLINEA *pline   = &pSource->line;

    if(nrSources >= MAX_MIXER_SOURCES) {
        dprintf(("ERROR: mixerAddSource: out of room!!!"));
        DebugInt3();
        return NULL;
    }

    memset(pline, 0, sizeof(MIXERLINEA));

    pline->cbStruct              = sizeof(MIXERLINEA);
    pline->cConnections          = 0;
    pline->cControls             = 0;
    pline->dwDestination         = 0;
    pline->fdwLine               = MIXERLINE_LINEF_SOURCE;
    pline->Target.dwDeviceID     = 0;
    pline->Target.wMid           = WINMM_MIXER_CAPS_WMID;
    pline->Target.vDriverVersion = WINMM_MIXER_CAPS_VERSION;
    pline->dwSource              = nrSources;
    OSLibMixGetLineCaps(dwSource, &pline->cChannels);
    nrSources++;

    switch(dwSource) {
    case MIXER_SRC_IN_L_MONOIN:
    case MIXER_SRC_IN_W_MONOIN:
    case MIXER_SRC_IN_L_PHONE:
    case MIXER_SRC_IN_W_PHONE:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_IN_L_MIC:
    case MIXER_SRC_IN_W_MIC:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_IN_L_LINE:
    case MIXER_SRC_IN_W_LINE:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_LINE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_IN_L_CD:
    case MIXER_SRC_IN_W_CD:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_IN_L_SPDIF:
    case MIXER_SRC_IN_W_SPDIF:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_DIGITAL;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_IN_L_VIDEO:
    case MIXER_SRC_IN_W_VIDEO:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_SRC_IN_L_AUX:
    case MIXER_SRC_IN_W_AUX:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_AUX;
        break;

    case MIXER_SRC_IN_L_PCM:
    case MIXER_SRC_IN_W_PCM:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_WAVEOUT;
        break;

    case MIXER_SRC_IN_L_WAVETABLE:
    case MIXER_SRC_IN_W_WAVETABLE:
    case MIXER_SRC_IN_L_MIDI:
    case MIXER_SRC_IN_W_MIDI:
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_MIDIOUT;
        break;

    default:
        DebugInt3();
        return NULL;
    }
    strncpy(pline->szShortName, szSourceName[dwSource][0], sizeof(pline->szShortName));
    strncpy(pline->szName,      szSourceName[dwSource][1], sizeof(pline->szName));
    strncpy(pline->Target.szPname, WINMM_MIXERSTRING_A, sizeof(pline->Target.szPname));

    //add to array of mixer line pointers
    pline->dwLineID      = nrLines;
    pmixerLines[nrLines] = pSource;
    nrLines++;

    //store line id in source to line mapping array
    pSource->id = dwSource;

    //increase nr of inputs at destination line
    pDestLine->line.cConnections++;
    pDestLine->Connections[pDestLine->cConnections] = pline->dwLineID;
    pDestLine->cConnections++;

    dprintf(("Adding Source %s (%s) with destination %s (%d -> %d)", pline->szName, pline->szShortName, pDestLine->line.szName, dwSource, pline->dwLineID));

    return pSource;
}
/******************************************************************************/
/******************************************************************************/
static MIXLINE *mixerAddDestination(DWORD dwDest)
{
    MIXLINE    *pDest  = &mixerDest[nrDestinations];
    MIXERLINEA *pline  = &pDest->line;

    if(nrDestinations >= MAX_MIXER_DESTINATIONS) {
        dprintf(("ERROR: mixerAddDestination: out of room!!!"));
        DebugInt3();
        return NULL;
    }
    memset(pline, 0, sizeof(MIXERLINEA));
    pline->cbStruct        = sizeof(MIXERLINEA);
    pline->cConnections    = 0;
    pline->cControls       = 0;
    pline->dwSource        = (DWORD)-1;
    pline->dwDestination   = nrDestinations;

    switch(dwDest) {
    case MIXER_DEST_LINEOUT:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
        pline->fdwLine         = MIXERLINE_LINEF_ACTIVE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_DEST_SPDIFOUT:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_DIGITAL;
        pline->fdwLine         = MIXERLINE_LINEF_ACTIVE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_UNDEFINED;
        break;

    case MIXER_DEST_WAVEIN:
        pline->cChannels       = 2;
        pline->dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        pline->fdwLine         = MIXERLINE_LINEF_ACTIVE;
        pline->Target.dwType   = MIXERLINE_TARGETTYPE_WAVEIN;
        break;
        
    default:
        DebugInt3();
        return NULL;
    }
    strncpy(pline->szShortName, szDestName[dwDest][0], sizeof(pline->szShortName));
    strncpy(pline->szName,      szDestName[dwDest][1], sizeof(pline->szName));
    strncpy(pline->Target.szPname, WINMM_MIXERSTRING_A, sizeof(pline->Target.szPname));
    pline->Target.dwDeviceID     = 0;
    pline->Target.wMid           = WINMM_MIXER_CAPS_WMID;
    pline->Target.wPid           = WINMM_MIXER_CAPS_WPID;
    pline->Target.vDriverVersion = WINMM_MIXER_CAPS_VERSION;
    
    dprintf(("Adding destination %s (%s) connections %d controls %d", pline->szName, pline->szShortName, pline->cConnections, pline->cControls));
    nrDestinations++;

    pline->dwLineID      = nrLines;
    pmixerLines[nrLines] = pDest;
    nrLines++;

    //save internal id
    pDest->id = dwDest;
    return pDest;
}
/******************************************************************************/
/******************************************************************************/
static MIXERCONTROLA * mixerAddControl(DWORD dwControl, MIXLINE *pSrcLine)
{
    MIXCONTROL    *pmixctrl = &mixerControls[nrControls];
    MIXERCONTROLA *pctrl    = &pmixctrl->ctrl;

    if(nrControls >= MAX_MIXER_CONTROLS) {
        dprintf(("ERROR: mixerAddControl: out of room!!!"));
        DebugInt3();
        return NULL;
    }

    memset(pctrl, 0, sizeof(MIXERCONTROLA));
    pctrl->cbStruct = sizeof(MIXERCONTROLA);
    pctrl->cMultipleItems   = 0;
    pctrl->dwControlID      = nrControls;
    pctrl->fdwControl       = 0;
    switch(dwControl) {
    case MIX_CTRL_MUX_IN_W_SRC:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_MULTIPLE | MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_MUX;
        pctrl->Bounds.s.lMinimum = 0;
        pctrl->Bounds.s.lMaximum = 1;
        pctrl->Metrics.cSteps    = 1;
        pctrl->cMultipleItems    = 0;   //to be filled in later (see caller)
        break;

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
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
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
    case MIX_CTRL_VOL_IN_W_STEREOMIX:
    case MIX_CTRL_VOL_IN_W_MONOMIX:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
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
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
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
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
        break;

    case MIX_CTRL_OUT_L_3DCENTER:
        pctrl->fdwControl       = MIXERCONTROL_CONTROLF_UNIFORM;
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_FADER;
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
        break;

    case MIX_CTRL_OUT_L_TREBLE:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_TREBLE;
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
        break;

    case MIX_CTRL_OUT_L_BASS:
        pctrl->dwControlType    = MIXERCONTROL_CONTROLTYPE_BASS;
        OSLibMixGetCtrlCaps(dwControl, &pctrl->Bounds.s.lMinimum, &pctrl->Bounds.s.lMaximum, &pctrl->Metrics.cSteps);
        break;

    default:
        DebugInt3();
        return FALSE;
    }
 
    //add control to list of controls associated with source line
    pSrcLine->Controls[pSrcLine->cControls] = pctrl->dwControlID;
    pSrcLine->line.cControls++;
    pSrcLine->cControls++;

    strncpy(pctrl->szShortName, szCtrlName[dwControl][0], sizeof(pctrl->szShortName));
    strncpy(pctrl->szName,      szCtrlName[dwControl][1], sizeof(pctrl->szName));

    //save internal id
    pmixctrl->id = dwControl;
    nrControls++;
    dprintf(("Adding control %s (%s)", pctrl->szName, pctrl->szShortName));
    return pctrl;
}
/******************************************************************************/
/******************************************************************************/

