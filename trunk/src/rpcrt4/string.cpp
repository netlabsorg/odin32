/* $Id: string.cpp,v 1.1 2000-04-02 22:02:57 davidr Exp $ */
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
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringFreeA (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR * String
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));
    
    HeapFree(GetProcessHeap(), 0, *String);
    
    return RPC_S_OK;
}


// ----------------------------------------------------------------------
// RpcStringFreeW
// ----------------------------------------------------------------------
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringFreeW (
    IN OUT unsigned short __RPC_FAR * __RPC_FAR * String
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));
    
    HeapFree(GetProcessHeap(), 0, *String);
    
    return RPC_S_OK;
}

