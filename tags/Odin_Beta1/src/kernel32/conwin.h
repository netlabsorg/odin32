/* $Id: conwin.h,v 1.2 1999-06-20 10:55:36 sandervl Exp $ */

#ifndef _CONWIN_H_
#define _CONWIN_H_

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#endif

// PH: unfortunately, ODIN/WINE headers don't mix with OS/2 headers
#define STD_INPUT_HANDLE         ((DWORD)-10)
#define STD_OUTPUT_HANDLE        ((DWORD)-11)
#define STD_ERROR_HANDLE         ((DWORD)-12)
#define GENERIC_READ                 0x80000000
#define GENERIC_WRITE                0x40000000
#define FILE_SHARE_READ              0x00000001
#define FILE_SHARE_WRITE             0x00000002

#endif /* _CONWIN_H_ */

