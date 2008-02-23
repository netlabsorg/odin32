#ifndef RPC_NO_WINDOWS_H
#include <windows.h>
#endif

#ifndef __RPC_H__
#define __RPC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define __RPC_WIN32__
#define __RPC_NT__

#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate MIDL_user_allocate
#define midl_user_free     MIDL_user_free
#define __MIDL_USER_DEFINED
#endif

#define RPC_UNICODE_SUPPORTED
#define __RPC_FAR
#define __RPC_API  
#define __RPC_USER 
#define __RPC_STUB 
#define RPC_ENTRY  __stdcall

#include "rpcdce.h"

#ifdef __cplusplus
}
#endif

#endif

