/* $Id: server.cpp,v 1.1 2000-04-04 19:49:03 davidr Exp $ */

/*
 * RPCRT4 library
 * Stubs
 *
 * 2000/04/03
 * 
 * Copyright 2000 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#include "rpcrt4.h"

/* server */
// ----------------------------------------------------------------------
// RpcServerInqBindings
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerInqBindings (
    OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerInqIf
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerInqIf (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid, OPTIONAL
    OUT RPC_MGR_EPV __RPC_FAR * __RPC_FAR * MgrEpv
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerListen
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerListen (
    IN unsigned int MinimumCallThreads,
    IN unsigned int MaxCalls,
    IN unsigned int DontWait
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerRegisterIf
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerRegisterIf (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid OPTIONAL,
    IN RPC_MGR_EPV __RPC_FAR * MgrEpv OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerRegisterIfEx
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerRegisterIfEx (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid,
    IN RPC_MGR_EPV __RPC_FAR * MgrEpv,
    IN unsigned int Flags,
    IN unsigned int MaxCalls,
    IN RPC_IF_CALLBACK_FN __RPC_FAR *IfCallback
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerUnregisterIf
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUnregisterIf (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid, OPTIONAL
    IN unsigned int WaitForCallsToComplete
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerUseAllProtseqs
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseAllProtseqs (
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerUseAllProtseqsEx
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseAllProtseqsEx (
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerUseAllProtseqsIf
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseAllProtseqsIf (
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcServerUseAllProtseqsIfEx
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseAllProtseqsIfEx (
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



/* server */

// ----------------------------------------------------------------------
// RpcServerUseProtseqA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqExA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqExA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqExW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqExW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqEpA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqEpA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned char __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqEpExA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqEpExA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned char __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqEpW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqEpW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned short __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqEpExW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqEpExW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN unsigned short __RPC_FAR * Endpoint,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqIfA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqIfA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqIfExA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqIfExA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqIfW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqIfW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerUseProtseqIfExW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerUseProtseqIfExW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcServerInqDefaultPrincNameA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerInqDefaultPrincNameA (
    IN unsigned long AuthnSvc,
    OUT unsigned char __RPC_FAR * __RPC_FAR * PrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerInqDefaultPrincNameW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerInqDefaultPrincNameW (
    IN unsigned long AuthnSvc,
    OUT unsigned short __RPC_FAR * __RPC_FAR * PrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcServerRegisterAuthInfoA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerRegisterAuthInfoA (
    IN unsigned char __RPC_FAR * ServerPrincName,
    IN unsigned long AuthnSvc,
    IN RPC_AUTH_KEY_RETRIEVAL_FN GetKeyFn OPTIONAL,
    IN void __RPC_FAR * Arg OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcServerRegisterAuthInfoW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerRegisterAuthInfoW (
    IN unsigned short __RPC_FAR * ServerPrincName,
    IN unsigned long AuthnSvc,
    IN RPC_AUTH_KEY_RETRIEVAL_FN GetKeyFn OPTIONAL,
    IN void __RPC_FAR * Arg OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcServerTestCancel (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


