/* $Id: timegettime.cpp,v 1.2 1999-06-10 16:24:34 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>

/******************************************************************************/
/******************************************************************************/
DWORD WIN32API timeGetTime(void)
{
#ifdef DEBUG
////    WriteLog("WINMM:timeGetTime\n");
#endif
    return O32_timeGetTime();
}
/******************************************************************************/
/******************************************************************************/
