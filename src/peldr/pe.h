/* $Id: pe.h,v 1.4 1999-08-16 13:54:07 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * PELDR main exe loader header file
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PE_H__
#define __PE_H__

extern char szErrorTitle[];
extern char szMemErrorMsg[];
extern char szFileErrorMsg[];
extern char szPEErrorMsg[];
extern char szCPUErrorMsg[];
extern char szExeErrorMsg[];
extern char szInteralErrorMsg[];

ULONG SYSTEM ReserveMem();

#endif // NOIMAGE

