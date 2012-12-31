/* $Id: winmfcmsg.h,v 1.1 1999-10-17 20:14:53 sandervl Exp $ */

#ifndef __WINE_WINMFCMSG_H
#define __WINE_WINMFCMSG_H

/* MFC messages [360-38f] */
#define WM_QUERYAFXWNDPROC  0x0360
#define WM_SIZEPARENT       0x0361
#define WM_SETMESSAGESTRING 0x0362
#define WM_IDLEUPDATECMDUI  0x0363 
#define WM_INITIALUPDATE    0x0364
#define WM_COMMANDHELP      0x0365
#define WM_HELPHITTEST      0x0366
#define WM_EXITHELPMODE     0x0367
#define WM_RECALCPARENT     0x0368
#define WM_SIZECHILD        0x0369
#define WM_KICKIDLE         0x036A 
#define WM_QUERYCENTERWND   0x036B
#define WM_DISABLEMODAL     0x036C
#define WM_FLOATSTATUS      0x036D 
#define WM_ACTIVATETOPLEVEL 0x036E 
#define WM_QUERY3DCONTROLS  0x036F 
#define WM_SOCKET_NOTIFY    0x0373
#define WM_SOCKET_DEAD      0x0374
#define WM_POPMESSAGESTRING 0x0375
#define WM_OCC_LOADFROMSTREAM           0x0376
#define WM_OCC_LOADFROMSTORAGE          0x0377
#define WM_OCC_INITNEW                  0x0378
#define WM_OCC_LOADFROMSTREAM_EX        0x037A
#define WM_OCC_LOADFROMSTORAGE_EX       0x037B
#define WM_QUEUE_SENTINEL   0x0379

/* end of MFC messages */

#endif  /* __WINE_WINMFCMSG_H */
