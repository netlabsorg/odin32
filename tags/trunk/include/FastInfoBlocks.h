/* $Id: FastInfoBlocks.h,v 1.1 2003-02-28 10:28:09 sandervl Exp $
 *
 * Fast InfoBlock Access.
 *
 * Copyright (c) 2003 knut st. osmundsen <bird@anduin.net>
 *
 * Project Odin Software License can be found in LICENSE.TXT.
 *
 */

#ifndef __FastInfoBlocks_h__
#define __FastInfoBlocks_h__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/**
 * Gets the current Process ID.
 */
#if 0
#define fibGetPid()             (_gpfibPIB->pib_ulpid)
#else
#define fibGetPid()             (_gpfibLIS->pidCurrent)
#endif

/**
 * Gets the current Thread ID.
 */
#define fibGetTid()             (_gpfibLIS->tidCurrent)

/**
 * Gets the handle of the executable of this process.
 */
#if 1
#define fibGetExeHandle()       (_gpfibPIB->pib_hmte)
#else
#define fibGetExeHandle()       (_gpfibLIS->hmod)
#endif

/**
 * Gets the Environment Selector.
 */
#define fibGetEnvSel()          (_gpfibLIS->selEnv)

/**
 * Gets the Environment Selector.
 */
#define fibGetCmdLineOff()      (_gpfibLIS->offCmdLine)

/**
 * Gets the Environment Pointer (32bit FLAT pointer).
 */
#if 1
#define fibGetEnv()             (_gpfibPIB->pib_pchenv)
#else
#define fibGetEnv()             ( ((fibGetEnvSel() & ~7) << 13) )
#endif

/**
 * Gets the Commandline Pointer (32bit FLAT pointer).
 */
#if 1
#define fibGetCmdLine()         (_gpfibPIB->pib_pchcmd)
#else
#define fibGetCmdLine()         ( ((fibGetEnvSel() & ~7) << 13) | fibGetCmdLineOff() )
#endif


/**
 * Gets the Process Status.
 */
#define fibGetProcessStatus()   (_gpfibLIS->rfProcStatus)
/** Is this process in an exit list? */
#define fibIsInExitList()       (fibGetProcessStatus() & 1)


/**
 * Gets the Process Type.
 */
#define fibGetProcessType()     (_gpfibLIS->typeProcess)

/** Is this a Detached (daemon) process? */
#define fibIsDetachedProcess()  (fibGetProcessType() == 4)
/** Is this a PM process? */
#define fibIsPMProcess()        (fibGetProcessType() == 3)
/** Is this a VIO (windowed console) process? */
#define fibIsVIOProcess()       (fibGetProcessType() == 2)
/** Is this a VDM (Virtual Dos Mode) process? */
#define fibIsVDMProcess()       (fibGetProcessType() == 1)
/** Is this a full screen process? */
#define fibIsFullScreeProcess() (fibGetProcessType() == 0)





/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#pragma pack(1)

/**
 * Local Info Segment (per process).
 */
extern struct fibLIS
{
    unsigned short  pidCurrent;  /* Current process ID */
    unsigned short  pidParent;  /* Process ID of parent */
    unsigned short  prtyCurrent; /* Current thread priority */
    unsigned short  tidCurrent;  /* Current thread ID */
    unsigned short  sgCurrent; /* Screengroup */
    unsigned char   rfProcStatus; /* Process status bits */
    unsigned char   LIS_fillbyte1;  /* filler byte */
    unsigned short  fFoureground;       /* Current process is in foreground */
    unsigned char   typeProcess;   /* Current process type */
    unsigned char   LIS_fillbyte2;  /* filler byte */

    unsigned short  selEnv;     /* @@V1 Environment selector */
    unsigned short  offCmdLine;    /* @@V1 Offset of command line start */
    unsigned short  cbDataSegment;/* @@V1 Length of Data Segment */
    unsigned short  cbStack;  /* @@V1 STACKSIZE from the .EXE file */
    unsigned short  cbHeap;   /* @@V1 HEAPSIZE  from the .EXE file */
    unsigned short  hmod;    /* @@V1 Module handle of the application */
    unsigned short  selDS;   /* @@V1 Data Segment Handle of application */

    unsigned short  LIS_PackSel;    /* First tiled selector in this EXE */
    unsigned short  LIS_PackShrSel; /* First selector above shared arena */
    unsigned short  LIS_PackPckSel; /* First selector above packed arena */
}  * _gpfibLIS;

/**
 * Global Info Segment (system)
 */
extern struct fibGIS
{
    /* Time (offset 0x00) */
    unsigned long   SIS_BigTime;    /* Time from 1-1-1970 in seconds */
    unsigned long   SIS_MsCount;    /* Freerunning milliseconds counter */
    unsigned char   SIS_HrsTime;    /* Hours */
    unsigned char   SIS_MinTime;    /* Minutes */
    unsigned char   SIS_SecTime;    /* Seconds */
    unsigned char   SIS_HunTime;    /* Hundredths of seconds */
    unsigned short  SIS_TimeZone;   /* Timezone in min from GMT (Set to EST) */
    unsigned short  SIS_ClkIntrvl;  /* Timer interval (units=0.0001 secs) */

    /* Date (offset 0x10) */
    unsigned char   SIS_DayDate;    /* Day-of-month (1-31) */
    unsigned char   SIS_MonDate;    /* Month (1-12) */
    unsigned short  SIS_YrsDate;    /* Year (>= 1980) */
    unsigned char   SIS_DOWDate;    /* Day-of-week (1-1-80 = Tues = 3) */

    /* Version (offset 0x15) */
    unsigned char   SIS_VerMajor;   /* Major version number */
    unsigned char   SIS_VerMinor;   /* Minor version number */
    unsigned char   SIS_RevLettr;   /* Revision letter */

    /* System Status (offset 0x18) */
    unsigned char   SIS_CurScrnGrp; /* Fgnd screen group # */
    unsigned char   SIS_MaxScrnGrp; /* Maximum number of screen groups */
    unsigned char   SIS_HugeShfCnt; /* Shift count for huge segments */
    unsigned char   SIS_ProtMdOnly; /* Protect-mode-only indicator */
    unsigned short  SIS_FgndPID;    /* Foreground process ID */

    /* Scheduler Parms (offset 0x1E) */
    unsigned char   SIS_Dynamic;    /* Dynamic variation flag (1=enabled) */
    unsigned char   SIS_MaxWait;    /* Maxwait (seconds) */
    unsigned short  SIS_MinSlice;   /* Minimum timeslice (milliseconds) */
    unsigned short  SIS_MaxSlice;   /* Maximum timeslice (milliseconds) */

    /* Boot Drive (offset 0x24) */
    unsigned short  SIS_BootDrv;    /* Drive from which system was booted */

    /* RAS Major Event Code Table (offset 0x26) */
    unsigned char   SIS_mec_table[32]; /* Table of RAS Major Event Codes (MECs) */

    /* Additional Session Data (offset 0x46) */
    unsigned char   SIS_MaxVioWinSG;  /* Max. no. of VIO windowable SG's */
    unsigned char   SIS_MaxPresMgrSG; /* Max. no. of Presentation Manager SG's */

    /* Error logging Information (offset 0x48) */
    unsigned short  SIS_SysLog;     /* Error Logging Status */

    /* Additional RAS Information (offset 0x4A) */
    unsigned short  SIS_MMIOBase;   /* Memory mapped I/O selector */
    unsigned long   SIS_MMIOAddr;   /* Memory mapped I/O address  */

    /* Additional 2.0 Data (offset 0x50) */
    unsigned char   SIS_MaxVDMs;      /* Max. no. of Virtual DOS machines */
    unsigned char   SIS_Reserved;
}  * _gpfibGIS;


/**
 * Process Info Block.
 */
extern struct ProcessInformationBlock
{
    unsigned long   pib_ulpid;          /* Process I.D. */
    unsigned long   pib_ulppid;         /* Parent process I.D. */
    unsigned long   pib_hmte;           /* Program (.EXE) module handle */
    char *          pib_pchcmd;         /* Command line pointer */
    char *          pib_pchenv;         /* Environment pointer */
    unsigned long   pib_flstatus;       /* Process' status bits */
    unsigned long   pib_ultype;         /* Process' type code */
}   * _gpfibPIB;

#pragma pack()


/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
int     fibInit(void);
void    fibDumpAll(void);


#ifdef __cplusplus
}
#endif
#endif
