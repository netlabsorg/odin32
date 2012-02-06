/* $Id: shell32_odin.cpp,v 1.5 2002-06-09 12:41:22 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *             Shell basics
 *
 *  1998 Marcus Meissner
 *  1998 Juergen Schmied (jsch)  *  <juergen.schmied@metronet.de>
 *
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <stdlib.h>
#include <string.h>

#define CINTERFACE

#include "winerror.h"
#include "heap.h"
#include "resource.h"
#include "dlgs.h"
#include "sysmetrics.h"
#include "debugtools.h"
#include "winreg.h"
#include "authors.h"
#include "winversion.h"

#include "shellapi.h"
#include "pidl.h"

#include "shlobj.h"
#include "shell32_main.h"
#include "shlguid.h"
#include "undocshell.h"
#include "shpolicy.h"
#include "shlwapi.h"

#include <heapstring.h>
#include <misc.h>
#include <peexe.h>
#include <neexe.h>
#include <cursoricon.h>
#include <ctype.h>
#include <module.h>

#include "os2_integration.h"
#include "odinbuild.h"


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-MAIN)

typedef struct
{ LPCSTR  szApp;
    LPCSTR  szOtherStuff;
    HICON hIcon;
} ABOUT_INFO;

#define		IDC_STATIC_TEXT		100
#define		IDC_LISTBOX		99
#define		IDC_WINE_TEXT		98
#define         IDC_ODIN_TEXT           98

#define		DROP_FIELD_TOP		(-15)
#define		DROP_FIELD_HEIGHT	15

extern HICON hIconTitleFont;

static BOOL __get_dropline( HWND hWnd, LPRECT lprect )
{ HWND hWndCtl = GetDlgItem(hWnd, IDC_WINE_TEXT);
    if( hWndCtl )
  { GetWindowRect( hWndCtl, lprect );
	MapWindowPoints( 0, hWnd, (LPPOINT)lprect, 2 );
	lprect->bottom = (lprect->top += DROP_FIELD_TOP);
	return TRUE;
    }
    return FALSE;
}


/*************************************************************************
 * AboutDlgProc32                           (internal)
 */
#define IDC_ODINLOGO    2001
#define IDC_ODINBUILDNR 98
#define IDB_ODINLOGO    5555

BOOL WINAPI AboutDlgProc( HWND hWnd, UINT msg, WPARAM wParam,
                              LPARAM lParam )
{
    HWND hWndCtl;
    char Template[512], AppTitle[512];

    switch(msg)
    {
    case WM_INITDIALOG:
    {
        ABOUT_INFO *info = (ABOUT_INFO *)lParam;
        if(info)
        {
          const char* const *pstr = SHELL_People;

            SendDlgItemMessageA(hWnd, stc1, STM_SETICON,info->hIcon, 0);
            GetWindowTextA( hWnd, Template, sizeof(Template) );
            sprintf( AppTitle, Template, info->szApp );
            SetWindowTextA( hWnd, AppTitle );
            SetWindowTextA( GetDlgItem(hWnd, IDC_STATIC_TEXT), info->szOtherStuff );

            GetWindowTextA( GetDlgItem(hWnd, IDC_ODINBUILDNR), Template, sizeof(Template) );
            sprintf( AppTitle, Template, ODIN32_VERSION_MAJOR,
                     ODIN32_VERSION_MINOR, ODIN32_BUILD_NR, __DATE__);
            SetWindowTextA( GetDlgItem(hWnd, IDC_ODINBUILDNR), AppTitle);
            HWND hwndOdinLogo = GetDlgItem(hWnd, IDC_ODINLOGO);
            if(hwndOdinLogo) {
                        HBITMAP hBitmap = LoadBitmapA(shell32_hInstance, MAKEINTRESOURCEA(IDB_ODINLOGO));
                        SendMessageA(hwndOdinLogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
            }

            hWndCtl = GetDlgItem(hWnd, IDC_LISTBOX);
            SendMessageA( hWndCtl, WM_SETREDRAW, 0, 0 );
#if 0 //CB: not used (hIconTitleFont not valid!!!), default font is ok
            SendMessageA( hWndCtl, WM_SETFONT, hIconTitleFont, 0 );
#endif
            while (*pstr)
            {
                SendMessageA( hWndCtl, LB_ADDSTRING, (WPARAM)-1, (LPARAM)*pstr );
                pstr++;
            }
            SendMessageA( hWndCtl, WM_SETREDRAW, 1, 0 );
        }
        return 1;
    }

    case WM_PAINT:
    {
        RECT rect;
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint( hWnd, &ps );

        if( __get_dropline( hWnd, &rect ) ) {
            SelectObject( hDC, GetStockObject( BLACK_PEN ) );
            MoveToEx( hDC, rect.left, rect.top, NULL );
            LineTo( hDC, rect.right, rect.bottom );
        }
        EndPaint( hWnd, &ps );
        break;
   }

    case WM_LBTRACKPOINT:
   hWndCtl = GetDlgItem(hWnd, IDC_LISTBOX);
   if( (INT)GetKeyState( VK_CONTROL ) < 0 )
      { if( DragDetect( hWndCtl, *((LPPOINT)&lParam) ) )
        { INT idx = SendMessageA( hWndCtl, LB_GETCURSEL, 0, 0 );
      if( idx != -1 )
          { INT length = SendMessageA( hWndCtl, LB_GETTEXTLEN, (WPARAM)idx, 0 );
          HGLOBAL hMemObj = GlobalAlloc( GMEM_MOVEABLE, length + 1 );
          char* pstr = (char*)GlobalLock( hMemObj );

          if( pstr )
            { HCURSOR hCursor = LoadCursorA( 0, (LPCSTR)OCR_DRAGOBJECT );
         SendMessageA( hWndCtl, LB_GETTEXT, (WPARAM)idx, (LPARAM)pstr );
         SendMessageA( hWndCtl, LB_DELETESTRING, (WPARAM)idx, 0 );
         UpdateWindow( hWndCtl );

         if( !DragObject(hWnd, hWnd, DRAGOBJ_DATA, hMemObj, hCursor) )
             SendMessageA( hWndCtl, LB_ADDSTRING, (WPARAM)-1, (LPARAM)pstr );
          }
            if( hMemObj )
              GlobalFree( hMemObj );
      }
       }
   }
   break;

    case WM_QUERYDROPOBJECT:
   if( wParam == 0 )
      { LPDRAGINFO lpDragInfo = (LPDRAGINFO)lParam;
       if( lpDragInfo && lpDragInfo->wFlags == DRAGOBJ_DATA )
        { RECT rect;
      if( __get_dropline( hWnd, &rect ) )
          { POINT pt;
       pt.x=lpDragInfo->pt.x;
       pt.x=lpDragInfo->pt.y;
          rect.bottom += DROP_FIELD_HEIGHT;
          if( PtInRect( &rect, pt ) )
            { SetWindowLongA( hWnd, DWL_MSGRESULT, 1 );
         return TRUE;
          }
      }
       }
   }
   break;

    case WM_DROPOBJECT:
   if( wParam == hWnd )
      { LPDRAGINFO lpDragInfo = (LPDRAGINFO)lParam;
       if( lpDragInfo && lpDragInfo->wFlags == DRAGOBJ_DATA && lpDragInfo->hList )
        { char* pstr = (char*)GlobalLock( (HGLOBAL)(lpDragInfo->hList) );
      if( pstr )
          { static char __appendix_str[] = " with";

          hWndCtl = GetDlgItem( hWnd, IDC_ODIN_TEXT );
          SendMessageA( hWndCtl, WM_GETTEXT, 512, (LPARAM)Template );
          if( !strncmp( Template, "ODIN", 4 ) )
         SetWindowTextA( GetDlgItem(hWnd, IDC_STATIC_TEXT), Template );
          else
          { char* pch = Template + strlen(Template) - strlen(__appendix_str);
         *pch = '\0';
         SendMessageA( GetDlgItem(hWnd, IDC_LISTBOX), LB_ADDSTRING,
               (WPARAM)-1, (LPARAM)Template );
          }

          strcpy( Template, pstr );
          strcat( Template, __appendix_str );
          SetWindowTextA( hWndCtl, Template );
          SetWindowLongA( hWnd, DWL_MSGRESULT, 1 );
          return TRUE;
      }
       }
   }
   break;

    case WM_COMMAND:
        if (wParam == IDOK)
        {
            EndDialog(hWnd, TRUE);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hWnd, TRUE);
        break;
    }
    return 0;
}


/*************************************************************************
 * ShellAboutA                              [SHELL32.243]
 */

BOOL WIN32API ShellAboutA(HWND hWnd, LPCSTR szApp, LPCSTR szOtherStuff,
                          HICON  hIcon )
{   ABOUT_INFO info;
    HRSRC hRes;
    LPVOID dlgTemplate;

    if(!(hRes = FindResourceA(shell32_hInstance, "SHELL_ABOUT_MSGBOX", RT_DIALOGA)))
        return FALSE;
    if(!(dlgTemplate = (LPVOID)LoadResource(shell32_hInstance, hRes)))
        return FALSE;

    info.szApp        = szApp;
    info.szOtherStuff = szOtherStuff;
    info.hIcon        = hIcon;
    if (!hIcon) info.hIcon = LoadIconA( 0, MAKEINTRESOURCEA(OIC_ODINICON) );
    return DialogBoxIndirectParamA( GetWindowLongA( hWnd, GWL_HINSTANCE ),
                                    (DLGTEMPLATE*)dlgTemplate , hWnd, AboutDlgProc, (LPARAM)&info );
}


/*************************************************************************
 * ShellAboutW                              [SHELL32.244]
 */
BOOL WIN32API ShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff,
                          HICON  hIcon )
{   INT ret;
    ABOUT_INFO info;
    HRSRC hRes;
    LPVOID dlgTemplate;

    if(!(hRes = FindResourceA(shell32_hInstance, "SHELL_ABOUT_MSGBOX", RT_DIALOGA)))
        return FALSE;
    if(!(dlgTemplate = (LPVOID)LoadResource(shell32_hInstance, hRes)))
        return FALSE;

    info.szApp        = HEAP_strdupWtoA( GetProcessHeap(), 0, szApp );
    info.szOtherStuff = HEAP_strdupWtoA( GetProcessHeap(), 0, szOtherStuff );
    info.hIcon        = hIcon;
    if (!hIcon) info.hIcon = LoadIconA( 0, MAKEINTRESOURCEA(OIC_ODINICON) );
    ret = DialogBoxIndirectParamA( GetWindowLongA( hWnd, GWL_HINSTANCE ),
                                   (DLGTEMPLATE*)dlgTemplate, hWnd, AboutDlgProc, (LPARAM)&info );
    HeapFree( GetProcessHeap(), 0, (LPSTR)info.szApp );
    HeapFree( GetProcessHeap(), 0, (LPSTR)info.szOtherStuff );
    return ret;
}

/*************************************************************************
 *          ExtractAssociatedIcon        [SHELL.36]
 *
 * Return icon for given file (either from file itself or from associated
 * executable) and patch parameters if needed.
 */
HICON WIN32API ExtractAssociatedIconA(HINSTANCE hInst, LPSTR lpIconPath,
                                      LPWORD lpiIcon)
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

//iconcache.c
DWORD SHELL_GetResourceTable(HFILE hFile, LPBYTE *retptr);

//CB: from loader/pe_resource.c

/**********************************************************************
 *          GetResDirEntryW
 *
 *      Helper function - goes down one level of PE resource tree
 *
 */
PIMAGE_RESOURCE_DIRECTORY GetResDirEntryW(PIMAGE_RESOURCE_DIRECTORY resdirptr,
                                           LPCWSTR name,DWORD root,
                                           BOOL allowdefault)
{
    int entrynum;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY entryTable;
    int namelen;

    if (HIWORD(name)) {
        if (name[0]=='#') {
                char    buf[10];

                lstrcpynWtoA(buf,name+1,10);
                return GetResDirEntryW(resdirptr,(LPCWSTR)atoi(buf),root,allowdefault);
        }
        entryTable = (PIMAGE_RESOURCE_DIRECTORY_ENTRY) (
                        (BYTE *) resdirptr +
                        sizeof(IMAGE_RESOURCE_DIRECTORY));
        namelen = lstrlenW(name);
        for (entrynum = 0; entrynum < resdirptr->NumberOfNamedEntries; entrynum++)
        {
                PIMAGE_RESOURCE_DIR_STRING_U str =
                (PIMAGE_RESOURCE_DIR_STRING_U) (root +
                        entryTable[entrynum].u1.s.NameOffset);
                if(namelen != str->Length)
                        continue;
                if(lstrncmpiW(name,str->NameString,str->Length)==0)
                        return (PIMAGE_RESOURCE_DIRECTORY) (
                                root +
                                entryTable[entrynum].u2.s.OffsetToDirectory);
        }
        return NULL;
    } else {
        entryTable = (PIMAGE_RESOURCE_DIRECTORY_ENTRY) (
                        (BYTE *) resdirptr +
                        sizeof(IMAGE_RESOURCE_DIRECTORY) +
                        resdirptr->NumberOfNamedEntries * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY));
        for (entrynum = 0; entrynum < resdirptr->NumberOfIdEntries; entrynum++)
            if ((DWORD)entryTable[entrynum].u1.Name == (DWORD)name)
                return (PIMAGE_RESOURCE_DIRECTORY) (
                        root +
                        entryTable[entrynum].u2.s.OffsetToDirectory);
        /* just use first entry if no default can be found */
        if (allowdefault && !name && resdirptr->NumberOfIdEntries)
                return (PIMAGE_RESOURCE_DIRECTORY) (
                        root +
                        entryTable[0].u2.s.OffsetToDirectory);
        return NULL;
    }
}

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
 *				SHELL_GetResourceTable
 */
DWORD SHELL_GetResourceTable(HFILE hFile,LPBYTE *retptr)
{	IMAGE_DOS_HEADER	mz_header;
	char			magic[4];
	int			size;

	TRACE("\n");  

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
	{ IMAGE_OS2_HEADER	ne_header;
	  LPBYTE		pTypeInfo = (LPBYTE)-1;

	  if (_lread(hFile,&ne_header,sizeof(ne_header))!=sizeof(ne_header))
	    return 0;

	  if (ne_header.ne_magic != IMAGE_OS2_SIGNATURE)
	    return 0;

	  size = ne_header.ne_restab - ne_header.ne_rsrctab;

	  if( size > sizeof(NE_TYPEINFO) )
	  { pTypeInfo = (BYTE*)HeapAlloc( GetProcessHeap(), 0, size);
	    if( pTypeInfo ) 
	    { _llseek(hFile, mz_header.e_lfanew+ne_header.ne_rsrctab, SEEK_SET);
	      if( _lread( hFile, (char*)pTypeInfo, size) != size )
	      { HeapFree( GetProcessHeap(), 0, pTypeInfo); 
		pTypeInfo = NULL;
	      }
	    }
	  }
	  *retptr = pTypeInfo;
	  return IMAGE_OS2_SIGNATURE;
	}
  	return 0; /* failed */
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
 * ShellExecuteA                            [SHELL32.245]
 */

HINSTANCE WIN32API ShellExecuteA(HWND hWnd, LPCSTR lpOperation,
                                 LPCSTR lpFile,
                                 LPCSTR lpParameters,
                                 LPCSTR lpDirectory,
                                 INT    iShowCmd )
{ 
  HINSTANCE retval=31;
  char old_dir[1024];
  char cmd[256];
  LONG cmdlen = sizeof( cmd );
  LPSTR tok;

  if (lpFile==NULL) 
    return 0; /* should not happen */
  
  if (lpOperation==NULL) /* default is open */
    lpOperation="open";

  if (lpDirectory)
  {
    // @@@PH 2002-02-26 might throw whole process off track
    // in case of concurrency
    GetCurrentDirectoryA( sizeof(old_dir), old_dir );
    SetCurrentDirectoryA( lpDirectory );
  }

  cmd[0] = '\0';
  retval = SHELL_FindExecutable( lpFile, lpOperation, cmd );

  if (retval > 32)  /* Found */
  {
    if (lpParameters)
    {
      strcat(cmd," ");
      strcat(cmd,lpParameters);
    }

    dprintf(("starting %s\n",cmd));
    retval = WinExec( cmd, iShowCmd );
  }
  else
  {
    // - path might be an URL
    // - argument might be associated with some class / app

    // 2002-02-26 PH
    // File Extension Association is missing. We'd have to lookup
    // i. e. ".doc" and create the command.
    // @@@PH

    // build lookup key
    char lpstrShellSubkey[256] = "\\shell\\";
    /* Looking for ...protocol\shell\lpOperation\command */
    strcat( lpstrShellSubkey, lpOperation );
    strcat( lpstrShellSubkey, "\\command" );

    // First, check for URL association
    if(PathIsURLA((LPSTR)lpFile))    /* File not found, check for URL */
    {
      LPSTR lpstrRes;
      INT iSize;

      lpstrRes = strchr(lpFile,':');
      if (NULL != lpstrRes)
      {
        char szProtocol[256];
        
        iSize = lpstrRes - lpFile;
  
        strncpy(szProtocol,
                lpFile, 
                min( sizeof( szProtocol ), iSize) );
  
        /* Remove File Protocol from lpFile */
        /* In the case file://path/file     */
        if(!strnicmp(lpFile,"file",iSize))
        {
          lpFile += iSize;
          while(*lpFile == ':') lpFile++;
        }
  
        /* Get the application for the protocol and execute it */
        if (RegQueryValueA(HKEY_CLASSES_ROOT,
                           szProtocol, 
                           cmd,
                           &cmdlen ) == ERROR_SUCCESS )
        {
          LPSTR tmp;
          char param[256] = "";
          LONG paramlen = 256;
  
          /* Get the parameters needed by the application
             from the associated ddeexec key */
          tmp = strstr(szProtocol,"command");
          tmp[0] = '\0';
          strcat(szProtocol,"ddeexec");
  
          if(RegQueryValueA(HKEY_CLASSES_ROOT,
                            szProtocol,
                            param,
                            &paramlen ) == ERROR_SUCCESS)
          {
            strcat(cmd," ");
            strcat(cmd,param);
            cmdlen += paramlen;
          }
        }
      }
    }


    // Second, check for filename extension association
    if (0 == cmd[0])
    {
      LPSTR lpstrRDot = strrchr(lpFile, '.');
      if (NULL != lpstrRDot)
      {
        char szAssociation[256];
        LONG lAssociationLen = sizeof( szAssociation );

        // lookup associated application (or COM object) for
        // this extension
        if (RegQueryValueA(HKEY_CLASSES_ROOT,
                           lpstrRDot,
                           szAssociation,
                           &lAssociationLen) == ERROR_SUCCESS)
        {
          // append the shell subkey
          strcat(szAssociation, lpstrShellSubkey);
          lAssociationLen = sizeof( szAssociation );

          // lookup application for retrieved association
          if (RegQueryValueA(HKEY_CLASSES_ROOT,
                             szAssociation,
                             cmd,
                             &cmdlen) == ERROR_SUCCESS)
          {
            // received cmd now
          }
        }
      }
    }
      
      
#ifdef __WIN32OS2__
    if (0 == cmd[0])
    {
      // OS/2 specific addon:
      // if no windows association is found, pass on to the workplace shell
      // eventually.
      retval = ShellExecuteOS2(hWnd,
                               lpOperation,
                               lpFile,
                               lpParameters,
                               lpDirectory,
                               iShowCmd);
    }
#endif

    if (0 != cmd[0])
    {
      // resolve the parameters
      // @@@PH 2002-02-26 there might be more than one parameter only
      /* Is there a replace() function anywhere? */
      tok=strstr( cmd, "%1" );
      if (tok != NULL)
      {
        tok[0]='\0'; /* truncate string at the percent */
        strcat( cmd, lpFile ); /* what if no dir in xlpFile? */
        tok=strstr( cmd, "%1" );
        if ((tok!=NULL) && (strlen(tok)>2))
        {
          strcat( cmd, &tok[2] );
        }
      }
    }

    /* Nothing was done yet, try to execute the cmdline directly,
     maybe it's an OS/2 program */
    if (0 == cmd[0])
    {
      strcpy(cmd,lpFile);
      strcat(cmd,lpParameters ? lpParameters : "");
    }
      
    // now launch ... something :)
    retval = WinExec( cmd, iShowCmd );
  }

  if (lpDirectory)
    SetCurrentDirectoryA( old_dir );
  
  return retval;
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

UINT WIN32API DragQueryFileAorW(HDROP hDrop, UINT iFile, LPTSTR lpszFile,
                                UINT cch)
{
  // @@@PH maybe they want automatic determination here
  if (SHELL_OsIsUnicode())
    return DragQueryFileW(hDrop, iFile, (LPWSTR)lpszFile, cch);
  else
    return DragQueryFileA(hDrop, iFile, lpszFile, cch);
}


LPCSTR debugstr_guid1( void *id1 )
{
 static char str[40];
 GUID *id = (GUID *)id1;

    if (!id) return "(null)";
    if (!HIWORD(id))
    {
        sprintf( str, "<guid-0x%04x>", LOWORD(id) );
    }
    else
    {
        sprintf( str, "{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                 id->Data1, id->Data2, id->Data3,
                 id->Data4[0], id->Data4[1], id->Data4[2], id->Data4[3],
                 id->Data4[4], id->Data4[5], id->Data4[6], id->Data4[7] );
    }
    return str;
}

