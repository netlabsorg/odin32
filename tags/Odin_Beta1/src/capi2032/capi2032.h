/* $Id: capi2032.h,v 1.4 2000-10-20 22:28:24 sandervl Exp $ */

/*
 * CAPI2032 implementation
 *
 * first implementation 1998 Felix Maschek
 *
 * rewritten 2000 Carsten Tenbrink
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __CAPI2032_H__
#define __CAPI2032_H__

ULONG APIENTRY CAPI_REGISTER(
    ULONG MessageBufferSize,
    ULONG maxLogicalConnection,
    ULONG maxBDataBlocks,
    ULONG maxBDataLen,
    ULONG * pApplID );

ULONG APIENTRY CAPI_RELEASE(
    ULONG ApplID );

ULONG APIENTRY CAPI_PUT_MESSAGE(
    ULONG ApplID,
    PVOID pCAPIMessage );

ULONG APIENTRY CAPI_GET_MESSAGE(
    ULONG ApplID,
    PVOID * ppCAPIMessage );

ULONG APIENTRY CAPI_SET_SIGNAL(
    ULONG ApplID,
    ULONG hEventSem );

VOID APIENTRY CAPI_GET_MANUFACTURER(
    char * SzBuffer );

ULONG APIENTRY CAPI_GET_VERSION(
    ULONG * pCAPIMajor,
    ULONG * pCAPIMinor,
    ULONG * pManufacturerMajor,
    ULONG * pManufacturerMinor );

ULONG APIENTRY CAPI_GET_SERIAL_NUMBER(
    char * SzBuffer );

ULONG APIENTRY CAPI_GET_PROFILE(
    PVOID SzBuffer,
    ULONG CtrlNr );

ULONG APIENTRY CAPI_INSTALLED(void);

#endif
