/* $Id: classes.cpp,v 1.3 2000-03-26 16:34:39 cbratschi Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Corel WINE 20000324 level
 */

/*
 *      file type mapping
 *      (HKEY_CLASSES_ROOT - Stuff)
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "winerror.h"
#include "winreg.h"

#include "shlobj.h"
#include "shell32_main.h"
#include "shlguid.h"
#include "shresdef.h"

#include <misc.h>


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(shell32-classes)

#define MAX_EXTENSION_LENGTH 20


/*****************************************************************************
 * Implementation                                                            *
 *****************************************************************************/

BOOL HCR_MapTypeToValue ( LPCSTR szExtension, LPSTR szFileType, DWORD len, BOOL bPrependDot)
{
        HKEY    hkey;
        char    szTemp[MAX_EXTENSION_LENGTH + 2];

  dprintf(("SHELL32:classes:HCR_MapTypeToValue(%s,%08xh,%08xh,%08xh)\n",
           szExtension,
           szFileType,
           len,
           bPrependDot));

        if (bPrependDot)
          strcpy(szTemp, ".");

        lstrcpynA(szTemp+((bPrependDot)?1:0), szExtension, MAX_EXTENSION_LENGTH);

        if (RegOpenKeyExA(HKEY_CLASSES_ROOT,szTemp,0,0x02000000,&hkey))
        { return FALSE;
        }

        if (RegQueryValueA(hkey,NULL,szFileType,(LPLONG)&len))
        { RegCloseKey(hkey);
          return FALSE;
        }

        RegCloseKey(hkey);

        TRACE("-- %s\n", szFileType );

        return TRUE;
}

BOOL HCR_GetExecuteCommand ( LPCSTR szClass, LPCSTR szVerb, LPSTR szDest, DWORD len )
{
        HKEY    hkey;
        char    sTemp[256];

  dprintf(("SHELL32:classes:HCR_GetExecuteCommand(%s,%s,%08xh,%08xh)\n",
           szClass,
           szVerb,
           szDest,
           len));

        sprintf(sTemp, "%s\\shell\\%s\\command",szClass, szVerb);

        if (RegOpenKeyExA(HKEY_CLASSES_ROOT,sTemp,0,0x02000000,&hkey))
        { return FALSE;
        }

        if (RegQueryValueA(hkey,NULL,szDest,(LPLONG)&len))
        { RegCloseKey(hkey);
          return FALSE;
        }
        RegCloseKey(hkey);

        TRACE("-- %s\n", szDest );

        return TRUE;

}
/***************************************************************************************
*       HCR_GetDefaultIcon      [internal]
*
* Gets the icon for a filetype
*/
BOOL HCR_GetDefaultIcon (LPCSTR szClass, LPSTR szDest, DWORD len, LPDWORD dwNr)
{
        HKEY    hkey;
        char    sTemp[256];
        char    sNum[5];

  dprintf(("SHELL32:classes:HCR_GetDefaultIcon(%s,%08xh,%08xh,%08xh)\n",
           szClass,
           szDest,
           len,
           dwNr));

        sprintf(sTemp, "%s\\DefaultIcon",szClass);

        if (RegOpenKeyExA(HKEY_CLASSES_ROOT,sTemp,0,0x02000000,&hkey))
        { return FALSE;
        }

        if (RegQueryValueA(hkey,NULL,szDest,(LPLONG)&len))
        { RegCloseKey(hkey);
          return FALSE;
        }

        RegCloseKey(hkey);

        if (ParseFieldA (szDest, 2, sNum, 5))
        { *dwNr=atoi(sNum);
        }

        ParseFieldA (szDest, 1, szDest, len);

        TRACE("-- %s %li\n", szDest, *dwNr );

        return TRUE;
}

/***************************************************************************************
*       HCR_GetClassName        [internal]
*
* Gets the name of a registred class
*/
BOOL HCR_GetClassName (REFIID riid, LPSTR szDest, DWORD len)
{
        HKEY    hkey;
        char    xriid[50];
        BOOL ret = FALSE;
        DWORD buflen = len;

  dprintf(("SHELL32:classes:HCR_GetClassName(%08xh,%08xh,%08xh)\n",
           riid,
           szDest,
           len));

        strcpy(xriid,"CLSID\\");
        WINE_StringFromCLSID(riid,&xriid[strlen(xriid)]);

        TRACE("%s\n",xriid );

        if (!RegOpenKeyExA(HKEY_CLASSES_ROOT,xriid,0,KEY_READ,&hkey))
        {
          if (!RegQueryValueExA(hkey,"",0,NULL,(LPBYTE)szDest,&len))
          {
            ret = TRUE;
          }
          RegCloseKey(hkey);
        }

        if (!ret || !szDest[0])
        {
          if(IsEqualIID(riid, &CLSID_ShellDesktop))
          {
            LoadStringA(shell32_hInstance, IDS_DESKTOP, szDest, buflen);
            ret = TRUE;
          }
          else if (IsEqualIID(riid, &CLSID_MyComputer))
          {
            LoadStringA(shell32_hInstance, IDS_MYCOMPUTER, szDest, buflen);
            ret = TRUE;
          }
        }

        TRACE("-- %s\n", szDest);

        return ret;
}

/***************************************************************************************
*       HCR_GetFolderAttributes [internal]
*
* gets the folder attributes of a class
*
* FIXME
*	verify the defaultvalue for *szDest
*/
BOOL HCR_GetFolderAttributes (REFIID riid, LPDWORD szDest)
{
        HKEY    hkey;
        char    xriid[60];
        DWORD   attributes;
        DWORD   len = 4;

  dprintf(("SHELL32:classes:HCR_GetFolderAttributes(%08xh,%08xh)\n",
           riid,
           szDest));

        strcpy(xriid,"CLSID\\");
        WINE_StringFromCLSID(riid,&xriid[strlen(xriid)]);
        TRACE("%s\n",xriid );

	if (!szDest) return FALSE;
	*szDest = SFGAO_FOLDER|SFGAO_FILESYSTEM;

        strcat (xriid, "\\ShellFolder");

        if (RegOpenKeyExA(HKEY_CLASSES_ROOT,xriid,0,KEY_READ,&hkey))
        {
          return FALSE;
        }

        if (RegQueryValueExA(hkey,"Attributes",0,NULL,(LPBYTE)&attributes,&len))
        {
          RegCloseKey(hkey);
          return FALSE;
        }

        RegCloseKey(hkey);

        TRACE("-- 0x%08lx\n", attributes);

        *szDest = attributes;

        return TRUE;
}

