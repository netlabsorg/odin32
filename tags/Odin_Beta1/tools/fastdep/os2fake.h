/* $Id: os2fake.h,v 1.1 2001-03-14 20:14:34 bird Exp $
 * 
 * Structures, defines and function prototypes for the OS/2 fake library.
 * 
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
 
 
#ifndef _os2fake_h_
#define _os2fake_h_

                    
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifndef OS2ENTRY
#define OS2ENTRY    
#endif

#ifndef CCHMAXPATHCOMP
#define CCHMAXPATHCOMP          256
#endif

#ifndef CCHMAXPATH
#define CCHMAXPATH              260
#endif

#ifndef FIL_STANDARD
#define FIL_STANDARD            1
#define FIL_QUERYEASIZE         2
#define FIL_QUERYEASFROMLIST    3
#define FIL_QUERYFULLNAME       5
#endif
   
#define FILE_NORMAL             0x0000
#define FILE_READONLY           0x0001
#define FILE_HIDDEN             0x0002
#define FILE_SYSTEM             0x0004
#define FILE_DIRECTORY          0x0010
#define FILE_ARCHIVED           0x0020

#ifndef HDIR_CREATE
#define HDIR_CREATE             (-1)
#endif

#ifndef TRUE
#define TRUE                    1
#endif
#ifndef FALSE
#define FALSE                   0
#endif

#ifndef NO_ERROR
#define NO_ERROR                0
#endif



/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef char *          PCH;

typedef char *          PSZ;
typedef const char *    PCSZ;

typedef unsigned long   ULONG;
typedef ULONG *         PULONG;

typedef unsigned short  USHORT;
typedef USHORT *        PUSHORT;

#if !defined(_WINDEF_)
typedef unsigned int    UINT;
typedef UINT *          PUINT;

typedef unsigned char   UCHAR;
typedef UCHAR *         PUCHAR;

#if !defined(CHAR)
typedef char            CHAR;
typedef CHAR *          PCHAR;
#endif

typedef unsigned long   BOOL;
typedef BOOL *          PBOOL;
#endif

#if !defined(VOID)
typedef void            VOID;
#endif
#if !defined(_WINNT_) && !defined(PVOID)
typedef VOID *          PVOID;  
#endif

typedef unsigned long   HDIR;
typedef HDIR *          PHDIR;

typedef unsigned long   APIRET;


typedef struct _FTIME   /* ftime */
{
#if defined(__IBMC__) || defined(__IBMCPP__)
    UINT    twosecs : 5;
    UINT    minutes : 6;
    UINT    hours   : 5;
#else
    USHORT  twosecs : 5;
    USHORT  minutes : 6;
    USHORT  hours   : 5;
#endif
} FTIME;
typedef FTIME *PFTIME;


typedef struct _FDATE   /* fdate */
{
#if defined(__IBMC__) || defined(__IBMCPP__)
    UINT    day     : 5;
    UINT    month   : 4;
    UINT    year    : 7;
#else
    USHORT  day     : 5;
    USHORT  month   : 4;
    USHORT  year    : 7;
#endif
} FDATE;
typedef FDATE   *PFDATE;


typedef struct _FILESTATUS3     /* fsts3 */
{
    FDATE   fdateCreation;
    FTIME   ftimeCreation;
    FDATE   fdateLastAccess;
    FTIME   ftimeLastAccess;
    FDATE   fdateLastWrite;
    FTIME   ftimeLastWrite;
    ULONG   cbFile;
    ULONG   cbFileAlloc;
    ULONG   attrFile;
} FILESTATUS3;
typedef FILESTATUS3 *PFILESTATUS3;

typedef struct _FILEFINDBUF3    /* findbuf3 */
{
    ULONG   oNextEntryOffset;    
    FDATE   fdateCreation;
    FTIME   ftimeCreation;
    FDATE   fdateLastAccess;
    FTIME   ftimeLastAccess;
    FDATE   fdateLastWrite;
    FTIME   ftimeLastWrite;
    ULONG   cbFile;
    ULONG   cbFileAlloc;
    ULONG   attrFile;            
    UCHAR   cchName;
    CHAR    achName[CCHMAXPATHCOMP];
} FILEFINDBUF3;
typedef FILEFINDBUF3 *PFILEFINDBUF3;


/*******************************************************************************
*   Function Prototypes                                                        *
*******************************************************************************/
APIRET OS2ENTRY         DosQueryPathInfo(
                            PCSZ        pszPathName,
                            ULONG       ulInfoLevel,
                            PVOID       pInfoBuf,
                            ULONG       cbInfoBuf);

APIRET OS2ENTRY         DosFindFirst(
                            PCSZ        pszFileSpec,
                            PHDIR       phdir,
                            ULONG       flAttribute,
                            PVOID       pFindBuf,
                            ULONG       cbFindBuf,
                            PULONG      pcFileNames,
                            ULONG       ulInfoLevel);

APIRET OS2ENTRY         DosFindNext(
                            HDIR        hDir,
                            PVOID       pFindBuf,
                            ULONG       cbFindBuf,
                            PULONG      pcFileNames);

APIRET OS2ENTRY         DosFindClose(
                            HDIR        hDir);



#endif

