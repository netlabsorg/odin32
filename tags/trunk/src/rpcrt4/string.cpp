/* $Id: string.cpp,v 1.3 2000-08-02 20:18:23 bird Exp $ */
/*
 * RPCRT4 library
 * RpcString manipulation
 *
 * 2000/02/05
 *
 * Copyright 2000 David J. Raison
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include "rpcrt4.h"

// ----------------------------------------------------------------------
// RpcStringFreeA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcStringFreeA (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR * String OPTIONAL
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));

    HeapFree(GetProcessHeap(), 0, *String);

    return RPC_S_OK;
}


// ----------------------------------------------------------------------
// RpcStringFreeW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcStringFreeW (
    IN OUT unsigned short __RPC_FAR * __RPC_FAR * String
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));

    HeapFree(GetProcessHeap(), 0, *String);

    return RPC_S_OK;
}

// ----------------------------------------------------------------------
// RpcStringBindingComposeA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcStringBindingComposeW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcStringBindingParseA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcStringBindingParseW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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

