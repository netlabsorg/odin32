/*
 * Auxilary multimedia apis
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>

#include <string.h>

#include <misc.h>
#include <unicode.h>

#include "winmm.h"
#include "aux.h"

/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API auxGetDevCapsA(UINT uDeviceID, LPAUXCAPSA pac, UINT cbac)
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
MMRESULT WIN32API auxGetDevCapsW(UINT uDeviceID, LPAUXCAPSW pac, UINT cbac)
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
MMRESULT WIN32API auxSetVolume(UINT uDeviceID, DWORD dwVolume)
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
MMRESULT WIN32API auxGetVolume(UINT uDeviceID, LPDWORD pdwVolume)
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
UINT WIN32API auxGetNumDevs(void)
{
  if(auxOS2Open() == FALSE) {
        return(0);
  }
  return 1;
}
/******************************************************************************/
/******************************************************************************/
MMRESULT WIN32API auxOutMessage(UINT uDeviceID, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
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
