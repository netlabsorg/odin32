/* $Id: crtdll.h,v 1.1 1999-05-24 20:19:10 ktk Exp $ */

#ifndef __WINE_CRTDLL_H
#define __WINE_CRTDLL_H

#define CRTDLL_LC_ALL		0
#define CRTDLL_LC_COLLATE	1
#define CRTDLL_LC_CTYPE		2
#define CRTDLL_LC_MONETARY	3
#define CRTDLL_LC_NUMERIC	4
#define CRTDLL_LC_TIME		5
#define CRTDLL_LC_MIN		LC_ALL
#define CRTDLL_LC_MAX		LC_TIME

/* ctype defines */
#define CRTDLL_UPPER		0x1
#define CRTDLL_LOWER		0x2
#define CRTDLL_DIGIT		0x4
#define CRTDLL_SPACE		0x8
#define CRTDLL_PUNCT		0x10
#define CRTDLL_CONTROL		0x20
#define CRTDLL_BLANK		0x40
#define CRTDLL_HEX		0x80
#define CRTDLL_LEADBYTE		0x8000
#define CRTDLL_ALPHA		(0x0100|CRTDLL_UPPER|CRTDLL_LOWER)

/* function prototypes used in crtdll.c */
extern int LastErrorToErrno(DWORD);

#endif /* __WINE_CRTDLL_H */
