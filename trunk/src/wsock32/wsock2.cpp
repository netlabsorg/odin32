/* $Id: wsock2.cpp,v 1.1 2001-07-07 10:44:10 achimha Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Winsock 2.0 functions for OS/2
 *
 * Copyright (C) 2001 Achim Hasenmueller <achimha@innotek.de>
 *
 * Some parts based on Wine code: (dlls\winsock\socket.c)
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 *
 */

#include <odin.h>
#include <debugtools.h>
#include "wsock32.h"


/***********************************************************************
 *		WSAEventSelect (WS2_32.39)
 */
int WINAPI WSAEventSelect(SOCKET s, WSAEVENT hEvent, LONG lEvent)
{
    TRACE("WS2_32: WSAEventSelect socket %08x, hEvent %08x, event %08x", s, hEvent, (unsigned)lEvent);

    // forward call to worker function in HEVENT notification mode
    return WSAAsyncSelectWorker(s, WSA_SELECT_HEVENT, (int)hEvent, 0, lEvent);
}
