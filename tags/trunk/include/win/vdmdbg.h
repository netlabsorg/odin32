/* $Id: vdmdbg.h,v 1.1 2001-06-13 04:29:47 bird Exp $
 *
 * VDMDbg - Virtual Dos Machine Debug interface.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _vdmdbg_h_
#define _vdmdbg_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define STATUS_VDM_EVENT    STATUS_SEGMENT_NOTIFICATION

/*
 * Debug event identifiers.
 */
#ifndef DBG_SEGLOAD
#define DBG_SEGLOAD                 0
#define DBG_SEGMOVE                 1
#define DBG_SEGFREE                 2
#define DBG_MODLOAD                 3
#define DBG_MODFREE                 4
#define DBG_SINGLESTEP              5
#define DBG_BREAK                   6
#define DBG_GPFAULT                 7
#define DBG_DIVOVERFLOW             8
#define DBG_INSTRFAULT              9
#define DBG_TASKSTART               10
#define DBG_TASKSTOP                11
#define DBG_DLLSTART                12
#define DBG_DLLSTOP                 13
#define DBG_ATTACH                  14
#define DBG_TOOLHELP                15
#define DBG_STACKFAULT              16
#define DBG_WOWINIT                 17
#define DBG_TEMPBP                  18
#define DBG_MODMOVE                 19
#define DBG_INIT                    20
#define DBG_GPFAULT2                21
#endif


/*
 * These flags are set in the same WORD as the DBG_ event ids (above).
 */
#define VDMEVENT_NEEDS_INTERACTIVE  0x8000
#define VDMEVENT_VERBOSE            0x4000
#define VDMEVENT_PE                 0x2000
#define VDMEVENT_ALLFLAGS           0xe000

/*
 * These flags are set in the second WORD of the exception event
 * parameters.
 */
#define VDMEVENT_V86                0x0001
#define VDMEVENT_PM16               0x0002

/*
 * The following flags control the contents of the CONTEXT structure.
 */
#define VDMCONTEXT_i386             0x00010000    /* this assumes that i386 and */
#define VDMCONTEXT_i486             0x00010000    /* i486 have identical context records */

#define VDMCONTEXT_CONTROL         (VDMCONTEXT_i386 | 0x00000001L) /* SS:SP, CS:IP, FLAGS, BP */
#define VDMCONTEXT_INTEGER         (VDMCONTEXT_i386 | 0x00000002L) /* AX, BX, CX, DX, SI, DI */
#define VDMCONTEXT_SEGMENTS        (VDMCONTEXT_i386 | 0x00000004L) /* DS, ES, FS, GS */
#define VDMCONTEXT_FLOATING_POINT  (VDMCONTEXT_i386 | 0x00000008L) /* 387 state */
#define VDMCONTEXT_DEBUG_REGISTERS (VDMCONTEXT_i386 | 0x00000010L) /* DB 0-3,6,7 */

#define VDMCONTEXT_FULL (VDMCONTEXT_CONTROL | VDMCONTEXT_INTEGER |\
                      VDMCONTEXT_SEGMENTS)


/*
 * More CONTEXT stuff.
 */
#define VDMCONTEXT_TO_PROGRAM_COUNTER(Context) (PVOID)((Context)->Eip)

#define VDMCONTEXT_LENGTH  (sizeof(VDMCONTEXT))
#define VDMCONTEXT_ALIGN   (sizeof(ULONG))
#define VDMCONTEXT_ROUND   (VDMCONTEXT_ALIGN - 1)

#define V86FLAGS_CARRY      0x00001
#define V86FLAGS_PARITY     0x00004
#define V86FLAGS_AUXCARRY   0x00010
#define V86FLAGS_ZERO       0x00040
#define V86FLAGS_SIGN       0x00080
#define V86FLAGS_TRACE      0x00100
#define V86FLAGS_INTERRUPT  0x00200
#define V86FLAGS_DIRECTION  0x00400
#define V86FLAGS_OVERFLOW   0x00800
#define V86FLAGS_IOPL       0x03000
#define V86FLAGS_IOPL_BITS  0x12
#define V86FLAGS_RESUME     0x10000
#define V86FLAGS_V86        0x20000     /* Used to detect RealMode v. ProtMode */
#define V86FLAGS_ALIGNMENT  0x40000

/*
 * Sizes within the module structures.
 */
#define MAX_MODULE_NAME     8 + 1
#define MAX_PATH16          255

/*
 * Seginfo flags.
 */
#define SN_CODE             0           /* Protect mode code segment */
#define SN_DATA             1           /* Protect mode data segment */
#define SN_V86              2           /* V86 mode segment */


/*
 * GlobalFirst/GlobalNext flags
 */
#define GLOBAL_ALL          0
#define GLOBAL_LRU          1
#define GLOBAL_FREE         2

/*
 * GLOBALENTRY.wType entries
 */
#define GT_UNKNOWN          0
#define GT_DGROUP           1
#define GT_DATA             2
#define GT_CODE             3
#define GT_TASK             4
#define GT_RESOURCE         5
#define GT_MODULE           6
#define GT_FREE             7
#define GT_INTERNAL         8
#define GT_SENTINEL         9
#define GT_BURGERMASTER     10

/*
 * If GLOBALENTRY.wType==GT_RESOURCE Then
 *      The following is GLOBALENTRY.wData:
 */
#define GD_USERDEFINED      0
#define GD_CURSORCOMPONENT  1
#define GD_BITMAP           2
#define GD_ICONCOMPONENT    3
#define GD_MENU             4
#define GD_DIALOG           5
#define GD_STRING           6
#define GD_FONTDIR          7
#define GD_FONT             8
#define GD_ACCELERATORS     9
#define GD_RCDATA           10
#define GD_ERRTABLE         11
#define GD_CURSOR           12
#define GD_ICON             14
#define GD_NAMETABLE        15
#define GD_MAX_RESOURCE     15

/*
 * Macros to access the VDM_EVENT parameters.
 */
#define W1(x) ((USHORT)(x.ExceptionInformation[0]))
#define W2(x) ((USHORT)(x.ExceptionInformation[0] >> 16))
#define W3(x) ((USHORT)(x.ExceptionInformation[1]))
#define W4(x) ((USHORT)(x.ExceptionInformation[1] >> 16))
#define DW3(x) (x.ExceptionInformation[2])
#define DW4(x) (x.ExceptionInformation[3])

/*
 * Used with VDMEnumProcessWOW I guess.
 */
#define WOW_SYSTEM  (DWORD)0x0001

/*
 * ???
 */
#define VDMADDR_V86     2
#define VDMADDR_PM16    4
#define VDMADDR_PM32   16

/*
 * ??
 */
#define VDMDBG_BREAK_DOSTASK    0x00000001
#define VDMDBG_BREAK_WOWTASK    0x00000002
#define VDMDBG_BREAK_LOADDLL    0x00000004
#define VDMDBG_BREAK_EXCEPTIONS 0x00000008
#define VDMDBG_BREAK_DEBUGGER   0x00000010
#define VDMDBG_TRACE_HISTORY    0x00000080


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#pragma pack(4)
typedef BOOL (WIN32API *PROCESSENUMPROC)( DWORD dwProcessId, DWORD dwAttributes, LPARAM lpUserDefined );
typedef BOOL (WIN32API *TASKENUMPROC)( DWORD dwThreadId, WORD hMod16, WORD hTask16, LPARAM lpUserDefined );
typedef BOOL (WIN32API *TASKENUMPROCEX)( DWORD dwThreadId, WORD hMod16, WORD hTask16,
                                         LPSTR pszModName, LPSTR pszFileName, LPARAM lpUserDefined );
typedef struct _CONTEXT VDMCONTEXT;
typedef struct _LDT_ENTRY VDMLDT_ENTRY;
typedef VDMCONTEXT *LPVDMCONTEXT;
typedef VDMLDT_ENTRY *LPVDMLDT_ENTRY;


typedef struct _SEGMENT_NOTE {
    WORD    Selector1;                      /* Selector of operation */
    WORD    Selector2;                      /* Dest. Sel. for moving segments */
    WORD    Segment;                        /* Segment within Module */
    CHAR    Module[MAX_MODULE_NAME + 1];    /* Module name */
    CHAR    FileName[MAX_PATH16 + 1];       /* PathName to executable image */
    WORD    Type;                           /* Code / Data, etc. */
    DWORD   Length;                         /* Length of image */
} SEGMENT_NOTE;

typedef struct _IMAGE_NOTE {
    CHAR    Module[MAX_MODULE_NAME + 1];    /* Module */
    CHAR    FileName[MAX_PATH16 + 1];       /* Path to executable image */
    WORD    hModule;                        /* 16-bit hModule */
    WORD    hTask;                          /* 16-bit hTask */
} IMAGE_NOTE;

typedef struct {
    DWORD   dwSize;
    char    szModule[MAX_MODULE_NAME + 1];
    HANDLE  hModule;
    WORD    wcUsage;
    char    szExePath[MAX_PATH16 + 1];
    WORD    wNext;
} MODULEENTRY, *LPMODULEENTRY;

typedef struct _TEMP_BP_NOTE {
    WORD    Seg;                            /* Dest. Segment or Selector */
    DWORD   Offset;                         /* Dest. Offset */
    BOOL    bPM;                            /* TRUE for PM, FALSE for V86 */
} TEMP_BP_NOTE;

typedef struct _VDM_SEGINFO {
    WORD    Selector;                       /* Selector or RM segment */
    WORD    SegNumber;                      /* Logical segment number in executable */
    DWORD   Length;                         /* Length of segment */
    WORD    Type;                           /* Type (0=v86, 1=PM) */
    CHAR    ModuleName[MAX_MODULE_NAME];    /* Module */
    CHAR    FileName[MAX_PATH16];           /* Path to executable image */
} VDM_SEGINFO;

typedef struct {
    DWORD   dwSize;
    DWORD   dwAddress;
    DWORD   dwBlockSize;
    HANDLE  hBlock;
    WORD    wcLock;
    WORD    wcPageLock;
    WORD    wFlags;
    BOOL    wHeapPresent;
    HANDLE  hOwner;
    WORD    wType;
    WORD    wData;
    DWORD   dwNext;
    DWORD   dwNextAlt;
} GLOBALENTRY, *LPGLOBALENTRY;

typedef DWORD (CALLBACK* DEBUGEVENTPROC)( LPDEBUG_EVENT, LPVOID );

/*
 * Typedefinitions for the entry points.
 */
typedef BOOL  (WIN32API *VDMPROCESSEXCEPTIONPROC)(LPDEBUG_EVENT);
typedef BOOL  (WIN32API *VDMGETTHREADSELECTORENTRYPROC)(HANDLE, HANDLE, DWORD, LPVDMLDT_ENTRY);
typedef ULONG (WIN32API *VDMGETPOINTERPROC)(HANDLE, HANDLE, WORD, DWORD, BOOL);
typedef BOOL  (WIN32API *VDMGETCONTEXTPROC)(HANDLE, HANDLE, LPVDMCONTEXT);
typedef BOOL  (WIN32API *VDMSETCONTEXTPROC)(HANDLE, HANDLE, LPVDMCONTEXT);
typedef BOOL  (WIN32API *VDMKILLWOWPROC)(VOID);
typedef BOOL  (WIN32API *VDMDETECTWOWPROC)(VOID);
typedef BOOL  (WIN32API *VDMBREAKTHREADPROC)(HANDLE);
typedef BOOL  (WIN32API *VDMGETSELECTORMODULEPROC)(HANDLE, HANDLE, WORD, PUINT, LPSTR,  UINT, LPSTR,  UINT);
typedef BOOL  (WIN32API *VDMGETMODULESELECTORPROC)(HANDLE, HANDLE, UINT, LPSTR, LPWORD);
typedef BOOL  (WIN32API *VDMMODULEFIRSTPROC)(HANDLE, HANDLE, LPMODULEENTRY, DEBUGEVENTPROC, LPVOID);
typedef BOOL  (WIN32API *VDMMODULENEXTPROC)(HANDLE, HANDLE, LPMODULEENTRY, DEBUGEVENTPROC, LPVOID);
typedef BOOL  (WIN32API *VDMGLOBALFIRSTPROC)(HANDLE, HANDLE, LPGLOBALENTRY, WORD, DEBUGEVENTPROC, LPVOID);
typedef BOOL  (WIN32API *VDMGLOBALNEXTPROC)(HANDLE, HANDLE, LPGLOBALENTRY, WORD, DEBUGEVENTPROC, LPVOID);

typedef INT   (WIN32API *VDMENUMPROCESSWOWPROC)(PROCESSENUMPROC, LPARAM);
typedef INT   (WIN32API *VDMENUMTASKWOWPROC)(DWORD, TASKENUMPROC, LPARAM);
typedef INT   (WIN32API *VDMENUMTASKWOWEXPROC)(DWORD, TASKENUMPROCEX, LPARAM);
typedef BOOL  (WIN32API *VDMTERMINATETASKINWOWPROC)(DWORD, WORD);
typedef BOOL  (WIN32API *VDMSTARTTASKINWOWPROC)(DWORD, LPSTR, WORD);

typedef DWORD (WIN32API *VDMGETDBGFLAGSPROC)(HANDLE);
typedef BOOL  (WIN32API *VDMSETDBGFLAGSPROC)(HANDLE, DWORD);
typedef BOOL  (WIN32API *VDMISMODULELOADEDPROC)(LPSTR);
typedef BOOL  (WIN32API *VDMGETSEGMENTINFOPROC)(WORD, ULONG, BOOL, VDM_SEGINFO);
typedef BOOL  (WIN32API *VDMGETSYMBOLPROC)(LPSTR, WORD, DWORD, BOOL, BOOL, LPSTR, PDWORD);
typedef BOOL  (WIN32API *VDMGETADDREXPRESSIONPROC)(LPSTR, LPSTR, LPWORD, PDWORD, LPWORD);

#pragma pack()


BOOL WIN32API VDMProcessException(
    LPDEBUG_EVENT   lpDebugEvent
    );

BOOL WIN32API VDMGetThreadSelectorEntry(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    LPVDMLDT_ENTRY  lpSelectorEntry
    );

ULONG  WIN32API VDMGetPointer(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    DWORD           dwOffset,
    BOOL            fProtMode
    );

BOOL WIN32API VDMGetThreadContext(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
    );

BOOL WIN32API VDMSetThreadContext(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
    );

BOOL WIN32API VDMGetContext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPVDMCONTEXT    lpVDMContext
    );

BOOL WIN32API VDMSetContext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPVDMCONTEXT    lpVDMContext
    );

BOOL WIN32API VDMGetSelectorModule(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    PUINT           lpSegmentNumber,
    LPSTR           lpModuleName,
    UINT            nNameSize,
    LPSTR           lpModulePath,
    UINT            nPathSize
    );

BOOL WIN32API VDMGetModuleSelector(
    HANDLE          hProcess,
    HANDLE          hThread,
    UINT            wSegmentNumber,
    LPSTR           lpModuleName,
    LPWORD          lpSelector
    );

BOOL WIN32API VDMModuleFirst(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
    );

BOOL WIN32API VDMModuleNext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
    );

BOOL WIN32API VDMGlobalFirst(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPGLOBALENTRY   lpGlobalEntry,
    WORD            wFlags,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
    );

BOOL WIN32API VDMGlobalNext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPGLOBALENTRY   lpGlobalEntry,
    WORD            wFlags,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
    );

INT WIN32API VDMEnumProcessWOW(
    PROCESSENUMPROC fp,
    LPARAM          lparam
    );

INT WIN32API VDMEnumTaskWOW(
    DWORD           dwProcessId,
    TASKENUMPROC    fp,
    LPARAM          lparam
    );

INT WIN32API VDMEnumTaskWOWEx(
    DWORD           dwProcessId,
    TASKENUMPROCEX  fp,
    LPARAM          lparam
    );

BOOL WIN32API VDMTerminateTaskWOW(
    DWORD           dwProcessId,
    WORD            htask
    );

BOOL WIN32API VDMStartTaskInWOW(
    DWORD           dwProcessId,
    LPSTR           lpCommandLine,
    WORD            wShow
    );

BOOL WIN32API VDMKillWOW(VOID);

BOOL WIN32API VDMDetectWOW(VOID);

BOOL WIN32API VDMBreakThread(
    HANDLE          hProcess,
    HANDLE          hThread
    );

DWORD WIN32API VDMGetDbgFlags(
    HANDLE          hProcess
    );

BOOL WIN32API VDMSetDbgFlags(
    HANDLE          hProcess,
    DWORD           dwFlags
    );

//
// VDMIsModuleLoaded can be used to determine if the 16-bit
// executable referenced by the full path name parameter is
// loaded in ntvdm.
//
// Note that this function uses an internal table in vdmdbg.dll
// to determine a module's existence. One important usage of this
// function is to print a message when a particular module is
// loaded for the first time. To accomplish this, call this
// routine during a DBG_SEGLOAD notification BEFORE the entry
// point VDMProcessException has been called. If it returns FALSE,
// then the module has not yet been loaded.
//
BOOL WIN32API VDMIsModuleLoaded(
    LPSTR szPath
    );

BOOL WIN32API VDMGetSegmentInfo(
    WORD Selector,
    ULONG Offset,
    BOOL bProtectMode,
    VDM_SEGINFO *pSegInfo
    );

//
// VDMGetSymbol
//
// This routine reads the standard .SYM file format.
//
// szModule         - module name (max 9 chars)
// SegNumber        - logical segment number of segment (see VDM_SEGINFO)
// Offset           - offset in segment
// bProtectMode     - TRUE for PM, FALSE for V86 mode
// bNextSymbol      - FALSE to find nearest sym BEFORE offset, TRUE for AFTER
// szSymbolName     - receives symbol name (must point to 256 byte buffer)
// pDisplacement    - distance in bytes from nearest symbol
//

BOOL WIN32API VDMGetSymbol(
    LPSTR szModule,
    WORD SegNumber,
    DWORD Offset,
    BOOL bProtectMode,
    BOOL bNextSymbol,
    LPSTR szSymbolName,
    PDWORD pDisplacement
    );

BOOL WIN32API VDMGetAddrExpression(
    LPSTR  szModule,
    LPSTR  szSymbol,
    LPWORD  Selector,
    PDWORD Offset,
    LPWORD  Type
    );


#ifdef __cplusplus
}
#endif

#endif
