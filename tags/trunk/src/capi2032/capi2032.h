/* $Id: capi2032.h,v 1.3 1999-06-10 14:27:04 phaller Exp $ */

/*
 * CAPI2032 implementation
 *
 * Copyright 1998 Felix Maschek
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __CAPI2032_H__
#define __CAPI2032_H__

ULONG PASCAL CAPI_REGISTER(
    ULONG MessageBufferSize,
    ULONG maxLogicalConnection,
    ULONG maxBDataBlocks,
    ULONG maxBDataLen,
    ULONG * pApplID );

ULONG PASCAL CAPI_RELEASE(
    ULONG ApplID );

ULONG PASCAL CAPI_PUT_MESSAGE(
    ULONG ApplID,
    PVOID pCAPIMessage );

ULONG PASCAL CAPI_GET_MESSAGE(
    ULONG ApplID,
    PVOID * ppCAPIMessage );

ULONG PASCAL CAPI_SET_SIGNAL(
    ULONG ApplID,
    ULONG hEventSem );

ULONG PASCAL CAPI_GET_MANUFACTURER(
    char * SzBuffer );

ULONG PASCAL CAPI_GET_VERSION(
    ULONG * pCAPIMajor,
    ULONG * pCAPIMinor,
    ULONG * pManufacturerMajor,
    ULONG * pManufacturerMinor );

ULONG PASCAL CAPI_GET_SERIAL_NUMBER(
    char * SzBuffer );

ULONG PASCAL CAPI_GET_PROFILE(
    PVOID SzBuffer,
    ULONG CtrlNr );

ULONG PASCAL CAPI_INSTALLED();

#endif
