/* $Id: kLog.h,v 1.1 2001-10-14 22:52:16 bird Exp $
 *
 * kLog - Generic Logging and Trace Routines.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 */

#ifndef _kLog_h_
#define _kLog_h_


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * Standard log types.
 */
#define KLOG_INITMOD    1               /* Function entry */
#define KLOG_TERMMOD    2               /* Function entry */
#define KLOG_ENTRY      3               /* Function entry */
#define KLOG_EXIT       4               /* Function exit  */
#define KLOG_ASSERT     5               /* Assertion failed / Kernel Panic. */

#define KLOG_USER       0x1000          /* First user log type.  */


/*
 * Flags.
 */
#define KLOG_TYPEDEF_PTR    0x8000      /* flags this typedef as a pointer. */
#define KLOG_TYPEDEF_SIZE   0xfff       /* Mask to get the size. */

#define KLOG_TYPE_DISABLED  0x00
#define KLOG_TYPE_ENABLED   0x01
#define KLOG_TYPE_COMOUTPUT 0x10
#define KLOG_TYPE_MASK      0x11

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


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(1)
typedef unsigned long int  HKLOGMOD;    /* Loghandle for a module. */


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
    const char *            pszType;
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
 * Log Module Data.
 */
typedef struct kLogModuleData
{
    const char *    pszModuleName;      /* Module name. */
    const char *    pszModuleDescription; /* Module description. */

    int             cTypedefs;          /* Number of typesdef in paTypedefs. */
    PKLOGTYPEDEFINFO paTypedefs;        /* Array of typedef descriptions. */

    int             cUserTypes;         /* Number of user types in paUserTypes. */
    PKLOGTYPEINFO   paUserTypes;        /* Array of user type descriptions. */

    int             cDefTypes;          /* Number of default types in paDefTypes. */
    PKLOGDEFTYPEINFO paDefTypes;        /* Array of default type descriptions. */

    /* Module Data Used by Log System - Do NOT touch! */
    AVLULNODECORE   avlnodecore;        /* AVL Tree node with module handle as key. */
    unsigned long   ulLastSeq;           /* Last sequence number. */

} KLOGMODDATA, *PKLOGMODDATA;


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

HKLOGMOD    KLIBCALL kLogInitMod(PKLOGMODDATA pModData, KLOGPOS_DECL);
#define KLOGINITMOD(pModData)              kLogInitMod(pModData, KLOGPOS_EXT)

void        KLIBCALL kLog(HKLOGMOD hLogMod, int iType, KLOGPOS_DECL, ...);


#endif

