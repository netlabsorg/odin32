/* $Id: fileio.h,v 1.1 2001-08-10 19:32:27 sandervl Exp $ */

#ifndef __FILEIO_H__
#define __FILEIO_H__

void DOSFS_Hash( LPCSTR name, LPSTR buffer, BOOL dir_format,
                 BOOL ignore_case = TRUE);

#endif
