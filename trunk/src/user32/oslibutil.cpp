/* $Id: oslibutil.cpp,v 1.7 2001-12-12 16:40:44 sandervl Exp $ */
/*
 * Window API utility functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <wprocess.h>
#include "oslibutil.h"

#define DBG_LOCALLOG	DBG_oslibutil
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HAB GetThreadHAB()
{
 TEB *teb;

  teb = GetThreadTEB();
  if(teb)
  {
        return (HAB)teb->o.odin.hab;
  }

  dprintf(("GetThreadHAB: teb == NULL!!"));
  return 0;
}
//******************************************************************************
//******************************************************************************
void SetThreadHAB(HAB hab)
{
 TEB *teb;

  teb = GetThreadTEB();
  if(teb)
  {
        teb->o.odin.hab = (ULONG)hab;
  }
  else  dprintf(("SetThreadHAB: teb == NULL!!"));
}
//******************************************************************************
//******************************************************************************
HMQ GetThreadMessageQueue()
{
 TEB *teb;

  teb = GetThreadTEB();
  if(teb)
  {
        return (HMQ)teb->o.odin.hmq;
  }

  dprintf(("GetThreadMessageQueue: teb == NULL!!"));
  return 0;
}
//******************************************************************************
//******************************************************************************
void SetThreadMessageQueue(HMQ hmq)
{
 TEB *teb;

  teb = GetThreadTEB();
  if(teb)
  {
        teb->o.odin.hmq = (ULONG)hmq;
  }
  else  dprintf(("SetThreadMessageQueue: teb == NULL!!"));
}
//******************************************************************************
//******************************************************************************
