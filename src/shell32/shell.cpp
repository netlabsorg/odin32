/* $Id: shell.cpp,v 1.4 2000-01-27 21:50:45 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *             Shell Library Functions
 *
 *  1998 Marcus Meissner
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <ctype.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include <winbase.h>
#include <winuser.h>
//#include "wine/shell16.h"
#include "winerror.h"
//#include "file.h"
#include "heap.h"
#include "ldt.h"
#include "module.h"
#include "neexe.h"
#include "dlgs.h"
#include "cursoricon.h"
#include "shellapi.h"
#include "shlobj.h"
#include "debugtools.h"
#include "winreg.h"
//#include "syslevel.h"
#include "imagelist.h"
#include "shell32_main.h"

#include <heapstring.h>
#include <misc.h>


/*****************************************************************************
 * Implementation                                                            *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-SHELL)


/* .ICO file ICONDIR definitions */

#include "pshpack1.h"

typedef struct
{
    BYTE        bWidth;          /* Width, in pixels, of the image  */
    BYTE        bHeight;         /* Height, in pixels, of the image */
    BYTE        bColorCount;     /* Number of colors in image (0 if >=8bpp) */
    BYTE        bReserved;       /* Reserved ( must be 0)     */
    WORD        wPlanes;         /* Color Planes        */
    WORD        wBitCount;       /* Bits per pixel         */
    DWORD       dwBytesInRes;    /* How many bytes in this resource?   */
    DWORD       dwImageOffset;   /* Where in the file is this image?   */
} icoICONDIRENTRY, *LPicoICONDIRENTRY;

typedef struct
{
    WORD            idReserved;   /* Reserved (must be 0)     */
    WORD            idType;       /* Resource Type (1 for icons) */
    WORD            idCount;      /* How many images?         */
    icoICONDIRENTRY idEntries[1]; /* An entry for each image (idCount of 'em) */
} icoICONDIR, *LPicoICONDIR;

#include "poppack.h"

static const char* lpstrMsgWndCreated    = "OTHERWINDOWCREATED";
static const char* lpstrMsgWndDestroyed  = "OTHERWINDOWDESTROYED";
static const char* lpstrMsgShellActivate = "ACTIVATESHELLWINDOW";

static HWND16  SHELL_hWnd = 0;
static HHOOK   SHELL_hHook = 0;
static UINT16  uMsgWndCreated = 0;
static UINT16  uMsgWndDestroyed = 0;
static UINT16  uMsgShellActivate = 0;
HINSTANCE16 SHELL_hInstance = 0;
HINSTANCE SHELL_hInstance32;
static int SHELL_Attach = 0;


/*************************************************************************
 *          DragAcceptFiles32               [SHELL32.54]
 */

//SvL: DON'T USE ODINFUNCTION MACRO'S HERE; SetWindowLong (style) sends messages
//     to the win32 app!!!! (FS messed up)
void WINAPI DragAcceptFiles(HWND hWnd, BOOL b)
{
  LONG exstyle;

  dprintf(("DragAcceptFiles %x %d", hWnd, b));
  if( !IsWindow(hWnd) )
   return;
  exstyle = GetWindowLongA(hWnd,GWL_EXSTYLE);
  if (b)exstyle |= WS_EX_ACCEPTFILES;
  else   exstyle &= ~WS_EX_ACCEPTFILES;
  SetWindowLongA(hWnd,GWL_EXSTYLE,exstyle);
}

/*************************************************************************
 *          SHELL_DragQueryFile          [internal]
 *
 */
static UINT SHELL_DragQueryFile(LPSTR lpDrop, LPWSTR lpwDrop, UINT lFile,
              LPSTR lpszFile, LPWSTR lpszwFile, UINT lLength)
{
    UINT i;

    i = 0;
    if (lpDrop) {
    while (i++ < lFile) {
   while (*lpDrop++); /* skip filename */
   if (!*lpDrop)
     return (lFile == 0xFFFFFFFF) ? i : 0;
      }
    }
    if (lpwDrop) {
      while (i++ < lFile) {
   while (*lpwDrop++); /* skip filename */
   if (!*lpwDrop)
     return (lFile == 0xFFFFFFFF) ? i : 0;
      }
    }

    if (lpDrop)  i = lstrlenA(lpDrop);
    if (lpwDrop) i = lstrlenW(lpwDrop);
    i++;
    if (!lpszFile && !lpszwFile) {
      return i;   /* needed buffer size */
    }
    i = (lLength > i) ? i : lLength;
    if (lpszFile) {
      if (lpDrop) lstrcpynA (lpszFile,  lpDrop,  i);
      else        lstrcpynWtoA(lpszFile,  lpwDrop, i);
    } else {
      if (lpDrop) lstrcpynAtoW(lpszwFile, lpDrop,  i);
      else        lstrcpynW (lpszwFile, lpwDrop, i);
    }
    return i;
}

/*************************************************************************
 *          DragQueryFile32A             [SHELL32.81] [shell32.82]
 */

ODINFUNCTION4(UINT, DragQueryFileA, HDROP, hDrop,
                                    UINT,  lFile,
                                    LPSTR, lpszFile,
                                    UINT,  lLength)
{ /* hDrop is a global memory block allocated with GMEM_SHARE
     * with DROPFILESTRUCT as a header and filenames following
     * it, zero length filename is in the end */

    LPDROPFILESTRUCT lpDropFileStruct;
    LPSTR lpCurrent;
    UINT i;

    lpDropFileStruct = (LPDROPFILESTRUCT) GlobalLock(hDrop);
    if(!lpDropFileStruct)
      return 0;

    lpCurrent = (LPSTR) lpDropFileStruct + lpDropFileStruct->lSize;
    i = SHELL_DragQueryFile(lpCurrent, NULL, lFile, lpszFile, NULL, lLength);
    GlobalUnlock(hDrop);
    return i;
}

/*************************************************************************
 *          DragQueryFile32W             [shell32.133]
 */
ODINFUNCTION4(UINT, DragQueryFileW, HDROP,  hDrop,
                                    UINT,   lFile,
                                    LPWSTR, lpszwFile,
                                    UINT,   lLength)
{
    LPDROPFILESTRUCT lpDropFileStruct;
    LPWSTR lpwCurrent;
    UINT i;

    lpDropFileStruct = (LPDROPFILESTRUCT) GlobalLock(hDrop);
    if(!lpDropFileStruct)
      return 0;

    lpwCurrent = (LPWSTR) lpDropFileStruct + lpDropFileStruct->lSize;
    i = SHELL_DragQueryFile(NULL, lpwCurrent, lFile, NULL, lpszwFile,lLength);
    GlobalUnlock(hDrop);
    return i;
}


/*****************************************************************************
 * Name      : UINT DragQueryFileAorW
 * Purpose   :
 * Parameters: HDROP  hDrop    - drop structure handle
 *             UINT   iFile    - index of file to query
 *             LPTSTR lpszFile - target buffer
 *             UINT   cch      - target buffer size
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:00]
 *****************************************************************************/

ODINFUNCTION4(UINT,   DragQueryFileAorW,
              HDROP,  hDrop,
              UINT,   iFile,
              LPTSTR, lpszFile,
              UINT,   cch)
{
  // @@@PH maybe they want automatic determination here
  if (VERSION_OsIsUnicode())
    return DragQueryFileW(hDrop, iFile, (LPWSTR)lpszFile, cch);
  else
    return DragQueryFileA(hDrop, iFile, lpszFile, cch);
}


/*************************************************************************
 *          DragFinish32                    [SHELL32.80]
 */
ODINPROCEDURE1(DragFinish, HDROP, h)
{
  GlobalFree((HGLOBAL)h);
}


/*************************************************************************
 *          DragQueryPoint32                [SHELL32.135]
 */
ODINFUNCTION2(BOOL,   DragQueryPoint,
              HDROP,  hDrop,
              POINT*, p)
{
  LPDROPFILESTRUCT lpDropFileStruct;
  BOOL             bRet;

  lpDropFileStruct = (LPDROPFILESTRUCT) GlobalLock(hDrop);

  memcpy(p,&lpDropFileStruct->ptMousePos,sizeof(POINT));
  bRet = lpDropFileStruct->fInNonClientArea;

  GlobalUnlock(hDrop);
  return bRet;
}

/*************************************************************************
 * SHELL_FindExecutable [Internal]
 *
 * Utility for code sharing between FindExecutable and ShellExecute
 */
HINSTANCE SHELL_FindExecutable( LPCSTR lpFile,
                                         LPCSTR lpOperation,
                                         LPSTR lpResult)
{ char *extension = NULL; /* pointer to file extension */
    char tmpext[5];         /* local copy to mung as we please */
    char filetype[256];     /* registry name for this filetype */
    LONG filetypelen=256;   /* length of above */
    char command[256];      /* command from registry */
    LONG commandlen=256;    /* This is the most DOS can handle :) */
    char buffer[256];       /* Used to GetProfileString */
    HINSTANCE retval=31;  /* default - 'No association was found' */
    char *tok;              /* token pointer */
    int i;                  /* random counter */
    char xlpFile[256];      /* result of SearchPath */

    dprintf(("SHELL32:SHELL:SHELL_FindExecutable(%s,%s,%08xh)\n",
             lpFile,
             lpOperation,
             lpResult));

    lpResult[0]='\0'; /* Start off with an empty return string */

    /* trap NULL parameters on entry */
    if (( lpFile == NULL ) || ( lpResult == NULL ) || ( lpOperation == NULL ))
  { dprintf(("SHELL32:SHELL:SHELL_FindExecutable(lpFile=%s,lpResult=%s,lpOperation=%s): NULL parameter\n",
           lpFile, lpOperation, lpResult));
        return 2; /* File not found. Close enough, I guess. */
    }

    if (SearchPathA( NULL, lpFile,".exe",sizeof(xlpFile),xlpFile,NULL))
  {  dprintf(("SHELL32:SHELL:SHELL_FindExecutable(SearchPath32A returned non-zero\n"));
        lpFile = xlpFile;
    }

    /* First thing we need is the file's extension */
    extension = strrchr( xlpFile, '.' ); /* Assume last "." is the one; */
               /* File->Run in progman uses */
               /* .\FILE.EXE :( */
    dprintf(("SHELL32:SHELL:SHELL_FindExecutable(xlpFile=%s,extension=%s)\n",
             xlpFile, extension));

    if ((extension == NULL) || (extension == &xlpFile[strlen(xlpFile)]))
  { dprintf(("SHELL32:SHELL:SHELL_FindExecutable Returning 31 - No association\n"));
        return 31; /* no association */
    }

    /* Make local copy & lowercase it for reg & 'programs=' lookup */
    lstrcpynA( tmpext, extension, 5 );
    CharLowerA( tmpext );


    dprintf(("SHELL32:SHELL:SHELL_FindExecutable(%s file)\n", tmpext));

    /* Three places to check: */
    /* 1. win.ini, [windows], programs (NB no leading '.') */
    /* 2. Registry, HKEY_CLASS_ROOT\<filetype>\shell\open\command */
    /* 3. win.ini, [extensions], extension (NB no leading '.' */
    /* All I know of the order is that registry is checked before */
    /* extensions; however, it'd make sense to check the programs */
    /* section first, so that's what happens here. */

    /* See if it's a program - if GetProfileString fails, we skip this
     * section. Actually, if GetProfileString fails, we've probably
     * got a lot more to worry about than running a program... */
    if ( GetProfileStringA("windows", "programs", "exe pif bat com",
                    buffer, sizeof(buffer)) > 0 )
  { for (i=0;i<strlen(buffer); i++) buffer[i]=tolower(buffer[i]);

      tok = strtok(buffer, " \t"); /* ? */
      while( tok!= NULL)
        {
         if (strcmp(tok, &tmpext[1])==0) /* have to skip the leading "." */
           {
            strcpy(lpResult, xlpFile);
            /* Need to perhaps check that the file has a path
             * attached */
            dprintf(("SHELL32:SHELL:SHELL_FindExecutable found %s\n",
                     lpResult));
            return 33;

      /* Greater than 32 to indicate success FIXME According to the
       * docs, I should be returning a handle for the
       * executable. Does this mean I'm supposed to open the
       * executable file or something? More RTFM, I guess... */
           }
         tok=strtok(NULL, " \t");
        }
     }

    /* Check registry */
    if (RegQueryValueA( HKEY_CLASSES_ROOT, tmpext, filetype,
                         &filetypelen ) == ERROR_SUCCESS )
    {
   filetype[filetypelen]='\0';
    dprintf(("SHELL32:SHELL:SHELL_FindExecutable(File type: %s)\n",
             filetype));

   /* Looking for ...buffer\shell\lpOperation\command */
   strcat( filetype, "\\shell\\" );
   strcat( filetype, lpOperation );
   strcat( filetype, "\\command" );

   if (RegQueryValueA( HKEY_CLASSES_ROOT, filetype, command,
                             &commandlen ) == ERROR_SUCCESS )
   {
       /* Is there a replace() function anywhere? */
       command[commandlen]='\0';
       strcpy( lpResult, command );
       tok=strstr( lpResult, "%1" );
       if (tok != NULL)
       {
      tok[0]='\0'; /* truncate string at the percent */
      strcat( lpResult, xlpFile ); /* what if no dir in xlpFile? */
      tok=strstr( command, "%1" );
      if ((tok!=NULL) && (strlen(tok)>2))
      {
          strcat( lpResult, &tok[2] );
      }
       }
       retval=33; /* FIXME see above */
   }
    }
    else /* Check win.ini */
    {
   /* Toss the leading dot */
   extension++;
   if ( GetProfileStringA( "extensions", extension, "", command,
                                  sizeof(command)) > 0)
     {
      if (strlen(command)!=0)
        {
         strcpy( lpResult, command );
         tok=strstr( lpResult, "^" ); /* should be ^.extension? */
         if (tok != NULL)
           {
            tok[0]='\0';
            strcat( lpResult, xlpFile ); /* what if no dir in xlpFile? */
            tok=strstr( command, "^" ); /* see above */
            if ((tok != NULL) && (strlen(tok)>5))
              {
               strcat( lpResult, &tok[5]);
              }
           }
         retval=33; /* FIXME - see above */
        }
     }
   }

    dprintf(("SHELL32:SHELL:SHELL_FindExecutable (returning %s)\n", lpResult));
    return retval;
}


/*************************************************************************
 *          SHELL_GetResourceTable
 */
static DWORD SHELL_GetResourceTable(HFILE hFile,LPBYTE *retptr)
{  IMAGE_DOS_HEADER                      mz_header;
   char        magic[4];
   int         size;

   dprintf(("SHELL32:Shell:SHELL_GetResourceTable(%08xh,%08xh)\n",
            hFile,
            retptr));

   *retptr = NULL;
   _llseek( hFile, 0, SEEK_SET );
   if ((_lread(hFile,&mz_header,sizeof(mz_header)) != sizeof(mz_header)) || (mz_header.e_magic != IMAGE_DOS_SIGNATURE))
   { /* .ICO file ? */
          if (mz_header.e_cblp == 1)
     { /* ICONHEADER.idType, must be 1 */
       *retptr = (LPBYTE)-1;
       return 1;
     }
     else
       return 0; /* failed */
   }
   _llseek( hFile, mz_header.e_lfanew, SEEK_SET );

   if (_lread( hFile, magic, sizeof(magic) ) != sizeof(magic))
     return 0;

   _llseek( hFile, mz_header.e_lfanew, SEEK_SET);

   if (*(DWORD*)magic  == IMAGE_NT_SIGNATURE)
     return IMAGE_NT_SIGNATURE;

   if (*(WORD*)magic == IMAGE_OS2_SIGNATURE)
   { IMAGE_OS2_HEADER                    ne_header;
     LPBYTE    pTypeInfo = (LPBYTE)-1;

     if (_lread(hFile,&ne_header,sizeof(ne_header))!=sizeof(ne_header))
       return 0;

     if (ne_header.ne_magic != IMAGE_OS2_SIGNATURE)
       return 0;

     size = ne_header.rname_tab_offset - ne_header.resource_tab_offset;

//@@@PH no NE support
#if 0
     if( size > sizeof(NE_TYPEINFO) )
     { pTypeInfo = (BYTE*)HeapAlloc( GetProcessHeap(), 0, size);
       if( pTypeInfo )
       { _llseek(hFile, mz_header.e_lfanew+ne_header.resource_tab_offset, SEEK_SET);
         if( _lread( hFile, (char*)pTypeInfo, size) != size )
         { HeapFree( GetProcessHeap(), 0, pTypeInfo);
      pTypeInfo = NULL;
         }
       }
     }
#endif

     *retptr = pTypeInfo;
     return IMAGE_OS2_SIGNATURE;
   }
   return 0; /* failed */
}


/*************************************************************************
 *          ExtractAssociatedIcon        [SHELL.36]
 *
 * Return icon for given file (either from file itself or from associated
 * executable) and patch parameters if needed.
 */
ODINFUNCTION3(HICON,     ExtractAssociatedIconA,
              HINSTANCE, hInst,
              LPSTR,     lpIconPath,
              LPWORD,    lpiIcon)
{
   HICON hIcon;

   hIcon = ExtractIconA(hInst, lpIconPath, *lpiIcon);

   if( hIcon < 2 )
   { if( hIcon == 1 ) /* no icons found in given file */
     { char  tempPath[0x104];
       UINT16  uRet = FindExecutableA(lpIconPath,NULL,tempPath);

       if( uRet > 32 && tempPath[0] )
       { strcpy(lpIconPath,tempPath);
         hIcon = ExtractIconA(hInst, lpIconPath, *lpiIcon);
         if( hIcon > 2 )
           return hIcon;
       }
       else hIcon = 0;
     }

     if( hIcon == 1 )
       *lpiIcon = 2;   /* MSDOS icon - we found .exe but no icons in it */
     else
       *lpiIcon = 6;   /* generic icon - found nothing */

     GetModuleFileNameA(hInst, lpIconPath, 0x80);
     hIcon = LoadIconA( hInst, (LPCSTR)*lpiIcon);
   }
   return hIcon;
}

