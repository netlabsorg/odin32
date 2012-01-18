/*
 * PELDR main exe loader code
 *
 * Copyright 1998-2001 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2012 Dmitriy Kuminov
 *
 * Command line options:
 *   /OPT:[x1=y,x2=z,..]
 *   x = CURDIR    -> set current directory to y
 *   (not other options available at this time)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_DOSMODULEMGR
#define INCL_DOSSESMGR
#define INCL_WIN
#include <os2.h>
#include <bseord.h>
#include <win32type.h>
#include <misc.h>
#include <wprocess.h>
#include <win/peexe.h>
#include <odinpe.h>
#include "pe.h"

#include "helpers.h"

char INFO_BANNER[]      = "Usage: PE winexe commandline";
char szErrorTitle[]     = "Odin";
char szLoadErrorMsg[]   = "Can't load executable %s";
char szFileNotFound[]   = "File not found: %s";
char szFileErrorMsg[]   = "File IO error";
char szPEErrorMsg[]     = "Not a valid win32 exe. (perhaps 16 bits windows)";
char szCPUErrorMsg[]    = "%s doesn't run on x86 machines";
char szExeErrorMsg[]    = "%s isn't an executable";
char szInteralErrorMsg[]= "Internal Error while loading %s";
char szInteralErrorMsg1[]= "Internal Error";
char szNoKernel32Msg[]  = "Can't load/find kernel32.dll (rc=%d, module %s)";
char szBadKernel32Msg[] = "Invalid or outdated kernel32.dll";
char szErrorExports[]   = "Unable to process exports of %s";
char szErrorMemory[]    = "Memory allocation failure while loading %s";
char szErrorImports[]   = "Failure to load \"%s\" due to bad or missing %s";

char szErrDosStartSession[] = "Failed to start win16 (%s) for %s (rc=%d)";

char fullpath[CCHMAXPATH];

typedef HAB  (* APIENTRY WININITIALIZEPROC)(ULONG flOptions);
typedef BOOL (* APIENTRY WINTERMINATEPROC)(HAB hab);
typedef HMQ  (* APIENTRY WINCREATEMSGQUEUEPROC) (HAB hab, LONG cmsg);
typedef BOOL (* APIENTRY WINDESTROYMSGQUEUEPROC) (HMQ hmq);
typedef ULONG (* APIENTRY WINMESSAGEBOXPROC) (HWND hwndParent,
                                              HWND hwndOwner,
                                              PCSZ  pszText,
                                              PCSZ  pszCaption,
                                              ULONG idWindow,
                                              ULONG flStyle);
typedef void (* KRNL32EXCEPTPROC) (void *exceptframe);

WININITIALIZEPROC      MyWinInitialize      = 0;
WINTERMINATEPROC       MyWinTerminate       = 0;
WINCREATEMSGQUEUEPROC  MyWinCreateMsgQueue  = 0;
WINDESTROYMSGQUEUEPROC MyWinDestroyMsgQueue = 0;
WINMESSAGEBOXPROC      MyWinMessageBox      = 0;
KRNL32EXCEPTPROC       Krnl32SetExceptionHandler = 0;
KRNL32EXCEPTPROC       Krnl32UnsetExceptionHandler = 0;


WIN32CTOR   CreateWin32Exe       = 0;
ULONG       reservedMemory       = 0;
BOOL        fConsoleApp          = FALSE;

BOOL AllocateExeMem(char *filename, BOOL *fNEExe);

#ifdef DEBUG
void print(const char *fmt, ...)
{
    static char buf[1024];
    char *s, *e;
    ULONG dummy;
    int len;
    va_list args;
    va_start(args, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    s = buf;
    while (*s)
    {
        e = (char *)str_find_char(s, '\n');
        DosWrite((HFILE)1, s, e - s, &dummy);
        if (*e == '\n')
        {
            DosWrite((HFILE)1, "\n\r", 2, &dummy);
            e++;
        }
        s = e;
    }
}
#define DBG(a) do { print a; } while (0)
#else
#define DBG(a) do {} while (0)
#endif

//******************************************************************************
//******************************************************************************
int simple_main()
{
    DBG(("PE: BEGIN (DEBUG mode)\n"));

    HAB    hab = 0;                             /* PM anchor block handle       */
    HMQ    hmq = 0;                             /* Message queue handle         */
    char   exeName[CCHMAXPATH];
    char   fullpath[CCHMAXPATH * 2];
    char   errorMod[CCHMAXPATH];
    char   szErrorMsg[512];
    char  *pszErrorMsg = NULL;
    APIRET  rc;
    HMODULE hmodPMWin = 0, hmodKernel32 = 0;
    PTIB   ptib;
    PPIB   ppib;
    char  *cmdline, *win32cmdline, *peoptions;
    BOOL   fVioConsole, fIsNEExe;

    char *pszTemp, *pszTemp2;

    DosGetInfoBlocks(&ptib, &ppib);

#ifdef COMMAND_LINE_VERSION
    if(DosGetInfoBlocks(&ptib, &ppib) == 0)
    {
        //switch process type to PM so the command line app can create PM
        //windows
        ppib->pib_ultype = 3;
    }
#endif

    fullpath[0] = 0;
    DosQueryModuleName(ppib->pib_hmte, sizeof(fullpath), fullpath);

    pszTemp = fullpath + strlen(fullpath) - 1;
    while (pszTemp >= fullpath && (*pszTemp != '\\' && *pszTemp != '/'))
        --pszTemp;
    if (pszTemp >= fullpath)
    {
        *pszTemp = 0;
        strcat(fullpath, ";%BeginLIBPATH%");
        DosSetExtLIBPATH(fullpath, BEGIN_LIBPATH);
        DBG(("PE: Added '%s' to BEGINLIBPATH\n", fullpath));
    }

    cmdline = ppib->pib_pchcmd;
    if (cmdline)
    {
        cmdline += strlen(cmdline) + 1; // skip executable name
        if (!*cmdline)
            cmdline = NULL;
    }

    if (cmdline)
    {
        DBG(("PE: Full command line:  '%s'\n", cmdline));

        pszTemp = (char *)str_skip_char(cmdline, ' ');

        // get PE options
        if (str_starts_with(pszTemp, "/OPT:["))
        {
            const char *end = str_find_char(pszTemp , ']');
            if (*end)
                pszTemp = (char *)str_skip_char(end + 1, ' ');
            else
                peoptions = NULL;
        }

        // get Win32 executable name
        int delim = ' ';
        if (*pszTemp == '"')
        {
            pszTemp++;
            delim = '"';
        }
        pszTemp2 = exeName;
        while (*pszTemp && *pszTemp != delim &&
               (pszTemp2 - exeName) < CCHMAXPATH - 1)
        {
            *pszTemp2++ = *pszTemp++;
        }
        *pszTemp2 = '\0';
        if (delim == '"')
            pszTemp++;
        pszTemp = (char *)str_skip_char(pszTemp, ' ');

        // get Win32 command line
        win32cmdline = pszTemp;

        // add the .EXE extension if missing
        int i = strlen(exeName);
        if (i < 4 || !((exeName[i-1] == 'E' || exeName[i-1] == 'e') &&
                       (exeName[i-2] == 'X' || exeName[i-2] == 'x') &&
                       (exeName[i-3] == 'E' || exeName[i-3] == 'e') &&
                       exeName[i-4] == '.'))
        {
            if (i + 4 < CCHMAXPATH)
                strcat(exeName, ".exe");
        }

        // try to locate the executable
        pszTemp = exeName + strlen(exeName) - 1;
        while (pszTemp >= exeName && (*pszTemp != '\\' && *pszTemp != '/'))
            --pszTemp;
        if (pszTemp < exeName)
        {
            // no path information, perform a search

            char newExeName[CCHMAXPATH];

            if(DosSearchPath( SEARCH_CUR_DIRECTORY | SEARCH_ENVIRONMENT | SEARCH_IGNORENETERRS
                              , "WINDOWSPATH"           /* environment value */
                              , exeName                 /* Name of file to look for */
                              , newExeName              /* Result of the search     */
                              , sizeof(newExeName)      /* Length of search buffer  */
                              ) == NO_ERROR)
            {
                strcpy(exeName, newExeName);
            }
        }

        DBG(("PE: Win32 EXE:          '%s'\n", exeName));
        DBG(("PE: Win32 command line: '%s'\n", win32cmdline));

        FILESTATUS3 fstat3;
        if (DosQueryPathInfo(exeName, FIL_STANDARD, (PVOID)&fstat3, sizeof(fstat3)) != NO_ERROR)
        {
            snprintf(szErrorMsg, sizeof(szErrorMsg), szFileNotFound, exeName);
            pszErrorMsg = szErrorMsg;
        }
    }
    else
    {
        // No command line, show the usage message
        pszErrorMsg = INFO_BANNER;
    }

    if (!pszErrorMsg)
    {
        if (AllocateExeMem(exeName, &fIsNEExe))
        {
            if (fIsNEExe)
            {
                STARTDATA sdata = {0};
                ULONG idSession;
                PID   pid;

                sdata.Length    = sizeof(sdata);
                sdata.PgmName   = (PSZ)"w16odin.exe";
                strcpy(fullpath, exeName);
                strcat(fullpath, " ");
                strcat(fullpath, win32cmdline);
                sdata.PgmInputs = fullpath;
                sdata.FgBg      = SSF_FGBG_FORE;
                sdata.SessionType = SSF_TYPE_WINDOWEDVDM;
                rc = DosStartSession(&sdata, &idSession, &pid);
                if (rc)
                {
                    snprintf(szErrorMsg, sizeof(szErrorMsg), szErrDosStartSession, sdata.PgmName, exeName, rc);
                    pszErrorMsg = szErrorMsg;
                }
                else
                {
                    // we don't need to do anything else, return shortly
                    DBG(("PE: END (returning 0)\n"));
                    return 0;
                }
            }
        }
        else
        {
            snprintf(szErrorMsg, sizeof(szErrorMsg), szErrorMemory, exeName);
            pszErrorMsg = szErrorMsg;
        }
    }

    // Resolve PM functions (needed to show the pszErrorMsg box too)
    rc = DosLoadModule(exeName, sizeof(exeName), "PMWIN", &hmodPMWin);
    rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32INITIALIZE, NULL, (PFN *)&MyWinInitialize);
    rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32TERMINATE, NULL, (PFN *)&MyWinTerminate);
    rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32CREATEMSGQUEUE, NULL, (PFN *)&MyWinCreateMsgQueue);
    rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32DESTROYMSGQUEUE, NULL, (PFN *)&MyWinDestroyMsgQueue);
    rc = DosQueryProcAddr(hmodPMWin, ORD_WIN32MESSAGEBOX, NULL, (PFN *)&MyWinMessageBox);

    if ((hab = MyWinInitialize(0)) &&
        (hmq = MyWinCreateMsgQueue(hab, 0)))
    {
        if (!pszErrorMsg)
        {
            errorMod[0] = 0;
            rc = DosLoadModule(errorMod, sizeof(errorMod), "KERNEL32", &hmodKernel32);
            if (rc)
            {
                snprintf(szErrorMsg, sizeof(szErrorMsg), szNoKernel32Msg, rc, errorMod);
                pszErrorMsg = szErrorMsg;
            }
            else
            {
                rc = DosQueryProcAddr(hmodKernel32, 0, "_CreateWin32PeLdrExe@36", (PFN *)&CreateWin32Exe);
                if (rc)
                {
                    snprintf(szErrorMsg, sizeof(szErrorMsg), szBadKernel32Msg);
                    pszErrorMsg = szErrorMsg;
                }
            }

            if (!pszErrorMsg)
            {
#ifdef COMMAND_LINE_VERSION
                fVioConsole = TRUE;
#else
                fVioConsole = FALSE;
#endif
                DBG(("PE: fVioConsole: %d\n", fVioConsole));

                rc = CreateWin32Exe(exeName, win32cmdline, peoptions, reservedMemory, 0,
                                    fConsoleApp, fVioConsole, errorMod, sizeof(errorMod));
                if (rc != LDRERROR_SUCCESS)
                {
                    switch (rc)
                    {
                    case LDRERROR_INVALID_MODULE:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szLoadErrorMsg, exeName);
                        break;
                    case LDRERROR_INVALID_CPU:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szCPUErrorMsg, exeName);
                        break;
                    case LDRERROR_FILE_SYSTEM:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szExeErrorMsg, exeName);
                        break;
                    case LDRERROR_MEMORY:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szErrorMemory, exeName);
                        break;
                    case LDRERROR_EXPORTS:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szErrorExports, exeName);
                        break;
                    case LDRERROR_IMPORTS:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szErrorImports, exeName, errorMod);
                        break;
                    case LDRERROR_INVALID_SECTION:
                    default:
                        snprintf(szErrorMsg, sizeof(szErrorMsg), szInteralErrorMsg, exeName);
                        break;
                    }

                    pszErrorMsg = szErrorMsg;
                }
            }
        }

        if (pszErrorMsg)
            MyWinMessageBox(HWND_DESKTOP, NULLHANDLE, pszErrorMsg, szErrorTitle,
                            0, MB_OK | MB_ERROR | MB_MOVEABLE);
    }
    else
    {
        DBG(("PE: WinInitialize/WinCreateMsgQueue failed!\n"));
    }

    if(hmodKernel32)
        DosFreeModule(hmodKernel32);

    if (hmq)
        MyWinDestroyMsgQueue(hmq);
    if (hab)
        MyWinTerminate(hab);

    if(hmodPMWin)
        DosFreeModule(hmodPMWin);

#ifdef DEBUG
    if (pszErrorMsg)
        DBG(("PE: Error: '%s'\n", pszErrorMsg));
#endif

    DBG(("PE: END (returning %d)\n", pszErrorMsg ? 1 : 0));
    return pszErrorMsg ? 1 : 0;
}

//******************************************************************************
//SvL: Reserve memory for win32 exes without fixups
//     This is done before any Odin or PMWIN dll is loaded, so we'll get
//     a very low virtual address. (which is exactly what we want)
//******************************************************************************
BOOL AllocateExeMem(char *filename, BOOL *fNEExe)
{
    HFILE  dllfile = 0;
    char   *tmp;
    ULONG  action, ulRead, signature;
    APIRET rc;
    IMAGE_DOS_HEADER doshdr;
    IMAGE_OPTIONAL_HEADER oh;
    IMAGE_FILE_HEADER     fh;
    ULONG  address = 0;
    ULONG  alloccnt = 0;
    ULONG  diff, i, baseAddress;
    ULONG  ulSysinfo, flAllocMem = 0;
    BOOL   ret = FALSE;

    //Reserve enough space to store 4096 pointers to 1MB memory chunks
    static ULONG memallocs[4096];

    *fNEExe = FALSE;

    rc = DosOpen(filename, &dllfile, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW, OPEN_SHARE_DENYNONE|OPEN_ACCESS_READONLY, NULL);
    if(rc) {
        goto end; //oops
    }

    //read dos header
    if(DosRead(dllfile, (LPVOID)&doshdr, sizeof(doshdr), &ulRead)) {
        goto end;
    }
    if(DosSetFilePtr(dllfile, doshdr.e_lfanew, FILE_BEGIN, &ulRead)) {
        goto end;
    }
    //read signature dword
    if(DosRead(dllfile, (LPVOID)&signature, sizeof(signature), &ulRead)) {
        goto end;
    }
    //read pe header
    if(DosRead(dllfile, (LPVOID)&fh, sizeof(fh), &ulRead)) {
        goto end;
    }
    //read optional header
    if(DosRead(dllfile, (LPVOID)&oh, sizeof(oh), &ulRead)) {
        goto end;
    }
    if(doshdr.e_magic != IMAGE_DOS_SIGNATURE || signature != IMAGE_NT_SIGNATURE) {
        if(LOWORD(signature) == IMAGE_OS2_SIGNATURE) {
            *fNEExe = TRUE;
        }
        goto end;
    }
    fConsoleApp = (oh.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);

    DBG(("PE: AllocateExeMem: oh.Subsystem: %d\n", oh.Subsystem));
    DBG(("PE: AllocateExeMem: oh.ImageBase: 0x%08X\n", oh.ImageBase));

    // check for high memory support
    rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &ulSysinfo, sizeof(ulSysinfo));
    if (rc == 0 && ulSysinfo > 512)   //VirtualAddresslimit is in MB
    {
        flAllocMem = PAG_ANY;      // high memory support. Let's use it!
    }

    DBG(("PE: AllocateExeMem: VIRTUALADDRESSLIMIT: %d (rc %d)\n", ulSysinfo, rc));

    if(oh.ImageBase < 512*1024*1024) {
        flAllocMem = 0;
    }
    else {
        if(flAllocMem == 0) {
            goto end; //no support for > 512 MB
        }
    }
    while(TRUE) {
        rc = DosAllocMem((PPVOID)&address, FALLOC_SIZE, PAG_READ | flAllocMem);
        if(rc) break;

        if(address + FALLOC_SIZE >= oh.ImageBase) {
            if(address > oh.ImageBase) {//we've passed it!
                DosFreeMem((PVOID)address);
                break;
            }
            //found the right address
            DosFreeMem((PVOID)address);

            diff = oh.ImageBase - address;
            if(diff) {
                rc = DosAllocMem((PPVOID)&address, diff, PAG_READ | flAllocMem);
                if(rc) break;
            }
            rc = DosAllocMem((PPVOID)&baseAddress, oh.SizeOfImage, PAG_READ | PAG_WRITE | flAllocMem);
            if(rc) break;

            if(diff) DosFreeMem((PVOID)address);

            reservedMemory = baseAddress;
            break;
        }
        memallocs[alloccnt++] = address;
    }
    for(i=0;i<alloccnt;i++) {
        DosFreeMem((PVOID)memallocs[i]);
    }
    DBG(("PE: AllocateExeMem: reservedMemory: 0x%08X\n", reservedMemory));
    ret = TRUE;
end:
    if(dllfile) DosClose(dllfile);
    DBG(("PE: AllocateExeMem: returning %d\n", ret));
    return ret;
}
//******************************************************************************
//******************************************************************************
