/* $Id: pe.h,v 1.5 2000-01-30 14:48:51 sandervl Exp $ */

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

#ifndef PAG_ANY
    #define PAG_ANY    0x00000400
#endif

#ifndef QSV_VIRTUALADDRESSLIMIT
    #define QSV_VIRTUALADDRESSLIMIT 30
#endif

#define FALLOC_SIZE (1024*1024)

extern char szErrorTitle[];
extern char szMemErrorMsg[];
extern char szFileErrorMsg[];
extern char szPEErrorMsg[];
extern char szCPUErrorMsg[];
extern char szExeErrorMsg[];
extern char szInteralErrorMsg[];

#endif // NOIMAGE

