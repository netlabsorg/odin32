/* $Id: os2newapi.h,v 1.1 1999-06-19 10:53:36 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2NEWAPI_H__
#define __OS2NEWAPI_H__

#ifdef INCL_WIN
#ifdef INCL_WINMESSAGEMGR
BOOL APIENTRY WinThreadAssocQueue(HAB hab, HMQ hmq);
BOOL APIENTRY WinWakeThread(HMQ hmq);
BOOL APIENTRY WinReplyMsg(HAB hab, HMQ hmqSender, HMQ hmqReceiver, MRESULT mresult);
HMQ  APIENTRY WinQueueFromID(HAB hab, PID pid, TID tid);
BOOL APIENTRY WinQueryQueueInfo(HMQ hmq, PMQINFO pmqi, ULONG cbCopy);
HMQ  APIENTRY WinQuerySendMsg(HAB hab, HMQ hmqSender, HMQ hmqReceiver, PQMSG pqmsg);
BOOL APIENTRY WinLockInput(HMQ hmq, ULONG fLock);
#endif
#endif

#endif //__OS2SEL_H__