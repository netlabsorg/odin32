/* $Id: kLog.h,v 1.4 2001-10-19 01:48:36 bird Exp $
 *
 * kLog - Generic Logging and Trace Routines.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 */

#ifndef _kLog_h_
#define _kLog_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * Standard log types.
 */
#define KLOG_TYPE_LOGINIT    0          /* Log Initiated. */
#define KLOG_TYPE_INITMOD    1          /* Log Module Init. */
#define KLOG_TYPE_TERMMOD    2          /* Log Module Termination. */
#define KLOG_TYPE_START      3          /* Start sequence and function entry. */
#define KLOG_TYPE_ENTRY      4          /* Function entry. */
#define KLOG_TYPE_EXIT       5          /* Function exit. */
#define KLOG_TYPE_STOP       6          /* Stop sequence and function exit.  */
#define KLOG_TYPE_PRINTF     7          /* kprintf and printf log output. */
#define KLOG_TYPE_ASSERT     8          /* Assertion failed / Kernel Panic. */
#define KLOG_TYPE_LOGASSERT  9          /* Assertion failed within kLog. */

#define KLOG_TYPE_LAST_REQ   KLOG_TYPE_ASSERT   /* Last required type. */

#define KLOG_TYPE_USER       0x1000     /* First user log type.  */


/*
 * Flags.
 */
#define KLOG_TYPEDEF_PTR        0x8000  /* flags this typedef as a pointer. */
#define KLOG_TYPEDEF_SIZE       0x0fff  /* Mask to get the size. */

#define KLOG_FLAGS_DISABLED     0x0000
#define KLOG_FLAGS_ENABLED      0x0001
#ifdef RING0
#define KLOG_FLAGS_COMOUTPUT    0x0010
#else
#define KLOG_FLAGS_COMOUTPUT    0x0000
#endif
#define KLOG_FLAGS_MASK         0x0011
#define KLOG_FLAGS_AS_ABOVE     0xffff  /* Inherit options from above option level. */

/*
 * Define default file and function names.
 */
#ifndef KLOGFILE
#   define KLOGFILE         __FILE__
#endif
#ifndef KLOGFUNCTION
#   define KLOGFUNCTION     __FUNCTION__
#endif
#define KLOGPOS_EXT         KLOGFILE, __LINE__, KLOGFUNCTION    /* external call */
#define KLOGPOS_INT         pszFile, iLine, pszFunction         /* Internal call */
#define KLOGPOS_DECL        const char *pszFile, int iLine, const char *pszFunction


/*
 * RAS Data.
 */
#define KLOG_MAJOR          253
#define KLOG_MINOR          42


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef unsigned long int  HKLOGMOD;    /* Loghandle for a module. */

#ifdef _kAVL_h_

/**
 * Describes one struct member.
 */
typedef struct kLogMemberInfo
{
    const char *    pszName;
    const char *    pszFormat;
    const short     cb;
    const short     offset;
} KLOGMEMBERINFO, *PKLOGMEMBERINFO;


/**
 * Describes one user typedef.
 */
typedef struct kLogTypedefInfo
{
    union
    {
        const char *        pszType;
        AVLSTRNODECORE      avlnodecore;
    } u1;
    const char *            pszFormat;
    const unsigned short    cb;
    const unsigned short    cMembers;
    const PKLOGMEMBERINFO
                    paMembers;
} KLOGTYPEDEFINFO, *PKLOGTYPEDEFINFO;


/**
 * Describes one user log/trace type.
 */
typedef struct kLogTypeInfo
{
    const char *            pszType;
    const char *            pszFormat;
    unsigned short          fCurrent;
    const unsigned short    fDefault;
} KLOGTYPEINFO, *PKLOGTYPEINFO;


/**
 * Holds stateinfo on one default log/trace type.
 */
typedef struct kLogDefTypeInfo
{
    unsigned short          fCurrent;
    const unsigned short    fDefault;
} KLOGDEFTYPEINFO, *PKLOGDEFTYPEINFO;


/**
 * Describes one file.
 */
typedef struct kLogFileInfo
{
    union
    {
        const char *        pszFile;
        AVLSTRNODECORE      avlnodecore;
    } u1;
    unsigned short          fCurrent;
    const unsigned short    fDefault;
} KLOGFILEINFO, *PKLOGFILEINFO;



/**
 * Log Module Data.
 */
typedef struct kLogModuleData
{
    const char *    pszModuleName;      /* Module name. */
    const char *    pszModuleDescription;/*Module description. */

    int                     cTypedefs;  /* Number of typesdef in paTypedefs. */
    PKLOGTYPEDEFINFO        paTypedefs; /* Array of typedef descriptions. */

    int                     cUserTypes; /* Number of user types in paUserTypes. */
    PKLOGTYPEINFO           paUserTypes;/* Array of user type descriptions. */

    int                     cDefTypes;  /* Number of default types in paDefTypes. */
    PKLOGDEFTYPEINFO        paDefTypes; /* Array of default type descriptions. */

    int                     cFiles;     /* Number of files in paFiles. */
    PKLOGFILEINFO           paFiles;    /* Array of per file settings. */

    unsigned short          fCurrent;
    const unsigned short    fDefault;

    /* Module Data Used by Log System - Do NOT touch! */
    AVLULNODECORE           avlnodecore;/* AVL Tree node with module handle as key. */
    PAVLSTRNODECORE         pavlTypedefs;/*Tree of typedef into entries. */
    PAVLSTRNODECORE         pavlFiles;  /* Tree of file into entries. */
    unsigned long           ulLastSeq;  /* Last sequence number. */

} KLOGMODDATA, *PKLOGMODDATA;
#endif /* #ifdef _kAVL_h_ */


/**
 * The sequence handle on the stack.
 */
typedef union kLogSeqHandle
{
    char    achMagic[16];
    struct kLogSeqHandleData
    {
        unsigned long   ulMagic;
        unsigned long   ulSeq;
    } data;
} HKLOGSEQ, *PHKLOGSEQ;

#pragma pack()

/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
KBOOL       KLIBCALL kLogInit(unsigned long cbBufferSize);
KBOOL       KLIBCALL kLogInitBuffer(unsigned long cbBufferSize);

#ifdef _kAVL_h_
HKLOGMOD    KLIBCALL kLogInitMod(PKLOGMODDATA pMod, KLOGPOS_DECL);
#endif
void        KLIBCALL kLogEntry(HKLOGMOD hLogMod, KLOGPOS_DECL, const char *pszFuncProto, KBOOL fOptlink, ...);
void        KLIBCALL kLogExit(HKLOGMOD hLogMod, KLOGPOS_DECL, const char *pszReturnType, unsigned uReturn);
void        KLIBCALL kLog(HKLOGMOD hLogMod, KLOGPOS_DECL, int iType, ...);
#if defined(va_arg) || defined(va_list) || defined(va_start) || defined(va_end)
void        KLIBCALL kLogv(HKLOGMOD hLogMod, KLOGPOS_DECL, int iType, va_list args);
va_list     KLIBCALL kLogFixPrintf(const char *pszFormat, ...); /* a hack! */
#endif

#define KLOGINITMOD(pMod)                   kLogInitMod(pMod, KLOGPOS_EXT)
#define KLOGENTRY(hLogMod, pszProto, parg0) kLogEntry(hLogMod, KLOGPOS_EXT, pszFuncProto, FALSE, parg0)
#define KLOGEXIT(hLogMod, pszType, uValue)  kLogExit(hLogMod, KLOGPOS_EXT, pszType, uValue)

#ifdef __cplusplus
}
#endif

#endif

