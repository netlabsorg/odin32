/*
** Module   :CODEPAGE.H
** Abstract :
**
** Copyright (C) Vit Timchishin
**
** Log: Wed  22/12/1999 Created
**
*/
#include <uconv.h>

#ifndef __CODEPAGE_H
#define __CODEPAGE_H

#define CODEPAGE_SECTION    "Codepages"

ULONG GetDisplayCodepage();
ULONG GetWindowsCodepage();
UconvObject GetDisplayUconvObject();
UconvObject GetWindowsUconvObject();

void CODEPAGE_Init(void);

#endif  /*__CODEPAGE_H*/

