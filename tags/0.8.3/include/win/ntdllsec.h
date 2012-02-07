/* $$ */

#ifndef __NTDLL_SEC_H__
#define __NTDLL_SEC_H__

#ifdef OS2_INCLUDED
#include <win32type.h>
#else
#include <winnt.h>
#endif

//SvL: Security data for win32 process (nothing fancy; just trying to fool apps
//     into thinking the security subsystem is in perfect shape.
#define SECTYPE_THREAD		0
#define SECTYPE_PROCESS		1
#define SECTYPE_INITIALIZED     2

typedef struct {
 DWORD               dwType;
 TOKEN_USER          SidUser;
 TOKEN_GROUPS       *pTokenGroups;
 PRIVILEGE_SET      *pPrivilegeSet;
 TOKEN_PRIVILEGES   *pTokenPrivileges;
 TOKEN_OWNER         TokenOwner;
 TOKEN_PRIMARY_GROUP PrimaryGroup;
 TOKEN_DEFAULT_DACL  DefaultDACL;
 TOKEN_SOURCE        TokenSource;
 TOKEN_TYPE          TokenType;
} PROCESSTHREAD_SECURITYINFO;

//Per process info; Should probably be stored in process database structure
extern PROCESSTHREAD_SECURITYINFO ProcSecInfo;

#endif // __NTDLL_SEC_H__
