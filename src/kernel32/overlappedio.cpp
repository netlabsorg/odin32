/* $Id: overlappedio.cpp,v 1.1 2001-12-05 14:16:38 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 COM device access class
 *
 * 1999 Achim Hasenmueller <achimha@innotek.de>
 * 2001 Sander van Leeuwen <sandervl@xs4all.nl>
 *
 * TODO: Overlapped IO only supports one request at a time
 * TODO: Overlapped IO not thread safe
 *
 */



#include <os2win.h>
#include <string.h>
#include <handlemanager.h>
#include "handlenames.h"
#include <heapstring.h>
#include "hmdevice.h"
#include "hmcomm.h"
#include "oslibdos.h"

#define DBG_LOCALLOG  DBG_overlappedio
#include "dbglocal.h"


DWORD CALLBACK OverlappedIOEventThread(LPVOID lpThreadParam);
DWORD CALLBACK OverlappedIOReadWriteThread(LPVOID lpThreadParam);

//******************************************************************************
//******************************************************************************


//******************************************************************************
//******************************************************************************
