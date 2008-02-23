/* $Id: kdtLoad.c,v 1.1 2002-09-30 23:53:54 bird Exp $
 *
 * Ring-3 Device Testing: Loader and Load Hacks.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define DWORD   ULONG
#define WORD    USHORT
#define FOR_EXEHDR 1


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#include <os2.h>
#include <exe386.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kDevTest.h"

#ifndef QS_MTE
/*******************************************************************************
*   From the OS/2 4.5 toolkit.                                                 *
*******************************************************************************/
/* defines and structures for DosQuerySysState */
#pragma pack(1)
/* record types */
#define QS_PROCESS      0x0001
#define QS_SEMAPHORE    0x0002
//#define QS_MTE          0x0004
#define QS_FILESYS      0x0008
#define QS_SHMEMORY     0x0010
#define QS_DISK         0x0020
#define QS_HWCONFIG     0x0040
#define QS_NAMEDPIPE    0x0080
#define QS_THREAD       0x0100
#define QS_MODVER       0x0200

/* valid EntityList bit settings */
#define QS_SUPPORTED    (QS_PROCESS|QS_SEMAPHORE|QS_MTE|QS_FILESYS|QS_SHMEMORY|QS_MODVER)

/* All structures must be padded to dword boundaries if necessary  */
/* The semicolon that is needed to terminate the structure field   */
/* must be added in the structure definition itself, because H2INC */
/* ignores it in a #define statement. */
#define PADSHORT        USHORT        pad_sh
#define PADCHAR         UCHAR         pad_ch

#define QS_END          0L       /* last FILESYS record */

/* Global Record structure
 * Holds all global system information. Placed first in user buffer
 */
typedef struct _QSGREC {  /* qsGrec */
        ULONG         cThrds;
        ULONG         c32SSem;
        ULONG         cMFTNodes;
} QSGREC;

/* Thread Record structure
 *      Holds all per thread information.
 */
typedef struct _QSTREC {  /* qsTrec */
        ULONG         RecType;        /* Record Type */
        USHORT        tid;            /* thread ID */
        USHORT        slot;           /* "unique" thread slot number */
        ULONG         sleepid;        /* sleep id thread is sleeping on */
        ULONG         priority;       /* thread priority */
        ULONG         systime;        /* thread system time */
        ULONG         usertime;       /* thread user time */
        UCHAR         state;          /* thread state */
        PADCHAR;
        PADSHORT;
} QSTREC;

/* Process Record structure
 *      Holds all per process information.
 *      ________________________________
 *      |       RecType                 |
 *      |-------------------------------|
 *      |       pThrdRec                |----|
 *      |-------------------------------|    |
 *      |       pid                     |    |
 *      |-------------------------------|    |
 *      |       ppid                    |    |
 *      |-------------------------------|    |
 *      |       type                    |    |
 *      |-------------------------------|    |
 *      |       stat                    |    |
 *      |-------------------------------|    |
 *      |       sgid                    |    |
 *      |-------------------------------|    |
 *      |       hMte                    |    |
 *      |-------------------------------|    |
 *      |       cTCB                    |    |
 *      |-------------------------------|    |
 *      |       c32PSem                 |    |
 *      |-------------------------------|    |
 *      |       p32SemRec               |----|---|
 *      |-------------------------------|    |   |
 *      |       c16Sem                  |    |   |
 *      |-------------------------------|    |   |
 *      |       cLib                    |    |   |
 *      |-------------------------------|    |   |
 *      |       cShrMem                 |    |   |
 *      |-------------------------------|    |   |
 *      |       cFS                     |    |   |
 *      |-------------------------------|    |   |
 *      |       p16SemRec               |----|---|----|
 *      |-------------------------------|    |   |    |
 *      |       pLibRec                 |----|---|----|------|
 *      |-------------------------------|    |   |    |      |
 *      |       pShrMemRec              |----|---|----|------|----|
 *      |-------------------------------|    |   |    |      |    |
 *      |       pFSRec                  |----|---|----|------|----|-----|
 *      |-------------------------------|    |   |    |      |    |     |
 *      |       32SemPPRUN[0]           |<---|---|    |      |    |     |
 *      |          .                    |    |        |      |    |     |
 *      |          .                    |    |        |      |    |     |
 *      |          .                    |    |        |      |    |     |
 *      |       32SemPPRUN[c32PSem-1]   |    |        |      |    |     |
 *      |-------------------------------|    |        |      |    |     |
 *      |       16SemIndx[0]            |<---|--------|      |    |     |
 *      |          .                    |    |               |    |     |
 *      |          .                    |    |               |    |     |
 *      |          .                    |    |               |    |     |
 *      |       16SemIndx[c16Sem-1]     |    |               |    |     |
 *      |-------------------------------|    |               |    |     |
 *      |       hmte[0] (or "name str") |<---|---------------|    |     |
 *      |          .                    |    |                    |     |
 *      |          .                    |    |                    |     |
 *      |          .                    |    |                    |     |
 *      |       hmte[cLib-1]            |    |                    |     |
 *      |-------------------------------|    |                    |     |
 *      |       hshmem[0]               |<---|--------------------|     |
 *      |          .                    |    |                          |
 *      |          .                    |    |                          |
 *      |          .                    |    |                          |
 *      |       hshmem[cShrMem-1]       |    |                          |
 *      |-------------------------------|    |                          |
 *      |       fsinfo[0]               |<---|--------------------------|
 *      |          .                    |    |
 *      |          .                    |    |
 *      |          .                    |    |
 *      |       fsinfo[cFS-1]           |    |
 *      |-------------------------------|    |
 *                                      <-----
 *      NOTE that the process name string will be stored in place of hmtes
 *              if MTE information is NOT being requested.
 *      NOTE that following this structure in the user buffer is
 *              an array c32Sems long of PRUN structures for 32 bit sems
 *              an array c16Sems long of indices for 16 bit sems
 *              the process name string
 */
typedef struct _QSPREC {  /* qsPrec */
        ULONG         RecType;        /* type of record being processed */
        QSTREC  FAR *pThrdRec;      /* ptr to thread recs for this proc */
        USHORT        pid;            /* process ID */
        USHORT        ppid;           /* parent process ID */
        ULONG         type;           /* process type */
        ULONG         stat;           /* process status */
        ULONG         sgid;           /* process screen group */
        USHORT        hMte;           /* program module handle for process */
        USHORT        cTCB;           /* # of TCBs in use */
        ULONG         c32PSem;        /* # of private 32-bit sems in use */
        void      FAR *p32SemRec;     /* pointer to head of 32bit sem info */
        USHORT        c16Sem;         /* # of 16 bit system sems in use */
        USHORT        cLib;           /* number of runtime linked libraries */
        USHORT        cShrMem;        /* number of shared memory handles */
        USHORT        cFH;            /* number of open files - BUGBUG see SFNContainer! */
        USHORT   FAR  *p16SemRec;     /* pointer to head of 16 bit sem info */
        USHORT   FAR  *pLibRec;       /* ptr to list of runtime libraries */
        USHORT   FAR  *pShrMemRec;    /* ptr to list of shared mem handles */
        USHORT   FAR  *pFSRec;        /* pointer to list of file handles */
} QSPREC;

/*
 *      16 bit system semaphore structure
 *      ________________________________
 *      |       pNextRec                |----|
 *      |-------------------------------|    |
 *      |SysSemData     :               |    |
 *      |       SysSemOwner             |    |
 *      |       SysSemFlag              |    |
 *      |       SysSemRecCnt            |    |
 *      |       SysSemProcCnt           |    |
 *      |-------------------------------|    |
 *      |-------------------------------|    |
 *      |-------------------------------|    |
 *      |       SysSemPtr               |    |
 *      |-------------------------------|    |
 *      |SysSemName:                    |    |
 *      |       "pathname"              |    |
 *      |-------------------------------|    |
 *                                      <-----
 */


/* SysSemFlag values */

#define QS_SYSSEM_WAITING 0x01               /* a thread is waiting on the sem */
#define QS_SYSSEM_MUXWAITING 0x02            /* a thread is muxwaiting on the sem */
#define QS_SYSSEM_OWNER_DIED 0x04            /* the process/thread owning the sem died */
#define QS_SYSSEM_EXCLUSIVE 0x08             /* indicates a exclusive system semaphore */
#define QS_SYSSEM_NAME_CLEANUP 0x10          /* name table entry needs to be removed */
#define QS_SYSSEM_THREAD_OWNER_DIED 0x20     /* the thread owning the sem died */
#define QS_SYSSEM_EXITLIST_OWNER 0x40        /* the exitlist thread owns the sem */

typedef struct _QSS16REC {   /* qsS16rec */
        ULONG         NextRec;        /* offset to next record in buffer */
                                      /* System Semaphore Table Structure */
        USHORT        SysSemOwner ;   /* thread owning this semaphore */
        UCHAR         SysSemFlag ;    /* system semaphore flag bit field */
        UCHAR         SysSemRefCnt ;  /* number of references to this sys sem */
        UCHAR         SysSemProcCnt ; /* number of requests for this owner */
        UCHAR         SysSemPad ;     /* pad byte to round structure up to word */
        USHORT        pad_sh;
        USHORT        SemPtr;         /* RMP SysSemPtr field */
        char          SemName;        /* start of semaphore name string */
} QSS16REC;

typedef struct _QSS16HEADREC {  /* qsS16Hrec */
        ULONG         SRecType;
        ULONG         SpNextRec;      /* overlays NextRec of 1st QSS16REC*/
        ULONG         S32SemRec;
        ULONG         S16TblOff;      /* offset of SysSemDataTable */
        ULONG         pSem16Rec;
} QSS16HEADREC;

/*
 *      System wide Shared Mem information
 *      ________________________________
 *      |       NextRec                 |
 *      |-------------------------------|
 *      |       hmem                    |
 *      |-------------------------------|
 *      |       sel                     |
 *      |-------------------------------|
 *      |       refcnt                  |
 *      |-------------------------------|
 *      |       name                    |
 *      |_______________________________|
 *
 */
typedef struct _QSMREC {  /* qsMrec */
        ULONG         MemNextRec;       /* offset to next record in buffer */
        USHORT        hmem;             /* handle for shared memory */
        USHORT        sel;              /* selector */
        USHORT        refcnt;           /* reference count */
        char          Memname;          /* start of shared memory name string */
} QSMREC;

/*
 *      32 bit system semaphore structure
 *      ________________________________
 *      |       pNextRec                |----|
 *      |-------------------------------|    |
 *      |       QSHUN[0]                |    |
 *      |-------------------------------|    |
 *      |         MuxQ                  |    |
 *      |-------------------------------|    |
 *      |         OpenQ                 |    |
 *      |-------------------------------|    |
 *      |         SemName               |    |
 *      |-------------------------------|<---|
 *      |          .                    |
 *      |          .                    |
 *      |-------------------------------|<---|
 *      |       pNextRec                |----|
 *      |-------------------------------|    |
 *      |       QSHUN[c32SSem-1]        |    |
 *      |-------------------------------|    |
 *      |         MuxQ                  |    |
 *      |-------------------------------|    |
 *      |         OpenQ                 |    |
 *      |-------------------------------|    |
 *      |         SemName               |    |
 *      |-------------------------------|<---|
 */

/*
 *  32- bit Semaphore flags
 */

#define QS_DC_SEM_SHARED   0x0001   //  Shared Mutex, Event or MUX semaphore
#define QS_DCMW_WAIT_ANY   0x0002   //  Wait on any event/mutex to occur
#define QS_DCMW_WAIT_ALL   0x0004   //  Wait on all events/mutexs to occur
#define QS_DCM_MUTEX_SEM   0x0008   //  Mutex semaphore
#define QS_DCE_EVENT_SEM   0x0010   //  Event semaphore
#define QS_DCMW_MUX_SEM    0x0020   //  Muxwait semaphore
#define QS_DC_SEM_PM       0x0040   //  PM Shared Event Semphore
#define QS_DE_POSTED       0x0040   //  event sem is in the posted state
#define QS_DM_OWNER_DIED   0x0080   //  The owning process died
#define QS_DMW_MTX_MUX     0x0100   //  MUX contains mutex sems
#define QS_DHO_SEM_OPEN    0x0200   //  Device drivers have opened this semaphore
#define QS_DE_16BIT_MW     0x0400   //  Part of a 16-bit MuxWait
#define QS_DCE_POSTONE     0x0800   //  Post one flag event semaphore
#define QS_DCE_AUTORESET   0x1000   //  Auto-reset event semaphore

typedef struct qsopenq_s {    /* qsopenq */
        PID           pidOpener;      /* process id of opening process */
        USHORT        OpenCt;         /* number of opens for this process */
} QSOPENQ;
typedef struct qsevent_s {    /* qsevent */
        QSOPENQ       *pOpenQ;        /* pointer to open q entries */
        UCHAR         *pName;         /* pointer to semaphore name */
        ULONG         *pMuxQ;         /* pointer to the mux queue */
        USHORT        flags;
        USHORT        PostCt;         /* # of posts */
} QSEVENT;
typedef struct qsmutex_s {    /* qsmutex */
        QSOPENQ       *pOpenQ;        /* pointer to open q entries */
        UCHAR         *pName;         /* pointer to semaphore name */
        ULONG         *pMuxQ;         /* pointer to the mux queue */
        USHORT        flags;
        USHORT        ReqCt;          /* # of requests */
        USHORT        SlotNum;        /* slot # of owning thread */
        PADSHORT;
} QSMUTEX;
typedef struct qsmux_s {   /* qsmux */
        QSOPENQ         *pOpenQ;        /* pointer to open q entries */
        UCHAR           *pName;         /* pointer to semaphore name */
        void            *pSemRec;       /* array of semaphore record entries */
        USHORT          flags;
        USHORT          cSemRec;        /* count of semaphore records */
        USHORT          WaitCt;         /* # threads waiting on the mux */
        PADSHORT;
} QSMUX;
typedef struct qsshun_s {  /* qsshun */
        QSEVENT         qsSEvt;         /* shared event sem */
        QSMUTEX         qsSMtx;         /* shared mutex sem */
        QSMUX           qsSMux;         /* shared mux sem */
} QSHUN;
typedef struct _QSS32REC {   /* qsS32rec */
        void            *pNextRec;      /* pointer to next record in buffer */
        QSHUN           qsh;            /* qstate version of SHUN record */
} QSS32REC;

/*
 *      System wide MTE information
 *      ________________________________
 *      |       pNextRec                |----|
 *      |-------------------------------|    |
 *      |       hmte                    |    |
 *      |-------------------------------|    |
 *      |       ctImpMod                |    |
 *      |-------------------------------|    |
 *      |       ctObj                   |    |
 *      |-------------------------------|    |
 *      |       pObjInfo                |----|----------|
 *      |-------------------------------|    |          |
 *      |       pName                   |----|----|     |
 *      |-------------------------------|    |    |     |
 *      |       imported module handles |    |    |     |
 *      |          .                    |    |    |     |
 *      |          .                    |    |    |     |
 *      |          .                    |    |    |     |
 *      |-------------------------------| <--|----|     |
 *      |       "pathname"              |    |          |
 *      |-------------------------------| <--|----------|
 *      |       Object records          |    |
 *      |       (if requested)          |    |
 *      |_______________________________|    |
 *                                      <-----
 *      NOTE that if the level bit is set to QS_MTE, the base Lib record will be followed
 *      by a series of object records (qsLObj_t); one for each object of the
 *      module.
 */

typedef struct _QSLOBJREC {  /* qsLOrec */
        ULONG         oaddr;  /* object address */
        ULONG         osize;  /* object size */
        ULONG         oflags; /* object flags */
} QSLOBJREC;

typedef struct _QSLREC {     /* qsLrec */
        void  FAR        *pNextRec;      /* pointer to next record in buffer */
        USHORT           hmte;           /* handle for this mte */
        USHORT           fFlat;          /* true if 32 bit module */
        ULONG            ctImpMod;       /* # of imported modules in table */
        ULONG            ctObj;          /* # of objects in module (mte_objcnt)*/
        QSLOBJREC FAR  *pObjInfo;      /* pointer to per object info if any */
        UCHAR     FAR    *pName;         /* -> name string following struc */
#if 0
        USHORT            ahmte[1];      /* Array of imported module hmtes */
#endif
} QSLREC;

/* Used for 9th bit (Extended Module Data Summary)*/
typedef struct _QSEXLREC {   /* qsELrec */
        struct          _QSEXLREC *next; /*  Pointer to next Extended Module Data */
        USHORT          hndmod;           /*  Module Handle */
        USHORT          pid;              /*  Process ID */
        USHORT          type;             /*  Type of Module */
        ULONG           refcnt;           /*  Size of reference array */
        ULONG           segcnt;           /*  Number of segments in module */
        void            *_reserved_;
        UCHAR FAR       *name;            /*  Pointer to Module Name  */
        ULONG           ModuleVersion;    /*  Module version value  */
        UCHAR FAR       *ShortModName;    /*  New Pointer to Module short name */
        ULONG           modref;           /*  Start of array of handles of module */
} QSEXLREC;

/*
 *      System wide FILE information
 *      ________________________________
 *      |       RecType                 |
 *      |-------------------------------|
 *      |       pNextRec                |-------|
 *      |-------------------------------|       |
 *      |       ctSft                   |       |
 *      |-------------------------------|       |
 *      |       pSft                    |---|   |
 *      |-------------------------------|   |   |
 *      |       name                    |   |   |
 *      |-------------------------------|<--|   |
 *      |       qsSft[0]                |       |
 *      |-------------------------------|       |
 *      |       ...                     |       |
 *      |-------------------------------|       |
 *      |       qsSft[ctSft -1]         |       |
 *      |_______________________________|       |
 *      |       name                    |
 *      |_______________________________|
 *                                      <-------|
 */
typedef struct _QSSFT {   /* qsSft - the size of this should be 0x16! */
        USHORT        sfn;            /* 0 SFN sf_fsi.sfi_selfSFN */
        USHORT        refcnt;         /* 2 sf_ref_count */
        USHORT        flags;          /* 4 sf_flags */
        USHORT        flags2;         /* 6 sf_flags2 */
        USHORT        mode;           /* 8 sf_fsi.sfi_mode - mode of access */
        USHORT        mode2;          /* 10 sf_fsi.sfi_mode2 - mode of access */
        ULONG         size;           /* 12 sf_fsi.sfi_size */
        USHORT        hVPB;           /* 16 sf_fsi.sfi_hVPB handle of volume */
        USHORT        attr;           /* 18 sf_attr */
        PADSHORT;                     /* 20 */
} QSSFT;

typedef struct qsFrec_s {  /* qsFrec */
        ULONG         RecType;        /* Record Type */
        void          *pNextRec;      /* pointer to next record in buffer */
        ULONG         ctSft;          /* # sft entries for this MFT entry */
        QSSFT       *pSft;          /* -> start of sft entries in buf */
#if 0
        char          name[1];        /* kso: start of name? */
#endif
} QSFREC;


/* Pointer Record Structure
 *      This structure is the first in the user buffer.
 *      It contains pointers to heads of record types that are loaded
 *      into the buffer.
 */

typedef struct _QSPTRREC {   /* qsPRec */
        QSGREC        *pGlobalRec;
        QSPREC        *pProcRec;        /* ptr to head of process records */
        QSS16HEADREC  *p16SemRec;       /* ptr to head of 16 bit sem recds */
        QSS32REC      *p32SemRec;       /* ptr to head of 32 bit sem recds */
        QSMREC        *pMemRec;         /* ptr to head of shared mem recs */
        QSLREC        *pLibRec;         /* ptr to head of mte records */
        QSMREC        *pShrMemRec;      /* ptr to head of shared mem records */
        QSFREC        *pFSRec;          /* ptr to head of file sys records */
} QSPTRREC;

#pragma pack()
#endif

/* IMPORTANT, the sizeof QSSFT is 0x16 bytes! */
#define SIZEOFQSSFT_T   0x16




/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
#ifndef QS_MTE
   /* from OS/2 Toolkit v4.5 */

   APIRET APIENTRY DosQuerySysState(ULONG EntityList, ULONG EntityLevel, PID pid,
                                    TID tid, PVOID pDataBuf, ULONG cbBuf);
   #define QS_MTE         0x0004
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
BOOL    kdtLoadModule(const char *pszModule, PMODINFO pModInfo);



BOOL    kdtLoadModule(const char *pszModule, PMODINFO pModInfo)
{
    static char     achBuffer[1024*256];
    char            szError[256];
    HMODULE         hmod = NULLHANDLE;
    int             rc;
    char            szName[CCHMAXPATH];
    char *          pszSrcName;
    const char *    pszName;
    const char *    psz2;
    char *          pszTmp;
    ULONG           ulAction;
    HFILE           hFile;
    struct e32_exe* pe32 = (struct e32_exe*)(void*)&achBuffer[0];
    QSPTRREC *      pPtrRec = (QSPTRREC*)(void*)&achBuffer[0];
    QSLREC *      pLrec;
    int             i;
    FILESTATUS3     fsts3;

    memset(pModInfo, 0, sizeof(*pModInfo));

    /*
     * Make a temporary copy of the module in the temp directory.
     */
    if (DosScanEnv("TMP", &pszTmp) != NO_ERROR || pszTmp == NULL)
    {
        printf("Environment variable TMP is not set.\n");
        return FALSE;
    }
    strcpy(szName, pszTmp);
    if (szName[strlen(pszTmp) - 1] != '\\' && szName[strlen(pszTmp) - 1] != '/')
        strcat(szName, "\\");

    pszName = strrchr(pszModule, '\\');
    if (!pszName || (psz2 = strrchr(pszModule, '/')) > pszName)
        pszName = psz2;
    if (!pszName || (psz2 = strrchr(pszModule, ':')) > pszName)
        pszName = psz2;
    if (!pszName)
        pszName = pszModule;
    strcat(szName, pszName);
    rc = DosCopy((PSZ)pszModule, szName, DCPY_EXISTING);
    if (rc != NO_ERROR)
    {
        printf("Failed to copy %s to %s.\n", pszSrcName, szName);
        return FALSE;
    }
    if (DosQueryPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3)) != NO_ERROR
        ||  !(fsts3.attrFile = FILE_ARCHIVED)
        ||  DosSetPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3), 0) != NO_ERROR
        )
    {
        printf("Failed to set attributes for %s.\n", szName);
        return FALSE;
    }

    /*
     * Patch the module.
     *      Remove the entrypoint and mark it as DLL.
     */
    ulAction = 0;
    rc = DosOpen(szName, &hFile, &ulAction, 0, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
                 NULL);
    if (rc != NO_ERROR)
    {
        printf("Failed to open temporary module file %s. rc = %d\n", &szName[0], rc);
        return FALSE;
    }
    rc = DosRead(hFile, &achBuffer[0], 0x200, &ulAction);
    if (rc != NO_ERROR)
    {
        DosClose(hFile);
        printf("Failed to read LX header from temporary module file.\n");
        return FALSE;
    }
    pe32 = (struct e32_exe*)(void*)&achBuffer[*(unsigned long*)(void*)&achBuffer[0x3c]];
    if (*(PUSHORT)pe32->e32_magic != E32MAGIC)
    {
        DosClose(hFile);
        printf("Failed to read LX header from temporary module file (2).\n");
        return FALSE;
    }
    pe32->e32_eip = 0;
    pe32->e32_startobj = 0;
    pe32->e32_mflags &= ~(E32LIBTERM | E32LIBINIT | E32MODMASK);
    pe32->e32_mflags |= E32MODDLL;
    if ((rc = DosSetFilePtr(hFile, *(unsigned long*)(void*)&achBuffer[0x3c], FILE_BEGIN, &ulAction)) != NO_ERROR
        || (rc = DosWrite(hFile, pe32, sizeof(struct e32_exe), &ulAction)) != NO_ERROR)
    {
        DosClose(hFile);
        printf("Failed to write patched LX header to temporary module file. rc=%d\n", rc);
        return FALSE;
    }
    DosClose(hFile);

    /*
     * Load the module.
     */
    rc = DosLoadModule(szError, sizeof(szError), szName, &hmod);
    if (rc != NO_ERROR && (rc != ERROR_INVALID_PARAMETER && hmod == NULLHANDLE))
    {
        printf("Failed to load %s image %s.", szName, szError);
        return FALSE;
    }

    /*
     * Get object information.
     */
    rc = DosQuerySysState(QS_MTE, QS_MTE, 0L, 0L, pPtrRec, sizeof(achBuffer));
    if (rc != NO_ERROR)
    {
        printf("DosQuerySysState failed with rc=%d.\n", rc);
        return FALSE;
    }

    pLrec = pPtrRec->pLibRec;
    while (pLrec != NULL)
    {
        /*
         * Bug detected in OS/2 FP13. Probably a problem which occurs
         * in _LDRSysMteInfo when qsCheckCache is calle before writing
         * object info. The result is that the cache flushed and the
         * attempt of updating the QSLREC next and object pointer is
         * not done. This used to work earlier and on Aurora AFAIK.
         *
         * The fix for this problem is to check if the pObjInfo is NULL
         * while the number of objects isn't 0 and correct this. pNextRec
         * will also be NULL at this time. This will be have to corrected
         * before we exit the loop or moves to the next record.
         * There is also a nasty alignment of the object info... Hope
         * I got it right. (This aligment seems new to FP13.)
         */
        if (pLrec->pObjInfo == NULL /*&& pLrec->pNextRec == NULL*/ && pLrec->ctObj > 0)
            {
            pLrec->pObjInfo = (QSLOBJREC*)(void*)(
                (char*)(void*)pLrec
                + ((sizeof(QSLREC)                              /* size of the lib record */
                   + pLrec->ctImpMod * sizeof(short)            /* size of the array of imported modules */
                   + strlen((char*)(void*)pLrec->pName) + 1     /* size of the filename */
                   + 3) & ~3));                                 /* the size is align on 4 bytes boundrary */
            pLrec->pNextRec = (QSLREC*)(void*)((char*)(void*)pLrec->pObjInfo
                                                 + sizeof(QSLOBJREC) * pLrec->ctObj);
            }
        if (pLrec->hmte == hmod)
            break;

        /*
         * Next record
         */
        pLrec = (QSLREC*)pLrec->pNextRec;
    }

    if (pLrec == NULL)
    {
        printf("DosQuerySysState(%s): not found\n", pszName);
        return FALSE;
    }
    if (pLrec->pObjInfo == NULL)
    {
        printf("DosQuerySysState(%s): no object info\n", pszName);
        return FALSE;
    }


    /*
     * Fill the module info stuff.
     */
    for (i = 0; i < pLrec->ctObj; i++)
    {
        pModInfo->aObjs[i].ote_size    = pLrec->pObjInfo[i].osize;
        pModInfo->aObjs[i].ote_base    = pLrec->pObjInfo[i].oaddr;
        pModInfo->aObjs[i].ote_flags   = pLrec->pObjInfo[i].oflags;
        pModInfo->aObjs[i].ote_pagemap = i > 0 ? pModInfo->aObjs[i-1].ote_pagemap + pModInfo->aObjs[i-1].ote_mapsize : 0;
        pModInfo->aObjs[i].ote_mapsize = (pLrec->pObjInfo[i].osize + 0x0FFF) / 0x1000;
        pModInfo->aObjs[i].ote_sel     = ((ULONG)FlatToSel(pLrec->pObjInfo[i].oaddr)) >> 16;
        pModInfo->aObjs[i].ote_hob     = 0;
    }
    pModInfo->cObjs = pLrec->ctObj;
    pModInfo->hmod = hmod;
    strcpy(pModInfo->szName, pszName);


    return TRUE;

}


/**
 * Loads a device driver into memory using DosLoadModule.
 *
 * @returns Pointer to module info structure which the caller
 *          is responsible for cleaning up!
 * @returns NULL on failure, error message is written to screen.
 * @param   pszModule   Name of the module to load.
 * @remark  32-bit stack.
 */
PMODINFO    kdtLoadDriver(const char *pszModule)
{
    PMODINFO pModInfo = malloc(sizeof(MODINFO));
    if (pModInfo)
    {
        if (kdtLoadModule(pszModule, pModInfo))
        {
            return pModInfo;
        }
        free(pModInfo);
    }

    return NULL;
}



/**
 * Validates that the module is a correctly linked driver module.
 *
 * @returns Success indicator.
 * @param   pModInfo    Pointer to module in question.
 */
BOOL    kdtLoadValidateDriver(PMODINFO pModInfo)
{
    if (pModInfo->cObjs <= 2)
    {
        printf("kDevTest: Invalid driver module - too few objects! (%d)\n", pModInfo->cObjs);
        return FALSE;
    }

    if (   !(pModInfo->aObjs[0].ote_flags & OBJWRITE)
        ||  (pModInfo->aObjs[0].ote_flags & OBJBIGDEF)
        )
    {
        printf("kDevTest: Invalid driver module - 1st object isn't 16-bit data!\n");
        return FALSE;
    }

    if (   !(pModInfo->aObjs[1].ote_flags & OBJEXEC)
        ||  (pModInfo->aObjs[1].ote_flags & OBJBIGDEF)
        )
    {
        printf("kDevTest: Invalid driver module - 2nd object isn't 16-bit code!\n");
        return FALSE;
    }

    return TRUE;
}


