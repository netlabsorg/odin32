/* $Id: progress.h,v 1.3 2001-03-31 13:23:34 sandervl Exp $ */

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


#ifdef __cplusplus
extern "C" {
#endif

VOID PROGRESS_Register (VOID);
VOID PROGRESS_Unregister (VOID);

#ifdef __cplusplus
}
#endif


#endif  /* __WINE_PROGRESS_H */
