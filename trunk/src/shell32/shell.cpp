/* $Id: shell.cpp,v 1.5 2000-03-26 16:34:43 cbratschi Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *             Shell Library Functions
 *
 *  1998 Marcus Meissner
 *
 * Corel WINE 20000324 level
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
#include "winversion.h"

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

static HWND  SHELL_hWnd = 0;
static HHOOK SHELL_hHook = 0;
static UINT  uMsgWndCreated = 0;
static UINT  uMsgWndDestroyed = 0;
static UINT  uMsgShellActivate = 0;
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
       LPSTR tmp;
       char param[256];
       LONG paramlen = 256;

       /* Get the parameters needed by the application
          from the associated ddeexec key */
       tmp = strstr(filetype,"command");
       tmp[0] = '\0';
       strcat(filetype,"ddeexec");

       if(RegQueryValueA( HKEY_CLASSES_ROOT, filetype, param,&paramlen ) == ERROR_SUCCESS)
       {
         strcat(command," ");
         strcat(command,param);
         commandlen += paramlen;
       }

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
#if 1
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
#if 0 //CB: DirectResAlloc16 not yet ported
/*************************************************************************
 *                      SHELL_LoadResource
 */
static HGLOBAL SHELL_LoadResource(HINSTANCE hInst, HFILE hFile, NE_NAMEINFO* pNInfo, WORD sizeShift)
{       BYTE*  ptr;
        HGLOBAL handle = DirectResAlloc( hInst, 0x10, (DWORD)pNInfo->length << sizeShift);

        TRACE("\n");

        if( (ptr = (BYTE*)GlobalLock( handle )) )
        { _llseek( hFile, (DWORD)pNInfo->offset << sizeShift, SEEK_SET);
          _lread( hFile, (char*)ptr, pNInfo->length << sizeShift);
          return handle;
        }
        return 0;
}

/*************************************************************************
 *                      ICO_LoadIcon
 */
static HGLOBAL ICO_LoadIcon(HINSTANCE hInst, HFILE hFile, LPicoICONDIRENTRY lpiIDE)
{       BYTE*  ptr;
        HGLOBAL handle = DirectResAlloc( hInst, 0x10, lpiIDE->dwBytesInRes);
        TRACE("\n");
        if( (ptr = (BYTE*)GlobalLock( handle )) )
        { _llseek( hFile, lpiIDE->dwImageOffset, SEEK_SET);
          _lread( hFile, (char*)ptr, lpiIDE->dwBytesInRes);
          return handle;
        }
        return 0;
}

/*************************************************************************
 *                      ICO_GetIconDirectory
 *
 *  Read .ico file and build phony ICONDIR struct for GetIconID
 */
static HGLOBAL ICO_GetIconDirectory(HINSTANCE hInst, HFILE hFile, LPicoICONDIR* lplpiID )
{ WORD    id[3];  /* idReserved, idType, idCount */
  LPicoICONDIR  lpiID;
  int           i;

  TRACE("\n");
  _llseek( hFile, 0, SEEK_SET );
  if( _lread(hFile,(char*)id,sizeof(id)) != sizeof(id) ) return 0;

  /* check .ICO header
   *
   * - see http://www.microsoft.com/win32dev/ui/icons.htm
   */

  if( id[0] || id[1] != 1 || !id[2] ) return 0;

  i = id[2]*sizeof(icoICONDIRENTRY) ;

  lpiID = (LPicoICONDIR)HeapAlloc( GetProcessHeap(), 0, i + sizeof(id));

  if( _lread(hFile,(char*)lpiID->idEntries,i) == i )
  { HGLOBAL handle = DirectResAlloc( hInst, 0x10,
                                     id[2]*sizeof(CURSORICONDIRENTRY) + sizeof(id) );
     if( handle )
    { CURSORICONDIR*     lpID = (CURSORICONDIR*)GlobalLock( handle );
       lpID->idReserved = lpiID->idReserved = id[0];
       lpID->idType = lpiID->idType = id[1];
       lpID->idCount = lpiID->idCount = id[2];
       for( i=0; i < lpiID->idCount; i++ )
      { memcpy((void*)(lpID->idEntries + i),
                   (void*)(lpiID->idEntries + i), sizeof(CURSORICONDIRENTRY) - 2);
            lpID->idEntries[i].wResId = i;
         }
      *lplpiID = lpiID;
       return handle;
     }
  }
  /* fail */

  HeapFree( GetProcessHeap(), 0, lpiID);
  return 0;
}
#endif
/*************************************************************************
 *                      InternalExtractIcon             [SHELL.39]
 *
 * This abortion is called directly by Progman
 */
HGLOBAL WINAPI InternalExtractIcon(HINSTANCE hInstance,
                                     LPCSTR lpszExeFileName, UINT nIconIndex, WORD n )
{       HGLOBAL hRet = 0;
        HGLOBAL*        RetPtr = NULL;
        LPBYTE          pData;
        OFSTRUCT        ofs;
        DWORD           sig;
        HFILE           hFile = OpenFile( lpszExeFileName, &ofs, OF_READ );
        UINT            iconDirCount = 0,iconCount = 0;
        LPBYTE          peimage;
        HANDLE  fmapping;

        TRACE("(%04x,file %s,start %d,extract %d\n",
                       hInstance, lpszExeFileName, nIconIndex, n);

        if( hFile == HFILE_ERROR || !n )
          return 0;

        hRet = GlobalAlloc( GMEM_FIXED | GMEM_ZEROINIT, sizeof(HICON16)*n);
        RetPtr = (HGLOBAL*)GlobalLock(hRet);

        *RetPtr = (n == 0xFFFF)? 0: 1;  /* error return values */

        sig = SHELL_GetResourceTable(hFile,&pData);
#if 0 //CB: some functions not (yet) supported
        if( sig==IMAGE_OS2_SIGNATURE || sig==1 ) /* .ICO file */
        { HICON  hIcon = 0;
          NE_TYPEINFO* pTInfo = (NE_TYPEINFO*)(pData + 2);
          NE_NAMEINFO* pIconStorage = NULL;
          NE_NAMEINFO* pIconDir = NULL;
          LPicoICONDIR lpiID = NULL;

          if( pData == (BYTE*)-1 )
          { hIcon = ICO_GetIconDirectory(hInstance, hFile, &lpiID);     /* check for .ICO file */
            if( hIcon )
            { iconDirCount = 1; iconCount = lpiID->idCount;
            }
          }
          else while( pTInfo->type_id && !(pIconStorage && pIconDir) )
          { if( pTInfo->type_id == NE_RSCTYPE_GROUP_ICON )      /* find icon directory and icon repository */
            { iconDirCount = pTInfo->count;
              pIconDir = ((NE_NAMEINFO*)(pTInfo + 1));
              TRACE("\tfound directory - %i icon families\n", iconDirCount);
            }
            if( pTInfo->type_id == NE_RSCTYPE_ICON )
            { iconCount = pTInfo->count;
              pIconStorage = ((NE_NAMEINFO*)(pTInfo + 1));
              TRACE("\ttotal icons - %i\n", iconCount);
            }
            pTInfo = (NE_TYPEINFO *)((char*)(pTInfo+1)+pTInfo->count*sizeof(NE_NAMEINFO));
          }

          /* load resources and create icons */

          if( (pIconStorage && pIconDir) || lpiID )
          { if( nIconIndex == (UINT16)-1 )
            { RetPtr[0] = iconDirCount;
            }
            else if( nIconIndex < iconDirCount )
            { UINT   i, icon;
              if( n > iconDirCount - nIconIndex )
                n = iconDirCount - nIconIndex;

              for( i = nIconIndex; i < nIconIndex + n; i++ )
              { /* .ICO files have only one icon directory */

                if( lpiID == NULL )
                  hIcon = SHELL_LoadResource( hInstance, hFile, pIconDir + i, *(WORD*)pData );
                RetPtr[i-nIconIndex] = GetIconID( hIcon, 3 );
                GlobalFree(hIcon);
              }

              for( icon = nIconIndex; icon < nIconIndex + n; icon++ )
              { hIcon = 0;
                if( lpiID )
                { hIcon = ICO_LoadIcon( hInstance, hFile, lpiID->idEntries + RetPtr[icon-nIconIndex]);
                }
                else
                { for( i = 0; i < iconCount; i++ )
                  { if( pIconStorage[i].id == (RetPtr[icon-nIconIndex] | 0x8000) )
                    { hIcon = SHELL_LoadResource( hInstance, hFile, pIconStorage + i,*(WORD*)pData );
                    }
                  }
                }
                if( hIcon )
                { RetPtr[icon-nIconIndex] = LoadIconHandler16( hIcon, TRUE );
                  FarSetOwner16( RetPtr[icon-nIconIndex], GetExePtr(hInstance) );
                }
                else
                { RetPtr[icon-nIconIndex] = 0;
                }
              }
            }
          }
          if( lpiID )
            HeapFree( GetProcessHeap(), 0, lpiID);
          else
            HeapFree( GetProcessHeap(), 0, pData);
        }
#endif
        if( sig == IMAGE_NT_SIGNATURE)
        { LPBYTE                idata,igdata;
          PIMAGE_DOS_HEADER     dheader;
          PIMAGE_NT_HEADERS     pe_header;
          PIMAGE_SECTION_HEADER pe_sections;
          PIMAGE_RESOURCE_DIRECTORY     rootresdir,iconresdir,icongroupresdir;
          PIMAGE_RESOURCE_DATA_ENTRY    idataent,igdataent;
          int                   i,j;
          PIMAGE_RESOURCE_DIRECTORY_ENTRY       xresent;
          CURSORICONDIR         **cids;

          fmapping = CreateFileMappingA(hFile,NULL,PAGE_READONLY|SEC_COMMIT,0,0,NULL);
          if (fmapping == 0)
          { /* FIXME, INVALID_HANDLE_VALUE? */
            WARN("failed to create filemap.\n");
            hRet = 0;
            goto end_2; /* failure */
          }
          peimage = (BYTE*)MapViewOfFile(fmapping,FILE_MAP_READ,0,0,0);
          if (!peimage)
          { WARN("failed to mmap filemap.\n");
            hRet = 0;
            goto end_2; /* failure */
          }
          dheader = (PIMAGE_DOS_HEADER)peimage;

          /* it is a pe header, SHELL_GetResourceTable checked that */
          pe_header = (PIMAGE_NT_HEADERS)(peimage+dheader->e_lfanew);

          /* probably makes problems with short PE headers... but I haven't seen
          * one yet...
          */
          pe_sections = (PIMAGE_SECTION_HEADER)(((char*)pe_header)+sizeof(*pe_header));
          rootresdir = NULL;

          for (i=0;i<pe_header->FileHeader.NumberOfSections;i++)
          { if (pe_sections[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
              continue;
            /* FIXME: doesn't work when the resources are not in a seperate section */
            if (pe_sections[i].VirtualAddress == pe_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress)
            { rootresdir = (PIMAGE_RESOURCE_DIRECTORY)((char*)peimage+pe_sections[i].PointerToRawData);
              break;
            }
          }

          if (!rootresdir)
          { WARN("haven't found section for resource directory.\n");
            goto end_4; /* failure */
          }

          icongroupresdir = GetResDirEntryW(rootresdir,RT_GROUP_ICONW, (DWORD)rootresdir,FALSE);

          if (!icongroupresdir)
          { WARN("No Icongroupresourcedirectory!\n");
            goto end_4; /* failure */
          }

          iconDirCount = icongroupresdir->NumberOfNamedEntries+icongroupresdir->NumberOfIdEntries;

          if( nIconIndex == (UINT)-1 )
          { RetPtr[0] = iconDirCount;
            goto end_3; /* success */
          }

          if (nIconIndex >= iconDirCount)
          { WARN("nIconIndex %d is larger than iconDirCount %d\n",nIconIndex,iconDirCount);
            GlobalFree(hRet);
            goto end_4; /* failure */
          }

          cids = (CURSORICONDIR**)HeapAlloc(GetProcessHeap(),0,n*sizeof(CURSORICONDIR*));

          /* caller just wanted the number of entries */
          xresent = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(icongroupresdir+1);

          /* assure we don't get too much ... */
          if( n > iconDirCount - nIconIndex )
          { n = iconDirCount - nIconIndex;
          }

          /* starting from specified index ... */
          xresent = xresent+nIconIndex;

          for (i=0;i<n;i++,xresent++)
          { CURSORICONDIR       *cid;
            PIMAGE_RESOURCE_DIRECTORY   resdir;

            /* go down this resource entry, name */
            resdir = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)rootresdir+(xresent->u2.s.OffsetToDirectory));

            /* default language (0) */
            resdir = GetResDirEntryW(resdir,(LPWSTR)0,(DWORD)rootresdir,TRUE);
            igdataent = (PIMAGE_RESOURCE_DATA_ENTRY)resdir;

            /* lookup address in mapped image for virtual address */
            igdata = NULL;

            for (j=0;j<pe_header->FileHeader.NumberOfSections;j++)
            { if (igdataent->OffsetToData < pe_sections[j].VirtualAddress)
                continue;
              if (igdataent->OffsetToData+igdataent->Size > pe_sections[j].VirtualAddress+pe_sections[j].SizeOfRawData)
                continue;
              igdata = peimage+(igdataent->OffsetToData-pe_sections[j].VirtualAddress+pe_sections[j].PointerToRawData);
            }

            if (!igdata)
            { WARN("no matching real address for icongroup!\n");
              goto end_4;       /* failure */
            }
            /* found */
            cid = (CURSORICONDIR*)igdata;
            cids[i] = cid;
            RetPtr[i] = LookupIconIdFromDirectoryEx(igdata,TRUE,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),0);
          }

          iconresdir=GetResDirEntryW(rootresdir,RT_ICONW,(DWORD)rootresdir,FALSE);

          if (!iconresdir)
          { WARN("No Iconresourcedirectory!\n");
            goto end_4; /* failure */
          }

          for (i=0;i<n;i++)
          { PIMAGE_RESOURCE_DIRECTORY   xresdir;
            xresdir = GetResDirEntryW(iconresdir,(LPWSTR)(DWORD)RetPtr[i],(DWORD)rootresdir,FALSE);
            xresdir = GetResDirEntryW(xresdir,(LPWSTR)0,(DWORD)rootresdir,TRUE);
            idataent = (PIMAGE_RESOURCE_DATA_ENTRY)xresdir;
            idata = NULL;

            /* map virtual to address in image */
            for (j=0;j<pe_header->FileHeader.NumberOfSections;j++)
            { if (idataent->OffsetToData < pe_sections[j].VirtualAddress)
                continue;
              if (idataent->OffsetToData+idataent->Size > pe_sections[j].VirtualAddress+pe_sections[j].SizeOfRawData)
                continue;
              idata = peimage+(idataent->OffsetToData-pe_sections[j].VirtualAddress+pe_sections[j].PointerToRawData);
            }
            if (!idata)
            { WARN("no matching real address found for icondata!\n");
              RetPtr[i]=0;
              continue;
            }
            RetPtr[i] = CreateIconFromResourceEx(idata,idataent->Size,TRUE,0x00030000,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),0);
          }
          goto end_3;   /* sucess */
        }
        goto end_1;     /* return array with icon handles */

/* cleaning up (try & catch would be nicer) */
end_4:  hRet = 0;       /* failure */
end_3:  UnmapViewOfFile(peimage);       /* success */
end_2:  CloseHandle(fmapping);
end_1:  _lclose( hFile);
        return hRet;
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
       UINT  uRet = FindExecutableA(lpIconPath,NULL,tempPath);

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

