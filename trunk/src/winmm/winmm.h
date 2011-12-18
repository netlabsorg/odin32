/* $Id: winmm.h,v 1.4 2000-04-02 14:51:10 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINMM_H__
#define __WINMM_H__

const char * getWinmmMsg( MMRESULT result );

typedef struct {
       UINT			uDeviceID;
       UINT			type;
       UINT			mmdIndex;		/* index to low-level driver in MMDrvs table */
       DWORD			dwDriverInstance;	/* this value is driver related, as opposed to
							 * opendesc.dwInstance which is client (callback) related */
       DWORD			dwFlags;
       DWORD			dwCallback;
       DWORD			dwClientInstance;
} DEVICE_STRUCT;

#define WINMM_WAVEOUT	0
#define WINMM_WAVEIN	1
#define WINMM_MIDIOUT	2
#define WINMM_MIDIIN	3
#define WINMM_AUX	4
#define WINMM_MIXER	5

#endif
