/* $Id: flashaudio.h,v 1.00 2010-02-20 12:00:00 rlwalsh Exp $ */

/*
 * Query/Enable/Disable FlashWaveOut class in winmm\waveoutflash.cpp
 *
 * Copyright 2010 Richard L Walsh (rich@e-vertise.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

/******************************************************************************/

#ifndef __FLASHAUDIO_H__
#define __FLASHAUDIO_H__

/* This key goes in the [WINMM] section of 'odin.ini'.  Only the user
 * should add this key, and only when it is truly needed, because it
 * overrides the default setting & the effect of ODIN_EnableFlashAudio(). 
 * Valid values are "=1" or "=0".  When it isn't present, the default is
 * "=0", i.e. off, and the DartWaveOut class is used instead.
 */
#define KEY_FLASHAUDIO      "FlashAudio"

BOOL WIN32API ODIN_IsFlashAudioEnabled();
BOOL WIN32API ODIN_EnableFlashAudio(BOOL enable);

#endif
