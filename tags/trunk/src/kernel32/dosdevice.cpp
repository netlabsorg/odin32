/* $Id: dosdevice.cpp,v 1.1 2001-11-29 10:31:07 phaller Exp $
 *
 * Win32 Kernel Symbolic Link Subsystem for OS/2
 *
 * 2001/11/29 Patrick Haller <patrick.haller@innotek.de>
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

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD QueryDosDeviceA
 * Purpose   : The QueryDosDevice function lets an application obtain information
 *             about MS-DOS device names. The function can obtain the current
 *             mapping for a particular MS-DOS device name. The function can also
 *             obtain a list of all existing MS-DOS device names.
 *             MS-DOS device names are stored as symbolic links in the Windows NT
 *             object name space. The code that converts an MS-DOS path into a
 *             corresponding Windows NT path uses these symbolic links to map
 *             MS-DOS devices and drive letters. The QueryDosDevice function
 *             provides a mechanism whereby a Win32-based application can query
 *             the names of the symbolic links used to implement the MS-DOS device
 *             namespace as well as the value of each specific symbolic link.
 * Parameters: LPCTSTR lpDeviceName address of MS-DOS device name string
 *             LPTSTR  lpTargetPath ddress of buffer for storing query results
 *             DWORD   ucchMax       maximum storage capacity of buffer
 * Variables :
 * Result    : pointer to lpTargetPath
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API QueryDosDeviceA(LPCTSTR lpDeviceName,
                               LPTSTR  lpTargetPath,
                               DWORD   ucchMax)
{
  dprintf(("Kernel32: QueryDosDeviceA(%s,%08xh,%08xh) not implemented.\n",
           lpDeviceName,
           lpTargetPath,
           ucchMax));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD QueryDosDeviceW
 * Purpose   : The QueryDosDevice function lets an application obtain information
 *             about MS-DOS device names. The function can obtain the current
 *             mapping for a particular MS-DOS device name. The function can also
 *             obtain a list of all existing MS-DOS device names.
 *             MS-DOS device names are stored as symbolic links in the Windows NT
 *             object name space. The code that converts an MS-DOS path into a
 *             corresponding Windows NT path uses these symbolic links to map
 *             MS-DOS devices and drive letters. The QueryDosDevice function
 *             provides a mechanism whereby a Win32-based application can query
 *             the names of the symbolic links used to implement the MS-DOS device
 *             namespace as well as the value of each specific symbolic link.
 * Parameters: LPCTSTR lpDeviceName address of MS-DOS device name string
 *             LPTSTR  lpTargetPath ddress of buffer for storing query results
 *             DWORD   ucchMax       maximum storage capacity of buffer
 * Variables :
 * Result    : pointer to lpTargetPath
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API QueryDosDeviceW(LPCWSTR lpDeviceName,
                               LPWSTR  lpTargetPath,
                               DWORD   ucchMax)
{
  dprintf(("Kernel32: QueryDosDeviceW(%s,%08xh,%08xh) not implemented.\n",
           lpDeviceName,
           lpTargetPath,
           ucchMax));

  return (0);
}
