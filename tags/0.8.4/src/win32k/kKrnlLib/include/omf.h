/* $Id: omf.h,v 1.1 2001-09-14 01:51:32 bird Exp $ */
/*
 * OMF stuff
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OMF_H_
#define _OMF_H_

/* record types */
#define LIBHDR      0xF0
#define LIBEND      0xF1
#define THEADR      0x80
#define MODEND      0x8a
#define MODEND2     0x8b
#define COMENT      0x88
#define     IMPDEF  0x01
#define PUBDEF      0x90
#define LNAMES      0x96
#define SEGDEF      0x98
#define SEGDEF2     0x99
#define GRPDEF      0x9A
#define FIXUPP      0x9C
#define FIXUPP2     0x9D
#define LEDATA      0xA0
#define LEDATA2     0xA1

#endif
