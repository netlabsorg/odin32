/* $Id: progress.h,v 1.2 2000-03-17 17:10:52 cbratschi Exp $ */

/*
 * Progress class extra info
 *
 * Copyright 1997 Dimitrie O. Paun
 */

#ifndef __WINE_PROGRESS_H
#define __WINE_PROGRESS_H

#include "windef.h"

typedef struct
{
  COMCTL32_HEADER header;

  INT       CurVal;       /* Current progress value */
  INT       MinVal;       /* Minimum progress value */
  INT       MaxVal;       /* Maximum progress value */
  INT       Step;         /* Step to use on PMB_STEPIT */
  COLORREF  ColorBar;     /* Bar color */
  COLORREF  ColorBk;      /* Background color */
  HFONT     hFont;        /* Handle to font (not unused) */
} PROGRESS_INFO;


extern VOID PROGRESS_Register (VOID);
extern VOID PROGRESS_Unregister (VOID);

#endif  /* __WINE_PROGRESS_H */
