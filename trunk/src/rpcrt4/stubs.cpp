/* $Id: stubs.cpp,v 1.1 2000-04-02 22:02:58 davidr Exp $ */

/*
 * RPCRT4 library
 * Stubs
 *
 * 2000/02/05
 * 
 * Copyright 2000 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

//#include <os2win.h>
//#include <odinwrap.h>

#include "rpcrt4.h"

//ODINDEBUGCHANNEL(rpcrt4)


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingCopy (
    IN RPC_BINDING_HANDLE SourceBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * DestinationBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFree (
    IN OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetOption( IN RPC_BINDING_HANDLE hBinding,
                     IN unsigned long      option,
                     IN unsigned long      optionValue )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqOption( IN  RPC_BINDING_HANDLE hBinding,
                     IN  unsigned long      option,
                     OUT unsigned long     *pOptionValue )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFromStringBindingA (
    IN unsigned char __RPC_FAR * StringBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFromStringBindingW (
    IN unsigned short __RPC_FAR * StringBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqObject (
    IN RPC_BINDING_HANDLE Binding,
    OUT UUID __RPC_FAR * ObjectUuid
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingReset (
    IN RPC_BINDING_HANDLE Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetObject (
    IN RPC_BINDING_HANDLE Binding,
    IN UUID __RPC_FAR * ObjectUuid
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqDefaultProtectLevel(
    IN  unsigned long AuthnSvc,
    OUT unsigned long __RPC_FAR *AuthnLevel
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingToStringBindingA (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * StringBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingToStringBindingW (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned short __RPC_FAR * __RPC_FAR * StringBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingVectorFree (
    IN OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingComposeA (
    IN unsigned char __RPC_FAR * ObjUuid OPTIONAL,
    IN unsigned char __RPC_FAR * Protseq OPTIONAL,
    IN unsigned char __RPC_FAR * NetworkAddr OPTIONAL,
    IN unsigned char __RPC_FAR * Endpoint OPTIONAL,
    IN unsigned char __RPC_FAR * Options OPTIONAL,
    OUT unsigned char __RPC_FAR * __RPC_FAR * StringBinding OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingComposeW (
    IN unsigned short __RPC_FAR * ObjUuid OPTIONAL,
    IN unsigned short __RPC_FAR * Protseq OPTIONAL,
    IN unsigned short __RPC_FAR * NetworkAddr OPTIONAL,
    IN unsigned short __RPC_FAR * Endpoint OPTIONAL,
    IN unsigned short __RPC_FAR * Options OPTIONAL,
    OUT unsigned short __RPC_FAR * __RPC_FAR * StringBinding OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingParseA (
    IN unsigned char __RPC_FAR * StringBinding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * ObjUuid OPTIONAL,
    OUT unsigned char __RPC_FAR * __RPC_FAR * Protseq OPTIONAL,
    OUT unsigned char __RPC_FAR * __RPC_FAR * NetworkAddr OPTIONAL,
    OUT unsigned char __RPC_FAR * __RPC_FAR * Endpoint OPTIONAL,
    OUT unsigned char __RPC_FAR * __RPC_FAR * NetworkOptions OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingParseW (
    IN unsigned short __RPC_FAR * StringBinding,
    OUT unsigned short __RPC_FAR * __RPC_FAR * ObjUuid OPTIONAL,
    OUT unsigned short __RPC_FAR * __RPC_FAR * Protseq OPTIONAL,
    OUT unsigned short __RPC_FAR * __RPC_FAR * NetworkAddr OPTIONAL,
    OUT unsigned short __RPC_FAR * __RPC_FAR * Endpoint OPTIONAL,
    OUT unsigned short __RPC_FAR * __RPC_FAR * NetworkOptions OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcIfInqId (
    IN RPC_IF_HANDLE RpcIfHandle,
    OUT RPC_IF_ID __RPC_FAR * RpcIfId
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkIsProtseqValidA (
    IN unsigned char __RPC_FAR * Protseq
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkIsProtseqValidW (
    IN unsigned short __RPC_FAR * Protseq
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqComTimeout (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned int __RPC_FAR * Timeout
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetComTimeout (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned int Timeout
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetCancelTimeout(
    long Timeout
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



/* server */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkInqProtseqsA (
    OUT RPC_PROTSEQ_VECTORA __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkInqProtseqsW (
    OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcObjectInqType (
    IN UUID __RPC_FAR * ObjUuid,
    OUT UUID __RPC_FAR * TypeUuid OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcObjectSetInqFn (
    IN RPC_OBJECT_INQ_FN __RPC_FAR * InquiryFn
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcObjectSetType (
    IN UUID __RPC_FAR * ObjUuid,
    IN UUID __RPC_FAR * TypeUuid OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcProtseqVectorFreeA (
    IN OUT RPC_PROTSEQ_VECTORA __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcProtseqVectorFreeW (
    IN OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqBindings (
    OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseAllProtseqs (
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
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

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqA (
    IN unsigned char __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqW (
    IN unsigned short __RPC_FAR * Protseq,
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtStatsVectorFree (
    IN RPC_STATS_VECTOR ** StatsVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqStats (
    IN RPC_BINDING_HANDLE Binding,
    OUT RPC_STATS_VECTOR ** Statistics
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtIsServerListening (
    IN RPC_BINDING_HANDLE Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtStopServerListening (
    IN RPC_BINDING_HANDLE Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtWaitServerListen (
    void
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetServerStackSize (
    IN unsigned long ThreadStackSize
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
RPCRTAPI
void
RPC_ENTRY
RpcSsDontSerializeContext (
    void
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEnableIdleCleanup (
    void
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqIfIds (
    IN RPC_BINDING_HANDLE Binding,
    OUT RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR * IfIdVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcIfIdVectorFree (
    IN OUT RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR * IfIdVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqServerPrincNameA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long AuthnSvc,
    OUT unsigned char __RPC_FAR * __RPC_FAR * ServerPrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqServerPrincNameW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long AuthnSvc,
    OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqDefaultPrincNameA (
    IN unsigned long AuthnSvc,
    OUT unsigned char __RPC_FAR * __RPC_FAR * PrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqDefaultPrincNameW (
    IN unsigned long AuthnSvc,
    OUT unsigned short __RPC_FAR * __RPC_FAR * PrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpResolveBinding (
    IN RPC_BINDING_HANDLE Binding,
    IN RPC_IF_HANDLE IfSpec
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNsBindingInqEntryNameA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    OUT unsigned char __RPC_FAR * __RPC_FAR * EntryName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNsBindingInqEntryNameW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    OUT unsigned short __RPC_FAR * __RPC_FAR * EntryName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientA (
    IN RPC_BINDING_HANDLE ClientBinding, OPTIONAL
    OUT RPC_AUTHZ_HANDLE __RPC_FAR * Privs,
    OUT unsigned char __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthzSvc OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientW (
    IN RPC_BINDING_HANDLE ClientBinding, OPTIONAL
    OUT RPC_AUTHZ_HANDLE __RPC_FAR * Privs,
    OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthzSvc OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoA (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
    OUT RPC_AUTH_IDENTITY_HANDLE __RPC_FAR * AuthIdentity, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthzSvc OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoW (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
    OUT RPC_AUTH_IDENTITY_HANDLE __RPC_FAR * AuthIdentity, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthzSvc OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned char __RPC_FAR * ServerPrincName,
    IN unsigned long AuthnLevel,
    IN unsigned long AuthnSvc,
    IN RPC_AUTH_IDENTITY_HANDLE AuthIdentity, OPTIONAL
    IN unsigned long AuthzSvc
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoExA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned char __RPC_FAR * ServerPrincName,
    IN unsigned long AuthnLevel,
    IN unsigned long AuthnSvc,
    IN RPC_AUTH_IDENTITY_HANDLE AuthIdentity, OPTIONAL
    IN unsigned long AuthzSvc,
    IN RPC_SECURITY_QOS *SecurityQos OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned short __RPC_FAR * ServerPrincName,
    IN unsigned long AuthnLevel,
    IN unsigned long AuthnSvc,
    IN RPC_AUTH_IDENTITY_HANDLE AuthIdentity, OPTIONAL
    IN unsigned long AuthzSvc
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoExW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned short __RPC_FAR * ServerPrincName,
    IN unsigned long AuthnLevel,
    IN unsigned long AuthnSvc,
    IN RPC_AUTH_IDENTITY_HANDLE AuthIdentity, OPTIONAL
    IN unsigned long AuthzSvc, OPTIONAL
    IN RPC_SECURITY_QOS *SecurityQOS
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoExA (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
    OUT RPC_AUTH_IDENTITY_HANDLE __RPC_FAR * AuthIdentity, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthzSvc, OPTIONAL
    IN  unsigned long RpcQosVersion,
    OUT RPC_SECURITY_QOS *SecurityQOS
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoExW (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnLevel, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthnSvc, OPTIONAL
    OUT RPC_AUTH_IDENTITY_HANDLE __RPC_FAR * AuthIdentity, OPTIONAL
    OUT unsigned long __RPC_FAR * AuthzSvc OPTIONAL,
    IN  unsigned long RpcQosVersion,
    OUT RPC_SECURITY_QOS *SecurityQOS
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
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


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingServerFromClient (
    IN RPC_BINDING_HANDLE ClientBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * ServerBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
void
RPC_ENTRY
RpcRaiseException (
    IN RPC_STATUS exception
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcTestCancel(
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerTestCancel (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcCancelThread(
    IN void * Thread
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterNoReplaceA (
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR * BindingVector,
    IN UUID_VECTOR * UuidVector OPTIONAL,
    IN unsigned char * Annotation
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterNoReplaceW (
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR * BindingVector,
    IN UUID_VECTOR * UuidVector OPTIONAL,
    IN unsigned short  * Annotation
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterA (
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR * BindingVector,
    IN UUID_VECTOR * UuidVector OPTIONAL,
    IN unsigned char * Annotation
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterW (
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR * BindingVector,
    IN UUID_VECTOR * UuidVector OPTIONAL,
    IN unsigned short * Annotation
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpUnregister(
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR * BindingVector,
    IN UUID_VECTOR * UuidVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
DceErrorInqTextA (
    IN RPC_STATUS RpcStatus,
    OUT unsigned char __RPC_FAR * ErrorText
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
DceErrorInqTextW (
    IN RPC_STATUS RpcStatus,
    OUT unsigned short __RPC_FAR * ErrorText
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqBegin (
    IN RPC_BINDING_HANDLE EpBinding OPTIONAL,
    IN unsigned long InquiryType,
    IN RPC_IF_ID __RPC_FAR * IfId OPTIONAL,
    IN unsigned long VersOption OPTIONAL,
    IN UUID __RPC_FAR * ObjectUuid OPTIONAL,
    OUT RPC_EP_INQ_HANDLE __RPC_FAR * InquiryContext
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqDone (
    IN OUT RPC_EP_INQ_HANDLE __RPC_FAR * InquiryContext
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqNextA (
    IN RPC_EP_INQ_HANDLE InquiryContext,
    OUT RPC_IF_ID __RPC_FAR * IfId,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding OPTIONAL,
    OUT UUID __RPC_FAR * ObjectUuid OPTIONAL,
    OUT unsigned char __RPC_FAR * __RPC_FAR * Annotation OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqNextW (
    IN RPC_EP_INQ_HANDLE InquiryContext,
    OUT RPC_IF_ID __RPC_FAR * IfId,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding OPTIONAL,
    OUT UUID __RPC_FAR * ObjectUuid OPTIONAL,
    OUT unsigned short __RPC_FAR * __RPC_FAR * Annotation OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpUnregister (
    IN RPC_BINDING_HANDLE EpBinding OPTIONAL,
    IN RPC_IF_ID __RPC_FAR * IfId,
    IN RPC_BINDING_HANDLE Binding,
    IN UUID __RPC_FAR * ObjectUuid OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetAuthorizationFn (
    IN RPC_MGMT_AUTHORIZATION_FN AuthorizationFn
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqParameter (
    IN unsigned Parameter,
    IN unsigned long __RPC_FAR * Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetParameter (
    IN unsigned Parameter,
    IN unsigned long Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtBindingInqParameter (
    IN RPC_BINDING_HANDLE Handle,
    IN unsigned Parameter,
    IN unsigned long __RPC_FAR * Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtBindingSetParameter (
    IN RPC_BINDING_HANDLE Handle,
    IN unsigned Parameter,
    IN unsigned long Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


