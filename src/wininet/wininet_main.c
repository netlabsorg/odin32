/* $Id: wininet_main.c,v 1.2 2001-09-05 10:31:47 bird Exp $
 *
 * WinInet
 *
 * Copyright (c) 2000 Patrik Stridvall
 *
 */

#include "windef.h"
#include "winerror.h"

#include "debugtools.h"

DEFAULT_DEBUG_CHANNEL(wininet);

/***********************************************************************
 *      WININET_DllInstall (WININET.@)
 */
HRESULT WINAPI WININET_DllInstall(BOOL bInstall, LPCWSTR cmdline)
{
  FIXME("(%s, %s): stub\n", bInstall?"TRUE":"FALSE",
    debugstr_w(cmdline));

  return S_OK;
}

