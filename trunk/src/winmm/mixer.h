/* $Id: mixer.h,v 1.1 2002-05-22 20:19:19 sandervl Exp $ */

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

#define WINMM_MIXERSTRING_A    		"OS/2 WINMM Mixer"
#define WINMM_MIXERSTRING_W		(LPWSTR)L"OS/2 WINMM Mixer"

#define WINMM_SHORTNAME_WAVEOUT_A	"Wave Out"
#define WINMM_SHORTNAME_WAVEOUT_W	(LPWSTR)L"Wave Out"
#define WINMM_SHORTNAME_LINEOUT_A	"Line Out"
#define WINMM_SHORTNAME_LINEOUT_W	(LPWSTR)L"Line Out"

#define MAX_MIXER_DESTINATIONS      8
#define MAX_MIXER_SOURCES           16
#define MAX_MIXER_CONTROLS          64


#ifdef __cplusplus
extern "C" {
#endif

BOOL mixerInit();
void mixerExit();

#ifdef __cplusplus
}
#endif

#endif //__WINMM_MIXER_H__

