/*
 * Defines the COM interfaces and APIs related to IShellLink.
 *
 * Depends on 'obj_base.h'.
 */

#ifndef __WINE_WINE_OBJ_SHELLLINK_H
#define __WINE_WINE_OBJ_SHELLLINK_H

#include "wine/obj_base.h"
#include "winbase.h"
#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

/*****************************************************************************
 * Predeclare the interfaces
 */
DEFINE_SHLGUID(IID_IShellLink,          0x000214EEL, 0, 0);
typedef struct IShellLink IShellLink,*LPSHELLLINK;
#define IShellLinkA IShellLink

DEFINE_SHLGUID(IID_IShellLinkW,         0x000214F9L, 0, 0);
typedef struct IShellLinkW IShellLinkW,*LPSHELLLINKW;

/*****************************************************************************
 * 
 */
typedef enum 
{	SLR_NO_UI	= 0x0001,
	SLR_ANY_MATCH	= 0x0002,
	SLR_UPDATE	= 0x0004
} SLR_FLAGS;

/*****************************************************************************
 * GetPath fFlags 
 */
typedef enum 
{	SLGP_SHORTPATH		= 0x0001,
	SLGP_UNCPRIORITY	= 0x0002
} SLGP_FLAGS;
/*****************************************************************************
 * IShellLink interface
 */
#define ICOM_INTERFACE IShellLink
#define IShellLink_METHODS \
    ICOM_METHOD4( HRESULT, GetPath, LPSTR, pszFile, INT, cchMaxPath, WIN32_FIND_DATAA *, pfd, DWORD, fFlags) \
    ICOM_METHOD1( HRESULT, GetIDList, LPITEMIDLIST *, ppidl) \
    ICOM_METHOD1( HRESULT, SetIDList, LPCITEMIDLIST, pidl) \
    ICOM_METHOD2( HRESULT, GetDescription, LPSTR, pszName, INT, cchMaxName) \
    ICOM_METHOD1( HRESULT, SetDescription, LPCSTR, pszName) \
    ICOM_METHOD2( HRESULT, GetWorkingDirectory, LPSTR, pszDir,INT, cchMaxPath) \
    ICOM_METHOD1( HRESULT, SetWorkingDirectory, LPCSTR, pszDir) \
    ICOM_METHOD2( HRESULT, GetArguments, LPSTR, pszArgs, INT, cchMaxPath) \
    ICOM_METHOD1( HRESULT, SetArguments, LPCSTR, pszArgs) \
    ICOM_METHOD1( HRESULT, GetHotkey, WORD*, pwHotkey) \
    ICOM_METHOD1( HRESULT, SetHotkey, WORD, wHotkey) \
    ICOM_METHOD1( HRESULT, GetShowCmd, INT*, piShowCmd) \
    ICOM_METHOD1( HRESULT, SetShowCmd, INT, iShowCmd) \
    ICOM_METHOD3( HRESULT, GetIconLocation, LPSTR, pszIconPath, INT, cchIconPath,INT *, piIcon) \
    ICOM_METHOD2( HRESULT, SetIconLocation, LPCSTR, pszIconPath,INT, iIcon) \
    ICOM_METHOD2( HRESULT, SetRelativePath, LPCSTR, pszPathRel, DWORD, dwReserved) \
    ICOM_METHOD2( HRESULT, Resolve, HWND, hwnd, DWORD, fFlags) \
    ICOM_METHOD1( HRESULT, SetPath, LPCSTR, pszFile)
#define IShellLink_IMETHODS \
    IUnknown_IMETHODS \
    IShellLink_METHODS
ICOM_DEFINE(IShellLink,IUnknown)
#undef ICOM_INTERFACE

#ifdef ICOM_CINTERFACE
/*** IUnknown methods ***/
#define IShellLink_QueryInterface(p,a,b)	ICOM_CALL2(QueryInterface,p,a,b)
#define IShellLink_AddRef(p)			ICOM_CALL (AddRef,p)
#define IShellLink_Release(p)			ICOM_CALL (Release,p)
/*** IShellLink methods ***/
#define IShellLink_GetPath(p,a,b,c,d)		ICOM_CALL4(GetPath,p,a,b,c,d)
#define IShellLink_GetIDList(p,a)		ICOM_CALL1(GetIDList,p,a)
#define IShellLink_SetIDList(p,a)		ICOM_CALL1(SetIDList,p,a)
#define IShellLink_GetDescription(p,a,b)	ICOM_CALL2(GetDescription,p,a,b)
#define IShellLink_SetDescription(p,a)		ICOM_CALL1(SetDescription,p,a)
#define IShellLink_GetWorkingDirectory(p,a,b)	ICOM_CALL2(GetWorkingDirectory,p,a,b)
#define IShellLink_SetWorkingDirectory(p,a)	ICOM_CALL1(SetWorkingDirectory,p,a)
#define IShellLink_GetArguments(p,a,b)		ICOM_CALL2(GetArguments,p,a,b)
#define IShellLink_SetArguments(p,a)		ICOM_CALL1(SetArguments,p,a)
#define IShellLink_GetHotkey(p,a)		ICOM_CALL1(GetHotkey,p,a)
#define IShellLink_SetHotkey(p,a)		ICOM_CALL1(SetHotkey,p,a)
#define IShellLink_GetShowCmd(p,a)		ICOM_CALL1(GetShowCmd,p,a)
#define IShellLink_SetShowCmd(p,a)		ICOM_CALL1(SetShowCmd,p,a)
#define IShellLink_GetIconLocation(p,a,b,c)	ICOM_CALL3(GetIconLocation,p,a,b,c)
#define IShellLink_SetIconLocation(p,a,b)	ICOM_CALL2(SetIconLocation,p,a,b)
#define IShellLink_SetRelativePath(p,a,b)	ICOM_CALL2(SetRelativePath,p,a,b)
#define IShellLink_Resolve(p,a,b)		ICOM_CALL2(Resolve,p,a,b)
#define IShellLink_SetPath(p,a)			ICOM_CALL1(SetPath,p,a)
#endif

/*****************************************************************************
 * IShellLinkW interface
 */
#define ICOM_INTERFACE IShellLinkW
#define IShellLinkW_METHODS \
    ICOM_METHOD4( HRESULT, GetPath, LPWSTR, pszFile, INT, cchMaxPath, WIN32_FIND_DATAA *, pfd, DWORD, fFlags) \
    ICOM_METHOD1( HRESULT, GetIDList, LPITEMIDLIST *, ppidl) \
    ICOM_METHOD1( HRESULT, SetIDList, LPCITEMIDLIST, pidl) \
    ICOM_METHOD2( HRESULT, GetDescription, LPWSTR, pszName, INT, cchMaxName) \
    ICOM_METHOD1( HRESULT, SetDescription, LPCWSTR, pszName) \
    ICOM_METHOD2( HRESULT, GetWorkingDirectory, LPWSTR, pszDir,INT, cchMaxPath) \
    ICOM_METHOD1( HRESULT, SetWorkingDirectory, LPCWSTR, pszDir) \
    ICOM_METHOD2( HRESULT, GetArguments, LPWSTR, pszArgs, INT, cchMaxPath) \
    ICOM_METHOD1( HRESULT, SetArguments, LPCWSTR, pszArgs) \
    ICOM_METHOD1( HRESULT, GetHotkey, WORD*, pwHotkey) \
    ICOM_METHOD1( HRESULT, SetHotkey, WORD, wHotkey) \
    ICOM_METHOD1( HRESULT, GetShowCmd, INT*, piShowCmd) \
    ICOM_METHOD1( HRESULT, SetShowCmd, INT, iShowCmd) \
    ICOM_METHOD3( HRESULT, GetIconLocation, LPWSTR, pszIconPath, INT, cchIconPath,INT *, piIcon) \
    ICOM_METHOD2( HRESULT, SetIconLocation, LPCWSTR, pszIconPath,INT, iIcon) \
    ICOM_METHOD2( HRESULT, SetRelativePath, LPCWSTR, pszPathRel, DWORD, dwReserved) \
    ICOM_METHOD2( HRESULT, Resolve, HWND, hwnd, DWORD, fFlags) \
    ICOM_METHOD1( HRESULT, SetPath, LPCWSTR, pszFile)
#define IShellLinkW_IMETHODS \
    IUnknown_IMETHODS \
    IShellLinkW_METHODS
ICOM_DEFINE(IShellLinkW,IUnknown)
#undef ICOM_INTERFACE

#ifdef ICOM_CINTERFACE
/*** IUnknown methods ***/
#define IShellLinkW_QueryInterface(p,a,b)	ICOM_CALL2(QueryInterface,p,a,b)
#define IShellLinkW_AddRef(p)			ICOM_CALL (AddRef,p)
#define IShellLinkW_Release(p)			ICOM_CALL (Release,p)
/*** IShellLinkW methods ***/
#define IShellLinkW_GetPath(p,a,b,c,d)		ICOM_CALL4(GetPath,p,a,b,c,d)
#define IShellLinkW_GetIDList(p,a)		ICOM_CALL1(GetIDList,p,a)
#define IShellLinkW_SetIDList(p,a)		ICOM_CALL1(SetIDList,p,a)
#define IShellLinkW_GetDescription(p,a,b)	ICOM_CALL2(GetDescription,p,a,b)
#define IShellLinkW_SetDescription(p,a)		ICOM_CALL1(SetDescription,p,a)
#define IShellLinkW_GetWorkingDirectory(p,a,b)	ICOM_CALL2(GetWorkingDirectory,p,a,b)
#define IShellLinkW_SetWorkingDirectory(p,a)	ICOM_CALL1(SetWorkingDirectory,p,a)
#define IShellLinkW_GetArguments(p,a,b)		ICOM_CALL2(GetArguments,p,a,b)
#define IShellLinkW_SetArguments(p,a)		ICOM_CALL1(SetArguments,p,a)
#define IShellLinkW_GetHotkey(p,a)		ICOM_CALL1(GetHotkey,p,a)
#define IShellLinkW_SetHotkey(p,a)		ICOM_CALL1(SetHotkey,p,a)
#define IShellLinkW_GetShowCmd(p,a)		ICOM_CALL1(GetShowCmd,p,a)
#define IShellLinkW_SetShowCmd(p,a)		ICOM_CALL1(SetShowCmd,p,a)
#define IShellLinkW_GetIconLocation(p,a,b,c)	ICOM_CALL3(GetIconLocation,p,a,b,c)
#define IShellLinkW_SetIconLocation(p,a,b)	ICOM_CALL2(SetIconLocation,p,a,b)
#define IShellLinkW_SetRelativePath(p,a,b)	ICOM_CALL2(SetRelativePath,p,a,b)
#define IShellLinkW_Resolve(p,a,b)		ICOM_CALL2(Resolve,p,a,b)
#define IShellLinkW_SetPath(p,a)			ICOM_CALL1(SetPath,p,a)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* __WINE_WINE_OBJ_SHELLLINK_H */
