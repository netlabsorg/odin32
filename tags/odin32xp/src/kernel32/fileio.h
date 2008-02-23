/* $Id: fileio.h,v 1.2 2002-08-16 09:56:30 sandervl Exp $ */

#ifndef __FILEIO_H__
#define __FILEIO_H__

static void DOSFS_Hash( LPCSTR name, LPSTR buffer, BOOL dir_format,
                 BOOL ignore_case = TRUE);

/* locale-independent case conversion */
inline static char FILE_tolower( char c )
{
    if (c >= 'A' && c <= 'Z') c += 32;
    return c;
}
inline static char FILE_toupper( char c )
{
    if (c >= 'a' && c <= 'z') c -= 32;
    return c;
}


#define MAX_PATHNAME_LEN        1024

#endif
