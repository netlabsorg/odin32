/*
 * RTMIDI code
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _MIDI_HPP_
#define _MIDI_HPP_

#include "misc.h"
#include "winmm.h"
#include "irtmidi.hpp"

// The Midi class represent a win32 midi device.
class Midi
{
  public:
   Midi( UINT uDeviceId );
   virtual ~Midi();
   UINT device() const
      { return iDeviceId; };
   void setCallback( DWORD dwCallback,
                     DWORD dwCallbackInstance,
                     DWORD dwflags );
   virtual MMRESULT open()=0;
   virtual MMRESULT close()=0;
   void callback( UINT msg, DWORD p1, DWORD p2);

  protected:
   UINT            iDeviceId;
   IMidiInstance * iHwdInstance;   // The Hardware Instance that this connected to
   IAppMidiInstance* iAppInstance;

   LPDRVCALLBACK iCallbackFunction;
   HWND         iCallbackWindow;
   DWORD        iCallbackInstance;
};

class MidiIn : public Midi
{
  public:
   MidiIn( UINT uDeviceId );
   virtual ~MidiIn();
   virtual MMRESULT open();
   virtual MMRESULT start();
   virtual MMRESULT stop();
   virtual MMRESULT close();
   void    getMessages();
   static MidiIn* find( HMIDIIN );
   static MidiIn* find( UINT uDeviceId );
  private:
   IAppMidiInstance* iAppInstance;
   unsigned long     iStartTime;
   int               iThreadId;
   BOOL              iRunning;
   static MidiIn*    iFirst;   // First in linked list of MidiIn devices
   MidiIn*           iNext;    // Next in linked list of handles
};

class MidiOut : public Midi
{
  public:
   MidiOut( UINT uDeviceId );
   virtual ~MidiOut();
   virtual MMRESULT open();
   virtual MMRESULT close();
   virtual MMRESULT sendMessage( DWORD msg ) const;
   virtual MMRESULT sendSysexMessage( BYTE* msg, ULONG msgLen ) const;
   virtual MMRESULT sendVolume( DWORD volume ) const;
   virtual DWORD    getVolume() const;
   static MidiOut* find( HMIDIOUT );
   static MidiOut* find( UINT uDeviceId );
  protected:
   static MidiOut*   iFirst;   // First in linked list of MidiOut devices
   MidiOut*          iNext;    // Next in linked list of handles
   DWORD             iVolume;
};

class MidiMapper : public MidiOut
{
  public:
   MidiMapper();
   virtual ~MidiMapper();
   virtual MMRESULT open();
   virtual MMRESULT close();
   virtual MMRESULT sendMessage( DWORD msg ) const;
   virtual MMRESULT sendSysexMessage( BYTE* msg, ULONG msgLen ) const;
  private:
};

#endif
