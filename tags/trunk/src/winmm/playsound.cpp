/* $Id: playsound.cpp,v 1.7 2001-03-24 13:23:48 sandervl Exp $ */

/*
 * Playsound implementation
 *
 * Wine port (winmm\mmsystem.c)
 *
 * Copyright 1993 Martin Ayotte
 *                Eric POUECH
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <odinwrap.h>
#include <mmsystem.h>
#include <heapstring.h>
#include <misc.h>
#include <string.h>
#include <win\debugtools.h>

#define DBG_LOCALLOG	DBG_playsound
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-PLAYSOUND)


static HANDLE		PlaySound_hThread = 0;
static HANDLE		PlaySound_hPlayEvent = 0;
static HANDLE		PlaySound_hReadyEvent = 0;
static HANDLE		PlaySound_hMiddleEvent = 0;
static BOOL		PlaySound_Result = FALSE;
static int		PlaySound_Stop = FALSE;
static int		PlaySound_Playing = FALSE;

static LPCSTR		PlaySound_pszSound = NULL;
static HMODULE		PlaySound_hmod = 0;
static DWORD		PlaySound_fdwSound = 0;
static int		PlaySound_Loop = FALSE;
static int		PlaySound_SearchMode = 0; /* 1 - sndPlaySound search order
						     2 - PlaySound order */

static HMMIO	get_mmioFromFile(LPCSTR lpszName)
{
    return mmioOpenA((LPSTR)lpszName, NULL,
		     MMIO_ALLOCBUF | MMIO_READ | MMIO_DENYWRITE);
}

static HMMIO	get_mmioFromProfile(UINT uFlags, LPCSTR lpszName) 
{
    char	str[128];
    LPSTR	ptr;
    HMMIO  	hmmio;
    
    TRACE("searching in SystemSound List !\n");
    GetProfileStringA("Sounds", (LPSTR)lpszName, "", str, sizeof(str));
    if (strlen(str) == 0) {
	if (uFlags & SND_NODEFAULT) return 0;
	GetProfileStringA("Sounds", "Default", "", str, sizeof(str));
	if (strlen(str) == 0) return 0;
    }
    if ((ptr = (LPSTR)strchr(str, ',')) != NULL) *ptr = '\0';
    hmmio = get_mmioFromFile(str);
    if (hmmio == 0) {
	WARN("can't find SystemSound='%s' !\n", str);
	return 0;
    }
    return hmmio;
}

struct playsound_data {
    HANDLE	hEvent;
    DWORD	dwEventCount;
};

static void CALLBACK PlaySound_Callback(HWAVEOUT hwo, UINT uMsg, 
					DWORD dwInstance,  
					DWORD dwParam1, DWORD dwParam2)
{
    struct playsound_data*	s = (struct playsound_data*)dwInstance;

    switch (uMsg) {
    case WOM_OPEN:
    case WOM_CLOSE:
	break;
    case WOM_DONE:
	InterlockedIncrement((LPLONG)&s->dwEventCount);
	TRACE("Returning waveHdr=%lx\n", dwParam1);
	SetEvent(s->hEvent);
	break;
    default:
	ERR("Unknown uMsg=%d\n", uMsg);
    }
}

static void PlaySound_WaitDone(struct playsound_data* s) 
{
    for (;;) {
	ResetEvent(s->hEvent);
	if (InterlockedDecrement((LPLONG)&s->dwEventCount) >= 0) {
	    break;
	}
	InterlockedIncrement((LPLONG)&s->dwEventCount);
	
	WaitForSingleObject(s->hEvent, INFINITE);
    }
}

static BOOL WINAPI proc_PlaySound(LPCSTR lpszSoundName, UINT uFlags)
{
    BOOL		bRet = FALSE;
    HMMIO		hmmio = 0;
    MMCKINFO		ckMainRIFF;
    MMCKINFO        	mmckInfo;
    LPWAVEFORMATEX      lpWaveFormat = NULL;
    HWAVE		hWave = 0;
    LPWAVEHDR		waveHdr = NULL;
    INT			count, bufsize, left, index;
    struct playsound_data	s;

    s.hEvent = 0;

    TRACE("SoundName='%s' uFlags=%04X !\n", lpszSoundName, uFlags);
    if (lpszSoundName == NULL) {
	TRACE("Stop !\n");
	return FALSE;
    }
    if (uFlags & SND_MEMORY) {
	MMIOINFO	mminfo;
	memset(&mminfo, 0, sizeof(mminfo));
	mminfo.fccIOProc = FOURCC_MEM;
	mminfo.pchBuffer = (LPSTR)lpszSoundName;
	mminfo.cchBuffer = -1;
	TRACE("Memory sound %p\n", lpszSoundName);
	hmmio = mmioOpenA(NULL, &mminfo, MMIO_READ);
    } else {
	hmmio = 0;
	if (uFlags & SND_ALIAS)
	    if ((hmmio = get_mmioFromProfile(uFlags, lpszSoundName)) == 0) 
		return FALSE;
	
	if (uFlags & SND_FILENAME)
	    if ((hmmio=get_mmioFromFile(lpszSoundName)) == 0) return FALSE;
	
	if (PlaySound_SearchMode == 1) {
	    PlaySound_SearchMode = 0;
	    if ((hmmio = get_mmioFromFile(lpszSoundName)) == 0) 
		hmmio = get_mmioFromProfile(uFlags, lpszSoundName);
	}
	
	if (PlaySound_SearchMode == 2) {
	    PlaySound_SearchMode = 0;
	    if ((hmmio = get_mmioFromProfile(uFlags | SND_NODEFAULT, lpszSoundName)) == 0) 
		if ((hmmio = get_mmioFromFile(lpszSoundName)) == 0)	
		    hmmio = get_mmioFromProfile(uFlags, lpszSoundName);
	}
    }
    if (hmmio == 0) return FALSE;

    if (mmioDescend(hmmio, &ckMainRIFF, NULL, 0))
	goto errCleanUp;

    TRACE("ParentChunk ckid=%.4s fccType=%.4s cksize=%08lX \n",
	  (LPSTR)&ckMainRIFF.ckid, (LPSTR)&ckMainRIFF.fccType, ckMainRIFF.cksize);

    if ((ckMainRIFF.ckid != FOURCC_RIFF) ||
	(ckMainRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
	goto errCleanUp;

    mmckInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &mmckInfo, &ckMainRIFF, MMIO_FINDCHUNK))
	goto errCleanUp;

    TRACE("Chunk Found ckid=%.4s fccType=%.4s cksize=%08lX \n",
	  (LPSTR)&mmckInfo.ckid, (LPSTR)&mmckInfo.fccType, mmckInfo.cksize);

    lpWaveFormat = (WAVEFORMATEX*)HeapAlloc(GetProcessHeap(), 0, mmckInfo.cksize);
    if (mmioRead(hmmio, (HPSTR)lpWaveFormat, mmckInfo.cksize) < sizeof(WAVEFORMAT))
	goto errCleanUp;

    TRACE("wFormatTag=%04X !\n", 	lpWaveFormat->wFormatTag);
    TRACE("nChannels=%d \n", 		lpWaveFormat->nChannels);
    TRACE("nSamplesPerSec=%ld\n", 	lpWaveFormat->nSamplesPerSec);
    TRACE("nAvgBytesPerSec=%ld\n", 	lpWaveFormat->nAvgBytesPerSec);
    TRACE("nBlockAlign=%d \n", 		lpWaveFormat->nBlockAlign);
    TRACE("wBitsPerSample=%u !\n", 	lpWaveFormat->wBitsPerSample);

    /* move to end of 'fmt ' chunk */
    mmioAscend(hmmio, &mmckInfo, 0);

    mmckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &mmckInfo, &ckMainRIFF, MMIO_FINDCHUNK))
	goto errCleanUp;

    TRACE("Chunk Found ckid=%.4s fccType=%.4s cksize=%08lX\n", 
	  (LPSTR)&mmckInfo.ckid, (LPSTR)&mmckInfo.fccType, mmckInfo.cksize);

    s.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

    if (waveOutOpen(&hWave, WAVE_MAPPER, lpWaveFormat, (DWORD)PlaySound_Callback,
		    (DWORD)&s, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	goto errCleanUp;

    /* make it so that 3 buffers per second are needed */
    bufsize = (((lpWaveFormat->nAvgBytesPerSec / 3) - 1) / lpWaveFormat->nBlockAlign + 1) *
	lpWaveFormat->nBlockAlign;
    waveHdr = (LPWAVEHDR)HeapAlloc(GetProcessHeap(), 0, 2 * sizeof(WAVEHDR) + 2 * bufsize);
    waveHdr[0].lpData = (char*)waveHdr + 2 * sizeof(WAVEHDR);
    waveHdr[1].lpData = (char*)waveHdr + 2 * sizeof(WAVEHDR) + bufsize;
    waveHdr[0].dwUser = waveHdr[1].dwUser = 0L;
    waveHdr[0].dwLoops = waveHdr[1].dwLoops = 0L;
    waveHdr[0].dwFlags = waveHdr[1].dwFlags = 0L;
    waveHdr[0].dwBufferLength = waveHdr[1].dwBufferLength = bufsize;
    if (waveOutPrepareHeader(hWave, &waveHdr[0], sizeof(WAVEHDR)) || 
	waveOutPrepareHeader(hWave, &waveHdr[1], sizeof(WAVEHDR))) {
	goto errCleanUp;
    }

    do {
	index = 0;
	left = mmckInfo.cksize;
	s.dwEventCount = 1L; /* for first buffer */

	mmioSeek(hmmio, mmckInfo.dwDataOffset, SEEK_SET);
	while (left) {
	    if (PlaySound_Stop) {
		PlaySound_Stop = PlaySound_Loop = FALSE;
		break;
	    }
	    count = mmioRead(hmmio, waveHdr[index].lpData, min(bufsize, left));
	    if (count < 1) break;
	    left -= count;
	    waveHdr[index].dwBufferLength = count;
	    waveHdr[index].dwFlags &= ~WHDR_DONE;
	    waveOutWrite(hWave, &waveHdr[index], sizeof(WAVEHDR));
	    index ^= 1;
	    PlaySound_WaitDone(&s);
	}
	bRet = TRUE;
    } while (PlaySound_Loop);

    PlaySound_WaitDone(&s);
    waveOutReset(hWave);

    waveOutUnprepareHeader(hWave, &waveHdr[0], sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWave, &waveHdr[1], sizeof(WAVEHDR));

errCleanUp:
    CloseHandle(s.hEvent);
    HeapFree(GetProcessHeap(), 0, waveHdr);
    HeapFree(GetProcessHeap(), 0, lpWaveFormat);
    if (hWave)		while (waveOutClose(hWave) == WAVERR_STILLPLAYING) Sleep(100);
    if (hmmio) 		mmioClose(hmmio, 0);

    return bRet;
}

static DWORD WINAPI PlaySound_Thread(LPVOID arg) 
{
    DWORD     res;
    
    for (;;) {
	PlaySound_Playing = FALSE;
	SetEvent(PlaySound_hReadyEvent);
	res = WaitForSingleObject(PlaySound_hPlayEvent, INFINITE);
	ResetEvent(PlaySound_hReadyEvent);
	SetEvent(PlaySound_hMiddleEvent);
	if (res == WAIT_FAILED) ExitThread(2);
	if (res != WAIT_OBJECT_0) continue;
	PlaySound_Playing = TRUE;
	
	if ((PlaySound_fdwSound & SND_RESOURCE) == SND_RESOURCE) {
	    HRSRC	hRES;
	    HGLOBAL	hGLOB;
	    void*	ptr;

	    if ((hRES = FindResourceA(PlaySound_hmod, PlaySound_pszSound, "WAVE")) == 0) {
		PlaySound_Result = FALSE;
		continue;
	    }
	    if ((hGLOB = LoadResource(PlaySound_hmod, hRES)) == 0) {
		PlaySound_Result = FALSE;
		continue;
	    }
	    if ((ptr = LockResource(hGLOB)) == NULL) {
		FreeResource(hGLOB);
		PlaySound_Result = FALSE;
		continue;
	    }
	    PlaySound_Result = proc_PlaySound((LPCSTR)ptr, 
					      ((UINT16)PlaySound_fdwSound ^ SND_RESOURCE) | SND_MEMORY);
	    FreeResource(hGLOB);
	    continue;
	}
	PlaySound_Result = proc_PlaySound(PlaySound_pszSound, (UINT16)PlaySound_fdwSound);
    }
}


/*****************************************************************************
 *             Plays a sound specified by the given filename, resource, or
 *             system event.
 * Parameters: LPCSTR  pszSound
 *             HMODULE hMod
 *             DWORD   fdwSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(BOOL, PlaySoundA,
              LPCSTR, pszSound,
              HMODULE, hmod,
              DWORD, fdwSound)
{
    static LPSTR StrDup = NULL;
    
    TRACE("pszSound='%p' hmod=%04X fdwSound=%08lX\n",
	  pszSound, hmod, fdwSound);
    
    if (PlaySound_hThread == 0) { /* This is the first time they called us */
	DWORD	id;
	if ((PlaySound_hReadyEvent = CreateEventA(NULL, TRUE, FALSE, NULL)) == 0)
	    return FALSE;
	if ((PlaySound_hMiddleEvent = CreateEventA(NULL, FALSE, FALSE, NULL)) == 0)
	    return FALSE;
	if ((PlaySound_hPlayEvent = CreateEventA(NULL, FALSE, FALSE, NULL)) == 0)
	    return FALSE;
	if ((PlaySound_hThread = CreateThread(NULL, 0, PlaySound_Thread, 0, 0, &id)) == 0) 
	    return FALSE;
    }
    
    /* FIXME? I see no difference between SND_WAIT and SND_NOSTOP ! */ 
    if ((fdwSound & (SND_NOWAIT | SND_NOSTOP)) && PlaySound_Playing) 
	return FALSE;
    
    /* Trying to stop if playing */
    if (PlaySound_Playing) PlaySound_Stop = TRUE;
    
    /* Waiting playing thread to get ready. I think 10 secs is ok & if not then leave*/
    if (WaitForSingleObject(PlaySound_hReadyEvent, 1000*10) != WAIT_OBJECT_0)
	return FALSE;
    
    if (!pszSound || (fdwSound & SND_PURGE)) 
	return FALSE; /* We stoped playing so leaving */
    
    if (PlaySound_SearchMode != 1) PlaySound_SearchMode = 2;
    if (!(fdwSound & SND_ASYNC)) {
	if (fdwSound & SND_LOOP) 
	    return FALSE;
	PlaySound_pszSound = pszSound;
	PlaySound_hmod = hmod;
	PlaySound_fdwSound = fdwSound;
	PlaySound_Result = FALSE;
	SetEvent(PlaySound_hPlayEvent);
	if (WaitForSingleObject(PlaySound_hMiddleEvent, INFINITE) != WAIT_OBJECT_0) 
	    return FALSE;
	if (WaitForSingleObject(PlaySound_hReadyEvent, INFINITE) != WAIT_OBJECT_0) 
	    return FALSE;
	return PlaySound_Result;
    } else {
	PlaySound_hmod = hmod;
	PlaySound_fdwSound = fdwSound;
	PlaySound_Result = FALSE;
	if (StrDup) {
	    HeapFree(GetProcessHeap(), 0, StrDup);
	    StrDup = NULL;
	}
	if (!((fdwSound & SND_MEMORY) || ((fdwSound & SND_RESOURCE) && 
					  !((DWORD)pszSound >> 16)) || !pszSound)) {
	    StrDup = HEAP_strdupA(GetProcessHeap(), 0,pszSound);
	    PlaySound_pszSound = StrDup;
	} else PlaySound_pszSound = pszSound;
	PlaySound_Loop = fdwSound & SND_LOOP;
	SetEvent(PlaySound_hPlayEvent);
	ResetEvent(PlaySound_hMiddleEvent);
	return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
 *             Plays a sound specified by the given filename, resource, or
 *             system event.
 * Parameters: LPCSTR  pszSound
 *             HMODULE hMod
 *             DWORD   fdwSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION3(BOOL, PlaySoundW,
              LPCWSTR, pszSound,
              HMODULE, hmod,
              DWORD, fdwSound)
{
    LPSTR 	pszSoundA;
    BOOL	bSound;
    
    if (!((fdwSound & SND_MEMORY) || ((fdwSound & SND_RESOURCE) && 
				      !((DWORD)pszSound >> 16)) || !pszSound)) {
	pszSoundA = HEAP_strdupWtoA(GetProcessHeap(), 0,pszSound);
	bSound = PlaySoundA(pszSoundA, hmod, fdwSound);
	HeapFree(GetProcessHeap(), 0, pszSoundA);
    } else  
	bSound = PlaySoundA((LPCSTR)pszSound, hmod, fdwSound);
    
    return bSound;
}


/*****************************************************************************
 *             Plays a sound specified by the given filename
 * Parameters: LPCSTR  pszSound
 *             UINT    fuSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION2(BOOL, sndPlaySoundA,
              LPCSTR, lpszSoundName,
              UINT, uFlags)
{
    PlaySound_SearchMode = 1;
    return PlaySoundA(lpszSoundName, 0, uFlags);
}


/*****************************************************************************
 *             Plays a sound specified by the given filename
 * Parameters: LPCWSTR  pszSound
 *             HMODULE  hMod
 *             DWORD    fdwSound
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/05/05 10:44]
 *****************************************************************************/

ODINFUNCTION2(BOOL, sndPlaySoundW,
              LPCWSTR, lpszSoundName,
              UINT, uFlags)
{
    PlaySound_SearchMode = 1;
    return PlaySoundW(lpszSoundName, 0, uFlags);
}

