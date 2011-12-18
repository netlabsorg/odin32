/* $Id: mixerdata.cpp,v 1.4 2002-05-26 10:52:31 sandervl Exp $ */

/*
 * Mixer multimedia
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include "mixer.h"


/******************************************************************************/
/******************************************************************************/
const char *szCtrlName[MIX_CTRL_MAX][2] = {
    { 
        "MasterVol",
        "Master Volume Control for LineOut"
    },
    {
        "MasterMute",
        "Master Mute Control for LineOut"
    },
    { 
        "MonoInVol",
        "Volume Control for Mono In"
    },
    { 
        "MonoInMute",
        "Mute Control for Mono In"
    },
    { 
        "PhoneVol",
        "Volume Control for Telephone"
    },
    { 
        "PhoneMute",
        "Mute Control for Telephone"
    },
    { 
        "MicVol",
        "Volume Control for Microphone"
    },
    { 
        "MicMute",
        "Mute Control for Microphone"
    },
    { 
        "MicBoost",
        "Microphone Boost Control"
    },
    { 
        "LineInVol",
        "Volume Control for Line In"
    },
    { 
        "LineInMute",
        "Mute Control for Line In"
    },
    { 
        "CDVol",
        "Volume Control for CD"
    },
    { 
        "CDMute",
        "Mute Control for CD"
    },
    { 
        "SPDIFInVol",
        "Volume Control for SPDIF In"
    },
    { 
        "SPDIFInMute",
        "Mute Control for SPDIF In"
    },
    { 
        "SPDIFOutVol",
        "Master Volume Control for SPDIF Out"
    },
    { 
        "SPDIFOutMute",
        "Master Mute Control for SPDIF Out"
    },
    { 
        "VideoVol",
        "Volume Control for Video"
    },
    { 
        "VideoMute",
        "Mute Control for Video"
    },
    { 
        "AuxVol",
        "Volume Control for Aux"
    },
    { 
        "AuxMute",
        "Mute Control for Aux"
    },
    { 
        "WaveOutVol",
        "Volume Control for WaveOut"
    },
    { 
        "WaveOutMute",
        "Mute Control for WaveOut"
    },
    { 
        "WavetableVol",
        "Volume Control for Wavetable MIDI"
    },
    { 
        "WavetableMute",
        "Mute Control for Wavetable MIDI"
    },
    { 
        "MIDIVol",
        "Volume Control for MIDI"
    },
    { 
        "MIDIMute",
        "Mute Control for MIDI"
    },
    { 
        "3D Center",
        "3D Center Control for LineOut"
    },
    { 
        "3D Depth",
        "3D Depth Control for LineOut"
    },
    { 
        "Treble",
        "Treble Control"
    },
    { 
        "Bass",
        "Bass Control"
    },
    { 
        "WaveInMux",
        "Mux Control for Recording"
    },
    { 
        "MonoRecVol",
        "Volume Control for Mono Recording"
    },
    { 
        "PhoneRecVol",
        "Volume Control for Telephone Recording"
    },
    { 
        "MicRecVol",
        "Volume Control for Microphone Recording"
    },
    { 
        "LineRecVol",
        "Volume Control for Line Recording"
    },
    { 
        "CDRecVol",
        "Volume Control for CD Recording"
    },
    { 
        "SPDIFRecVol",
        "Volume Control for SPDIF Recording"
    },
    { 
        "VideoRecVol",
        "Volume Control for Video Recording"
    },
    { 
        "AuxRecVol",
        "Volume Control for Aux Recording"
    },
    { 
        "WaveRecVol",
        "Volume Control for WaveOut Recording"
    },
    { 
        "WTRecVol",
        "Volume Control for Wavetable MIDI Recording"
    },
    { 
        "MIDIVol",
        "Volume Control for MIDI Recording"
    },
    {
        "StereoMixVol",
        "Volume Control for Stereo Mixer Recording",
    },
    {
        "MonoMixVol",
        "Volume Control for Mono Mixer Recording",
    },
};
/******************************************************************************/
/******************************************************************************/
const char *szDestName[MIXER_DEST_MAX][2] = {
  {
      "Volume Control",
      "Volume Control",
  },
  {
      "RecSource",
      "Recording Source",
  },
  {
      "SPDIF",
      "SPDIF Control",
  }
};
/******************************************************************************/
/******************************************************************************/
const char *szSourceName[MIXER_SRC_MAX][2] = {
  {
      "MonoIn",
      "Mono In",
  },
  {
      "Phone",
      "Telephone",
  },
  {
      "Mic",
      "Microphone",
  },
  {
      "Line",
      "Line",
  },
  {
      "CD",
      "CD-ROM",
  },
  {
      "SPDIF",
      "SPDIF",
  },
  {
      "Video",
      "Video",
  },
  {
      "Aux",
      "Auxiliary",
  },
  {
      "Wave",
      "Wave Out",
  },
  {
      "Wavetable",
      "Wavetable Synthesizer",
  },
  {
      "MIDI",
      "MIDI Synthesizer",
  },
  {
      "MonoIn",
      "Mono In",
  },
  {
      "Phone",
      "Telephone",
  },
  {
      "Mic",
      "Microphone",
  },
  {
      "Line",
      "Line",
  },
  {
      "CD",
      "CD-ROM",
  },
  {
      "SPDIF",
      "SPDIF",
  },
  {
      "Video",
      "Video",
  },
  {
      "Aux",
      "Auxiliary",
  },
  {
      "Wave",
      "Wave Out",
  },
  {
      "Wavetable",
      "Wavetable Synthesizer",
  },
  {
      "MIDI",
      "MIDI Synthesizer",
  },
  {
      "StereoMix",
      "Stereo Mixer",
  },
  {
      "MonoMix",
      "Mono Mixer",
  }
};
/******************************************************************************/
/******************************************************************************/

