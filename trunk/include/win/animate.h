/* $Id: animate.h,v 1.2 1999-10-01 15:52:19 cbratschi Exp $ */

/*
 * Animation class extra info
 *
 * Copyright 1998 Eric Kohl
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_ANIMATE_H
#define __WINE_ANIMATE_H


typedef struct tagANIMATE_INFO
{
    LPVOID  lpAvi;   /* pointer to avi data */
    HFILE   hFile;   /* handle to avi file */
    HGLOBAL hRes;
    INT     cSize;
    INT     cRepeat;
    INT     wFrom;
    INT     wTo;
    INT     wPos;
    INT     cFrames;
    BOOL    bPlaying;
    POINT   pos;
    POINT   size;
    INT     msFrame;
    BOOL    bThread;
} ANIMATE_INFO;


extern VOID ANIMATE_Register (VOID);
extern VOID ANIMATE_Unregister (VOID);

#endif  /* __WINE_ANIMATE_H */
