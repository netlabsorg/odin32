/* $Id: pidl.cpp,v 1.9 2000-08-18 02:01:17 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 *
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * pidl Handling
 *
 * Copyright 1998 Juergen Schmied
 *
 * NOTES
 *  a pidl == NULL means desktop and is legal
 *
 * Corel WINE 20000324 level
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "debugtools.h"
#include "shell.h"
#include "shlguid.h"
#include "winerror.h"
#include "winnls.h"
#include "winversion.h"
#include "shell32_main.h"

#include "pidl.h"
#include "wine/undocshell.h"

#include <heapstring.h>
#include <misc.h>


ODINDEBUGCHANNEL(SHELL32-PIDL)


/*****************************************************************************
 * Local prototypes (performance optimization)                               *
 *****************************************************************************/

LPITEMIDLIST _Optlink ODIN_ILGetNext         (LPITEMIDLIST pidl);
BOOL         _Optlink ODIN__ILIsDesktop      (LPCITEMIDLIST pidl);
BOOL         _Optlink ODIN__ILIsValue        (LPCITEMIDLIST pidl);
LPPIDLDATA   _Optlink ODIN__ILGetDataPointer (LPITEMIDLIST pidl);
LPITEMIDLIST _Optlink ODIN_ILFindLastID      (LPITEMIDLIST pidl);
LPSTR        _Optlink ODIN__ILGetTextPointer (PIDLTYPE type, LPPIDLDATA pidldata);
DWORD        _Optlink ODIN__ILSimpleGetText  (LPCITEMIDLIST pidl,LPSTR szOut, UINT uOutSize);
DWORD        _Optlink ODIN_ILGetSize         (LPITEMIDLIST pidl);
LPITEMIDLIST _Optlink ODIN_ILClone           (LPCITEMIDLIST pidl);
LPITEMIDLIST _Optlink ODIN_ILCloneFirst      (LPCITEMIDLIST pidl);




#ifdef DEBUG
void pdump (LPCITEMIDLIST pidl)
{  DWORD type;
   char * szData;
   char * szShortName;
   char szName[MAX_PATH];
   BOOL bIsShellDebug;

   LPITEMIDLIST pidltemp = pidl;
   if (!TRACE_ON(pidl))
     return;

   /* silence the sub-functions */
   bIsShellDebug = TRACE_ON(shell);
   __SET_DEBUGGING(__DBCL_TRACE, dbch_shell, FALSE);
   __SET_DEBUGGING(__DBCL_TRACE, dbch_pidl, FALSE);

   if (! pidltemp)
   {
     MESSAGE ("-------- pidl=NULL (Desktop)\n");
   }
   else
   {
     MESSAGE ("-------- pidl=%p\n", pidl);
     if (pidltemp->mkid.cb)
     {
       do
       {
         type   = ODIN__ILGetDataPointer(pidltemp)->type;
         szData = ODIN__ILGetTextPointer(type, ODIN__ILGetDataPointer(pidltemp));
         szShortName = _ILGetSTextPointer(type, ODIN__ILGetDataPointer(pidltemp));
         ODIN__ILSimpleGetText(pidltemp, szName, MAX_PATH);

         MESSAGE ("-- pidl=%p size=%u type=%lx name=%s (%s,%s)\n",
                  pidltemp, pidltemp->mkid.cb,type,szName,debugstr_a(szData), debugstr_a(szShortName));

         pidltemp = ODIN_ILGetNext(pidltemp);

       } while (pidltemp->mkid.cb);
     }
     else
     {
       MESSAGE ("empty pidl (Desktop)\n");
     }
     pcheck(pidl);
   }

   __SET_DEBUGGING(__DBCL_TRACE, dbch_shell, bIsShellDebug);
   __SET_DEBUGGING(__DBCL_TRACE, dbch_pidl, TRUE);

}
#endif

#define BYTES_PRINTED 32
BOOL pcheck (LPCITEMIDLIST pidl)
{       DWORD type, ret=TRUE;
   BOOL bIsPidlDebug;

        LPITEMIDLIST pidltemp = pidl;

   bIsPidlDebug = TRACE_ON(shell);
   __SET_DEBUGGING(__DBCL_TRACE, dbch_pidl, FALSE);

        if (pidltemp && pidltemp->mkid.cb)
        { do
          { type   = ODIN__ILGetDataPointer(pidltemp)->type;
            switch (type)
       { case PT_DESKTOP:
         case PT_MYCOMP:
         case PT_SPECIAL:
         case PT_DRIVE:
         case PT_DRIVE1:
         case PT_DRIVE2:
         case PT_DRIVE3:
         case PT_FOLDER:
         case PT_VALUE:
         case PT_FOLDER1:
         case PT_WORKGRP:
         case PT_COMP:
         case PT_NETWORK:
         case PT_SHARE:
         case PT_IESPECIAL:
      break;
         default:
         {
      char szTemp[BYTES_PRINTED*4 + 1];
      int i;
      unsigned char c;

      memset(szTemp, ' ', BYTES_PRINTED*4 + 1);
      for ( i = 0; (i<pidltemp->mkid.cb) && (i<BYTES_PRINTED); i++)
      {
        c = ((unsigned char *)pidltemp)[i];

        szTemp[i*3+0] = ((c>>4)>9)? (c>>4)+55 : (c>>4)+48;
        szTemp[i*3+1] = ((0x0F&c)>9)? (0x0F&c)+55 : (0x0F&c)+48;
        szTemp[i*3+2] = ' ';
        szTemp[i+BYTES_PRINTED*3]  =  (c>=0x20 && c <=0x80) ? c : '.';
      }
      szTemp[BYTES_PRINTED*4] = 0x00;
      ERR_(pidl)("unknown IDLIST type size=%u type=%lx\n%s\n",pidltemp->mkid.cb,type, szTemp);
      ret = FALSE;
         }
       }
       pidltemp = ODIN_ILGetNext(pidltemp);
     } while (pidltemp->mkid.cb);
   }
   __SET_DEBUGGING(__DBCL_TRACE, dbch_pidl, bIsPidlDebug);
   return ret;
}

/*************************************************************************
 * ILGetDisplayName                            [SHELL32.15]
 */

ODINFUNCTION2(BOOL,ILGetDisplayName,LPCITEMIDLIST, pidl,
                                    LPSTR,         path)
{
   TRACE_(shell)("pidl=%p %p semi-stub\n",pidl,path);
   return SHGetPathFromIDListA(pidl, path);
}


/*************************************************************************
 * ILFindLastID [SHELL32.16]
 *
 * NOTES
 *   observed: pidl=Desktop return=pidl
 */

ODINFUNCTION1(LPITEMIDLIST,ILFindLastID,LPITEMIDLIST, pidl)
{  LPITEMIDLIST   pidlLast = pidl;

   while (pidl->mkid.cb)
   {
     pidlLast = pidl;
     pidl = ODIN_ILGetNext(pidl);
   }
   return pidlLast;
}
/*************************************************************************
 * ILRemoveLastID [SHELL32.17]
 *
 * NOTES
 *   when pidl=Desktop return=FALSE
 */

ODINFUNCTION1(BOOL,ILRemoveLastID,LPCITEMIDLIST, pidl)
{
   if (!pidl || !pidl->mkid.cb)
     return 0;
   ODIN_ILFindLastID(pidl)->mkid.cb = 0;
   return 1;
}

/*************************************************************************
 * ILClone [SHELL32.18]
 *
 * NOTES
 *    dupicate an idlist
 */

ODINFUNCTION1(LPITEMIDLIST,ILClone,LPCITEMIDLIST,pidl)
{ DWORD    len;
  LPITEMIDLIST  newpidl;

  if (!pidl)
    return NULL;

  len = ODIN_ILGetSize(pidl);
  newpidl = (LPITEMIDLIST)SHAlloc(len);
  if (newpidl)
    memcpy(newpidl,pidl,len);

  TRACE_(pidl)("pidl=%p newpidl=%p\n",pidl, newpidl);
  pdump(pidl);

  return newpidl;
}


/*************************************************************************
 * ILCloneFirst [SHELL32.19]
 *
 * NOTES
 *  duplicates the first idlist of a complex pidl
 */

ODINFUNCTION1(LPITEMIDLIST,ILCloneFirst,LPCITEMIDLIST,pidl)
{  DWORD len;
   LPITEMIDLIST pidlNew = NULL;

   pdump(pidl);

   if (pidl)
   {
     len = pidl->mkid.cb;
     pidlNew = (LPITEMIDLIST) SHAlloc (len+2);
     if (pidlNew)
     {
       memcpy(pidlNew,pidl,len+2);          /* 2 -> mind a desktop pidl */

       if (len)
         ODIN_ILGetNext(pidlNew)->mkid.cb = 0x00;
     }
   }
   TRACE_(pidl)("-- newpidl=%p\n",pidlNew);

   return pidlNew;
}
/*************************************************************************
 * ILLoadFromStream
 *
 * NOTES
 *   the first two bytes are the len, the pidl is following then
 */

ODINFUNCTION2(HRESULT,ILLoadFromStream,IStream*,      pStream,
                                       LPITEMIDLIST*, ppPidl)
{  WORD     wLen = 0;
   DWORD    dwBytesRead;
   HRESULT     ret = E_FAIL;

   if (*ppPidl)
   { SHFree(*ppPidl);
     *ppPidl = NULL;
   }

   IStream_AddRef (pStream);

   if (SUCCEEDED(IStream_Read(pStream, (LPVOID)&wLen, 2, &dwBytesRead)))
   { *ppPidl = (ITEMIDLIST*)SHAlloc (wLen);
     if (SUCCEEDED(IStream_Read(pStream, *ppPidl , wLen, &dwBytesRead)))
     { ret = S_OK;
     }
     else
     { SHFree(*ppPidl);
       *ppPidl = NULL;
     }
   }

   /* we are not jet fully compatible */
   if (!pcheck(*ppPidl))
   { SHFree(*ppPidl);
     *ppPidl = NULL;
   }


   IStream_Release (pStream);

   return ret;
}
/*************************************************************************
 * SHILCreateFromPath                    [SHELL32.28]
 *
 * NOTES
 *   wraper for IShellFolder::ParseDisplayName()
 */

ODINFUNCTION3(HRESULT,SHILCreateFromPathA,LPCSTR,        path,
                                          LPITEMIDLIST*, ppidl,
                                          DWORD*,        attributes)
{  LPSHELLFOLDER sf;
   WCHAR lpszDisplayName[MAX_PATH];
   DWORD pchEaten;
   HRESULT ret = E_FAIL;

   LocalToWideChar(lpszDisplayName, (LPSTR)path, MAX_PATH);

   if (SUCCEEDED (SHGetDesktopFolder(&sf)))
   {
     ret = IShellFolder_ParseDisplayName(sf,0, NULL,lpszDisplayName,&pchEaten,ppidl,attributes);
     IShellFolder_Release(sf);
   }
   return ret;
}

ODINFUNCTION3(HRESULT,SHILCreateFromPathW,LPCWSTR,       path,
                                          LPITEMIDLIST*, ppidl,
                                          DWORD*,        attributes)
{  LPSHELLFOLDER sf;
   DWORD pchEaten;
   HRESULT ret = E_FAIL;

   if (SUCCEEDED (SHGetDesktopFolder(&sf)))
   {
     ret = IShellFolder_ParseDisplayName(sf,0, NULL, (LPOLESTR)path, &pchEaten, ppidl, attributes);
     IShellFolder_Release(sf);
   }
   return ret;
}

ODINFUNCTION3(HRESULT,SHILCreateFromPathAW,LPCVOID,       path,
                                           LPITEMIDLIST*, ppidl,
                                           DWORD*,        attributes)
{
   if ( VERSION_OsIsUnicode())
     return SHILCreateFromPathW ((LPCWSTR)path, ppidl, attributes);
   return SHILCreateFromPathA ((LPCSTR)path, ppidl, attributes);
}

/*************************************************************************
 * SHCloneSpecialIDList [SHELL32.89]
 *
 * PARAMETERS
 *  hwndOwner  [in]
 *  nFolder    [in]                      CSIDL_xxxxx ??
 *
 * RETURNS
 *  pidl ??
 * NOTES
 *     exported by ordinal
 */

ODINFUNCTION3(LPITEMIDLIST,SHCloneSpecialIDList,HWND,  hwndOwner,
                                                DWORD, nFolder,
                                                DWORD, x3)
{  LPITEMIDLIST ppidl;
   WARN_(shell)("(hwnd=0x%x,csidl=0x%lx,0x%lx):semi-stub.\n",
                hwndOwner,nFolder,x3);

   SHGetSpecialFolderLocation(hwndOwner, nFolder, &ppidl);

   return ppidl;
}

/*************************************************************************
 * ILGlobalClone [SHELL32.97]
 *
 */

ODINFUNCTION1(LPITEMIDLIST,ILGlobalClone,LPCITEMIDLIST, pidl)
{  DWORD    len;
   LPITEMIDLIST  newpidl;

   if (!pidl)
     return NULL;

   len = ODIN_ILGetSize(pidl);
   newpidl = (LPITEMIDLIST)pCOMCTL32_Alloc(len);
   if (newpidl)
     memcpy(newpidl,pidl,len);

   TRACE_(pidl)("pidl=%p newpidl=%p\n",pidl, newpidl);
   pdump(pidl);

   return newpidl;
}

/*************************************************************************
 * ILIsEqual [SHELL32.21]
 *
 */

ODINFUNCTION2(BOOL,ILIsEqual,LPCITEMIDLIST, pidl1,
                             LPCITEMIDLIST, pidl2)
{
   char  szData1[MAX_PATH];
   char  szData2[MAX_PATH];

   LPITEMIDLIST pidltemp1 = pidl1;
   LPITEMIDLIST pidltemp2 = pidl2;

   /* explorer reads from registry directly (StreamMRU),
      so we can only check here */
   if ((!pcheck (pidl1)) || (!pcheck (pidl2))) return FALSE;

   pdump (pidl1);
   pdump (pidl2);

   if ( (!pidl1) || (!pidl2) ) return FALSE;

   while (pidltemp1->mkid.cb && pidltemp2->mkid.cb)
   {
       ODIN__ILSimpleGetText(pidltemp1, szData1, MAX_PATH);
       ODIN__ILSimpleGetText(pidltemp2, szData2, MAX_PATH);

       if (strcmp ( szData1, szData2 )!=0 )
         return FALSE;

       pidltemp1 = ODIN_ILGetNext(pidltemp1);
       pidltemp2 = ODIN_ILGetNext(pidltemp2);
   }

   if (!pidltemp1->mkid.cb && !pidltemp2->mkid.cb)
   {
     return TRUE;
   }

   return FALSE;
}
/*************************************************************************
 * ILIsParent [SHELL32.23]
 *
 * parent=a/b  child=a/b/c -> true, c is in folder a/b
 *    child=a/b/c/d -> false if bImmediate is true, d is not in folder a/b
 *    child=a/b/c/d -> true if bImmediate is false, d is in a subfolder of a/b
 */

ODINFUNCTION3(BOOL,ILIsParent,LPCITEMIDLIST, pidlParent,
                              LPCITEMIDLIST, pidlChild,
                              BOOL,          bImmediate)
{
   char  szData1[MAX_PATH];
   char  szData2[MAX_PATH];

   LPITEMIDLIST pParent = pidlParent;
   LPITEMIDLIST pChild = pidlChild;

   while (pParent->mkid.cb && pChild->mkid.cb)
   {
     ODIN__ILSimpleGetText(pParent, szData1, MAX_PATH);
     ODIN__ILSimpleGetText(pChild, szData2, MAX_PATH);

     if (strcmp ( szData1, szData2 )!=0 )
       return FALSE;

     pParent = ODIN_ILGetNext(pParent);
     pChild = ODIN_ILGetNext(pChild);
   }

   if ( pParent->mkid.cb || ! pChild->mkid.cb) /* child shorter or has equal length to parent */
     return FALSE;

   if ( ODIN_ILGetNext(pChild)->mkid.cb && bImmediate) /* not immediate descent */
     return FALSE;

   return TRUE;
}

/*************************************************************************
 * ILFindChild [SHELL32.24]
 *
 * NOTES
 *  Compares elements from pidl1 and pidl2.
 *
 *  pidl1 is desktop                        pidl2
 *  pidl1 shorter pidl2                     pointer to first different element of pidl2
 *          if there was at least one equal element
 *  pidl2 shorter pidl1                     0
 *  pidl2 equal pidl1                       pointer to last 0x00-element of pidl2
 */

ODINFUNCTION2(LPITEMIDLIST,ILFindChild,LPCITEMIDLIST, pidl1,
                                       LPCITEMIDLIST, pidl2)
{
   char  szData1[MAX_PATH];
   char  szData2[MAX_PATH];

   LPITEMIDLIST pidltemp1 = pidl1;
   LPITEMIDLIST pidltemp2 = pidl2;
   LPITEMIDLIST ret=NULL;

   /* explorer reads from registry directly (StreamMRU),
      so we can only check here */
   if ((!pcheck (pidl1)) || (!pcheck (pidl2)))
     return FALSE;

   pdump (pidl1);
   pdump (pidl2);

   if ( ODIN__ILIsDesktop(pidl1) )
   {
     ret = pidl2;
   }
   else
   {
     while (pidltemp1->mkid.cb && pidltemp2->mkid.cb)
     {
       ODIN__ILSimpleGetText(pidltemp1, szData1, MAX_PATH);
       ODIN__ILSimpleGetText(pidltemp2, szData2, MAX_PATH);

       if (strcmp(szData1,szData2))
         break;

       pidltemp1 = ODIN_ILGetNext(pidltemp1);
       pidltemp2 = ODIN_ILGetNext(pidltemp2);
       ret = pidltemp2;
     }

     if (pidltemp1->mkid.cb)
     {
       ret = NULL; /* elements of pidl1 left*/
     }
   }
   TRACE_(shell)("--- %p\n", ret);
   return ret; /* pidl 1 is shorter */
}

/*************************************************************************
 * ILCombine [SHELL32.25]
 *
 * NOTES
 *  Concatenates two complex idlists.
 *  The pidl is the first one, pidlsub the next one
 *  Does not destroy the passed in idlists!
 */

ODINFUNCTION2(LPITEMIDLIST, ILCombine, LPCITEMIDLIST, pidl1,
                                       LPCITEMIDLIST, pidl2)
{
   DWORD    len1,len2;
   LPITEMIDLIST  pidlNew;

   if(!pidl1 && !pidl2) return NULL;

   pdump (pidl1);
   pdump (pidl2);

   if(!pidl1)
   {
     pidlNew = ODIN_ILClone(pidl2);
     return pidlNew;
   }

   if(!pidl2)
   {
     pidlNew = ODIN_ILClone(pidl1);
     return pidlNew;
   }

   len1  = ODIN_ILGetSize(pidl1)-2;
   len2  = ODIN_ILGetSize(pidl2);
   pidlNew  = (ITEMIDLIST*)SHAlloc(len1+len2);

   if (pidlNew)
   {
     memcpy(pidlNew,pidl1,len1);
     memcpy(((BYTE *)pidlNew)+len1,pidl2,len2);
   }

   /*  TRACE(pidl,"--new pidl=%p\n",pidlNew);*/
   return pidlNew;
}
/*************************************************************************
 *  SHGetRealIDL [SHELL32.98]
 *
 * NOTES
 */

ODINFUNCTION3(LPITEMIDLIST, SHGetRealIDL, LPSHELLFOLDER, lpsf,
                                          LPITEMIDLIST,  pidl,
                                          DWORD,         z)
{
   FIXME_(pidl)("sf=%p pidl=%p 0x%04lx\n",lpsf,pidl,z);

   pdump (pidl);
   return 0;
}

/*************************************************************************
 *  SHLogILFromFSIL [SHELL32.95]
 *
 * NOTES
 *  pild = CSIDL_DESKTOP                 ret = 0
 *  pild = CSIDL_DRIVES                     ret = 0
 */

ODINFUNCTION1(LPITEMIDLIST,SHLogILFromFSIL,LPITEMIDLIST,pidl)
{
   FIXME_(pidl)("(pidl=%p)\n",pidl);

   pdump(pidl);

   return 0;
}

/*************************************************************************
 * ODIN_ILGetSize [SHELL32.152]
 *  gets the byte size of an idlist including zero terminator (pidl)
 *
 * PARAMETERS
 *  pidl ITEMIDLIST
 *
 * RETURNS
 *  size of pidl
 *
 * NOTES
 *  exported by ordinal
 */

ODINFUNCTION1(DWORD,ILGetSize,LPITEMIDLIST,pidl)
{
   LPSHITEMID si = &(pidl->mkid);
   DWORD  len=0;

   if (pidl)
   { while (si->cb)
     { len += si->cb;
       si  = (LPSHITEMID)(((LPBYTE)si)+si->cb);
     }
     len += 2;
   }
   TRACE_(pidl)("pidl=%p size=%lu\n",pidl, len);
   return len;
}

/*************************************************************************
 * ODIN_ILGetNext [SHELL32.153]
 *  gets the next simple pidl of a complex pidl
 *
 * observed return values:
 *  null -> null
 *  desktop -> null
 *  simple pidl -> pointer to 0x0000 element
 *
 */

ODINFUNCTION1(LPITEMIDLIST,ILGetNext,LPITEMIDLIST,pidl)
{
   WORD len;

   if(pidl)
   {
     len =  pidl->mkid.cb;
     if (len)
     {
       pidl = (LPITEMIDLIST) (((LPBYTE)pidl)+len);
       return pidl;
     }
   }
   return NULL;
}
/*************************************************************************
 * ILAppend [SHELL32.154]
 *
 * NOTES
 *  Adds the single item to the idlist indicated by pidl.
 *  if bEnd is 0, adds the item to the front of the list,
 *  otherwise adds the item to the end. (???)
 *  Destroys the passed in idlist! (???)
 */

ODINFUNCTION3(LPITEMIDLIST,ILAppend,LPITEMIDLIST,  pidl,
                                    LPCITEMIDLIST, item,
                                    BOOL,          bEnd)
{
   LPITEMIDLIST idlRet;

   WARN_(pidl)("(pidl=%p,pidl=%p,%08u)semi-stub\n",pidl,item,bEnd);

   pdump (pidl);
   pdump (item);

   if (ODIN__ILIsDesktop(pidl))
   {
      idlRet = ODIN_ILClone(item);
      if (pidl)
        SHFree (pidl);
      return idlRet;
   }

   if (bEnd)
   {
     idlRet=ILCombine(pidl,item);
   }
   else
   {
     idlRet=ILCombine(item,pidl);
   }

   SHFree(pidl);
   return idlRet;
}
/*************************************************************************
 * ILFree [SHELL32.155]
 *
 * NOTES
 *     free_check_ptr - frees memory (if not NULL)
 *     allocated by SHMalloc allocator
 *     exported by ordinal
 */

ODINFUNCTION1(DWORD,ILFree,LPITEMIDLIST, pidl)
{  TRACE_(pidl)("(pidl=0x%08lx)\n",(DWORD)pidl);

   if (!pidl)
     return FALSE;

  SHFree(pidl);
  return TRUE;
}
/*************************************************************************
 * ILGlobalFree [SHELL32.156]
 *
 */

ODINPROCEDURE1(ILGlobalFree,
               LPITEMIDLIST, pidl)
{
   TRACE_(pidl)("%p\n",pidl);

   if (pidl)
     pCOMCTL32_Free (pidl);
}
/*************************************************************************
 * ILCreateFromPath [SHELL32.157]
 *
 */

ODINFUNCTION1(LPITEMIDLIST,ILCreateFromPathA,LPCSTR,path)
{
   LPITEMIDLIST pidlnew;
   DWORD attributes = 0;

   TRACE_(shell)("%s\n",path);

   if (SUCCEEDED (SHILCreateFromPathA (path, &pidlnew, &attributes)))
     return pidlnew;
   return FALSE;
}

ODINFUNCTION1(LPITEMIDLIST,ILCreateFromPathW,LPCWSTR,path)
{
   LPITEMIDLIST pidlnew;
   DWORD attributes = 0;

   TRACE_(shell)("%s\n",debugstr_w(path));

   if (SUCCEEDED (SHILCreateFromPathW (path, &pidlnew, &attributes)))
     return pidlnew;
   return FALSE;
}

ODINFUNCTION1(LPITEMIDLIST,ILCreateFromPathAW,LPCVOID,path)
{
   if ( VERSION_OsIsUnicode())
     return ILCreateFromPathW ((LPCWSTR)path);
   return ILCreateFromPathA ((LPCSTR)path);
}
/*************************************************************************
 *  SHSimpleIDListFromPath [SHELL32.162]
 */

ODINFUNCTION1(LPITEMIDLIST,SHSimpleIDListFromPathA,LPCSTR, lpszPath)
{
   LPITEMIDLIST   pidl=NULL;
   HANDLE   hFile;
   WIN32_FIND_DATAA                      stffile;

   if (!lpszPath) return NULL;

   hFile = FindFirstFileA(lpszPath, &stffile);

   if ( hFile != INVALID_HANDLE_VALUE )
   {
     if (stffile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
     {
       pidl = _ILCreateFolder (&stffile);
     }
     else
     {
       pidl = _ILCreateValue (&stffile);
     }
     FindClose (hFile);
   }
   return pidl;
}

ODINFUNCTION1(LPITEMIDLIST,SHSimpleIDListFromPathW,LPCWSTR, lpszPath)
{
   char  lpszTemp[MAX_PATH];

   WideCharToLocal(lpszTemp, (LPWSTR)lpszPath, MAX_PATH);

   return SHSimpleIDListFromPathA (lpszTemp);
}

ODINFUNCTION1(LPITEMIDLIST,SHSimpleIDListFromPathAW,LPCVOID, lpszPath)
{
   if ( VERSION_OsIsUnicode())
     return SHSimpleIDListFromPathW ((LPWSTR)lpszPath);
   return SHSimpleIDListFromPathA ((LPSTR)lpszPath);
}

/*************************************************************************
 * SHGetSpecialFolderLocation               [SHELL32.223]
 *
 * gets the folder locations from the registry and creates a pidl
 * creates missing reg keys and directorys
 *
 * PARAMS
 *   hwndOwner [I]
 *   nFolder   [I] CSIDL_xxxxx
 *   ppidl     [O] PIDL of a special folder
 *
 */
ODINFUNCTION3(HRESULT,SHGetSpecialFolderLocation,HWND,          hwndOwner,
                                                 INT,           nFolder,
                                                 LPITEMIDLIST*, ppidl)
{
   CHAR     szPath[256];
   HRESULT     hr = E_INVALIDARG;

   *ppidl = NULL;

   if (ppidl)
   {
     switch (nFolder)
     {
       case CSIDL_DESKTOP:
         *ppidl = _ILCreateDesktop();
         hr = NOERROR;
         break;

       case CSIDL_DRIVES:
         *ppidl = _ILCreateMyComputer();
         hr = NOERROR;
         break;

       case CSIDL_NETWORK:
         *ppidl = _ILCreateNetwork ();
         hr = NOERROR;
         break;

       case CSIDL_CONTROLS:
         *ppidl = _ILCreateControl ();
         hr = NOERROR;
         break;

       case CSIDL_PRINTERS:
         *ppidl = _ILCreatePrinter ();
         hr = NOERROR;
         break;

       case CSIDL_BITBUCKET:
         *ppidl = _ILCreateBitBucket ();
         hr = NOERROR;
         break;

       default:
         if (SHGetSpecialFolderPathA(hwndOwner, szPath, nFolder, TRUE))
         {
      DWORD attributes=0;
      TRACE_(shell)("Value=%s\n",szPath);
      hr = SHILCreateFromPathA(szPath, ppidl, &attributes);
         }
     }
   }

   TRACE_(shell)("-- (new pidl %p)\n",*ppidl);
   return hr;
}

/*************************************************************************
 * SHGetDataFromIDListA [SHELL32.247]
 *
 * NOTES
 *  the pidl can be a simple one. since we cant get the path out of the pidl
 *  we have to take all data from the pidl
 */

ODINFUNCTION5(HRESULT,SHGetDataFromIDListA,LPSHELLFOLDER, psf,
                                           LPCITEMIDLIST, pidl,
                                           int,           nFormat,
                                           LPVOID,        dest,
                                           int,           len)
{
   DWORD type;

   TRACE_(shell)("sf=%p pidl=%p 0x%04x %p 0x%04x stub\n",psf,pidl,nFormat,dest,len);

   if (! psf || !dest )  return E_INVALIDARG;

   switch (nFormat)
   {
     case SHGDFIL_FINDDATA:
       {
               WIN32_FIND_DATAA * pfd = (WIN32_FIND_DATAA*)dest;

               if ( len < sizeof (WIN32_FIND_DATAA)) return E_INVALIDARG;

               ZeroMemory(pfd, sizeof (WIN32_FIND_DATAA));
               _ILGetFileDateTime( pidl, &(pfd->ftLastWriteTime));
               pfd->dwFileAttributes = _ILGetFileAttributes(pidl, NULL, 0);
               pfd->nFileSizeLow = _ILGetFileSize ( pidl, NULL, 0);
               type   = ODIN__ILGetDataPointer(pidl)->type;
               lstrcpynA(pfd->cFileName,ODIN__ILGetTextPointer(type,ODIN__ILGetDataPointer(pidl)), MAX_PATH);
               lstrcpynA(pfd->cAlternateFileName,_ILGetSTextPointer(type,ODIN__ILGetDataPointer(pidl)), 14);
       }
       return NOERROR;

     case SHGDFIL_NETRESOURCE:
     case SHGDFIL_DESCRIPTIONID:
       FIXME_(shell)("SHGDFIL %i stub\n", nFormat);
       break;

     default:
       ERR_(shell)("Unknown SHGDFIL %i, please report\n", nFormat);
   }

   return E_INVALIDARG;
}
/*************************************************************************
 * SHGetDataFromIDListW [SHELL32.247]
 *
 */

ODINFUNCTION5(HRESULT,SHGetDataFromIDListW,LPSHELLFOLDER, psf,
                                           LPCITEMIDLIST, pidl,
                                           int,           nFormat,
                                           LPVOID,        dest,
                                           int,           len)
{
        DWORD type;

        if (! psf || !dest )  return E_INVALIDARG;

        switch (nFormat)
        {
          case SHGDFIL_FINDDATA:
            {
               WIN32_FIND_DATAW * pfd = (WIN32_FIND_DATAW*)dest;

               if ( len < sizeof (WIN32_FIND_DATAW)) return E_INVALIDARG;

               ZeroMemory(pfd, sizeof (WIN32_FIND_DATAA));
               _ILGetFileDateTime( pidl, &(pfd->ftLastWriteTime));
               pfd->dwFileAttributes = _ILGetFileAttributes(pidl, NULL, 0);
               pfd->nFileSizeLow = _ILGetFileSize ( pidl, NULL, 0);
               type   = ODIN__ILGetDataPointer(pidl)->type;
               lstrcpynAtoW(pfd->cFileName,ODIN__ILGetTextPointer(type,ODIN__ILGetDataPointer(pidl)), MAX_PATH);
               lstrcpynAtoW(pfd->cAlternateFileName,_ILGetSTextPointer(type,ODIN__ILGetDataPointer(pidl)), 14);
            }
            return NOERROR;
          case SHGDFIL_NETRESOURCE:
          case SHGDFIL_DESCRIPTIONID:
            FIXME_(shell)("SHGDFIL %i stub\n", nFormat);
            break;

          default:
            ERR_(shell)("Unknown SHGDFIL %i, please report\n", nFormat);
        }

        return E_INVALIDARG;
}

/*************************************************************************
 * SHGetPathFromIDListA                     [SHELL32.261][NT 4.0: SHELL32.220]
 *
 * PARAMETERS
 *  pidl,   [IN] pidl
 *  pszPath [OUT] path
 *
 * RETURNS
 *  path from a passed PIDL.
 *
 * NOTES
 * NULL returns FALSE
 * desktop pidl gives path to desktopdirectory back
 * special pidls returning FALSE
 *
 * FIXME
 *  fnGetDisplayNameOf can return different types of OLEString
 */

ODINFUNCTION2(BOOL,SHGetPathFromIDListA,LPCITEMIDLIST, pidl,
                                        LPSTR,         pszPath)
{  STRRET str;
   LPSHELLFOLDER shellfolder;

   if (!pidl) return FALSE;

   pdump(pidl);
#ifdef SHELL_NO_DESKTOP
   if(ODIN__ILIsDesktop(pidl))
   {
      SHGetSpecialFolderPathA(0, pszPath, CSIDL_DESKTOPDIRECTORY, FALSE);
   }
   else
#endif
   {
     if (SHGetDesktopFolder(&shellfolder)==S_OK)
     {
       IShellFolder_GetDisplayNameOf(shellfolder,pidl,SHGDN_FORPARSING,&str);
       StrRetToStrNA (pszPath, MAX_PATH, &str, pidl);
       IShellFolder_Release(shellfolder);
     }
   }
   TRACE_(shell)("-- (%s)\n",pszPath);

   return TRUE;
}
/*************************************************************************
 * SHGetPathFromIDListW                        [SHELL32.262]
 */

ODINFUNCTION2(BOOL,SHGetPathFromIDListW,LPCITEMIDLIST, pidl,
                                        LPWSTR,        pszPath)
{  char sTemp[MAX_PATH];

   SHGetPathFromIDListA (pidl, sTemp);
   lstrcpyAtoW(pszPath, sTemp);

   TRACE_(shell)("-- (%s)\n",debugstr_w(pszPath));

   return TRUE;
}

/*************************************************************************
 * SHBindToParent                        [shell version 5.0]
 */

ODINFUNCTION4(HRESULT,SHBindToParent,LPCITEMIDLIST, pidl,
                                     REFIID,        riid,
                                     LPVOID*,       ppv,
                                     LPCITEMIDLIST*,ppidlLast)
{
   IShellFolder   * psf;
   LPITEMIDLIST   pidlChild, pidlParent;
   HRESULT     hr=E_FAIL;

   pdump(pidl);

   *ppv = NULL;
   if (ppidlLast) *ppidlLast = NULL;

   if (_ILIsPidlSimple(pidl))
   {
     /* we are on desktop level */
     if (ppidlLast)
       *ppidlLast = ODIN_ILClone(pidl);
     hr = SHGetDesktopFolder((IShellFolder**)ppv);
   }
   else
   {
     pidlChild =  ODIN_ILClone(ODIN_ILFindLastID(pidl));
     pidlParent = ODIN_ILClone(pidl);
     ILRemoveLastID(pidlParent);

     hr = SHGetDesktopFolder(&psf);

     if (SUCCEEDED(hr))
       hr = IShellFolder_BindToObject(psf, pidlParent, NULL, riid, ppv);

     if (SUCCEEDED(hr) && ppidlLast)
       *ppidlLast = pidlChild;
     else
       ILFree (pidlChild);

     SHFree (pidlParent);
     if (psf) IShellFolder_Release(psf);
   }


   TRACE_(shell)("-- psf=%p pidl=%p ret=0x%08lx\n", *ppv, (ppidlLast)?*ppidlLast:NULL, hr);
   return hr;
}

/*************************************************************************
 * SHGetPathFromIDListAW                    [SHELL32.221][NT 4.0: SHELL32.219]
 */

ODINFUNCTION2(BOOL,SHGetPathFromIDListAW,LPCITEMIDLIST, pidl,
                                         LPVOID,        pszPath)
{
   if (VERSION_OsIsUnicode())
     return SHGetPathFromIDListW(pidl,(LPWSTR)pszPath);
   return SHGetPathFromIDListA(pidl,(LPSTR)pszPath);
}

/**************************************************************************
 *
 *    internal functions
 *
 * ### 1. section creating pidls ###
 *
 *************************************************************************
 *  _ILCreateDesktop()
 *  _ILCreateIExplore()
 *  _ILCreateMyComputer()
 *  _ILCreateDrive()
 *  _ILCreateFolder()
 *  _ILCreateValue()
 */

ODINFUNCTION0(LPITEMIDLIST,_ILCreateDesktop)
{
  return _ILCreate(PT_DESKTOP, NULL, 0);
}

ODINFUNCTION0(LPITEMIDLIST,_ILCreateMyComputer)
{
  return _ILCreate(PT_MYCOMP, &CLSID_MyComputer, sizeof(GUID));
}

ODINFUNCTION0(LPITEMIDLIST,_ILCreateIExplore)
{
   return _ILCreate(PT_MYCOMP, &CLSID_Internet, sizeof(GUID));
}

ODINFUNCTION0(LPITEMIDLIST,_ILCreateControl)
{
   return _ILCreate(PT_SPECIAL, &CLSID_ControlPanel, sizeof(GUID));
}

ODINFUNCTION0(LPITEMIDLIST,_ILCreatePrinter)
{
   return _ILCreate(PT_SPECIAL, &CLSID_Printers, sizeof(GUID));
}

ODINFUNCTION0(LPITEMIDLIST,_ILCreateNetwork)
{
   return _ILCreate(PT_MYCOMP, &CLSID_NetworkPlaces, sizeof(GUID));
}

ODINFUNCTION0(LPITEMIDLIST,_ILCreateBitBucket)
{
   return _ILCreate(PT_MYCOMP, &CLSID_RecycleBin, sizeof(GUID));
}

ODINFUNCTION1(LPITEMIDLIST,_ILCreateDrive,LPCSTR,lpszNew)
{  char sTemp[4];
   lstrcpynA (sTemp,lpszNew,4);
   sTemp[2]='\\';
   sTemp[3]=0x00;
   TRACE_(pidl)("(%s)\n",sTemp);
   return _ILCreate(PT_DRIVE,(LPVOID)&sTemp[0],4);
}

ODINFUNCTION1(LPITEMIDLIST,_ILCreateFolder,WIN32_FIND_DATAA*, stffile )
{
   char  buff[MAX_PATH + 14 +1]; /* see WIN32_FIND_DATA */
   char *   pbuff = buff;
   ULONG len, len1;
   LPITEMIDLIST pidl;

   TRACE_(pidl)("(%s, %s)\n",stffile->cAlternateFileName, stffile->cFileName);

   /* prepare buffer with both names */
   len = strlen (stffile->cFileName) + 1;
   memcpy (pbuff, stffile->cFileName, len);
   pbuff += len;

   if (stffile->cAlternateFileName)
   {
     len1 = strlen (stffile->cAlternateFileName)+1;
     memcpy (pbuff, stffile->cAlternateFileName, len1);
   }
   else
   {
     len1 = 1;
     *pbuff = 0x00;
   }

   pidl = _ILCreate(PT_FOLDER, (LPVOID)buff, len + len1);

   /* set attributes */
   if (pidl)
   {
     LPPIDLDATA pData;
     pData = ODIN__ILGetDataPointer(pidl);
     FileTimeToDosDateTime(&(stffile->ftLastWriteTime),&pData->u.folder.uFileDate,&pData->u.folder.uFileTime);
     pData->u.folder.dwFileSize = stffile->nFileSizeLow;
     pData->u.folder.uFileAttribs=stffile->dwFileAttributes;
   }

   return pidl;
}

ODINFUNCTION1(LPITEMIDLIST,_ILCreateValue,WIN32_FIND_DATAA*, stffile)
{
   char  buff[MAX_PATH + 14 +1]; /* see WIN32_FIND_DATA */
   char *   pbuff = buff;
   ULONG len, len1;
   LPITEMIDLIST pidl;

   TRACE_(pidl)("(%s, %s)\n",stffile->cAlternateFileName, stffile->cFileName);

   /* prepare buffer with both names */
   len = strlen (stffile->cFileName) + 1;
   memcpy (pbuff, stffile->cFileName, len);
   pbuff += len;

   if (stffile->cAlternateFileName)
   {
     len1 = strlen (stffile->cAlternateFileName)+1;
     memcpy (pbuff, stffile->cAlternateFileName, len1);
   }
   else
   {
     len1 = 1;
     *pbuff = 0x00;
   }

   pidl = _ILCreate(PT_VALUE, (LPVOID)buff, len + len1);

   /* set attributes */
   if (pidl)
   {
     LPPIDLDATA pData;
     pData = ODIN__ILGetDataPointer(pidl);
     FileTimeToDosDateTime(&(stffile->ftLastWriteTime),&pData->u.folder.uFileDate,&pData->u.folder.uFileTime);
     pData->u.folder.dwFileSize = stffile->nFileSizeLow;
     pData->u.folder.uFileAttribs=stffile->dwFileAttributes;
   }

   return pidl;
}

ODINFUNCTION1(LPITEMIDLIST,_ILCreateSpecial,LPCSTR,szGUID)
{
   IID   iid;
   CLSIDFromStringA(szGUID,&iid);
   return _ILCreate(PT_MYCOMP, &iid, sizeof(IID));
}

/**************************************************************************
 *  _ILCreate()
 *  Creates a new PIDL
 *  type = PT_DESKTOP | PT_DRIVE | PT_FOLDER | PT_VALUE
 *  pIn = data
 *  uInSize = size of data (raw)
 */

ODINFUNCTION3(LPITEMIDLIST,_ILCreate,PIDLTYPE,type,
                                     LPCVOID, pIn,
                                     UINT16,  uInSize)
{  LPITEMIDLIST   pidlOut = NULL, pidlTemp = NULL;
   LPPIDLDATA     pData;
   UINT16         uSize = 0;
   LPSTR pszDest;

   switch (type)
   { case PT_DESKTOP:
       uSize = 0;
       pidlOut = (ITEMIDLIST*)SHAlloc(uSize + 2);
       pidlOut->mkid.cb = uSize;
       TRACE_(pidl)("- create Desktop\n");
       break;

     case PT_MYCOMP:
       uSize = 2 + 2 + sizeof(GUID);
       pidlOut = (ITEMIDLIST*)SHAlloc(uSize + 2);
       ZeroMemory(pidlOut, uSize + 2);
       pidlOut->mkid.cb = uSize;
       pData =ODIN__ILGetDataPointer(pidlOut);
       pData->type = type;
       memcpy(&(pData->u.mycomp.guid), pIn, uInSize);
       TRACE_(pidl)("- create GUID-pidl\n");
       break;

     case PT_DRIVE:
       uSize = 2 + 23;
       pidlOut = (ITEMIDLIST*)SHAlloc(uSize + 2);
       ZeroMemory(pidlOut, uSize + 2);
       pidlOut->mkid.cb = uSize;
       pData =ODIN__ILGetDataPointer(pidlOut);
       pData->type = type;
       pszDest =  ODIN__ILGetTextPointer(type, pData);
       memcpy(pszDest, pIn, uInSize);
       TRACE_(pidl)("- create Drive: %s\n",debugstr_a(pszDest));
       break;

     case PT_FOLDER:
     case PT_VALUE:
       uSize = 2 + 12 + uInSize;
       pidlOut = (ITEMIDLIST*)SHAlloc(uSize + 2);
       ZeroMemory(pidlOut, uSize + 2);
       pidlOut->mkid.cb = uSize;
       pData =ODIN__ILGetDataPointer(pidlOut);
       pData->type = type;
       pszDest =  ODIN__ILGetTextPointer(type, pData);
       memcpy(pszDest, pIn, uInSize);
       TRACE_(pidl)("- create Value: %s\n",debugstr_a(pszDest));
       break;
   }

   pidlTemp = ODIN_ILGetNext(pidlOut);
   if (pidlTemp)
     pidlTemp->mkid.cb = 0x00;

   TRACE_(pidl)("-- (pidl=%p, size=%u)\n", pidlOut, uSize);
   return pidlOut;
}

/**************************************************************************
 *  _ILGetDrive()
 *
 *  Gets the text for the drive eg. 'c:\'
 *
 * RETURNS
 *  strlen (lpszText)
 */

ODINFUNCTION3(DWORD,_ILGetDrive,LPCITEMIDLIST, pidl,
                                LPSTR,         pOut,
                                UINT16,        uSize)
{
   if(_ILIsMyComputer(pidl))
     pidl = ODIN_ILGetNext(pidl);

   if (pidl && _ILIsDrive(pidl))
     return ODIN__ILSimpleGetText(pidl, pOut, uSize);

   return 0;
}

/**************************************************************************
 *
 * ### 2. section testing pidls ###
 *
 **************************************************************************
 *  ODIN__ILIsDesktop()
 *  _ILIsMyComputer()
 *  _ILIsSpecialFolder()
 *  _ILIsDrive()
 *  _ILIsFolder()
 *  ODIN__ILIsValue()
 *  _ILIsPidlSimple()
 */

ODINFUNCTION1(BOOL,_ILIsDesktop,LPCITEMIDLIST, pidl)
{
   return ( !pidl || (pidl && pidl->mkid.cb == 0x00) );
}

ODINFUNCTION1(BOOL,_ILIsMyComputer,LPCITEMIDLIST, pidl)
{
   REFIID iid = _ILGetGUIDPointer(pidl);

   TRACE_(pidl)("(%p)\n",pidl);

   if (iid)
     return IsEqualIID(iid, &CLSID_MyComputer);
   return FALSE;
}

ODINFUNCTION1(BOOL,_ILIsSpecialFolder,LPCITEMIDLIST, pidl)
{
   LPPIDLDATA lpPData = ODIN__ILGetDataPointer(pidl);
   TRACE_(pidl)("(%p)\n",pidl);
   return (pidl && ( (lpPData && (PT_MYCOMP== lpPData->type || PT_SPECIAL== lpPData->type)) ||
           (pidl && pidl->mkid.cb == 0x00)
         ));
}

ODINFUNCTION1(BOOL,_ILIsDrive,LPCITEMIDLIST, pidl)
{  LPPIDLDATA lpPData = ODIN__ILGetDataPointer(pidl);
   TRACE_(pidl)("(%p)\n",pidl);
   return (pidl && lpPData && (PT_DRIVE == lpPData->type ||
                PT_DRIVE1 == lpPData->type ||
                PT_DRIVE2 == lpPData->type ||
                PT_DRIVE3 == lpPData->type));
}

ODINFUNCTION1(BOOL,_ILIsFolder,LPCITEMIDLIST, pidl)
{  LPPIDLDATA lpPData = ODIN__ILGetDataPointer(pidl);
   TRACE_(pidl)("(%p)\n",pidl);
   return (pidl && lpPData && (PT_FOLDER == lpPData->type || PT_FOLDER1 == lpPData->type));
}

ODINFUNCTION1(BOOL,_ILIsValue,LPCITEMIDLIST, pidl)
{  LPPIDLDATA lpPData = ODIN__ILGetDataPointer(pidl);
   TRACE_(pidl)("(%p)\n",pidl);
   return (pidl && lpPData && PT_VALUE == lpPData->type);
}

/**************************************************************************
 * _ILIsPidlSimple
 */
ODINFUNCTION1(BOOL,_ILIsPidlSimple,LPCITEMIDLIST, pidl)
{
   BOOL ret = TRUE;

   if(! ODIN__ILIsDesktop(pidl))              /* pidl=NULL or mkid.cb=0 */
   {
     WORD len = pidl->mkid.cb;
     LPCITEMIDLIST pidlnext = (LPCITEMIDLIST) (((LPBYTE)pidl) + len );
     if (pidlnext->mkid.cb)
       ret = FALSE;
   }

   TRACE_(pidl)("%s\n", ret ? "Yes" : "No");
   return ret;
}

/**************************************************************************
 *
 * ### 3. section getting values from pidls ###
 */

 /**************************************************************************
 *  ODIN__ILSimpleGetText
 *
 * gets the text for the first item in the pidl (eg. simple pidl)
 *
 * returns the lenght of the string
 */

ODINFUNCTION3(DWORD, _ILSimpleGetText, LPCITEMIDLIST, pidl,
                                       LPSTR,         szOut,
                                       UINT,          uOutSize)
{
   LPPIDLDATA  pData;
   DWORD    dwReturn=0;
   LPSTR    szSrc;
   GUID const *   riid;
   char szTemp[MAX_PATH];

   if (!pidl) return 0;

   if (szOut)
     *szOut = 0;

   pData = ODIN__ILGetDataPointer(pidl);

   if (ODIN__ILIsDesktop(pidl))
   {
    /* desktop */
     if (HCR_GetClassName(&CLSID_ShellDesktop, szTemp, MAX_PATH))
     {
       if (szOut)
         lstrcpynA(szOut, szTemp, uOutSize);

       dwReturn = strlen (szTemp);
     }
   }
   else if (( szSrc = ODIN__ILGetTextPointer(pData->type, pData) ))
   {
     /* filesystem */
     if (szOut)
       lstrcpynA(szOut, szSrc, MAX_PATH);

     dwReturn = strlen(szSrc);
   }
   else if (( riid = _ILGetGUIDPointer(pidl) ))
   {
     /* special folder */
     if ( HCR_GetClassName(riid, szTemp, MAX_PATH) )
     {
       if (szOut)
         lstrcpynA(szOut, szTemp, uOutSize);

       dwReturn = strlen (szTemp);
     }
   }
   else
   {
     ERR_(pidl)("-- no text\n");
   }

   TRACE_(pidl)("-- (%p=%s 0x%08lx)\n",szOut,(char*)szOut,dwReturn);
   return dwReturn;
}

/**************************************************************************
 *
 * ### 4. getting pointers to parts of pidls ###
 *
 **************************************************************************
 *  ODIN__ILGetDataPointer()
 */

ODINFUNCTION1(LPPIDLDATA,_ILGetDataPointer,LPITEMIDLIST, pidl)
{
   if(pidl && pidl->mkid.cb != 0x00)
     return (LPPIDLDATA) &(pidl->mkid.abID);
   return NULL;
}

/**************************************************************************
 *  ODIN__ILGetTextPointer()
 * gets a pointer to the long filename string stored in the pidl
 */

ODINFUNCTION2(LPSTR,_ILGetTextPointer,PIDLTYPE,   type,
                                      LPPIDLDATA, pidldata)
{/*   TRACE(pidl,"(type=%x data=%p)\n", type, pidldata);*/

   if(!pidldata)
   { return NULL;
   }

   switch (type)
   {
     case PT_MYCOMP:
     case PT_SPECIAL:
       return NULL;

     case PT_DRIVE:
     case PT_DRIVE1:
     case PT_DRIVE2:
     case PT_DRIVE3:
       return (LPSTR)&(pidldata->u.drive.szDriveName);

     case PT_FOLDER:
     case PT_FOLDER1:
     case PT_VALUE:
     case PT_IESPECIAL:
       return (LPSTR)&(pidldata->u.file.szNames);

     case PT_WORKGRP:
     case PT_COMP:
     case PT_NETWORK:
     case PT_SHARE:
       return (LPSTR)&(pidldata->u.network.szNames);
   }
   return NULL;
}

/**************************************************************************
 *  _ILGetSTextPointer()
 * gets a pointer to the short filename string stored in the pidl
 */

ODINFUNCTION2(LPSTR,_ILGetSTextPointer,PIDLTYPE,   type,
                                       LPPIDLDATA, pidldata)
{/*   TRACE(pidl,"(type=%x data=%p)\n", type, pidldata);*/

   if(!pidldata)
     return NULL;

   switch (type)
   {
     case PT_FOLDER:
     case PT_VALUE:
     case PT_IESPECIAL:
       return (LPSTR)(pidldata->u.file.szNames + strlen (pidldata->u.file.szNames) + 1);

     case PT_WORKGRP:
       return (LPSTR)(pidldata->u.network.szNames + strlen (pidldata->u.network.szNames) + 1);
   }
   return NULL;
}

/**************************************************************************
 * _ILGetGUIDPointer()
 *
 * returns reference to guid stored in some pidls
 */

ODINFUNCTION1(REFIID,_ILGetGUIDPointer,LPCITEMIDLIST, pidl)
{
   LPPIDLDATA pdata =ODIN__ILGetDataPointer(pidl);

   if (pdata)
   {
     switch (pdata->type)
     {
       case PT_SPECIAL:
       case PT_MYCOMP:
         return (REFIID) &(pdata->u.mycomp.guid);
     }
   }
   return NULL;
}

/*************************************************************************
 * _ILGetFileDateTime
 *
 * Given the ItemIdList, get the FileTime
 *
 * PARAMS
 *      pidl        [I] The ItemIDList
 *      pFt         [I] the resulted FILETIME of the file
 *
 * RETURNS
 *     True if Successful
 *
 * NOTES
 *
 */

ODINFUNCTION2(BOOL, _ILGetFileDateTime,LPCITEMIDLIST, pidl,
                                       FILETIME *,    pFt)
{
    LPPIDLDATA pdata =ODIN__ILGetDataPointer(pidl);

    switch (pdata->type)
    {
        case PT_FOLDER:
            DosDateTimeToFileTime(pdata->u.folder.uFileDate, pdata->u.folder.uFileTime, pFt);
            break;
        case PT_VALUE:
            DosDateTimeToFileTime(pdata->u.file.uFileDate, pdata->u.file.uFileTime, pFt);
            break;
        default:
            return FALSE;
    }
    return TRUE;
}


ODINFUNCTION3(BOOL, _ILGetFileDate, LPCITEMIDLIST, pidl,
                                    LPSTR,         pOut,
                                    UINT,          uOutSize)
{
        FILETIME ft;
        FILETIME lft;
        SYSTEMTIME time;
        char strTime[16];
        UINT nTempSize=16;

        if (! _ILGetFileDateTime( pidl, &ft )) return FALSE;

        FileTimeToLocalFileTime(&ft, &lft);
        FileTimeToSystemTime (&lft, &time);

        if(GetDateFormatA(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&time, NULL,  pOut, uOutSize) > 0)
        {
            if(GetTimeFormatA(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &time, NULL, strTime, nTempSize) > 0)
            {
                strcat(pOut," ");
                strcat(pOut, strTime);
                return TRUE;
            }
        }
        return FALSE;
}

/*************************************************************************
 * _ILGetFileSize
 *
 * Given the ItemIdList, get the FileSize
 *
 * PARAMS
 *      pidl    [I] The ItemIDList
 * pOut  [I] The buffer to save the result
 *      uOutsize [I] The size of the buffer
 *
 * RETURNS
 *     The FileSize
 *
 * NOTES
 * pOut can be null when no string is needed
 *
 */

ODINFUNCTION3(BOOL,_ILGetFileSize,LPCITEMIDLIST, pidl,
                                  LPSTR,         pOut,
                                  UINT,          uOutSize)
{
   LPPIDLDATA pdata =ODIN__ILGetDataPointer(pidl);
   DWORD dwSize;

   switch (pdata->type)
   { case PT_VALUE:
       dwSize = pdata->u.file.dwFileSize;
       if (pOut) StrFormatByteSizeA(dwSize, pOut, uOutSize);
       return dwSize;
   }
   if (pOut) *pOut = 0x00;
   return 0;
}

/*************************************************************************
 * _ILGetFileSizeKB
 *
 * Given the ItemIdList, get the FileSize in KiloByte
 *
 * PARAMS
 *      pidl    [I] The ItemIDList
 *      pOut    [I] The buffer to save the result
 *      uOutsize [I] The size of the buffer
 *
 * RETURNS
 *     The FileSize in KiloBytes
 *
 * NOTES
 *      - pOut can be null when no string is needed
 *      - Standard files in explorer or FileOpen have the file size in
 *          Kilobyte
 *
 */
DWORD _ILGetFileSizeKB (LPCITEMIDLIST pidl, LPSTR pOut, UINT uOutSize)
{
    LPPIDLDATA pdata =ODIN__ILGetDataPointer(pidl);
    DWORD dwSize;
    char tempSizeStr[24];
    char kbStr[] = "KB";
    int nTempSizeLen, nNumOfComma;
    int i,j,k;

    switch (pdata->type)
    {
        case PT_VALUE:
        {
            if(pdata->u.file.dwFileSize >= 1024)
            {
                dwSize = pdata->u.file.dwFileSize / 1024;
            }
            else if(pdata->u.file.dwFileSize > 0)
            {
                dwSize = 1;
            }
            else
            {
                dwSize = 0;
            }

            if (pOut)
            {
                sprintf(tempSizeStr,"%0ld",dwSize);
                nTempSizeLen = strlen(tempSizeStr);
                nNumOfComma = nTempSizeLen / 4;
                uOutSize = nNumOfComma + strlen(tempSizeStr);
                for(i=nTempSizeLen, j=uOutSize, k=0; j >= 0; j--, k++)
                {
                    if((k %4) == 0 && k != 0)
                    {
                        pOut[j] = ',';
                        k = 0;
                    }
                    else
                    {
                        pOut[j] = tempSizeStr[i--];
                    }
                }
                strcat(pOut,kbStr);
            }
            return dwSize;
        }
    }
    return 0;
}

ODINFUNCTION3(BOOL,_ILGetExtension,LPCITEMIDLIST, pidl,
                                   LPSTR,         pOut,
                                   UINT,          uOutSize)
{
   char szTemp[MAX_PATH];
   const char * pPoint;
   LPITEMIDLIST  pidlTemp=pidl;

   if (!pidl) return FALSE;

   pidlTemp = ODIN_ILFindLastID(pidl);

   if (!ODIN__ILIsValue(pidlTemp)) return FALSE;
   if (!ODIN__ILSimpleGetText(pidlTemp, szTemp, MAX_PATH)) return FALSE;

   pPoint = PathFindExtensionA(szTemp);

   if (! *pPoint) return FALSE;

   pPoint++;
   lstrcpynA(pOut, pPoint, uOutSize);
   TRACE_(pidl)("%s\n",pOut);

   return TRUE;
}

/*************************************************************************
 * _ILGetFileType
 *
 * Given the ItemIdList, get the file type description
 *
 * PARAMS
 *      pidl        [I] The ItemIDList (simple)
 *      pOut        [I] The buffer to save the result
 *      uOutsize    [I] The size of the buffer
 *
 * RETURNS
 * nothing
 *
 * NOTES
 * This function copies as much as possible into the buffer.
 */
ODINPROCEDURE3(_ILGetFileType,LPCITEMIDLIST, pidl,
                              LPSTR,         pOut,
                              UINT,          uOutSize)
{
   if(ODIN__ILIsValue(pidl))
   {
     char sTemp[64];
          if(uOutSize > 0)
          {
            pOut[0] = 0;
          }
     if (_ILGetExtension (pidl, sTemp, 64))
     {
       if (!( HCR_MapTypeToValue(sTemp, sTemp, 64, TRUE)
           && HCR_MapTypeToValue(sTemp, pOut, uOutSize, FALSE )))
       {
         lstrcpynA (pOut, sTemp, uOutSize - 6);
         strcat (pOut, "-file");
       }
     }
   }
   else
   {
     lstrcpynA(pOut, "Folder", uOutSize);
   }
}

/*************************************************************************
 * _ILGetFileAttributes
 *
 * Given the ItemIdList, get the Attrib string format
 *
 * PARAMS
 *      pidl        [I] The ItemIDList
 *      pOut        [I] The buffer to save the result
 *      uOutsize    [I] The size of the Buffer
 *
 * RETURNS
 *     Attributes
 *
 * NOTES
 *
 */
ODINFUNCTION3(BOOL, _ILGetFileAttributes, LPCITEMIDLIST, pidl,
                                        LPSTR,         pOut,
                                        UINT,          uOutSize)
{
    LPPIDLDATA pData =ODIN__ILGetDataPointer(pidl);
    WORD wAttrib = 0;
    int i;
  
    if (! pData) return 0;
  
    switch(pData->type)
    {
        case PT_FOLDER:
            wAttrib = pData->u.folder.uFileAttribs;
            break;
        case PT_VALUE:
            wAttrib = pData->u.file.uFileAttribs;
            break;
    }
  
    if(uOutSize >= 6)
    {
      i=0;
      if(wAttrib & FILE_ATTRIBUTE_READONLY)
      {
          pOut[i++] = 'R';
      }
      if(wAttrib & FILE_ATTRIBUTE_HIDDEN)
      {
          pOut[i++] = 'H';
      }
      if(wAttrib & FILE_ATTRIBUTE_SYSTEM)
      {
          pOut[i++] = 'S';
      }
      if(wAttrib & FILE_ATTRIBUTE_ARCHIVE)
      {
          pOut[i++] = 'A';
      }
      if(wAttrib & FILE_ATTRIBUTE_COMPRESSED)
      {
          pOut[i++] = 'C';
      }
      pOut[i] = 0x00;
    }
    return wAttrib;
}


/*************************************************************************
* ILFreeaPidl
*
* free a aPidl struct
*/
ODINPROCEDURE2(_ILFreeaPidl,
               LPITEMIDLIST *, apidl, 
               UINT, cidl)
{
	int   i;

	if(apidl)
	{
	  for(i = 0; i < cidl; i++) SHFree(apidl[i]);
	  SHFree(apidl);
	}
}

/*************************************************************************
* ILCopyaPidl
*
* copys a aPidl struct
*/
ODINFUNCTION2(LPITEMIDLIST *,_ILCopyaPidl,
              LPITEMIDLIST *,apidlsrc, 
              UINT,          cidl)
{
	int i;
	LPITEMIDLIST * apidldest = (LPITEMIDLIST*)SHAlloc(cidl * sizeof(LPITEMIDLIST));
	if(!apidlsrc) return NULL;

	for(i = 0; i < cidl; i++)
	  apidldest[i] = ILClone(apidlsrc[i]);

	return apidldest;
}

/*************************************************************************
* _ILCopyCidaToaPidl
*
* creates aPidl from CIDA
*/
ODINFUNCTION2(LPITEMIDLIST*, _ILCopyCidaToaPidl,
              LPITEMIDLIST*, pidl, 
              LPCIDA,        cida)
{
	int i;
	LPITEMIDLIST * dst = (LPITEMIDLIST*)SHAlloc(cida->cidl * sizeof(LPITEMIDLIST));

	if(!dst) return NULL;

	if (pidl)
	  *pidl = ILClone((LPITEMIDLIST)(&((LPBYTE)cida)[cida->aoffset[0]]));

	for(i = 0; i < cida->cidl; i++)
	  dst[i] = ILClone((LPITEMIDLIST)(&((LPBYTE)cida)[cida->aoffset[i + 1]]));

	return dst;
}
