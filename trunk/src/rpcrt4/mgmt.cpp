/* $Id: mgmt.cpp,v 1.1 2000-04-04 19:49:03 davidr Exp $ */

/*
 * RPCRT4 library
 * Mgmt Stubs
 *
 * 2000/04/03
 * 
 * Copyright 2000 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#include "rpcrt4.h"

// ----------------------------------------------------------------------
// RpcMgmtInqDefaultProtectLevel
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqDefaultProtectLevel(
    IN  unsigned long AuthnSvc,
    OUT unsigned long __RPC_FAR *AuthnLevel
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
// ----------------------------------------------------------------------
// RpcMgmtInqComTimeout
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqComTimeout (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned int __RPC_FAR * Timeout
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
// ----------------------------------------------------------------------
// RpcMgmtSetComTimeout
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtSetComTimeout (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned int Timeout
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */
// ----------------------------------------------------------------------
// RpcMgmtSetCancelTimeout
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtSetCancelTimeout(
    long Timeout
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcMgmtStatsVectorFree
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtStatsVectorFree (
    IN RPC_STATS_VECTOR ** StatsVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcMgmtInqStats
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqStats (
    IN RPC_BINDING_HANDLE Binding,
    OUT RPC_STATS_VECTOR ** Statistics
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcMgmtIsServerListening
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtIsServerListening (
    IN RPC_BINDING_HANDLE Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcMgmtStopServerListening
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtStopServerListening (
    IN RPC_BINDING_HANDLE Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcMgmtWaitServerListen
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtWaitServerListen (
    void
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcMgmtSetServerStackSize
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtSetServerStackSize (
    IN unsigned long ThreadStackSize
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

/* client */
// ----------------------------------------------------------------------
// RpcMgmtEnableIdleCleanup
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtEnableIdleCleanup (
    void
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtInqIfIds
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqIfIds (
    IN RPC_BINDING_HANDLE Binding,
    OUT RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR * IfIdVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtInqServerPrincNameA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqServerPrincNameA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long AuthnSvc,
    OUT unsigned char __RPC_FAR * __RPC_FAR * ServerPrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtInqServerPrincNameW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqServerPrincNameW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long AuthnSvc,
    OUT unsigned short __RPC_FAR * __RPC_FAR * ServerPrincName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcMgmtEpEltInqBegin
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcMgmtEpEltInqDone
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtEpEltInqDone (
    IN OUT RPC_EP_INQ_HANDLE __RPC_FAR * InquiryContext
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtEpEltInqNextA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcMgmtEpEltInqNextW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcMgmtEpUnregister
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcMgmtSetAuthorizationFn
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtSetAuthorizationFn (
    IN RPC_MGMT_AUTHORIZATION_FN AuthorizationFn
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtInqParameter
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtInqParameter (
    IN unsigned Parameter,
    IN unsigned long __RPC_FAR * Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtSetParameter
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY RpcMgmtSetParameter (
    IN unsigned Parameter,
    IN unsigned long Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtBindingInqParameter
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtBindingInqParameter (
    IN RPC_BINDING_HANDLE Handle,
    IN unsigned Parameter,
    IN unsigned long __RPC_FAR * Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcMgmtBindingSetParameter
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcMgmtBindingSetParameter (
    IN RPC_BINDING_HANDLE Handle,
    IN unsigned Parameter,
    IN unsigned long Value
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


