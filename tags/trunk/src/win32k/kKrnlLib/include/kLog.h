/* $Id: kLog.h,v 1.6 2001-10-24 03:57:39 bird Exp $
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
 * Define defaults.
 */
#ifndef KLOGFILEDEF
    #define KLOGFILEDEF     __FILE__
#endif
#ifndef KLOGFUNCTIONDEF
    #define KLOGFUNCTIONDEF __FUNCTION__
#endif

#ifndef KLOGFILE
    #ifndef NOFILEID
        #define KLOGFILE    szFileId
    #else
        #define KLOGFILE    KLOGFUNCTIONDEF
    #endif
#endif
#ifndef KLOGFUNCTION
    #define KLOGFUNCTION    szkLogFunctionName
#endif
#ifndef KLOGRETURNTYPE
    #define KLOGRETURNTYPE  szkLogReturnType
#endif
#ifndef KLOGMODHANDLE
    #define KLOGMODHANDLE   hkLogMod
#endif
#ifndef KLOGSEQHANDLE
    #define KLOGSEQHANDLE   kLogSeqHandle
#endif

/*
 * Position parameters.
 */
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
void        KLIBCALL kLogTermMod(HKLOGMOD hLogMod, KLOGPOS_DECL);

int         KLIBCALL kLogStart(HKLOGMOD hLogMod, KLOGPOS_DECL, PHKLOGSEQ phSeq, const char *pszFuncProto, KBOOL fOptlink, ...);
int         KLIBCALL kLogEntry(HKLOGMOD hLogMod, KLOGPOS_DECL, const char *pszFuncProto, KBOOL fOptlink, ...);
void        KLIBCALL kLogExit(HKLOGMOD hLogMod, KLOGPOS_DECL, const char *pszReturnType, ...);
void        KLIBCALL kLogStop(HKLOGMOD hLogMod, KLOGPOS_DECL, PHKLOGSEQ phSeq, const char *pszReturnType, ...);

void        KLIBCALL kLog(HKLOGMOD hLogMod, KLOGPOS_DECL, int iType, ...);
#if defined(va_arg) || defined(va_list) || defined(va_start) || defined(va_end)
void        KLIBCALL kLogv(HKLOGMOD hLogMod, KLOGPOS_DECL, int iType, va_list args);
#endif

int         KLIBCALL kLogFixPrintf1(const char *pszFormat, ...);
void        KLIBCALL kLogFixPrintf2(HKLOGMOD hLogMod, KLOGPOS_DECL, int iType, int iBuffer);



/*******************************************************************************
*   Macros                                                                     *
*******************************************************************************/
#if defined(DEBUG) || defined(LOGGING_ENABLED)
    #define KLOGINITMOD(pMod)                                           KLOGMODHANDLE = kLogInitMod(pMod, KLOGPOS_EXT)
    #define KLOGTERMMOD()                                               kLogTermMod(KLOGMODHANDLE, KLOGPOS_EXT)

    #define KLOGSTARTPARMS                                              KLOGMODHANDLE, KLOGPOS_EXT, &KLOGSEQHANDLE
    #define KLOGENTRYPARMS                                              KLOGMODHANDLE, KLOGPOS_EXT
    #define KLOGSTARTVARS(pszRetType)                                   static const char   KLOGFUNCTION[] = KLOGFUNCTIONDEF; \
                                                                        static const char   KLOGRETURNTYPE[] = pszRetType; \
                                                                        HLOGSEQ             KLOGSEQHANDLE; \
                                                                        int                 kLogDummyVariable
    #define KLOGENTRYVARS(pszRetType)                                   static const char   KLOGFUNCTION[] = KLOGFUNCTIONDEF; \
                                                                        static const char   KLOGRETURNTYPE[] = pszRetType; \
                                                                        int                 kLogDummyVariable

    #define KLOGSTART( pszRetType, pszProto, arg0)                      KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, FALSE, &(arg0))
    #define KLOGSTART0(pszRetType)                                      KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, NULL, TRUE)
    #define KLOGSTART1(pszRetType, pszProto, a0)                        KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0)
    #define KLOGSTART2(pszRetType, pszProto, a0,a1)                     KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1)
    #define KLOGSTART3(pszRetType, pszProto, a0,a1,a2)                  KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1,a2)
    #define KLOGSTART4(pszRetType, pszProto, a0,a1,a2,a3)               KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1,a2,a3)
    #define KLOGSTART5(pszRetType, pszProto, a0,a1,a2,a3,a4)            KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1,a2,a2,a4)
    #define KLOGSTART6(pszRetType, pszProto, a0,a1,a2,a3,a4,a5)         KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1,a2,a2,a4,a5)
    #define KLOGSTART7(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6)      KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1,a2,a2,a4,a5,a6)
    #define KLOGSTART8(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6,a7)   KLOGSTARTVARS(pszRetType) = kLogStart(KLOGSTARTPARMS, pszProto, TRUE,a0,a1,a2,a2,a4,a5,a6,a7)

    #define KLOGENTRY( pszRetType, pszProto, arg0)                      KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, FALSE, &(arg0))
    #define KLOGENTRY0(pszRetType)                                      KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, NULL, TRUE)
    #define KLOGENTRY1(pszRetType, pszProto, a0)                        KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0)
    #define KLOGENTRY2(pszRetType, pszProto, a0,a1)                     KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1)
    #define KLOGENTRY3(pszRetType, pszProto, a0,a1,a2)                  KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1,a2)
    #define KLOGENTRY4(pszRetType, pszProto, a0,a1,a2,a3)               KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1,a2,a3)
    #define KLOGENTRY5(pszRetType, pszProto, a0,a1,a2,a3,a4)            KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1,a2,a2,a4)
    #define KLOGENTRY6(pszRetType, pszProto, a0,a1,a2,a3,a4,a5)         KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1,a2,a2,a4,a5)
    #define KLOGENTRY7(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6)      KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1,a2,a2,a4,a5,a6)
    #define KLOGENTRY8(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6,a7)   KLOGENTRYVARS(pszRetType) = kLogEntry(KLOGENTRYPARMS, pszProto, TRUE,a0,a1,a2,a2,a4,a5,a6,a7)

    #define KLOGEXIT(uValue)                                            kLogExit( KLOGMODHANDLE, KLOGPOS_EXT,                 KLOGRETURNTYPE, uValue);  kLogDummyVariable = kLogDummyVariable
    #define KLOGEXITVOID()                                              kLogExit( KLOGMODHANDLE, KLOGPOS_EXT,                 KLOGRETURNTYPE, NULL);    kLogDummyVariable = kLogDummyVariable

    #define KLOGSTOP(uValue)                                            kLogStop( KLOGMODHANDLE, KLOGPOS_EXT, &KLOGSEQHANDLE, KLOGRETURNTYPE, uValue);  kLogDummyVariable = kLogDummyVariable
    #define KLOGSTOPVOID(uValue)                                        kLogStop( KLOGMODHANDLE, KLOGPOS_EXT, &KLOGSEQHANDLE, KLOGRETURNTYPE, NULL);    kLogDummyVariable = kLogDummyVariable

    #ifdef KLOG_USE_RETURN
        #if KLOG_USE_RETURN
            #define return(value)                                       KLOGEXIT(value); return (value)
        #else
            #define return(value)                                       KLOGSTOP(value); return (value)
        #endif
    #endif

    #define dprintf(a)                                                  kLogFixPrintf2(KLOGMODHANDLE, KLOGPOS_EXT, KLOG_TYPE_PRINTF, kLogFixPrintf1 a)
    #define kprintf(a)                                                  dprintf(a)

#else

    #define KLOGINITMOD(pMod)                                           ((HKLOGMOD)-1)
    #define KLOGTERMMOD()                                               ((void)0)

    #define KLOGSTART( pszRetType, pszProto, arg0)                      ((void)0)
    #define KLOGSTART0(pszRetType)                                      ((void)0)
    #define KLOGSTART1(pszRetType, pszProto, a0)                        ((void)0)
    #define KLOGSTART2(pszRetType, pszProto, a0,a1)                     ((void)0)
    #define KLOGSTART3(pszRetType, pszProto, a0,a1,a2)                  ((void)0)
    #define KLOGSTART4(pszRetType, pszProto, a0,a1,a2,a3)               ((void)0)
    #define KLOGSTART5(pszRetType, pszProto, a0,a1,a2,a3,a4)            ((void)0)
    #define KLOGSTART6(pszRetType, pszProto, a0,a1,a2,a3,a4,a5)         ((void)0)
    #define KLOGSTART7(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6)      ((void)0)
    #define KLOGSTART8(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6,a7)   ((void)0)

    #define KLOGENTRY( pszRetType, pszProto, arg0)                      ((void)0)
    #define KLOGENTRY0(pszRetType)                                      ((void)0)
    #define KLOGENTRY1(pszRetType, pszProto, a0)                        ((void)0)
    #define KLOGENTRY2(pszRetType, pszProto, a0,a1)                     ((void)0)
    #define KLOGENTRY3(pszRetType, pszProto, a0,a1,a2)                  ((void)0)
    #define KLOGENTRY4(pszRetType, pszProto, a0,a1,a2,a3)               ((void)0)
    #define KLOGENTRY5(pszRetType, pszProto, a0,a1,a2,a3,a4)            ((void)0)
    #define KLOGENTRY6(pszRetType, pszProto, a0,a1,a2,a3,a4,a5)         ((void)0)
    #define KLOGENTRY7(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6)      ((void)0)
    #define KLOGENTRY8(pszRetType, pszProto, a0,a1,a2,a3,a4,a5,a6,a7)   ((void)0)

    #define KLOGEXIT(uValue)                                            ((void)0)
    #define KLOGEXITVOID()                                              ((void)0)

    #define KLOGSTOP(uValue)                                            ((void)0)
    #define KLOGSTOPVOID()                                              ((void)0)

    #define kprintf(a)                                                  ((void)0)
    #define dprintf(a)                                                  ((void)0)
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern HKLOGMOD KLOGMODHANDLE;   /* Define this on the behalf of the kKL user. */

#ifdef __cplusplus
}
#endif

#endif

