/* $Id: obj_shellextinit.h,v 1.6 2000-08-18 02:04:20 phaller Exp $ */
/************************************************************
 *    IShellExtInit
 */

#ifndef __WINE_WINE_OBJ_SHELLEXTINIT_H
#define __WINE_WINE_OBJ_SHELLEXTINIT_H

#include "winbase.h"
#include "winuser.h"
#include "wine/obj_base.h"
#include "wine/obj_dataobject.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

typedef struct 	IShellExtInit IShellExtInit, *LPSHELLEXTINIT;

#define ICOM_INTERFACE IShellExtInit
#define IShellExtInit_METHODS \
	ICOM_METHOD3(HRESULT, Initialize, LPCITEMIDLIST, pidlFolder, LPDATAOBJECT, lpdobj, HKEY, hkeyProgID)
#define IShellExtInit_IMETHODS \
	IUnknown_IMETHODS \
	IShellExtInit_METHODS
ICOM_DEFINE(IShellExtInit,IUnknown)
#undef ICOM_INTERFACE

#ifdef ICOM_CINTERFACE
/*** IUnknown methods ***/
#define IShellExtInit_QueryInterface(p,a,b)	ICOM_CALL2(QueryInterface,p,a,b)
#define IShellExtInit_AddRef(p)			ICOM_CALL(AddRef,p)
#define IShellExtInit_Release(p)		ICOM_CALL(Release,p)
/*** IShellExtInit methods ***/
#define IShellExtInit_Initialize(p,a,b,c)	ICOM_CALL3(Initialize,p,a,b,c)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* __WINE_WINE_OBJ_SHELLEXTINIT_H */
