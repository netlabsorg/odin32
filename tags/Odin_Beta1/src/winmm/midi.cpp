/* $Id: midi.cpp,v 1.9 2001-02-27 21:13:59 sandervl Exp $ */

/*
 * RTMIDI code
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <unicode.h>

#include "midi.hpp"
#include <winos2def.h>
#include <wprocess.h>

#define DBG_LOCALLOG	DBG_midi
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-MIDI)

//SvL: 23/09/99: WinPostMsg no longer works, as win32 window handles are no longer PM handles
BOOL WIN32API PostMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
   Work to do:
   - Some entrys allow a device ID to stand in for a handle, allow that.
   - Output of long messages
   - Midi Streams
*/

char * getWinmmMsg( MMRESULT result )
{
  char * ret;
  switch ( result )
  {
    case MMSYSERR_ERROR:
      ret = "Unspecified error";
      break;
    case MMSYSERR_BADDEVICEID:
      ret = "Device ID out of range";
      break;
    case MMSYSERR_NOTENABLED:
      ret = "driver failed enable";
      break;
    case MMSYSERR_ALLOCATED:
      ret = "device already allocated";
      break;
    case MMSYSERR_INVALHANDLE:
      ret = "device handle is invalid";
      break;
    case MMSYSERR_NODRIVER:
      ret = "no device driver present";
      break;
    case MMSYSERR_NOMEM:
      ret = "memory allocation error";
      break;
    case MMSYSERR_NOTSUPPORTED:
      ret = "function isn't supported";
      break;
    case MMSYSERR_BADERRNUM:
      ret = "error value out of range";
      break;
    case MMSYSERR_INVALFLAG:
      ret = "invalid flag passed";
      break;
    case MMSYSERR_INVALPARAM:
      ret = "invalid parameter passed";
      break;
    case MMSYSERR_HANDLEBUSY:
      ret = "handle being used simultaneously on another thread";
      break;
    case MMSYSERR_INVALIDALIAS:
      ret = "specified alias not found";
      break;
    case MMSYSERR_BADDB:
      ret = "bad registry database";
      break;
    case MMSYSERR_KEYNOTFOUND:
      ret = "registry key not found";
      break;
    case MMSYSERR_READERROR:
      ret = "registry read error";
      break;
    case MMSYSERR_WRITEERROR:
      ret = "registry write error";
      break;
    case MMSYSERR_DELETEERROR:
      ret = "registry delete error";
      break;
    case MMSYSERR_VALNOTFOUND:
      ret = "registry value not found";
      break;
    case MMSYSERR_NODRIVERCB:
      ret = "driver does not call DriverCallback";
      break;
    case MIDIERR_NODEVICE:
      ret = "Midi Mapper not Implemented yet.";
      break;
    default:
      ret = NULL;
      break;
  }
  return ret;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiConnect,
              HMIDI, hMidiIn,
              HMIDIOUT, hMidiOut,
              LPVOID, pReserved)
{
  // TODO: Implement using instance connections
  dprintf(("WINMM:midiConnect -- not Implemented\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     MidiIn * midiIn = MidiIn::find( hMidiIn );
     if ( midiIn )
       rc = MMSYSERR_NOTSUPPORTED;
     else
       rc = MMSYSERR_INVALHANDLE;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiDisconnect,
              HMIDI, hMidiIn,
              HMIDIOUT, hMidiOut,
              LPVOID, pReserved)
{
  // TODO: Implement using instance connections
  dprintf(("WINMM:midiDisconnect -- not Implemented\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     MidiIn * midiIn = MidiIn::find( hMidiIn );
     if ( midiIn )
       rc = MMSYSERR_NOTSUPPORTED;
     else
       rc = MMSYSERR_INVALHANDLE;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInAddBuffer,
              HMIDIIN, hMidiIn,
              LPMIDIHDR, lpMidiInHdr,
              UINT, cbMidiInHdr)
{
  dprintf(("WINMM:midiInAddBuffer -- not Implemented\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiInClose,
              HMIDIIN, hMidiIn)
{
  dprintf(("WINMM:midiInClose -- partially Implemented\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     delete midiIn;
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInGetDevCapsA,
              UINT, uDeviceId,
              LPMIDIINCAPSA, midiInCaps,
              UINT,  sMidiInCaps)
{
  dprintf(("WINMM:midiInGetDevCapsA(%u)\n", uDeviceId ));
  MMRESULT rc;
  if ( sMidiInCaps == 0 )
    rc = MMSYSERR_NOERROR;
  else
  {
    IMidiInstance * theInst = IRTMIDI->inInstance( uDeviceId );
    if ( theInst )
    {
       midiInCaps->wMid = 0;            // Manufacturer ID
       midiInCaps->wPid = 0;            // Product ID
       midiInCaps->vDriverVersion = 0x0001;  // Driver Version
       strncpy( midiInCaps->szPname, theInst->name(), MAXPNAMELEN );
       midiInCaps->dwSupport = 0;
       rc = MMSYSERR_NOERROR;
    }
    else
       rc = MMSYSERR_BADDEVICEID;
  }
  return rc;
}


/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInGetDevCapsW,
              UINT, uDeviceId,
              LPMIDIINCAPSW, midiInCaps,
              UINT,  sMidiInCaps )
{
  dprintf(("WINMM:midiInGetDevCapsA(%u)\n", uDeviceId ));
  MMRESULT rc;
  if ( sMidiInCaps == 0 )
    rc = MMSYSERR_NOERROR;
  else
  {
    IMidiInstance * theInst = IRTMIDI->inInstance( uDeviceId );
    if ( theInst )
    {
       midiInCaps->wMid = 0;            // Manufacturer ID
       midiInCaps->wPid = 0;            // Product ID
       midiInCaps->vDriverVersion = 0x0001;  // Driver Version
       char name[ MAXPNAMELEN ];
       strncpy( name, theInst->name(), MAXPNAMELEN );
       AsciiToUnicode( name, midiInCaps->szPname );
       midiInCaps->dwSupport = 0;
       rc = MMSYSERR_NOERROR;
    }
    else
       rc = MMSYSERR_BADDEVICEID;
  }
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInGetErrorTextA,
              MMRESULT, wError,
              LPSTR, lpText,
              UINT, cchText)
{
  dprintf(("WINMM:midiInGetErrorTextA(%d)\n", wError ));
  char * theMsg = getWinmmMsg( wError );
  if ( theMsg )
    strncpy( lpText, theMsg, cchText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", wError );
    strncpy( lpText, errMsg, cchText );
  }
  return MMSYSERR_NOERROR;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInGetErrorTextW,
              MMRESULT, wError,
              LPWSTR, lpText,
              UINT, cchText)
{
  dprintf(("WINMM:midiInGetErrorTextW(%d)\n", wError ));
  char * theMsg = getWinmmMsg( wError );
  if ( theMsg )
    AsciiToUnicode( theMsg, lpText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", wError );
    AsciiToUnicode( errMsg, lpText );
  }
  return MMSYSERR_NOERROR;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, midiInGetID,
              HMIDIIN, hMidiIn,
              LPUINT, puDeviceID)
{
  dprintf(("WINMM:midiInGetID\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     *puDeviceID = midiIn->device();
     dprintf(("     device(%d)\n", *puDeviceID ));
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION4(UINT, midiInMessage,
              HMIDIIN, hmi,
              UINT, msg,
              DWORD, dw1,
              DWORD, dw2)
{
  dprintf(("WINMM:midiInMessage -- not Implemented\n" ));
  return 0;
}

/******************************************************************************/
ODINFUNCTION0(UINT, midiInGetNumDevs)
{
  UINT i = IRTMIDI->numInInstances();
  return i;
}

/******************************************************************************/
ODINFUNCTION5(MMRESULT, midiInOpen,
              LPHMIDIIN, lphMidiIn,
              UINT, uDeviceId,
              DWORD, dwCallback,
              DWORD, dwCallbackInstance,
              DWORD, dwflags)
{
  dprintf(("WINMM:midiInOpen(%d) --  partial Implementation\n", uDeviceId ));
  dprintf(("      dwflags(%lx)\n", dwflags ));
  MMRESULT rc;
  // Create a MidiIn
  MidiIn * midiIn = new MidiIn( uDeviceId );
  if ( midiIn )
  {
    *lphMidiIn = (HMIDIIN)midiIn;

    midiIn->setCallback( dwCallback, dwCallbackInstance, dwflags );

    rc = midiIn->open();
  }
  else
    rc = MMSYSERR_BADDEVICEID;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInPrepareHeader,
              HMIDIIN, hMidiIn,
              LPMIDIHDR, lpMidiInHdr,
              UINT, cbMidiInHdr)
{
  dprintf(("WINMM:midiInPrepareHeader -- not Implemented\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
  return MMSYSERR_INVALHANDLE;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiInReset,
              HMIDIIN, hMidiIn)
{
  dprintf(("WINMM:midiInReset -- not Implemented\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiInStart,
              HMIDIIN, hMidiIn)
{
  dprintf(("WINMM:midiInStart\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     rc = midiIn->start();
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiInStop,
              HMIDIIN, hMidiIn)
{
  dprintf(("WINMM:midiInStop\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     rc = midiIn->stop();
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiInUnprepareHeader,
              HMIDIIN, hMidiIn,
              LPMIDIHDR, lpMidiInHdr,
              UINT, cbMidiInHdr)
{
  dprintf(("WINMM:midiInUnPrepareHeader -- not Implemented\n" ));
  MMRESULT rc;
  MidiIn * midiIn = MidiIn::find( hMidiIn );
  if ( midiIn )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION4(MMRESULT, midiOutCacheDrumPatches,
              HMIDIOUT, hMidiOut,
              UINT, wPatch,
              WORD *, lpKeyArray,
              UINT, wFlags)
{
  // Valid only for an Internal synth.  So we won't do it for now.
  dprintf(("WINMM:midiOutCacheDrumPatches\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION4(MMRESULT, midiOutCachePatches,
              HMIDIOUT, hMidiOut,
              UINT, wBank,
              WORD *, lpPatchArray,
              UINT, wFlags)
{
  // Valid only for an Internal synth.  So we won't do it for now.
  dprintf(("WINMM:midiOutCachePatches\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiOutClose,
              HMIDIOUT, hMidiOut)
{
  dprintf(("WINMM:midiOutClose - partially implemented\n" ));
  // TODO: Check if buffers are still playing and return MIDIERR_STILLPLAYING
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     delete midiOut;
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutGetDevCapsA,
              UINT, uDeviceId,
              LPMIDIOUTCAPSA, midiOutCaps,
              UINT,  sMidiOutCaps )
{
  // TODO: Actually fill in the important fields
  dprintf(("WINMM:midiOutGetDevCapsA(%u)\n", uDeviceId ));
  MMRESULT rc;
  if ( sMidiOutCaps == 0 )
    rc = MMSYSERR_NOERROR;
  else
  if ( uDeviceId == 0xFFFF || uDeviceId == 0xFFFFFFFF )
  {
     // We got here a Midi Mapper
     midiOutCaps->wMid = 0;            // Manufacturer ID
     midiOutCaps->wPid = 0;            // Product ID
     midiOutCaps->vDriverVersion = 0x0001;  // Driver Version
     strncpy( midiOutCaps->szPname, "MIDI Mapper", MAXPNAMELEN );
     midiOutCaps->wTechnology = MOD_MAPPER;
     midiOutCaps->wVoices = 0;
     midiOutCaps->wNotes = 0;
     midiOutCaps->wChannelMask = 0;
     midiOutCaps->dwSupport = MIDICAPS_VOLUME
                            | MIDICAPS_LRVOLUME;
     rc = MMSYSERR_NOERROR;
  }
  else
  {
    IMidiInstance * theInst = IRTMIDI->outInstance( uDeviceId );
    if ( theInst )
    {
       // need to fill in capabilities of the MIDI Device
       midiOutCaps->wMid = 0;            // Manufacturer ID
       midiOutCaps->wPid = 0;            // Product ID
       midiOutCaps->vDriverVersion = 0x0001;  // Driver Version
       strncpy( midiOutCaps->szPname, theInst->name(), MAXPNAMELEN );
       midiOutCaps->wTechnology = MOD_MIDIPORT;
       midiOutCaps->wVoices = 0;
       midiOutCaps->wNotes = 0;
       midiOutCaps->wChannelMask = 0;
       midiOutCaps->dwSupport = MIDICAPS_VOLUME
                              | MIDICAPS_LRVOLUME;
       rc = MMSYSERR_NOERROR;
    }
    else
       rc = MMSYSERR_BADDEVICEID;
  }
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutGetDevCapsW,
              UINT, uDeviceId,
              LPMIDIOUTCAPSW, midiOutCaps,
              UINT,  sMidiOutCaps )
{
  // TODO: Actually fill in the important fields
  dprintf(("WINMM:midiOutGetDevCapsW(%u)- partially implemented\n", uDeviceId ));
  MMRESULT rc;
  if ( sMidiOutCaps == 0 )
    rc = MMSYSERR_NOERROR;
  else
  if ( uDeviceId == 0xFFFF || uDeviceId == 0xFFFFFFFF )
  {
     // We got here a Midi Mapper
     midiOutCaps->wMid = 0;            // Manufacturer ID
     midiOutCaps->wPid = 0;            // Product ID
     midiOutCaps->vDriverVersion = 0x0001;  // Driver Version
     AsciiToUnicode( "MIDI Mapper", midiOutCaps->szPname );
     midiOutCaps->wTechnology = MOD_MAPPER;
     midiOutCaps->wVoices = 0;
     midiOutCaps->wNotes = 0;
     midiOutCaps->wChannelMask = 0;
     midiOutCaps->dwSupport = MIDICAPS_VOLUME
                            | MIDICAPS_LRVOLUME;
     rc = MMSYSERR_NOERROR;
  }
  else
  {
    IMidiInstance * theInst = IRTMIDI->outInstance( uDeviceId );
    if ( theInst )
    {
       // need to fill in capabilities of the MIDI Device
       midiOutCaps->wMid = 0;            // Manufacturer ID
       midiOutCaps->wPid = 0;            // Product ID
       midiOutCaps->vDriverVersion = 0x0001;  // Driver Version
       char name[ MAXPNAMELEN ];
       strncpy( name, theInst->name(), MAXPNAMELEN );
       AsciiToUnicode( name, midiOutCaps->szPname );
       midiOutCaps->wTechnology = MOD_MIDIPORT;
       midiOutCaps->wVoices = 0;
       midiOutCaps->wNotes = 0;
       midiOutCaps->wChannelMask = 0;
       midiOutCaps->dwSupport = MIDICAPS_VOLUME
                              | MIDICAPS_LRVOLUME;
       rc = MMSYSERR_NOERROR;
    }
    else
       rc = MMSYSERR_BADDEVICEID;
  }
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutGetErrorTextA,
              MMRESULT, wError,
              LPSTR, lpText,
              UINT, cchText)
{
  dprintf(("WINMM:midiOutGetErrorTextA(%d)\n", wError ));
  char * theMsg = getWinmmMsg( wError );
  if ( theMsg )
    strncpy( lpText, theMsg, cchText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", wError );
    strncpy( lpText, errMsg, cchText );
  }
  return MMSYSERR_NOERROR;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutGetErrorTextW,
              MMRESULT, wError,
              LPWSTR, lpText,
              UINT, cchText)
{
  dprintf(("WINMM:midiOutGetErrorTextW(%d) - need to translate\n", wError ));
  char * theMsg = getWinmmMsg( wError );
  if ( theMsg )
    AsciiToUnicode( theMsg, lpText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", wError );
    AsciiToUnicode( errMsg, lpText );
  }
  return MMSYSERR_NOERROR;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, midiOutGetID,
              HMIDIOUT, hMidiOut,
              LPUINT, puDeviceID)
{
  dprintf(("WINMM:midiOutGetID\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     *puDeviceID = midiOut->device();
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION0(UINT, midiOutGetNumDevs)
{
  UINT i = IRTMIDI->numOutInstances();
  return i;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, midiOutGetVolume,
              HMIDIOUT, hMidiOut,
              LPDWORD, lpdwVolume)
{
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     *lpdwVolume = midiOut->getVolume();
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutLongMsg,
              HMIDIOUT, hMidiOut,
              LPMIDIHDR, lpMidiOutHdr,
              UINT, cbMidiOutHdr)
{
  // TODO: Implement this
  // - Buffer must be "prepared" first.
  // - send async or sync, use callback (MOM_DONE) when done and mark buffer as done
  dprintf(("WINMM:midiOutLongMsg -- not Implemented\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION4(UINT, midiOutMessage,
              HMIDIOUT, hMidiOut,
              UINT, msg,
              DWORD, dw1,
              DWORD, dw2)
{
  // TODO: Need to find out wha drivers actually return.
  dprintf(("WINMM:midiOutMessage -- not Implemented\n" ));
  return 0;
}

/******************************************************************************/
ODINFUNCTION5(MMRESULT, midiOutOpen,
              LPHMIDIOUT, lphMidiOut,
              UINT, uDeviceId,
              DWORD, dwCallback,
              DWORD, dwCallbackInstance,
              DWORD, dwflags)
{
  // TODO: - Handle thread callback, if any program really needs it
  // Note uDeviceId of MIDIMAPPER is special ID for MIDI Mapper

  dprintf(("WINMM:midiOutOpen(%lX)\n", uDeviceId ));
  dprintf(("      dwflags(%lx)\n", dwflags ));
  MMRESULT rc;
  MidiOut * midiOut;
  // Create a MidiOut
  if ( uDeviceId == 0xFFFF || uDeviceId == 0xFFFFFFFF )
  {
    dprintf(("      open of Midi Mapper requested\n"));
    midiOut = new MidiMapper();
  }
  else
  {
    if ( uDeviceId < IRTMIDI->numOutInstances() )
      midiOut = new MidiOut( uDeviceId );
    else
      midiOut = NULL;
  }
  if ( midiOut )
  {
    *lphMidiOut = (HMIDIOUT)midiOut;

    midiOut->setCallback( dwCallback, dwCallbackInstance, dwflags );

    rc = midiOut->open();
  }
  else
    rc = MMSYSERR_BADDEVICEID;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutPrepareHeader,
              HMIDIOUT, hMidiOut,
              LPMIDIHDR, lpMidiOutHdr,
              UINT, cbMidiOutHdr)
{
  // TODO: Either implement or treat as a NOOP as we may not need to "prepare"
  //       for RTMIDI.
  dprintf(("WINMM:midiOutPrepareHeader -- not Implemented\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION1(MMRESULT, midiOutReset,
              HMIDIOUT, hMidiOut)
{
  // TODO: - return pending output buffers to callback
  //       - Send a note off for each note on each channel
  //       - Turn off sustain controller on each channel
  dprintf(("WINMM:midiOutReset -- not Implemented\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
    return MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, midiOutSetVolume,
              HMIDIOUT, hMidiOut,
              DWORD, dwVolume)
{
  dprintf(("WINMM:midiOutSetVolume\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     midiOut->sendVolume( dwVolume );
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION2(MMRESULT, midiOutShortMsg,
              HMIDIOUT, hMidiOut,
              DWORD, dwMsg)
{
  dprintf(("WINMM:midiOutShortMsg(%X)\n", dwMsg ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     midiOut->sendMessage( dwMsg );
     rc = MMSYSERR_NOERROR;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

/******************************************************************************/
ODINFUNCTION3(MMRESULT, midiOutUnprepareHeader,
              HMIDIOUT, hMidiOut,
              LPMIDIHDR, lpMidiOutHdr,
              UINT, cbMidiOutHdr)
{
  // TODO: - return MIDIERR_STILLPLAYING if buffer is playing
  //       - otherwise there is no need to unprepare
  dprintf(("WINMM:midiOutUnprepareHeader -- not Implemented\n" ));
  MMRESULT rc;
  MidiOut * midiOut = MidiOut::find( hMidiOut );
  if ( midiOut )
  {
     rc = MMSYSERR_NOTSUPPORTED;
  }
  else
     rc = MMSYSERR_INVALHANDLE;
  return rc;
}

// TO ADD: midiStream Close, Open, Out, Pause, Position, Property, Restart, Stop

/******************************************************************************/
/* Midi Classes to interface to RTMidi                                        */
/******************************************************************************/
Midi::Midi( UINT uDeviceId )
    :iDeviceId( uDeviceId )
    ,iHwdInstance( NULL )
    ,iAppInstance( NULL )
    ,iCallbackFunction( 0 )
    ,iCallbackWindow( 0 )
    ,iCallbackInstance( )
{
}

Midi::~Midi()
{
}

void Midi::setCallback( DWORD dwCallback,
                        DWORD dwCallbackInstance,
                        DWORD dwflags )
{
   iCallbackInstance = dwCallbackInstance;

   // What kind of callback?
   if ( (dwflags & CALLBACK_TYPEMASK) == CALLBACK_WINDOW )
      iCallbackWindow = (HWND)dwCallback;
   else
   if ( (dwflags & CALLBACK_TYPEMASK) == CALLBACK_FUNCTION )
      iCallbackFunction = (LPDRVCALLBACK)dwCallback;
}

void Midi::callback( UINT msg, DWORD p1, DWORD p2)
{
  USHORT selTIB;

  dprintf(("WINMM: callback %x %lx %lx\n", msg, p1, p2 ));

   if ( iCallbackFunction )
   {
     selTIB = SetWin32TIB();
     iCallbackFunction( (ULONG)this, msg, iCallbackInstance, p1, p2 );
     SetFS(selTIB);
   }
   else
   if ( iCallbackWindow )
   {
      PostMessageA( iCallbackWindow, msg, (WPARAM)p1, (LPARAM)p2 );
   }
}


static void _Optlink TimerInHandler(void *);

MidiIn * MidiIn::iFirst = NULL;

MidiIn::MidiIn( UINT uDeviceId )
   : Midi( uDeviceId )
   , iNext( NULL )
{
   // Create application instance
   iAppInstance = new IAppMidiInstance( MIDI_INST_ATTR_CAN_RECV );
   // Lookup Hardware instance
   iHwdInstance = IRTMIDI->inInstance( uDeviceId );
   // Add us to the linked list
   iNext = iFirst;
   iFirst = this;
}

MidiIn* MidiIn::find( HMIDIIN theMidiIn )
{
  dprintf(( "      MidiIn::find(%X)\n", theMidiIn ));
  for( MidiIn * m = iFirst; m; m->iNext )
  {
    if ( m == (MidiIn*)theMidiIn )
      return m;
  }
  return NULL;
}

MidiIn* MidiIn::find( UINT uDeviceId )
{
  for( MidiIn * m = iFirst; m; m->iNext )
  {
    if ( m->device() == uDeviceId )
      return m;
  }
  return NULL;
}

MidiIn::~MidiIn()
{
  // Remove from linked list
  if ( iFirst == this )
    iFirst = iNext;
  else
  {
    for ( MidiIn* m = iFirst; m; m->iNext )
    {
      if ( m->iNext == this )
      {
        m->iNext = iNext;
        break;
      }
    }
  }
}

MMRESULT MidiIn::open()
{
   // Enable Appl instance to receive and the hardware instance to send
   iAppInstance->enableReceive( (ULONG)TRUE );
   iHwdInstance->enableSend( (ULONG)TRUE );

   // Link the two instances
   iHwdInstance->addLink( iAppInstance );
   callback( MIM_OPEN, 0, 0 );
   return MMSYSERR_NOERROR;
}

MMRESULT MidiIn::close()
{
   // Disable the link
   iHwdInstance->removeLink( iAppInstance );
   callback( MIM_CLOSE, 0, 0 );
   return MMSYSERR_NOERROR;
}

MMRESULT MidiIn::start()
{
   iRunning = TRUE;
   iStartTime = IRTMIDI->currentTime();
   iThreadId = _beginthread(TimerInHandler, NULL, 0x4000, (void *)this);
   dprintf(("WINMM: MidiIn::start %lx\n", iThreadId ));
   return MMSYSERR_NOERROR;
}

void MidiIn::getMessages()
{
   dprintf(("WINMM: MidiIn::getMessages\n" ));
   while( iRunning )
   {
      ULONG msgTime;
      ULONG msg;
      ULONG rc;
      rc = iAppInstance->getMessage( &msgTime, &msg );
      if ( iRunning )
      {
        if ( rc == 0 )
        {
          callback( MIM_DATA, msg, msgTime-iStartTime );
        }
        else
        {
          dprintf(("WINMM: MidiIn::getMessages(%s)\n", IRTMIDI->RCExplanation() ));
        }
      }
   }
   dprintf(("WINMM: MidiIn stopped\n"));
}

MMRESULT MidiIn::stop()
{
   // Here we will stop the thread by posting the stop flag and then
   // sending a zero message to ensure it sees the flag
   iRunning = FALSE;

   MINSTANCE stopper;
   IRTMIDI->MidiCreateInstance( IRTMIDI->applicationClass(), &stopper, "stopper", 0 );
   IRTMIDI->MidiEnableInstance( stopper, MIDI_ENABLE_SEND );
   IRTMIDI->MidiAddLink( stopper, iAppInstance->instance(), 0, 0 );
   MESSAGE msg;
   msg.ulSourceInstance = stopper;
   msg.ulTime           = 0;
   msg.ulTrack          = 0;
   msg.msg.ulMessage    = 0xf7fffff0;
   IRTMIDI->MidiSendMessages( &msg, 1, 0 );
   IRTMIDI->MidiRemoveLink( stopper, iAppInstance->instance(), 0, 0 );
   IRTMIDI->MidiDisableInstance( stopper, MIDI_DISABLE_SEND );
   IRTMIDI->MidiDeleteInstance( stopper, 0 );

   // Sleep to let it stop
   DosSleep( 10l );

   return MMSYSERR_NOERROR;
}

static void _Optlink TimerInHandler(void *vMidiIn)
{
  unsigned long rc=0;
  MidiIn * midiIn = (MidiIn*)vMidiIn;
  midiIn->getMessages();
  _endthread();
}

MidiOut * MidiOut::iFirst = NULL;

MidiOut::MidiOut( UINT uDeviceId )
   : Midi( uDeviceId )
   , iNext( NULL )
{
  // Create application instance
  if ( uDeviceId == 0xFFFF || uDeviceId == 0xFFFFFFFF )
  {
    dprintf(("      MidiOut::mapper(%X)\n", uDeviceId ));
    iAppInstance = 0;
    iHwdInstance = 0;
  }
  else
  {
    iAppInstance = new IAppMidiInstance( MIDI_INST_ATTR_CAN_SEND );
    iHwdInstance = IRTMIDI->outInstance( uDeviceId );
    dprintf(( "WINMM: MidiOut::MidiOut(%s)\n",iHwdInstance->name()));
  }
  // Add us to the linked list
  iNext = iFirst;
  iFirst = this;
}

MidiOut* MidiOut::find( HMIDIOUT theMidiOut )
{
  dprintf(( "      MidiOut::find(%X)\n", theMidiOut ));
  for( MidiOut * m = iFirst; m; m->iNext )
  {
    if ( m == (MidiOut*)theMidiOut )
      return m;
  }
  return NULL;
}

MidiOut* MidiOut::find( UINT uDeviceId )
{
  for( MidiOut * m = iFirst; m; m->iNext )
  {
    if ( m->device() == uDeviceId )
      return m;
  }
  return NULL;
}

MidiOut::~MidiOut()
{
  // Remove from linked list
  if ( iFirst == this )
    iFirst = iNext;
  else
  {
    for ( MidiOut* m = iFirst; m; m->iNext )
    {
      if ( m->iNext == this )
      {
        m->iNext = iNext;
        break;
      }
    }
  }
}

MMRESULT MidiOut::open()
{
   dprintf(( "WINMM: MidiOut::open(%s)\n",iHwdInstance->name()));
   // Enable Appl instance to send and the hwd instance to receive
   iAppInstance->enableSend( (ULONG)TRUE );
   iHwdInstance->enableReceive( (ULONG)TRUE );

   // Link the two instances
   iAppInstance->addLink( iHwdInstance );
   callback( MOM_OPEN, 0, 0 );
   return MMSYSERR_NOERROR;
}

MMRESULT MidiOut::close()
{
   dprintf(( "WINMM: MidiOut::close(%s)\n",iHwdInstance->name()));
   iAppInstance->removeLink( iHwdInstance );
   callback( MOM_CLOSE, 0, 0 );
   return MMSYSERR_NOERROR;
}

MMRESULT MidiOut::sendMessage( DWORD inMsg ) const
{
   dprintf(( "WINMM: MidiOut::sendMessage(%s)%X\n",iHwdInstance->name(),inMsg ));
   iAppInstance->sendMessage( inMsg );
   ULONG rc = IRTMIDI->lastRC();
   if ( rc )
   {  dprintf(("MidiOut:send failure(%s)", IRTMIDI->RCExplanation() )); }
   return MMSYSERR_NOERROR;
}

MMRESULT MidiOut::sendSysexMessage( BYTE* inMsg, ULONG msgLen ) const
{
   dprintf(( "WINMM: MidiOut::sendSysexMessage(%s) %d bytes\n",iHwdInstance->name(),msgLen ));
   iAppInstance->sendSysexMessage( inMsg, msgLen );
   ULONG rc = IRTMIDI->lastRC();
   if ( rc )
   {  dprintf(("MidiOut:send failure(%s)", IRTMIDI->RCExplanation() )); }
   return MMSYSERR_NOERROR;
}

DWORD MidiOut::getVolume() const
{
  return iVolume;
}

MMRESULT MidiOut::sendVolume( DWORD volume ) const
{
   dprintf(( "WINMM: MidiOut::sendVolume(%s) %lX bytes\n",iHwdInstance->name(), volume));
   ((MidiOut*)this)->iVolume = volume;
   BYTE msg[10];
   msg[0] = 0xF0;
   msg[1] = 0x00;
   msg[2] = 0x00;
   msg[3] = 0x3A;
   msg[4] = 0x03;
   msg[5] = 0x09;
   msg[6] = 0x0000FFFF & volume;         // left vol
   msg[7] = 0x0000FFFF & (volume >> 16); // right vol
   msg[8] = 0x00;
   msg[9] = 0xf7;
   return sendSysexMessage( msg, 10 );
}

MidiMapper::MidiMapper()
  : MidiOut( 0xFFFFFFFF )
{
}

MidiMapper::~MidiMapper()
{
}

MMRESULT MidiMapper::open()
{
  // Do nothing for now
  return MMSYSERR_NOERROR;
}

MMRESULT MidiMapper::close()
{
  // Do nothing for now
  return MMSYSERR_NOERROR;
}

MMRESULT MidiMapper::sendMessage( DWORD inMsg ) const
{
  // Do nothing for now
  return MMSYSERR_NOERROR;
}

MMRESULT MidiMapper::sendSysexMessage( BYTE* inMsg, ULONG msgLen ) const
{
  // Do nothing for now
  return MMSYSERR_NOERROR;
}


