/* $Id: mixeros2.h,v 1.5 2002-05-25 17:19:41 sandervl Exp $ */

/*
 * Mixer multimedia
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __WINMM_OS2MIXER_H__
#define __WINMM_OS2MIXER_H__

#define MIXER_IOCTL90_MAX_VOLUME      	100
#define MIXER_WIN32_MIN_VOLUME          0
#define MIXER_WIN32_MAX_VOLUME          65535
#define MIXER_WIN32_CSTEP_VOLUME        31

#define WIN32_TO_IOCTL90_VOLUME(a)      ((a*MIXER_IOCTL90_MAX_VOLUME)/(MIXER_WIN32_MAX_VOLUME-MIXER_WIN32_MIN_VOLUME))
#define IOCTL90_TO_WIN32_VOLUME(a)      ((a*(MIXER_WIN32_MAX_VOLUME-MIXER_WIN32_MIN_VOLUME))/MIXER_IOCTL90_MAX_VOLUME)

#define MIX_CTRL_VOL_OUT_LINE     	0
#define MIX_CTRL_MUTE_OUT_LINE     	1
#define MIX_CTRL_VOL_IN_L_MONO    	2
#define MIX_CTRL_MUTE_IN_L_MONO    	3
#define MIX_CTRL_VOL_IN_L_PHONE   	4
#define MIX_CTRL_MUTE_IN_L_PHONE   	5
#define MIX_CTRL_VOL_IN_L_MIC     	6
#define MIX_CTRL_MUTE_IN_L_MIC     	7
#define MIX_CTRL_BOOST_IN_L_MIC 	8
#define MIX_CTRL_VOL_IN_L_LINE    	9
#define MIX_CTRL_MUTE_IN_L_LINE    	10
#define MIX_CTRL_VOL_IN_L_CD      	11
#define MIX_CTRL_MUTE_IN_L_CD      	12
#define MIX_CTRL_VOL_IN_L_SPDIF   	13
#define MIX_CTRL_MUTE_IN_L_SPDIF   	14
#define MIX_CTRL_VOL_OUT_SPDIF    	15
#define MIX_CTRL_MUTE_OUT_SPDIF    	16
#define MIX_CTRL_VOL_IN_L_VIDEO   	17
#define MIX_CTRL_MUTE_IN_L_VIDEO   	18
#define MIX_CTRL_VOL_IN_L_AUX     	19
#define MIX_CTRL_MUTE_IN_L_AUX     	20
#define MIX_CTRL_VOL_IN_L_PCM     	21
#define MIX_CTRL_MUTE_IN_L_PCM     	22
#define MIX_CTRL_VOL_IN_L_WAVETABLE    	23
#define MIX_CTRL_MUTE_IN_L_WAVETABLE   	24
#define MIX_CTRL_VOL_IN_L_MIDI    	25
#define MIX_CTRL_MUTE_IN_L_MIDI   	26
#define MIX_CTRL_OUT_L_3DCENTER		27
#define MIX_CTRL_OUT_L_3DDEPTH 		28
#define MIX_CTRL_OUT_L_TREBLE       	29
#define MIX_CTRL_OUT_L_BASS    		30
#define MIX_CTRL_MUX_IN_W_SRC    	31
#define MIX_CTRL_VOL_IN_W_MONO     	32
#define MIX_CTRL_VOL_IN_W_PHONE   	33
#define MIX_CTRL_VOL_IN_W_MIC     	34
#define MIX_CTRL_VOL_IN_W_LINE    	35
#define MIX_CTRL_VOL_IN_W_CD   		36
#define MIX_CTRL_VOL_IN_W_SPDIF     	37
#define MIX_CTRL_VOL_IN_W_VIDEO     	38
#define MIX_CTRL_VOL_IN_W_AUX     	39
#define MIX_CTRL_VOL_IN_W_PCM     	40
#define MIX_CTRL_VOL_IN_W_WAVETABLE    	41
#define MIX_CTRL_VOL_IN_W_MIDI     	42
#define MIX_CTRL_MAX               	43

#define MIXER_SRC_MONOIN      	0
#define MIXER_SRC_PHONE       	1
#define MIXER_SRC_MIC     	2
#define MIXER_SRC_LINE     	3
#define MIXER_SRC_CD    	4
#define MIXER_SRC_SPDIF     	5
#define MIXER_SRC_VIDEO    	6
#define MIXER_SRC_AUX    	7
#define MIXER_SRC_PCM    	8
#define MIXER_SRC_WAVETABLE    	9
#define MIXER_SRC_MIDI		10
#define MIXER_SRC_MAX          	11

//max controls connected to a source line
#define MAX_SOURCE_CONTROLS     8

#define MIXER_DEST_LINEOUT      0
#define MIXER_DEST_WAVEIN       1
#define MIXER_DEST_SPDIFOUT     2
#define MIXER_DEST_MAX          3

#define MAX_MIXER_LINES         (MIXER_SRC_MAX+MIXER_DEST_MAX)

#ifdef __cplusplus
extern "C" {
#endif

BOOL OSLibMixerOpen();
void OSLibMixerClose();

BOOL OSLibMixIsControlPresent(DWORD dwControl);
BOOL OSLibMixSetVolume(DWORD dwControl, BOOL fMute, DWORD dwVolLeft, DWORD dwVolRight);
BOOL OSLibMixGetVolume(DWORD dwControl, BOOL *pfMute, DWORD *pdwVolLeft, DWORD *pdwVolRight);
BOOL OSLibMixSetRecSource(DWORD dwRecSrc, DWORD dwVolL, DWORD dwVolR);
BOOL OSLibMixGetRecSource(DWORD *pdwRecSrc, DWORD *pdwVolL, DWORD *pdwVolR);
BOOL OSLibMixIsRecSourcePresent(DWORD dwRecSrc);
BOOL OSLibMixGetCtrlCaps(DWORD dwControl, LONG *plMinimum, LONG *plMaximum, DWORD *pcSteps);

#ifdef __cplusplus
}
#endif

#endif //__WINMM_OS2MIXER_H__
