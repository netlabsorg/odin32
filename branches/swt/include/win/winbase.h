#ifndef __WINE_WINBASE_H
#define __WINE_WINBASE_H

#include "winnt.h"

#include "pshpack1.h"


#ifdef __cplusplus
extern "C" {
#endif

///-------------------------------------------------------------------[swt-os2]
typedef struct tagACTCTXA {
    ULONG   cbSize;
    DWORD   dwFlags;
    LPCSTR  lpSource;
    USHORT  wProcessorArchitecture;
    LANGID  wLangId;
    LPCSTR  lpAssemblyDirectory;
    LPCSTR  lpResourceName;
    LPCSTR  lpApplicationName;
    HMODULE hModule;
} ACTCTXA, *PACTCTXA;

typedef struct tagACTCTXW {
    ULONG   cbSize;
    DWORD   dwFlags;
    LPCWSTR lpSource;
    USHORT  wProcessorArchitecture;
    LANGID  wLangId;
    LPCWSTR lpAssemblyDirectory;
    LPCWSTR lpResourceName;
    LPCWSTR lpApplicationName;
    HMODULE hModule;
} ACTCTXW, *PACTCTXW;

DECL_WINELIB_TYPE_AW(ACTCTX)
DECL_WINELIB_TYPE_AW(PACTCTX)

typedef const ACTCTXA *PCACTCTXA;
typedef const ACTCTXW *PCACTCTXW;
DECL_WINELIB_TYPE_AW(PCACTCTX)
///----------------------------------------------------------------------------

typedef struct tagCOORD {
    INT16 x;
    INT16 y;
} COORD, *LPCOORD;


  /* Windows Exit Procedure flag values */
#define WEP_FREE_DLL        0
#define WEP_SYSTEM_EXIT     1

typedef DWORD (* CALLBACK LPTHREAD_START_ROUTINE)(LPVOID);

/* This is also defined in winnt.h */
/* typedef struct _EXCEPTION_RECORD {
    DWORD   ExceptionCode;
    DWORD   ExceptionFlags;
    struct  _EXCEPTION_RECORD *ExceptionRecord;
    LPVOID  ExceptionAddress;
    DWORD   NumberParameters;
    DWORD   ExceptionInformation[15];
} EXCEPTION_RECORD; */

typedef struct _EXCEPTION_DEBUG_INFO {
/*    EXCEPTION_RECORD ExceptionRecord; */
    DWORD dwFirstChange;
} EXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID   lpBaseOfDll;
    DWORD    dwDebugInfoFileOffset;
    DWORD    nDebugInfoSize;
    LPVOID   lpImageName;
    WORD     fUnicode;
} LOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD  fUnicode;
    WORD  nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO;

typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO      Exception;
        CREATE_THREAD_DEBUG_INFO  CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO    ExitThread;
        EXIT_PROCESS_DEBUG_INFO   ExitProcess;
        LOAD_DLL_DEBUG_INFO       LoadDll;
        UNLOAD_DLL_DEBUG_INFO     UnloadDll;
        OUTPUT_DEBUG_STRING_INFO  DebugString;
        RIP_INFO                  RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

#define OFS_MAXPATHNAME 128
typedef struct
{
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    BYTE reserved[4];
    BYTE szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT;

#define OF_READ               0x0000
#define OF_WRITE              0x0001
#define OF_READWRITE          0x0002
#define OF_SHARE_COMPAT       0x0000
#define OF_SHARE_EXCLUSIVE    0x0010
#define OF_SHARE_DENY_WRITE   0x0020
#define OF_SHARE_DENY_READ    0x0030
#define OF_SHARE_DENY_NONE    0x0040
#define OF_PARSE              0x0100
#define OF_DELETE             0x0200
#define OF_VERIFY             0x0400   /* Used with OF_REOPEN */
#define OF_SEARCH             0x0400   /* Used without OF_REOPEN */
#define OF_CANCEL             0x0800
#define OF_CREATE             0x1000
#define OF_PROMPT             0x2000
#define OF_EXIST              0x4000
#define OF_REOPEN             0x8000

/* SetErrorMode values */
#define SEM_FAILCRITICALERRORS      0x0001
#define SEM_NOGPFAULTERRORBOX       0x0002
#define SEM_NOALIGNMENTFAULTEXCEPT  0x0004
#define SEM_NOOPENFILEERRORBOX      0x8000

/* CopyFileEx flags */
#define COPY_FILE_FAIL_IF_EXISTS        0x00000001
#define COPY_FILE_RESTARTABLE           0x00000002
#define COPY_FILE_OPEN_SOURCE_FOR_WRITE 0x00000004

/* GetTempFileName() Flags */
#define TF_FORCEDRIVE           0x80


#define DRIVE_UNKNOWN              0
#define DRIVE_NO_ROOT_DIR          1
#define DRIVE_CANNOTDETERMINE      0
#define DRIVE_DOESNOTEXIST         1
#define DRIVE_REMOVABLE            2
#define DRIVE_FIXED                3
#define DRIVE_REMOTE               4
/* Win32 additions */
#define DRIVE_CDROM                5
#define DRIVE_RAMDISK              6

#define MAX_COMPUTERNAME_LENGTH    15

/* The security attributes structure */
typedef struct
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
typedef struct
{
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
} FILETIME, *LPFILETIME, *PFILETIME;
#endif /* _FILETIME_ */

/* Find* structures */
typedef struct
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    CHAR      cFileName[260];
    CHAR      cAlternateFileName[14];
} WIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    WCHAR     cFileName[260];
    WCHAR     cAlternateFileName[14];
} WIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

DECL_WINELIB_TYPE_AW(WIN32_FIND_DATA)
DECL_WINELIB_TYPE_AW(LPWIN32_FIND_DATA)

typedef enum _FINDEX_INFO_LEVELS
{
	FindExInfoStandard,
	FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;

typedef enum _FINDEX_SEARCH_OPS
{
	FindExSearchNameMatch,
	FindExSearchLimitToDirectories,
	FindExSearchLimitToDevices,
	FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

typedef struct
{
    LPVOID lpData;
    DWORD cbData;
    BYTE cbOverhead;
    BYTE iRegionIndex;
    WORD wFlags;
    union {
        struct {
            HANDLE hMem;
            DWORD dwReserved[3];
        } Block;
        struct {
            DWORD dwCommittedSize;
            DWORD dwUnCommittedSize;
            LPVOID lpFirstBlock;
            LPVOID lpLastBlock;
        } Region;
    } DUMMYUNIONNAME;
} PROCESS_HEAP_ENTRY, *PPROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY;

#define PROCESS_HEAP_REGION                   0x0001
#define PROCESS_HEAP_UNCOMMITTED_RANGE        0x0002
#define PROCESS_HEAP_ENTRY_BUSY               0x0004
#define PROCESS_HEAP_ENTRY_MOVEABLE           0x0010
#define PROCESS_HEAP_ENTRY_DDESHARE           0x0020

#define INVALID_HANDLE_VALUE16  ((HANDLE16) -1)
#define INVALID_HANDLE_VALUE  ((HANDLE) -1)
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES  ((DWORD)-1)

/* comm */

/*
 Serial provider type
 */
#define SP_SERIALCOMM ((DWORD)0x00000001)
/*
 Provider SubTypes
 */
#define PST_UNSPECIFIED     ((DWORD)0x00000000)
#define PST_RS232           ((DWORD)0x00000001)
#define PST_PARALLELPORT    ((DWORD)0x00000002)
#define PST_RS422           ((DWORD)0x00000003)
#define PST_RS423           ((DWORD)0x00000004)
#define PST_RS449           ((DWORD)0x00000005)
#define PST_MODEM           ((DWORD)0x00000006)
#define PST_FAX             ((DWORD)0x00000021)
#define PST_SCANNER         ((DWORD)0x00000022)
#define PST_NETWORK_BRIDGE  ((DWORD)0x00000100)
#define PST_LAT             ((DWORD)0x00000101)
#define PST_TCPIP_TELNET    ((DWORD)0x00000102)
#define PST_X25             ((DWORD)0x00000103)
/*
 Provider Caps
 */
#define PCF_DTRDSR          ((DWORD)0x00000001)
#define PCF_RTSCTS          ((DWORD)0x00000002)
#define PCF_RLSD            ((DWORD)0x00000004)
#define PCF_PARITY_CHECK    ((DWORD)0x00000008)
#define PCF_XONXOFF         ((DWORD)0x00000010)
#define PCF_SETXCHAR        ((DWORD)0x00000020)
#define PCF_TOTALTIMEOUTS   ((DWORD)0x00000040)
#define PCF_INITTIMEOUTS    ((DWORD)0x00000080)
#define PCF_SPECIALCHARS    ((DWORD)0x00000100)
#define PCF_16BITMODE       ((DWORD)0x00000200)

/*
 Provider setable parameters
 */
#define SP_PARITY          ((DWORD)0x00000001)
#define SP_BAUD            ((DWORD)0x00000002)
#define SP_DATABITS        ((DWORD)0x00000004)
#define SP_STOPBIT         ((DWORD)0x00000008)
#define SP_HANDSHAKEING    ((DWORD)0x00000010)
#define SP_PARITY_CHECK    ((DWORD)0x00000020)
#define SP_RLSD            ((DWORD)0x00000040)
/*
 Settable baudrates in the provider
*/
#define BAUD_075           ((DWORD)0x00000001)
#define BAUD_110           ((DWORD)0x00000002)
#define BAUD_134_5         ((DWORD)0x00000004)
#define BAUD_150           ((DWORD)0x00000008)
#define BAUD_300           ((DWORD)0x00000010)
#define BAUD_600           ((DWORD)0x00000020)
#define BAUD_1200          ((DWORD)0x00000040)
#define BAUD_1800          ((DWORD)0x00000080)
#define BAUD_2400          ((DWORD)0x00000100)
#define BAUD_4800          ((DWORD)0x00000200)
#define BAUD_7200          ((DWORD)0x00000400)
#define BAUD_9600          ((DWORD)0x00000800)
#define BAUD_14400         ((DWORD)0x00001000)
#define BAUD_19200         ((DWORD)0x00002000)
#define BAUD_38400         ((DWORD)0x00004000)
#define BAUD_56K           ((DWORD)0x00008000)
#define BAUD_128K          ((DWORD)0x00010000)
#define BAUD_115200        ((DWORD)0x00020000)
#define BAUD_57600         ((DWORD)0x00040000)
#define BAUD_USER          ((DWORD)0x10000000)

/*
 Setable Databits
 */
#define DATABITS_5         ((DWORD)0x00000001)
#define DATABITS_6         ((DWORD)0x00000002)
#define DATABITS_7         ((DWORD)0x00000004)
#define DATABITS_8         ((DWORD)0x00000008)
#define DATABITS_16        ((DWORD)0x00000010)
#define DATABITS_16X       ((DWORD)0x00000020)

/*
 Setable Stop and Parity Bits
 */

#define STOPBITS_10       ((DWORD)0x00000001)
#define STOPBITS_15       ((DWORD)0x00000002)
#define STOPBITS_20       ((DWORD)0x00000004)
#define PARITY_NONE       ((DWORD)0x00000100)
#define PARITY_ODD        ((DWORD)0x00000200)
#define PARITY_EVEN       ((DWORD)0x00000400)
#define PARITY_MARK       ((DWORD)0x00000800)
#define PARITY_SPACE      ((DWORD)0x00001000)

#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

#define NOPARITY        0
#define ODDPARITY       1
#define EVENPARITY      2
#define MARKPARITY      3
#define SPACEPARITY     4
#define ONESTOPBIT      0
#define ONE5STOPBITS    1
#define TWOSTOPBITS     2

#define IGNORE          0
#define INFINITE16      0xFFFF
#define INFINITE      0xFFFFFFFF

#define CE_RXOVER       0x0001
#define CE_OVERRUN      0x0002
#define CE_RXPARITY     0x0004
#define CE_FRAME        0x0008
#define CE_BREAK        0x0010
#define CE_CTSTO        0x0020
#define CE_DSRTO        0x0040
#define CE_RLSDTO       0x0080
#define CE_TXFULL       0x0100
#define CE_PTO          0x0200
#define CE_IOE          0x0400
#define CE_DNS          0x0800
#define CE_OOP          0x1000
#define CE_MODE 0x8000

#define IE_BADID        -1
#define IE_OPEN -2
#define IE_NOPEN        -3
#define IE_MEMORY       -4
#define IE_DEFAULT      -5
#define IE_HARDWARE     -10
#define IE_BYTESIZE     -11
#define IE_BAUDRATE     -12

#define EV_RXCHAR       0x0001
#define EV_RXFLAG       0x0002
#define EV_TXEMPTY      0x0004
#define EV_CTS          0x0008
#define EV_DSR          0x0010
#define EV_RLSD 0x0020
#define EV_BREAK        0x0040
#define EV_ERR          0x0080
#define EV_RING 0x0100
#define EV_PERR 0x0200
#define EV_CTSS 0x0400
#define EV_DSRS 0x0800
#define EV_RLSDS        0x1000
#define EV_RINGTE       0x2000
#define EV_RingTe       EV_RINGTE

#define SETXOFF 1
#define SETXON          2
#define SETRTS          3
#define CLRRTS          4
#define SETDTR          5
#define CLRDTR          6
#define RESETDEV        7
#define SETBREAK        8
#define CLRBREAK        9

#define GETBASEIRQ      10

/* Purge functions for Comm Port */
#define PURGE_TXABORT       0x0001  /* Kill the pending/current writes to the
                                       comm port */
#define PURGE_RXABORT       0x0002  /*Kill the pending/current reads to
                                     the comm port */
#define PURGE_TXCLEAR       0x0004  /* Kill the transmit queue if there*/
#define PURGE_RXCLEAR       0x0008  /* Kill the typeahead buffer if there*/


/* Modem Status Flags */
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)

#define RTS_CONTROL_DISABLE     0
#define RTS_CONTROL_ENABLE      1
#define RTS_CONTROL_HANDSHAKE   2
#define RTS_CONTROL_TOGGLE      3

#define DTR_CONTROL_DISABLE     0
#define DTR_CONTROL_ENABLE      1
#define DTR_CONTROL_HANDSHAKE   2

#define CSTF_CTSHOLD    0x01
#define CSTF_DSRHOLD    0x02
#define CSTF_RLSDHOLD   0x04
#define CSTF_XOFFHOLD   0x08
#define CSTF_XOFFSENT   0x10
#define CSTF_EOF        0x20
#define CSTF_TXIM       0x40

#define MAKEINTRESOURCEA(i) (LPSTR)((DWORD)((WORD)(i)))
#define MAKEINTRESOURCEW(i) (LPWSTR)((DWORD)((WORD)(i)))
#define MAKEINTRESOURCE WINELIB_NAME_AW(MAKEINTRESOURCE)

/* Predefined resource types */
#define RT_CURSORA         MAKEINTRESOURCEA(1)
#define RT_CURSORW         MAKEINTRESOURCEW(1)
#define RT_CURSOR            WINELIB_NAME_AW(RT_CURSOR)
#define RT_BITMAPA         MAKEINTRESOURCEA(2)
#define RT_BITMAPW         MAKEINTRESOURCEW(2)
#define RT_BITMAP            WINELIB_NAME_AW(RT_BITMAP)
#define RT_ICONA           MAKEINTRESOURCEA(3)
#define RT_ICONW           MAKEINTRESOURCEW(3)
#define RT_ICON              WINELIB_NAME_AW(RT_ICON)
#define RT_MENUA           MAKEINTRESOURCEA(4)
#define RT_MENUW           MAKEINTRESOURCEW(4)
#define RT_MENU              WINELIB_NAME_AW(RT_MENU)
#define RT_DIALOGA         MAKEINTRESOURCEA(5)
#define RT_DIALOGW         MAKEINTRESOURCEW(5)
#define RT_DIALOG            WINELIB_NAME_AW(RT_DIALOG)
#define RT_STRINGA         MAKEINTRESOURCEA(6)
#define RT_STRINGW         MAKEINTRESOURCEW(6)
//#define RT_STRING            WINELIB_NAME_AW(RT_STRING)
#define RT_STRING         MAKEINTRESOURCE(6)
#define RT_FONTDIRA        MAKEINTRESOURCEA(7)
#define RT_FONTDIRW        MAKEINTRESOURCEW(7)
#define RT_FONTDIR           WINELIB_NAME_AW(RT_FONTDIR)
#define RT_FONTA           MAKEINTRESOURCEA(8)
#define RT_FONTW           MAKEINTRESOURCEW(8)
#define RT_FONT              WINELIB_NAME_AW(RT_FONT)
#define RT_ACCELERATORA    MAKEINTRESOURCEA(9)
#define RT_ACCELERATORW    MAKEINTRESOURCEW(9)
#define RT_ACCELERATOR       WINELIB_NAME_AW(RT_ACCELERATOR)
#define RT_RCDATAA         MAKEINTRESOURCEA(10)
#define RT_RCDATAW         MAKEINTRESOURCEW(10)
#define RT_RCDATA            WINELIB_NAME_AW(RT_RCDATA)
#define RT_MESSAGELISTA    MAKEINTRESOURCEA(11)
#define RT_MESSAGELISTW    MAKEINTRESOURCEW(11)
#define RT_MESSAGELIST       WINELIB_NAME_AW(RT_MESSAGELIST)
#define RT_GROUP_CURSORA   MAKEINTRESOURCEA(12)
#define RT_GROUP_CURSORW   MAKEINTRESOURCEW(12)
#define RT_GROUP_CURSOR      WINELIB_NAME_AW(RT_GROUP_CURSOR)
#define RT_GROUP_ICONA     MAKEINTRESOURCEA(14)
#define RT_GROUP_ICONW     MAKEINTRESOURCEW(14)
#define RT_GROUP_ICON        WINELIB_NAME_AW(RT_GROUP_ICON)


#define LMEM_FIXED          0
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_DISCARDED      0x4000
#define LMEM_LOCKCOUNT      0x00FF
#define LMEM_INVALID_HANDLE 0x8000

#define LHND                (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define LPTR                (LMEM_FIXED | LMEM_ZEROINIT)

#define NONZEROLHND         (LMEM_MOVEABLE)
#define NONZEROLPTR         (LMEM_FIXED)

#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_DISCARDED      0x4000
#define GMEM_LOCKCOUNT      0x00ff
#define GMEM_INVALID_HANDLE 0x8000

#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)

#define GlobalLRUNewest(h)  ((HANDLE)(h))
#define GlobalLRUOldest(h)  ((HANDLE)(h))
#define GlobalDiscard(h)    (GlobalReAlloc((h),0,GMEM_MOVEABLE))

#define INVALID_ATOM        ((ATOM)0)
#define MAXINTATOM          0xc000
#define MAKEINTATOMA(atom)  ((LPCSTR)((ULONG_PTR)((WORD)(atom))))
#define MAKEINTATOMW(atom)  ((LPCWSTR)((ULONG_PTR)((WORD)(atom))))
#ifndef MAKEINTATOM //bird: it's in windef.h too for us.
#define MAKEINTATOM  WINELIB_NAME_AW(MAKEINTATOM)
#endif

typedef struct tagMEMORYSTATUS
{
    DWORD    dwLength;
    DWORD    dwMemoryLoad;
    DWORD    dwTotalPhys;
    DWORD    dwAvailPhys;
    DWORD    dwTotalPageFile;
    DWORD    dwAvailPageFile;
    DWORD    dwTotalVirtual;
    DWORD    dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;


//
// Define the NamedPipe definitions
//


//
// Define the dwOpenMode values for CreateNamedPipe
//

#define PIPE_ACCESS_INBOUND         0x00000001
#define PIPE_ACCESS_OUTBOUND        0x00000002
#define PIPE_ACCESS_DUPLEX          0x00000003

//
// Define the Named Pipe End flags for GetNamedPipeInfo
//

#define PIPE_CLIENT_END             0x00000000
#define PIPE_SERVER_END             0x00000001

//
// Define the dwPipeMode values for CreateNamedPipe
//

#define PIPE_WAIT                   0x00000000
#define PIPE_NOWAIT                 0x00000001
#define PIPE_READMODE_BYTE          0x00000000
#define PIPE_READMODE_MESSAGE       0x00000002
#define PIPE_TYPE_BYTE              0x00000000
#define PIPE_TYPE_MESSAGE           0x00000004

//
// Define the well known values for CreateNamedPipe nMaxInstances
//

#define PIPE_UNLIMITED_INSTANCES    255

#define NMPWAIT_WAIT_FOREVER		0xffffffff
#define NMPWAIT_NOWAIT			0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT	0x00000000

#ifndef NOLOGERROR

/* LogParamError and LogError values */

/* Error modifier bits */
#define ERR_WARNING             0x8000
#define ERR_PARAM               0x4000

#define ERR_SIZE_MASK           0x3000
#define ERR_BYTE                0x1000
#define ERR_WORD                0x2000
#define ERR_DWORD               0x3000

/* LogParamError() values */

/* Generic parameter values */
#define ERR_BAD_VALUE           0x6001
#define ERR_BAD_FLAGS           0x6002
#define ERR_BAD_INDEX           0x6003
#define ERR_BAD_DVALUE          0x7004
#define ERR_BAD_DFLAGS          0x7005
#define ERR_BAD_DINDEX          0x7006
#define ERR_BAD_PTR             0x7007
#define ERR_BAD_FUNC_PTR        0x7008
#define ERR_BAD_SELECTOR        0x6009
#define ERR_BAD_STRING_PTR      0x700a
#define ERR_BAD_HANDLE          0x600b

/* KERNEL parameter errors */
#define ERR_BAD_HINSTANCE       0x6020
#define ERR_BAD_HMODULE         0x6021
#define ERR_BAD_GLOBAL_HANDLE   0x6022
#define ERR_BAD_LOCAL_HANDLE    0x6023
#define ERR_BAD_ATOM            0x6024
#define ERR_BAD_HFILE           0x6025

/* USER parameter errors */
#define ERR_BAD_HWND            0x6040
#define ERR_BAD_HMENU           0x6041
#define ERR_BAD_HCURSOR         0x6042
#define ERR_BAD_HICON           0x6043
#define ERR_BAD_HDWP            0x6044
#define ERR_BAD_CID             0x6045
#define ERR_BAD_HDRVR           0x6046

/* GDI parameter errors */
#define ERR_BAD_COORDS          0x7060
#define ERR_BAD_GDI_OBJECT      0x6061
#define ERR_BAD_HDC             0x6062
#define ERR_BAD_HPEN            0x6063
#define ERR_BAD_HFONT           0x6064
#define ERR_BAD_HBRUSH          0x6065
#define ERR_BAD_HBITMAP         0x6066
#define ERR_BAD_HRGN            0x6067
#define ERR_BAD_HPALETTE        0x6068
#define ERR_BAD_HMETAFILE       0x6069


/* LogError() values */

/* KERNEL errors */
#define ERR_GALLOC              0x0001
#define ERR_GREALLOC            0x0002
#define ERR_GLOCK               0x0003
#define ERR_LALLOC              0x0004
#define ERR_LREALLOC            0x0005
#define ERR_LLOCK               0x0006
#define ERR_ALLOCRES            0x0007
#define ERR_LOCKRES             0x0008
#define ERR_LOADMODULE          0x0009

/* USER errors */
#define ERR_CREATEDLG           0x0040
#define ERR_CREATEDLG2          0x0041
#define ERR_REGISTERCLASS       0x0042
#define ERR_DCBUSY              0x0043
#define ERR_CREATEWND           0x0044
#define ERR_STRUCEXTRA          0x0045
#define ERR_LOADSTR             0x0046
#define ERR_LOADMENU            0x0047
#define ERR_NESTEDBEGINPAINT    0x0048
#define ERR_BADINDEX            0x0049
#define ERR_CREATEMENU          0x004a

/* GDI errors */
#define ERR_CREATEDC            0x0080
#define ERR_CREATEMETA          0x0081
#define ERR_DELOBJSELECTED      0x0082
#define ERR_SELBITMAP           0x0083



/* Debugging support (DEBUG SYSTEM ONLY) */
typedef struct
{
    UINT16  flags;
    DWORD   dwOptions WINE_PACKED;
    DWORD   dwFilter WINE_PACKED;
    CHAR    achAllocModule[8] /* CHAR is always packed (avoid GCC warning) */;
    DWORD   dwAllocBreak WINE_PACKED;
    DWORD   dwAllocCount WINE_PACKED;
} WINDEBUGINFO, *LPWINDEBUGINFO;

/* WINDEBUGINFO flags values */
#define WDI_OPTIONS         0x0001
#define WDI_FILTER          0x0002
#define WDI_ALLOCBREAK      0x0004

/* dwOptions values */
#define DBO_CHECKHEAP       0x0001
#define DBO_BUFFERFILL      0x0004
#define DBO_DISABLEGPTRAPPING 0x0010
#define DBO_CHECKFREE       0x0020

#define DBO_SILENT          0x8000

#define DBO_TRACEBREAK      0x2000
#define DBO_WARNINGBREAK    0x1000
#define DBO_NOERRORBREAK    0x0800
#define DBO_NOFATALBREAK    0x0400
#define DBO_INT3BREAK       0x0100

/* DebugOutput flags values */
#define DBF_TRACE           0x0000
#define DBF_WARNING         0x4000
#define DBF_ERROR           0x8000
#define DBF_FATAL           0xc000

/* dwFilter values */
#define DBF_KERNEL          0x1000
#define DBF_KRN_MEMMAN      0x0001
#define DBF_KRN_LOADMODULE  0x0002
#define DBF_KRN_SEGMENTLOAD 0x0004
#define DBF_USER            0x0800
#define DBF_GDI             0x0400
#define DBF_MMSYSTEM        0x0040
#define DBF_PENWIN          0x0020
#define DBF_APPLICATION     0x0008
#define DBF_DRIVER          0x0010

#endif /* NOLOGERROR */

typedef struct {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;


/* The 'overlapped' data structure used by async I/O functions.
 */
typedef struct {
        DWORD Internal;
        DWORD InternalHigh;
        DWORD Offset;
        DWORD OffsetHigh;
        HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef VOID (* WINAPI LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode,
                                                        DWORD dwNumberOfBytesTransfered,
                                                        LPOVERLAPPED lpOverlapped);

// LockFileEx flags

#define LOCKFILE_FAIL_IMMEDIATELY   0x00000001
#define LOCKFILE_EXCLUSIVE_LOCK     0x00000002


/* Process startup information.
 */

/* STARTUPINFO.dwFlags */
#define STARTF_USESHOWWINDOW    0x00000001
#define STARTF_USESIZE          0x00000002
#define STARTF_USEPOSITION      0x00000004
#define STARTF_USECOUNTCHARS    0x00000008
#define STARTF_USEFILLATTRIBUTE 0x00000010
#define STARTF_RUNFULLSCREEN    0x00000020
#define STARTF_FORCEONFEEDBACK  0x00000040
#define STARTF_FORCEOFFFEEDBACK 0x00000080
#define STARTF_USESTDHANDLES    0x00000100
#define STARTF_USEHOTKEY        0x00000200

typedef struct {
        DWORD cb;               /* 00: size of struct */
        LPSTR lpReserved;       /* 04: */
        LPSTR lpDesktop;        /* 08: */
        LPSTR lpTitle;          /* 0c: */
        DWORD dwX;              /* 10: */
        DWORD dwY;              /* 14: */
        DWORD dwXSize;          /* 18: */
        DWORD dwYSize;          /* 1c: */
        DWORD dwXCountChars;    /* 20: */
        DWORD dwYCountChars;    /* 24: */
        DWORD dwFillAttribute;  /* 28: */
        DWORD dwFlags;          /* 2c: */
        WORD wShowWindow;       /* 30: */
        WORD cbReserved2;       /* 32: */
        BYTE *lpReserved2;      /* 34: */
        HANDLE hStdInput;       /* 38: */
        HANDLE hStdOutput;      /* 3c: */
        HANDLE hStdError;       /* 40: */
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct {
        DWORD cb;
        LPWSTR lpReserved;
        LPWSTR lpDesktop;
        LPWSTR lpTitle;
        DWORD dwX;
        DWORD dwY;
        DWORD dwXSize;
        DWORD dwYSize;
        DWORD dwXCountChars;
        DWORD dwYCountChars;
        DWORD dwFillAttribute;
        DWORD dwFlags;
        WORD wShowWindow;
        WORD cbReserved2;
        BYTE *lpReserved2;
        HANDLE hStdInput;
        HANDLE hStdOutput;
        HANDLE hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

DECL_WINELIB_TYPE_AW(STARTUPINFO)
DECL_WINELIB_TYPE_AW(LPSTARTUPINFO)

typedef struct {
        HANDLE  hProcess;
        HANDLE  hThread;
        DWORD           dwProcessId;
        DWORD           dwThreadId;
} PROCESS_INFORMATION,*LPPROCESS_INFORMATION;

typedef struct {
        LONG Bias;
        WCHAR StandardName[32];
        SYSTEMTIME StandardDate;
        LONG StandardBias;
        WCHAR DaylightName[32];
        SYSTEMTIME DaylightDate;
        LONG DaylightBias;
} TIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

#define TIME_ZONE_ID_UNKNOWN    0
#define TIME_ZONE_ID_STANDARD   1
#define TIME_ZONE_ID_DAYLIGHT   2

#define TIME_ZONE_ID_INVALID    ((DWORD)0xFFFFFFFF)

/* CreateProcess: dwCreationFlag values
 */
#define DEBUG_PROCESS                       0x00000001
#define DEBUG_ONLY_THIS_PROCESS             0x00000002
#define CREATE_SUSPENDED                    0x00000004
#define DETACHED_PROCESS                    0x00000008
#define CREATE_NEW_CONSOLE                  0x00000010
#define NORMAL_PRIORITY_CLASS               0x00000020
#define IDLE_PRIORITY_CLASS                 0x00000040
#define HIGH_PRIORITY_CLASS                 0x00000080
#define REALTIME_PRIORITY_CLASS             0x00000100
#define CREATE_NEW_PROCESS_GROUP            0x00000200
#define CREATE_UNICODE_ENVIRONMENT          0x00000400
#define CREATE_SEPARATE_WOW_VDM             0x00000800
#define CREATE_SHARED_WOW_VDM               0x00001000
#define STACK_SIZE_PARAM_IS_A_RESERVATION   0x00010000
#define CREATE_DEFAULT_ERROR_MODE           0x04000000
#define CREATE_NO_WINDOW                    0x08000000
#define PROFILE_USER                        0x10000000
#define PROFILE_KERNEL                      0x20000000
#define PROFILE_SERVER                      0x40000000


/* File object type definitions
 */
#define FILE_TYPE_UNKNOWN       0
#define FILE_TYPE_DISK          1
#define FILE_TYPE_CHAR          2
#define FILE_TYPE_PIPE          3
#define FILE_TYPE_REMOTE        32768

/* File creation flags
 */
#define FILE_FLAG_WRITE_THROUGH    0x80000000UL
#define FILE_FLAG_OVERLAPPED       0x40000000L
#define FILE_FLAG_NO_BUFFERING     0x20000000L
#define FILE_FLAG_RANDOM_ACCESS    0x10000000L
#define FILE_FLAG_SEQUENTIAL_SCAN  0x08000000L
#define FILE_FLAG_DELETE_ON_CLOSE  0x04000000L
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000L
#define FILE_FLAG_POSIX_SEMANTICS  0x01000000L
#define CREATE_NEW              1
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define TRUNCATE_EXISTING       5

/* Standard handle identifiers
 */
#define STD_INPUT_HANDLE        ((DWORD) -10)
#define STD_OUTPUT_HANDLE       ((DWORD) -11)
#define STD_ERROR_HANDLE        ((DWORD) -12)

typedef struct
{
  int dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  int dwVolumeSerialNumber;
  int nFileSizeHigh;
  int nFileSizeLow;
  int nNumberOfLinks;
  int nFileIndexHigh;
  int nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION ;


/* Power Management functions */

#define AC_LINE_OFFLINE                 0x00
#define AC_LINE_ONLINE                  0x01
#define AC_LINE_BACKUP_POWER            0x02
#define AC_LINE_UNKNOWN                 0xFF

#define BATTERY_FLAG_HIGH               0x01
#define BATTERY_FLAG_LOW                0x02
#define BATTERY_FLAG_CRITICAL           0x04
#define BATTERY_FLAG_CHARGING           0x08
#define BATTERY_FLAG_NO_BATTERY         0x80
#define BATTERY_FLAG_UNKNOWN            0xFF

#define BATTERY_PERCENTAGE_UNKNOWN      0xFF

#define BATTERY_LIFE_UNKNOWN        0xFFFFFFFF

typedef struct _SYSTEM_POWER_STATUS {
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
}   SYSTEM_POWER_STATUS, *LPSYSTEM_POWER_STATUS;


typedef struct tagSYSTEM_INFO
{
    union {
        DWORD   dwOemId;
        struct {
                WORD wProcessorArchitecture;
                WORD wReserved;
        } x;
    } u;
    DWORD       dwPageSize;
    LPVOID      lpMinimumApplicationAddress;
    LPVOID      lpMaximumApplicationAddress;
    DWORD       dwActiveProcessorMask;
    DWORD       dwNumberOfProcessors;
    DWORD       dwProcessorType;
    DWORD       dwAllocationGranularity;
    WORD        wProcessorLevel;
    WORD        wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

/* {G,S}etPriorityClass */
#define NORMAL_PRIORITY_CLASS   0x00000020
#define IDLE_PRIORITY_CLASS     0x00000040
#define HIGH_PRIORITY_CLASS     0x00000080
#define REALTIME_PRIORITY_CLASS 0x00000100

typedef BOOL (* CALLBACK ENUMRESTYPEPROCA)(HMODULE,LPSTR,LONG);
typedef BOOL (* CALLBACK ENUMRESTYPEPROCW)(HMODULE,LPWSTR,LONG);
typedef BOOL (* CALLBACK ENUMRESNAMEPROCA)(HMODULE,LPCSTR,LPSTR,LONG);
typedef BOOL (* CALLBACK ENUMRESNAMEPROCW)(HMODULE,LPCWSTR,LPWSTR,LONG);
typedef BOOL (* CALLBACK ENUMRESLANGPROCA)(HMODULE,LPCSTR,LPCSTR,WORD,LONG);
typedef BOOL (* CALLBACK ENUMRESLANGPROCW)(HMODULE,LPCWSTR,LPCWSTR,WORD,LONG);

DECL_WINELIB_TYPE_AW(ENUMRESTYPEPROC)
DECL_WINELIB_TYPE_AW(ENUMRESNAMEPROC)
DECL_WINELIB_TYPE_AW(ENUMRESLANGPROC)

/* flags that can be passed to LoadLibraryEx */
#define DONT_RESOLVE_DLL_REFERENCES     0x00000001
#define LOAD_LIBRARY_AS_DATAFILE        0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH   0x00000008

/* ifdef _x86_ ... */
typedef struct _LDT_ENTRY {
    WORD        LimitLow;
    WORD        BaseLow;
    union {
        struct {
            BYTE        BaseMid;
            BYTE        Flags1;/*Declare as bytes to avoid alignment problems */
            BYTE        Flags2;
            BYTE        BaseHi;
        } Bytes;
        struct {
            unsigned    BaseMid         : 8;
            unsigned    Type            : 5;
            unsigned    Dpl             : 2;
            unsigned    Pres            : 1;
            unsigned    LimitHi         : 4;
            unsigned    Sys             : 1;
            unsigned    Reserved_0      : 1;
            unsigned    Default_Big     : 1;
            unsigned    Granularity     : 1;
            unsigned    BaseHi          : 8;
        } Bits;
    } HighWord;
} LDT_ENTRY, *LPLDT_ENTRY;


typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTES_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

typedef struct _DllVersionInfo {
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
} DLLVERSIONINFO;

/*
 * This one seems to be a Win32 only definition. It also is defined with
 * WINAPI instead of CALLBACK in the windows headers.
 */
typedef DWORD (* WINAPI LPPROGRESS_ROUTINE)(LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER,
                                           LARGE_INTEGER, DWORD, DWORD, HANDLE,
                                           HANDLE, LPVOID);


#define WAIT_FAILED             0xffffffff
#define WAIT_OBJECT_0           0
#define WAIT_ABANDONED          STATUS_ABANDONED_WAIT_0
#define WAIT_ABANDONED_0        STATUS_ABANDONED_WAIT_0
#define WAIT_IO_COMPLETION      STATUS_USER_APC
#define WAIT_TIMEOUT            STATUS_TIMEOUT

#define STILL_ACTIVE            STATUS_PENDING

#define PAGE_NOACCESS           0x01
#define PAGE_READONLY           0x02
#define PAGE_READWRITE          0x04
#define PAGE_WRITECOPY          0x08
#define PAGE_EXECUTE            0x10
#define PAGE_EXECUTE_READ       0x20
#define PAGE_EXECUTE_READWRITE  0x40
#define PAGE_EXECUTE_WRITECOPY  0x80
#define PAGE_GUARD              0x100
#define PAGE_NOCACHE            0x200

#define MEM_COMMIT              0x00001000
#define MEM_RESERVE             0x00002000
#define MEM_DECOMMIT            0x00004000
#define MEM_RELEASE             0x00008000
#define MEM_FREE                0x00010000
#define MEM_PRIVATE             0x00020000
#define MEM_MAPPED              0x00040000
#define MEM_RESET               0x00080000
#define MEM_TOP_DOWN            0x00100000

#define SEC_FILE                0x00800000
#define SEC_IMAGE               0x01000000
#define SEC_RESERVE             0x04000000
#define SEC_COMMIT              0x08000000
#define SEC_NOCACHE             0x10000000

#define FILE_BEGIN              0
#define FILE_CURRENT            1
#define FILE_END                2

#define FILE_CASE_SENSITIVE_SEARCH      0x00000001
#define FILE_CASE_PRESERVED_NAMES       0x00000002
#define FILE_UNICODE_ON_DISK            0x00000004
#define FILE_PERSISTENT_ACLS            0x00000008

#define FILE_MAP_COPY                   0x00000001
#define FILE_MAP_WRITE                  0x00000002
#define FILE_MAP_READ                   0x00000004
#define FILE_MAP_ALL_ACCESS             0x000f001f

#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_DELAY_UNTIL_REBOOT     0x00000004

#define FS_CASE_SENSITIVE               FILE_CASE_SENSITIVE_SEARCH
#define FS_CASE_IS_PRESERVED            FILE_CASE_PRESERVED_NAMES
#define FS_UNICODE_STORED_ON_DISK       FILE_UNICODE_ON_DISK


#define STATUS_SUCCESS                   0x00000000
#define STATUS_WAIT_0                    0x00000000
#define STATUS_ABANDONED_WAIT_0          0x00000080
#define STATUS_USER_APC                  0x000000C0
#define STATUS_TIMEOUT                   0x00000102
#define STATUS_PENDING                   0x00000103
#define STATUS_GUARD_PAGE_VIOLATION      0x80000001
#define STATUS_DATATYPE_MISALIGNMENT     0x80000002
#define STATUS_BREAKPOINT                0x80000003
#define STATUS_SINGLE_STEP               0x80000004
#define STATUS_BUFFER_OVERFLOW           0x80000005
#define STATUS_ACCESS_VIOLATION          0xC0000005
#define STATUS_IN_PAGE_ERROR             0xC0000006
#define STATUS_INVALID_PARAMETER         0xC000000D
#define STATUS_NO_MEMORY                 0xC0000017
#define STATUS_ILLEGAL_INSTRUCTION       0xC000001D
#define STATUS_BUFFER_TOO_SMALL          0xC0000023
#define STATUS_NONCONTINUABLE_EXCEPTION  0xC0000025
#define STATUS_INVALID_DISPOSITION       0xC0000026
#define STATUS_UNKNOWN_REVISION          0xC0000058
#define STATUS_INVALID_SECURITY_DESCR    0xC0000079
#define STATUS_ARRAY_BOUNDS_EXCEEDED     0xC000008C
#define STATUS_FLOAT_DENORMAL_OPERAND    0xC000008D
#define STATUS_FLOAT_DIVIDE_BY_ZERO      0xC000008E
#define STATUS_FLOAT_INEXACT_RESULT      0xC000008F
#define STATUS_FLOAT_INVALID_OPERATION   0xC0000090
#define STATUS_FLOAT_OVERFLOW            0xC0000091
#define STATUS_FLOAT_STACK_CHECK         0xC0000092
#define STATUS_FLOAT_UNDERFLOW           0xC0000093
#define STATUS_INTEGER_DIVIDE_BY_ZERO    0xC0000094
#define STATUS_INTEGER_OVERFLOW          0xC0000095
#define STATUS_PRIVILEGED_INSTRUCTION    0xC0000096
#define STATUS_INVALID_PARAMETER_2       0xC00000F0
#define STATUS_STACK_OVERFLOW            0xC00000FD
#define STATUS_CONTROL_C_EXIT            0xC000013A

#define DUPLICATE_CLOSE_SOURCE          0x00000001
#define DUPLICATE_SAME_ACCESS           0x00000002

#define HANDLE_FLAG_INHERIT             0x00000001
#define HANDLE_FLAG_PROTECT_FROM_CLOSE  0x00000002

#define HINSTANCE_ERROR 32

#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN    (0x7fffffff)
#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE

#define TLS_OUT_OF_INDEXES              ((DWORD)0xFFFFFFFF)

typedef struct
{
  int type;
} wine_exception;

typedef struct
{
  int pad[39];
  int edi;
  int esi;
  int ebx;
  int edx;
  int ecx;
  int eax;

  int ebp;
  int eip;
  int cs;
  int eflags;
  int esp;
  int ss;
} exception_info;

/* Could this type be considered opaque? */
typedef struct {
        LPVOID  DebugInfo;
        LONG LockCount;
        LONG RecursionCount;
        HANDLE OwningThread;
        HANDLE LockSemaphore;
        DWORD Reserved;
}CRITICAL_SECTION;

#ifdef __WINE__
#ifdef __WIN32OS2__
#define CRITICAL_SECTION_INIT(name) { 0, -1, 0, 0, 0, 0 }
#else
#define CRITICAL_SECTION_INIT(name) { (void *)(__FILE__ ": " name), -1, 0, 0, 0, 0 }
#endif
#endif

typedef struct {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        CHAR szCSDVersion[128];
//~ } OSVERSIONINFOA;
///-------------------------------------------------------------------[swt-os2]
/// From Wine::include/winnt.h
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;
///----------------------------------------------------------------------------

typedef struct {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        WCHAR szCSDVersion[128];
//~ } OSVERSIONINFOW;
///-------------------------------------------------------------------[swt-os2]
/// From Wine::include/winnt.h
} OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW, RTL_OSVERSIONINFOW, *PRTL_OSVERSIONINFOW;
///----------------------------------------------------------------------------

DECL_WINELIB_TYPE_AW(OSVERSIONINFO)
///-------------------------------------------------------------------[swt-os2]
/// From Wine::include/winnt.h
DECL_WINELIB_TYPE_AW(POSVERSIONINFO)
DECL_WINELIB_TYPE_AW(LPOSVERSIONINFO)
///----------------------------------------------------------------------------

typedef struct _OSVERSIONINFOEXA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR  szCSDVersion[128];
    WORD  wServicePackMajor;
    WORD  wServicePackMinor;
    //~ WORD  wReserved[2];
///-------------------------------------------------------------------[swt-os2]
/// From Wine::include/winnt.h
/// That's handy, to reserve an array of 2 WORDS, call it wReserved, then
/// use some of the reserved space in a later definition and call the remaining
/// byte also wReserved.
	WORD wSuiteMask;
	BYTE wProductType;  /// Should be named bProductType
	BYTE wReserved;     /// Should be named bReserved
///----------------------------------------------------------------------------
} OSVERSIONINFOEXA, *POSVERSIONINFOEXA, *LPOSVERSIONINFOEXA;
typedef struct _OSVERSIONINFOEXW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR szCSDVersion[128];
    WORD  wServicePackMajor;
    WORD  wServicePackMinor;
    //~ WORD  wReserved[2];
//~ } OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW;
///-------------------------------------------------------------------[swt-os2]
/// From Wine::include/winnt.h
/// That's handy, to reserve an array of 2 WORDS, call it wReserved, then
/// use some of the reserved space in a later definition and call the remaining
/// byte also wReserved.
	WORD wSuiteMask;
	BYTE wProductType;  /// Should be named bProductType
	BYTE wReserved;     /// Should be named bReserved
} OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW, RTL_OSVERSIONINFOEXW, *PRTL_OSVERSIONINFOEXW;
///----------------------------------------------------------------------------

DECL_WINELIB_TYPE_AW(OSVERSIONINFOEX)
DECL_WINELIB_TYPE_AW(POSVERSIONINFOEX)
DECL_WINELIB_TYPE_AW(LPOSVERSIONINFOEX)

#define VER_SET_CONDITION(_m_,_t_,_c_) ((_m_)=VerSetConditionMask((_m_),(_t_),(_c_)))

#define	VER_PLATFORM_WIN32s             	0
#define	VER_PLATFORM_WIN32_WINDOWS      	1
#define	VER_PLATFORM_WIN32_NT           	2

#define	VER_MINORVERSION			0x00000001
#define	VER_MAJORVERSION			0x00000002
#define	VER_BUILDNUMBER				0x00000004
#define	VER_PLATFORMID				0x00000008
#define	VER_SERVICEPACKMINOR			0x00000010
#define	VER_SERVICEPACKMAJOR			0x00000020
#define	VER_SUITENAME				0x00000040
#define	VER_PRODUCT_TYPE			0x00000080

#define	VER_NT_WORKSTATION			1
#define	VER_NT_DOMAIN_CONTROLLER		2
#define	VER_NT_SERVER				3

#define	VER_SUITE_SMALLBUSINESS			0x00000001
#define	VER_SUITE_ENTERPRISE			0x00000002
#define	VER_SUITE_BACKOFFICE			0x00000004
#define	VER_SUITE_COMMUNICATIONS		0x00000008
#define	VER_SUITE_TERMINAL			0x00000010
#define	VER_SUITE_SMALLBUSINESS_RESTRICTED	0x00000020
#define	VER_SUITE_EMBEDDEDNT			0x00000040
#define	VER_SUITE_DATACENTER			0x00000080
#define	VER_SUITE_SINGLEUSERTS			0x00000100
#define	VER_SUITE_PERSONAL			0x00000200

#define	VER_EQUAL				1
#define	VER_GREATER				2
#define	VER_GREATER_EQUAL			3
#define	VER_LESS				4
#define	VER_LESS_EQUAL				5
#define	VER_AND					6
#define	VER_OR					7

typedef struct _COMMPROP {
    WORD wPacketLength;
    WORD wPacketVersion;
    DWORD dwServiceMask;
    DWORD dwReserved1;
    DWORD dwMaxTxQueue;
    DWORD dwMaxRxQueue;
    DWORD dwMaxBaud;
    DWORD dwProvSubType;
    DWORD dwProvCapabilities;
    DWORD dwSettableParams;
    DWORD dwSettableBaud;
    WORD wSettableData;
    WORD wSettableStopParity;
    DWORD dwCurrentTxQueue;
    DWORD dwCurrentRxQueue;
    DWORD dwProvSpec1;
    DWORD dwProvSpec2;
    WCHAR wcProvChar[1];
} COMMPROP,*LPCOMMPROP;

typedef struct tagCOMSTAT
{
    // DWORD status; // maybe make this a union ??
    unsigned fCtsHold:1;
    unsigned fDsrHold:1;
    unsigned fRlsdHold:1;
    unsigned fXoffHold:1;
    unsigned fXoffSend:1;
    unsigned fEof:1;
    unsigned fTxim:1;
    unsigned fReserved:25;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT,*LPCOMSTAT;

typedef struct tagDCB
{
    DWORD DCBlength;
    DWORD BaudRate;
    unsigned fBinary               :1;
    unsigned fParity               :1;
    unsigned fOutxCtsFlow          :1;
    unsigned fOutxDsrFlow          :1;
    unsigned fDtrControl           :2;
    unsigned fDsrSensitivity       :1;
    unsigned fTXContinueOnXoff     :1;
    unsigned fOutX                 :1;
    unsigned fInX                  :1;
    unsigned fErrorChar            :1;
    unsigned fNull                 :1;
    unsigned fRtsControl           :2;
    unsigned fAbortOnError         :1;
    unsigned fDummy2               :17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
} DCB, *LPDCB;



typedef struct tagCOMMTIMEOUTS {
        DWORD   ReadIntervalTimeout;
        DWORD   ReadTotalTimeoutMultiplier;
        DWORD   ReadTotalTimeoutConstant;
        DWORD   WriteTotalTimeoutMultiplier;
        DWORD   WriteTotalTimeoutConstant;
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

typedef struct _COMMCONFIG {
    DWORD dwSize;
    WORD wVersion;
    WORD wReserved;
    DCB dcb;
    DWORD dwProviderSubType;
    DWORD dwProviderOffset;
    DWORD dwProviderSize;
    WCHAR wcProviderData[1];
} COMMCONFIG,*LPCOMMCONFIG;

typedef void (* CALLBACK PAPCFUNC)(ULONG_PTR);
typedef void (* CALLBACK PTIMERAPCROUTINE)(LPVOID,DWORD,DWORD);

typedef enum _COMPUTER_NAME_FORMAT
{
	ComputerNameNetBIOS,
	ComputerNameDnsHostname,
	ComputerNameDnsDomain,
	ComputerNameDnsFullyQualified,
	ComputerNamePhysicalNetBIOS,
	ComputerNamePhysicalDnsHostname,
	ComputerNamePhysicalDnsDomain,
	ComputerNamePhysicalDnsFullyQualified,
	ComputerNameMax
} COMPUTER_NAME_FORMAT;

#include "poppack.h"

BOOL      WINAPI BuildCommDCBA(LPCSTR,LPDCB);
BOOL      WINAPI BuildCommDCBW(LPCWSTR,LPDCB);
#define     BuildCommDCB WINELIB_NAME_AW(BuildCommDCB)
BOOL      WINAPI BuildCommDCBAndTimeoutsA(LPCSTR,LPDCB,LPCOMMTIMEOUTS);
BOOL      WINAPI BuildCommDCBAndTimeoutsW(LPCWSTR,LPDCB,LPCOMMTIMEOUTS);
#define     BuildCommDCBAndTimeouts WINELIB_NAME_AW(BuildCommDCBAndTimeouts)
BOOL      WINAPI SetCommTimeouts(HANDLE,LPCOMMTIMEOUTS);
BOOL      WINAPI SetCommState(HANDLE,LPDCB);
BOOL      WINAPI TransmitCommChar(HANDLE,CHAR);

BOOL        WINAPI GetCommConfig(HANDLE,LPCOMMCONFIG,LPDWORD);
BOOL        WINAPI GetCommMask(HANDLE,LPDWORD);
BOOL        WINAPI GetCommModemStatus(HANDLE,LPDWORD);
BOOL        WINAPI GetCommProperties(HANDLE,LPCOMMPROP);
BOOL        WINAPI GetCommState(HANDLE,LPDCB);
BOOL        WINAPI GetCommTimeouts(HANDLE,LPCOMMTIMEOUTS);


/*DWORD WINAPI GetVersion( void );*/
BOOL WINAPI GetVersionExA(OSVERSIONINFOA*);
BOOL WINAPI GetVersionExW(OSVERSIONINFOW*);
#define GetVersionEx WINELIB_NAME_AW(GetVersionEx)

/*int WinMain(HINSTANCE, HINSTANCE prev, char *cmd, int show);*/

void      WINAPI DeleteCriticalSection(CRITICAL_SECTION *lpCrit);
void      WINAPI EnterCriticalSection(CRITICAL_SECTION *lpCrit);
void      WINAPI InitializeCriticalSection(CRITICAL_SECTION *lpCrit);
void      WINAPI LeaveCriticalSection(CRITICAL_SECTION *lpCrit);
void      WINAPI MakeCriticalSectionGlobal(CRITICAL_SECTION *lpCrit);
HANDLE  WINAPI OpenProcess(DWORD access, BOOL inherit, DWORD id);
BOOL    WINAPI GetProcessWorkingSetSize(HANDLE,LPDWORD,LPDWORD);
BOOL  WINAPI GetProcessShutdownParameters( LPDWORD lpdwLevel, LPDWORD lpdwFlags );
DWORD WINAPI GetProcessFlags( DWORD processid );
DWORD WINAPI GetProcessVersion(DWORD Processid);
DWORD WINAPI GetProcessHeaps(DWORD nrofheaps,HANDLE *heaps) ;
BOOL  WINAPI GetProcessTimes(HANDLE     hProcess,
                             LPFILETIME lpCreationTime,
                             LPFILETIME lpExitTime,
                             LPFILETIME lpKernelTime,
                             LPFILETIME lpUserTime);
DWORD     WINAPI QueueUserAPC(PAPCFUNC,HANDLE,ULONG_PTR);
void      WINAPI RaiseException(DWORD,DWORD,DWORD,const LPDWORD);
BOOL    WINAPI SetProcessWorkingSetSize(HANDLE,DWORD,DWORD);
BOOL    WINAPI SetProcessPriorityBoost(HANDLE hprocess,BOOL disableboost);
BOOL    WINAPI SetProcessWorkingSetSize(HANDLE hProcess,DWORD minset,
                                        DWORD maxset);
BOOL    WINAPI SetProcessShutdownParameters(DWORD level,DWORD flags);
BOOL    WINAPI TerminateProcess(HANDLE,DWORD);
BOOL    WINAPI TerminateThread(HANDLE,DWORD);
BOOL    WINAPI GetExitCodeThread(HANDLE,LPDWORD);

/* GetBinaryType return values.
 */

#define SCS_32BIT_BINARY    0
#define SCS_DOS_BINARY      1
#define SCS_WOW_BINARY      2
#define SCS_PIF_BINARY      3
#define SCS_POSIX_BINARY    4
#define SCS_OS216_BINARY    5

BOOL WINAPI GetBinaryTypeA( LPCSTR lpApplicationName, LPDWORD lpBinaryType );
BOOL WINAPI GetBinaryTypeW( LPCWSTR lpApplicationName, LPDWORD lpBinaryType );
#define GetBinaryType WINELIB_NAME_AW(GetBinaryType)

BOOL16      WINAPI GetWinDebugInfo16(LPWINDEBUGINFO,UINT16);
BOOL16      WINAPI SetWinDebugInfo16(LPWINDEBUGINFO);
/* Declarations for functions that exist only in Win32 */

BOOL        WINAPI AttachThreadInput(DWORD,DWORD,BOOL);
//BOOL        WINAPI AccessCheck(PSECURITY_DESCRIPTOR,HANDLE,DWORD,PGENERIC_MAPPING,PPRIVILEGE_SET,LPDWORD,LPDWORD,LPBOOL);
BOOL        WINAPI AdjustTokenPrivileges(HANDLE,BOOL,LPVOID,DWORD,LPVOID,LPDWORD);
BOOL        WINAPI AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY,BYTE,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,PSID *);
BOOL        WINAPI AllocateLocallyUniqueId(PLUID);
BOOL      WINAPI AllocConsole(void);
BOOL      WINAPI AreFileApisANSI(void);
BOOL        WINAPI BackupEventLogA(HANDLE,LPCSTR);
BOOL        WINAPI BackupEventLogW(HANDLE,LPCWSTR);
#define     BackupEventLog WINELIB_NAME_AW(BackupEventLog)
BOOL        WINAPI BackupRead(HANDLE,LPBYTE,DWORD,LPDWORD,BOOL,BOOL,LPVOID*);
BOOL        WINAPI BackupSeek(HANDLE,DWORD,DWORD,LPDWORD,LPDWORD,LPVOID*);
BOOL        WINAPI BackupWrite(HANDLE,LPBYTE,DWORD,LPDWORD,BOOL,BOOL,LPVOID*);
BOOL      WINAPI Beep(DWORD,DWORD);
BOOL        WINAPI ClearEventLogA(HANDLE,LPCSTR);
BOOL        WINAPI ClearEventLogW(HANDLE,LPCWSTR);
#define     ClearEventLog WINELIB_NAME_AW(ClearEventLog)
BOOL        WINAPI CloseEventLog(HANDLE);
BOOL      WINAPI CloseHandle(HANDLE);
BOOL        WINAPI CommConfigDialogA(LPCSTR,HANDLE,LPCOMMCONFIG);
BOOL        WINAPI CommConfigDialogW(LPCWSTR,HANDLE,LPCOMMCONFIG);
#define     CommConfigDialog WINELIB_NAME_AW(CommConfigDialog)
BOOL      WINAPI ContinueDebugEvent(DWORD,DWORD,DWORD);
HANDLE    WINAPI ConvertToGlobalHandle(HANDLE hSrc);
BOOL      WINAPI CopyFileA(LPCSTR,LPCSTR,BOOL);
BOOL      WINAPI CopyFileW(LPCWSTR,LPCWSTR,BOOL);
#define     CopyFile WINELIB_NAME_AW(CopyFile)
BOOL      WINAPI CopyFileExA(LPCSTR, LPCSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);
BOOL      WINAPI CopyFileExW(LPCWSTR, LPCWSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);
#define     CopyFileEx WINELIB_NAME_AW(CopyFileEx)
BOOL        WINAPI CopySid(DWORD,PSID,PSID);
INT       WINAPI CompareFileTime(LPFILETIME,LPFILETIME);
HANDLE    WINAPI CreateEventA(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCSTR);
HANDLE    WINAPI CreateEventW(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCWSTR);
#define     CreateEvent WINELIB_NAME_AW(CreateEvent)
HANDLE    WINAPI CreateFileA(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,
                                 DWORD,DWORD,HANDLE);
HANDLE    WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,
                                 DWORD,DWORD,HANDLE);
#define     CreateFile WINELIB_NAME_AW(CreateFile)
HANDLE    WINAPI CreateFileMappingA(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,
                                        DWORD,DWORD,LPCSTR);
HANDLE    WINAPI CreateFileMappingW(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,
                                        DWORD,DWORD,LPCWSTR);
#define     CreateFileMapping WINELIB_NAME_AW(CreateFileMapping)
HANDLE    WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
HANDLE    WINAPI CreateMutexW(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
#define     CreateMutex WINELIB_NAME_AW(CreateMutex)
BOOL      WINAPI CreatePipe(PHANDLE,PHANDLE,LPSECURITY_ATTRIBUTES,DWORD);
BOOL      WINAPI CreateProcessA(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,
                                    LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,
                                    LPSTARTUPINFOA,LPPROCESS_INFORMATION);
BOOL      WINAPI CreateProcessW(LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,
                                    LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,
                                    LPSTARTUPINFOW,LPPROCESS_INFORMATION);
#define     CreateProcess WINELIB_NAME_AW(CreateProcess)
HANDLE      WINAPI CreateRemoteThread(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
HANDLE      WINAPI CreateSemaphoreA(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCSTR);
HANDLE      WINAPI CreateSemaphoreW(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCWSTR);
#define     CreateSemaphore WINELIB_NAME_AW(CreateSemaphore)
DWORD       WINAPI CreateTapePartition(HANDLE,DWORD,DWORD,DWORD);
HANDLE      WINAPI CreateThread(LPSECURITY_ATTRIBUTES,DWORD,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
HANDLE      WINAPI CreateWaitableTimerA(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
HANDLE      WINAPI CreateWaitableTimerW(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
#define     CreateWaitableTimer WINELIB_NAME_AW(CreateWaitableTimer)
BOOL        WINAPI DebugActiveProcess(DWORD);
void        WINAPI DebugBreak(void);
BOOL        WINAPI DeregisterEventSource(HANDLE);
BOOL        WINAPI DisableThreadLibraryCalls(HMODULE);
BOOL        WINAPI DosDateTimeToFileTime(WORD,WORD,LPFILETIME);
BOOL        WINAPI DuplicateHandle(HANDLE,HANDLE,HANDLE,HANDLE*,DWORD,BOOL,DWORD);
BOOL      WINAPI EnumResourceLanguagesA(HMODULE,LPCSTR,LPCSTR,
                                            ENUMRESLANGPROCA,LONG);
BOOL      WINAPI EnumResourceLanguagesW(HMODULE,LPCWSTR,LPCWSTR,
                                            ENUMRESLANGPROCW,LONG);
#define     EnumResourceLanguages WINELIB_NAME_AW(EnumResourceLanguages)
BOOL      WINAPI EnumResourceNamesA(HMODULE,LPCSTR,ENUMRESNAMEPROCA,
                                        LONG);
BOOL      WINAPI EnumResourceNamesW(HMODULE,LPCWSTR,ENUMRESNAMEPROCW,
                                        LONG);
#define     EnumResourceNames WINELIB_NAME_AW(EnumResourceNames)
BOOL      WINAPI EnumResourceTypesA(HMODULE,ENUMRESTYPEPROCA,LONG);
BOOL      WINAPI EnumResourceTypesW(HMODULE,ENUMRESTYPEPROCW,LONG);
#define     EnumResourceTypes WINELIB_NAME_AW(EnumResourceTypes)
BOOL        WINAPI EqualSid(PSID, PSID);
BOOL        WINAPI EqualPrefixSid(PSID,PSID);
DWORD       WINAPI EraseTape(HANDLE,DWORD,BOOL);
VOID        WINAPI ExitProcess(DWORD);
VOID        WINAPI ExitThread(DWORD);
DWORD       WINAPI ExpandEnvironmentStringsA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI ExpandEnvironmentStringsW(LPCWSTR,LPWSTR,DWORD);
#define     ExpandEnvironmentStrings WINELIB_NAME_AW(ExpandEnvironmentStrings)
BOOL      WINAPI FileTimeToDosDateTime(const FILETIME*,LPWORD,LPWORD);
BOOL      WINAPI FileTimeToLocalFileTime(const FILETIME*,LPFILETIME);
BOOL      WINAPI FileTimeToSystemTime(const FILETIME*,LPSYSTEMTIME);
HANDLE    WINAPI FindFirstChangeNotificationA(LPCSTR,BOOL,DWORD);
HANDLE    WINAPI FindFirstChangeNotificationW(LPCWSTR,BOOL,DWORD);
#define     FindFirstChangeNotification WINELIB_NAME_AW(FindFirstChangeNotification)
BOOL      WINAPI FindNextChangeNotification(HANDLE);
BOOL      WINAPI FindCloseChangeNotification(HANDLE);
HRSRC     WINAPI FindResourceExA(HMODULE,LPCSTR,LPCSTR,WORD);
HRSRC     WINAPI FindResourceExW(HMODULE,LPCWSTR,LPCWSTR,WORD);
#define     FindResourceEx WINELIB_NAME_AW(FindResourceEx)
BOOL      WINAPI FlushConsoleInputBuffer(HANDLE);
BOOL      WINAPI FlushFileBuffers(HANDLE);
BOOL      WINAPI FlushViewOfFile(LPCVOID, DWORD);
DWORD       WINAPI FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,
                                    DWORD,LPDWORD);
DWORD       WINAPI FormatMessageW(DWORD,LPCVOID,DWORD,DWORD,LPWSTR,
                                    DWORD,LPDWORD);
#define     FormatMessage WINELIB_NAME_AW(FormatMessage)
BOOL      WINAPI FreeConsole(void);
BOOL      WINAPI FreeEnvironmentStringsA(LPSTR);
BOOL      WINAPI FreeEnvironmentStringsW(LPWSTR);
#define     FreeEnvironmentStrings WINELIB_NAME_AW(FreeEnvironmentStrings)
PVOID       WINAPI FreeSid(PSID);
VOID        WINAPI FreeLibraryAndExitThread(HINSTANCE,DWORD);
UINT      WINAPI GetACP(void);
LPCSTR      WINAPI GetCommandLineA(void);
LPCWSTR     WINAPI GetCommandLineW(void);
#define     GetCommandLine WINELIB_NAME_AW(GetCommandLine)
DWORD WINAPI GetCompressedFileSizeA(LPCTSTR lpFileName, LPDWORD lpFileSizeHigh);
DWORD WINAPI GetCompressedFileSizeW(LPCWSTR lpFileName, LPDWORD lpFileSizeHigh);
#define     GetCompressedFileSize WINELIB_NAME_AW(GetCompressedFileSize)
BOOL      WINAPI GetComputerNameA(LPSTR,LPDWORD);
BOOL      WINAPI GetComputerNameW(LPWSTR,LPDWORD);
#define     GetComputerName WINELIB_NAME_AW(GetComputerName)
UINT      WINAPI GetConsoleCP(void);
BOOL      WINAPI GetConsoleMode(HANDLE,LPDWORD);
UINT      WINAPI GetConsoleOutputCP(void);
DWORD       WINAPI GetConsoleTitleA(LPSTR,DWORD);
DWORD       WINAPI GetConsoleTitleW(LPWSTR,DWORD);
#define     GetConsoleTitle WINELIB_NAME_AW(GetConsoleTitle)
BOOL      WINAPI GetCommMask(HANDLE, LPDWORD);
BOOL      WINAPI GetCommModemStatus(HANDLE, LPDWORD);
HANDLE    WINAPI GetCurrentProcess(void);
DWORD       WINAPI GetCurrentProcessId(void);
HANDLE    WINAPI GetCurrentThread(void);
DWORD       WINAPI GetCurrentThreadId(void);
BOOL        WINAPI GetDefaultCommConfigA(LPCSTR,LPCOMMCONFIG,LPDWORD);
BOOL        WINAPI GetDefaultCommConfigW(LPCWSTR,LPCOMMCONFIG,LPDWORD);
#define     GetDefaultCommConfig WINELIB_NAME_AW(GetDefaultCommConfig)
LPSTR       WINAPI GetEnvironmentStringsA(void);
LPWSTR      WINAPI GetEnvironmentStringsW(void);
#define     GetEnvironmentStrings WINELIB_NAME_AW(GetEnvironmentStrings)
DWORD       WINAPI GetEnvironmentVariableA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetEnvironmentVariableW(LPCWSTR,LPWSTR,DWORD);
#define     GetEnvironmentVariable WINELIB_NAME_AW(GetEnvironmentVariable)
BOOL      WINAPI GetFileAttributesExA(LPCSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
BOOL      WINAPI GetFileAttributesExW(LPCWSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
#define     GetFileAttributesEx WINELIB_NAME_AW(GetFileAttributesEx)
BOOL        WINAPI GetFileInformationByHandle(HANDLE,BY_HANDLE_FILE_INFORMATION*);
BOOL        WINAPI GetFileSecurityA(LPCSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
BOOL        WINAPI GetFileSecurityW(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
#define     GetFileSecurity WINELIB_NAME_AW(GetFileSecurity)
DWORD       WINAPI GetFileSize(HANDLE,LPDWORD);
BOOL      WINAPI GetFileTime(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME);
DWORD       WINAPI GetFileType(HANDLE);
DWORD       WINAPI GetFullPathNameA(LPCSTR,DWORD,LPSTR,LPSTR*);
DWORD       WINAPI GetFullPathNameW(LPCWSTR,DWORD,LPWSTR,LPWSTR*);
#define     GetFullPathName WINELIB_NAME_AW(GetFullPathName)
BOOL      WINAPI GetHandleInformation(HANDLE,LPDWORD);
DWORD       WINAPI GetLargestConsoleWindowSize(HANDLE);
DWORD       WINAPI GetLengthSid(PSID);
VOID        WINAPI GetLocalTime(LPSYSTEMTIME);
DWORD       WINAPI GetLogicalDrives(void);
DWORD       WINAPI GetLongPathNameA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetLongPathNameW(LPCWSTR,LPWSTR,DWORD);
#define     GetLongPathName WINELIB_NAME_AW(GetLongPathName)
BOOL      WINAPI GetNumberOfConsoleInputEvents(HANDLE,LPDWORD);
BOOL      WINAPI GetNumberOfConsoleMouseButtons(LPDWORD);
BOOL        WINAPI GetNumberOfEventLogRecords(HANDLE,PDWORD);
UINT      WINAPI GetOEMCP(void);
BOOL        WINAPI GetOldestEventLogRecord(HANDLE,PDWORD);
DWORD       WINAPI GetPriorityClass(HANDLE);
BOOL        WINAPI GetProcessAffinityMask(HANDLE  hProcess,
                                          LPDWORD lpProcessAffinityMask,
                                          LPDWORD lpSystemAffinityMask);
HANDLE    WINAPI GetProcessHeap(void);
DWORD       WINAPI GetProcessVersion(DWORD);
//BOOL        WINAPI GetSecurityDescriptorControl(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR_CONTROL,LPDWORD);
BOOL        WINAPI GetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR,LPBOOL,PACL *,LPBOOL);
BOOL        WINAPI GetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR,PSID *,LPBOOL);
DWORD       WINAPI GetSecurityDescriptorLength(PSECURITY_DESCRIPTOR);
BOOL        WINAPI GetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR,PSID *,LPBOOL);
BOOL        WINAPI GetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR,LPBOOL,PACL *,LPBOOL);
PSID_IDENTIFIER_AUTHORITY WINAPI GetSidIdentifierAuthority(PSID);
DWORD       WINAPI GetSidLengthRequired(BYTE);
PDWORD      WINAPI GetSidSubAuthority(PSID,DWORD);
PUCHAR      WINAPI GetSidSubAuthorityCount(PSID);
DWORD       WINAPI GetShortPathNameA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetShortPathNameW(LPCWSTR,LPWSTR,DWORD);
#define     GetShortPathName WINELIB_NAME_AW(GetShortPathName)
HANDLE      WINAPI GetStdHandle(DWORD);
BOOL        WINAPI GetStringTypeExA(LCID,DWORD,LPCSTR,INT,LPWORD);
BOOL        WINAPI GetStringTypeExW(LCID,DWORD,LPCWSTR,INT,LPWORD);
#define     GetStringTypeEx WINELIB_NAME_AW(GetStringTypeEx)
VOID        WINAPI GetSystemInfo(LPSYSTEM_INFO);
BOOL        WINAPI GetSystemPowerStatus(LPSYSTEM_POWER_STATUS);
VOID        WINAPI GetSystemTime(LPSYSTEMTIME);
VOID        WINAPI GetSystemTimeAsFileTime(LPFILETIME);
DWORD       WINAPI GetTapeParameters(HANDLE,DWORD,LPDWORD,LPVOID);
DWORD       WINAPI GetTapePosition(HANDLE,DWORD,LPDWORD,LPDWORD,LPDWORD);
DWORD       WINAPI GetTapeStatus(HANDLE);
DWORD       WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION);
BOOL        WINAPI GetThreadContext(HANDLE,CONTEXT *);
INT       WINAPI GetThreadPriority(HANDLE);
BOOL      WINAPI GetThreadSelectorEntry(HANDLE,DWORD,LPLDT_ENTRY);
BOOL        WINAPI GetThreadTimes(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
BOOL        WINAPI GetTokenInformation(HANDLE,TOKEN_INFORMATION_CLASS,LPVOID,DWORD,LPDWORD);
BOOL        WINAPI GetUserNameA(LPSTR,LPDWORD);
BOOL        WINAPI GetUserNameW(LPWSTR,LPDWORD);
#define     GetUserName WINELIB_NAME_AW(GetUserName)
VOID        WINAPI GlobalMemoryStatus(LPMEMORYSTATUS);
LPVOID      WINAPI HeapAlloc(HANDLE,DWORD,DWORD);
DWORD       WINAPI HeapCompact(HANDLE,DWORD);
HANDLE    WINAPI HeapCreate(DWORD,DWORD,DWORD);
BOOL      WINAPI HeapDestroy(HANDLE);
BOOL      WINAPI HeapFree(HANDLE,DWORD,LPVOID);
BOOL      WINAPI HeapLock(HANDLE);
LPVOID      WINAPI HeapReAlloc(HANDLE,DWORD,LPVOID,DWORD);
DWORD       WINAPI HeapSize(HANDLE,DWORD,LPVOID);
BOOL      WINAPI HeapUnlock(HANDLE);
BOOL      WINAPI HeapValidate(HANDLE,DWORD,LPCVOID);
BOOL      WINAPI HeapWalk      (HANDLE, LPVOID);
DWORD       WINAPI InitializeAcl(PACL,DWORD,DWORD);
BOOL        WINAPI InitializeSecurityDescriptor(PSECURITY_DESCRIPTOR,DWORD);
BOOL        WINAPI InitializeSid(PSID,PSID_IDENTIFIER_AUTHORITY,BYTE);
BOOL        WINAPI IsTextUnicode(CONST LPVOID lpBuffer, int cb, LPINT lpi);
BOOL        WINAPI IsValidSecurityDescriptor(PSECURITY_DESCRIPTOR);
BOOL        WINAPI IsValidSid(PSID);
BOOL        WINAPI ImpersonateLoggedOnUser(HANDLE);
BOOL        WINAPI ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL);
BOOL        WINAPI IsProcessorFeaturePresent(DWORD);
LONG        WINAPI InterlockedCompareExchange( PLONG dest, LONG xchg, LONG compare );
LONG        WINAPI InterlockedDecrement(LPLONG);
LONG        WINAPI InterlockedExchange(LPLONG,LONG);
LONG        WINAPI InterlockedExchangeAdd( PLONG dest, LONG incr );
LONG        WINAPI InterlockedIncrement(LPLONG);

/* FIXME: should handle platforms where sizeof(void*) != sizeof(long) */
#define InterlockedCompareExchangePointer(dest, xchg, compare) \
    (PVOID)InterlockedCompareExchange( (PLONG)dest, (LONG)xchg, (LONG)compare )

#define InterlockedExchangePointer(dest, val) \
    (PVOID)InterlockedExchange( (PLONG)dest, (LONG)val )

BOOL      WINAPI IsDBCSLeadByteEx(UINT,BYTE);
BOOL      WINAPI IsProcessorFeaturePresent(DWORD);
BOOL      WINAPI IsValidLocale(DWORD,DWORD);
BOOL      WINAPI LocalFileTimeToFileTime(const FILETIME*,LPFILETIME);
BOOL      WINAPI LockFile(HANDLE,DWORD,DWORD,DWORD,DWORD);
BOOL      WINAPI LockFileEx(HANDLE, DWORD, DWORD, DWORD, DWORD, LPOVERLAPPED);
BOOL        WINAPI LookupPrivilegeValueA(LPCSTR,LPCSTR,LPVOID);
BOOL        WINAPI LookupPrivilegeValueW(LPCWSTR,LPCWSTR,LPVOID);
#define     LookupPrivilegeValue WINELIB_NAME_AW(LookupPrivilegeValue)
BOOL        WINAPI MakeSelfRelativeSD(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,LPDWORD);
HMODULE   WINAPI MapHModuleSL(HMODULE16);
HMODULE16   WINAPI MapHModuleLS(HMODULE);
LPVOID      WINAPI MapViewOfFile(HANDLE,DWORD,DWORD,DWORD,DWORD);
LPVOID      WINAPI MapViewOfFileEx(HANDLE,DWORD,DWORD,DWORD,DWORD,LPVOID);
BOOL      WINAPI MoveFileA(LPCSTR,LPCSTR);
BOOL      WINAPI MoveFileW(LPCWSTR,LPCWSTR);
#define     MoveFile WINELIB_NAME_AW(MoveFile)
BOOL      WINAPI MoveFileExA(LPCSTR,LPCSTR,DWORD);
BOOL      WINAPI MoveFileExW(LPCWSTR,LPCWSTR,DWORD);
#define     MoveFileEx WINELIB_NAME_AW(MoveFileEx)
INT       WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
BOOL        WINAPI NotifyChangeEventLog(HANDLE,HANDLE);
INT       WINAPI WideCharToMultiByte(UINT,DWORD,LPCWSTR,INT,LPSTR,INT,LPCSTR,BOOL*);
HANDLE      WINAPI OpenBackupEventLogA(LPCSTR,LPCSTR);
HANDLE      WINAPI OpenBackupEventLogW(LPCWSTR,LPCWSTR);
#define     OpenBackupEventLog WINELIB_NAME_AW(OpenBackupEventLog)
HANDLE    WINAPI OpenEventA(DWORD,BOOL,LPCSTR);
HANDLE    WINAPI OpenEventW(DWORD,BOOL,LPCWSTR);
#define     OpenEvent WINELIB_NAME_AW(OpenEvent)
HANDLE      WINAPI OpenEventLogA(LPCSTR,LPCSTR);
HANDLE      WINAPI OpenEventLogW(LPCWSTR,LPCWSTR);
#define     OpenEventLog WINELIB_NAME_AW(OpenEventLog)
HANDLE    WINAPI OpenFileMappingA(DWORD,BOOL,LPCSTR);
HANDLE    WINAPI OpenFileMappingW(DWORD,BOOL,LPCWSTR);
#define     OpenFileMapping WINELIB_NAME_AW(OpenFileMapping)
HANDLE    WINAPI OpenMutexA(DWORD,BOOL,LPCSTR);
HANDLE    WINAPI OpenMutexW(DWORD,BOOL,LPCWSTR);
#define     OpenMutex WINELIB_NAME_AW(OpenMutex)
HANDLE    WINAPI OpenProcess(DWORD,BOOL,DWORD);
BOOL        WINAPI OpenProcessToken(HANDLE,DWORD,PHANDLE);
HANDLE    WINAPI OpenSemaphoreA(DWORD,BOOL,LPCSTR);
HANDLE    WINAPI OpenSemaphoreW(DWORD,BOOL,LPCWSTR);
#define     OpenSemaphore WINELIB_NAME_AW(OpenSemaphore)
BOOL        WINAPI OpenThreadToken(HANDLE,DWORD,BOOL,PHANDLE);

BOOL WIN32API PostQueuedCompletionStatus(HANDLE       CompletionPort,
                                            DWORD        dwNumberOfBytesTransferred,
                                            DWORD        dwCompletionKey,
                                            LPOVERLAPPED lpOverlapped);

DWORD       WINAPI PrepareTape(HANDLE,DWORD,BOOL);
BOOL      WINAPI PulseEvent(HANDLE);
BOOL      WINAPI PurgeComm(HANDLE,DWORD);
DWORD       WINAPI QueryDosDeviceA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI QueryDosDeviceW(LPCWSTR,LPWSTR,DWORD);
#define     QueryDosDevice WINELIB_NAME_AW(QueryDosDevice)
BOOL      WINAPI QueryPerformanceCounter(PLARGE_INTEGER);
BOOL      WINAPI QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);

BOOL      WINAPI ReadConsoleA(HANDLE,LPVOID,DWORD,LPDWORD,LPVOID);
BOOL      WINAPI ReadConsoleW(HANDLE,LPVOID,DWORD,LPDWORD,LPVOID);
#define     ReadConsole WINELIB_NAME_AW(ReadConsole)
BOOL      WINAPI ReadConsoleOutputCharacterA(HANDLE,LPSTR,DWORD,
                                                 COORD,LPDWORD);
#define     ReadConsoleOutputCharacter WINELIB_NAME_AW(ReadConsoleOutputCharacter)
BOOL        WINAPI ReadEventLogA(HANDLE,DWORD,DWORD,LPVOID,DWORD,DWORD *,DWORD *);
BOOL        WINAPI ReadEventLogW(HANDLE,DWORD,DWORD,LPVOID,DWORD,DWORD *,DWORD *);
#define     ReadEventLog WINELIB_NAME_AW(ReadEventLog)
BOOL      WINAPI ReadFile(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL      WINAPI ReadFileEx(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
                            LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

HANDLE      WINAPI RegisterEventSourceA(LPCSTR,LPCSTR);
HANDLE      WINAPI RegisterEventSourceW(LPCWSTR,LPCWSTR);
#define     RegisterEventSource WINELIB_NAME_AW(RegisterEventSource)
DWORD WINAPI RegisterServiceProcess(DWORD dwProcessId, DWORD dwType);
BOOL      WINAPI ReleaseMutex(HANDLE);
BOOL      WINAPI ReleaseSemaphore(HANDLE,LONG,LPLONG);
BOOL        WINAPI ReportEventA(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCSTR *,LPVOID);
BOOL        WINAPI ReportEventW(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCWSTR *,LPVOID);
#define     ReportEvent WINELIB_NAME_AW(ReportEvent)
BOOL      WINAPI ResetEvent(HANDLE);
DWORD       WINAPI ResumeThread(HANDLE);
VOID        WINAPI RtlFillMemory(LPVOID,UINT,UINT);
#define     FillMemory RtlFillMemory
VOID        WINAPI RtlMoveMemory(LPVOID,LPCVOID,UINT);
#define     MoveMemory RtlMoveMemory
VOID        WINAPI RtlZeroMemory(LPVOID,UINT);
#define     ZeroMemory RtlZeroMemory
VOID        WINAPI RtlCopyMemory(LPVOID,const VOID*, UINT);
#define     CopyMemory RtlCopyMemory
BOOL        WINAPI RevertToSelf(void);
DWORD       WINAPI SearchPathA(LPCSTR,LPCSTR,LPCSTR,DWORD,LPSTR,LPSTR*);
DWORD       WINAPI SearchPathW(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPWSTR,LPWSTR*);
#define     SearchPath WINELIB_NAME_AW(SearchPath)
BOOL        WINAPI SetCommConfig(HANDLE,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetCommBreak(HANDLE);
BOOL        WINAPI SetCommMask(HANDLE,DWORD);
BOOL        WINAPI SetCommState(HANDLE,LPDCB);
BOOL        WINAPI SetCommTimeouts(HANDLE,LPCOMMTIMEOUTS);
BOOL      WINAPI SetComputerNameA(LPCSTR);
BOOL      WINAPI SetComputerNameW(LPCWSTR);
#define     SetComputerName WINELIB_NAME_AW(SetComputerName)
BOOL        WINAPI SetDefaultCommConfigA(LPCSTR,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetDefaultCommConfigW(LPCWSTR,LPCOMMCONFIG,DWORD);
#define     SetDefaultCommConfig WINELIB_NAME_AW(SetDefaultCommConfig)
BOOL      WINAPI SetConsoleCursorPosition(HANDLE,COORD);
BOOL      WINAPI SetConsoleMode(HANDLE,DWORD);
BOOL      WINAPI SetConsoleTitleA(LPCSTR);
BOOL      WINAPI SetConsoleTitleW(LPCWSTR);
#define     SetConsoleTitle WINELIB_NAME_AW(SetConsoleTitle)
BOOL        WINAPI SetDefaultCommConfigA(LPCSTR,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetDefaultCommConfigW(LPCWSTR,LPCOMMCONFIG,DWORD);
#define     SetDefaultCommConfig WINELIB_NAME_AW(SetDefaultCommConfig)
BOOL      WINAPI SetEndOfFile(HANDLE);
BOOL      WINAPI DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode,
                                 LPVOID lpInBuffer, DWORD nInBufferSize,
                                 LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                 LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

BOOL      WINAPI SetEnvironmentVariableA(LPCSTR,LPCSTR);
BOOL      WINAPI SetEnvironmentVariableW(LPCWSTR,LPCWSTR);
#define     SetEnvironmentVariable WINELIB_NAME_AW(SetEnvironmentVariable)
BOOL      WINAPI SetEvent(HANDLE);
VOID        WINAPI SetFileApisToANSI(void);
VOID        WINAPI SetFileApisToOEM(void);
DWORD       WINAPI SetFilePointer(HANDLE,LONG,LPLONG,DWORD);
BOOL        WINAPI SetFilePointerEx(HANDLE,LARGE_INTEGER,LARGE_INTEGER*,DWORD);
BOOL        WINAPI SetFileSecurityA(LPCSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
BOOL        WINAPI SetFileSecurityW(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
#define     SetFileSecurity WINELIB_NAME_AW(SetFileSecurity)
BOOL        WINAPI SetFileTime(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);
BOOL        WINAPI SetHandleInformation(HANDLE,DWORD,DWORD);
BOOL        WINAPI SetLocalTime(const SYSTEMTIME*);
BOOL        WINAPI SetPriorityClass(HANDLE,DWORD);
BOOL        WINAPI SetProcessAffinityMask( HANDLE hProcess, DWORD affmask );
BOOL        WINAPI SetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR,BOOL,PACL,BOOL);
BOOL        WINAPI SetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR,PSID,BOOL);
BOOL        WINAPI SetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR,PSID,BOOL);
BOOL        WINAPI SetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR,BOOL,PACL,BOOL);
BOOL      WINAPI SetStdHandle(DWORD,HANDLE);
BOOL      WINAPI SetSystemPowerState(BOOL,BOOL);
BOOL      WINAPI SetSystemTime(const SYSTEMTIME*);
DWORD       WINAPI SetTapeParameters(HANDLE,DWORD,LPVOID);
BOOL        WINAPI SetTapePosition(HANDLE,DWORD,DWORD,DWORD,DWORD,BOOL);
DWORD       WINAPI SetThreadAffinityMask(HANDLE,DWORD);
BOOL        WINAPI SetThreadContext(HANDLE,const CONTEXT *);
DWORD       WINAPI SetThreadExecutionState(EXECUTION_STATE);
BOOL        WINAPI SetThreadPriority(HANDLE,INT);
BOOL        WINAPI SetThreadPriorityBoost(HANDLE,BOOL);
BOOL        WINAPI SetThreadToken(PHANDLE,HANDLE);
BOOL      WINAPI SetThreadLocale(LCID);
BOOL      WINAPI SetThreadPriority(HANDLE,INT);
BOOL      WINAPI SetTimeZoneInformation(const LPTIME_ZONE_INFORMATION);
BOOL        WINAPI SetupComm(HANDLE,DWORD,DWORD);
VOID        WINAPI Sleep(DWORD);
DWORD       WINAPI SleepEx(DWORD,BOOL);
DWORD       WINAPI SuspendThread(HANDLE);
BOOL        WINAPI SwitchToThread(void);
BOOL      WINAPI SystemTimeToFileTime(const SYSTEMTIME*,LPFILETIME);
DWORD       WINAPI TlsAlloc(void);
BOOL      WINAPI TlsFree(DWORD);
LPVOID      WINAPI TlsGetValue(DWORD);
BOOL      WINAPI TlsSetValue(DWORD,LPVOID);
BOOL      WINAPI UnlockFile(HANDLE,DWORD,DWORD,DWORD,DWORD);
BOOL      WINAPI UnmapViewOfFile(LPVOID);
BOOL      WINAPI FlushInstructionCache(HANDLE,LPCVOID,DWORD);
LPVOID      WINAPI VirtualAlloc(LPVOID,DWORD,DWORD,DWORD);
BOOL      WINAPI VirtualFree(LPVOID,DWORD,DWORD);
BOOL      WINAPI VirtualLock(LPVOID,DWORD);
BOOL      WINAPI VirtualProtect(LPVOID,DWORD,DWORD,LPDWORD);
BOOL      WINAPI VirtualProtectEx(HANDLE,LPVOID,DWORD,DWORD,LPDWORD);
DWORD       WINAPI VirtualQuery(LPCVOID,LPMEMORY_BASIC_INFORMATION,DWORD);
DWORD       WINAPI VirtualQueryEx(HANDLE,LPCVOID,LPMEMORY_BASIC_INFORMATION,DWORD);
BOOL      WINAPI VirtualUnlock(LPVOID,DWORD);
BOOL      WINAPI WaitCommEvent(HANDLE,LPDWORD,LPOVERLAPPED);
BOOL      WINAPI WaitForDebugEvent(LPDEBUG_EVENT,DWORD);
DWORD       WINAPI WaitForMultipleObjects(DWORD,const HANDLE*,BOOL,DWORD);
DWORD       WINAPI WaitForMultipleObjectsEx(DWORD,const HANDLE*,BOOL,DWORD,BOOL);
DWORD       WINAPI WaitForSingleObject(HANDLE,DWORD);
DWORD       WINAPI WaitForSingleObjectEx(HANDLE,DWORD,BOOL);
BOOL      WINAPI WriteConsoleA(HANDLE,LPCVOID,DWORD,LPDWORD,LPVOID);
BOOL      WINAPI WriteConsoleW(HANDLE,LPCVOID,DWORD,LPDWORD,LPVOID);
#define     WriteConsole WINELIB_NAME_AW(WriteConsole)
BOOL      WINAPI WriteFile(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL      WINAPI WriteFileEx(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
                             LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
DWORD       WINAPI WriteTapemark(HANDLE,DWORD,DWORD,BOOL);
DWORD       WINAPI GetLastError(void);
LANGID      WINAPI GetSystemDefaultLangID(void);
LCID        WINAPI GetSystemDefaultLCID(void);
LANGID      WINAPI GetUserDefaultLangID(void);
LCID        WINAPI GetUserDefaultLCID(void);
ATOM        WINAPI AddAtomA(LPCSTR);
ATOM        WINAPI AddAtomW(LPCWSTR);
#define     AddAtom WINELIB_NAME_AW(AddAtom)
BOOL      WINAPI CancelIo(HANDLE hFile);
BOOL        WINAPI CancelWaitableTimer(HANDLE);
BOOL        WINAPI CheckTokenMembership(HANDLE,PSID,PBOOL);
BOOL        WINAPI ClearCommBreak(HANDLE);
BOOL        WINAPI ClearCommError(HANDLE,LPDWORD,LPCOMSTAT);
BOOL        WINAPI ClearEventLogA(HANDLE,LPCSTR);
BOOL        WINAPI ClearEventLogW(HANDLE,LPCWSTR);
#define     ClearEventLog WINELIB_NAME_AW(ClearEventLog)
BOOL      WINAPI CreateDirectoryA(LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL      WINAPI CreateDirectoryW(LPCWSTR,LPSECURITY_ATTRIBUTES);
#define     CreateDirectory WINELIB_NAME_AW(CreateDirectory)
BOOL      WINAPI CreateDirectoryExA(LPCSTR,LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL      WINAPI CreateDirectoryExW(LPCWSTR,LPCWSTR,LPSECURITY_ATTRIBUTES);
#define     CreateDirectoryEx WINELIB_NAME_AW(CreateDirectoryEx)
HANDLE    WINAPI CreateIoCompletionPort( HANDLE FileHandle,
                                         HANDLE ExistingCompletionPort,
                                         DWORD CompletionKey,
                                         DWORD NumberOfConcurrentThreads);
BOOL        WINAPI DefineDosDeviceA(DWORD,LPCSTR,LPCSTR);
BOOL        WINAPI DefineDosDeviceW(DWORD,LPCWSTR,LPCWSTR);
#define     DefineDosDevice WINELIB_NAME_AW(DefineDosDevice)

ATOM        WINAPI DeleteAtom(ATOM);
BOOL      WINAPI DeleteFileA(LPCSTR);
BOOL      WINAPI DeleteFileW(LPCWSTR);
#define     DeleteFile WINELIB_NAME_AW(DeleteFile)
void        WINAPI FatalAppExitA(UINT,LPCSTR);
void        WINAPI FatalAppExitW(UINT,LPCWSTR);
#define     FatalAppExit WINELIB_NAME_AW(FatalAppExit)
VOID WIN32API FatalExit( UINT exitCode);
ATOM        WINAPI FindAtomA(LPCSTR);
ATOM        WINAPI FindAtomW(LPCWSTR);
#define     FindAtom WINELIB_NAME_AW(FindAtom)
BOOL      WINAPI FindClose(HANDLE);
HANDLE    WINAPI FindFirstFileA(LPCSTR,LPWIN32_FIND_DATAA);
HANDLE    WINAPI FindFirstFileW(LPCWSTR,LPWIN32_FIND_DATAW);
#define     FindFirstFile WINELIB_NAME_AW(FindFirstFile)
HANDLE      WINAPI FindFirstFileExA(LPCSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
HANDLE      WINAPI FindFirstFileExW(LPCWSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
#define     FindFirstFileEx WINELIB_NAME_AW(FindFirstFileEx)
HANDLE    WINAPI FindFirstFileMultiA(LPCSTR,LPWIN32_FIND_DATAA,DWORD*); //Odin only
BOOL      WINAPI FindNextFileA(HANDLE,LPWIN32_FIND_DATAA);
BOOL      WINAPI FindNextFileW(HANDLE,LPWIN32_FIND_DATAW);
#define     FindNextFile WINELIB_NAME_AW(FindNextFile)
BOOL      WINAPI FindNextFileMultiA(HANDLE,LPWIN32_FIND_DATAA,DWORD*); //Odin only
HRSRC     WINAPI FindResourceA(HMODULE,LPCSTR,LPCSTR);
HRSRC     WINAPI FindResourceW(HMODULE,LPCWSTR,LPCWSTR);
#define     FindResource WINELIB_NAME_AW(FindResource)

VOID      WINAPI FreeLibrary16(HINSTANCE16);
BOOL      WINAPI FreeLibrary(HMODULE);
#define     FreeModule(handle) FreeLibrary(handle)
#define     FreeProcInstance(proc) /*nothing*/
BOOL      WINAPI FreeResource(HGLOBAL);
UINT      WINAPI GetAtomNameA(ATOM,LPSTR,INT);
UINT      WINAPI GetAtomNameW(ATOM,LPWSTR,INT);
#define     GetAtomName WINELIB_NAME_AW(GetAtomName)
UINT      WINAPI GetCurrentDirectoryA(UINT,LPSTR);
UINT      WINAPI GetCurrentDirectoryW(UINT,LPWSTR);
#define     GetCurrentDirectory WINELIB_NAME_AW(GetCurrentDirectory)
BOOL      WINAPI GetDiskFreeSpaceA(LPCSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
BOOL      WINAPI GetDiskFreeSpaceW(LPCWSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
#define     GetDiskFreeSpace WINELIB_NAME_AW(GetDiskFreeSpace)
BOOL      WINAPI GetDiskFreeSpaceExA(LPCSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
BOOL      WINAPI GetDiskFreeSpaceExW(LPCWSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
#define     GetDiskFreeSpaceEx WINELIB_NAME_AW(GetDiskFreeSpaceEx)
UINT      WINAPI GetDriveTypeA(LPCSTR);
UINT      WINAPI GetDriveTypeW(LPCWSTR);
#define     GetDriveType WINELIB_NAME_AW(GetDriveType)
DWORD       WINAPI GetFileAttributesA(LPCSTR);
DWORD       WINAPI GetFileAttributesW(LPCWSTR);
#define     GetFileAttributes WINELIB_NAME_AW(GetFileAttributes)
#define     GetFreeSpace(w) (0x100000L)
UINT      WINAPI GetLogicalDriveStringsA(UINT,LPSTR);
UINT      WINAPI GetLogicalDriveStringsW(UINT,LPWSTR);
#define     GetLogicalDriveStrings WINELIB_NAME_AW(GetLogicalDriveStrings)
INT       WINAPI GetLocaleInfoA(LCID,LCTYPE,LPSTR,INT);
INT       WINAPI GetLocaleInfoW(LCID,LCTYPE,LPWSTR,INT);
#define     GetLocaleInfo WINELIB_NAME_AW(GetLocaleInfo)
DWORD       WINAPI GetModuleFileNameA(HMODULE,LPSTR,DWORD);
DWORD       WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
#define     GetModuleFileName WINELIB_NAME_AW(GetModuleFileName)
HMODULE   WINAPI GetModuleHandleA(LPCSTR);
HMODULE   WINAPI GetModuleHandleW(LPCWSTR);
#define     GetModuleHandle WINELIB_NAME_AW(GetModuleHandle)
BOOL      WINAPI GetOverlappedResult(HANDLE,LPOVERLAPPED,LPDWORD,BOOL);
UINT      WINAPI GetPrivateProfileIntA(LPCSTR,LPCSTR,INT,LPCSTR);
UINT      WINAPI GetPrivateProfileIntW(LPCWSTR,LPCWSTR,INT,LPCWSTR);
#define     GetPrivateProfileInt WINELIB_NAME_AW(GetPrivateProfileInt)
INT       WINAPI GetPrivateProfileSectionA(LPCSTR,LPSTR,DWORD,LPCSTR);
INT       WINAPI GetPrivateProfileSectionW(LPCWSTR,LPWSTR,DWORD,LPCWSTR);
#define     GetPrivateProfileSection WINELIB_NAME_AW(GetPrivateProfileSection)
DWORD       WINAPI GetPrivateProfileSectionNamesA(LPSTR,DWORD,LPCSTR);
DWORD       WINAPI GetPrivateProfileSectionNamesW(LPWSTR,DWORD,LPCWSTR);
#define     GetPrivateProfileSectionNames WINELIB_NAME_AW(GetPrivateProfileSectionNames)
INT       WINAPI GetPrivateProfileStringA(LPCSTR,LPCSTR,LPCSTR,LPSTR,UINT,LPCSTR);
INT       WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,UINT,LPCWSTR);
#define     GetPrivateProfileString WINELIB_NAME_AW(GetPrivateProfileString)
BOOL      WINAPI GetPrivateProfileStructA(LPCSTR,LPCSTR,LPVOID,UINT,LPCSTR);
BOOL      WINAPI GetPrivateProfileStructW(LPCWSTR,LPCWSTR,LPVOID,UINT,LPCWSTR);
#define     GetPrivateProfileStruct WINELIB_NAME_AW(GetPrivateProfileStruct)
FARPROC   WINAPI GetProcAddress(HMODULE,LPCSTR);
UINT      WINAPI GetProfileIntA(LPCSTR,LPCSTR,INT);
UINT      WINAPI GetProfileIntW(LPCWSTR,LPCWSTR,INT);
#define     GetProfileInt WINELIB_NAME_AW(GetProfileInt)
INT       WINAPI GetProfileSectionA(LPCSTR,LPSTR,DWORD);
INT       WINAPI GetProfileSectionW(LPCWSTR,LPWSTR,DWORD);
#define     GetProfileSection WINELIB_NAME_AW(GetProfileSection)
INT       WINAPI GetProfileStringA(LPCSTR,LPCSTR,LPCSTR,LPSTR,UINT);
INT       WINAPI GetProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,UINT);
#define     GetProfileString WINELIB_NAME_AW(GetProfileString)

BOOL      WINAPI GetQueuedCompletionStatus(HANDLE       CompletionPort,
                                           LPDWORD      lpNumberOfBytesTransferred,
                                           LPDWORD      lpCompletionKey,
                                           LPOVERLAPPED *lpOverlapped,
                                           DWORD        dwMilliseconds);
VOID        WINAPI GetStartupInfoA(LPSTARTUPINFOA);
VOID        WINAPI GetStartupInfoW(LPSTARTUPINFOW);
#define     GetStartupInfo WINELIB_NAME_AW(GetStartupInfo)
BOOL      WINAPI GetStringTypeA(LCID,DWORD,LPCSTR,INT,LPWORD);
BOOL      WINAPI GetStringTypeW(DWORD,LPCWSTR,INT,LPWORD);
#define     GetStringType WINELIB_NAME_AW(GetStringType)
UINT      WINAPI GetSystemDirectoryA(LPSTR,UINT);
UINT      WINAPI GetSystemDirectoryW(LPWSTR,UINT);
#define     GetSystemDirectory WINELIB_NAME_AW(GetSystemDirectory)
UINT      WINAPI GetTempFileNameA(LPCSTR,LPCSTR,UINT,LPSTR);
UINT      WINAPI GetTempFileNameW(LPCWSTR,LPCWSTR,UINT,LPWSTR);
#define     GetTempFileName WINELIB_NAME_AW(GetTempFileName)
UINT      WINAPI GetTempPathA(UINT,LPSTR);
UINT      WINAPI GetTempPathW(UINT,LPWSTR);
#define     GetTempPath WINELIB_NAME_AW(GetTempPath)
LONG        WINAPI GetVersion(void);
BOOL      WINAPI GetExitCodeProcess(HANDLE,LPDWORD);
BOOL      WINAPI GetVolumeInformationA(LPCSTR,LPSTR,DWORD,LPDWORD,LPDWORD,LPDWORD,LPSTR,DWORD);
BOOL      WINAPI GetVolumeInformationW(LPCWSTR,LPWSTR,DWORD,LPDWORD,LPDWORD,LPDWORD,LPWSTR,DWORD);
#define     GetVolumeInformation WINELIB_NAME_AW(GetVolumeInformation)
UINT      WINAPI GetWindowsDirectoryA(LPSTR,UINT);
UINT      WINAPI GetWindowsDirectoryW(LPWSTR,UINT);
#define     GetWindowsDirectory WINELIB_NAME_AW(GetWindowsDirectory)
HGLOBAL16   WINAPI GlobalAlloc16(UINT16,DWORD);
HGLOBAL   WINAPI GlobalAlloc(UINT,DWORD);
DWORD       WINAPI GlobalCompact(DWORD);
UINT      WINAPI GlobalFlags(HGLOBAL);
HGLOBAL16   WINAPI GlobalFree16(HGLOBAL16);
HGLOBAL   WINAPI GlobalFree(HGLOBAL);
HGLOBAL   WINAPI GlobalHandle(LPCVOID);
WORD        WINAPI GlobalFix16(HGLOBAL16);
VOID        WINAPI GlobalFix(HGLOBAL);
LPVOID      WINAPI GlobalLock16(HGLOBAL16);
LPVOID      WINAPI GlobalLock(HGLOBAL);
HGLOBAL   WINAPI GlobalReAlloc(HGLOBAL,DWORD,UINT);
DWORD       WINAPI GlobalSize16(HGLOBAL16);
DWORD       WINAPI GlobalSize(HGLOBAL);
VOID        WINAPI GlobalUnfix16(HGLOBAL16);
VOID        WINAPI GlobalUnfix(HGLOBAL);
BOOL16      WINAPI GlobalUnlock16(HGLOBAL16);
BOOL      WINAPI GlobalUnlock(HGLOBAL);
BOOL16      WINAPI GlobalUnWire16(HGLOBAL16);
BOOL      WINAPI GlobalUnWire(HGLOBAL);
SEGPTR      WINAPI GlobalWire16(HGLOBAL16);
LPVOID      WINAPI GlobalWire(HGLOBAL);
BOOL      WINAPI InitAtomTable(DWORD);
BOOL      WINAPI IsBadCodePtr(FARPROC);
BOOL      WINAPI IsBadHugeReadPtr(LPCVOID,UINT);
BOOL      WINAPI IsBadHugeWritePtr(LPVOID,UINT);
BOOL      WINAPI IsBadReadPtr(LPCVOID,UINT);
BOOL      WINAPI IsBadStringPtrA(LPCSTR,UINT);
BOOL      WINAPI IsBadStringPtrW(LPCWSTR,UINT);
#define     IsBadStringPtr WINELIB_NAME_AW(IsBadStringPtr)
BOOL      WINAPI IsBadWritePtr(LPVOID,UINT);
BOOL      WINAPI IsDBCSLeadByte(BYTE);
BOOL        WINAPI IsDebuggerPresent(void);
HINSTANCE16 WINAPI LoadLibrary16(LPCSTR);
HMODULE   WINAPI LoadLibraryA(LPCSTR);
HMODULE   WINAPI LoadLibraryW(LPCWSTR);
#define     LoadLibrary WINELIB_NAME_AW(LoadLibrary)
HMODULE   WINAPI LoadLibraryExA(LPCSTR,HFILE,DWORD);
HMODULE   WINAPI LoadLibraryExW(LPCWSTR,HFILE,DWORD);
#define     LoadLibraryEx WINELIB_NAME_AW(LoadLibraryEx)
HINSTANCE16 WINAPI LoadModule16(LPCSTR,LPVOID);
HINSTANCE WINAPI LoadModule(LPCSTR,LPVOID);
HGLOBAL   WINAPI LoadResource(HMODULE,HRSRC);
HLOCAL    WINAPI LocalAlloc(UINT,DWORD);
UINT      WINAPI LocalCompact(UINT);
UINT      WINAPI LocalFlags(HLOCAL);
HLOCAL    WINAPI LocalFree(HLOCAL);
HLOCAL    WINAPI LocalHandle(LPCVOID);
LPVOID      WINAPI LocalLock(HLOCAL);
HLOCAL    WINAPI LocalReAlloc(HLOCAL,DWORD,UINT);
UINT      WINAPI LocalShrink(HGLOBAL,UINT);
UINT      WINAPI LocalSize(HLOCAL);
BOOL      WINAPI LocalUnlock(HLOCAL);
LPVOID      WINAPI LockResource(HGLOBAL);
#define UnlockResource(hResData) ((hResData), 0)

#define     LockSegment(handle) GlobalFix((HANDLE)(handle))
#define     MakeProcInstance(proc,inst) (proc)
HFILE16     WINAPI OpenFile16(LPCSTR,OFSTRUCT*,UINT16);
HFILE     WINAPI OpenFile(LPCSTR,OFSTRUCT*,UINT);
VOID        WINAPI OutputDebugStringA(LPCSTR);
VOID        WINAPI OutputDebugStringW(LPCWSTR);
#define     OutputDebugString WINELIB_NAME_AW(OutputDebugString)
BOOL      WINAPI ReadProcessMemory(HANDLE, LPCVOID, LPVOID, DWORD, LPDWORD);
BOOL      WINAPI RemoveDirectoryA(LPCSTR);
BOOL      WINAPI RemoveDirectoryW(LPCWSTR);
#define     RemoveDirectory WINELIB_NAME_AW(RemoveDirectory)
BOOL      WINAPI SetCurrentDirectoryA(LPCSTR);
BOOL      WINAPI SetCurrentDirectoryW(LPCWSTR);
#define     SetCurrentDirectory WINELIB_NAME_AW(SetCurrentDirectory)
UINT      WINAPI SetErrorMode(UINT);
BOOL      WINAPI SetFileAttributesA(LPCSTR,DWORD);
BOOL      WINAPI SetFileAttributesW(LPCWSTR,DWORD);
#define     SetFileAttributes WINELIB_NAME_AW(SetFileAttributes)
UINT      WINAPI SetHandleCount(UINT);
#define     SetSwapAreaSize(w) (w)
BOOL        WINAPI SetVolumeLabelA(LPCSTR,LPCSTR);
BOOL        WINAPI SetVolumeLabelW(LPCWSTR,LPCWSTR);
#define     SetVolumeLabel WINELIB_NAME_AW(SetVolumeLabel)
BOOL        WINAPI SetWaitableTimer(HANDLE,const LARGE_INTEGER*,LONG,PTIMERAPCROUTINE,LPVOID,BOOL);
DWORD       WINAPI SizeofResource(HMODULE,HRSRC);
BOOL        WINAPI UnlockFileEx(HANDLE,DWORD,DWORD,DWORD,LPOVERLAPPED);
#define     UnlockSegment(handle) GlobalUnfix((HANDLE)(handle))
DWORD WINAPI VerLanguageNameA( UINT wLang, LPSTR szLang, UINT nSize );
DWORD WINAPI VerLanguageNameW( UINT wLang, LPWSTR szLang, UINT nSize );
#define     VerLanguageName WINELIB_NAME_AW(VerLanguageName)
BOOL      WINAPI WritePrivateProfileSectionA(LPCSTR,LPCSTR,LPCSTR);
BOOL      WINAPI WritePrivateProfileSectionW(LPCWSTR,LPCWSTR,LPCWSTR);
#define     WritePrivateProfileSection WINELIB_NAME_AW(WritePrivateProfileSection)
BOOL      WINAPI WritePrivateProfileStringA(LPCSTR,LPCSTR,LPCSTR,LPCSTR);
BOOL      WINAPI WritePrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR);
BOOL        WINAPI WriteProcessMemory(HANDLE,LPCVOID,LPVOID,DWORD,LPDWORD);
#define     WritePrivateProfileString WINELIB_NAME_AW(WritePrivateProfileString)
BOOL         WINAPI WriteProfileSectionA(LPCSTR,LPCSTR);
BOOL         WINAPI WriteProfileSectionW(LPCWSTR,LPCWSTR);
#define     WritePrivateProfileSection WINELIB_NAME_AW(WritePrivateProfileSection)
BOOL      WINAPI WritePrivateProfileStructA(LPCSTR,LPCSTR,LPVOID,UINT,LPCSTR);
BOOL      WINAPI WritePrivateProfileStructW(LPCWSTR,LPCWSTR,LPVOID,UINT,LPCWSTR);
#define     WritePrivateProfileStruct WINELIB_NAME_AW(WritePrivateProfileStruct)
BOOL      WINAPI WriteProfileStringA(LPCSTR,LPCSTR,LPCSTR);
BOOL      WINAPI WriteProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR);
#define     WriteProfileString WINELIB_NAME_AW(WriteProfileString)
#define     Yield32()
LPSTR       WINAPI lstrcatA(LPSTR,LPCSTR);
LPWSTR      WINAPI lstrcatW(LPWSTR,LPCWSTR);
#define     lstrcat WINELIB_NAME_AW(lstrcat)
LPSTR       WINAPI lstrcpyA(LPSTR,LPCSTR);
LPWSTR      WINAPI lstrcpyW(LPWSTR,LPCWSTR);
#define     lstrcpy WINELIB_NAME_AW(lstrcpy)
LPSTR       WINAPI lstrcpynA(LPSTR,LPCSTR,INT);
LPWSTR      WINAPI lstrcpynW(LPWSTR,LPCWSTR,INT);
#define     lstrcpyn WINELIB_NAME_AW(lstrcpyn)
INT       WINAPI lstrlenA(LPCSTR);
INT       WINAPI lstrlenW(LPCWSTR);
#define     lstrlen WINELIB_NAME_AW(lstrlen)
HINSTANCE WINAPI WinExec(LPCSTR,UINT);
LONG        WINAPI _hread(HFILE,LPVOID,LONG);
LONG        WINAPI _hwrite(HFILE,LPCSTR,LONG);
HFILE     WINAPI _lcreat(LPCSTR,INT);
HFILE     WINAPI _lclose(HFILE);
LONG        WINAPI _llseek(HFILE,LONG,INT);
HFILE     WINAPI _lopen(LPCSTR,INT);
UINT      WINAPI _lread(HFILE,LPVOID,UINT);
UINT      WINAPI _lwrite(HFILE,LPCSTR,UINT);
SEGPTR      WINAPI WIN16_GlobalLock16(HGLOBAL16);
INT       WINAPI lstrcmpA(LPCSTR,LPCSTR);
INT       WINAPI lstrcmpW(LPCWSTR,LPCWSTR);
#define     lstrcmp WINELIB_NAME_AW(lstrcmp)
INT       WINAPI lstrcmpiA(LPCSTR,LPCSTR);
INT       WINAPI lstrcmpiW(LPCWSTR,LPCWSTR);
#define     lstrcmpi WINELIB_NAME_AW(lstrcmpi)
int       WINAPI lstrncmpiA(LPCSTR, LPCSTR, size_t);
int       WINAPI lstrncmpiW(LPCWSTR, LPCWSTR, size_t);
#define     lstrncmpi WINELIB_NAME_AW(lstrncmpi)

VOID        WINAPI SetLastError(DWORD);


BOOL WINAPI SetTokenInformation(HANDLE                  hToken,
                                TOKEN_INFORMATION_CLASS tic,
                                LPVOID                  lpvInformation,
                                DWORD                   cbInformation);

BOOL WINAPI GetUserNameA(LPSTR lpBuffer, LPDWORD lpcchBuffer);
BOOL WINAPI GetUserNameW(LPWSTR lpBuffer, LPDWORD lpccBuffer);
#define     GetUserName WINELIB_NAME_AW(GetUserName)

BOOL WINAPI AbortSystemShutdownA(LPTSTR lpMachineName);
BOOL WINAPI AbortSystemShutdownW(LPWSTR lpMachineName);
#define     AbortSystemShutdown WINELIB_NAME_AW(AbortSystemShutdown)
BOOL WINAPI AccessCheckAndAuditAlarmA(LPCSTR              SubsystemName,
                                           LPVOID               HandleId,
                                           LPTSTR               ObjectTypeName,
                                           LPTSTR               ObjectName,
                                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                                           DWORD                DesiredAccess,
                                           PGENERIC_MAPPING     GenericMapping,
                                           BOOL                 ObjectCreation,
                                           LPDWORD              GrantedAccess,
                                           LPBOOL               AccessStatus,
                                           LPBOOL               pfGenerateOnClose);
BOOL WINAPI AccessCheckAndAuditAlarmW(LPCWSTR              SubsystemName,
                                           LPVOID               HandleId,
                                           LPWSTR               ObjectTypeName,
                                           LPWSTR               ObjectName,
                                           PSECURITY_DESCRIPTOR SecurityDescriptor,
                                           DWORD                DesiredAccess,
                                           PGENERIC_MAPPING     GenericMapping,
                                           BOOL                 ObjectCreation,
                                           LPDWORD              GrantedAccess,
                                           LPBOOL               AccessStatus,
                                           LPBOOL               pfGenerateOnClose);
#define     AccessCheckAndAuditAlarm WINELIB_NAME_AW(AccessCheckAndAuditAlarm)
BOOL WINAPI AddAccessAllowedAce(PACL  pAcl,
                                     DWORD dwAceRevision,
                                     DWORD AccessMask,
                                     PSID  pSid);
BOOL WINAPI AddAccessDeniedAce(PACL  pAcl,
                                    DWORD dwAceRevision,
                                    DWORD AccessMask,
                                    PSID  pSid);
BOOL WINAPI AddAce(PACL   pAcl,
                        DWORD  dwAceRevision,
                        DWORD  dwStartingAceIndex,
                        LPVOID pAceList,
                        DWORD  nAceListLength);
BOOL WINAPI AddAuditAccessAce(PACL  pAcl,
                                   DWORD dwAceRevision,
                                   DWORD dwAccessMask,
                                   PSID  pSid,
                                   BOOL  bAuditSuccess,
                                   BOOL  bAuditFailure);
BOOL WINAPI AdjustTokenGroups(HANDLE         TokenHandle,
                                   BOOL           ResetToDefault,
                                   PTOKEN_GROUPS  NewState,
                                   DWORD          BufferLength,
                                   PTOKEN_GROUPS  PreviousState,
                                   LPDWORD         ReturnLength);
BOOL WINAPI AllocateLocallyUniqueId(PLUID  Luid);
BOOL WINAPI AreAllAccessesGranted(DWORD  GrantedAccess,
                                       DWORD  DesiredAccess);
BOOL WINAPI AreAnyAccessesGranted(DWORD  GrantedAccess,
                                       DWORD  DesiredAccess);
BOOL WINAPI CreatePrivateObjectSecurity(PSECURITY_DESCRIPTOR  ParentDescriptor,
                                             PSECURITY_DESCRIPTOR  CreatorDescriptor,
                                             PSECURITY_DESCRIPTOR *NewDescriptor,
                                             BOOL                  IsDirectoryObject,
                                             HANDLE                Token,
                                             PGENERIC_MAPPING      GenericMapping);
BOOL WINAPI CreateProcessAsUserA(HANDLE                 hToken,
                                      LPCSTR                lpApplicationName,
                                      LPTSTR                 lpCommandLine,
                                      LPSECURITY_ATTRIBUTES  lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES  lpThreadAttributes,
                                      BOOL                   bInheritHandles,
                                      DWORD                  dwCreationFlags,
                                      LPVOID                 lpEnvironment,
                                      LPCSTR                 lpCurrentDirectory,
                                      LPSTARTUPINFOA         lpStartupInfo,
                                      LPPROCESS_INFORMATION  lpProcessInformation);
BOOL WINAPI CreateProcessAsUserW(HANDLE                 hToken,
                                      LPCWSTR                lpApplicationName,
                                      LPWSTR                 lpCommandLine,
                                      LPSECURITY_ATTRIBUTES  lpProcessAttributes,
                                      LPSECURITY_ATTRIBUTES  lpThreadAttributes,
                                      BOOL                   bInheritHandles,
                                      DWORD                  dwCreationFlags,
                                      LPVOID                 lpEnvironment,
                                      LPCWSTR                lpCurrentDirectory,
                                      LPSTARTUPINFOA         lpStartupInfo,
                                      LPPROCESS_INFORMATION  lpProcessInformation);
#define     CreateProcessAsUser WINELIB_NAME_AW(CreateProcessAsUser)
BOOL WINAPI DeleteAce(PACL  pAcl,
                           DWORD dwAceIndex);
BOOL WINAPI DestroyPrivateObjectSecurity(PSECURITY_DESCRIPTOR *ObjectDescriptor);
BOOL WINAPI DuplicateToken(HANDLE                       ExistingTokenHandle,
                                SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
                                PHANDLE                      DuplicateTokenHandle);
BOOL WINAPI FindFirstFreeAce(PACL pAcl,
                                  LPVOID *pAce);
BOOL WINAPI GetAce(PACL   pAcl,
                        DWORD  dwAceIndex,
                        LPVOID *pAce);
BOOL WINAPI GetAclInformation(PACL                  pAcl,
                                   LPVOID                pAclInformation,
                                   DWORD                 nAclInformationLength,
                                   ACL_INFORMATION_CLASS dwAclInformationClass);
BOOL WINAPI GetKernelObjectSecurity(HANDLE               Handle,
                                         SECURITY_INFORMATION RequestedInformation,
                                         PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                         DWORD                nLength,
                                         LPDWORD              lpnLengthNeeded);
BOOL WINAPI GetPrivateObjectSecurity(PSECURITY_DESCRIPTOR ObjectDescriptor,
                                          SECURITY_INFORMATION SecurityInformation,
                                          PSECURITY_DESCRIPTOR ResultantDescriptor,
                                          DWORD                DescriptorLength,
                                          LPDWORD               ReturnLength);
BOOL WINAPI ImpersonateLoggedOnUser(HANDLE hToken);
BOOL WINAPI ImpersonateNamedPipeClient(HANDLE hNamedPipe);
BOOL WINAPI InitiateSystemShutdownA(LPTSTR lpMachineName,
                                         LPTSTR lpMessage,
                                         DWORD  dwTimeout,
                                         BOOL   bForceAppsClosed,
                                         BOOL   bRebootAfterShutdown);
BOOL WINAPI InitiateSystemShutdownW(LPWSTR lpMachineName,
                                         LPWSTR lpMessage,
                                         DWORD  dwTimeout,
                                         BOOL   bForceAppsClosed,
                                         BOOL   bRebootAfterShutdown);
#define     InitiateSystemShutdown WINELIB_NAME_AW(InitiateSystemShutdown)
BOOL WINAPI IsValidAcl(PACL pAcl);
BOOL WINAPI LogonUserA(LPTSTR  lpszUsername,
                            LPTSTR  lpszDomain,
                            LPTSTR  lpszPassword,
                            DWORD   dwLogonType,
                            DWORD   dwLogonProvider,
                            PHANDLE phToken);
BOOL WINAPI LogonUserW(LPWSTR  lpszUsername,
                            LPWSTR  lpszDomain,
                            LPWSTR  lpszPassword,
                            DWORD   dwLogonType,
                            DWORD   dwLogonProvider,
                            PHANDLE phToken);
#define     LogonUser WINELIB_NAME_AW(LogonUser)
BOOL WINAPI IsValidSid(PSID pSid);
BOOL WINAPI EqualSid(PSID pSid1,
                          PSID pSid2);
BOOL WINAPI EqualPrefixSid(PSID pSid1,
                                PSID pSid2);
DWORD  WINAPI GetSidLengthRequired(BYTE nSubAuthorityCount);
BOOL WINAPI AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                                          BYTE nSubAuthorityCount,
                                          DWORD nSubAuthority0,
                                          DWORD nSubAuthority1,
                                          DWORD nSubAuthority2,
                                          DWORD nSubAuthority3,
                                          DWORD nSubAuthority4,
                                          DWORD nSubAuthority5,
                                          DWORD nSubAuthority6,
                                          DWORD nSubAuthority7,
                                          PSID* pSid);
VOID* WINAPI FreeSid(PSID pSid);
BOOL WINAPI InitializeSecurityDescriptor(PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                              DWORD dwRevision);
BOOL WINAPI InitializeSid(PSID Sid,
                               PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                               BYTE nSubAuthorityCount);
DWORD* WINAPI GetSidSubAuthority(PSID pSid, DWORD nSubAuthority);
BYTE * WINAPI GetSidSubAuthorityCount(PSID pSid);
DWORD  WINAPI GetLengthSid(PSID pSid);
BOOL WINAPI CopySid(DWORD nDestinationSidLength,
                         PSID pDestinationSid,
                         PSID pSourceSid);
BOOL WINAPI LookupAccountSidA(LPCSTR lpSystemName,
                                   PSID Sid,
                                   LPSTR Name,
                                   LPDWORD cchName,
                                   LPSTR ReferencedDomainName,
                                   LPDWORD cchReferencedDomainName,
                                   PSID_NAME_USE peUse);
BOOL WINAPI LookupAccountSidW(LPCWSTR lpSystemName,
                                   PSID Sid,
                                   LPWSTR Name,
                                   LPDWORD cchName,
                                   LPWSTR ReferencedDomainName,
                                   LPDWORD cchReferencedDomainName,
                                   PSID_NAME_USE peUse);
#define     LookupAccountSid WINELIB_NAME_AW(LookupAccountSid)
PSID_IDENTIFIER_AUTHORITY WINAPI GetSidIdentifierAuthority(PSID pSid);
BOOL WINAPI LookupAccountNameA(LPCSTR       lpSystemName,
                                    LPCSTR       lpAccountName,
                                    PSID          Sid,
                                    LPDWORD       cbSid,
                                    LPTSTR        ReferencedDomainName,
                                    LPDWORD       cbReferencedDomainName,
                                    PSID_NAME_USE peUse);
BOOL WINAPI LookupAccountNameW(LPCWSTR       lpSystemName,
                                    LPCWSTR       lpAccountName,
                                    PSID          Sid,
                                    LPDWORD       cbSid,
                                    LPWSTR        ReferencedDomainName,
                                    LPDWORD       cbReferencedDomainName,
                                    PSID_NAME_USE peUse);
#define     LookupAccountName WINELIB_NAME_AW(LookupAccountName)
BOOL WINAPI LookupPrivilegeDisplayNameA(LPCSTR lpSystemName,
                                             LPCSTR lpName,
                                             LPTSTR  lpDisplayName,
                                             LPDWORD cbDisplayName,
                                             LPDWORD lpLanguageId);
BOOL WINAPI LookupPrivilegeDisplayNameW(LPCWSTR lpSystemName,
                                             LPCWSTR lpName,
                                             LPWSTR  lpDisplayName,
                                             LPDWORD cbDisplayName,
                                             LPDWORD lpLanguageId);
#define     LookupPrivilegeDisplayName WINELIB_NAME_AW(LookupPrivilegeDisplayName)
BOOL WINAPI LookupPrivilegeNameA(LPCSTR lpSystemName,
                                      PLUID   lpLuid,
                                      LPTSTR  lpName,
                                      LPDWORD cbName);
BOOL WINAPI LookupPrivilegeNameW(LPCWSTR lpSystemName,
                                      PLUID   lpLuid,
                                      LPWSTR  lpName,
                                      LPDWORD cbName);
#define     LookupPrivilegeName WINELIB_NAME_AW(LookupPrivilegeName)
BOOL WINAPI MakeAbsoluteSD(PSECURITY_DESCRIPTOR  pSelfRelativeSecurityDescriptor,
                                PSECURITY_DESCRIPTOR  pAbsoluteSecurityDescriptor,
                                LPDWORD               lpdwAbsoluteSecurityDescriptorSize,
                                PACL                  pDacl,
                                LPDWORD               lpdwDaclSize,
                                PACL                  pSacl,
                                LPDWORD               lpdwSaclSize,
                                PSID                  pOwner,
                                LPDWORD               lpdwOwnerSize,
                                PSID                  pPrimaryGroup,
                                LPDWORD               lpdwPrimaryGroupSize);
VOID WINAPI MapGenericMask(LPDWORD           AccessMask,
                                PGENERIC_MAPPING GenericMapping);
BOOL WINAPI ObjectCloseAuditAlarmA(LPCSTR SubsystemName,
                                        LPVOID  HandleId,
                                        BOOL    GenerateOnClose);
BOOL WINAPI ObjectCloseAuditAlarmW(LPCWSTR SubsystemName,
                                        LPVOID  HandleId,
                                        BOOL    GenerateOnClose);
#define     ObjectCloseAuditAlarm WINELIB_NAME_AW(ObjectCloseAuditAlarm)
BOOL WINAPI ObjectOpenAuditAlarmA(LPCSTR              SubsystemName,
                                       LPVOID               HandleId,
                                       LPTSTR               ObjectTypeName,
                                       LPTSTR               ObjectName,
                                       PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                       HANDLE               ClientToken,
                                       DWORD                DesiredAccess,
                                       DWORD                GrantedAccess,
                                       PPRIVILEGE_SET       Privileges,
                                       BOOL                 ObjectCreation,
                                       BOOL                 AccessGranted,
                                       LPBOOL               GenerateOnClose);
BOOL WINAPI ObjectOpenAuditAlarmW(LPCWSTR              SubsystemName,
                                       LPVOID               HandleId,
                                       LPWSTR               ObjectTypeName,
                                       LPWSTR               ObjectName,
                                       PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                       HANDLE               ClientToken,
                                       DWORD                DesiredAccess,
                                       DWORD                GrantedAccess,
                                       PPRIVILEGE_SET       Privileges,
                                       BOOL                 ObjectCreation,
                                       BOOL                 AccessGranted,
                                       LPBOOL               GenerateOnClose);
#define     ObjectOpenAuditAlarm WINELIB_NAME_AW(ObjectOpenAuditAlarm)
BOOL WINAPI ObjectPrivilegeAuditAlarmA(LPCSTR        lpszSubsystem,
                                            LPVOID         lpvHandleId,
                                            HANDLE         hClientToken,
                                            DWORD          dwDesiredAccess,
                                            PPRIVILEGE_SET pps,
                                            BOOL           fAccessGranted);
BOOL WINAPI ObjectPrivilegeAuditAlarmW(LPCWSTR        lpszSubsystem,
                                            LPVOID         lpvHandleId,
                                            HANDLE         hClientToken,
                                            DWORD          dwDesiredAccess,
                                            PPRIVILEGE_SET pps,
                                            BOOL           fAccessGranted);
#define     ObjectPrivilegeAuditAlarm WINELIB_NAME_AW(ObjectPrivilegeAuditAlarm)
BOOL WINAPI PrivilegeCheck(HANDLE         hClientToken,
                                PPRIVILEGE_SET pps,
                                LPBOOL         lpfResult);
BOOL WINAPI PrivilegedServiceAuditAlarmA(LPCSTR        lpszSubsystem,
                                              LPCSTR        lpszService,
                                              HANDLE         hClientToken,
                                              PPRIVILEGE_SET pps,
                                              BOOL           fAccessGranted);
BOOL WINAPI PrivilegedServiceAuditAlarmW(LPCWSTR        lpszSubsystem,
                                              LPCWSTR        lpszService,
                                              HANDLE         hClientToken,
                                              PPRIVILEGE_SET pps,
                                              BOOL           fAccessGranted);
#define     PrivilegedServiceAuditAlarm WINELIB_NAME_AW(PrivilegedServiceAuditAlarm)
BOOL WINAPI SetAclInformation(PACL                  pAcl,
                                   LPVOID                lpvAclInfo,
                                   DWORD                 cbAclInfo,
                                   ACL_INFORMATION_CLASS aclic);
BOOL WINAPI SetKernelObjectSecurity(HANDLE               hObject,
                                         SECURITY_INFORMATION si,
                                         PSECURITY_DESCRIPTOR psd);
BOOL WINAPI    SetPrivateObjectSecurity(SECURITY_INFORMATION  si,
                                          PSECURITY_DESCRIPTOR  psdSource,
                                          PSECURITY_DESCRIPTOR  *lppsdTarget,
                                          PGENERIC_MAPPING      pgm,
                                          HANDLE                hClientToken);


HANDLE WINAPI CreateNamedPipeA(LPCSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                               DWORD nMaxInstances, DWORD nOutBufferSize,
                               DWORD nInBufferSize, DWORD nDefaultTimeOut,
                               LPSECURITY_ATTRIBUTES lpSecurityAttributes);
HANDLE WINAPI CreateNamedPipeW(LPCWSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode,
                               DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize,
                               DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
#define     CreateNamedPipe WINELIB_NAME_AW(CreateNamedPipe)

BOOL WINAPI GetNamedPipeHandleStateA(HANDLE hNamedPipe, LPDWORD lpState,
                                     LPDWORD lpCurInstances, LPDWORD lpMaxCollectionCount,
                                     LPDWORD lpCollectDataTimeout, LPSTR lpUserName,
                                     DWORD nMaxUserNameSize);
BOOL WINAPI GetNamedPipeHandleStateW(HANDLE hNamedPipe,LPDWORD lpState, LPDWORD lpCurInstances,
                                     LPDWORD lpMaxCollectionCount, LPDWORD lpCollectDataTimeout,
                                     LPWSTR lpUserName, DWORD nMaxUserNameSize);
#define     GetNamedPipeHandleState WINELIB_NAME_AW(GetNamedPipeHandleState)

BOOL WINAPI CallNamedPipeA(LPCSTR lpNamedPipeName, LPVOID lpInBuffer, DWORD nInBufferSize,
                           LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesRead,
                           DWORD nTimeOut);
BOOL WINAPI CallNamedPipeW(LPCWSTR lpNamedPipeName, LPVOID lpInBuffer, DWORD nInBufferSize,
                           LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesRead,
                           DWORD nTimeOut);
#define     CallNamedPipe WINELIB_NAME_AW(CallNamedPipe)


BOOL WINAPI WaitNamedPipeA(LPCSTR lpNamedPipeName, DWORD nTimeOut);
BOOL WINAPI WaitNamedPipeW(LPCWSTR lpNamedPipeName, DWORD nTimeOut);

#define     WaitNamedPipe WINELIB_NAME_AW(WaitNamedPipe)

BOOL WINAPI CreatePipe(PHANDLE hReadPipe, PHANDLE hWritePipe, LPSECURITY_ATTRIBUTES lpPipeAttributes,
                       DWORD nSize);
BOOL WINAPI ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped);
BOOL WINAPI DisconnectNamedPipe(HANDLE hNamedPipe);
BOOL WINAPI SetNamedPipeHandleState(HANDLE hNamedPipe, LPDWORD lpMode,
                                    LPDWORD lpMaxCollectionCount, LPDWORD lpCollectDataTimeout);
BOOL WINAPI GetNamedPipeInfo(HANDLE hNamedPipe, LPDWORD lpFlags, LPDWORD lpOutBufferSize,
                             LPDWORD lpInBufferSize, LPDWORD lpMaxInstances);
BOOL WINAPI PeekNamedPipe(HANDLE hNamedPipe, LPVOID lpBuffer, DWORD nBufferSize,
                          LPDWORD lpBytesRead, LPDWORD lpTotalBytesAvail,
                          LPDWORD lpBytesLeftThisMessage);
BOOL WINAPI TransactNamedPipe(HANDLE hNamedPipe, LPVOID lpInBuffer, DWORD nInBufferSize,
                              LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesRead,
                              LPOVERLAPPED lpOverlapped);

HANDLE WINAPI CreateMailslotA(LPCSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes);
HANDLE WINAPI CreateMailslotW(LPCWSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes);
#define     CreateMailslot WINELIB_NAME_AW(WaitNamedPipe)

BOOL WINAPI GetMailslotInfo(HANDLE hMailslot, LPDWORD lpMaxMessageSize,
                            LPDWORD lpNextSize, LPDWORD lpMessageCount,
                            LPDWORD lpReadTimeout);

BOOL WINAPI SetMailslotInfo(HANDLE hMailslot, DWORD lReadTimeout);

/* undocumented functions */

typedef struct tagSYSLEVEL
{
    CRITICAL_SECTION crst;
    INT              level;
} SYSLEVEL;

/* [GS]etProcessDword offsets */
#define  GPD_APP_COMPAT_FLAGS    (-56)
#define  GPD_LOAD_DONE_EVENT     (-52)
#define  GPD_HINSTANCE16         (-48)
#define  GPD_WINDOWS_VERSION     (-44)
#define  GPD_THDB                (-40)
#define  GPD_PDB                 (-36)
#define  GPD_STARTF_SHELLDATA    (-32)
#define  GPD_STARTF_HOTKEY       (-28)
#define  GPD_STARTF_SHOWWINDOW   (-24)
#define  GPD_STARTF_SIZE         (-20)
#define  GPD_STARTF_POSITION     (-16)
#define  GPD_STARTF_FLAGS        (-12)
#define  GPD_PARENT              (- 8)
#define  GPD_FLAGS               (- 4)
#define  GPD_USERDATA            (  0)

void        WINAPI DisposeLZ32Handle(HANDLE);
HANDLE      WINAPI DosFileHandleToWin32Handle(HFILE);
DWORD       WINAPI GetProcessDword(DWORD,INT);
VOID        WINAPI GetpWin16Lock(SYSLEVEL**);
DWORD       WINAPI MapLS(LPCVOID);
DWORD       WINAPI MapProcessHandle(HANDLE);
LPVOID      WINAPI MapSL(DWORD);
VOID        WINAPI ReleaseThunkLock(DWORD*);
VOID        WINAPI RestoreThunkLock(DWORD);
void        WINAPI SetProcessDword(DWORD,INT,DWORD);
VOID        WINAPI UnMapLS(DWORD);
HFILE       WINAPI Win32HandleToDosFileHandle(HANDLE);
VOID        WINAPI _CheckNotSysLevel(SYSLEVEL *lock);
DWORD       WINAPI _ConfirmWin16Lock(void);
DWORD       WINAPI _ConfirmSysLevel(SYSLEVEL*);
VOID        WINAPI _EnterSysLevel(SYSLEVEL*);
VOID        WINAPI _LeaveSysLevel(SYSLEVEL*);

#ifdef __WIN32OS2__
HANDLE WINAPI FindFirstVolumeA(LPTSTR lpszVolumeName, DWORD cchBufferLength);
HANDLE WINAPI FindFirstVolumeW(LPWSTR lpszVolumeName, DWORD cchBufferLength);
#define     FindFirstVolume WINELIB_NAME_AW(FindFirstVolume)

BOOL   WINAPI FindNextVolumeA(HANDLE hFindVolume, LPTSTR lpszVolumeName,
                              DWORD cchBufferLength);
BOOL   WINAPI FindNextVolumeW(HANDLE hFindVolume, LPWSTR lpszVolumeName,
                              DWORD cchBufferLength);
#define     FindNextVolume WINELIB_NAME_AW(FindNextVolume)

BOOL   WINAPI FindVolumeClose(HANDLE hFindVolume);

HANDLE WINAPI FindFirstVolumeMountPointA(LPTSTR lpszRootPathName,
                                         LPTSTR lpszVolumeMountPoint,
                                         DWORD cchBufferLength);
HANDLE WINAPI FindFirstVolumeMountPointW(LPWSTR lpszRootPathName,
                                         LPWSTR lpszVolumeMountPoint,
                                         DWORD cchBufferLength);
#define     FindFirstVolumeMountPoint WINELIB_NAME_AW(FindFirstVolumeMountPoint)

BOOL WINAPI FindNextVolumeMountPointA(HANDLE hFindVolumeMountPoint,
                                      LPTSTR lpszVolumeMountPoint,
                                      DWORD cchBufferLength);
BOOL WINAPI FindNextVolumeMountPointW(HANDLE hFindVolumeMountPoint,
                                      LPWSTR lpszVolumeMountPoint,
                                      DWORD cchBufferLength);
#define     FindNextVolumeMountPoint WINELIB_NAME_AW(FindNextVolumeMountPoint)

BOOL WINAPI FindVolumeMountPointClose(HANDLE hFindVolumeMountPoint);

BOOL WINAPI GetVolumeNameForVolumeMountPointA(LPCSTR lpszVolumeMountPoint,
                                              LPSTR lpszVolumeName,
                                              DWORD cchBufferLength);
BOOL WINAPI GetVolumeNameForVolumeMountPointW(LPCWSTR lpszVolumeMountPoint,
                                              LPWSTR lpszVolumeName,
                                              DWORD cchBufferLength);
#define     GetVolumeNameForVolumeMountPoint WINELIB_NAME_AW(GetVolumeNameForVolumeMountPoint)

BOOL WINAPI GetVolumePathNameA(LPCSTR,LPSTR,DWORD);
BOOL WINAPI GetVolumePathNameW(LPCWSTR,LPWSTR,DWORD);
#define     GetVolumePathName WINELIB_NAME_AW(GetVolumePathName)

#endif

#ifdef __cplusplus
}
#endif

#endif  /* __WINE_WINBASE_H */
