/* $Id: joy.cpp,v 1.7 2000-02-17 14:09:30 sandervl Exp $ */
/*
 * Odin Joystick apis
 *
 * Copyright 1999 Przemyslaw Dobrowolski <dobrawka@asua.org.pl>
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
#include <misc.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>

#include "winmm.h"
#include "os2timer.h"
#include "joyos2.h"

#define DBG_LOCALLOG	DBG_joy
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-JOY)


#define MAXJOYDRIVERS 2

static INT  aJoyThreshold[MAXJOYDRIVERS] = {0,0};
static BOOL aJoyCaptured[MAXJOYDRIVERS]  = {FALSE,FALSE};

void _Optlink joySendMessages(void *pData);

LONG JoyGetPos(HANDLE hGame, UINT wID, LPJOYINFO lpInfo)
{
  GAME_STATUS_STRUCT gs;

  if (wID >= MAXJOYDRIVERS) return (JOYERR_PARMS);

  if (hGame==0) return (JOYERR_NOCANDO);

  JoyGetStatus( hGame, &gs );

  if (wID==JOYSTICKID1)
  {
    if (gs.curdata.A.x < 1024)
      lpInfo->wXpos    = gs.curdata.A.x * 64;
    else
      lpInfo->wXpos    = 65535; // _NOT_ 65536!!! ;-)

    if (gs.curdata.A.y < 1024)
      lpInfo->wYpos    = gs.curdata.A.y * 64;
    else
      lpInfo->wYpos    = 65535; // _NOT_ 65536!!! ;-)

    lpInfo->wZpos    = 0;
    lpInfo->wButtons = 0;
    if ( (gs.curdata.butMask & JOY_BUT1_BIT) == 0 )
      lpInfo->wButtons |= JOY_BUTTON1;

    if ( (gs.curdata.butMask & JOY_BUT2_BIT) == 0 )
      lpInfo->wButtons |= JOY_BUTTON2;
  }

  if (wID==JOYSTICKID2)
  {
    if (gs.curdata.B.x < 1024)
      lpInfo->wXpos    = gs.curdata.A.x * 64;
    else
      lpInfo->wXpos    = 65535; // _NOT_ 65536!!!

    if (gs.curdata.B.y < 1024)
      lpInfo->wYpos    = gs.curdata.A.x * 64;
    else
      lpInfo->wYpos    = 65535; // _NOT_ 65536!!!

    lpInfo->wZpos    = 0;
    lpInfo->wButtons = 0;
    if ( (gs.curdata.butMask & JOY_BUT3_BIT) == 0 )
      lpInfo->wButtons |= JOY_BUTTON1;

    if ( (gs.curdata.butMask & JOY_BUT4_BIT) == 0 )
      lpInfo->wButtons |= JOY_BUTTON2;
  }

  return (0);
}

/*****************************************************************************
ODINFUNCTION0(*, :
ODINFUNCTION0(INT, joyGetNumDevs* Purpose   : Get number of installed joysticks
 * Status    : Done
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 10:00]
 *****************************************************************************/
ODINFUNCTION0(UINT, joyGetNumDevs)
{
  HANDLE            hJoy;
  GAME_PARM_STRUCT  gameInfo;
  WORD              rc;
  INT               joy_count = 0;

  rc=JoyOpen(&hJoy);
  if ( rc==0 )
  {
    JoyGetParams(hJoy,&gameInfo);
    JoyClose(hJoy);
    if ((gameInfo.useA == FALSE) && (gameInfo.useB == FALSE))
        joy_count = 0;
    else if (gameInfo.useA == FALSE)
        joy_count = 1;
    else if (gameInfo.useB == FALSE)
        joy_count = 1;
    else
        joy_count = 2;

    dprintf(("WINMM:joyGetNumDevs = %d\n",joy_count));
  }
  else
    // No GAMEDD$ driver
    dprintf(("WINMM: joyGetNumDevs - No GAMEDD$ driver\n"));

  return(joy_count);
}

/*****************************************************************************
ODINFUNCTION1(*, :,
ODINFUNCTION1(UINT,, *,
              Purpose, :Get Joystick capatibities (Unicode)
 * Status    : Done
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:40]
 *****************************************************************************/
ODINFUNCTION3(MMRESULT, joyGetDevCapsW,
              UINT, wID,
              LPJOYCAPSW, lpCaps,
              UINT, wSize)
{
    if (wID >= MAXJOYDRIVERS) return JOYERR_PARMS;

    if (JoyInstalled(wID) == 0)
    {
      lpCaps->wMid = MM_MICROSOFT;
      lpCaps->wPid = MM_PC_JOYSTICK;
      AsciiToUnicode("OS/2 Joystick", lpCaps->szPname);
      lpCaps->wXmin = 0;
      lpCaps->wXmax = 0xffff;
      lpCaps->wYmin = 0;
      lpCaps->wYmax = 0xffff;
      // OS/2 Joystick unknown Z-axes
      lpCaps->wZmin = 0;
      lpCaps->wZmax = 0;
      lpCaps->wNumButtons = 2; // FIXME: OS/2 can use Joysticks with 4 buttons?
      lpCaps->wPeriodMin = 10;
      lpCaps->wPeriodMax = 1000;
      lpCaps->wRmin = 0;
      lpCaps->wRmax = 0;
      lpCaps->wUmin = 0;
      lpCaps->wUmax = 0;
      lpCaps->wVmin = 0;
      lpCaps->wVmax = 0;
      lpCaps->wCaps = 0;
      lpCaps->wMaxAxes = 4;
      lpCaps->wNumAxes = 2;
      lpCaps->wMaxButtons = 2;
      AsciiToUnicode("",lpCaps->szRegKey);
      AsciiToUnicode("",lpCaps->szOEMVxD);
      dprintf(("OK!!!\n"));
      return JOYERR_NOERROR;
    }
    else
      return MMSYSERR_NODRIVER;
}

/*****************************************************************************
ODINFUNCTION1(*, :,
ODINFUNCTION1(UINT,, *,
              Purpose, :Get Joystick capatibities (Unicode)
 * Status    : Done
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION3(MMRESULT, joyGetDevCapsA,
              UINT, wID,
              LPJOYCAPSA, lpCaps,
              UINT, wSize)
{
    if (wID >= MAXJOYDRIVERS) return JOYERR_PARMS;

    if (JoyInstalled(wID) == 0)
    {
      lpCaps->wMid = MM_MICROSOFT;
      lpCaps->wPid = MM_PC_JOYSTICK;
      strcpy(lpCaps->szPname, "OS/2 Joystick"); /* joystick product name */
      lpCaps->wXmin = 0;
      lpCaps->wXmax = 0xffff;
      lpCaps->wYmin = 0;
      lpCaps->wYmax = 0xffff;
      // OS/2 Joystick unknown Z-axes
      lpCaps->wZmin = 0;
      lpCaps->wZmax = 0;
      lpCaps->wNumButtons = 2; // FIXME: OS/2 can use Joysticks with 4 buttons?
      lpCaps->wPeriodMin = 10;
      lpCaps->wPeriodMax = 1000;
      lpCaps->wRmin = 0;
      lpCaps->wRmax = 0;
      lpCaps->wUmin = 0;
      lpCaps->wUmax = 0;
      lpCaps->wVmin = 0;
      lpCaps->wVmax = 0;
      lpCaps->wCaps = 0;
      lpCaps->wMaxAxes = 4;
      lpCaps->wNumAxes = 2;
      lpCaps->wMaxButtons = 2;
      strcpy(lpCaps->szRegKey,"");
      strcpy(lpCaps->szOEMVxD,"");
      return JOYERR_NOERROR;
    }
    else
      return MMSYSERR_NODRIVER;
}

/*****************************************************************************
ODINFUNCTION1(*, :,
ODINFUNCTION1(MMRESULT,, *,
              Purpose, :Get the extended actual joystick position
 * Status    : Done (but not all functions are functionally but Quake2
 *             running with this function)
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 23:42]
 *****************************************************************************/
ODINFUNCTION2(MMRESULT, joyGetPosEx,
              UINT, uJoyID,
              LPJOYINFOEX, pji)
{
  JOYINFO            ji;
  HANDLE             hGamePort;
  GAME_CALIB_STRUCT  gc;
  DWORD              rc;

  if (uJoyID >= MAXJOYDRIVERS) return JOYERR_PARMS;

  rc=JoyInstalled(uJoyID);

  if (rc) return (rc);

  rc=JoyOpen(&hGamePort);

  if (rc) return (MMSYSERR_NODRIVER);

  JoyGetPos(hGamePort,uJoyID,&ji);
  JoyGetCalValues(hGamePort,&gc);

  JoyClose(hGamePort);

  if (pji->dwSize>11) pji->dwXpos         =  0;
  if (pji->dwSize>15) pji->dwYpos         =  0;
  if (pji->dwSize>19) pji->dwZpos         =  0;// not supported for OS/2 driver!
  if (pji->dwSize>23) pji->dwRpos         =  0;// not supported for OS/2 driver!
  if (pji->dwSize>27) pji->dwUpos         =  0;// not supported for OS/2 driver!
  if (pji->dwSize>31) pji->dwVpos         =  0;// not supported for OS/2 driver!
  if (pji->dwSize>35) pji->dwButtons      =  0;
  if (pji->dwSize>35) pji->dwButtonNumber =  0;
  if (pji->dwSize>43) pji->dwPOV          = -1;// FIXME: Win98 returns that code!!! Wrong?
  if (pji->dwSize>47) pji->dwReserved1    =  0;// FIXME!
  if (pji->dwSize>51) pji->dwReserved2    =  0;// FIXME!

  if (pji->dwFlags & JOY_RETURNCENTERED)
  {
    if (uJoyID==JOYSTICKID1)
    {
      if (pji->dwSize>11)
        if (gc.Ax.centre < 1024)
          pji->dwXpos    = gc.Ax.centre * 64;
        else
          pji->dwXpos    = 65535; // _NOT_ 65536!!! ;-)

      if (pji->dwSize>15)
        if (gc.Ay.centre < 1024)
          pji->dwYpos    = gc.Ay.centre * 64;
        else
          pji->dwYpos    = 65535; // _NOT_ 65536!!! ;-)
    }
    if (uJoyID==JOYSTICKID2)
    {
      if (pji->dwSize>11)
        if (gc.Ax.centre < 1024)
          pji->dwXpos    = gc.Bx.centre * 64;
        else
          pji->dwXpos    = 65535; // _NOT_ 65536!!! ;-)

      if (pji->dwSize>15)
        if (gc.Ay.centre < 1024)
          pji->dwYpos    = gc.By.centre * 64;
        else
          pji->dwYpos    = 65535; // _NOT_ 65536!!! ;-)
    }
  }

  if (pji->dwFlags & JOY_RETURNBUTTONS)
  {
    if (pji->dwSize>35) pji->dwButtons      =  ji.wButtons;
    if (pji->dwSize>35) pji->dwButtonNumber = (ji.wButtons & 2) ? 2 : 1;
  }

  if (pji->dwFlags & JOY_RETURNX)
   if (pji->dwSize>11) pji->dwXpos         =  ji.wXpos;

  if (pji->dwFlags & JOY_RETURNY)
   if (pji->dwSize>11) pji->dwYpos         =  ji.wXpos;

  return JOYERR_NOERROR;
}

/*****************************************************************************
ODINFUNCTION2(*, :,
ODINFUNCTION6(MMRESULT,, joyGetPos,
              *,, Purpose:,
              Get, the,
              actual, joystick,
              position *, Status,
              :, Done,
              * *, Author: Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION2(MMRESULT, joyGetPos,
              UINT, uJoyID,
              LPJOYINFO, pji)
{
  HANDLE   hGame;
  MMRESULT rc;
  if (uJoyID >= MAXJOYDRIVERS) return JOYERR_PARMS;

  rc=JoyInstalled(uJoyID);

  if (rc) return (rc);

  if (JoyOpen(&hGame)) return (MMSYSERR_NODRIVER);

  rc=JoyGetPos(hGame,uJoyID,pji);

  JoyClose(hGame);

  return JOYERR_NOERROR;
}
/*****************************************************************************
ODINFUNCTION2(*, :,
ODINFUNCTION2(MMRESULT,, joyGetThreshold,
              *,, Status:,
              Done *, *Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION2(MMRESULT, joyGetThreshold,
              UINT, wID,
              LPUINT, lpThreshold)
{
    dprintf(("WINMM:joyGetThreshold %d %X\n",wID, lpThreshold));

    if (wID >= MAXJOYDRIVERS) return JOYERR_PARMS;

    *lpThreshold = aJoyThreshold[wID];

    return JOYERR_NOERROR;
}

/*****************************************************************************
ODINFUNCTION2(*, :,
ODINFUNCTION2(MMRESULT,, joySetThreshold,
              *,, Status:,
              Done *, *Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION2(MMRESULT, joySetThreshold,
              UINT, wID,
              UINT, wThreshold)
{
   if (wID >= MAXJOYDRIVERS) return JOYERR_PARMS;

   aJoyThreshold[wID] = wThreshold;

   return JOYERR_NOERROR;
}

typedef struct _JOYTHREADOPT
{
  HWND   hWnd;
  UINT   wID;
  UINT   wPeriod;
  BOOL   bChanged;
} JOYTHREADOPT;
typedef JOYTHREADOPT *PJOYTHREADOPT;

/*****************************************************************************
ODINFUNCTION4(*, :,
ODINFUNCTION4(MMRESULT,, joySetCapture,
              *,, Purpose,
              :,, Start,
              joystick,, movescapturing,
              *, Status: Done
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION4(MMRESULT, joySetCapture,
              HWND, hWnd,
              UINT, wID,
              UINT, wPeriod,
              BOOL, bChanged)
{
   JOYTHREADOPT *newthr;
   INT          iThreadId;
   DWORD        rc;

   if (wID >= MAXJOYDRIVERS) return JOYERR_PARMS;

   rc=JoyInstalled(wID);
   if (rc != JOYERR_NOERROR) return (rc);

   newthr=(PJOYTHREADOPT)malloc(sizeof(JOYTHREADOPT));
   newthr->hWnd     = hWnd;
   newthr->wID      = wID;
   newthr->wPeriod  = wPeriod;
   newthr->bChanged = bChanged;
   aJoyCaptured[wID] = TRUE;
   iThreadId = _beginthread(joySendMessages, NULL, 0x4000, (void *)newthr);
   if (iThreadId!=0)
   {
     dprintf(("WINMM:joySetCapture ThreadID = %d\n", iThreadId));
     return JOYERR_NOERROR;
   }
   else
     dprintf(("WINMM:joySetCapture Thread not created!!!!\n"));

   return JOYERR_NOCANDO; /* FIXME: what should be returned ? */
}

/*****************************************************************************
ODINFUNCTION1(*, :,
ODINFUNCTION1(MMRESULT,, *,
              Purpose, :Stop capturing joystick moves
 * Status    : Done (but must be rewriting)
 *
 * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION1(MMRESULT, joyReleaseCapture,
              UINT, wID)
{
  // TODO: Semaphores or waiting for thread...
  if (wID >= MAXJOYDRIVERS) return JOYERR_PARMS;

  if (aJoyCaptured[wID] != TRUE) return JOYERR_NOCANDO;

  aJoyCaptured[wID] = FALSE;

  return JOYERR_NOERROR;
}

/*****************************************************************************
ODINFUNCTION1(*, :,
ODINFUNCTION4(MMRESULT,, *,
              Status, :,
              Stub, but,
              done, ;-,
              * *, Author)
: Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
 *****************************************************************************/
ODINFUNCTION1(MMRESULT, joyConfigChanged,
              DWORD, dwFlags)
{
  return JOYERR_NOERROR;
}


// ***************************************************************************
// * [Internal]
// * Name      : _Optlink joySendMessages
// * Purpose   : Capturing thread
// * Status    : Done
// *
// * Author    : Przemyslaw Dobrowolski [Tue, 1999/06/29 09:00]
// ***************************************************************************
void _Optlink joySendMessages(void *pData)
{
  PJOYTHREADOPT      opt = (PJOYTHREADOPT) pData;
  JOYINFO            ji;
  JOYINFO            jiOld;
  HANDLE             hGame;
  DWORD               wTest;
  INT                rc;

  rc=JoyOpen(&hGame);

  if (rc) aJoyCaptured[opt->wID]=FALSE;

  rc=JoyGetPos( hGame, opt->wID, &ji );

  if (rc) aJoyCaptured[opt->wID]=FALSE;

  jiOld=ji;

  while (aJoyCaptured[opt->wID])
  {
     rc=JoyGetPos( hGame, opt->wID, &ji );

     if (rc) aJoyCaptured[opt->wID]=FALSE;

     wTest=MAKELONG(ji.wXpos, ji.wYpos);
     if ((ji.wXpos!=jiOld.wXpos) || (ji.wYpos!=jiOld.wXpos))
       PostMessageA(opt->hWnd, MM_JOY1MOVE + opt->wID, ji.wButtons, wTest);

     if (ji.wButtons != jiOld.wButtons)
     { UINT ButtonChanged = (ji.wButtons ^ jiOld.wButtons)<<8;

       if (jiOld.wButtons < ji.wButtons)
          PostMessageA(opt->hWnd, MM_JOY1BUTTONDOWN + opt->wID, ButtonChanged, MAKELONG(ji.wXpos, ji.wYpos));
       else if (jiOld.wButtons > ji.wButtons)
          PostMessageA(opt->hWnd, MM_JOY1BUTTONUP + opt->wID, ButtonChanged, MAKELONG(ji.wXpos, ji.wYpos));

     }
     Sleep(opt->wPeriod+1); // Fixme!!!
     jiOld=ji;
  }
  free(opt);
  dprintf(("WINMM:Joystick-internal->joySendMessages ended!\n"));
  // Huh... We must close thread ;-)
  _endthread();
}

