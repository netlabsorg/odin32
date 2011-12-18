/* $Id: auxos2.cpp,v 1.9 2001-10-24 22:47:41 sandervl Exp $ */

/*
 * Auxilary multimedia OS/2 implementation
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

#define  INCL_BASE
#define  INCL_OS2MM
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <os2mewrap.h>  //Odin32 OS/2 MMPM/2 api wrappers
#include <string.h>

#define  OS2_ONLY
#include "win32type.h"
#include "misc.h"
#include "unicode.h"

#include "auxiliary.h"
#include "initwinmm.h"

#define DBG_LOCALLOG	DBG_auxos2
#include "dbglocal.h"

ULONG auxDeviceId = -1;

/******************************************************************************/
/******************************************************************************/
BOOL auxOS2Open()
{
 MCI_AMP_OPEN_PARMS AmpOpenParms;
 APIRET rc;

  if(fMMPMAvailable == FALSE) return FALSE;

  if(auxDeviceId != -1) {
        return TRUE;
  }
  // Setup the open structure, pass the playlist and tell MCI_OPEN to use it
  memset(&AmpOpenParms,0,sizeof(AmpOpenParms));

  AmpOpenParms.usDeviceID = ( USHORT ) 0;
  AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;

  rc = mymciSendCommand(0, MCI_OPEN,
                      MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                      (PVOID) &AmpOpenParms,
                      0);
  if(rc) {
        dprintf(("auxOpen: MCI_OPEN returned %X\n", rc));
        return(FALSE);
  }
  auxDeviceId = AmpOpenParms.usDeviceID;
  return(TRUE);
}
/******************************************************************************/
/******************************************************************************/
void auxOS2Close()
{
 MCI_GENERIC_PARMS    GenericParms;

   if(auxDeviceId == -1)
        return;

   // Generic parameters
   GenericParms.hwndCallback = 0;       //hwndFrame

   // Close the device
   mymciSendCommand(auxDeviceId, MCI_CLOSE, MCI_WAIT, (PVOID)&GenericParms, 0);
}
/******************************************************************************/
/******************************************************************************/
DWORD auxOS2SetVolume(DWORD dwVolume)
{
 MCI_MASTERAUDIO_PARMS maudio = {0};
 APIRET rc;

  maudio.ulMasterVolume = (dwVolume*100)/65536;  //TODO: Not correct, should be logartihmic
  rc = mymciSendCommand(auxDeviceId, MCI_MASTERAUDIO, MCI_MASTERVOL |
                      MCI_WAIT, (PVOID)&maudio,0);
  if(rc) {
        dprintf(("auxOS2SetVolume returned %X\n", rc));
  }
  return(0); //MMSYSERR_NOERROR
}
/******************************************************************************/
/******************************************************************************/
DWORD auxOS2GetVolume(DWORD *dwVolume)
{
 MCI_MASTERAUDIO_PARMS maudio = {0};
 APIRET rc;

  rc = mymciSendCommand(auxDeviceId, MCI_MASTERAUDIO, MCI_QUERYCURRENTSETTING |
                      MCI_MASTERVOL | MCI_WAIT, (PVOID)&maudio,0);
  if(rc) {
        dprintf(("auxOS2GetVolume returned %X\n", rc));
  }
  *dwVolume = (maudio.ulReturn*65536)/100;
  return(0); //MMSYSERR_NOERROR
}
/******************************************************************************/
/******************************************************************************/

