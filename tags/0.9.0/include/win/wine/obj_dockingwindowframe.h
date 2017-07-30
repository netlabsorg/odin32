/* $Id: obj_dockingwindowframe.h,v 1.5 1999-08-22 22:52:07 sandervl Exp $ */
/************************************************************
 *    IDockingWindowFrame
 */

#ifndef __WINE_WINE_OBJ_DOCKINGWINDOWFRAME_H
#define __WINE_WINE_OBJ_DOCKINGWINDOWFRAME_H

#include "winbase.h"
#include "winuser.h"
#include "wine/obj_base.h"
#include "wine/obj_inplace.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

typedef struct IDockingWindowFrame IDockingWindowFrame,	*LPDOCKINGWINDOWFRAME;
DEFINE_GUID (IID_IDockingWindowFrame,	0x47D2657AL, 0x7B27, 0x11D0, 0x8C, 0xA9, 0x00, 0xA0, 0xC9, 0x2D, 0xBF, 0xE8);

#define DWFRF_NORMAL		0x0000  /* femove toolbar flags*/
#define DWFRF_DELETECONFIGDATA	0x0001
#define DWFAF_HIDDEN		0x0001   /* add tolbar*/

#define ICOM_INTERFACE IDockingWindowFrame
#define IDockingWindowFrame_METHODS \
	ICOM_METHOD3(HRESULT, AddToolbar, IUnknown*, punkSrc, LPCWSTR, pwszItem, DWORD, dwAddFlags) \
	ICOM_METHOD2(HRESULT, RemoveToolbar, IUnknown*, punkSrc, DWORD, dwRemoveFlags) \
	ICOM_METHOD3(HRESULT, FindToolbar, LPCWSTR, pwszItem, REFIID, riid, LPVOID*, ppvObj)
#define IDockingWindowFrame_IMETHODS \
	IOleWindow_IMETHODS \
	IDockingWindowFrame_METHODS
ICOM_DEFINE(IDockingWindowFrame,IOleWindow)
#undef ICOM_INTERFACE

#ifdef ICOM_CINTERFACE
#define IDockingWindowFrame_QueryInterface(p,a,b)	ICOM_CALL2(QueryInterface,p,a,b)
#define IDockingWindowFrame_AddRef(p)	ICOM_CALL(AddRef,p)
#define IDockingWindowFrame_Release(p)	ICOM_CALL(Release,p)
#define IDockingWindowFrame_GetWindow(p,a)	ICOM_CALL1(GetWindow,p,a)
#define IDockingWindowFrame_ContextSensitiveHelp(p,a)	ICOM_CALL1(ContextSensitiveHelp,p,a)
#define IDockingWindowFrame_AddToolbar(p,a,b,c)	ICOM_CALL3(AddToolbar,p,a,b,c)
#define IDockingWindowFrame_RemoveToolbar(p,a,b)	ICOM_CALL2(RemoveToolbar,p,a,b)
#define IDockingWindowFrame_FindToolbar(p,a,b,c)	ICOM_CALL3(FindToolbar,p,a,b,c)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* __WINE_WINE_OBJ_DOCKINGWINDOWFRAME_H */
