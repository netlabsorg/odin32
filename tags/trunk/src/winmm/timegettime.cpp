/* $Id: timegettime.cpp,v 1.3 1999-08-19 18:46:05 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <odinwrap.h>


ODINDEBUGCHANNEL(WINMM-TIMEGETTIME)

/******************************************************************************/
/******************************************************************************/
ODINFUNCTION0(DWORD, timeGetTime)
{
  return O32_timeGetTime();
}
/******************************************************************************/
/******************************************************************************/

