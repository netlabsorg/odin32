/* $Id: binding.cpp,v 1.1 2000-04-04 19:49:02 davidr Exp $ */

/*
 * RPCRT4 library
 * Binding Stubs
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
// RpcBindingCopy
// ----------------------------------------------------------------------
/* client */
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingCopy (
    IN RPC_BINDING_HANDLE SourceBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * DestinationBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingFree
// ----------------------------------------------------------------------
/* client */
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingFree (
    IN OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingSetOption
// ----------------------------------------------------------------------
/* client */
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingSetOption( IN RPC_BINDING_HANDLE hBinding,
                     IN unsigned long      option,
                     IN unsigned long      optionValue )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingInqOption
// ----------------------------------------------------------------------
/* client */
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingInqOption( IN  RPC_BINDING_HANDLE hBinding,
                     IN  unsigned long      option,
                     OUT unsigned long     *pOptionValue )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingFromStringBindingA
// ----------------------------------------------------------------------
/* client */

RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingFromStringBindingA (
    IN unsigned char __RPC_FAR * StringBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcBindingFromStringBindingW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingFromStringBindingW (
    IN unsigned short __RPC_FAR * StringBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcBindingInqObject
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingInqObject (
    IN RPC_BINDING_HANDLE Binding,
    OUT UUID __RPC_FAR * ObjectUuid
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcBindingReset
// ----------------------------------------------------------------------
/* client */
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingReset (
    IN RPC_BINDING_HANDLE Binding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcBindingSetObject
// ----------------------------------------------------------------------
/* client */
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingSetObject (
    IN RPC_BINDING_HANDLE Binding,
    IN UUID __RPC_FAR * ObjectUuid
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingToStringBindingA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingToStringBindingA (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned char __RPC_FAR * __RPC_FAR * StringBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}


// ----------------------------------------------------------------------
// RpcBindingToStringBindingW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingToStringBindingW (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned short __RPC_FAR * __RPC_FAR * StringBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingVectorFree
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingVectorFree (
    IN OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVector
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

// ----------------------------------------------------------------------
// RpcBindingInqAuthClientA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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

// ----------------------------------------------------------------------
// RpcBindingInqAuthClientW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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

// ----------------------------------------------------------------------
// RpcBindingInqAuthInfoA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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

// ----------------------------------------------------------------------
// RpcBindingInqAuthInfoW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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

// ----------------------------------------------------------------------
// RpcBindingSetAuthInfoA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcBindingSetAuthInfoExA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcBindingSetAuthInfoW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcBindingSetAuthInfoExW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcBindingInqAuthInfoExA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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


// ----------------------------------------------------------------------
// RpcBindingInqAuthInfoExW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
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

// ----------------------------------------------------------------------
// RpcBindingServerFromClient
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
RpcBindingServerFromClient (
    IN RPC_BINDING_HANDLE ClientBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * ServerBinding
    )
{
    dprintf(("RPCRT4: %s - Stub", __FUNCTION__));
    return RPC_S_OUT_OF_MEMORY;
}

