/* $Id: builtin.cpp,v 1.1 1999-09-01 18:50:32 sandervl Exp $ */
/*
 *      MSACM32 library
 *
 *      Copyright 1998  Patrik Stridvall
 *	Copyright 1999	Jens Wiessner
 */

#include "winbase.h"
#include "winerror.h"
#include "windef.h"
#include "debugtools.h"
#include "driver.h"
#include "mmsystem.h"
#include "msacm.h"
#include "msacmdrv.h"

/***********************************************************************
 *           MSACM_BuiltinDrivers
 */
LONG WINAPI MSACM_DummyDriverProc(
   DWORD dwDriverId, HDRVR hdrvr, UINT msg,
   LONG lParam1, LONG lParam2); 

WINE_ACMBUILTINDRIVER MSACM_BuiltinDrivers[] = {
  { "MSACM.dummy", &MSACM_DummyDriverProc },
  { NULL, NULL }
};

/***********************************************************************
 *           MSACM_DummyDriverProc
 */
LONG WINAPI MSACM_DummyDriverProc(
   DWORD dwDriverId, HDRVR hdrvr, UINT msg,
   LONG lParam1, LONG lParam2)
{
  switch(msg)
    {
    case DRV_LOAD:
    case DRV_ENABLE:
    case DRV_OPEN:
    case DRV_CLOSE:
    case DRV_DISABLE:
    case DRV_FREE:
    case DRV_CONFIGURE:
    case DRV_QUERYCONFIGURE:
    case DRV_INSTALL:
    case DRV_REMOVE:
    case DRV_EXITSESSION:
    case DRV_EXITAPPLICATION:
    case DRV_POWER:
    case ACMDM_DRIVER_NOTIFY:
    case ACMDM_DRIVER_DETAILS:
    case ACMDM_HARDWARE_WAVE_CAPS_INPUT:
    case ACMDM_HARDWARE_WAVE_CAPS_OUTPUT:
    case ACMDM_FORMATTAG_DETAILS:
    case ACMDM_FORMAT_DETAILS:
    case ACMDM_FORMAT_SUGGEST:   
    case ACMDM_FILTERTAG_DETAILS:
    case ACMDM_FILTER_DETAILS:
    case ACMDM_STREAM_OPEN:
    case ACMDM_STREAM_CLOSE:
    case ACMDM_STREAM_SIZE:
    case ACMDM_STREAM_CONVERT:
    case ACMDM_STREAM_RESET:
    case ACMDM_STREAM_PREPARE:
    case ACMDM_STREAM_UNPREPARE:
    case ACMDM_STREAM_UPDATE:
    default:
      /* FIXME: DefDriverProc not implemented  */
      DefDriverProc(dwDriverId, hdrvr, msg, lParam1, lParam2);
      break;
    }
  return 0;
} 

