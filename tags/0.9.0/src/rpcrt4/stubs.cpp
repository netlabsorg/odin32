/* $Id: stubs.cpp,v 1.3 2000-10-16 17:59:22 sandervl Exp $ */

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

// ----------------------------------------------------------------------
// RpcIfInqId
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcIfInqId (
    IN RPC_IF_HANDLE RpcIfHandle,
    OUT RPC_IF_ID __RPC_FAR * RpcIfId
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcNetworkIsProtseqValidA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcNetworkIsProtseqValidA (
    IN unsigned char __RPC_FAR * Protseq
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcNetworkIsProtseqValidW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcNetworkIsProtseqValidW (
    IN unsigned short __RPC_FAR * Protseq
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcNetworkInqProtseqsA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcNetworkInqProtseqsA (
    OUT RPC_PROTSEQ_VECTORA __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcNetworkInqProtseqsW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcNetworkInqProtseqsW (
    OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcObjectInqType
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcObjectInqType (
    IN UUID __RPC_FAR * ObjUuid,
    OUT UUID __RPC_FAR * TypeUuid OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcObjectSetInqFn
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcObjectSetInqFn (
    IN RPC_OBJECT_INQ_FN __RPC_FAR * InquiryFn
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */
// ----------------------------------------------------------------------
// RpcObjectSetType
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcObjectSetType (
    IN UUID __RPC_FAR * ObjUuid,
    IN UUID __RPC_FAR * TypeUuid OPTIONAL
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* server */

// ----------------------------------------------------------------------
// RpcProtseqVectorFreeA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcProtseqVectorFreeA (
    IN OUT RPC_PROTSEQ_VECTORA __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcProtseqVectorFreeW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcProtseqVectorFreeW (
    IN OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



/* server */
// ----------------------------------------------------------------------
// RpcSsDontSerializeContext
// ----------------------------------------------------------------------
RPCRTAPI void RPC_ENTRY
RpcSsDontSerializeContext (
    void
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
}



// ----------------------------------------------------------------------
// RpcIfIdVectorFree
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcIfIdVectorFree (
    IN OUT RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR * IfIdVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



/* client */
// ----------------------------------------------------------------------
// RpcEpResolveBinding
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcEpResolveBinding (
    IN RPC_BINDING_HANDLE Binding,
    IN RPC_IF_HANDLE IfSpec
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


/* client */

// ----------------------------------------------------------------------
// RpcNsBindingInqEntryNameA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcNsBindingInqEntryNameA (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    OUT unsigned char __RPC_FAR * __RPC_FAR * EntryName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcNsBindingInqEntryNameW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcNsBindingInqEntryNameW (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned long EntryNameSyntax,
    OUT unsigned short __RPC_FAR * __RPC_FAR * EntryName
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}





// ----------------------------------------------------------------------
// RpcRaiseException
// ----------------------------------------------------------------------
RPCRTAPI void RPC_ENTRY
RpcRaiseException (
    IN RPC_STATUS exception
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
}


// ----------------------------------------------------------------------
// RpcTestCancel
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcTestCancel(
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcCancelThread
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcCancelThread(
    IN void * Thread
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}



// ----------------------------------------------------------------------
// RpcEpRegisterNoReplaceA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcEpRegisterNoReplaceW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcEpRegisterA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcEpRegisterW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcEpUnregister
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcEpUnregister(
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR * BindingVector,
    IN UUID_VECTOR * UuidVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// DceErrorInqTextA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
DceErrorInqTextA (
    IN RPC_STATUS RpcStatus,
    OUT unsigned char __RPC_FAR * ErrorText
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// DceErrorInqTextW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
DceErrorInqTextW (
    IN RPC_STATUS RpcStatus,
    OUT unsigned short __RPC_FAR * ErrorText
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
HRESULT RPC_ENTRY NdrDllRegisterProxy(
    HMODULE                  hDll,
//    const ProxyFileInfo **   pProxyFileList,
    void ** pProxyFileList,
    const CLSID *            pclsid)
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
HRESULT RPC_ENTRY NdrDllUnregisterProxy(
    HMODULE                  hDll,
//    const ProxyFileInfo **   pProxyFileList,
    void ** pProxyFileList,
    const CLSID *            pclsid)
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
