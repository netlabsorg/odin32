/* $Id: obj_surrogate.h,v 1.2 1999-08-22 22:52:11 sandervl Exp $ */
/*
 * Defines surrogate COM Interface
 */

#ifndef __WINE_WINE_OBJ_SURROGATE_H
#define __WINE_WINE_OBJ_SURROGATE_H

#include "wine/obj_base.h"


/*****************************************************************************
* Predeclare the interfaces
*/
DEFINE_OLEGUID(IID_ISurrogate,        0x00000046L, 0, 0);
typedef struct ISurrogate ISurrogate,*LPSURROGATE;

/*****************************************************************************
 * ISurrogate interface
 */
#define ICOM_INTERFACE ISurrogate
#define ISurrogate_METHODS \
    ICOM_METHOD1(HRESULT,LoadDllServer,  REFCLSID,rclsid) \
    ICOM_METHOD (HRESULT,FreeSurrogate)
#define ISurrogate_IMETHODS \
    IUnknown_IMETHODS \
    ISurrogate_METHODS
ICOM_DEFINE(ISurrogate,IUnknown)
#undef ICOM_INTERFACE

#ifdef ICOM_CINTERFACE
/*** IUnknown methods ***/
#define ISurrogate_QueryInterface(p,a,b) ICOM_CALL2(QueryInterface,p,a,b)
#define ISurrogate_AddRef(p)             ICOM_CALL (AddRef,p)
#define ISurrogate_Release(p)            ICOM_CALL (Release,p)

/*** ISurrogate methods ***/
#define ISurrogate_LoadDllServer(p,a) ICOM_CALL1(LoadDllServer,p,a)
#define ISurrogate_FreeSurrogate(p)      ICOM_CALL (Reset,p)

#endif

HRESULT WIN32API CoRegisterSurrogate(LPSURROGATE pSurrogate);


#endif // __WINE_WINE_OBJ_SURROGATE_H


