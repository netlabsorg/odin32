/* $Id: advapi32.h,v 1.3 1999-06-10 14:23:34 phaller Exp $ */

/*
 * Win32 advanced API functions for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __ADVAPI32_H__
#define __ADVAPI32_H__

// IsTextUnicode
#define IS_TEXT_UNICODE_ASCII16               0x0001
#define IS_TEXT_UNICODE_REVERSE_ASCII16       0x0010

#define IS_TEXT_UNICODE_STATISTICS            0x0002
#define IS_TEXT_UNICODE_REVERSE_STATISTICS    0x0020

#define IS_TEXT_UNICODE_CONTROLS              0x0004
#define IS_TEXT_UNICODE_REVERSE_CONTROLS      0x0040

#define IS_TEXT_UNICODE_SIGNATURE             0x0008
#define IS_TEXT_UNICODE_REVERSE_SIGNATURE     0x0080

#define IS_TEXT_UNICODE_ILLEGAL_CHARS         0x0100
#define IS_TEXT_UNICODE_ODD_LENGTH            0x0200
#define IS_TEXT_UNICODE_DBCS_LEADBYTE         0x0400
#define IS_TEXT_UNICODE_NULL_BYTES            0x1000

#define IS_TEXT_UNICODE_UNICODE_MASK          0x000F
#define IS_TEXT_UNICODE_REVERSE_MASK          0x00F0
#define IS_TEXT_UNICODE_NOT_UNICODE_MASK      0x0F00
#define IS_TEXT_UNICODE_NOT_ASCII_MASK        0xF000


#endif
