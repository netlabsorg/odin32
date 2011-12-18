/* $Id: mixer.h,v 1.5 2002-05-27 15:49:55 sandervl Exp $ */

/*
 * Mixer multimedia
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __WINMM_MIXER_H__
#define __WINMM_MIXER_H__

#include "mixeros2.h"

#define WINMM_MIXERSTRING_A    		"OS/2 WINMM Mixer"
#define WINMM_MIXERSTRING_W		(LPWSTR)L"OS/2 WINMM Mixer"

#define WINMM_MIXER_CAPS_WMID           1
#define WINMM_MIXER_CAPS_WPID           0x60
#define WINMM_MIXER_CAPS_VERSION	0x100

#define MAX_MIXER_DESTINATIONS      8
#define MAX_MIXER_SOURCES           16
#define MAX_MIXER_CONTROLS          64
#define MAX_MIXER_CONTROL_VAL       16

#ifdef __WINE_MMSYSTEM_H
typedef struct {
    MIXERLINEA line;
    int        cControls;
    //array of associated control (index in mixerControls)
    int        Controls[MAX_SOURCE_CONTROLS];
    int        cConnections;
    //array of connected line (index in pmixerLines)
    int        Connections[MAX_MIXER_SOURCES];
    int        id;	//internal id
} MIXLINE, *PMIXLINE;

typedef struct {
    MIXERCONTROLA                 ctrl;
    //control value(s)
    MIXERCONTROLDETAILS_UNSIGNED  val[MAX_MIXER_CONTROL_VAL];
    int                           id;	//internal id
} MIXCONTROL, *PMIXCONTROL;

#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL mixerInit();
void mixerExit();

extern const char *szCtrlName[MIX_CTRL_MAX][2];
extern const char *szSourceName[MIXER_SRC_MAX][2];
extern const char *szDestName[MIXER_DEST_MAX][2];

#ifdef __cplusplus
}
#endif

#endif //__WINMM_MIXER_H__

