/* $Id: joyos2.cpp,v 1.5 2003-04-02 12:59:23 sandervl Exp $ */
/*
 * OS/2 Joystick apis
 *
 * Copyright 1999 Przemysˆaw Dobrowolski <dobrawka@asua.org.pl>
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#define  INCL_BASE
#include <os2wrap.h>     //Odin32 OS/2 api wrappers
#include <misc.h>
#include "joyos2.h"

#define DBG_LOCALLOG	DBG_joyos2
#include "dbglocal.h"

#define GAMEPDDNAME     "GAME$"

#define IOCTL_JOYSTICK                  0x80 // IOCTL's

// IOCTL wraper functions
#define GAME_GET_PARMS                  0x02
#define GAME_GET_CALIB                  0x04
#define GAME_SET_CALIB                  0x05
#define GAME_GET_STATUS                 0x10

#define MAXJOYDRIVERS	2 // GAME$ knows only 2 joysticks

//******************************************************************************
//******************************************************************************
LONG JoyOpen(HANDLE *phGame)
{
  ULONG   action;
  APIRET  rc;

  if (phGame==NULL)
    return -1;

  rc = DosOpen( GAMEPDDNAME,
                phGame,
                &action,
                0,
                FILE_READONLY,
                FILE_OPEN,
                OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE,
                NULL );

  return  (rc);
}
//******************************************************************************
//******************************************************************************
LONG JoyGetParams(HANDLE hGame, GAME_PARM_STRUCT *pGameParams)
{
  ULONG             dataLen;
  APIRET            rc;

  dataLen = sizeof( *pGameParams );
  rc = DosDevIOCtl( hGame,
                    IOCTL_JOYSTICK,
                    GAME_GET_PARMS,                 // 0x80, 0x02
                    NULL,
                    0,
                    NULL,
                    pGameParams,
                    dataLen,
                    &dataLen);
  return (rc);
}

//******************************************************************************
//******************************************************************************
// For each stick:
//    tell user to centre joystick and press button
//    call get status with wait
//    tell user to move to upper left and press button
//    call get status with wait
//    tell user to move to lower right and press button
//    call get status with wait
// Then call set calibration IOCTL with these values
LONG JoyCalibrate(HANDLE hGame, GAME_CALIB_STRUCT  gameCalib)
{
  ULONG              parmLen;
  APIRET             rc;

  parmLen = sizeof( gameCalib );
  rc = DosDevIOCtl( hGame,
                    IOCTL_JOYSTICK,
                    GAME_SET_CALIB,                     // 0x80, 0x05
                    &gameCalib,
                    parmLen,
                    &parmLen,
                    NULL,
                    0,
                    NULL );
  return (rc);
}

//******************************************************************************
//******************************************************************************
LONG JoyGetStatus( HANDLE hGame, GAME_STATUS_STRUCT  *pGameStatus )
{
  ULONG               dataLen;
  APIRET              rc;

  dataLen = sizeof( *pGameStatus );
  rc = DosDevIOCtl( hGame,
                    IOCTL_JOYSTICK,
                    GAME_GET_STATUS,                // 0x80, 0x10
                    NULL,
                    0,
                    NULL,
                    pGameStatus,
                    dataLen,
                    &dataLen );
  return (rc);
}
//******************************************************************************
//******************************************************************************
LONG JoyClose( HANDLE hGame )
{
  APIRET  rc;

  rc = DosClose( hGame );

  return (rc);
}
//******************************************************************************
//******************************************************************************
LONG JoyGetCalValues( HANDLE hGame, GAME_CALIB_STRUCT  *pGameCalib )
{
  ULONG              dataLen;
  APIRET             rc;

  dataLen = sizeof( *pGameCalib );
  rc = DosDevIOCtl( hGame,
                    IOCTL_JOYSTICK,
                    GAME_GET_CALIB,                 // 0x80, 0x04
                    NULL,
                    0,
                    NULL,
                    pGameCalib,
                    dataLen,
                    &dataLen);

  return (rc);
}
//******************************************************************************
//******************************************************************************
LONG JoyInstalled(USHORT wID)
{
  BOOL             flReturn=1; // MMSYSERR_NODRIVER
  HANDLE            hJoy;
  APIRET           rc;
  GAME_PARM_STRUCT GameParams;


  rc=JoyOpen(&hJoy);
  if ( rc==0 )
  {
    JoyGetParams(hJoy,&GameParams);
    JoyClose(hJoy);

    if (( wID == 0 ) && (GameParams.useA > 0))
      flReturn=0; //
    else
      if (( wID == 1 ) && (GameParams.useB > 0))
        flReturn=0;
      else
        flReturn=167; // JOYERR_UNPLUGGED
  }
  return (flReturn);
}


