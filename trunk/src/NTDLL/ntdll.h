/* $Id: ntdll.h,v 1.1 1999-05-24 20:19:32 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 NT Runtime for OS/2
 *
 * 1998/05/20 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) ntdll.cpp     	1.0.0   1998/05/20 PH Start from WINE/NTDLL.C
 *
 * NT basis DLL
 * 
 * Copyright 1996 Marcus Meissner
 * Copyright 1988 Patrick Haller (adapted for win32os2)
 */

 /* Changes to the original NTDLL.C from the WINE project
  
  - includes replaced by the win32os2 standard includes
  - replaced WINAPI by WIN32API
  - moved in some string functions
  - replaced HANDLE32 by HANDLE
  - lstrlen32A -> OS2lstrlenA
  - lstrlen32W -> OS2lstrlenW
*/

#ifndef _WINNT_
#define _WINNT_

#ifdef __cplusplus
extern "C" {
#endif
  
  
#include "win32type.h"
  
typedef unsigned short int UINT16;
typedef unsigned long  int UINT32;
typedef          short int INT16;
typedef          long  int INT32;
  
  
/* Security Ids of NT */
typedef struct {
	BYTE	Value[6];
} SID_IDENTIFIER_AUTHORITY,*PSID_IDENTIFIER_AUTHORITY,*LPSID_IDENTIFIER_AUTHORITY;

typedef struct _SID {
	BYTE	Revision;
	BYTE	SubAuthorityCount;
	SID_IDENTIFIER_AUTHORITY	IdentifierAuthority;
	DWORD	SubAuthority[1];
} SID,*PSID,*LPSID;

#define	SID_REVISION			(1)	/* Current revision */
#define	SID_MAX_SUB_AUTHORITIES		(15)	/* current max subauths */
#define	SID_RECOMMENDED_SUB_AUTHORITIES	(1)	/* recommended subauths */

/* ACLs of NT */

#define	ACL_REVISION	2

#define	ACL_REVISION1	1
#define	ACL_REVISION2	2

typedef struct _ACL {
	BYTE	AclRevision;
	BYTE	Sbz1;
	WORD	AclSize;
	WORD	AceCount;
	WORD	Sbz2;
} ACL,*LPACL;

/* ACEs, directly starting after an ACL */
typedef struct _ACE_HEADER {
	BYTE	AceType;
	BYTE	AceFlags;
	WORD	AceSize;
} ACE_HEADER,*LPACE_HEADER;

/* AceType */
#define	ACCESS_ALLOWED_ACE_TYPE		0
#define	ACCESS_DENIED_ACE_TYPE		1
#define	SYSTEM_AUDIT_ACE_TYPE		2
#define	SYSTEM_ALARM_ACE_TYPE		3

/* inherit AceFlags */
#define	OBJECT_INHERIT_ACE		0x01
#define	CONTAINER_INHERIT_ACE		0x02
#define	NO_PROPAGATE_INHERIT_ACE	0x04
#define	INHERIT_ONLY_ACE		0x08
#define	VALID_INHERIT_FLAGS		0x0F

/* AceFlags mask for what events we (should) audit */
#define	SUCCESSFUL_ACCESS_ACE_FLAG	0x40
#define	FAILED_ACCESS_ACE_FLAG		0x80

/* different ACEs depending on AceType 
 * SidStart marks the begin of a SID
 * so the thing finally looks like this:
 * 0: ACE_HEADER
 * 4: ACCESS_MASK
 * 8... : SID
 */
typedef struct _ACCESS_ALLOWED_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} ACCESS_ALLOWED_ACE,*LPACCESS_ALLOWED_ACE;

typedef struct _ACCESS_DENIED_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} ACCESS_DENIED_ACE,*LPACCESS_DENIED_ACE;

typedef struct _SYSTEM_AUDIT_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} SYSTEM_AUDIT_ACE,*LPSYSTEM_AUDIT_ACE;

typedef struct _SYSTEM_ALARM_ACE {
	ACE_HEADER	Header;
	DWORD		Mask;
	DWORD		SidStart;
} SYSTEM_ALARM_ACE,*LPSYSTEM_ALARM_ACE;

#define	SECURITY_DESCRIPTOR_REVISION	1
#define	SECURITY_DESCRIPTOR_REVISION1	1

typedef WORD SECURITY_DESCRIPTOR_CONTROL;

#define	SE_OWNER_DEFAULTED	0x0001
#define	SE_GROUP_DEFAULTED	0x0002
#define	SE_DACL_PRESENT		0x0004
#define	SE_DACL_DEFAULTED	0x0008
#define	SE_SACL_PRESENT		0x0010
#define	SE_SACL_DEFAULTED	0x0020
#define	SE_SELF_RELATIVE	0x8000

typedef struct {
	BYTE	Revision;
	BYTE	Sbz1;
	SECURITY_DESCRIPTOR_CONTROL Control;
	LPSID	Owner;
	LPSID	Group;
	LPACL	Sacl;
	LPACL	Dacl;
} SECURITY_DESCRIPTOR,*PSECURITY_DESCRIPTOR,*LPSECURITY_DESCRIPTOR;


typedef enum tagSID_NAME_USE {
	SidTypeUser = 1,
	SidTypeGroup,
	SidTypeDomain,
	SidTypeAlias,
	SidTypeWellKnownGroup,
	SidTypeDeletedAccount,
	SidTypeInvalid,
	SidTypeUnknown
} SID_NAME_USE,*PSID_NAME_USE,*LPSID_NAME_USE;

/* NT lowlevel Strings (handled by Rtl* functions in NTDLL)
 * If they are zero terminated, Length does not include the terminating 0.
 */

typedef struct _STRING {
	UINT16	Length;
	UINT16	MaximumLength;
	LPSTR	Buffer;
} STRING,*LPSTRING,ANSI_STRING,*LPANSI_STRING;

typedef struct _CSTRING {
	UINT16	Length;
	UINT16	MaximumLength;
	LPCSTR	Buffer;
} CSTRING,*LPCSTRING;

typedef struct _UNICODE_STRING {
	UINT16	Length;		/* bytes */
	UINT16	MaximumLength;	/* bytes */
	LPWSTR	Buffer;
} UNICODE_STRING,*LPUNICODE_STRING;


BOOL32 WIN32API IsValidSid(LPSID);
BOOL32 WIN32API EqualSid(LPSID,LPSID);
BOOL32 WIN32API EqualPrefixSid(LPSID,LPSID);
DWORD  WIN32API GetSidLengthRequired(BYTE);
BOOL32 WIN32API AllocateAndInitializeSid(LPSID_IDENTIFIER_AUTHORITY,BYTE,DWORD,
                                       DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,
                                       DWORD,LPSID*);
VOID*  WIN32API FreeSid(LPSID);
BOOL32 WIN32API InitializeSecurityDescriptor(SECURITY_DESCRIPTOR*,DWORD);
BOOL32 WIN32API InitializeSid(LPSID,LPSID_IDENTIFIER_AUTHORITY,BYTE);
DWORD* WIN32API GetSidSubAuthority(LPSID,DWORD);
BYTE * WIN32API GetSidSubAuthorityCount(LPSID);
DWORD  WIN32API GetLengthSid(LPSID);
BOOL32 WIN32API CopySid(DWORD,LPSID,LPSID);
BOOL32 WIN32API LookupAccountSid32A(LPCSTR,PSID,LPCSTR,LPDWORD,LPCSTR,LPDWORD,
                                  PSID_NAME_USE);
BOOL32 WIN32API LookupAccountSid32W(LPCWSTR,PSID,LPCWSTR,LPDWORD,LPCWSTR,LPDWORD,
                                  PSID_NAME_USE);
LPSID_IDENTIFIER_AUTHORITY WIN32API GetSidIdentifierAuthority(LPSID);
  
  

#ifdef __cplusplus
}
#endif

#endif /* _WINNT_ */
