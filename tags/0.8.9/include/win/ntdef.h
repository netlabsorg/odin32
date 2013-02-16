#ifndef __WINE_NTDEF_H
#define __WINE_NTDEF_H

/* fixme: include basestd.h instead */
#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NTAPI   __stdcall

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef WINE_NTSTATUS_DECLARED
#define WINE_NTSTATUS_DECLARED
typedef LONG NTSTATUS;
#endif
#ifndef WINE_PNTSTATUS_DECLARED
#define WINE_PNTSTATUS_DECLARED
typedef NTSTATUS *PNTSTATUS;
#endif

typedef char *PSZ;
typedef CONST char *PCSZ;

typedef short CSHORT;
typedef CSHORT *PCSHORT;

/* NT lowlevel Strings (handled by Rtl* functions in NTDLL)
 * If they are zero terminated, Length does not include the terminating 0.
 */

typedef struct _STRING {
	USHORT	Length;
	USHORT	MaximumLength;
	PSTR	Buffer;
} STRING,*PSTRING,ANSI_STRING,*PANSI_STRING, OEM_STRING, *POEM_STRING;
/* @@@PH 1999/06/08 OEM-String support is experimental */

typedef struct _CSTRING {
	USHORT	Length;
	USHORT	MaximumLength;
	PCSTR	Buffer;
} CSTRING,*PCSTRING;

#ifndef __UNICODE_STRING_DEFINED__
#define __UNICODE_STRING_DEFINED__
typedef struct _UNICODE_STRING {
	USHORT	Length;		/* bytes */
	USHORT	MaximumLength;	/* bytes */
	PWSTR	Buffer;
} UNICODE_STRING,*PUNICODE_STRING;
#endif
/*
	Objects
*/

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_VALID_ATTRIBUTES    0x000003F2L

typedef struct _OBJECT_ATTRIBUTES
{   ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        /* type SECURITY_DESCRIPTOR */
    PVOID SecurityQualityOfService;  /* type SECURITY_QUALITY_OF_SERVICE */
} OBJECT_ATTRIBUTES;

typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

#ifdef __cplusplus
}
#endif

#endif
