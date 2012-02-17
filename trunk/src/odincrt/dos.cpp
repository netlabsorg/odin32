/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * OS/2 Dos API extensions
 *
 * Copyright 2010 Dmitriy Kuminov
 *
 */

/* Include files */
#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSPROFILE
#define  INCL_DOSERRORS
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <string.h>

#include "odincrt.h"

//#define EXTRALOG

#undef dprintf
#ifdef EXTRALOG

#include <stdio.h>
#include <stdarg.h>

void dbg(const char *fmt, ...)
{
    static FILE *f = 0;
    if (!f)
    {
        f = fopen ("/odin32_odincrt_dos.log", "w+");
        setbuf (f, NULL);
    }
    if (f)
    {
        va_list args;
        va_start (args, fmt);
        vfprintf (f, fmt, args);
        fprintf (f, "\n");
        va_end (args);
    }
}

#define dprintf(a) do { dbg a; } while(0)

#else // EXTRALOG
#define dprintf(a) do {} while(0)
#endif // EXTRALOG

static BOOL matchModuleName(PCSZ pszFullModname, PCSZ pszModname)
{
    // the staright case:
    if (stricmp(pszFullModname, pszModname) == 0)
        return TRUE;

    char fullFname[_MAX_FNAME];
    char fullExt[_MAX_EXT];
    _splitpath((char *)pszFullModname, NULL, NULL, fullFname, fullExt);

    char modFname[_MAX_FNAME];
    char modExt[_MAX_EXT];
    _splitpath((char *)pszModname, NULL, NULL, modFname, modExt);

    if (*modExt == 0)
        strcpy(modExt, ".DLL");

    return stricmp(fullFname, modFname) == 0 && stricmp(fullExt, modExt) == 0;
}

static BOOL walkModules(QSPTRREC *pPtrRec, USHORT hmteStart,
                        PCSZ pszModname, PHMODULE pHmod)
{
    QSLREC *pLibRec = pPtrRec->pLibRec;

    while (pLibRec)
    {
        // It happens that for some modules ctObj is > 0 but pObjInfo is
        // NULL and pNextRec points right to the object table instead of the
        // next record. This seems to be an OS/2 FP13 bug. Here is the solution
        // from winimagepe2lx.cpp. We need it too because of pNextRec.
        if (pLibRec->ctObj > 0 && pLibRec->pObjInfo == NULL)
        {
            pLibRec->pObjInfo = (QSLOBJREC *)
                ((char*) pLibRec
                 + ((sizeof(QSLREC)                         /* size of the lib record */
                     + pLibRec->ctImpMod * sizeof(USHORT)   /* size of the array of imported modules */
                     + strlen((char*)pLibRec->pName) + 1    /* size of the filename */
                     + 3) & ~3));                           /* the size is align on 4 bytes boundrary */
            pLibRec->pNextRec = (PVOID *)((char *)pLibRec->pObjInfo
                                          + sizeof(QSLOBJREC) * pLibRec->ctObj);
        }
        if (pLibRec->hmte == hmteStart)
        {
            if (matchModuleName((PCSZ)pLibRec->pName, pszModname))
            {
                *pHmod = pLibRec->hmte;
                return TRUE;
            }
            // mark as already walked
            pLibRec->hmte = NULLHANDLE;
            // walk through imported modules of this module
            USHORT *pImpMods = (USHORT *)(((ULONG) pLibRec) + sizeof(*pLibRec));
            for (ULONG i = 0; i < pLibRec->ctImpMod; ++i) {
                if (walkModules(pPtrRec, pImpMods[i], pszModname, pHmod))
                    return TRUE;
            }

            // break the loop since we already walked the module in question
            break;
        }
        pLibRec = (QSLREC *)pLibRec->pNextRec;
    }

    return FALSE;
}

APIRET WIN32API DosQueryModuleHandleStrict(PCSZ pszModname, PHMODULE pHmod)
{
    dprintf(("DosQueryModuleHandleStrict: BEGIN (%s, %p)", pszModname, pHmod));

    PPIB ppib;
    APIRET arc = DosGetInfoBlocks(NULL, &ppib);
    if (arc != NO_ERROR)
    {
        dprintf(("DosQueryModuleHandleStrict: DosGetInfoBlocks failed with %d", arc));
        return arc;
    }

    *pHmod = NULLHANDLE;

    // In LIBPATHSTRICT=T mode, there may be more than one module with the
    // given name loaded into the memory. For this reason, when looking for a
    // module by its basename, we first walk the modules belonging to the
    // current process (which is in effect of LIBPATHSTRICT=T) to pick up the
    // ones loaded due to this effect.
    char *buf = (char *)malloc(64 * 1024);
    if (buf == NULL)
    {
        dprintf(("DosQueryModuleHandleStrict: not enough memory"));
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    arc = DosQuerySysState(QS_PROCESS | QS_MTE, QS_MTE,
                           ppib->pib_ulpid, 0, buf, 64 * 1024);
    if (arc != NO_ERROR)
    {
        dprintf(("DosQueryModuleHandleStrict: DosQuerySysState failed with %d", arc));
        free(buf);
        return arc;
    }

    QSPTRREC *pPtrRec = (QSPTRREC *)buf;
    QSPREC *pProcRec = pPtrRec->pProcRec;

    // first walk the EXE's imported modules
    if (!walkModules(pPtrRec, pProcRec->hMte, pszModname, pHmod))
    {
        arc = ERROR_MOD_NOT_FOUND;

        // next, walk the modules loaded by the process
        for (USHORT i = 0; i < pProcRec->cLib; ++i)
        {
            USHORT hmte = pProcRec->pLibRec[i];
            if (walkModules(pPtrRec, hmte, pszModname, pHmod))
            {
                dprintf(("DosQueryModuleHandleStrict: found pHmod %x", *pHmod));
                arc = NO_ERROR;
                break;
            }
        }
        if (arc != NO_ERROR)
        {
            // last chance: fallback to DosQueryModuleHandle
            arc = DosQueryModuleHandle(pszModname, pHmod);
        }
    }

    dprintf(("DosQueryModuleHandleStrict: END (%d)", arc));
    free(buf);
    return arc;
}
