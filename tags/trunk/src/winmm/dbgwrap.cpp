#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <heapstring.h>

#define DBGWRAP_MODULE "WINMM"
#include <dbgwrap.h>

#include "dbglocal.h"

#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_auxiliary

DEBUGWRAP12(auxGetDevCapsA)
DEBUGWRAP12(auxGetDevCapsW)
DEBUGWRAP0(auxGetNumDevs)
DEBUGWRAP8(auxGetVolume)
DEBUGWRAP16(auxOutMessage)
DEBUGWRAP8(auxSetVolume)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_joy

DEBUGWRAP4(joyConfigChanged)
DEBUGWRAP12(joyGetDevCapsA)
DEBUGWRAP12(joyGetDevCapsW)
DEBUGWRAP0(joyGetNumDevs)
DEBUGWRAP8(joyGetPos)
DEBUGWRAP8(joyGetPosEx)
DEBUGWRAP8(joyGetThreshold)
DEBUGWRAP4(joyReleaseCapture)
DEBUGWRAP16(joySetCapture)
DEBUGWRAP8(joySetThreshold)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_mci

DEBUGWRAP12(mciDriverNotify)
DEBUGWRAP4(mciDriverYield)
DEBUGWRAP4(mciExecute)
DEBUGWRAP4(mciFreeCommandResource)
DEBUGWRAP4(mciGetCreatorTask)
DEBUGWRAP4(mciGetDeviceIDA)
DEBUGWRAP8(mciGetDeviceIDFromElementIDW)
DEBUGWRAP4(mciGetDeviceIDW)
DEBUGWRAP4(mciGetDriverData)
DEBUGWRAP12(mciGetErrorStringA)
DEBUGWRAP12(mciGetErrorStringW)
DEBUGWRAP8(mciGetYieldProc)
DEBUGWRAP12(mciLoadCommandResource)
DEBUGWRAP16(mciSendCommandA)
DEBUGWRAP16(mciSendCommandW)
DEBUGWRAP16(mciSendStringA)
DEBUGWRAP16(mciSendStringW)
DEBUGWRAP8(mciSetDriverData)
DEBUGWRAP12(mciSetYieldProc)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_midi

DEBUGWRAP12(midiConnect)
DEBUGWRAP12(midiDisconnect)
DEBUGWRAP12(midiInAddBuffer)
DEBUGWRAP4(midiInClose)
DEBUGWRAP12(midiInGetDevCapsA)
DEBUGWRAP12(midiInGetDevCapsW)
DEBUGWRAP12(midiInGetErrorTextA)
DEBUGWRAP12(midiInGetErrorTextW)
DEBUGWRAP8(midiInGetID)
DEBUGWRAP0(midiInGetNumDevs)
DEBUGWRAP16(midiInMessage)
DEBUGWRAP20(midiInOpen)
DEBUGWRAP12(midiInPrepareHeader)
DEBUGWRAP4(midiInReset)
DEBUGWRAP4(midiInStart)
DEBUGWRAP4(midiInStop)
DEBUGWRAP12(midiInUnprepareHeader)
DEBUGWRAP16(midiOutCacheDrumPatches)
DEBUGWRAP16(midiOutCachePatches)
DEBUGWRAP4(midiOutClose)
DEBUGWRAP12(midiOutGetDevCapsA)
DEBUGWRAP12(midiOutGetDevCapsW)
DEBUGWRAP12(midiOutGetErrorTextA)
DEBUGWRAP12(midiOutGetErrorTextW)
DEBUGWRAP8(midiOutGetID)
DEBUGWRAP0(midiOutGetNumDevs)
DEBUGWRAP8(midiOutGetVolume)
DEBUGWRAP12(midiOutLongMsg)
DEBUGWRAP16(midiOutMessage)
DEBUGWRAP20(midiOutOpen)
DEBUGWRAP12(midiOutPrepareHeader)
DEBUGWRAP4(midiOutReset)
DEBUGWRAP8(midiOutSetVolume)
DEBUGWRAP8(midiOutShortMsg)
DEBUGWRAP12(midiOutUnprepareHeader)
DEBUGWRAP4(midiStreamClose)
DEBUGWRAP24(midiStreamOpen)
DEBUGWRAP12(midiStreamOut)
DEBUGWRAP4(midiStreamPause)
DEBUGWRAP12(midiStreamPosition)
DEBUGWRAP12(midiStreamProperty)
DEBUGWRAP4(midiStreamRestart)
DEBUGWRAP4(midiStreamStop)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_mixer

DEBUGWRAP4(mixerClose)
DEBUGWRAP12(mixerGetControlDetailsA)
DEBUGWRAP12(mixerGetControlDetailsW)
DEBUGWRAP12(mixerGetDevCapsA)
DEBUGWRAP12(mixerGetDevCapsW)
DEBUGWRAP12(mixerGetID)
DEBUGWRAP12(mixerGetLineControlsA)
DEBUGWRAP12(mixerGetLineControlsW)
DEBUGWRAP12(mixerGetLineInfoA)
DEBUGWRAP12(mixerGetLineInfoW)
DEBUGWRAP0(mixerGetNumDevs)
DEBUGWRAP16(mixerMessage)
DEBUGWRAP20(mixerOpen)
DEBUGWRAP12(mixerSetControlDetails)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_mmio

DEBUGWRAP12(mmioAdvance)
DEBUGWRAP12(mmioAscend)
DEBUGWRAP8(mmioClose)
DEBUGWRAP12(mmioCreateChunk)
DEBUGWRAP16(mmioDescend)
DEBUGWRAP8(mmioFlush)
DEBUGWRAP12(mmioGetInfo)
DEBUGWRAP12(mmioInstallIOProcA)
DEBUGWRAP12(mmioInstallIOProcW)
DEBUGWRAP12(mmioOpenA)
DEBUGWRAP12(mmioOpenW)
DEBUGWRAP12(mmioRead)
DEBUGWRAP16(mmioRenameA)
DEBUGWRAP16(mmioRenameW)
DEBUGWRAP12(mmioSeek)
DEBUGWRAP16(mmioSendMessage)
DEBUGWRAP16(mmioSetBuffer)
DEBUGWRAP12(mmioSetInfo)
DEBUGWRAP8(mmioStringToFOURCCA)
DEBUGWRAP8(mmioStringToFOURCCW)
DEBUGWRAP12(mmioWrite)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_time

DEBUGWRAP0(mmsystemGetVersion)
DEBUGWRAP4(timeBeginPeriod)
DEBUGWRAP4(timeEndPeriod)
DEBUGWRAP8(timeGetDevCaps)
DEBUGWRAP8(timeGetSystemTime)
DEBUGWRAP_LVL2_0(timeGetTime)
DEBUGWRAP4(timeKillEvent)
DEBUGWRAP20(timeSetEvent)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_wavein

DEBUGWRAP12(waveInAddBuffer)
DEBUGWRAP4(waveInClose)
DEBUGWRAP12(waveInGetDevCapsA)
DEBUGWRAP12(waveInGetDevCapsW)
DEBUGWRAP12(waveInGetErrorTextA)
DEBUGWRAP12(waveInGetErrorTextW)
DEBUGWRAP8(waveInGetID)
DEBUGWRAP0(waveInGetNumDevs)
DEBUGWRAP12(waveInGetPosition)
DEBUGWRAP16(waveInMessage)
DEBUGWRAP24(waveInOpen)
DEBUGWRAP12(waveInPrepareHeader)
DEBUGWRAP4(waveInReset)
DEBUGWRAP4(waveInStart)
DEBUGWRAP4(waveInStop)
DEBUGWRAP12(waveInUnprepareHeader)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_waveout

DEBUGWRAP4(waveOutBreakLoop)
DEBUGWRAP4(waveOutClose)
DEBUGWRAP12(waveOutGetDevCapsA)
DEBUGWRAP12(waveOutGetDevCapsW)
DEBUGWRAP12(waveOutGetErrorTextA)
DEBUGWRAP12(waveOutGetErrorTextW)
DEBUGWRAP8(waveOutGetID)
DEBUGWRAP0(waveOutGetNumDevs)
DEBUGWRAP8(waveOutGetPitch)
DEBUGWRAP8(waveOutGetPlaybackRate)
DEBUGWRAP12(waveOutGetPosition)
DEBUGWRAP8(waveOutGetVolume)
DEBUGWRAP16(waveOutMessage)
DEBUGWRAP24(waveOutOpen)
DEBUGWRAP4(waveOutPause)
DEBUGWRAP12(waveOutPrepareHeader)
DEBUGWRAP4(waveOutReset)
DEBUGWRAP4(waveOutRestart)
DEBUGWRAP8(waveOutSetPitch)
DEBUGWRAP8(waveOutSetPlaybackRate)
DEBUGWRAP8(waveOutSetVolume)
DEBUGWRAP12(waveOutUnprepareHeader)
DEBUGWRAP12(waveOutWrite)

#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_driver

DEBUGWRAP12(CloseDriver)
DEBUGWRAP20(DefDriverProc)
DEBUGWRAP28(DriverCallback)
DEBUGWRAP4(GetDriverModuleHandle)
DEBUGWRAP12(OpenDriverA)
DEBUGWRAP12(OpenDriverW)
DEBUGWRAP16(SendDriverMessage)
DEBUGWRAP4(GetDriverFlags)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG	DBG_playsound

DEBUGWRAP8(sndPlaySoundA)
DEBUGWRAP8(sndPlaySoundW)
DEBUGWRAP12(PlaySoundA)
DEBUGWRAP12(PlaySoundW)

