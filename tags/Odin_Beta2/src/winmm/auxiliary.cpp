/* $Id: auxiliary.cpp,v 1.4 2002-05-22 15:50:24 sandervl Exp $ */

/*
 * Auxilary multimedia apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
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
#include <mmddk.h>
#include <odinwrap.h>
#include <string.h>

#include <misc.h>
#include <unicode.h>

#include "winmm.h"
#include "auxiliary.h"

#define DBG_LOCALLOG	DBG_auxiliary
#include "dbglocal.h"

/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI auxGetDevCapsA(UINT uDeviceID, LPAUXCAPSA pac, UINT cbac)
{
  if(uDeviceID == AUX_MAPPER) {//AUX mapper
        return(MMSYSERR_NODRIVER);
  }
  if(uDeviceID != 0) {
        return(MMSYSERR_BADDEVICEID);
  }
  if(auxOS2Open() == FALSE) {
        return(MMSYSERR_NODRIVER);
  }
  pac->wMid           = 0;
  pac->wPid           = 0;
  pac->vDriverVersion = 0x0001;
  strcpy(pac->szPname, "OS/2 Aux Device");
  pac->wTechnology    = AUXCAPS_AUXIN;
  pac->wReserved1     = 0;
  pac->dwSupport      = AUXCAPS_LRVOLUME | AUXCAPS_VOLUME;
  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI auxGetDevCapsW(UINT uDeviceID, LPAUXCAPSW pac, UINT cbac)
{
  if(uDeviceID == AUX_MAPPER) {//AUX mapper
        return(MMSYSERR_NODRIVER);
  }
  if(uDeviceID != 0) {
        return(MMSYSERR_BADDEVICEID);
  }
  if(auxOS2Open() == FALSE) {
        return(MMSYSERR_NODRIVER);
  }
  pac->wMid           = 0;
  pac->wPid           = 0;
  pac->vDriverVersion = 0x0001;
  AsciiToUnicode("OS/2 Aux Device", pac->szPname);
  pac->wTechnology    = AUXCAPS_AUXIN;
  pac->wReserved1     = 0;
  pac->dwSupport      = AUXCAPS_LRVOLUME | AUXCAPS_VOLUME;
  return MMSYSERR_NOERROR;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI auxSetVolume(UINT uDeviceID, DWORD dwVolume)
{
  if(uDeviceID == AUX_MAPPER) {//AUX mapper
        return(MMSYSERR_NODRIVER);
  }
  if(uDeviceID != 0) {
        return(MMSYSERR_BADDEVICEID);
  }
  if(auxOS2Open() == FALSE) {
        return(MMSYSERR_NODRIVER);
  }
  return auxOS2SetVolume(dwVolume);
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI auxGetVolume(UINT uDeviceID, LPDWORD pdwVolume)
{
  if(uDeviceID == AUX_MAPPER) {//AUX mapper
        return(MMSYSERR_NODRIVER);
  }
  if(uDeviceID != 0) {
        return(MMSYSERR_BADDEVICEID);
  }
  if(auxOS2Open() == FALSE) {
        return(MMSYSERR_NODRIVER);
  }
  return auxOS2GetVolume(pdwVolume);
}
/******************************************************************************/
/******************************************************************************/
UINT WINAPI auxGetNumDevs()
{
  if(auxOS2Open() == FALSE)
    return(0);
  else
    return 1;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WINAPI auxOutMessage(UINT uDeviceID, UINT uMsg, DWORD dwParam1, 
                              DWORD dwParam2)
{
  if(uDeviceID == AUX_MAPPER) {//AUX mapper
        return(MMSYSERR_NODRIVER);
  }
  if(uDeviceID != 0) {
        return(MMSYSERR_BADDEVICEID);
  }
  if(auxOS2Open() == FALSE) {
        return(MMSYSERR_NODRIVER);
  }
  switch(uMsg) {
        case DRVM_INIT:
        case DRVM_EXIT:
                return MMSYSERR_NOERROR;
        case AUXDM_GETNUMDEVS:
                return auxGetNumDevs();

        case AUXDM_GETDEVCAPS:
                return auxGetDevCapsA(uDeviceID, (AUXCAPSA *)dwParam1, dwParam2);

        case AUXDM_GETVOLUME:
                return auxGetVolume(uDeviceID, (DWORD *)dwParam1);

        case AUXDM_SETVOLUME:
                return auxSetVolume(uDeviceID, dwParam1);
  }
  return MMSYSERR_NOTSUPPORTED;
}
/******************************************************************************/
/******************************************************************************/

