/* $Id: obj_extracticon.h,v 1.3 1999-06-10 16:21:54 achimha Exp $ */
/************************************************************
 *    IExtractIconA
 */

#ifndef __WINE_WINE_OBJ_IEXTRACTICONA_H
#define __WINE_WINE_OBJ_IEXTRACTICONA_H

#include "winbase.h"
#include "winuser.h"
#include "wine/obj_base.h"

DEFINE_SHLGUID(IID_IExtractIconA,	0x000214EBL, 0, 0);
DEFINE_SHLGUID(IID_IExtractIconW,	0x000214FAL, 0, 0);
typedef struct IExtractIconA IExtractIconA,*LPEXTRACTICONA;

/* GetIconLocation() input flags*/
#define GIL_OPENICON     0x0001      /* allows containers to specify an "open" look */
#define GIL_FORSHELL     0x0002      /* icon is to be displayed in a ShellFolder */
#define GIL_ASYNC        0x0020      /* this is an async extract, return E_ASYNC */

/* GetIconLocation() return flags */
#define GIL_SIMULATEDOC  0x0001      /* simulate this document icon for this */
#define GIL_PERINSTANCE  0x0002      /* icons from this class are per instance (each file has its own) */
#define GIL_PERCLASS     0x0004      /* icons from this class per class (shared for all files of this type) */
#define GIL_NOTFILENAME  0x0008      /* location is not a filename, must call ::ExtractIcon */
#define GIL_DONTCACHE    0x0010      /* this icon should not be cached */


#define ICOM_INTERFACE IExtractIconA
#define IExtractIconA_METHODS \
	ICOM_METHOD5(HRESULT, GetIconLocation, UINT, uFlags, LPSTR, szIconFile, UINT, cchMax, INT*, piIndex, UINT *, pwFlags) \
	ICOM_METHOD5(HRESULT, Extract, LPCSTR, pszFile, UINT, nIconIndex, HICON*, phiconLarge, HICON*, phiconSmall, UINT, nIconSize)
#define IExtractIconA_IMETHODS \
	IUnknown_IMETHODS \
	IExtractIconA_METHODS
ICOM_DEFINE(IExtractIconA,IUnknown)
#undef ICOM_INTERFACE

#ifdef ICOM_CINTERFACE
#define IExtractIconA_QueryInterface(p,a,b)	ICOM_CALL2(QueryInterface,p,a,b)
#define IExtractIconA_AddRef(p)			ICOM_CALL(AddRef,p)
#define IExtractIconA_Release(p)		ICOM_CALL(Release,p)
#define IExtractIconA_GetIconLocation(p,a,b,c,d,e)	ICOM_CALL5(GetIconLocation,p,a,b,c,d,e)
#define IExtractIconA_Extract(p,a,b,c,d,e)	ICOM_CALL5(Extract,p,a,b,c,d,e)
#endif

#define IExtractIcon        IExtractIconA 

#endif /* __WINE_WINE_OBJ_IEXTRACTICONA_H */
