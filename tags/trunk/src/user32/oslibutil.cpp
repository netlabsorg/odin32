/* $Id: oslibutil.cpp,v 1.1 1999-09-15 23:18:54 sandervl Exp $ */
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

//******************************************************************************
//******************************************************************************
HAB GetThreadHAB()
{
 THDB *thdb;

  thdb = GetThreadTHDB();
  if(thdb)
  {
	return (HAB)thdb->hab;
  }

  dprintf(("GetThreadHAB: thdb == NULL!!"));
  return 0;
}
//******************************************************************************
//******************************************************************************
void SetThreadHAB(HAB hab)
{
 THDB *thdb;

  thdb = GetThreadTHDB();
  if(thdb)
  {
	thdb->hab = (ULONG)hab;
  }
  else 	dprintf(("SetThreadHAB: thdb == NULL!!"));
}
//******************************************************************************
//******************************************************************************
HMQ GetThreadMessageQueue()
{
 THDB *thdb;

  thdb = GetThreadTHDB();
  if(thdb)
  {
	return (HMQ)thdb->hmq;
  }

  dprintf(("GetThreadMessageQueue: thdb == NULL!!"));
  return 0;
}
//******************************************************************************
//******************************************************************************
void SetThreadMessageQueue(HMQ hmq)
{
 THDB *thdb;

  thdb = GetThreadTHDB();
  if(thdb)
  {
	thdb->hmq = (ULONG)hmq;	
  }
  else 	dprintf(("SetThreadMessageQueue: thdb == NULL!!"));
}
//******************************************************************************
//******************************************************************************
