/* $Id: mixeros2.h,v 1.1 2002-05-22 20:19:11 sandervl Exp $ */

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

#define MIXER_MAX_VOLUME        100

#define MIXER_CTRL_MONOIN       0
#define MIXER_CTRL_PHONE        1
#define MIXER_CTRL_MIC          2
#define MIXER_CTRL_LINE         3
#define MIXER_CTRL_CD           4
#define MIXER_CTRL_VIDEO        5
#define MIXER_CTRL_AUX          6
#define MIXER_CTRL_3D           7
#define MIXER_CTRL_STREAMVOL    8
#define MIXER_CTRL_RECORDSRC    9
#define MIXER_CTRL_RECORDGAIN   10
#define MIXER_CTRL_TREBLE       11
#define MIXER_CTRL_BASS         12

#define MIXER_RECSRC_MIC        0
#define MIXER_RECSRC_CD         1
#define MIXER_RECSRC_VIDEO      2
#define MIXER_RECSRC_AUX        3
#define MIXER_RECSRC_LINE       4
#define MIXER_RECSRC_RES5       5
#define MIXER_RECSRC_RES6       6
#define MIXER_RECSRC_PHONE      7

#ifdef __cplusplus
extern "C" {
#endif

BOOL OSLibMixerOpen();
void OSLibMixerClose();

BOOL OSLibMixIsControlPresent(DWORD dwControl);
BOOL OSLibMixSetVolume(DWORD dwControl, BOOL fMute, DWORD dwVolLeft, DWORD dwVolRight);
BOOL OSLibMixGetVolume(DWORD dwControl, BOOL *pfMute, DWORD *pdwVolLeft, DWORD *pdwVolRight);
BOOL OSLibMixSetRecSource(DWORD dwRecSrc);
BOOL OSLibMixGetRecSource(DWORD *pdwRecSrc);

#ifdef __cplusplus
}
#endif

#endif //__WINMM_OS2MIXER_H__
