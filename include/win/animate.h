/* $Id: animate.h,v 1.1 1999-05-24 20:19:08 ktk Exp $ */

/*
 * Animation class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_ANIMATE_H
#define __WINE_ANIMATE_H


typedef struct tagANIMATE_INFO
{
    LPVOID  lpAvi;   /* pointer to avi data */
    HFILE hFile;   /* handle to avi file */
} ANIMATE_INFO;


extern VOID ANIMATE_Register (VOID);
extern VOID ANIMATE_Unregister (VOID);

#endif  /* __WINE_ANIMATE_H */
