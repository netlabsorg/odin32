/* $Id: mmreg.h,v 1.1 1999-05-24 20:19:14 ktk Exp $ */

/*
 *      mmreg.h   -       Declarations for ???
 */


/***********************************************************************
 * Defines/Enums
 */

#ifndef _ACM_WAVEFILTER
#define _ACM_WAVEFILTER

#include "windef.h"

#define WAVE_FILTER_UNKNOWN     0x0000
#define WAVE_FILTER_DEVELOPMENT 0xFFFF

typedef struct _WAVEFILTER {
  DWORD   cbStruct;
  DWORD   dwFilterTag;
  DWORD   fdwFilter;
  DWORD   dwReserved[5];
} WAVEFILTER, *PWAVEFILTER, *NPWAVEFILTER, *LPWAVEFILTER;
#endif /* _ACM_WAVEFILTER */


#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct _WAVEFORMATEX {
  WORD   wFormatTag;
  WORD   nChannels;
  DWORD  nSamplesPerSec;
  DWORD  nAvgBytesPerSec;
  WORD   nBlockAlign;
  WORD   wBitsPerSample;
  WORD   cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

