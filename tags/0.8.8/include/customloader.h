/* $Id: customloader.h,v 1.6 2004/04/20 08:43:11 sandervl Exp $ */

#ifndef _CUSTOMLOADER_H_
#define _CUSTOMLOADER_H_

#include <win32type.h>
#include <winconst.h>

#define MAGIC_STUBEXE_SIGNATURE	0xA2A09876
#define MAGIC_STUBEXE_SIZE      8

//Use by LdrGeneric
extern char szErrorTitle[];
extern char szAppNotFound[];
extern char szDefaultExe[];
extern char szDefaultExePath[];
extern char szCustomDll[];

ULONG LdrAllocateImage(char *filename, ULONG *pulPEOffset, BOOL *pfConsoleApp);
ULONG LdrAllocateImageNE(char *filename, BOOL *pfNEExe, PBOOL pfConsoleApp);
int   LdrGeneric(int argc, char *argv[]);
int   LdrGenericStubExe(int argc, char *argv[]);
BOOL  LdrSetLibPath(char *pszHomeDir);

BOOL  LdrRegQueryKey(HKEY hRoot, char *szKeyPath, char *szKeyName, DWORD dwType,
                     LPBYTE lpData, DWORD *pcbCount);

/*
 * PE version checking function
 *
 * Returns: 0  - equal
 *          1  - file1 > file2
 *         -1  - file1 < file2
 *         -2  - error
 */
int   LdrCompareVersion(LPSTR lpszModule1, LPSTR lpszModule2);

#endif /* _CUSTOMLOADER_H_ */

