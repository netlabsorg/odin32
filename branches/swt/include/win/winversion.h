/* $Id: winversion.h,v 1.2 2000-05-19 12:08:37 sandervl Exp $ */

#ifndef __WINE_WINVERSION_H
#define __WINE_WINVERSION_H

typedef enum
{
    WIN31, /* Windows 3.1 */
    WIN95,   /* Windows 95 */
    NT351,   /* Windows NT 3.51 */
    NT40,    /* Windows NT 4.0 */  
    NB_WINDOWS_VERSIONS
} WINDOWS_VERSION;

#ifdef __cplusplus
extern "C" {
#endif

WINDOWS_VERSION VERSION_GetVersion(void); 
char *VERSION_GetVersionName(void); 
BOOL VERSION_OsIsUnicode(void);

#ifdef __cplusplus
}
#endif

#endif  /* __WINE_WINVERSION_H */
