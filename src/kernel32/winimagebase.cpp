/* $Id: winimagebase.cpp,v 1.16 2000-04-16 04:27:37 bird Exp $ */

/*
 * Win32 PE Image base class
 *
 * Copyright 1998-2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998-2000 Knut St. Osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_DOSFILEMGR                 /* File Manager values */
#define INCL_DOSMODULEMGR               /* DOS Module manager */
#define INCL_DOSERRORS                  /* DOS Error values */
#define INCL_DOSPROCESS                 /* DOS Process values */
#define INCL_DOSMISC                    /* DOS Miscellanous values */
#define INCL_WIN                        /* All Win API */
#define INCL_BASE                       /* All Dos API */
#include <os2wrap.h>                    /* Odin32 OS/2 API wrappers. */

#include <stdio.h>                      /* C Library Standard I/O */
#include <string.h>                     /* C Library string operations */
#include <stdlib.h>                     /* C Library Standard stuff */

#include <misc.h>                       /* Odin32 Miscellaneous definitions, debug stuff*/
#include <win32type.h>                  /* Odin32 Common types and definition. */
#include "winimagebase.h"               /* Odin32 Executable Image Base Class */
#include "windllbase.h"                 /* Odin32 Dll Base Class */
#include "winexebase.h"                 /* Odin32 Exe Base Class */
#include <pefile.h>                     /* Odin32 PE definitions */
#include <unicode.h>                    /* Odin32 Unicode conversion */
#include <winres.h>                     /* Odin32 Resource Class */
#include "oslibmisc.h"
#include "oslibdos.h"
#include "initterm.h"
#include <win\virtual.h>
#include "directory.h"                  /* InternalGet<Windows/System>Directory. */
#include <os2newapi.h>                  /* DosQueryHeaderInfo. */

#define DBG_LOCALLOG    DBG_winimagebase
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
Win32ImageBase::Win32ImageBase(HINSTANCE hInstance) :
    errorState(NO_ERROR), entryPoint(0), fullpath(NULL),
    tlsAddress(0), tlsIndexAddr(0), tlsInitSize(0), tlsTotalSize(0),
    tlsCallBackAddr(0), tlsIndex(-1), winres(NULL), pResDir(NULL),
    ulRVAResourceSection(0)
{
  magic = MAGIC_WINIMAGE;

  if(hInstance != -1) {
    this->hinstance = hInstance;

    char *name = OSLibGetDllName(hinstance);
    strcpy(szFileName, name);
    strupr(szFileName);

    //rename dll (os/2 -> win32) if necessary (i.e. OLE32OS2 -> OLE32)
    Win32DllBase::renameDll(szFileName, FALSE);

    name = strrchr(szFileName, '\\')+1;
    strcpy(szModule, name);

    char *dot = strrchr(szModule, '.');
    if(dot)
        *dot = 0;
  }
  else {
    szModule[0] = 0;
    this->hinstance = -1;
  }
}
//******************************************************************************
//******************************************************************************
Win32ImageBase::~Win32ImageBase()
{
 Win32Resource *res;

  while(winres)
  {
        res    = winres->next;
        delete(winres);
        winres = res;
  }
  if(fullpath)
        free(fullpath);
}
//******************************************************************************
//******************************************************************************
void Win32ImageBase::setFullPath(char *name)
{
  dassert(name, ("setFullPath, name == NULL"));
  fullpath = (char *)malloc(strlen(name)+1);
  dassert(fullpath, ("setFullPath, fullpath == NULL"));
  strcpy(fullpath, name);
}
//******************************************************************************
//Add image to dependency list of this image
//******************************************************************************
void Win32ImageBase::addDependency(Win32DllBase *image)
{
  loadedDlls.Push((ULONG)image);
}
//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::dependsOn(Win32DllBase *image)
{
 QueueItem    *item;
 BOOL          ret = FALSE;

  dlllistmutex.enter();
  item = loadedDlls.Head();
  while(item) {
    if(loadedDlls.getItem(item) == (ULONG)image) {
        ret = TRUE;
        break;
    }
    item = loadedDlls.getNext(item);
  }
  dlllistmutex.leave();
  return ret;
}
//******************************************************************************
//Returns required OS version for this image
//******************************************************************************
ULONG Win32ImageBase::getVersion()
{
  dprintf(("Win32ImageBase::getVersion: NOT IMPLEMENTED!"));
  return 0x40000; //NT 4
}


/**
 * Finds a executable module (or really any file) using the DLL search order.
 * The search order used is:
 *      1. The directory from which the application loaded.
 *      2. The current directory.
 *      3. System directory. (GetSystemDirectory returns its path)
 *         (Windows NT/2k directory name: SYSTEM32)
 *      4. (Windows NT/2k: The 16-bit Windows system directory. There
 *         is no function that obtains the path of this directory.
 *         (Directory name: SYSTEM)
 *         THIS IS NOT SEARCHED BY ODIN.)
 *      5. The Windows directory. (GetWindowsDirectory returns its path)
 *      6. The Directories listed in the PATH environment variable.
 *      7. The Directories listed in the BEGINLIBPATH.
 *      8. The Directories listed in the LIBPATH.
 *      9. The Directories listed in the ENDLIBPATH.
 *
 * @returns   Success indicator. TRUE: found  FALSE: not found.
 *            Note! pszFullname will normally contain the contents of
 *            pszFilename upon return, there is one case FALSE case when
 *            pszFullname will be empty upon return. That's when the buffer
 *            isn't large enough to hold the content of pszFilename including
 *            an extention.
 *            So, please check for the return value!
 * @param     pszFilename   File to find. This name should not have a path!
 *                          If it don't contains an '.', ".DLL" is appended to
 *                          the name.
 * @param     pszFullname   Pointer to output buffer, this will hold the
 *                          a fully qualified, uppercased, filename upon
 *                          successful return.
 * @param     cchFullname   Size of the buffer pointer to by pszFullname.
 *                          (A length of at least CCHMAXPATH is recommended.)
 * @parm      pszAltPath    Pointer to alternate first executable path. This
 *                          will include a filename or at least a backslash at
 *                          the end.
 *                          If this is NULL (which is it by default) the
 *                          executable path is used. If this is specified,
 *                          this path is used instead. This is intented used
 *                          to implement the LoadLibraryEx flag
 *                          LOAD_WITH_ALTERED_SEARCH_PATH.
 *
 * @status    Completely implemented.
 * @author    Sander van Leeuwen (sandervl@xs4all.nl)
 *            knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
BOOL Win32ImageBase::findDll(const char *pszFileName,
                             char *pszFullName, int cchFullName,
                             const char *pszAltPath /*=NULL*/)
{
    BOOL            fRet = FALSE;       /* Return value. (Pessimistic attitude! Init it to FALSE...) */
    char *          psz;                /* General string pointer. */
    int             cchFileName;        /* Length of the normalized filename (after ".DLL" is added). */
    struct localvars                    /* local variables which are to big to fit onto the stack. */
    {
        char            szPath[1024];   /* Path buffer. Used to store pathlists. 1024 should be enough */
                                        /* for LIBPATH (which at least had a limit of ca. 750 chars). */
        char            sz[CCHMAXPATH]; /* Filename/path buffer. (Normally used to build filenames */
                                        /* which are passed in as search experessions to DosFindFirst.) */
        FILEFINDBUF3    findbuf3;       /* DosFindFirst buffer. */
    } *             plv;
    int             iPath;              /* Current path or pathlist being examined. This is the loop */
                                        /* variable looping on the FINDDLL_* defines. */

    /* These defines sets the order the paths and pathlists are examined. */
    #define FINDDLL_EXECUTABLEDIR   1
    #define FINDDLL_CURRENTDIR      2
    #define FINDDLL_SYSTEM32DIR     3
    #define FINDDLL_SYSTEM16DIR     4
    #define FINDDLL_WINDIR          5
    #define FINDDLL_PATH            6
    #define FINDDLL_BEGINLIBPATH    7
    #define FINDDLL_LIBPATH         8
    #define FINDDLL_ENDLIBPATH      9
    #define FINDDLL_FIRST           FINDDLL_EXECUTABLEDIR
    #define FINDDLL_LAST            FINDDLL_ENDLIBPATH


    /** @sketch
     * Copy the filename to be found to the outputbuffer, and add .DLL if not '.'
     * is found in the name. This has two reasons:
     *  1) When searching paths we simply append the buffer contents to the path
     *     being examined.
     *  2) The buffer will at least return the passed in filename.
     */
    psz = strchr(pszFileName, '.');
    cchFileName = strlen(pszFileName) + (psz ? 0 : 4);
    if (cchFileName >= cchFullName)
    {
        dassert(cchFileName < cchFullName, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                 "cchFileName (%d) >= cchFullName (%d)",
                 pszFileName, pszFullName, cchFullName, cchFileName, cchFullName));
        *pszFullName = '\0';
        return FALSE;
    }
    strcpy(pszFullName, pszFileName);
    if (psz == NULL)
        strcpy(pszFullName + cchFileName - 4, ".DLL");


    /** @sketch
     * Allocate memory for local variables.
     */
    plv = (struct localvars *)malloc(sizeof(*plv));
    if (!plv)
    {
        dassert(plv, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                "malloc failed allocating %d bytes of memory for local variables.",
                pszFileName, pszFullName, cchFullName, sizeof(*plv)));
        return FALSE;
    }


    /** @sketch
     * Loop thru the paths and pathlists searching them for the filename.
     */
    for (iPath = FINDDLL_FIRST; iPath <= FINDDLL_LAST; iPath++)
    {
        APIRET          rc;             /* Returncode from OS/2 APIs. */
        const char  *   pszPath;        /* Pointer to the path being examined. */

        /** @sketch
         * Get the path/dir to examin. (This is determined by the value if iPath.)
         */
        switch (iPath)
        {
            case FINDDLL_EXECUTABLEDIR:
                if (pszAltPath)
                    pszPath = strcpy(plv->szPath, pszAltPath);
                else
                {
                    /* ASSUMES: getFullPath allways returns a fully qualified
                     *      path, ie. with at least one backslash. and that all
                     *      slashes are backslashes!
                     */
                    pszPath = strcpy(plv->szPath, WinExe->getFullPath());
                }
                psz = strrchr(plv->szPath, '\\');
                dassert(psz, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                        "WinExe->getFullPath returned a path not fully qualified: %s",
                        pszFileName, pszFullName, cchFullName, pszPath));
                if (psz)
                    *psz = '\0';
                else
                    continue;
                break;

            case FINDDLL_CURRENTDIR:
                pszPath = ".";
                break;

            case FINDDLL_SYSTEM32DIR:
                pszPath = InternalGetSystemDirectoryA();
                break;

            case FINDDLL_SYSTEM16DIR:
                #if 1
                continue;               /* Skip this index */
                #else
                pszPath = InternalGetWindowsDirectoryA();
                strcpy(plv->sz2, InternalGetWindowsDirectoryA());
                strcat(plv->sz2, "\SYSTEM");
                break;
                #endif

            case FINDDLL_WINDIR:
                pszPath = InternalGetWindowsDirectoryA();
                break;

            case FINDDLL_PATH:
                pszPath = getenv("PATH");
                break;

            case FINDDLL_BEGINLIBPATH:
                rc = DosQueryExtLIBPATH(plv->szPath, BEGIN_LIBPATH);
                if (rc != NO_ERROR)
                {
                    dassert(rc == NO_ERROR, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                             "DosQueryExtLIBPATH failed with rc=%d, iPath=%d",
                             pszFileName, pszFullName, cchFullName, rc, iPath));
                    continue;
                }
                pszPath = plv->szPath;
                break;

            case FINDDLL_LIBPATH:
                rc = DosQueryHeaderInfo(NULLHANDLE, 0, plv->szPath, sizeof(plv->szPath), QHINF_LIBPATH);
                if (rc != NO_ERROR)
                {
                    dassert(rc == NO_ERROR, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                             "DosQueryHeaderInfo failed with rc=%d, iPath=%d",
                             pszFileName, pszFullName, cchFullName, rc, iPath));
                    continue;
                }
                pszPath = plv->szPath;
                break;

            case FINDDLL_ENDLIBPATH:
                rc = DosQueryExtLIBPATH(plv->szPath, END_LIBPATH);
                if (rc != NO_ERROR)
                {
                    dassert(rc == NO_ERROR, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                             "DosQueryExtLIBPATH failed with rc=%d, iPath=%d",
                             pszFileName, pszFullName, cchFullName, rc, iPath));
                    continue;
                }
                pszPath = plv->szPath;
                break;

            default: /* !internalerror! */
                goto end;
        }


        /** @sketch
         * pszPath is now set to the pathlist to be searched.
         * So we'll loop thru all the paths in the list.
         */
        while (pszPath != NULL && *pszPath != '\0')
        {
            HDIR    hDir;               /* Find handle used when calling FindFirst. */
            ULONG   culFiles;           /* Number of files to find / found. */
            char *  pszNext;            /* Pointer to the next pathlist path */
            int     cch;                /* Length of path (including the slash after the slash is added). */

            /** @sketch
             * Find the end of the path.
             * Copy the path into the plv->sz buffer.
             * Set pszNext.
             */
            pszNext = strchr(pszPath, ';');
            if (pszNext != NULL)
            {
                cch = pszNext - pszPath;
                pszNext++;
            }
            else
                cch = strlen(pszPath);

            if (cch + cchFileName + 1 >= sizeof(plv->sz)) /* assertion */
            {
                dassert(cch + cchFileName + 1 < sizeof(plv->sz), ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                        "cch (%d) + cchFileName (%d) + 1 < sizeof(plv->sz) (%d) - paths too long!, iPath=%d",
                        pszFileName, pszFullName, cchFullName, cch, cchFileName, sizeof(plv->sz), iPath));
                pszPath = pszNext;
                continue;
            }
            memcpy(plv->sz, pszPath, cch); //arg! Someone made strncpy not work as supposed!


            /** @sketch
             * Add a '\\' and the filename (pszFullname) to the path;
             * then we'll have a fullpath.
             */
            plv->sz[cch++] = '\\';
            strcpy(&plv->sz[cch], pszFullName);


            /** @sketch
             *  Use DosFindFirst to check if the file exists.
             *  IF the file exists THEN
             *      Query Fullpath using OS/2 API.
             *      IF unsuccessful THEN return relative name.
             *          Check that the fullname buffer is large enough.
             *          Copy the filename found to the fullname buffer.
             *      ENDIF
             *      IF successful THEN uppercase the fullname buffer.
             *      goto end
             *  ENDIF
             */
            hDir = HDIR_CREATE;
            culFiles = 1;
            rc = DosFindFirst(plv->sz, &hDir, FILE_NORMAL,
                              &plv->findbuf3, sizeof(plv->findbuf3),
                              &culFiles, FIL_STANDARD);
            DosFindClose(hDir);
            if (culFiles >= 1 && rc == NO_ERROR)
            {
                /* Return full path - we'll currently return a relative path. */
                rc = DosQueryPathInfo(plv->sz, FIL_QUERYFULLNAME, pszFullName, cchFullName);
                fRet = rc == NO_ERROR;
                if (!fRet)
                {
                    /* Return a relative path - probably better that failing... */
                    dassert(rc == NO_ERROR, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                            "rc = %d",
                            pszFileName, pszFullName, cchFullName, rc));

                    if (cch + cchFileName + 1 <= cchFullName)
                    {
                        strcpy(pszFullName, plv->sz);
                        strcpy(pszFullName + cch, plv->findbuf3.achName);
                        fRet = TRUE;
                    }
                    else
                    {
                        dassert(cch + cchFileName + 1 > cchFullName, ("KERNEL32:Win32ImageBase::findDll(%s, 0x%08x, %d): "
                                "cch (%d) + cchFileName (%d) + 1 < cchFullName (%d); %s",
                                pszFileName, pszFullName, cchFullName, cch, cchFileName, cchFullName, plv->sz));
                    }
                }
                if (fRet) strupr(pszFullName);
                goto end;
            }

            pszPath = pszNext;
        }
    } /* for iPath */


end:
    /*
     * Cleanup: free local variables.
     */
    free(plv);
    return fRet;
}


//******************************************************************************
//******************************************************************************
BOOL Win32ImageBase::isPEImage(char *szFileName)
{
 char   filename[CCHMAXPATH];
 char  *syspath;
 HFILE  dllfile;
 IMAGE_FILE_HEADER     fh;
 HFILE  win32handle;
 ULONG  ulAction       = 0;      /* Action taken by DosOpen */
 ULONG  ulLocal        = 0;      /* File pointer position after DosSetFilePtr */
 APIRET rc             = NO_ERROR;            /* Return code */
 LPVOID win32file      = NULL;
 ULONG  ulRead;
 int    nSections, i;

  if (!findDll(szFileName, filename, sizeof(filename)))
  {
        dprintf(("KERNEL32:Win32ImageBase::isPEImage(%s) findDll failed to find the file.\n",
                 szFileName, rc));
        return FALSE;
  }

  rc = DosOpen(filename,                       /* File path name */
               &win32handle,                   /* File handle */
               &ulAction,                      /* Action taken */
               0L,                             /* File primary allocation */
               0L,                             /* File attribute */
               OPEN_ACTION_FAIL_IF_NEW |
               OPEN_ACTION_OPEN_IF_EXISTS,     /* Open function type */
               OPEN_FLAGS_NOINHERIT |
               OPEN_SHARE_DENYNONE  |
               OPEN_ACCESS_READONLY,           /* Open mode of the file */
               0L);                            /* No extended attribute */

  if (rc != NO_ERROR)
  {
        dprintf(("KERNEL32:Win32ImageBase::isPEImage(%s) failed with %u\n",
                  szFileName, rc));
        return(FALSE);
  }

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  IMAGE_DOS_HEADER *pdoshdr = (IMAGE_DOS_HEADER *)malloc(sizeof(IMAGE_DOS_HEADER));
  if(pdoshdr == NULL)   {
        DosClose(win32handle);                /* Close the file */
        return(FALSE);
  }
  rc = DosRead(win32handle, pdoshdr, sizeof(IMAGE_DOS_HEADER), &ulRead);
  if(rc != NO_ERROR) {
        DosClose(win32handle);                /* Close the file */
        return(FALSE);
  }
  ULONG hdrsize = pdoshdr->e_lfanew + SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER);
  free(pdoshdr);

  /* Move the file pointer back to the beginning of the file */
  DosSetFilePtr(win32handle, 0L, FILE_BEGIN, &ulLocal);

  win32file = malloc(hdrsize);
  if(win32file == NULL) {
        DosClose(win32handle);                /* Close the file */
        return(FALSE);
  }
  rc = DosRead(win32handle, win32file, hdrsize, &ulRead);
  if(rc != NO_ERROR) {
        goto failure;
  }

  if(GetPEFileHeader (win32file, &fh) == FALSE) {
        goto failure;
  }

  if(!(fh.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)) {//not valid
        goto failure;
  }
  if(fh.Machine != IMAGE_FILE_MACHINE_I386) {
        goto failure;
  }
  //IMAGE_FILE_SYSTEM == only drivers (device/file system/video etc)?
  if(fh.Characteristics & IMAGE_FILE_SYSTEM) {
        goto failure;
  }
  DosClose(win32handle);
  return(TRUE);

failure:
  free(win32file);
  DosClose(win32handle);
  return(FALSE);
}
//******************************************************************************
//******************************************************************************
/**
 * Static helper which finds the Win32ImageBase object corresponding to hModule.
 * @returns   Pointer to Win32ImageBase object corresponding to hModule.
 * @param     hModule  Odin32 modulehandler. 0 and -1 is aliases for the executable module
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 */
Win32ImageBase * Win32ImageBase::findModule(HMODULE hModule)
{
    Win32ImageBase *pRet;

    if (hModule == -1 || hModule == 0 ||       /* note: WinNt 4, SP4 don't accept -1 as the EXE handle.*/
        (WinExe != NULL && hModule == WinExe->getInstanceHandle())
        )
        pRet = WinExe;
    else
        pRet = Win32DllBase::findModule(hModule);

    if (pRet == NULL)
    {
        if (WinExe == NULL)
            dprintf(("Win32ImageBase::findModule: Module not found. WinExe is NULL, hModule=%#x\n", hModule));
        else
            dprintf(("Win32ImageBase::findModule: Module not found, hModule=%#x\n", hModule));
    }

    return pRet;
}


