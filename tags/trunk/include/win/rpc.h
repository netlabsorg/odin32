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

typedef void * I_RPC_HANDLE;
typedef long RPC_STATUS;

#define RPC_UNICODE_SUPPORTED
#define __RPC_FAR
#define __RPC_API  __stdcall
#define __RPC_USER __stdcall
#define __RPC_STUB __stdcall
#define RPC_ENTRY  __stdcall


RPC_STATUS RPC_ENTRY RpcImpersonateClient(RPC_BINDING_HANDLE BindingHandle);
RPC_STATUS RPC_ENTRY RpcRevertToSelfEx(RPC_BINDING_HANDLE BindingHandle);
RPC_STATUS RPC_ENTRY RpcRevertToSelf();
long       RPC_ENTRY I_RpcMapWin32Status(RPC_STATUS Status);

#ifdef __cplusplus
}
#endif

#endif

