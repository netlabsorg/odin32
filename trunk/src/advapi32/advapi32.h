/* $Id: advapi32.h,v 1.1 1999-05-24 20:19:33 ktk Exp $ */

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

//Why are the Open32 keys defined differently?
#define WINHKEY_CLASSES_ROOT           (( HKEY ) 0x80000000 )
#define WINHKEY_CURRENT_USER           (( HKEY ) 0x80000001 )
#define WINHKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define WINHKEY_USERS                  (( HKEY ) 0x80000003 )
#define WINHKEY_PERFORMANCE_DATA       (( HKEY ) 0x80000004 )


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
