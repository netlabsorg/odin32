/* $Id: winmmtype.h,v 1.4 2001-03-23 16:23:47 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINMMTYPE_H__
#define __WINMMTYPE_H__

typedef DWORD MMRESULT;

typedef struct
{
    WORD        wFormatTag;
    WORD        nChannels;
    DWORD       nSamplesPerSec;
    DWORD       nAvgBytesPerSec;
    WORD        nBlockAlign;
    WORD        wBitsPerSample;
    WORD        cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;

typedef struct wavehdr_tag {
    LPSTR       lpData;
    DWORD       dwBufferLength;
    DWORD       dwBytesRecorded;
    DWORD       dwUser;
    DWORD       dwFlags;
    DWORD       dwLoops;
    struct wavehdr_tag *lpNext;
    DWORD       reserved;
} WAVEHDR, *PWAVEHDR, *LPWAVEHDR;

#define HDRVR   DWORD

typedef void (* CALLBACK LPDRVCALLBACK)(HDRVR hdrvr, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

#define MMSYSERR_BASE          0

/* general error return values */
#define MMSYSERR_NOERROR      0
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10)
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11)
#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12)


#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13)
#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14)
#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15)
#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16)
#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17)
#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18)
#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19)
#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20)
#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 20)

#define WHDR_DONE       0x00000001
#define WHDR_PREPARED   0x00000002
#define WHDR_BEGINLOOP  0x00000004
#define WHDR_ENDLOOP    0x00000008
#define WHDR_INQUEUE    0x00000010


#define MM_WOM_OPEN         0x3BB
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#define MM_WIM_OPEN         0x3BE
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA

#define CALLBACK_TYPEMASK   	0x00070000l    	/* callback type mask */
#define CALLBACK_NULL       	0x00000000l    	/* no callback */
#define CALLBACK_WINDOW     	0x00010000l    	/* dwCallback is a HWND */
#define CALLBACK_TASK       	0x00020000l    	/* dwCallback is a HTASK */
#define CALLBACK_THREAD		(CALLBACK_TASK)	/* dwCallback is a thread ID */
#define CALLBACK_FUNCTION   	0x00030000l    	/* dwCallback is a FARPROC */
#define CALLBACK_EVENT		0x00050000l	/* dwCallback is an EVENT Handler */

#endif
