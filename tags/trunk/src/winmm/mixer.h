/* $Id: mixer.h,v 1.4 2002-05-26 10:52:31 sandervl Exp $ */

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


#ifdef __WINE_MMSYSTEM_H
typedef struct {
  MIXERLINEA line;
  //number of connected source lines
  int        cSources;
  int        Sources[MAX_MIXER_SOURCES];
} MIXERDEST;
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL mixerInit();
void mixerExit();

extern char *szCtrlName[MIX_CTRL_MAX][2];
extern char *szSourceName[MIXER_SRC_MAX][2];
extern char *szDestName[MIXER_DEST_MAX][2];

#ifdef __cplusplus
}
#endif

#endif //__WINMM_MIXER_H__

