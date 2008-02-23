/* $Id: dosdevice.cpp,v 1.3 2005-01-15 22:17:31 sao2l02 Exp $
 *
 * Win32 Kernel Symbolic Link Subsystem for OS/2
 *
 * 2001/11/29 Patrick Haller <patrick.haller@innotek.de>
 *
 *
 * QueryDosDeviceA/W borrowed from Rewind (pre-LGPL Wine)
 * Copyright 1993 Erik Bos
 * Copyright 1996 Alexandre Julliard
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

 /* To Do:
  * - implement both APIs using the HandleNames symbolic link resolver
  */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/
#include <os2win.h>
#include <stdio.h>
#include <winnls.h>
#include "unicode.h"
#include "handlemanager.h"
#include "handlenames.h"
#include <string.h>


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


// For DefineDosDevice

#define DDD_RAW_TARGET_PATH         0x00000001
#define DDD_REMOVE_DEFINITION       0x00000002
#define DDD_EXACT_MATCH_ON_REMOVE   0x00000004
#define DDD_NO_BROADCAST_SYSTEM     0x00000008



/*****************************************************************************
 * Name      : BOOL WIN32API DefineDosDeviceA
 * Purpose   : The DefineDosDeviceA function lets an application define,
 *             redefine, or delete MS-DOS device names.
 *             MS-DOS device names are stored as symbolic links in the object
 *             name space in 32-bit versions of Windows. The code that converts
 *             an MS-DOS path into a corresponding path in 32-bit versions of
 *             Windows uses these symbolic links to map MS-DOS devices and drive
 *             letters. The DefineDosDevice function provides a mechanism
 *             whereby a Win32-based application can modify the symbolic links
 *             used to implement the MS-DOS device name space.
 * Parameters: DWORD dwFlags         flags specifying aspects of device definition
 *             LPCSTR lpDeviceName   pointer to MS-DOS device name string
 *             LPCSTR lpTargetPath   pointer to MS-DOS or path string for
 *                                   32-bit Windows
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API DefineDosDeviceA( DWORD dwFlags, LPCSTR lpDeviceName,
                                   LPCSTR lpTargetPath  )
{

  dprintf(("KERNEL32: DefineDosDeviceA(%08x,%08x,%08x) not implemented\n",
           dwFlags,  lpDeviceName, lpTargetPath
          ));

  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API DefineDosDeviceW
 * Purpose   : UNICODE version of DefineDosDeviceA
 * Parameters: DWORD dwFlags         flags specifying aspects of device definition
 *             LPCSTR lpDeviceName   pointer to MS-DOS device name string
 *             LPCSTR lpTargetPath   pointer to MS-DOS or path string for
 *                                   32-bit Windows
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API DefineDosDeviceW( DWORD dwFlags, LPCWSTR lpDeviceName,
                                   LPCWSTR lpTargetPath )
{

  dprintf(("KERNEL32: DefineDosDeviceW(%08x,%08x,%08x) not implemented\n",
           dwFlags,  lpDeviceName, lpTargetPath
          ));

  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return (FALSE);
}


/***********************************************************************
 *           QueryDosDeviceA   (KERNEL32.@)
 *
 * returns array of strings terminated by \0, terminated by \0
 */
DWORD WINAPI QueryDosDeviceA(LPCSTR devname,LPSTR target,DWORD bufsize)
{
    LPSTR s;
    char  buffer[200];

    dprintf(("(%s,...)\n", devname ? devname : "<null>"));
    if (!devname) {
	/* return known MSDOS devices */
#ifdef __WIN32OS2__
        static const char devices[24] = "CON\0COM1\0COM2\0LPT1\0NUL\0";
        if(bufsize < 128 + sizeof(devices)) {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }

        memcpy( target, devices, sizeof(devices) );

        //Add all valid drive letters
        int drivemask = GetLogicalDrives();
        ULONG mask = 1;
        int i, len = 0;
        char *ptr = target+sizeof(devices)-1;

        for(i=0;i<26;i++)
        {
            if(drivemask & mask)
            {
                len += 3;
                *ptr++ = (char)('A' + i);
                *ptr++ = ':';
                *ptr++ = 0;
            }
            mask <<= 1;
        }
        *ptr = 0;
        
        SetLastError(ERROR_SUCCESS);
        return sizeof(devices)+len;
#else
        static const char devices[24] = "CON\0COM1\0COM2\0LPT1\0NUL\0\0";
        memcpy( target, devices, min(bufsize,sizeof(devices)) );
        return min(bufsize,sizeof(devices));
#endif
    }
    /* In theory all that are possible and have been defined.
     * Now just those below, since mirc uses it to check for special files.
     *
     * (It is more complex, and supports netmounted stuff, and \\.\ stuff,
     *  but currently we just ignore that.)
     */
#define CHECK(x) (strstr(devname,#x)==devname)
    if (CHECK(con) || CHECK(com) || CHECK(lpt) || CHECK(nul)) {
	strcpy(buffer,"\\DEV\\");
	strcat(buffer,devname);
	if ((s=strchr(buffer,':'))) *s='\0';
	lstrcpynA(target,buffer,bufsize);
	return strlen(buffer)+1;
    } else {
#ifdef __WIN32OS2__
        if(devname[1] == ':' && devname[2] == 0) {
            int ret = GetDriveTypeA(devname);
            if(ret != DRIVE_UNKNOWN) 
            {
                if(bufsize < 16) {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return 0;
                }
                sprintf(target, "\\\\.\\%s", devname);
                SetLastError(ERROR_SUCCESS);
                return strlen(target)+1;
            }
        }
#endif
	if (strchr(devname,':') || devname[0]=='\\') {
	    /* This might be a DOS device we do not handle yet ... */
	    dprintf(("(%s) not detected as DOS device!\n",devname));
	}
	SetLastError(ERROR_DEV_NOT_EXIST);
	return 0;
    }
}


/***********************************************************************
 *           QueryDosDeviceW   (KERNEL32.@)
 *
 * returns array of strings terminated by \0, terminated by \0
 */
DWORD WINAPI QueryDosDeviceW(LPCWSTR devname,LPWSTR target,DWORD bufsize)
{
    LPSTR devnameA = devname?HEAP_strdupWtoA(GetProcessHeap(),0,devname):NULL;
    LPSTR targetA = (LPSTR)HeapAlloc(GetProcessHeap(),0,bufsize);
    DWORD ret = QueryDosDeviceA(devnameA,targetA,bufsize);

    ret = MultiByteToWideChar( CP_ACP, 0, targetA, ret, target, bufsize );
    HeapFree(GetProcessHeap(),0,devnameA);
    HeapFree(GetProcessHeap(),0,targetA);
    return ret;
}
