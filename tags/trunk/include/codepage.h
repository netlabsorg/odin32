/*
** Module   :CODEPAGE.H
** Abstract :
**
** Copyright (C) Vit Timchishin
**
** Log: Wed  22/12/1999 Created
**
*/

#ifndef __CODEPAGE_H
#define __CODEPAGE_H

// AH 2001-04-03 use Odin builtin version, later toolkits
// include <os2def.h> almost everywhere...
#include <uniconv.h>

#define CODEPAGE_SECTION    "Codepages"

ULONG GetDisplayCodepage();
ULONG GetWindowsCodepage();
UconvObject GetDisplayUconvObject();
UconvObject GetWindowsUconvObject();

void CODEPAGE_Init(void);

#endif  /*__CODEPAGE_H*/

