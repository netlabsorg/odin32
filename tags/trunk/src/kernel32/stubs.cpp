/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 KERNEL32 Subsystem for OS/2
 *
 * 1998/05/19 PH Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) Stubs.H 1.0.0 1998/05/19 PH start
 */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/
#include <os2win.h>
#include <winnls.h>
#include "unicode.h"
#include <builtin.h>
#include "handlemanager.h"

#include "stubs.h"


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


// For DefineDosDevice

#define DDD_RAW_TARGET_PATH         0x00000001
#define DDD_REMOVE_DEFINITION       0x00000002
#define DDD_EXACT_MATCH_ON_REMOVE   0x00000004
#define DDD_NO_BROADCAST_SYSTEM     0x00000008


// For Backup functions

//
//  Stream Ids
//

#define BACKUP_INVALID          0x00000000
#define BACKUP_DATA             0x00000001
#define BACKUP_EA_DATA          0x00000002
#define BACKUP_SECURITY_DATA    0x00000003
#define BACKUP_ALTERNATE_DATA   0x00000004
#define BACKUP_LINK             0x00000005
#define BACKUP_PROPERTY_DATA    0x00000006

//
//  Stream Attributes
//

#define STREAM_NORMAL_ATTRIBUTE         0x00000000
#define STREAM_MODIFIED_WHEN_READ       0x00000001
#define STREAM_CONTAINS_SECURITY        0x00000002
#define STREAM_CONTAINS_PROPERTIES      0x00000004

// Named Pipes

#define NMPWAIT_WAIT_FOREVER            0xffffffff
#define NMPWAIT_NOWAIT                  0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT        0x00000000


// Locale
#define LCID DWORD

/*
 *  Locale Dependent Mapping Flags.
 */

#define LCMAP_LOWERCASE           0x00000100  /* lower case letters */
#define LCMAP_UPPERCASE           0x00000200  /* upper case letters */
#define LCMAP_SORTKEY             0x00000400  /* WC sort key (normalize) */
#define LCMAP_BYTEREV             0x00000800  /* byte reversal */

#define LCMAP_HIRAGANA            0x00100000  /* map katakana to hiragana */
#define LCMAP_KATAKANA            0x00200000  /* map hiragana to katakana */
#define LCMAP_HALFWIDTH           0x00400000  /* map double byte to single byte */
#define LCMAP_FULLWIDTH           0x00800000  /* map single byte to double byte */

#define LCMAP_LINGUISTIC_CASING   0x01000000  /* use linguistic rules for casing */

#define LCMAP_SIMPLIFIED_CHINESE  0x02000000  /* map traditional chinese to simplified chinese */
#define LCMAP_TRADITIONAL_CHINESE 0x04000000  /* map simplified chinese to traditional chinese */



/*
 *  Locale Enumeration Flags.
 */
#define LCID_INSTALLED            0x00000001  /* installed locale ids */
#define LCID_SUPPORTED            0x00000002  /* supported locale ids */



// For FindFirstFileEx

#define FIND_FIRST_EX_CASE_SENSITIVE   0x00000001

/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

 // For FindFirstFileEx

typedef enum _FINDEX_INFO_LEVELS {
    FindExInfoStandard,
    FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;

typedef enum _FINDEX_SEARCH_OPS {
    FindExSearchNameMatch,
    FindExSearchLimitToDirectories,
    FindExSearchLimitToDevices,
    FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

 // For Backup funtions

typedef struct _WIN32_STREAM_ID {

    DWORD dwStreamId;
    DWORD dwStreamAttributes;
    LARGE_INTEGER Size;
    DWORD dwStreamNameSize;
//    WCHAR  cStreamName[ ] ; /* @@@PH */
    WCHAR  cStreamName[1] ;
} WIN32_STREAM_ID;


//
//  File structures
//

//typedef struct _OVERLAPPED {
//    DWORD   Internal;
//    DWORD   InternalHigh;
//    DWORD   Offset;
//    DWORD   OffsetHigh;
//    HANDLE  hEvent;
//} OVERLAPPED, *LPOVERLAPPED;

//typedef struct _SECURITY_ATTRIBUTES {
//    DWORD nLength;
//    LPVOID lpSecurityDescriptor;
//    BOOL bInheritHandle;
//} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

//typedef struct _PROCESS_INFORMATION {
//    HANDLE hProcess;
//    HANDLE hThread;
//    DWORD dwProcessId;
//    DWORD dwThreadId;
//} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

// For Fiber functions

typedef VOID (WINAPI *PFIBER_START_ROUTINE)(
    LPVOID lpFiberParameter
    );
typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;

// For Enum CalendarInfo & EnumSystemCodePages see winnls.h

/*
 *  Calendar type constant.
 */
typedef DWORD CALTYPE;

/*
 *  Calendar ID.
 */
typedef DWORD CALID;


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/


/*****************************************************************************
 * Name      : BOOL WIN32API AreFileApisANSI (VOID)
 * Purpose   : The AreFileApisANSI function determines whether a set of Win32
 *             file functions is using the ANSI or OEM character set code page.
 *             This function is useful for 8-bit console input and output
 *             operations.
 * Parameters: NONE
 * Variables :
 * Result    : If the set of Win32 file functions is using the ANSI code page,
 *             the return value is nonzero.
 *             If the set of Win32 file functions is using the OEM code page,
 *             the return value is zero.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/02/19 11:46]
 *****************************************************************************/

BOOL WIN32API AreFileApisANSI (VOID)
{

  dprintf(("KERNEL32:AreFileApisANSI() not implemented - TRUE\n"
          ));

  return (TRUE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API BackupRead(
 * Purpose   : The BackupRead function reads data associated with a specified
 *             file or directory into a buffer. You use this function to back
 *             up a file or directory.
 * Parameters: HANDLE hFile                handle to file or directory
 *             LPBYTE lpBuffer             pointer to buffer to read to
 *             DWORD nNumberOfBytesToRead  number of bytes to read
 *             LPDWORD lpNumberOfBytesRead pointer to variable to receive
 *                                         number of bytes read
 *             BOOL bAbort                 termination type
 *             BOOL bProcessSecurity       process security flag
 *             LPVOID *lpContext           pointer to pointer to internal
 *                                         context information
 * Variables :
 * Result    : 0 on Error Nonzero if OK
 * Remark    : BackupRead processes all of the data pertaining to an opened
 *             object as a series of discrete byte streams. Each stream is
 *             preceded by a 32-bit aligned WIN32_STREAM_ID structure.
 *             Streams must be processed in the same order in which they were
 *             written to the tape. This ordering enables applications to
 *             compare the data backed up against the data on the source device.
 *             The data returned by BackupRead is to be used only as input to
 *             the BackupWrite function. This data is returned as one large data
 *             stream divided into substreams. The substreams are separated
 *             by WIN32_STREAM_ID headers.
 *
 *             If an error occurs while BackupRead is reading, the calling
 *             process can skip the bad data by calling the BackupSeek function.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API BackupRead(
    HANDLE hFile,   // handle to file or directory
    LPBYTE lpBuffer,    // pointer to buffer to read to
    DWORD nNumberOfBytesToRead, // number of bytes to read
    LPDWORD lpNumberOfBytesRead,    // pointer to variable to receive number of bytes read
    BOOL bAbort,    // termination type
    BOOL bProcessSecurity,  // process security flag
    LPVOID *lpContext   // pointer to pointer to internal context information
)
{

  dprintf(("KERNEL32:BackupRead(%08x,%08x,%08x,%08x,%08x,%08x,%08x) not implemented\n",
          hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead,
          bAbort, bProcessSecurity, lpContext
          ));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API BackupSeek()
 * Purpose   : The BackupSeek function seeks forward in a data stream initially
 *             accessed by using the BackupRead or BackupWrite function.
 * Parameters: HANDLE hFile               handle to open file
 *             DWORD dwLowBytesToSeek     low-order 32 bits of number of bytes
 *             DWORD dwHighBytesToSeek    high-order 32 bits of number of bytes
 *             LPDWORD lpdwLowByteSeeked  pointer to number of bytes function seeks
 *             LPDWORD lpdwHighByteSeeked pointer to number of bytes function seeks
 *             LPVOID *lpContext      pointer to internal context information
 *
 * Variables :
 * Result    : If the function could seek the requested amount, the function
 *             returns nonzero.
 *             If the function could not seek the requested amount, the function
 *             returns zero.
 * Remark    : Applications use the BackUpSeek function to skip portions of a
 *             data stream that cause errors. This function does not seek across
 *             stream headers. If an application attempts to seek past the end
 *             of a substream, the function fails, the lpdwLowByteSeeked and
 *             lpdwHighByteSeeked parameters indicate the actual number of bytes
 *             the function seeks, and the file position is placed at the start
 *             of the next stream header.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API BackupSeek(  HANDLE hFile, DWORD dwLowBytesToSeek,
                              DWORD dwHighBytesToSeek,
                              LPDWORD lpdwLowByteSeeked,
                              LPDWORD lpdwHighByteSeeked,LPVOID *lpContext)
{

  dprintf(("KERNEL32:BackupSeek(%08x,%08x,%08x,%08x,%08x,08x) not implemented\n",
           hFile, dwLowBytesToSeek,dwHighBytesToSeek,
           lpdwLowByteSeeked, lpdwHighByteSeeked, lpContext));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API BackupWrite
 * Purpose   : The BackupWrite function writes a stream of data from a buffer to
 *             a specified file or directory. The data must be divided into
 *             substreams separated by WIN32_STREAM_ID structures. You use this
 *             function to restore a file or directory that has been backed up.
 * Parameters: HANDLE hFile                   handle to file or directory
 *             LPBYTE lpBuffer                pointer to buffer containing data
 *                                            to write
 *             DWORD nNumberOfBytesToWrite    number of bytes to write
 *             LPDWORD lpNumberOfBytesWritten pointer to variable to receive
 *                                            number of bytes written
 *             BOOL bAbort                    termination type
 *             BOOL bProcessSecurity          process security
 *             LPVOID *lpContext          pointer to pointer to internal
 *                                            context information
 *
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero,
 *             indicating that an I/O error occurred.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API BackupWrite( HANDLE hFile, LPBYTE lpBuffer,
                               DWORD nNumberOfBytesToWrite,
                               LPDWORD lpNumberOfBytesWritten,
                               BOOL bAbort, BOOL bProcessSecurity,
                               LPVOID *lpContext)
{

  dprintf(("KERNEL32:BackupWrite(%08x,%0x8,%08x,%08x,%08x,%08x,%08x) not implemented\n",
          hFile, lpBuffer, nNumberOfBytesToWrite,
          lpNumberOfBytesWritten, bAbort,  bProcessSecurity, lpContext));

  return (FALSE);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API BeginUpdateResourceA
 * Purpose   : The BeginUpdateResource function returns a handle that can be
 *             used by the UpdateResource function to add, delete, or replace
 *             resources in an executable file.
 * Parameters: LPCSTR pFileName               pointer to file in which to
 *                                            update resources
 *             BOOL bDeleteExistingResources  deletion option
 *
 * Variables :
 * Result    : If the function succeeds, the return value is a handle that can
 *             be used by the UpdateResource and EndUpdateResource functions.
 *             The return value is NULL if the specified file is not an
 *             executable file, the executable file is already loaded,
 *             the file does not exist, or the file cannot be opened for writing.
 *             To get extended error information, call GetLastError
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

HANDLE WIN32API BeginUpdateResourceA( LPCSTR pFileName,
                                       BOOL bDeleteExistingResources)
{

  dprintf(("KERNEL32:  BeginUpdateResource(%08x,%08x) not implemented\n",
           pFileName, bDeleteExistingResources
          ));

  return (NULL);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API BeginUpdateResourceW
 * Purpose   : The BeginUpdateResource function returns a handle that can be
 *             used by the UpdateResource function to add, delete, or replace
 *             resources in an executable file.
 * Parameters: LPCWSTR pFileName              pointer to file in which to
 *                                            update resources
 *             BOOL bDeleteExistingResources  deletion option
 *
 * Variables :
 * Result    : If the function succeeds, the return value is a handle that can
 *             be used by the UpdateResource and EndUpdateResource functions.
 *             The return value is NULL if the specified file is not an
 *             executable file, the executable file is already loaded,
 *             the file does not exist, or the file cannot be opened for writing.
 *             To get extended error information, call GetLastError
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

HANDLE WIN32API BeginUpdateResourceW( LPCWSTR pFileName,
                                        BOOL bDeleteExistingResources)
{

  dprintf(("KERNEL32:  BeginUpdateResource(%08x,%08x) not implemented\n",
           pFileName, bDeleteExistingResources
          ));

  return (NULL);
}

/*****************************************************************************
 * Name      : BOOL WIN32AOI CallNamedPipeA
 * Purpose   : The CallNamedPipe function connects to a message-type pipe
 *             (and waits if an instance of the pipe is not available),
 *             writes to and reads from the pipe, and then closes the pipe.
 * Parameters:  LPCSTR lpNamedPipeName pointer to pipe name
 *              LPVOID lpInBuffer      pointer to write buffer
 *              DWORD nInBufferSize    size, in bytes, of write buffer
 *              LPVOID lpOutBuffer     pointer to read buffer
 *              DWORD nOutBufferSize   size, in bytes, of read buffer
 *              LPDWORD lpBytesRead    pointer to number of bytes read
 *              DWORD nTimeOut         time-out time, in milliseconds
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : Calling CallNamedPipe is equivalent to calling the CreateFile
 *             (or WaitNamedPipe, if CreateFile cannot open the pipe immediately),
 *             TransactNamedPipe, and CloseHandle functions. CreateFile is called
 *             with an access flag of GENERIC_READ | GENERIC_WRITE, an inherit
 *             handle flag of FALSE, and a share mode of zero (indicating no
 *             sharing of this pipe instance).
 *             If the message written to the pipe by the server process is
 *             longer than nOutBufferSize, CallNamedPipe returns FALSE, and
 *             GetLastError returns ERROR_MORE_DATA. The remainder of the
 *             message is discarded, because CallNamedPipe closes the handle
 *             to the pipe before returning.
 *
 *             CallNamedPipe fails if the pipe is a byte-type pipe.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CallNamedPipeA( LPCSTR lpNamedPipeName,
                                  LPVOID lpInBuffer, DWORD nInBufferSize,
                                  LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                  LPDWORD lpBytesRead, DWORD nTimeOut)
{

  dprintf(("KERNEL32:  CallNamedPipeA(%08x,%08x,%08x,%08x,%08x,%08x) not implemented\n",
           lpNamedPipeName, lpInBuffer, nInBufferSize,
           lpOutBuffer, nOutBufferSize, lpBytesRead,  nTimeOut
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32AOI CallNamedPipeA
 * Purpose   : The CallNamedPipe function connects to a message-type pipe
 *             (and waits if an instance of the pipe is not available),
 *             writes to and reads from the pipe, and then closes the pipe.
 * Parameters:  LPCWSTR lpNamedPipeName pointer to pipe name
 *              LPVOID lpInBuffer      pointer to write buffer
 *              DWORD nInBufferSize    size, in bytes, of write buffer
 *              LPVOID lpOutBuffer     pointer to read buffer
 *              DWORD nOutBufferSize   size, in bytes, of read buffer
 *              LPDWORD lpBytesRead    pointer to number of bytes read
 *              DWORD nTimeOut         time-out time, in milliseconds
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : Calling CallNamedPipe is equivalent to calling the CreateFile
 *             (or WaitNamedPipe, if CreateFile cannot open the pipe immediately),
 *             TransactNamedPipe, and CloseHandle functions. CreateFile is called
 *             with an access flag of GENERIC_READ | GENERIC_WRITE, an inherit
 *             handle flag of FALSE, and a share mode of zero (indicating no
 *             sharing of this pipe instance).
 *             If the message written to the pipe by the server process is
 *             longer than nOutBufferSize, CallNamedPipe returns FALSE, and
 *             GetLastError returns ERROR_MORE_DATA. The remainder of the
 *             message is discarded, because CallNamedPipe closes the handle
 *             to the pipe before returning.
 *
 *             CallNamedPipe fails if the pipe is a byte-type pipe.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CallNamedPipeW( LPCWSTR lpNamedPipeName,
                                  LPVOID lpInBuffer, DWORD nInBufferSize,
                                  LPVOID lpOutBuffer, DWORD nOutBufferSize,
                                  LPDWORD lpBytesRead, DWORD nTimeOut)
{

  dprintf(("KERNEL32:  CallNamedPipeA(%08x,%08x,%08x,%08x,%08x,%08x) not implemented\n",
           lpNamedPipeName, lpInBuffer, nInBufferSize,
           lpOutBuffer, nOutBufferSize, lpBytesRead,  nTimeOut
          ));

  return (FALSE);
}
/*****************************************************************************
 * Name      : BOOL WIN32API CancelIo
 * Purpose   : The CancelIO function cancels all pending input and output
 *             (I/O) operations that were issued by the calling thread for
 *             the specified file handle. The function does not cancel
 *             I/O operations issued for the file handle by other threads.
 * Parameters: HANDLE hFile   file handle for which to cancel I/O
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero All pending
 *             I/O operations issued by the calling thread for the file handle
 *             were successfully canceled.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : If there are any I/O operations in progress for the specified
 *             file handle, and they were issued by the calling thread, the
 *             CancelIO function cancels them.
 *             Note that the I/O operations must have been issued as
 *             overlapped I/O. If they were not, the I/O operations would not
 *             have returned to allow the thread to call the CancelIO function.
 *             Calling the CancelIO function with a file handle that was not
 *             opened with FILE_FLAG_OVERLAPPED does nothing.
 *             All I/O operations that are canceled will complete with the
 *             error ERROR_OPERATION_ABORTED. All completion notifications
 *             for the I/O operations will occur normally.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CancelIo(HANDLE hFile)
{

  dprintf(("KERNEL32:CancelIo(%08x) not implemented\n",
           hFile
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL CancelWaitableTimer
 * Purpose   : The CancelWaitableTimer function sets the specified "waitable"
 *             timer to the inactive state.
 * Parameters: HANDLE hTimer  handle to a timer object
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : The CancelWaitableTimer function does not change the signaled
 *             state of the timer. It stops the timer before it can be set to
 *             the signaled state. Therefore, threads performing a wait
 *             operation on the timer remain waiting until they time out or
 *             the timer is reactivated and its state is set to signaled.
 *
 *             To reactivate the timer, call the SetWaitableTimer function.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CancelWaitableTimer(HANDLE hTimer)
{

  dprintf(("KERNEL32:CancelWaitableTimer(%08x) not implemented\n",
           hTimer
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API ConnectNamedPipe
 * Purpose   : The ConnectNamedPipe function enables a named pipe server process
 *             to wait for a client process to connect to an instance of a
 *             named pipe. A client process connects by calling either the
 *             CreateFile or CallNamedPipe function.
 * Parameters:  HANDLE hNamedPipe      handle to named pipe to connect
 *              LPOVERLAPPED lpOverlapped  pointer to overlapped structure
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped)
{

  dprintf(("KERNEL32: ConnectNamedPipe(%08x,%08x) not implemented\n",
           hNamedPipe, lpOverlapped
          ));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API ContinueDebugEvent
 * Purpose   : The ContinueDebugEvent function enables a debugger to continue
 *             a thread that previously reported a debugging event.
 * Parameters: DWORD dwProcessId       process to continue
               DWORD dwThreadId        thread to continue
               DWORD dwContinueStatus  continuation status
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    : Only the thread that created dwProcessId with the CreateProcess
 *             function can call ContinueDebugEvent.
 *             After the ContinueDebugEvent function succeeds, the specified
 *             thread continues. Depending on the debugging event previously
 *             reported by the thread, different actions occur. If the continued
 *             thread previously reported an EXIT_THREAD_DEBUG_EVENT
 *             debugging event, ContinueDebugEvent closes the handle the
 *             debugger has to the thread. If the continued thread previously
 *             reported an EXIT_PROCESS_DEBUG_EVENT debugging event,
 *             ContinueDebugEvent closes the handles the debugger has to the
 *             process and to the thread.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API ContinueDebugEvent( DWORD dwProcessId, DWORD dwThreadId,
                                          DWORD dwContinueStatus)
{

  dprintf(("KERNEL32:  ContinueDebugEvent(%08x,%08x,%08x)not implemented\n",
           dwProcessId, dwThreadId, dwContinueStatus
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : LCID WIN32API ConvertDefaultLocale
 * Purpose   : The ConvertDefaultLocale function converts a special default
 *             locale value to an actual locale identifier.
 * Parameters: LCID Locale   special default locale value to be converted
 *             Value                   Description
 *             LOCALE_SYSTEM_DEFAULT   The system's default locale.
 *             LOCALE_USER_DEFAULT     The current user's default locale.
 *             zero                The language-neutral default locale.
 *                                     This is equivalent to the locale identifier
 *                                     created by calling the MAKELCID macro
 *                                     with a language identifier consisting
 *                                     of the LANG_NEUTRAL and SUBLANG_NEUTRAL
 *                                     values.
 *             Any sublanguage neutral default locale
 *                                     A locale identifier constructed by
 *                                     calling MAKELCID with a language
 *                                     identifier consisting of a primary
 *                                     language value, such as LANG_ENGLISH,
 *                                     and the SUBLANG_NEUTRAL value.
 *
 * Variables :
 * Result    : If the function succeeds, the return value is the appropriate
 *             actual locale identifier.
 *             If the function fails, the return value is the Locale parameter.
 *             The function fails when Locale is not one of the special
 *             default locale values listed above.
 * Remark    : A call to ConvertDefaultLocale(LOCALE_SYSTEM_DEFAULT)
 *              is equivalent to a call to GetSystemDefaultLCID.
 *             A call to ConvertDefaultLocale(LOCALE_USER_DEFAULT)
 *              is equivalent to a call to GetUserDefaultLCID.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

LCID WIN32API ConvertDefaultLocale(LCID Locale)
{

  dprintf(("KERNEL32: ConvertDefaultLocale(%08x) partly implmented - returns input\n",
           Locale
          ));

  return (Locale);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CopyFileExA
 * Purpose   : The CopyFileExA function copies an existing file to a new file.
 *             This function preserves extended attributes, OLE structured
 *             storage, NTFS alternate data streams, and file attributes.
 *             Security attributes for the existing file are not copied to
 *             the new file.
 * Parameters: LPCSTR lpExistingFileName   pointer to name of an existing file
 *             LPCSTR lpNewFileName        pointer to filename to copy to
 *             LPPROGRESS_ROUTINE lpProgressRoutine  pointer to the callback function
 *             LPVOID lpData               to be passed to the callback function
 *             LPBOOL pbCancel     flag that can be used to cancel the operation
 *             DWORD dwCopyFlags   flags that specify how the file is copied
 * Variables :
 * Result    : f the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CopyFileExA( LPCSTR             lpExistingFileName,
                              LPCSTR             lpNewFileName,
                              LPPROGRESS_ROUTINE lpProgressRoutine,
                              LPVOID             lpData,
                              LPBOOL             pbCancel,
                              DWORD              dwCopyFlags)
{

  dprintf(("KERNEL32: CopyFileExA(%08x,%08x,%08x,%08x,%08x,%08x) not implemented\n",
           lpExistingFileName,
           lpNewFileName,
           lpProgressRoutine,
           lpData,
           pbCancel,
           dwCopyFlags
          ));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API CopyFileExW
 * Purpose   : The CopyFileExW function copies an existing file to a new file.
 *             This function preserves extended attributes, OLE structured
 *             storage, NTFS alternate data streams, and file attributes.
 *             Security attributes for the existing file are not copied to
 *             the new file.
 * Parameters: LPCWSTR lpExistingFileName   pointer to name of an existing file
 *             LPCWSTR lpNewFileName        pointer to filename to copy to
 *             LPPROGRESS_ROUTINE lpProgressRoutine  pointer to the callback function
 *             LPVOID lpData               to be passed to the callback function
 *             LPBOOL pbCancel     flag that can be used to cancel the operation
 *             DWORD dwCopyFlags   flags that specify how the file is copied
 * Variables :
 * Result    : f the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Thu, 1998/05/19 11:46]
 *****************************************************************************/

BOOL WIN32API CopyFileExW( LPCWSTR            lpExistingFileName,
                              LPCWSTR            lpNewFileName,
                              LPPROGRESS_ROUTINE lpProgressRoutine,
                              LPVOID             lpData,
                              LPBOOL             pbCancel,
                              DWORD              dwCopyFlags)
{

  dprintf(("KERNEL32: CopyFileExW(%08x,%08x,%08x,%08x,%08x,%08x) not implemented\n",
           lpExistingFileName,
           lpNewFileName,
           lpProgressRoutine,
           lpData,
           pbCancel,
           dwCopyFlags
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CreateDirectoryExA
 * Purpose   : The CreateDirectoryExA function creates a new directory with a
 *             specified path that retains the attributes of a specified
 *             template directory. If the underlying file system supports
 *             security on files and directories, the function applies a
 *             specified security descriptor to the new directory.
 *             The new directory retains the other attributes of the specified
 *             template directory. Note that CreateDirectoryEx has a template
 *             parameter, while CreateDirectory does not.
 * Parameters: LPCSTR lpTemplateDirectory  pointer to path string of template
 *                                         directory
 *             LPCSTR lpNewDirectory      pointer to path string of directory
 *                                         to create
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes  pointer to security
 *                                                         descriptor
 *
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API CreateDirectoryExA( LPCSTR lpTemplateDirectory,
                                     LPCSTR lpNewDirectory,
                                     LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

  dprintf(("KERNEL32:CreateDirectoryExA(%08x,%08x,%08x) not implemented\n",
           lpTemplateDirectory,lpNewDirectory,lpSecurityAttributes
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API CreateDirectoryExW
 * Purpose   : The CreateDirectoryExW function creates a new directory with a
 *             specified path that retains the attributes of a specified
 *             template directory. If the underlying file system supports
 *             security on files and directories, the function applies a
 *             specified security descriptor to the new directory.
 *             The new directory retains the other attributes of the specified
 *             template directory. Note that CreateDirectoryEx has a template
 *             parameter, while CreateDirectory does not.
 * Parameters: LPCWSTR lpTemplateDirectory  pointer to path string of template
 *                                          directory
 *             LPCWSTR lpNewDirectory      pointer to path string of directory
 *                                         to create
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes  pointer to security
 *                                                         descriptor
 *
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 *             To get extended error information, call GetLastError.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API CreateDirectoryExW( LPCWSTR lpTemplateDirectory,
                                     LPCWSTR lpNewDirectory,
                                     LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

  dprintf(("KERNEL32:CreateDirectoryExW(%08x,%08x,%08x) not implemented\n",
           lpTemplateDirectory,lpNewDirectory,lpSecurityAttributes
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : LPVOID WIN32API CreateFiber
 * Purpose   : The CreateFiber function allocates a fiber object, assigns it a
 *             stack, and sets up execution to begin at the specified
 *             start address, typically the fiber function.
 *             This function does not schedule the fiber.
 * Parameters: DWORD dwStackSize             initial thread stack size, in bytes
 *             LPFIBER_START_ROUTINE lpStartAddress    pointer to fiber function
 *             LPVOID lpParameter                  argument for new fiber
 * Variables :
 * Result    : If the function succeeds, the return value is the address of
 *             the fiber.
 *             If the function fails, the return value is NULL.
 * Remark    : Before a thread can schedule a fiber using the SwitchToFiber
 *             function, it must call the ConvertThreadToFiber function so there
 *             is a fiber associated with the thread.
 *             The fiber function is of type FIBER_START_ROUTINE.
 *             It accepts a single value of type PVOID (fiber data) and does not
 *              return a value. The prototype for this function is as follows:
 *
 *              VOID WINAPI FiberFunc( PVOID lpParameter );
 *
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

LPVOID WIN32API CreateFiber( DWORD dwStackSize,
                                LPFIBER_START_ROUTINE lpStartAddress,
                                LPVOID lpParameter)
{

  dprintf(("KERNEL32:CreateFiber(%08x,%08x,%08x) not implemented\n",
           dwStackSize, lpStartAddress,lpParameter
          ));

  return (NULL);
}


/*****************************************************************************
 * Name      : HANDLE WIN32API CreateIoCompletionPort
 * Purpose   : The CreateIoCompletionPort function can associate an instance of
 *             an opened file with a newly created or an existing input/output
 *             completion port; or it can create an input/output completion port
 *             without associating it with a file.
 *             Associating an instance of an opened file with an input/output
 *             completion port lets an application receive notification of the
 *             completion of asynchronous input/output operations involving that
 *             file.
 * Parameters: HANDLE FileHandle              file handle to associate with I/O
 *                                            completion port. Must be created
 *                                            with flag FILE_FLAG_OVERLAPPED !
 *             HANDLE ExistingCompletionPort  optional handle to existing
 *                                            I/O completion port
 *             DWORD CompletionKey            per-file completion key for
 *                                            I/O completion packets
 *             DWORD NumberOfConcurrentThreads  number of threads allowed to
 *                                              execute concurrently
 * Variables :
 * Result    : If the function succeeds, the return value is the handle to the
 *             I/O completion port that is associated with the specified file.
 *             This return value is not NULL.
 *             If the function fails, the return value is NULL.
 * Remark    : The Win32 I/O system can be instructed to send I/O completion
 *             notification packets to input/output completion ports, where they
 *             are queued up. The CreateIoCompletionPort function provides a
 *             mechanism for this.
 *             When you perform an input/output operation with a file handle
 *             that has an associated input/output completion port, the
 *             I/O system sends a completion notification packet to the
 *             completion port when the I/O operation completes.
 *             The I/O completion port places the completion packet in a
 *             first-in-first-out queue.
 *             Use the GetQueuedCompletionStatus function to retrieve these
 *             queued I/O completion packets.
 *             Threads in the same process can use the PostQueuedCompletionStatus
 *             function to place I/O completion notification packets in a
 *             completion port's queue. This allows you to use the port to
 *             receive communications from other threads of the process,
 *             in addition to receiving I/O completion notification packets
 *             from the Win32 I/O system.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

HANDLE WIN32API CreateIoCompletionPort( HANDLE FileHandle,
                                           HANDLE ExistingCompletionPort,
                                           DWORD CompletionKey,
                                           DWORD NumberOfConcurrentThreads)
{

  dprintf(("KERNEL32: CreateIoCompletionPort(%08x,%08x,%08x,%08x) not implemented\n",
           FileHandle, ExistingCompletionPort, CompletionKey,
           NumberOfConcurrentThreads
          ));

  return (NULL);
}
/*****************************************************************************
 * Name      : HANDLE WIN32API CreateMailslotA
 * Purpose   : The CreateMailslot function creates a mailslot with the specified
 *             name and returns a handle that a mailslot server can use to
 *             perform operations on the mailslot. The mailslot is local to the
 *             computer that creates it. An error occurs if a mailslot with
 *             the specified name already exists.
 * Parameters: LPCSTR lpName              pointer to string for mailslot name
 *             DWORD nMaxMessageSize      maximum message size
 *             DWORD lReadTimeout         milliseconds before read time-out
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes pointer to security structure
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to
 *             the mailslot, for use in server mailslot operations.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

HANDLE WIN32API CreateMailslotA(LPCSTR lpName, DWORD nMaxMessageSize,
                                   DWORD lReadTimeout,
                                   LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

  dprintf(("KERNEL32: CreateMailslotA(%08x,%08x,%08x,%08x) not implemented - INVALID_HANDLE_VALUE\n",
           lpName, nMaxMessageSize, lReadTimeout, lpSecurityAttributes
          ));

  return (INVALID_HANDLE_VALUE);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API CreateMailslotW
 * Purpose   : The CreateMailslot function creates a mailslot with the specified
 *             name and returns a handle that a mailslot server can use to
 *             perform operations on the mailslot. The mailslot is local to the
 *             computer that creates it. An error occurs if a mailslot with
 *             the specified name already exists.
 * Parameters: LPCWSTR lpName             pointer to string for mailslot name
 *             DWORD nMaxMessageSize      maximum message size
 *             DWORD lReadTimeout         milliseconds before read time-out
 *             LPSECURITY_ATTRIBUTES lpSecurityAttributes  pointer to security
 *                                                         structure
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to
 *             the mailslot, for use in server mailslot operations.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

HANDLE WIN32API CreateMailslotW(LPCWSTR lpName, DWORD nMaxMessageSize,
                                   DWORD lReadTimeout,
                                   LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{

  dprintf(("KERNEL32: CreateMailslotW(%08x,%08x,%08x,%08x) not implemented - INVALID_HANDLE_VALUE\n",
           lpName, nMaxMessageSize, lReadTimeout, lpSecurityAttributes
          ));

  return (INVALID_HANDLE_VALUE);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API CreateRemoteThread
 * Purpose   : The CreateRemoteThread function creates a thread that runs in
 *             the address space of another process.
 * Parameters: HANDLE hProcess             handle to process to create thread in
 *             LPSECURITY_ATTRIBUTES lpThreadAttributes      pointer to thread
 *                                                           security attributes
 *             DWORD dwStackSize             initial thread stack size, in bytes
 *             LPTHREAD_START_ROUTINE lpStartAddress  pointer to thread function
 *             LPVOID lpParameter          pointer to argument for new thread
 *             DWORD dwCreationFlags       creation flags
 *             LPDWORD lpThreadId      pointer to returned thread identifier
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to
 *             the new thread.
 *             If the function fails, the return value is NULL.
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

HANDLE WIN32API CreateRemoteThread( HANDLE hProcess,
                                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                       DWORD dwStackSize,
                                       LPTHREAD_START_ROUTINE lpStartAddress,
                                       LPVOID lpParameter,DWORD dwCreationFlags,
                                       LPDWORD lpThreadId )
{

  dprintf(("KERNEL32: CreateRemoteThread(%08x,%08x,%08x,%08x,%08x,%08x,%08x) not implemented\n",
           hProcess, lpThreadAttributes, dwStackSize, lpStartAddress,
           lpParameter, dwCreationFlags, lpThreadId
          ));

  return (NULL);
}
/*****************************************************************************
 * Name      : DWORD WIN32API CreateTapePartition
 * Purpose   : The CreateTapePartition function reformats a tape.
 * Parameters: HANDLE hDevice             handle to open device
 *             DWORD dwPartitionMethode   type of new partition
 *             DWORD dwCount              number of new partitions to create
 *             DWORD dwSize               size of new partition, in megabytes
 * Variables :
 * Result    : If the function succeeds, the return value is NO_ERROR.
 *             If the function fails, it may return one of the following
 *             error codes:
 *             Error                         Description
 *             ERROR_BEGINNING_OF_MEDIA      An attempt to access data before
 *                                           the beginning-of-medium marker failed.
 *             ERROR_BUS_RESET               A reset condition was detected on the bus.
 *             ERROR_END_OF_MEDIA        The end-of-tape marker was reached
 *                                           during an operation.
 *             ERROR_FILEMARK_DETECTED       A filemark was reached during an operation.
 *             ERROR_SETMARK_DETECTED        A setmark was reached during an operation.
 *             ERROR_NO_DATA_DETECTED        The end-of-data marker was reached
 *                                           during an operation.
 *             ERROR_PARTITION_FAILURE       The tape could not be partitioned.
 *             ERROR_INVALID_BLOCK_LENGTH    The block size is incorrect on a
 *                                           new tape in a multivolume partition.
 *             ERROR_DEVICE_NOT_PARTITIONED  The partition information could not
 *                                           be found when a tape was being loaded.
 *             ERROR_MEDIA_CHANGED       The tape that was in the drive has
 *                                           been replaced or removed.
 *             ERROR_NO_MEDIA_IN_DRIVE       There is no media in the drive.
 *             ERROR_NOT_SUPPORTED       The tape driver does not support a
 *                                           requested function.
 *             ERROR_UNABLE_TO_LOCK_MEDIA    An attempt to lock the ejection
 *                                           mechanism failed.
 *             ERROR_UNABLE_TO_UNLOAD_MEDIA  An attempt to unload the tape failed.
 *             ERROR_WRITE_PROTECT       The media is write protected.
 * Remark    : Creating partitions reformats the tape. All previous information
 *             recorded on the tape is destroyed.
 *             Errorcodes are defined in winerror.h
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

DWORD WIN32API CreateTapePartition( HANDLE hDevice, DWORD dwPartitionMethod,
                                       DWORD dwCount, DWORD dwSize)
{

  dprintf(("KERNEL32: CreateTapePartition(%08x,%08x,%08x,%08x) not implemented - ERROR_NOT_SUPPORTED\n",
            hDevice, dwPartitionMethod, dwCount, dwSize
          ));

  return (ERROR_NOT_SUPPORTED);
}
/*****************************************************************************
 * Name      : HANDLE WIN23API CreateWaitableTimerA
 * Purpose   : The CreateWaitableTimerA function creates a "waitable" timer object.
 * Parameters: LPSECURITY_ATTRIBUTES lpTimerAttributes  pointer to security attributes
 *             BOOL bManualReset                        flag for manual reset state
 *             LPCTSTR lpTimerName                  pointer to timer object name
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to the
 *             timer object. If the named timer object exists before the
 *             function call, GetLastError returns ERROR_ALREADY_EXISTS.
 *             Otherwise, GetLastError returns zero.
 *             If the function fails, the return value is NULL
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

HANDLE WIN32API CreateWaitableTimerA( LPSECURITY_ATTRIBUTES lpTimerAttributes,
                                         BOOL bManualReset, LPCSTR lpTimerName)
{

  dprintf(("KERNEL32: CreateWaitableTimer(%08x,%08x,%08x) not implemented\n",
            lpTimerAttributes, bManualReset, lpTimerName
          ));

  return (NULL);
}

/*****************************************************************************
 * Name      : HANDLE WIN23API CreateWaitableTimerW
 * Purpose   : The CreateWaitableTimerW function creates a "waitable" timer object.
 * Parameters: LPSECURITY_ATTRIBUTES lpTimerAttributes  pointer to security attributes
 *             BOOL bManualReset                        flag for manual reset state
 *             LPCTSTR lpTimerName                  pointer to timer object name
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to the
 *             timer object. If the named timer object exists before the
 *             function call, GetLastError returns ERROR_ALREADY_EXISTS.
 *             Otherwise, GetLastError returns zero.
 *             If the function fails, the return value is NULL
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

HANDLE WIN32API CreateWaitableTimerW( LPSECURITY_ATTRIBUTES lpTimerAttributes,
                                         BOOL bManualReset, LPCWSTR lpTimerName)
{

  dprintf(("KERNEL32: CreateWaitableTimer(%08x,%08x,%08x) not implemented\n",
            lpTimerAttributes, bManualReset, lpTimerName
          ));

  return (NULL);
}

/*****************************************************************************
 * Name      : BOOL WIN32API DebugActiveProcess
 * Purpose   : The DebugActiveProcess function allows a debugger to attach to
 *             an active process and then debug it.
 * Parameters: DWORD dwProcessId    process to be debugged
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API DebugActiveProcess(DWORD dwProcessId)
{

  dprintf(("KERNEL32:DebugActiveProcess(%08x) not implemented\n",
           dwProcessId
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API DefineDosDeviceA
 * Purpose   : The DefineDosDeviceA function lets an application define,
 *             redefine, or delete MS-DOS device names.
 *             MS-DOS device names are stored as symbolic links in the object
 *             name space in 32-bit versions of Windows. The code that converts
 *             an MS-DOS path into a corresponding path in 32-bit versions of
 *             Windows uses these symbolic links to map MS-DOS devices and drive
 *             letters. The DefineDosDevice function provides a mechanism
 *             whereby a Win32-based application can modify the symbolic links
 *             used to implement the MS-DOS device name space.
 * Parameters: DWORD dwFlags         flags specifying aspects of device definition
 *             LPCSTR lpDeviceName   pointer to MS-DOS device name string
 *             LPCSTR lpTargetPath   pointer to MS-DOS or path string for
 *                                   32-bit Windows
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API DefineDosDeviceA( DWORD dwFlags, LPCSTR lpDeviceName,
                                   LPCSTR lpTargetPath  )
{

  dprintf(("KERNEL32: DefineDosDeviceA(%08x,%08x,%08x) not implemented\n",
           dwFlags,  lpDeviceName, lpTargetPath
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API DefineDosDeviceW
 * Purpose   : UNICODE version of DefineDosDeviceA
 * Parameters: DWORD dwFlags         flags specifying aspects of device definition
 *             LPCSTR lpDeviceName   pointer to MS-DOS device name string
 *             LPCSTR lpTargetPath   pointer to MS-DOS or path string for
 *                                   32-bit Windows
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API DefineDosDeviceW( DWORD dwFlags, LPCWSTR lpDeviceName,
                                   LPCWSTR lpTargetPath )
{

  dprintf(("KERNEL32: DefineDosDeviceW(%08x,%08x,%08x) not implemented\n",
           dwFlags,  lpDeviceName, lpTargetPath
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : VOID WIN32API DeleteFiber
 * Purpose   : The DeleteFiber function deletes an existing fiber
 * Parameters:  LPVOID lpFiber          pointer to the fiber to delete
 * Variables :
 * Result    : No returnvalue
 * Remark    : The DeleteFiber function deletes all data associated with the
 *             fiber. This data includes the stack, a subset of the registers,
 *             and the fiber data. If the currently running fiber calls
 *             DeleteFiber, the ExitThread function is called and the thread
 *             terminates. If the currently running fiber is deleted by another
 *             thread, the thread associated with the fiber is likely to
 *             terminate abnormally because the fiber stack has been freed.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

VOID WIN32API DeleteFiber(LPVOID lpFiber)
{

  dprintf(("KERNEL32: DeleteFiber(%08x) not implemented\n",
           lpFiber
          ));
}
/*****************************************************************************
 * Name      : BOOL WIN32API DisconnectNamedPipe
 * Purpose   : The DisconnectNamedPipe function disconnects the server end
 *             of a named pipe instance from a client process.
 * Parameters: HANDLE hNamedPipe    handle to named pipe
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API DisconnectNamedPipe(HANDLE hNamedPipe)
{

  dprintf(("KERNEL32: DisconnectNamedPipe(%08x) not implemented\n",
           hNamedPipe
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN3API EndUpdateResourceA
 * Purpose   : The EndUpdateResourceA function ends a resource update
 *             in an executable file.
 * Parameters: HANDLE hUpdate   update-file handle
 *             BOOL fDiscard    write flag
 * Variables :
 * Result    : If the function succeeds and the accumulated resource
 *             modifications specified by calls to the UpdateResource function
 *             are written to the specified executable file,
 *             the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EndUpdateResourceA(HANDLE hUpdate,BOOL fDiscard)
{

  dprintf(("KERNEL32:EndUpdateResourceA(%08x,%08x)not implemented\n",
           hUpdate, fDiscard
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN3API EndUpdateResourceW
 * Purpose   : The EndUpdateResourceW function ends a resource update
 *             in an executable file.
 * Parameters: HANDLE hUpdate   update-file handle
 *             BOOL fDiscard    write flag
 * Variables :
 * Result    : If the function succeeds and the accumulated resource
 *             modifications specified by calls to the UpdateResource function
 *             are written to the specified executable file,
 *             the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EndUpdateResourceW(HANDLE hUpdate,BOOL fDiscard)
{

  dprintf(("KERNEL32:EndUpdateResourceW(%08x,%08x) not implemented\n",
           hUpdate, fDiscard
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumCalendarInfoA
 * Purpose   : The EnumCalendarInfoA function enumerates calendar information
 *             for a specified locale. The CalType parameter specifies the type
 *             of calendar information to enumerate. The function returns the
 *             specified calendar information for all applicable calendars for
 *             the locale, or for a single requested calendar, depending on the
 *             value of the Calendar parameter.
 *             The EnumCalendarInfo function enumerates the calendar information
 *             by calling an application-defined callback function. It passes
 *             the callback function a pointer to a string buffer containing
 *             the requested calendar information. This continues until either
 *             the last applicable calendar is found or the callback function
 *             returns FALSE.
 * Parameters: CALINFO_ENUMPROCA lpCalInfoEnumProc  pointer to enumeration
 *                                                  callback function
 *             LCID Locale      locale whose calendar information is of interest
 *             CALID Calendar   calendar whose information is of interest
 *             CALTYPE CalType  type of calendar information of interest
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumCalendarInfoA(CALINFO_ENUMPROCA lpCalInfoEnumProc,
                                   LCID              Locale,
                                   CALID             Calendar,
                                   CALTYPE           CalType)
{

  dprintf(("KERNEL32: EnumCalendarInfoA(%08x,%08x,%08x,%08x)not implemented\n",
           lpCalInfoEnumProc,
           Locale,
           Calendar,
           CalType
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumCalendarInfoW
 * Purpose   : The EnumCalendarInfoW function enumerates calendar information
 *             for a specified locale. The CalType parameter specifies the type
 *             of calendar information to enumerate. The function returns the
 *             specified calendar information for all applicable calendars for
 *             the locale, or for a single requested calendar, depending on the
 *             value of the Calendar parameter.
 *             The EnumCalendarInfo function enumerates the calendar information
 *             by calling an application-defined callback function. It passes
 *             the callback function a pointer to a string buffer containing
 *             the requested calendar information. This continues until either
 *             the last applicable calendar is found or the callback function
 *             returns FALSE.
 * Parameters: CALINFO_ENUMPROCW lpCalInfoEnumProc  pointer to enumeration
 *                                                  callback function
 *             LCID Locale      locale whose calendar information is of interest
 *             CALID Calendar   calendar whose information is of interest
 *             CALTYPE CalType  type of calendar information of interest
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumCalendarInfoW(CALINFO_ENUMPROCW lpCalInfoEnumProc,
                                   LCID              Locale,
                                   CALID             Calendar,
                                   CALTYPE           CalType)
{

  dprintf(("KERNEL32: EnumCalendarInfoW(%08x,%08x,%08x,%08x)not implemented\n",
           lpCalInfoEnumProc,
           Locale,
           Calendar,
           CalType
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceLanguagesA
 * Purpose   : The EnumResourceLanguagesA function searches a module for each
 *             resource of the specified type and name and passes the language
 *             of each resource it locates to a defined callback function
 * Parameters: HMODULE hModule                  resource-module handle
 *             LPCTSTR lpType                   pointer to resource type
 *             LPCTSTR lpName,                  pointer to resource name
 *             ENUMRESLANGPROC lpEnumFunc       pointer to callback function
 *             LONG lParam                  application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 * Remark    : The EnumResourceLanguages function continues to enumerate
 *             resource languages until the callback function returns FALSE
 *             or all resource languages have been enumerated.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceLanguagesA(HMODULE hModule, LPCSTR lpType,
                                     LPCSTR lpName,
                                     ENUMRESLANGPROCA lpEnumFunc,
                                     LONG lParam)
{

  dprintf(("KERNEL32:EnumResourceLanguagesA(%08x,%08x,%08x,%08x,%08x)\n not implemented",
           hModule, lpType, lpName, lpEnumFunc, lParam
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceLanguagesW
 * Purpose   : The EnumResourceLanguagesW function searches a module for each
 *             resource of the specified type and name and passes the language
 *             of each resource it locates to a defined callback function
 * Parameters: HMODULE hModule                  resource-module handle
 *             LPCTSTR lpType                   pointer to resource type
 *             LPCTSTR lpName,                  pointer to resource name
 *             ENUMRESLANGPROC lpEnumFunc       pointer to callback function
 *             LONG lParam                  application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero.
 * Remark    : The EnumResourceLanguages function continues to enumerate
 *             resource languages until the callback function returns FALSE
 *             or all resource languages have been enumerated.
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceLanguagesW(HMODULE hModule, LPCWSTR lpType,
                                     LPCWSTR lpName,
                                     ENUMRESLANGPROCW lpEnumFunc,
                                     LONG lParam)
{

  dprintf(("KERNEL32:EnumResourceLanguagesW(%08x,%08x,%08x,%08x,%08x)\n not implemented",
           hModule, lpType, lpName, lpEnumFunc, lParam
          ));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceNamesW
 * Purpose   : The EnumResourceNames function searches a module for each
 *             resource of the specified type and passes the name of each
 *             resource it locates to an application-defined callback function
 * Parameters: HINSTANCE hModule             resource-module handling
 *             LPCTSTR lpszType              pointer to resource type
 *             ENUMRESNAMEPROC lpEnumFunc    pointer to callback function
 *             LONG lParam                   application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    : The EnumResourceNames function continues to enumerate resource
 *             names until the callback function returns FALSE or all resource
 *             names have been enumerated
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceNamesW(HMODULE          hModule,
                                 LPCWSTR          lpszType,
                                 ENUMRESNAMEPROCW lpEnumFunc,
                                 LONG             lParam)
{

  dprintf(("KERNEL32:EnumResourceNamesW(%08x,%08x,%08x,%08x) not implemented\n",
           hModule,
           lpszType,
           lpEnumFunc,
           lParam
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceTypesA
 * Purpose   : The EnumResourceTypesA function searches a module for resources
 *             and passes each resource type it finds to an application-defined
 *             callback function
 * Parameters: HMODULE hModule,                    resource-module handle
 *             ENUMRESTYPEPROC lpEnumFunc          pointer to callback function
 *             LONG lParam                         application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 * If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceTypesA(HMODULE hModule,
                                 ENUMRESTYPEPROCA lpEnumFunc, LONG lParam)
{

  dprintf(("KERNEL32:  EnumResourceTypesA(%08x,%08x,%08x) not implemented\n",
           hModule, lpEnumFunc, lParam
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumResourceTypesW
 * Purpose   : The EnumResourceTypesW function searches a module for resources
 *             and passes each resource type it finds to an application-defined
 *             callback function
 * Parameters: HMODULE hModule,                    resource-module handle
 *             ENUMRESTYPEPROC lpEnumFunc          pointer to callback function
 *             LONG lParam                         application-defined parameter
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 * If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumResourceTypesW(HMODULE hModule,
                                 ENUMRESTYPEPROCW lpEnumFunc, LONG lParam)
{

  dprintf(("KERNEL32:  EnumResourceTypesW(%08x,%08x,%08x) not implemented\n",
           hModule, lpEnumFunc, lParam
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumSystemCodePagesA
 * Purpose   : The EnumSystemCodePagesA function enumerates the code pages that
 *             are either installed on or supported by a system. The dwFlags
 *             parameter determines whether the function enumerates installed
 *             or supported code pages. The function enumerates the code pages
 *             by passing code page identifiers, one at a time, to the specified
 *             application-defined callback function. This continues until all
 *             of the installed or supported code page identifiers have been
 *             passed to the callback function, or the callback function
 *             returns FALSE
 * Parameters: CODEPAGE_ENUMPROCA lpCodePageEnumProc  pointer to enumeration
 *                                                    callback function
 *             DWORD dwFlags                          indicates which code pages
 *                                                    to enumerate
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumSystemCodePagesA(CODEPAGE_ENUMPROCA lpCodePageEnumProc,
                                   DWORD dwFlags)
{

  dprintf(("KERNEL32: EnumSystemCodePagesA(%08x,%08x) not implemented\n",
           lpCodePageEnumProc, dwFlags
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL WIN32API EnumSystemCodePagesW
 * Purpose   : The EnumSystemCodePagesW function enumerates the code pages that
 *             are either installed on or supported by a system. The dwFlags
 *             parameter determines whether the function enumerates installed
 *             or supported code pages. The function enumerates the code pages
 *             by passing code page identifiers, one at a time, to the specified
 *             application-defined callback function. This continues until all
 *             of the installed or supported code page identifiers have been
 *             passed to the callback function, or the callback function
 *             returns FALSE
 * Parameters: CODEPAGE_ENUMPROCW lpCodePageEnumProc  pointer to enumeration
 *                                                    callback function
 *             DWORD dwFlags                          indicates which code pages
 *                                                    to enumerate
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 17:46]
 *****************************************************************************/

BOOL WIN32API EnumSystemCodePagesW(CODEPAGE_ENUMPROCW lpCodePageEnumProc,
                                   DWORD dwFlags)
{

  dprintf(("KERNEL32: EnumSystemCodePagesA(%08x,%08x) not implemented\n",
           lpCodePageEnumProc, dwFlags
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : DWORD WIN32API EraseTape
 * Purpose   : The EraseTape function erases all or part of a tape.
 * Parameters: HANDLE hDevice         handle to open device
 *             DWORD dwEraseType      type of erasure to perform
 *             BOOL bImmediate        return after erase operation begins
 * Variables :
 * Result    : If the function succeeds, the return value is NO_ERROR.
 *             If the function fails, the return value is like in
 *             CreateTapePartition
 * Remark    : Some tape devices do not support certain tape operations. To de
 *             termine your tape device's capabilities, see your tape device
 *             documentation and use the GetTapeParameters function
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

DWORD WIN32API EraseTape( HANDLE hDevice, DWORD dwEraseType, BOOL bImmediate)
{

  dprintf(("KERNEL32: EraseTape(%08x,%08x,%08x) not implemented - ERROR_NOT_SUPPORTED\n",
           hDevice, dwEraseType, bImmediate
          ));

  return (ERROR_NOT_SUPPORTED);
}

/*****************************************************************************
 * Name      : DWORD WIN32API ExpandEnvironmentStringsA
 * Purpose   : The ExpandEnvironmentStringsA function expands environment-variable
 *             strings and replaces them with their defined values.
 * Parameters: LPCSTR lpSrc  pointer to string with environment variables
 *             LPSTR lpDst   pointer to string with expanded environment variables
 *             DWORD nSize   maximum characters in expanded string
 * Variables :
 * Result    : If the function succeeds, the return value is the number of
 *             characters stored in the destination buffer. If the number of
 *             characters is greater than the size of the destination buffer,
 *             the return value is the size of the buffer required to hold
 *             the expanded strings.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

DWORD WIN32API ExpandEnvironmentStringsA(LPCSTR lpSrc,LPSTR lpDst,DWORD nSize)
{

  dprintf(("KERNEL32:ExpandEnvironmentStringsA(%08x,%08x,%08x) not implemented\n",
           lpSrc, lpDst, nSize
          ));

  return (0);
}

/*****************************************************************************
 * Name      : DWORD WIN32API ExpandEnvironmentStringsW
 * Purpose   : The ExpandEnvironmentStringsA function expands environment-variable
 *             strings and replaces them with their defined values.
 * Parameters: LPCWSTR lpSrc  pointer to string with environment variables
 *             LPWSTR lpDst   pointer to string with expanded environment variables
 *             DWORD nSize   maximum characters in expanded string
 * Variables :
 * Result    : If the function succeeds, the return value is the number of
 *             characters stored in the destination buffer. If the number of
 *             characters is greater than the size of the destination buffer,
 *             the return value is the size of the buffer required to hold
 *             the expanded strings.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

DWORD WIN32API ExpandEnvironmentStringsW(LPCWSTR lpSrc,
                                            LPWSTR lpDst,
                                            DWORD nSize)
{

  dprintf(("KERNEL32:ExpandEnvironmentStringsW(%08x,%08x,%08x) not implemented\n",
           lpSrc, lpDst, nSize
          ));

  return (0);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API FindFirstChangeNotificationW
 * Purpose   : The FindFirstChangeNotification function creates a change
 *             notification handle and sets up initial change notification
 *             filter conditions. A wait on a notification handle succeeds when
 *             a change matching the filter conditions occurs in the specified
 *             directory or subtree.
 * Parameters: LPCWSTR lpPathName           pointer to name of directory to watch
 *             BOOL bWatchSubtree           flag for monitoring directory or
 *                                          directory tree
 *             DWORD dwNotifyFilter         filter conditions to watch for
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to a find
 *             change notification object.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

HANDLE WIN32API FindFirstChangeNotificationW( LPCWSTR lpPathName,
                                                BOOL bWatchSubtree,
                                                DWORD dwNotifyFilter)
{

  dprintf(("KERNEL32: FindFirstChangeNotificationW(%08x,%08x,%08x) not implemented - INVALID_HANDLE_VALUE\n",
            lpPathName, bWatchSubtree, dwNotifyFilter
          ));

  return (INVALID_HANDLE_VALUE);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API FindFirstFileExA
 * Purpose   : The FindFirstFileExA function searches a directory for a file
 *             whose name and attributes match those specified in the
 *             function call.
 * Parameters: LPCSTR lpFileName                 pointer to the name of the file
 *                                               to search for
 *             FINDEX_INFO_LEVELS fInfoLevelId   information level of the returned data
 *             LPVOID lpFindFileData             pointer to the returned information
 *             FINDEX_SEARCH_OPS fSearchOp       type of filtering to perform
 *             LPVOID lpSearchFilter             pointer to search criteria
 *             DWORD dwAdditionalFlags           additional search control flags
 * Variables :
 * Result    : If the function succeeds, the return value is a search handle
 *             that can be used in a subsequent call to the FindNextFile or
 *             FindClose functions.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

HANDLE WIN32API FindFirstFileExA( LPCSTR lpFileName,
                                     FINDEX_INFO_LEVELS fInfoLevelId,
                                     LPVOID lpFindFileData,
                                     FINDEX_SEARCH_OPS fSearchOp,
                                     LPVOID lpSearchFilter,
                                     DWORD dwAdditionalFlags)
{

  dprintf(("KERNEL32:  FindFirstFileExA(%08x,%08x,%08x,%08x,%08x,%08x)not implemented - INVALID_HANDLE_VALUE\n",
           lpFileName,
           fInfoLevelId,
           lpFindFileData,
           fSearchOp,
           lpSearchFilter,
           dwAdditionalFlags
          ));

  return (INVALID_HANDLE_VALUE);
}

/*****************************************************************************
 * Name      : HANDLE WIN32API FindFirstFileExW
 * Purpose   : The FindFirstFileExW function searches a directory for a file
 *             whose name and attributes match those specified in the
 *             function call.
 * Parameters: LPCWSTR lpFileName                pointer to the name of the file
 *                                               to search for
 *             FINDEX_INFO_LEVELS fInfoLevelId   information level of the returned data
 *             LPVOID lpFindFileData             pointer to the returned information
 *             FINDEX_SEARCH_OPS fSearchOp       type of filtering to perform
 *             LPVOID lpSearchFilter             pointer to search criteria
 *             DWORD dwAdditionalFlags           additional search control flags
 * Variables :
 * Result    : If the function succeeds, the return value is a search handle
 *             that can be used in a subsequent call to the FindNextFile or
 *             FindClose functions.
 *             If the function fails, the return value is INVALID_HANDLE_VALUE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

HANDLE WIN32API FindFirstFileExW( LPCWSTR lpFileName,
                                     FINDEX_INFO_LEVELS fInfoLevelId,
                                     LPVOID lpFindFileData,
                                     FINDEX_SEARCH_OPS fSearchOp,
                                     LPVOID lpSearchFilter,
                                     DWORD dwAdditionalFlags)
{

  dprintf(("KERNEL32:  FindFirstFileExW(%08x,%08x,%08x,%08x,%08x,%08x)not implemented - INVALID_HANDLE_VALUE\n",
           lpFileName,
           fInfoLevelId,
           lpFindFileData,
           fSearchOp,
           lpSearchFilter,
           dwAdditionalFlags
          ));

  return (INVALID_HANDLE_VALUE);
}

/*****************************************************************************
 * Name      : HRSRC WIN32API FindResourceExA
 * Purpose   : The FindResourceExA function determines the location of the
 *             resource with the specified type, name, and language in the
 *             specified module.
 * Parameters: HMODULE hModule  resource-module handle
 *             LPCSTR lpType    pointer to resource type
 *             LPCSTR lpName    pointer to resource name
 *             WORD wLanguage   resource language
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to the
 *             specified resource's info block. To obtain a handle to the
 *             resource, pass this handle to the LoadResource function.
 *             If the function fails, the return value is NULL
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

HRSRC  WIN32API FindResourceExA( HMODULE hModule, LPCSTR lpType,
                                    LPCSTR lpName, WORD wLanguage)
{

  dprintf(("KERNEL32:  FindResourceExA(%08x,%08x,%08x,%08x) not implemented\n",
            hModule, lpType, lpName, wLanguage
          ));

  return (NULL);
}

/*****************************************************************************
 * Name      : HRSRC WIN32API FindResourceExA
 * Purpose   : The FindResourceExA function determines the location of the
 *             resource with the specified type, name, and language in the
 *             specified module.
 * Parameters: HMODULE hModule  resource-module handle
 *             LPCSTR lpType    pointer to resource type
 *             LPCSTR lpName    pointer to resource name
 *             WORD wLanguage   resource language
 * Variables :
 * Result    : If the function succeeds, the return value is a handle to the
 *             specified resource's info block. To obtain a handle to the
 *             resource, pass this handle to the LoadResource function.
 *             If the function fails, the return value is NULL
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

HRSRC  WIN32API FindResourceExW(HMODULE hModule,
                                   LPCWSTR lpType,
                                   LPCWSTR lpName,
                                   WORD    wLanguage)
{

  dprintf(("KERNEL32:  FindResourceExW(%08x,%08x,%08x,%08x) not implemented\n",
            hModule, lpType, lpName, wLanguage
          ));

  return (NULL);
}

/*****************************************************************************
 * Name      : BOOL WIN32API FlushViewOfFile
 * Purpose   : The FlushViewOfFile function writes to the disk a byte range
 *             within a mapped view of a file.
 * Parameters: LPCVOID lpBaseAddress         start address of byte range to flush
 *             DWORD dwNumberOfBytesToFlush  number of bytes in range
 * Variables :
 * Result    : If the function succeeds, the return value is nonzero.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

BOOL WIN32API FlushViewOfFile(LPCVOID lpBaseAddress,
                              DWORD dwNumberOfBytesToFlush)
{

  dprintf(("KERNEL32:FlushViewOfFile(%08x,%08x) not implemented\n",
           lpBaseAddress,
           dwNumberOfBytesToFlush
          ));

  return (FALSE);
}

/*****************************************************************************
 * Name      : int WIN32API FoldStringA
 * Purpose   : The FoldStringW function maps one string to another, performing
 *             a specified transformation option.
 * Parameters: DWORD dwMapFlags,    // mapping transformation options
 *             LPCSTR lpSrcStr, // pointer to source string
 *             int cchSrc,  // size of source string, in bytes or characters
 *             LPSTR lpDestStr, // pointer to destination buffer
 *             int cchDest  // size of destination buffer, in bytes or characters
 * Variables :
 * Result    : If the function succeeds, the return value is the number of bytes
 *             (ANSI version) or characters (Unicode version) written to the
 *             destination buffer, or if the cchDest parameter is zero,
 *             the number of bytes or characters required to hold the mapped
 *             string.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

int WIN32API FoldStringA( DWORD dwMapFlags, LPCSTR lpSrcStr,
                               int cchSrc, LPSTR  lpDestStr, int cchDest)
{

  dprintf(("KERNEL32: FoldStringA(%08x,%08x,%08x,%08x,%08x) not implemented\n",
           dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest
          ));

  return (0);
}

/*****************************************************************************
 * Name      : int WIN32API FoldStringW
 * Purpose   : The FoldStringW function maps one string to another, performing
 *             a specified transformation option.
 * Parameters: DWORD dwMapFlags,    // mapping transformation options
 *             LPCSTR lpSrcStr, // pointer to source string
 *             int cchSrc,  // size of source string, in bytes or characters
 *             LPSTR lpDestStr, // pointer to destination buffer
 *             int cchDest  // size of destination buffer, in bytes or characters
 * Variables :
 * Result    : If the function succeeds, the return value is the number of bytes
 *             (ANSI version) or characters (Unicode version) written to the
 *             destination buffer, or if the cchDest parameter is zero,
 *             the number of bytes or characters required to hold the mapped
 *             string.
 *             If the function fails, the return value is zero
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

int WIN32API FoldStringW( DWORD dwMapFlags, LPCWSTR lpSrcStr,
                               int cchSrc, LPWSTR  lpDestStr, int cchDest)
{

  dprintf(("KERNEL32: FoldStringA(%08x,%08x,%08x,%08x,%08x) not implemented\n",
           dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest
          ));

  return (0);
}


/*****************************************************************************
 * Name      : VOID WIN32API FreeLibraryAndExitThread
 * Purpose   : The FreeLibraryAndExitThread function decrements the reference
 *             count of a loaded dynamic-link library (DLL) by one, and then
 *             calls ExitThread to terminate the calling thread.
 *             The function does not return.
 *
 *             The FreeLibraryAndExitThread function gives threads that are
 *             created and executed within a dynamic-link library an opportunity
 *             to safely unload the DLL and terminate themselves.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [Tha, 1998/05/21 20:57]
 *****************************************************************************/

VOID WIN32API FreeLibraryAndExitThread( HMODULE hLibModule, DWORD dwExitCode)
{

  dprintf(("KERNEL32:  FreeLibraryAndExitThread(%08x,%08x) not implemented\n",
           hLibModule, dwExitCode
          ));

}


/*****************************************************************************
 * Name      : int GetCurrencyFormatA
 * Purpose   : The GetCurrencyFormatA function formats a number string as a
 *             currency string for a specified locale.
 * Parameters: LCID    Locale   locale for which currency string is to be formatted
 *             DWORD   dwFlags  bit flag that controls the function's operation
 *             LPCTSTR lpValue  pointer to input number string
 *             CONST CURRENCYFMT *lpFormat pointer to a formatting information structure
 *             LPTSTR  lpCurrencyStr       pointer to output buffer
 *             int     cchCurrency         size of output buffer
 * Variables :
 * Result    : number of bytes written to buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API GetCurrencyFormatA(LCID              Locale,
                                DWORD             dwFlags,
                                LPCTSTR           lpValue,
                                CONST CURRENCYFMTA *lpFormat,
                                LPTSTR            lpCurrencyStr,
                                int               cchCurrency)
{
  dprintf(("KERNEL32: GetCurrencyFormatA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           Locale,
           dwFlags,
           lpValue,
           lpFormat,
           lpCurrencyStr,
           cchCurrency));

  return (0);
}


/*****************************************************************************
 * Name      : int GetCurrencyFormatW
 * Purpose   : The GetCurrencyFormatW function formats a number string as a
 *             currency string for a specified locale.
 * Parameters: LCID    Locale   locale for which currency string is to be formatted
 *             DWORD   dwFlags  bit flag that controls the function's operation
 *             LPCWSTR lpValue  pointer to input number string
 *             CONST CURRENCYFMT *lpFormat pointer to a formatting information structure
 *             LPWSTR  lpCurrencyStr       pointer to output buffer
 *             int     cchCurrency         size of output buffer
 * Variables :
 * Result    : number of bytes written to buffer
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API GetCurrencyFormatW(LCID              Locale,
                                DWORD             dwFlags,
                                LPCWSTR           lpValue,
                                CONST CURRENCYFMTW *lpFormat,
                                LPWSTR            lpCurrencyStr,
                                int               cchCurrency)
{
  dprintf(("KERNEL32: GetCurrencyFormatW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           Locale,
           dwFlags,
           lpValue,
           lpFormat,
           lpCurrencyStr,
           cchCurrency));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD GetHandleInformation
 * Purpose   : The GetHandleInformation function obtains information about certain
 *             properties of an object handle. The information is obtained as a set of bit flags.
 * Parameters: HANDLE  hObject
 *             LPDWORD lpdwFlags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetHandleInformation(HANDLE  hObject,
                                      LPDWORD lpdwFlags)
{
  dprintf(("KERNEL32: GetHandleInformation (%08xh, %08xh) not implemented\n",
           hObject,
           lpdwFlags));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetMailslotInfo
 * Purpose   : The GetMailslotInfo function retrieves information about the
 *             specified mailslot.
 * Parameters: HANDLE  hMailslot        mailslot handle
 *             LPDWORD lpMaxMessageSize address of maximum message size
 *             LPDWORD lpNextSize       address of size of next message
 *             LPDWORD lpMessageCount   address of number of messages
 *             LPDWORD lpReadTimeout    address of read time-out
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetMailslotInfo(HANDLE  hMailslot,
                                 LPDWORD lpMaxMessageSize,
                                 LPDWORD lpNextSize,
                                 LPDWORD lpMessageCount,
                                 LPDWORD lpReadTimeout)
{
  dprintf(("KERNEL32: GetMailslotInfo(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           hMailslot,
           lpMaxMessageSize,
           lpNextSize,
           lpMessageCount,
           lpReadTimeout));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetNamedPipeHandleStateA
 * Purpose   : The GetNamedPipeHandleStateA function retrieves information about
 *             a specified named pipe. The information returned can vary during
 *             the lifetime of an instance of the named pipe.
 * Parameters: HANDLE  hNamedPipe           handle of named pipe
 *             LPDWORD lpState              address of flags indicating pipe state
 *             LPDWORD lpCurInstances       address of number of current pipe instances
 *             LPDWORD lpMaxCollectionCount address of max. bytes before remote transmission
 *             LPDWORD lpCollectDataTimeout address of max. time before remote transmission
 *             LPTSTR  lpUserName           address of user name of client process
 *             DWORD   nMaxUserNameSize     size, in characters, of user name buffer
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetNamedPipeHandleStateA(HANDLE  hNamedPipe,
                                          LPDWORD lpState,
                                          LPDWORD lpCurInstances,
                                          LPDWORD lpMaxCollectionCount,
                                          LPDWORD lpCollectDataTimeout,
                                          LPTSTR  lpUserName,
                                          DWORD   nMaxUserNameSize)
{
  dprintf(("KERNEL32: GetNamedPipeHandleStateA(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           hNamedPipe,
           lpState,
           lpCurInstances,
           lpMaxCollectionCount,
           lpCollectDataTimeout,
           lpUserName,
           nMaxUserNameSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetNamedPipeHandleStateW
 * Purpose   : The GetNamedPipeHandleStateW function retrieves information about
 *             a specified named pipe. The information returned can vary during
 *             the lifetime of an instance of the named pipe.
 * Parameters: HANDLE  hNamedPipe           handle of named pipe
 *             LPDWORD lpState              address of flags indicating pipe state
 *             LPDWORD lpCurInstances       address of number of current pipe instances
 *             LPDWORD lpMaxCollectionCount address of max. bytes before remote transmission
 *             LPDWORD lpCollectDataTimeout address of max. time before remote transmission
 *             LPWSTR  lpUserName           address of user name of client process
 *             DWORD   nMaxUserNameSize     size, in characters, of user name buffer
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetNamedPipeHandleStateW(HANDLE  hNamedPipe,
                                          LPDWORD lpState,
                                          LPDWORD lpCurInstances,
                                          LPDWORD lpMaxCollectionCount,
                                          LPDWORD lpCollectDataTimeout,
                                          LPWSTR  lpUserName,
                                          DWORD   nMaxUserNameSize)
{
  dprintf(("KERNEL32: GetNamedPipeHandleStateW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           hNamedPipe,
           lpState,
           lpCurInstances,
           lpMaxCollectionCount,
           lpCollectDataTimeout,
           lpUserName,
           nMaxUserNameSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetNamedPipeInfo
 * Purpose   : The GetNamedPipeInfo function retrieves information about the specified named pipe.
 * Parameters: HANDLE  hNamedPipe      handle of named pipe
 *             LPDWORD lpFlags         address of flags indicating type of pipe
 *             LPDWORD lpOutBufferSize address of size, in bytes, of pipe's output buffer
 *             LPDWORD lpInBufferSize  address of size, in bytes, of pipe's input buffer
 *             LPDWORD lpMaxInstances  address of max. number of pipe instances
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetNamedPipeInfo(HANDLE  hNamedPipe,
                                  LPDWORD lpFlags,
                                  LPDWORD lpOutBufferSize,
                                  LPDWORD lpInBufferSize,
                                  LPDWORD lpMaxInstances)
{
  dprintf(("KERNEL32: GetNamedPipeInfo(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented\n",
           hNamedPipe,
           lpFlags,
           lpOutBufferSize,
           lpInBufferSize,
           lpMaxInstances));

  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL GetProcessShutdownParameters
 * Purpose   : The GetProcessShutdownParameters function retrieves shutdown
 *             parameters for the currently calling process.
 * Parameters: LPDWORD lpdwLevel
 *             LPDWORD lpdwFlags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetProcessShutdownParameters(LPDWORD lpdwLevel,
                                              LPDWORD lpdwFlags)
{
  dprintf(("Kernel32: GetProcessShutdownParameters(%08xh,%08xh) not implemented.\n",
           lpdwLevel,
           lpdwFlags));

  return (FALSE);
}



/*****************************************************************************
 * Name      : BOOL GetProcessTimes
 * Purpose   : The GetProcessTimes function obtains timing information about a specified process.
 * Parameters: HANDLE     hProcess       specifies the process of interest
 *             LPFILETIME lpCreationTime when the process was created
 *             LPFILETIME lpExitTime     when the process exited
 *             LPFILETIME lpKernelTime   time the process has spent in kernel mode
 *             LPFILETIME lpUserTime     time the process has spent in user mode
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetProcessTimes(HANDLE     hProcess,
                                 LPFILETIME lpCreationTime,
                                 LPFILETIME lpExitTime,
                                 LPFILETIME lpKernelTime,
                                 LPFILETIME lpUserTime)
{
  dprintf(("Kernel32: GetProcessTimes(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hProcess,
           lpCreationTime,
           lpExitTime,
           lpKernelTime,
           lpUserTime));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetProcessWorkingSetSize
 * Purpose   : The GetProcessWorkingSetSize function obtains the minimum and
 *             maximum working set sizes of a specified process.
 * Parameters: HANDLE  hProcess                open handle to the process of interest
 *             LPDWORD lpMinimumWorkingSetSize points to variable to receive minimum working set size
 *             LPDWORD lpMaximumWorkingSetSize points to variable to receive maximum working set size
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetProcessWorkingSetSize(HANDLE  hProcess,
                                          LPDWORD lpMinimumWorkingSetSize,
                                          LPDWORD lpMaximumWorkingSetSize)
{
  dprintf(("Kernel32: GetProcessWorkingSetSize(%08xh,%08xh,%08xh) not implemented - dummy values, TRUE.\n",
           hProcess,
           lpMinimumWorkingSetSize,
           lpMaximumWorkingSetSize));

  /* PH: we're returning dummy values here */
  *lpMinimumWorkingSetSize =   512 * 1024; /* 512k */
  *lpMaximumWorkingSetSize = 65536 * 1024; /* 64M  */

  return (TRUE);
}


/*****************************************************************************
 * Name      : BOOL GetQueuedCompletionStatus
 * Purpose   : The GetQueuedCompletionStatus function attempts to dequeue an
 *             I/O completion packet from a specified input/output completion
 *             port. If there is no completion packet queued, the function waits
 *             for a pending input/output operation associated with the completion
 *             port to complete. The function returns when it can dequeue a
 *             completion packet, or optionally when the function times out. If
 *             the function returns because of an I/O operation completion, it
 *             sets several variables that provide information about the operation.
 * Parameters: HANDLE CompletionPort              the I/O completion port of interest
 *             LPDWORD lpNumberOfBytesTransferred to receive number of bytes transferred during I/O
 *             LPDWORD lpCompletionKey            to receive file's completion key
 *             LPOVERLAPPED * lpOverlapped        to receive pointer to OVERLAPPED structure
 *             DWORD dwMilliseconds               optional timeout value
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetQueuedCompletionStatus(HANDLE       CompletionPort,
                                           LPDWORD      lpNumberOfBytesTransferred,
                                           LPDWORD      lpCompletionKey,
                                           LPOVERLAPPED *lpOverlapped,
                                           DWORD        dwMilliseconds)
{
  dprintf(("Kernel32: GetQueuedCompletionStatus(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.",
           CompletionPort,
           lpNumberOfBytesTransferred,
           lpCompletionKey,
           lpOverlapped,
           dwMilliseconds));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetSystemPowerStatus
 * Purpose   : The GetSystemPowerStatus function retrieves the power status of
 *             the system. The status indicates whether the system is running
 *             on AC or DC power, whether the battery is currently charging,
 *             and how much battery life currently remains.
 * Parameters: LPSYSTEM_POWER_STATUS lpSystemPowerStatus
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetSystemPowerStatus(LPSYSTEM_POWER_STATUS lpSystemPowerStatus)
{
  dprintf(("Kernel32: GetSystemPowerStatus(%08xh) not implemented.\n",
           lpSystemPowerStatus));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetSystemTimeAdjustment
 * Purpose   : The GetSystemTimeAdjustment function determines whether the system
 *             is applying periodic time adjustments to its time-of-day clock
 *             at each clock interrupt, along with the value and period of any
 *             such adjustments. Note that the period of such adjustments is
 *             equivalent to the time period between clock interrupts.
 * Parameters: PDWORD lpTimeAdjustment
 *                      size, in 100-nanosecond units, of a periodic time adjustment
 *             PDWORD lpTimeIncrement
 *                      time, in 100-nanosecond units, between periodic time adjustments
 *             PBOOL  lpTimeAdjustmentDisabled
 *                      whether periodic time adjustment is disabled or enabled
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetSystemTimeAdjustment(PDWORD lpTimeAdjustment,
                                         PDWORD lpTimeIncrement,
                                         PBOOL  lpTimeAdjustmentDisabled)
{
  dprintf(("KERNEL32: GetSystemTimeAdjustment(%08xh,%08xh,%08xh) not implemented.\n",
           lpTimeAdjustment,
           lpTimeIncrement,
           lpTimeAdjustmentDisabled));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetTapeParameters
 * Purpose   : The GetTapeParameters function retrieves information that
 *               describes the tape or the tape drive.
 * Parameters: HANDLE  hDevice           handle of open device
 *             DWORD   dwOperation       type of information requested
 *             LPDWORD lpdwSize          address of returned information
 *             LPVOID  lpTapeInformation tape media or drive information
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetTapeParameters(HANDLE  hDevice,
                                    DWORD   dwOperation,
                                    LPDWORD lpdwSize,
                                    LPVOID  lpTapeInformation)
{
  dprintf(("KERNEL32: GetTapeParameters(%08xh,%08xh,%08xh,%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice,
           dwOperation,
           lpdwSize,
           lpTapeInformation));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : BOOL GetTapePosition
 * Purpose   : The GetTapePosition function retrieves the current address of
 *             the tape, in logical or absolute blocks.
 * Parameters: HANDLE  hDevice        handle of open device
 *             DWORD   dwPositionType type of address to obtain
 *             LPDWORD lpdwPartition  address of current tape partition
 *             LPDWORD lpdwOffsetLow  address of low-order 32 bits of tape position
 *             LPDWORD lpdwOffsetHigh address of high-order 32 bits of tape position
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetTapePosition(HANDLE  hDevice,
                                  DWORD   dwPositionType,
                                  LPDWORD lpdwPartition,
                                  LPDWORD lpdwOffsetLow,
                                  LPDWORD lpdwOffsetHigh)
{
  dprintf(("KERNEL32: OS2GetTapePosition(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice,
           dwPositionType,
           lpdwPartition,
           lpdwOffsetLow,
           lpdwOffsetHigh));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : BOOL GetTapeStatus
 * Purpose   : The GetTapeStatus function indicates whether the tape device is
 *             ready to process tape commands.
 * Parameters: HANDLE  hDevice        handle of open device
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API GetTapeStatus(HANDLE hDevice)
{
  dprintf(("KERNEL32: OS2GetTapeStatus(%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : BOOL GetThreadSelectorEntry
 * Purpose   : The GetThreadSelectorEntry function retrieves a descriptor table
 *             entry for the specified selector and thread.
 * Parameters: HANDLE      hThread         handle of thread that contains selector
 *             DWORD       dwSelector      number of selector value to look up
 *             LPLDT_ENTRY lpSelectorEntry address of selector entry structure
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetThreadSelectorEntry(HANDLE      hThread,
                                     DWORD       dwSelector,
                                     LPLDT_ENTRY lpSelectorEntry)
{
  dprintf(("KERNEL32: GetThreadSelectorEntry(%08xh,%08xh,%08xh) not implemented.\n",
           hThread,
           dwSelector,
           lpSelectorEntry));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetThreadTimes
 * Purpose   : The GetThreadTimes function obtains timing information about a specified thread.
 * Parameters: HANDLE     hThread       specifies the thread of interest
 *             LPFILETIME lpCreationTime when the thread was created
 *             LPFILETIME lpExitTime     when the thread exited
 *             LPFILETIME lpKernelTime   time the thread has spent in kernel mode
 *             LPFILETIME lpUserTime     time the thread has spent in user mode
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetThreadTimes(HANDLE     hThread,
                                LPFILETIME lpCreationTime,
                                LPFILETIME lpExitTime,
                                LPFILETIME lpKernelTime,
                                LPFILETIME lpUserTime)
{
  dprintf(("Kernel32: GetThreadTimes(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hThread,
           lpCreationTime,
           lpExitTime,
           lpKernelTime,
           lpUserTime));

  return (FALSE);
}


/*****************************************************************************
 * Name      : HANDLE OpenFileMappingA
 * Purpose   : The OpenFileMappingA function opens a named file-mapping object.
 * Parameters: DWORD   dwDesiredAccess access mode
 *             BOOL    bInheritHandle  inherit flag
 *             LPCTSTR lpName          address of name of file-mapping object
 * Variables :
 * Result    : HANDLE to map object
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

HANDLE WIN32API OpenFileMappingA(DWORD   dwDesiredAccess,
                                    BOOL    bInheritHandle,
                                    LPCTSTR lpName)
{
  dprintf(("Kernel32: OpenFileMappingA(%08xh,%08xh,%s) not implemented.\n",
           dwDesiredAccess,
           bInheritHandle,
           lpName));

  return (0);
}


/*****************************************************************************
 * Name      : HANDLE OpenFileMappingW
 * Purpose   : The OpenFileMappingW function opens a named file-mapping object.
 * Parameters: DWORD   dwDesiredAccess access mode
 *             BOOL    bInheritHandle  inherit flag
 *             LPCTSTR lpName          address of name of file-mapping object
 * Variables :
 * Result    : HANDLE to map object
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

HANDLE WIN32API OpenFileMappingW(DWORD   dwDesiredAccess,
                                    BOOL    bInheritHandle,
                                    LPCWSTR lpName)
{
  dprintf(("Kernel32: OpenFileMappingW(%08xh,%08xh,%s) not implemented.\n",
           dwDesiredAccess,
           bInheritHandle,
           lpName));

  return (0);
}


/*****************************************************************************
 * Name      : BOOL PostQueuedCompletionStatus
 * Purpose   : The PostQueuedCompletionStatus function lets you post an I/O
 *             completion packet to an I/O completion port. The I/O completion
 *             packet will satisfy an outstanding call to the GetQueuedCompletionStatus
 *             function. The GetQueuedCompletionStatus function returns with the three
 *             values passed as the second, third, and fourth parameters of the call
 *             to PostQueuedCompletionStatus.
 * Parameters: HANDLE       CompletionPort             handle to an I/O completion port
 *             DWORD        dwNumberOfBytesTransferred
 *             DWORD        dwCompletionKey
 *             LPOVERLAPPED lpOverlapped
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API PostQueuedCompletionStatus(HANDLE       CompletionPort,
                                            DWORD        dwNumberOfBytesTransferred,
                                            DWORD        dwCompletionKey,
                                            LPOVERLAPPED lpOverlapped)
{
  dprintf(("Kernel32: PostQueuedCompletionStatus(%08xh,%08xh,%08xh,%08xh) not implemented.",
           CompletionPort,
           dwNumberOfBytesTransferred,
           dwCompletionKey,
           lpOverlapped));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD PrepareTape
 * Purpose   : The PrepareTape function prepares the tape to be accessed or removed.
 * Parameters: HANDLE hDevice      handle of open device
 *             DWORD  dwOperation  preparation method
 *             BOOL   bImmediate   return after operation begins
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API PrepareTape(HANDLE hDevice,
                              DWORD  dwOperation,
                              BOOL   bImmediate)
{
  dprintf(("Kernel32: PrepareTape(%08xh,%08xh,%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice,
           dwOperation,
           bImmediate));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : DWORD QueryDosDeviceA
 * Purpose   : The QueryDosDevice function lets an application obtain information
 *             about MS-DOS device names. The function can obtain the current
 *             mapping for a particular MS-DOS device name. The function can also
 *             obtain a list of all existing MS-DOS device names.
 *             MS-DOS device names are stored as symbolic links in the Windows NT
 *             object name space. The code that converts an MS-DOS path into a
 *             corresponding Windows NT path uses these symbolic links to map
 *             MS-DOS devices and drive letters. The QueryDosDevice function
 *             provides a mechanism whereby a Win32-based application can query
 *             the names of the symbolic links used to implement the MS-DOS device
 *             namespace as well as the value of each specific symbolic link.
 * Parameters: LPCTSTR lpDeviceName address of MS-DOS device name string
 *             LPTSTR  lpTargetPath ddress of buffer for storing query results
 *             DWORD   ucchMax       maximum storage capacity of buffer
 * Variables :
 * Result    : pointer to lpTargetPath
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API QueryDosDeviceA(LPCTSTR lpDeviceName,
                               LPTSTR  lpTargetPath,
                               DWORD   ucchMax)
{
  dprintf(("Kernel32: QueryDosDeviceA(%s,%08xh,%08xh) not implemented.\n",
           lpDeviceName,
           lpTargetPath,
           ucchMax));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD QueryDosDeviceW
 * Purpose   : The QueryDosDevice function lets an application obtain information
 *             about MS-DOS device names. The function can obtain the current
 *             mapping for a particular MS-DOS device name. The function can also
 *             obtain a list of all existing MS-DOS device names.
 *             MS-DOS device names are stored as symbolic links in the Windows NT
 *             object name space. The code that converts an MS-DOS path into a
 *             corresponding Windows NT path uses these symbolic links to map
 *             MS-DOS devices and drive letters. The QueryDosDevice function
 *             provides a mechanism whereby a Win32-based application can query
 *             the names of the symbolic links used to implement the MS-DOS device
 *             namespace as well as the value of each specific symbolic link.
 * Parameters: LPCTSTR lpDeviceName address of MS-DOS device name string
 *             LPTSTR  lpTargetPath ddress of buffer for storing query results
 *             DWORD   ucchMax       maximum storage capacity of buffer
 * Variables :
 * Result    : pointer to lpTargetPath
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API QueryDosDeviceW(LPCWSTR lpDeviceName,
                               LPWSTR  lpTargetPath,
                               DWORD   ucchMax)
{
  dprintf(("Kernel32: QueryDosDeviceW(%s,%08xh,%08xh) not implemented.\n",
           lpDeviceName,
           lpTargetPath,
           ucchMax));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD ReadProcessMemory
 * Purpose   : The ReadProcessMemory function reads memory in a specified process.
 *             The entire area to be read must be accessible, or the operation fails.
 * Parameters: HANDLE  hProcess            handle of the process whose memory is read
 *             LPCVOID lpBaseAddress       address to start reading
 *             LPVOID  lpBuffer            address of buffer to place read data
 *             DWORD   cbRead              number of bytes to read
 *             LPDWORD lpNumberOfBytesRead address of number of bytes read
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API ReadProcessMemory(HANDLE  hProcess,
                                    LPCVOID lpBaseAddress,
                                    LPVOID  lpBuffer,
                                    DWORD   cbRead,
                                    LPDWORD lpNumberOfBytesRead)
{
  dprintf(("Kernel32: ReadProcessMemory(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hProcess,
           lpBaseAddress,
           lpBuffer,
           cbRead,
           lpNumberOfBytesRead));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetComputerNameA
 * Purpose   : The SetComputerNameA function sets the computer name to be used
 *             the next time the system is restarted.
 * Parameters: LPCTSTR lpszName address of new computer name
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetComputerNameA(LPCTSTR lpszName)
{
  dprintf(("Kernel32: SetComputerNameA(%s) not implemented.\n",
           lpszName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetComputerNameW
 * Purpose   : The SetComputerNameW function sets the computer name to be used
 *             the next time the system is restarted.
 * Parameters: LPCTSTR lpszName address of new computer name
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetComputerNameW(LPCWSTR lpszName)
{
  dprintf(("Kernel32: SetComputerNameW(%s) not implemented.\n",
           lpszName));

  return (FALSE);
}


/*****************************************************************************
 * Name      : VOID SetFileApisToOEM
 * Purpose   : The SetFileApisToOEM function causes a set of Win32 file functions
 *             to use the OEM character set code page. This function is useful
 *             for 8-bit console input and output operations.
 * Parameters: VOID
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

VOID WIN32API SetFileApisToOEM(VOID)
{
  dprintf(("Kernel32: SetFileApisToOEM() not implemented.\n"));
}


/*****************************************************************************
 * Name      : BOOL SetHandleInformation
 * Purpose   : The SetHandleInformation function sets certain properties of an
 *             object handle. The information is specified as a set of bit flags.
 * Parameters: HANDLE hObject  handle to an object
 *             DWORD  dwMask   specifies flags to change
 *             DWORD  dwFlags  specifies new values for flags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetHandleInformation(HANDLE hObject,
                                      DWORD  dwMask,
                                      DWORD  dwFlags)
{
  dprintf(("KERNEL32: SetHandleInformation(%08xh,%08xh,%08xh) not implemented.\n",
           hObject,
           dwMask,
           dwFlags));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetMailslotInfo
 * Purpose   : The SetMailslotInfo function sets the time-out value used by the
 *             specified mailslot for a read operation.
 * Parameters: HANDLE hObject       handle to a mailslot object
 *             DWORD  dwReadTimeout read time-out
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetMailslotInfo(HANDLE hObject,
                                 DWORD  dwReadTimeout)
{
  dprintf(("KERNEL32: SetMailslotInfo(%08xh,%08xh) not implemented.\n",
           hObject,
           dwReadTimeout));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetNamedPipeHandleState
 * Purpose   : The SetNamedPipeHandleState function sets the read mode and the
 *             blocking mode of the specified named pipe. If the specified handle
 *             is to the client end of a named pipe and if the named pipe server
 *             process is on a remote computer, the function can also be used to
 *             control local buffering.
 * Parameters: HANDLE  hNamedPipe     handle of named pipe
 *             LPDWORD lpdwMode       address of new pipe mode
 *             LPDWORD lpcbMaxCollect address of max. bytes before remote transmission
 *             LPDWORD lpdwCollectDataTimeout address of max. time before remote transmission
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetNamedPipeHandleState(HANDLE  hNamedPipe,
                                         LPDWORD lpdwMode,
                                         LPDWORD lpcbMaxCollect,
                                         LPDWORD lpdwCollectDataTimeout)
{
  dprintf(("KERNEL32: SetNamedPipeHandleState(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hNamedPipe,
           lpdwMode,
           lpcbMaxCollect,
           lpdwCollectDataTimeout));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetProcessShutdownParameters
 * Purpose   : The SetProcessShutdownParameters function sets shutdown parameters
 *             for the currently calling process. This function sets a shutdown
 *             order for a process relative to the other processes in the system.
 * Parameters: DWORD dwLevel  shutdown priority
 *             DWORD dwFlags  shutdown flags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetProcessShutdownParameters(DWORD dwLevel,
                                              DWORD dwFlags)
{
  dprintf(("KERNEL32: SetProcessShutdownParameters(%08xh,%08xh) not implemented.\n",
           dwLevel,
           dwFlags));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetProcessWorkingSetSize
 * Purpose   : The SetProcessWorkingSetSize function sets the minimum and
 *             maximum working set sizes for a specified process.
 *             The working set of a process is the set of memory pages currently
 *             visible to the process in physical RAM memory. These pages are
 *             resident and available for an application to use without triggering
 *             a page fault. The size of the working set of a process is specified
 *             in bytes. The minimum and maximum working set sizes affect the
 *             virtual memory paging behavior of a process.
 * Parameters: HANDLE hProcess                open handle to the process of interest
 *             DWORD  dwMinimumWorkingSetSize specifies minimum working set size
 *             DWORD  dwMaximumWorkingSetSize specifies maximum working set size
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetProcessWorkingSetSize(HANDLE hProcess,
                                          DWORD  dwMinimumWorkingSetSize,
                                          DWORD  dwMaximumWorkingSetSize)
{
  dprintf(("KERNEL32: SetProcessWorkingSetSize(%08xh,%08xh,%08xh) not implemented.\n",
           hProcess,
           dwMinimumWorkingSetSize,
           dwMaximumWorkingSetSize));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetSystemPowerState
 * Purpose   : The SetSystemPowerState function suspends the system by shutting
 *             power down. Depending on the ForceFlag parameter, the function
 *             either suspends operation immediately or requests permission from
 *             all applications and device drivers before doing so.
 * Parameters: BOOL fSuspend
 *             BOOL fForce
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetSystemPowerState(BOOL fSuspend,
                                     BOOL fForce)
{
  dprintf(("KERNEL32: SetSystemPowerState(%08xh,%08xh) not implemented.\n",
           fSuspend,
           fForce));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL SetSystemTimeAdjustment
 * Purpose   : The SetSystemTimeAdjustment function tells the system to enable
 *             or disable periodic time adjustments to its time of day clock.
 *             Such time adjustments are used to synchronize the time of day
 *             with some other source of time information. When periodic time
 *             adjustments are enabled, they are applied at each clock interrupt.
 * Parameters: DWORD dwTimeAdjustment
 *             BOOL  bTimeAdjustmentDisabled
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetSystemTimeAdjustment(DWORD dwTimeAdjustment,
                                         BOOL  bTimeAdjustmentDisabled)
{
  dprintf(("KERNEL32: SetSystemTimeAdjustment(%08xh,%08xh) not implemented.\n",
           dwTimeAdjustment,
           bTimeAdjustmentDisabled));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD SetTapeParameters
 * Purpose   : The SetTapeParameters function either specifies the block size
 *             of a tape or configures the tape device.
 * Parameters: HANDLE hDevice           handle of open device
 *             DWORD  dwOperation       type of information to set
 *             LPVOID lpTapeInformation address of buffer with information to set
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetTapeParameters(HANDLE hDevice,
                                   DWORD  dwOperation,
                                   LPVOID lpTapeInformation)
{
  dprintf(("KERNEL32: SetTapeParameters(%08xh,%08xh,%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice,
           dwOperation,
           lpTapeInformation));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : DWORD SetTapePosition
 * Purpose   : The SetTapePosition sets the tape position on the specified device.
 * Parameters: HANDLE hDevice          handle of open device
 *             DWORD  dwPositionMethod type of positioning to perform
 *             DWORD  dwPartition      new tape partition
 *             DWORD  dwOffsetLow      low-order 32 bits of tape position
 *             DWORD  dwOffsetHigh     high-order 32 bits of tape position
 *             BOOL   bImmediate       return after operation begins
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetTapePosition(HANDLE hDevice,
                                 DWORD  dwPositionMethod,
                                 DWORD  dwPartition,
                                 DWORD  dwOffsetLow,
                                 DWORD  dwOffsetHigh,
                                 BOOL   bImmediate)
{
  dprintf(("KERNEL32: SetTapePosition(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice,
           dwPositionMethod,
           dwPartition,
           dwOffsetLow,
           dwOffsetHigh,
           bImmediate));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : DWORD SetThreadAffinityMask
 * Purpose   : The SetThreadAffinityMask function sets a processor affinity
 *             mask for a specified thread.
 *             A thread affinity mask is a bit vector in which each bit
 *             represents the processors that a thread is allowed to run on.
 *             A thread affinity mask must be a proper subset of the process
 *             affinity mask for the containing process of a thread. A thread
 *             is only allowed to run on the processors its process is allowed to run on.
 * Parameters: HANDLE hThread              handle to the thread of interest
 *             DWORD  dwThreadAffinityMask a thread affinity mask
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API SetThreadAffinityMask(HANDLE hThread,
                                        DWORD  dwThreadAffinityMask)
{
  dprintf(("KERNEL32: SetThreadAffinityMask(%08xh,%08xh) not implemented.\n",
           hThread,
           dwThreadAffinityMask));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD TransactNamedPipe
 * Purpose   : The TransactNamedPipe function combines into a single network
 *             operation the functions that write a message to and read a
 *             message from the specified named pipe.
 * Parameters: HANDLE       hNamedPipe  handle of named pipe
 *             LPVOID       lpvWriteBuf address of write buffer
 *             DWORD        cbWriteBuf  size of the write buffer, in bytes
 *             LPVOID       lpvReadBuf  address of read buffer
 *             DWORD        cbReadBuf   size of read buffer, in bytes
 *             LPDWORD      lpcbRead    address of variable for bytes actually read
 *             LPOVERLAPPED lpo         address of overlapped structure
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API TransactNamedPipe(HANDLE       hNamedPipe,
                                    LPVOID       lpvWriteBuf,
                                    DWORD        cbWriteBuf,
                                    LPVOID       lpvReadBuf,
                                    DWORD        cbReadBuf,
                                    LPDWORD      lpcbRead,
                                    LPOVERLAPPED lpo)
{
  dprintf(("KERNEL32: TransactNamedPipe(%08x,%08x,%08x,%08x,%08x,%08x,%08x) not implemented.\n",
           hNamedPipe,
           lpvWriteBuf,
           cbWriteBuf,
           lpvReadBuf,
           cbReadBuf,
           lpcbRead,
           lpo));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL UpdateResourceA
 * Purpose   : The UpdateResourceA function adds, deletes, or replaces a resource
 *             in an executable file.
 * Parameters: HANDLE  hUpdateFile update-file handle
 *             LPCTSTR lpszType    address of resource type to update
 *             LPCTSTR lpszName    address of resource name to update
 *             WORD    IDLanguage  language identifier of resource
 *             LPVOID  lpvData     address of resource data
 *             DWORD   cbData      length of resource data, in bytes
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UpdateResourceA(HANDLE  hUpdateFile,
                                 LPCTSTR lpszType,
                                 LPCTSTR lpszName,
                                 WORD    IDLanguage,
                                 LPVOID  lpvData,
                                 DWORD   cbData)
{
  dprintf(("KERNEL32: UpdateResourceA(%08xh,%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           hUpdateFile,
           lpszType,
           lpszName,
           IDLanguage,
           lpvData,
           cbData));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL UpdateResourceW
 * Purpose   : The UpdateResourceW function adds, deletes, or replaces a resource
 *             in an executable file.
 * Parameters: HANDLE  hUpdateFile update-file handle
 *             LPCTSTR lpszType    address of resource type to update
 *             LPCTSTR lpszName    address of resource name to update
 *             WORD    IDLanguage  language identifier of resource
 *             LPVOID  lpvData     address of resource data
 *             DWORD   cbData      length of resource data, in bytes
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API UpdateResourceW(HANDLE  hUpdateFile,
                                 LPCWSTR lpszType,
                                 LPCWSTR lpszName,
                                 WORD    IDLanguage,
                                 LPVOID  lpvData,
                                 DWORD   cbData)
{
  dprintf(("KERNEL32: UpdateResourceW(%08xh,%s,%s,%08xh,%08xh,%08xh) not implemented.\n",
           hUpdateFile,
           lpszType,
           lpszName,
           IDLanguage,
           lpvData,
           cbData));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WaitForDebugEvent
 * Purpose   : The WaitForDebugEvent function waits for a debugging event to
 *             occur in a process being debugged.
 * Parameters: LPDEBUG_EVENT lpde      address of structure for event information
 *             DWORD         dwTimeout number of milliseconds to wait for event
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WaitForDebugEvent(LPDEBUG_EVENT lpde,
                                DWORD         dwTimeout)
{
  dprintf(("KERNEL32: WaitForDebugEvent(%08xh,%08xh) not implemented.\n",
           lpde,
           dwTimeout));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WaitNamedPipeA
 * Purpose   : The WaitNamedPipe function waits until either a time-out interval
 *             elapses or an instance of the specified named pipe is available
 *             to be connected to (that is, the pipe's server process has a
 *             pending ConnectNamedPipe operation on the pipe).
 * Parameters: LPCTSTR lpszNamedPipeName
 *             DWORD   dwTimeout
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WaitNamedPipeA(LPCTSTR lpszNamedPipeName,
                                DWORD   dwTimeout)
{
  dprintf(("KERNEL32: WaitNamedPipeA(%s, %u) not implemented.\n",
           lpszNamedPipeName,
           dwTimeout));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WaitNamedPipeW
 * Purpose   : The WaitNamedPipe function waits until either a time-out interval
 *             elapses or an instance of the specified named pipe is available
 *             to be connected to (that is, the pipe's server process has a
 *             pending ConnectNamedPipe operation on the pipe).
 * Parameters: LPCWSTR lpszNamedPipeName
 *             DWORD   dwTimeout
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API WaitNamedPipeW(LPCWSTR lpszNamedPipeName,
                                DWORD   dwTimeout)
{
  dprintf(("KERNEL32: WaitNamedPipeW(%s, %u) not implemented.\n",
           lpszNamedPipeName,
           dwTimeout));

  return (FALSE);
}


/*****************************************************************************
 * Name      : DWORD WriteProcessMemory
 * Purpose   : The WriteProcessMemory function writes memory in a specified
 *             process. The entire area to be written to must be accessible,
 *             or the operation fails.
 * Parameters: HANDLE  hProcess               handle of process whose memory is written to
 *             LPVOID  lpBaseAddress          address to start writing to
 *             LPVOID  lpBuffer               address of buffer to write data to
 *             DWORD   cbWrite                number of bytes to write
 *             LPDWORD lpNumberOfBytesWritten actual number of bytes written
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API WriteProcessMemory(HANDLE  hProcess,
                                     LPCVOID lpBaseAddress,
                                     LPVOID  lpBuffer,
                                     DWORD   cbWrite,
                                     LPDWORD lpNumberOfBytesWritten)
{
  dprintf(("Kernel32: WriteProcessMemory(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hProcess,
           lpBaseAddress,
           lpBuffer,
           cbWrite,
           lpNumberOfBytesWritten));

  return (FALSE);
}


/*****************************************************************************
 * Name      : BOOL WriteTapemark
 * Purpose   : The WriteTapemark function writes a specified number of filemarks,
 *             setmarks, short filemarks, or long filemarks to a tape device.
 *             These tapemarks divide a tape partition into smaller areas.
 * Parameters: HANDLE hDevice         handle of open device
 *             DWORD  dwTapemarkType  type of tapemarks to write
 *             DWORD  dwTapemarkCount number of tapemarks to write
 *             BOOL   bImmediate      return after write begins
 * Variables :
 * Result    : API returncode
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

DWORD WIN32API WriteTapemark(HANDLE hDevice,
                                DWORD  dwTapemarkType,
                                DWORD  dwTapemarkCount,
                                BOOL   bImmediate)
{
  dprintf(("KERNEL32: WriteTapemark(%08xh,%08xh,%08xh,%08xh) not implemented - ERROR_NOT_SUPPORTED.\n",
           hDevice,
           dwTapemarkType,
           dwTapemarkCount,
           bImmediate));

  return (ERROR_NOT_SUPPORTED);
}


/*****************************************************************************
 * Name      : DWORD CmdBatNotification
 * Purpose   : Unknown, used by /winnt/cmd.exe
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/08 21:44]
 *****************************************************************************/

DWORD WIN32API CmdBatNotification(DWORD x1)
{
  dprintf(("KERNEL32: CmdBatNotification(%08xh) not implemented\n",
           x1));

  return (0);
}


/*****************************************************************************
 * Name      : DWORD GetVDMCurrentDirectories
 * Purpose   : Unknown, used by /winnt/cmd.exe
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/08 21:44]
 *****************************************************************************/

DWORD WIN32API GetVDMCurrentDirectories(DWORD x1)
{
  dprintf(("KERNEL32: GetVDMCurrentDirectories(%08xh) not implemented\n",
           x1));

  return (0);
}


