/*
 * 	internal Shell32 Library definitions
 */

#ifndef __WINE_SHELL_MAIN_H
#define __WINE_SHELL_MAIN_H

#include "commctrl.h"
#include "shell.h"
#include "docobj.h"

#include "wine/obj_shellfolder.h"
#include "wine/obj_dataobject.h"
#include "wine/obj_contextmenu.h"
#include "wine/obj_shellview.h"
#include "wine/obj_shelllink.h"
#include "wine/obj_extracticon.h"
#include "wine/obj_oleview.h"
#ifdef __WIN32OS2__
#include <heapstring.h>

#undef FIXME
#undef TRACE
#ifdef DEBUG
#define TRACE WriteLog("SHELL32: %s", __FUNCTION__); WriteLog
#define FIXME WriteLog("SHELL32: FIXME %s", __FUNCTION__); WriteLog
#else
#define TRACE 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#define FIXME 1 ? (void)0 : (void)((int (*)(char *, ...)) NULL)
#endif

#endif
/*******************************************
*  global SHELL32.DLL variables
*/
extern HMODULE	huser32;
extern HINSTANCE shell32_hInstance;
extern LONG	  shell32_ObjCount;
extern HIMAGELIST	ShellSmallIconList;
extern HIMAGELIST	ShellBigIconList;
#ifndef __WIN32OS2__
extern HDPA		sic_hdpa;
#endif

/*******************************************
* pointer to functions dynamically loaded
*/
extern LPVOID	(* WINAPI pCOMCTL32_Alloc) (INT);  
extern BOOL	(* WINAPI pCOMCTL32_Free) (LPVOID);  

#define pDPA_Create	DPA_Create
#define pDPA_InsertPtr  DPA_InsertPtr
#define pDPA_Sort       DPA_Sort
#define pDPA_GetPtr     DPA_GetPtr
#define pDPA_Destroy    DPA_Destroy
#define pDPA_Search     DPA_Search
#define pDPA_DeletePtr  DPA_DeletePtr

#define pDPA_GetPtrCount(hdpa)  (*(INT*)(hdpa))   

#define pLookupIconIdFromDirectoryEx LookupIconIdFromDirectoryEx
#define pCreateIconFromResourceEx    CreateIconFromResourceEx

#include <wine\undoccomctl32.h>

#define pAddMRUData	AddMRUData
#define pFindMRUData	FindMRUData
#define pEnumMRUListA	EnumMRUListA
#define pFreeMRUListA	FreeMRUListA
#define pCreateMRUListA CreateMRUListA

/* ole2 */
/*
extern HRESULT (* WINAPI pOleInitialize)(LPVOID reserved);
extern void (* WINAPI pOleUninitialize)(void);
extern HRESULT (* WINAPI pDoDragDrop)(IDataObject* pDataObject, IDropSource * pDropSource, DWORD dwOKEffect, DWORD * pdwEffect);
extern HRESULT (* WINAPI pRegisterDragDrop)(HWND hwnd, IDropTarget* pDropTarget);
extern HRESULT (* WINAPI pRevokeDragDrop)(HWND hwnd);
*/
BOOL WINAPI Shell_GetImageList(HIMAGELIST * lpBigList, HIMAGELIST * lpSmallList);

/* Iconcache */
#define INVALID_INDEX -1
BOOL SIC_Initialize(void);
void SIC_Destroy(void);
BOOL PidlToSicIndex (IShellFolder * sh, LPITEMIDLIST pidl, BOOL bBigIcon, UINT uFlags, UINT * pIndex);

/* Classes Root */
BOOL HCR_MapTypeToValue ( LPCSTR szExtension, LPSTR szFileType, DWORD len, BOOL bPrependDot);
BOOL HCR_GetExecuteCommand ( LPCSTR szClass, LPCSTR szVerb, LPSTR szDest, DWORD len );
BOOL HCR_GetDefaultIcon (LPCSTR szClass, LPSTR szDest, DWORD len, LPDWORD dwNr);
BOOL HCR_GetClassName (REFIID riid, LPSTR szDest, DWORD len);
BOOL HCR_GetFolderAttributes (REFIID riid, LPDWORD szDest);

DWORD 	WINAPI ParseFieldA(LPCSTR src,DWORD field,LPSTR dst,DWORD len);

/****************************************************************************
 * Class constructors
 */
LPDATAOBJECT	IDataObject_Constructor(HWND hwndOwner, LPITEMIDLIST myPidl, LPITEMIDLIST * apidl, UINT cidl);
LPENUMFORMATETC	IEnumFORMATETC_Constructor(UINT, const FORMATETC []);

LPCLASSFACTORY	IClassFactory_Constructor(REFCLSID);
IContextMenu *	ISvItemCm_Constructor(LPSHELLFOLDER pSFParent, LPCITEMIDLIST pidl, LPCITEMIDLIST *aPidls, UINT uItemCount);
IContextMenu *	ISvBgCm_Constructor(LPSHELLFOLDER pSFParent);
LPSHELLVIEW	IShellView_Constructor(LPSHELLFOLDER);
LPSHELLLINK	IShellLink_Constructor(BOOL);

IShellFolder * ISF_Desktop_Constructor(void);

/* kind of enumidlist */
#define EIDL_DESK	0
#define EIDL_MYCOMP	1
#define EIDL_FILE	2

LPENUMIDLIST	IEnumIDList_Constructor(LPCSTR,DWORD,DWORD);

LPEXTRACTICONA	IExtractIconA_Constructor(LPITEMIDLIST);
HRESULT		CreateStreamOnFile (LPCSTR pszFilename, IStream ** ppstm);	

/* fixme: rename the functions when the shell32.dll has it's own exports namespace */
HRESULT WINAPI  SHELL32_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppv);
HRESULT WINAPI  SHELL32_DllCanUnloadNow(void);

/* fixme: move away */
#define ResultFromShort(i) MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i))

/* menu merging */
#define MM_ADDSEPARATOR         0x00000001L
#define MM_SUBMENUSHAVEIDS      0x00000002L
HRESULT WINAPI Shell_MergeMenus (HMENU hmDst, HMENU hmSrc, UINT uInsert, UINT uIDAdjust, UINT uIDAdjustMax, ULONG uFlags);

/* initialisation for FORMATETC */
#define InitFormatEtc(fe, cf, med) \
	{\
	(fe).cfFormat=cf;\
	(fe).dwAspect=DVASPECT_CONTENT;\
	(fe).ptd=NULL;\
	(fe).tymed=med;\
	(fe).lindex=-1;\
	};

#define KeyStateToDropEffect(kst)\
	(((kst) & MK_CONTROL) ?\
	(((kst) & MK_SHIFT) ? DROPEFFECT_LINK : DROPEFFECT_COPY):\
	DROPEFFECT_MOVE)

/* Systray */
BOOL SYSTRAY_Init(void);

/* Clipboard */
void InitShellOle(void);
void FreeShellOle(void);
BOOL GetShellOle(void);

#ifdef __WIN32OS2__
extern HRESULT (* WINAPI pOleInitialize)(LPVOID reserved);
extern void    (* WINAPI pOleUninitialize)(void);
extern HRESULT (* WINAPI pRegisterDragDrop)(HWND hwnd, IDropTarget* pDropTarget);
extern HRESULT (* WINAPI pRevokeDragDrop)(HWND hwnd);
extern HRESULT (* WINAPI pDoDragDrop)(LPDATAOBJECT,LPDROPSOURCE,DWORD,DWORD*); 
extern void 	(* WINAPI pReleaseStgMedium)(STGMEDIUM* pmedium);
extern HRESULT (* WINAPI pOleSetClipboard)(IDataObject* pDataObj);
extern HRESULT (* WINAPI pOleGetClipboard)(IDataObject** ppDataObj);
#else
HRESULT (* WINAPI pOleInitialize)(LPVOID reserved);
void    (* WINAPI pOleUninitialize)(void);
HRESULT (* WINAPI pRegisterDragDrop)(HWND hwnd, IDropTarget* pDropTarget);
HRESULT (* WINAPI pRevokeDragDrop)(HWND hwnd);
HRESULT (* WINAPI pDoDragDrop)(LPDATAOBJECT,LPDROPSOURCE,DWORD,DWORD*); 
void 	(* WINAPI pReleaseStgMedium)(STGMEDIUM* pmedium);
HRESULT (* WINAPI pOleSetClipboard)(IDataObject* pDataObj);
HRESULT (* WINAPI pOleGetClipboard)(IDataObject** ppDataObj);
#endif

HGLOBAL RenderHDROP(LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);
HGLOBAL RenderSHELLIDLIST (LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);
HGLOBAL RenderSHELLIDLISTOFFSET (LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);
HGLOBAL RenderFILECONTENTS (LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);
HGLOBAL RenderFILEDESCRIPTOR (LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);
HGLOBAL RenderFILENAME (LPITEMIDLIST pidlRoot, LPITEMIDLIST * apidl, UINT cidl);
HGLOBAL RenderPREFEREDDROPEFFECT (DWORD dwFlags);

/* Change Notification */
void InitChangeNotifications(void);
void FreeChangeNotifications(void);

/* file operation */
#define ASK_DELETE_FILE		 1
#define ASK_DELETE_FOLDER	 2
#define ASK_DELETE_MULTIPLE_ITEM 3
#ifdef __WIN32OS2__
#define ASK_CREATE FOLDER        4
#define ASK_OVERWRITE_FILE       5
#endif

BOOL SHELL_DeleteDirectoryA(LPCSTR pszDir, BOOL bShowUI);
BOOL SHELL_DeleteFileA(LPCSTR pszFile, BOOL bShowUI);
#ifdef __WIN32OS2__
#define SHELL_WarnItemDelete SHELL_ConfirmDialog
BOOL SHELL_ConfirmDialog(int nKindOfDialog, LPCSTR szDir);
#else
BOOL SHELL_WarnItemDelete(int nKindOfDialog, LPCSTR szDir);
#endif

#ifdef __WIN32OS2__
#ifdef __cplusplus
extern "C" {
#endif
BOOL SHELL_OsIsUnicode(void);

#include <peexe.h>
PIMAGE_RESOURCE_DIRECTORY GetResDirEntryW(PIMAGE_RESOURCE_DIRECTORY resdirptr,
                                           LPCWSTR name,DWORD root,
                                           BOOL allowdefault);

#ifdef __cplusplus
}
#endif
#else
inline static BOOL SHELL_OsIsUnicode(void)
{
    /* if high-bit of version is 0, we are emulating NT */
    return !(GetVersion() & 0x80000000);
}
#endif

#endif

