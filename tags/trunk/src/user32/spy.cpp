/* $Id: spy.cpp,v 1.1 1999-06-25 21:35:27 sandervl Exp $ */

/*
 * Queue procedures to send messages to the spy server
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSPROCESS
#define INCL_DOSQUEUES
#define INCL_DOSMEMMGR
#define INCL_WINMESSAGEMGR
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>
#include <os2wrap.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <misc.h>
#include <spy.h>


#define  Q_BUFFER_SIZE  4096
#define  MAX_MESSAGES   (Q_BUFFER_SIZE/sizeof(Q_SPYMSG))

PID       pidServer  = 0;
HQUEUE    hqQueue    = 0;
Q_SPYMSG *pvdQMemory = 0;
int       msgIndex   = 0;

BOOL InitSpyQueue()
{
 APIRET rc;

   if (rc = DosOpenQueue(&pidServer, &hqQueue, "\\queues\\"Q_NAME))
   {
	dprintf(("InitSpyQueue: couldn't open spy queue rc=%d!", rc));
	return FALSE;
   }
   if (rc = DosAllocSharedMem((VOID **)&pvdQMemory,
                         NULL,
                         Q_BUFFER_SIZE,
                         fALLOCSHR))
   {
	dprintf(("InitSpyQueue: DosAllocSharedMem failed rc=%d", rc));
	DosCloseQueue(hqQueue);
	return FALSE;
   }

   /* give memory to server */
   if (DosGiveSharedMem(pvdQMemory, pidServer, PAG_READ | PAG_WRITE))
   {
	dprintf(("InitSpyQueue: DosGiveSharedMem failed"));
	DosCloseQueue(hqQueue);
	return FALSE;
   }
   return (TRUE);
}

void CloseSpyQueue()
{
   if(hqQueue)
   	DosCloseQueue(hqQueue);
   if(pvdQMemory)
   	DosFreeMem(pvdQMemory);
}

VOID PostSpyMessage(HWND hwnd, ULONG Msg, ULONG wParam, ULONG lParam)
{
 APIRET rc;

   if (hqQueue == 0)
	return;
   
   pvdQMemory[msgIndex].hwnd   = hwnd;
   pvdQMemory[msgIndex].Msg    = Msg;
   pvdQMemory[msgIndex].wParam = wParam;
   pvdQMemory[msgIndex].lParam = lParam;

   if (rc = DosWriteQueue(hqQueue,
                     Q_SPYMSG_WNDMSG,
                     sizeof(Q_SPYMSG),
                     &pvdQMemory[msgIndex],
                     0))
   {
	hqQueue = 0; //give up, server probably died
	dprintf(("PostSpyMessage: DosWriteQueue returned %d", rc));
	return;
   }
   if(++msgIndex >= MAX_MESSAGES) 
   {
	msgIndex = 0;
   }
}

