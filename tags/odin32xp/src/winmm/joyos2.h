/* $Id: joyos2.h,v 1.3 2003-04-02 12:59:24 sandervl Exp $ */
/*
 * Odin Joystick apis
 *
 * Copyright 1999 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __JOYOS2_H__
#define __JOYOS2_H__

// bit masks for each button
#define JOY_BUT1_BIT    0x10
#define JOY_BUT2_BIT    0x20
#define JOY_BUT3_BIT    0x40
#define JOY_BUT4_BIT    0x80
#define JOY_ALL_BUTS    (JOY_BUT1_BIT|JOY_BUT2_BIT|JOY_BUT3_BIT|JOY_BUT4_BIT)

typedef SHORT  GAME_POS;

typedef struct
{
  GAME_POS x;
  GAME_POS y;
} GAME_2DPOS_STRUCT;

typedef struct
{
  GAME_2DPOS_STRUCT A;       // Joy1
  GAME_2DPOS_STRUCT B;       // Joy2
  USHORT            butMask; // Buttons
} GAME_DATA_STRUCT;

typedef struct
{
  GAME_POS lower;
  GAME_POS centre;
  GAME_POS upper;
} GAME_3POS_STRUCT;

typedef struct
{
  GAME_DATA_STRUCT        curdata;
  USHORT                  b1cnt;
  USHORT                  b2cnt;
  USHORT                  b3cnt;
  USHORT                  b4cnt;
} GAME_STATUS_STRUCT;

typedef struct
{
  USHORT useA;
  USHORT useB;
  USHORT mode;
  USHORT format;
  USHORT sampDiv;
  USHORT scale;
  USHORT res1;
  USHORT res2;
} GAME_PARM_STRUCT;

typedef struct
{
  GAME_3POS_STRUCT Ax;
  GAME_3POS_STRUCT Ay;
  GAME_3POS_STRUCT Bx;
  GAME_3POS_STRUCT By;
} GAME_CALIB_STRUCT;

typedef struct
{
  GAME_3POS_STRUCT Ax;
  GAME_3POS_STRUCT Ay;
  GAME_3POS_STRUCT Bx;
  GAME_3POS_STRUCT By;
} GAME_DIGSET_STRUCT;

LONG JoyOpen( HANDLE *phGame );
LONG JoyGetParams( HANDLE hGame, GAME_PARM_STRUCT *pGameParams );
LONG JoyCalibrate( HANDLE hGame, GAME_CALIB_STRUCT  gameCalib );
LONG JoyGetCalValues( HANDLE hGame, GAME_CALIB_STRUCT *pGameCalib );
LONG JoyGetStatus( HANDLE hGame, GAME_STATUS_STRUCT  *pGameStatus );
LONG JoyClose( HANDLE hGame );
LONG JoyInstalled(USHORT wID);

#endif /* __JOYOS2_H__ */