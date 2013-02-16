/* -*- tab-width: 8; c-basic-offset: 4 -*- */

/*****************************************************************************
 * Copyright 1998, Luiz Otavio L. Zorzella
 *           1999, Eric Pouech
 *           2000, Chris Wohlgemuth
 * File:      mcimm.h
 * Purpose:   multimedia declarations (internal to WINMM & MMSYSTEM DLLs)
 *
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif 

#include "mmddk.h"

typedef struct tagWINE_MLD {
/* EPP struct tagWINE_MLD*	lpNext; */		/* not used so far */
       UINT			uDeviceID;
       UINT			type;
       UINT			mmdIndex;		/* index to low-level driver in MMDrvs table */
       DWORD			dwDriverInstance;	/* this value is driver related, as opposed to
							 * opendesc.dwInstance which is client (callback) related */
       WORD			bFrom32;
       WORD			dwFlags;
       DWORD			dwCallback;
       DWORD			dwClientInstance;
} WINE_MLD, *LPWINE_MLD;

typedef struct  {
       WINE_MLD			mld;
} WINE_WAVE, *LPWINE_WAVE;

typedef struct {
       WINE_MLD			mld;
       MIDIOPENDESC		mod;			/* FIXME: should be removed */
} WINE_MIDI, *LPWINE_MIDI;

typedef struct {
       WINE_MLD			mld;
} WINE_MIXER, *LPWINE_MIXER;

extern	BOOL		MMDRV_Init(void);
extern	UINT		MMDRV_GetNum(UINT);
extern	LPWINE_MLD	MMDRV_Alloc(UINT size, UINT type, LPHANDLE hndl, DWORD* dwFlags, 
				    DWORD* dwCallback, DWORD* dwInstance, BOOL bFrom32);
extern	void		MMDRV_Free(HANDLE hndl, LPWINE_MLD mld);
extern	DWORD		MMDRV_Open(LPWINE_MLD mld, UINT wMsg, DWORD dwParam1, DWORD dwParam2);
extern	DWORD		MMDRV_Close(LPWINE_MLD mld, UINT wMsg);
extern	LPWINE_MLD	MMDRV_Get(HANDLE hndl, UINT type, BOOL bCanBeID);
extern  LPWINE_MLD	MMDRV_GetRelated(HANDLE hndl, UINT srcType, BOOL bSrcCanBeID, UINT dstTyped);
extern	DWORD		MMDRV_Message(LPWINE_MLD mld, WORD wMsg, DWORD dwParam1, DWORD dwParam2, BOOL bFrom32);
extern	UINT		MMDRV_PhysicalFeatures(LPWINE_MLD mld, UINT uMsg, DWORD dwParam1, DWORD dwParam2);

#define	MMDRV_AUX		0
#define	MMDRV_MIXER		1
#define	MMDRV_MIDIIN		2
#define	MMDRV_MIDIOUT		3
#define	MMDRV_WAVEIN		4
#define	MMDRV_WAVEOUT		5
#define MMDRV_MAX		6

#define WINE_MMTHREAD_CREATED	0x4153494C	/* "BSIL" */
#define WINE_MMTHREAD_DELETED	0xDEADDEAD

typedef struct {
       DWORD			dwSignature;		/* 00 "BSIL" when ok, 0xDEADDEAD when being deleted */
       DWORD			dwCounter;		/* 04 > 1 when in mmThread functions */
       HANDLE			hThread;		/* 08 hThread */
       DWORD                    dwThreadID;     	/* 0C */
       FARPROC16		fpThread;		/* 10 address of thread proc (segptr or lin depending on dwFlags) */
       DWORD			dwThreadPmt;    	/* 14 parameter to be passed upon thread creation to fpThread */
       DWORD                    dwSignalCount;	     	/* 18 counter used for signaling */
       HANDLE                   hEvent;     		/* 1C event */
       HANDLE                   hVxD;		     	/* 20 return from OpenVxDHandle */
       DWORD                    dwStatus;       	/* 24 0x00, 0x10, 0x20, 0x30 */
       DWORD			dwFlags;		/* 28 dwFlags upon creation */
       HANDLE16			hTask;          	/* 2C handle to created task */
} WINE_MMTHREAD;

typedef struct tagWINE_MCIDRIVER {
        UINT			wDeviceID;
        UINT			wType;
	LPSTR			lpstrElementName;
        LPSTR			lpstrDeviceType;
        LPSTR			lpstrAlias;
	HDRVR			hDrv;
	DRIVERPROC16		driverProc;
	DWORD			dwPrivate;
        YIELDPROC		lpfnYieldProc;
        DWORD	                dwYieldData;
        BOOL			bIs32;
        HTASK16			hCreatorTask;
        UINT			uTypeCmdTable;
        UINT			uSpecificCmdTable;
        struct tagWINE_MCIDRIVER*lpNext;
} WINE_MCIDRIVER, *LPWINE_MCIDRIVER;

#define WINE_TIMER_IS32	0x80

typedef struct tagWINE_TIMERENTRY {
    UINT			wDelay;
    UINT			wResol;
    FARPROC16 			lpFunc;
    DWORD			dwUser;
    UINT16			wFlags;
    UINT16			wTimerID;
    UINT			uCurTime;
    struct tagWINE_TIMERENTRY*	lpNext;
} WINE_TIMERENTRY, *LPWINE_TIMERENTRY;

typedef struct tagWINE_MMIO {
    MMIOINFO			info;
    struct IOProcList*		ioProc;
    BOOL			bTmpIOProc;
    HANDLE			hMem;
    SEGPTR			buffer16;
    struct tagWINE_MMIO*	lpNext;
    BOOL			bBufferLoaded;
} WINE_MMIO, *LPWINE_MMIO;

typedef struct tagWINE_MM_IDATA {
    /* iData reference */
    DWORD			dwThisProcess;
    struct tagWINE_MM_IDATA*	lpNextIData;
    /* winmm part */
    HANDLE			hWinMM32Instance;
    HANDLE			hWinMM16Instance;
    HANDLE			h16Module32;
    CRITICAL_SECTION		cs;
    /* mm timer part */
    HANDLE			hMMTimer;
    DWORD			mmSysTimeMS;
    LPWINE_TIMERENTRY 		lpTimerList;
    int				nSizeLpTimers;
    LPWINE_TIMERENTRY		lpTimers;
    /* mci part */
    LPWINE_MCIDRIVER 		lpMciDrvs;
    /* low level drivers (unused yet) */
    /* LPWINE_WAVE		lpWave; */
    /* LPWINE_MIDI		lpMidi; */
    /* LPWINE_MIXER		lpMixer; */
    /* mmio part */
    LPWINE_MMIO			lpMMIO;
} WINE_MM_IDATA, *LPWINE_MM_IDATA;

/* function prototypes */

typedef LONG			(*MCIPROC16)(DWORD, HDRVR16, WORD, DWORD, DWORD);
typedef LONG			(*MCIPROC)(DWORD, HDRVR, DWORD, DWORD, DWORD);


#ifdef __cplusplus
}
#endif 













