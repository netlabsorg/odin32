/* $Id: joyos2.cpp,v 1.1 1999-06-29 15:55:17 sandervl Exp $ */
/*
 * OS/2 Joystick apis
 *
 * Copyright 1999 Przemysław Dobrowolski <dobrawka@asua.org.pl>
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_BASE
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <misc.h>
#include "joyos2.h"

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
LONG JoyOpen(HFILE *phGame)
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
LONG JoyGetParams(HFILE hGame, GAME_PARM_STRUCT *pGameParams)
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
LONG JoyCalibrate(HFILE hGame, GAME_CALIB_STRUCT  gameCalib)
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
LONG JoyGetStatus( HFILE hGame, GAME_STATUS_STRUCT  *pGameStatus )
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
LONG JoyClose( HFILE hGame )
{
  APIRET  rc;

  rc = DosClose( hGame );

  return (rc);
}
//******************************************************************************
//******************************************************************************
BOOL JoyInstalled(USHORT wID)
{
  BOOL             flReturn=FALSE;
  HFILE            hJoy;
  APIRET           rc;
  GAME_PARM_STRUCT GameParams;

  rc=JoyOpen(&hJoy);
  if ( rc==0 )
  {
    JoyGetParams(hJoy,&GameParams);
    JoyClose(hJoy);

    if (( wID == 0 ) && (GameParams.useA > 0))
      flReturn=TRUE;

    if (( wID == 1 ) && (GameParams.useB > 0))
      flReturn=TRUE;
  }
  return (flReturn);
}

