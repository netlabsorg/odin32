/* $Id: os2newapi.h,v 1.2 1999-06-19 13:57:50 sandervl Exp $ */
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

#ifdef INCL_DOSSEL
/* Segment attribute flags (used with DosAllocSeg) */

#define SEG_NONSHARED       0x0000
#define SEG_GIVEABLE        0x0001
#define SEG_GETTABLE        0x0002
#define SEG_DISCARDABLE     0x0004

#define DosAllocSeg     Dos16AllocSeg
#define DosFreeSeg      Dos16FreeSeg
#define DosGetSeg       Dos16GetSeg
#define DosGiveSeg      Dos16GiveSeg
#define DosReallocSeg   Dos16ReallocSeg
#define DosSizeSeg      Dos16SizeSeg

USHORT APIENTRY16 DosAllocSeg(USHORT cbSize, PSEL pSel, USHORT fsAlloc);
USHORT APIENTRY16 DosFreeSeg(SEL sel);
USHORT APIENTRY16 DosGetSeg(SEL sel);
USHORT APIENTRY16 DosGiveSeg(SEL sel, PID pid, PSEL pSelRecipient);
USHORT APIENTRY16 DosReallocSeg(USHORT cbNewSize, SEL sel);
USHORT APIENTRY16 DosSizeSeg(SEL sel, PULONG pcbSize);

#define DosSelToFlat Dos32SelToFlat
ULONG _Optlink DosSelToFlat(ULONG seladdr);

#define DosFlatToSel Dos32FlatToSel
ULONG _Optlink DosFlatToSel(ULONG lin);

#endif

#endif //__OS2SEL_H__