/* $Id: oleMenu.cpp,v 1.4 2001-01-25 20:17:24 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * OLEMENU functions.
 * 
 * 4/9/99
 * 
 * Copyright 1999 David J. Raison
 *
 * Some portions from Wine Implementation (2/9/99)
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Noel Borthwick 
 */

#include "ole32.h"
#include "commctrl.h"
#include "oString.h"
#include <assert.h>
#include "storage32.h"

// ======================================================================
// Local Data
// ======================================================================

typedef struct tagOleMenuDescriptor  /* OleMenuDescriptor */
{
    HWND              		hwndFrame;         /* The containers frame window */
    HWND              		hwndActiveObject;  /* The active objects window */
    OLEMENUGROUPWIDTHS		mgw;               /* OLE menu group widths for the shared menu */
    HMENU             		hmenuCombined;     /* The combined menu */
    BOOL              		bIsServerItem;     /* True if the currently open popup belongs to the server */
} OleMenuDescriptor;

typedef struct tagOleMenuHookItem   /* OleMenu hook item in per thread hook list */
{
    DWORD			tid;                /* Thread Id  */
    HANDLE			hHeap;             /* Heap this is allocated from */
    HHOOK			GetMsg_hHook;       /* message hook for WH_GETMESSAGE */
    HHOOK			CallWndProc_hHook;  /* message hook for WH_CALLWNDPROC */
} OleMenuHookItem;

/*
 * Dynamic pointer array of per thread message hooks (maintained by OleSetMenuDescriptor)
 */
static HDPA OLEMenu_MsgHookDPA = NULL;


// ======================================================================
// Prototypes.
// ======================================================================
static void OLEUTL_ReadRegistryDWORDValue(HKEY regKey, DWORD* pdwValue);

// These are the prototypes of the utility methods used to manage a shared menu
extern void OLEMenu_Initialize();
extern void OLEMenu_UnInitialize();

BOOL OLEMenu_InstallHooks( DWORD tid );
BOOL OLEMenu_UnInstallHooks( DWORD tid );
OleMenuHookItem * OLEMenu_IsHookInstalled( DWORD tid, INT *pixHook );
static BOOL OLEMenu_FindMainMenuIndex( HMENU hMainMenu, HMENU hPopupMenu, UINT *pnPos );
BOOL OLEMenu_SetIsServerMenu( HMENU hmenu, OleMenuDescriptor *pOleMenuDescriptor );
LRESULT CALLBACK OLEMenu_CallWndProc(INT code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OLEMenu_GetMsgProc(INT code, WPARAM wParam, LPARAM lParam);

// ======================================================================
// Public API's
// ======================================================================

/*
 * OleCreateMenuDescriptor [OLE32.97]
 * Creates an OLE menu descriptor for OLE to use when dispatching
 * menu messages and commands.
 *
 * PARAMS:
 *    hmenuCombined  -  Handle to the objects combined menu
 *    lpMenuWidths   -  Pointer to array of 6 LONG's indicating menus per group
 *
 */
HOLEMENU WIN32API OleCreateMenuDescriptor(
  HMENU                hmenuCombined,
  LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
  HOLEMENU hOleMenu;
  OleMenuDescriptor *pOleMenuDescriptor;
  int i;

  if ( !hmenuCombined || !lpMenuWidths )
    return 0;

  /* Create an OLE menu descriptor */
  if ( !(hOleMenu = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                sizeof(OleMenuDescriptor) ) ) )
    return 0;

  pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
  if ( !pOleMenuDescriptor )
    return 0;

  /* Initialize menu group widths and hmenu */
  for ( i = 0; i < 6; i++ )
    pOleMenuDescriptor->mgw.width[i] = lpMenuWidths->width[i];
  
  pOleMenuDescriptor->hmenuCombined = hmenuCombined;
  pOleMenuDescriptor->bIsServerItem = FALSE;
  GlobalUnlock( hOleMenu );
      
  return hOleMenu;
}

/*
 * OleDestroyMenuDescriptor [OLE32.99]
 * Destroy the shared menu descriptor
 */
HRESULT WIN32API OleDestroyMenuDescriptor(
  HOLEMENU hmenuDescriptor)
{
  if ( hmenuDescriptor )
    GlobalFree( hmenuDescriptor );
	return S_OK;
}

/*
 * OleSetMenuDescriptor [OLE32.129]
 * Installs or removes OLE dispatching code for the containers frame window
 * FIXME: The lpFrame and lpActiveObject parameters are currently ignored
 * OLE should install context sensitive help F1 filtering for the app when
 * these are non null.
 * 
 * PARAMS:
 *     hOleMenu         Handle to composite menu descriptor
 *     hwndFrame        Handle to containers frame window
 *     hwndActiveObject Handle to objects in-place activation window
 *     lpFrame          Pointer to IOleInPlaceFrame on containers window
 *     lpActiveObject   Pointer to IOleInPlaceActiveObject on active in-place object
 *
 * RETURNS:
 *      S_OK                               - menu installed correctly
 *      E_FAIL, E_INVALIDARG, E_UNEXPECTED - failure
 */
HRESULT WIN32API OleSetMenuDescriptor(
  HOLEMENU               hOleMenu,
  HWND                   hwndFrame,
  HWND                   hwndActiveObject,
  LPOLEINPLACEFRAME        lpFrame,
  LPOLEINPLACEACTIVEOBJECT lpActiveObject)
{
  OleMenuDescriptor *pOleMenuDescriptor = NULL;

  /* Check args */
  if ( !hwndFrame || (hOleMenu && !hwndActiveObject) )
    return E_INVALIDARG;

  if ( lpFrame || lpActiveObject )
  {
     dprintf(("OLE32: OleSetMenuDescriptor(%x, %x, %x, %p, %p), Context sensitive help filtering not implemented!\n",
	(unsigned int)hOleMenu,
	hwndFrame,
	hwndActiveObject,
	lpFrame,
	lpActiveObject));
  }

  /* Set up a message hook to intercept the containers frame window messages.
   * The message filter is responsible for dispatching menu messages from the
   * shared menu which are intended for the object.
   */

  if ( hOleMenu )  /* Want to install dispatching code */
  {
    /* If OLEMenu hooks are already installed for this thread, fail
     * Note: This effectively means that OleSetMenuDescriptor cannot
     * be called twice in succession on the same frame window
     * without first calling it with a null hOleMenu to uninstall */
    if ( OLEMenu_IsHookInstalled( GetCurrentThreadId(), NULL ) )
  return E_FAIL;
        
    /* Get the menu descriptor */
    pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
    if ( !pOleMenuDescriptor )
      return E_UNEXPECTED;

    /* Update the menu descriptor */
    pOleMenuDescriptor->hwndFrame = hwndFrame;
    pOleMenuDescriptor->hwndActiveObject = hwndActiveObject;

    GlobalUnlock( hOleMenu );
    pOleMenuDescriptor = NULL;
    
    /* Add a menu descriptor windows property to the frame window */
    SetPropA( hwndFrame, "PROP_OLEMenuDescriptor", hOleMenu );

    /* Install thread scope message hooks for WH_GETMESSAGE and WH_CALLWNDPROC */
    if ( !OLEMenu_InstallHooks( GetCurrentThreadId() ) )
      return E_FAIL;
  }
  else  /* Want to uninstall dispatching code */
  {
    /* Uninstall the hooks */
    if ( !OLEMenu_UnInstallHooks( GetCurrentThreadId() ) )
      return E_FAIL;
    
    /* Remove the menu descriptor property from the frame window */
    RemovePropA( hwndFrame, "PROP_OLEMenuDescriptor" );
  }
      
  return S_OK;
}

/*
 * OLEMenu_Initialize()
 *
 * Initializes the OLEMENU data structures.
 */
extern void OLEMenu_Initialize()
{
    dprintf(("OLE32: OLEMenu_Initialize"));

    /* Create a dynamic pointer array to store the hook handles */
    if ( !OLEMenu_MsgHookDPA )
	OLEMenu_MsgHookDPA = DPA_CreateEx( 2, GetProcessHeap() );
}

/*
 * OLEMenu_UnInitialize()
 *
 * Releases the OLEMENU data structures.
 */
extern void OLEMenu_UnInitialize()
{
    dprintf(("OLE32: OLEMenu_UnInitialize"));

  /* Release the hook table */
    if ( OLEMenu_MsgHookDPA )
	DPA_Destroy( OLEMenu_MsgHookDPA );

    OLEMenu_MsgHookDPA = NULL;
}

// ======================================================================
// Private functions.
// ======================================================================

/*
 * Internal methods to manage the shared OLE menu in response to the
 * OLE***MenuDescriptor API
 */

/*
 * OLEMenu_InstallHooks
 * Install thread scope message hooks for WH_GETMESSAGE and WH_CALLWNDPROC
 *
 * RETURNS: TRUE if message hooks were succesfully installed
 *          FALSE on failure
 */
BOOL OLEMenu_InstallHooks( DWORD tid )
{
  OleMenuHookItem *pHookItem = NULL;

  if ( !OLEMenu_MsgHookDPA ) /* No hook table? Create one */
  {
    /* Create a dynamic pointer array to store the hook handles */
    if ( !(OLEMenu_MsgHookDPA = DPA_CreateEx( 2, GetProcessHeap() )) ) 
      return FALSE;
  }

  /* Create an entry for the hook table */
  if ( !(pHookItem = (OleMenuHookItem *)HeapAlloc(GetProcessHeap(), 0,
                               sizeof(OleMenuHookItem)) ) )
    return FALSE;

  pHookItem->tid = tid;
  pHookItem->hHeap = GetProcessHeap();
  
  /* Install a thread scope message hook for WH_GETMESSAGE */
  pHookItem->GetMsg_hHook = SetWindowsHookExA( WH_GETMESSAGE, OLEMenu_GetMsgProc,
                                               0, GetCurrentThreadId() );
  if ( !pHookItem->GetMsg_hHook )
    goto CLEANUP;

  /* Install a thread scope message hook for WH_CALLWNDPROC */
  pHookItem->CallWndProc_hHook = SetWindowsHookExA( WH_CALLWNDPROC, OLEMenu_CallWndProc,
                                                    0, GetCurrentThreadId() );
  if ( !pHookItem->CallWndProc_hHook )
    goto CLEANUP;

  /* Insert the hook table entry */
  if ( -1 == DPA_InsertPtr( OLEMenu_MsgHookDPA, 0, pHookItem ) )
    goto CLEANUP;
  
  return TRUE;
  
CLEANUP:
  /* Unhook any hooks */
  if ( pHookItem->GetMsg_hHook )
    UnhookWindowsHookEx( pHookItem->GetMsg_hHook );
  if ( pHookItem->CallWndProc_hHook )
    UnhookWindowsHookEx( pHookItem->CallWndProc_hHook );
  /* Release the hook table entry */
  HeapFree(pHookItem->hHeap, 0, pHookItem );
  
  return FALSE;
}

/*
 * OLEMenu_UnInstallHooks
 * UnInstall thread scope message hooks for WH_GETMESSAGE and WH_CALLWNDPROC
 *
 * RETURNS: TRUE if message hooks were succesfully installed
 *          FALSE on failure
 */
BOOL OLEMenu_UnInstallHooks( DWORD tid )
{
  INT ixHook;
  OleMenuHookItem *pHookItem = NULL;

  if ( !OLEMenu_MsgHookDPA )  /* No hooks set */
    return TRUE;

  /* Lookup the hHook index for this tid */
  if ( !OLEMenu_IsHookInstalled( tid , &ixHook ) )
    return TRUE;

  /* Remove the hook entry from the table(the pointer itself is not deleted) */
  if ( !( pHookItem = (OleMenuHookItem *)DPA_DeletePtr(OLEMenu_MsgHookDPA, ixHook) ) )
    return FALSE;

  /* Uninstall the hooks installed for this thread */
  if ( !UnhookWindowsHookEx( pHookItem->GetMsg_hHook ) )
    goto CLEANUP;
  if ( !UnhookWindowsHookEx( pHookItem->CallWndProc_hHook ) )
    goto CLEANUP;

  /* Release the hook table entry */
  HeapFree(pHookItem->hHeap, 0, pHookItem );

  return TRUE;

CLEANUP:
  /* Release the hook table entry */
  if (pHookItem)
    HeapFree(pHookItem->hHeap, 0, pHookItem );

  return FALSE;
}

/*
 * OLEMenu_IsHookInstalled
 * Tests if OLEMenu hooks have been installed for a thread
 *
 * RETURNS: The pointer and index of the hook table entry for the tid
 *          NULL and -1 for the index if no hooks were installed for this thread
 */
OleMenuHookItem * OLEMenu_IsHookInstalled( DWORD tid, INT *pixHook )
{
  INT ixHook;
  OleMenuHookItem *pHookItem = NULL;

  if ( pixHook )
    *pixHook = -1;
  
  if ( !OLEMenu_MsgHookDPA )  /* No hooks set */
    return NULL;

  /* Do a simple linear search for an entry whose tid matches ours.
   * We really need a map but efficiency is not a concern here. */
  for( ixHook = 0; ; ixHook++ )
  {
    /* Retrieve the hook entry */
    if ( !( pHookItem = (OleMenuHookItem *)DPA_GetPtr(OLEMenu_MsgHookDPA, ixHook) ) )
      return NULL;

    if ( tid == pHookItem->tid )
    {
      if ( pixHook )
        *pixHook = ixHook;
      return pHookItem;
    }
  }
  
  return NULL;
}

/*
 *           OLEMenu_FindMainMenuIndex
 *
 * Used by OLEMenu API to find the top level group a menu item belongs to.
 * On success pnPos contains the index of the item in the top level menu group
 *
 * RETURNS: TRUE if the ID was found, FALSE on failure
 */
static BOOL OLEMenu_FindMainMenuIndex( HMENU hMainMenu, HMENU hPopupMenu, UINT *pnPos )
{
  UINT i, nItems;

  nItems = GetMenuItemCount( hMainMenu );

  for (i = 0; i < nItems; i++)
  {
    HMENU hsubmenu;
      
    /*  Is the current item a submenu? */
    if ( (hsubmenu = GetSubMenu(hMainMenu, i)) != NULL)
    {
      /* If the handle is the same we're done */
      if ( hsubmenu == hPopupMenu )
      {
        if (pnPos)
          *pnPos = i;
        return TRUE;
      }
      /* Recursively search without updating pnPos */
      else if ( OLEMenu_FindMainMenuIndex( hsubmenu, hPopupMenu, NULL ) )
      {
        if (pnPos)
          *pnPos = i;
        return TRUE;
      }
    }
  }

  return FALSE;
}

/*
 *           OLEMenu_SetIsServerMenu
 *
 * Checks whether a popup menu belongs to a shared menu group which is
 * owned by the server, and sets the menu descriptor state accordingly.
 * All menu messages from these groups should be routed to the server.
 *
 * RETURNS: TRUE if the popup menu is part of a server owned group
 *          FASE if the popup menu is part of a container owned group
 */
BOOL OLEMenu_SetIsServerMenu( HMENU hmenu, OleMenuDescriptor *pOleMenuDescriptor )
{
  UINT nPos = 0, nWidth, i;

  pOleMenuDescriptor->bIsServerItem = FALSE;

  /* Don't bother searching if the popup is the combined menu itself */
  if ( hmenu == pOleMenuDescriptor->hmenuCombined )
    return FALSE;

  /* Find the menu item index in the shared OLE menu that this item belongs to */
  if ( !OLEMenu_FindMainMenuIndex( pOleMenuDescriptor->hmenuCombined, hmenu,  &nPos ) )
    return FALSE;
  
  /* The group widths array has counts for the number of elements
   * in the groups File, Edit, Container, Object, Window, Help.
   * The Edit, Object & Help groups belong to the server object
   * and the other three belong to the container.
   * Loop thru the group widths and locate the group we are a member of.
   */
  for ( i = 0, nWidth = 0; i < 6; i++ )
  {
    nWidth += pOleMenuDescriptor->mgw.width[i];
    if ( nPos < nWidth )
    {
      /* Odd elements are server menu widths */
      pOleMenuDescriptor->bIsServerItem = (i%2) ? TRUE : FALSE;
      break;
    }
  }

  return pOleMenuDescriptor->bIsServerItem;
}

/*
 * OLEMenu_CallWndProc
 * Thread scope WH_CALLWNDPROC hook proc filter function (callback)
 * This is invoked from a message hook installed in OleSetMenuDescriptor.
 */
LRESULT CALLBACK OLEMenu_CallWndProc(INT code, WPARAM wParam, LPARAM lParam)
{
  LPCWPSTRUCT pMsg = NULL;
  HOLEMENU hOleMenu = 0;
  OleMenuDescriptor *pOleMenuDescriptor = NULL;
  OleMenuHookItem *pHookItem = NULL;
  WORD fuFlags;

    dprintf(("OLE32: OLEMenu_CallWndProc %i, %04x, %08x", code, wParam, (unsigned)lParam));

  /* Check if we're being asked to process the message */
  if ( HC_ACTION != code )
    goto NEXTHOOK;
      
  /* Retrieve the current message being dispatched from lParam */
  pMsg = (LPCWPSTRUCT)lParam;

  /* Check if the message is destined for a window we are interested in:
   * If the window has an OLEMenu property we may need to dispatch
   * the menu message to its active objects window instead. */

  hOleMenu = (HOLEMENU)GetPropA( pMsg->hwnd, "PROP_OLEMenuDescriptor" );
  if ( !hOleMenu )
    goto NEXTHOOK;

  /* Get the menu descriptor */
  pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
  if ( !pOleMenuDescriptor ) /* Bad descriptor! */
    goto NEXTHOOK;

  /* Process menu messages */
  switch( pMsg->message )
  {
    case WM_INITMENU:
    {
      /* Reset the menu descriptor state */
      pOleMenuDescriptor->bIsServerItem = FALSE;

      /* Send this message to the server as well */
      SendMessageA( pOleMenuDescriptor->hwndActiveObject,
                  pMsg->message, pMsg->wParam, pMsg->lParam );
      goto NEXTHOOK;
    }
    
    case WM_INITMENUPOPUP:
    {
      /* Save the state for whether this is a server owned menu */
      OLEMenu_SetIsServerMenu( (HMENU)pMsg->wParam, pOleMenuDescriptor );
      break;
    }
    
    case WM_MENUSELECT:
    {
      fuFlags = HIWORD(pMsg->wParam);  /* Get flags */
      if ( fuFlags & MF_SYSMENU )
         goto NEXTHOOK;

      /* Save the state for whether this is a server owned popup menu */
      else if ( fuFlags & MF_POPUP )
        OLEMenu_SetIsServerMenu( (HMENU)pMsg->lParam, pOleMenuDescriptor );

      break;
    }
    
    case WM_DRAWITEM:
    {
      LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) pMsg->lParam;
      if ( pMsg->wParam != 0 || lpdis->CtlType != ODT_MENU )
        goto NEXTHOOK;  /* Not a menu message */

      break;
    }

    default:
      goto NEXTHOOK;
  }

  /* If the message was for the server dispatch it accordingly */
  if ( pOleMenuDescriptor->bIsServerItem )
  {
    SendMessageA( pOleMenuDescriptor->hwndActiveObject,
                  pMsg->message, pMsg->wParam, pMsg->lParam );
  }
    
NEXTHOOK:
  if ( pOleMenuDescriptor )
    GlobalUnlock( hOleMenu );
  
  /* Lookup the hook item for the current thread */
  if ( !( pHookItem = OLEMenu_IsHookInstalled( GetCurrentThreadId(), NULL ) ) )
  {
    /* This should never fail!! */
    dprintf(("Warning: Could not retrieve hHook for current thread!"));
    return 0;
  }
  
  /* Pass on the message to the next hooker */
  return CallNextHookEx( pHookItem->CallWndProc_hHook, code, wParam, lParam );
}

/*
 * OLEMenu_GetMsgProc
 * Thread scope WH_GETMESSAGE hook proc filter function (callback)
 * This is invoked from a message hook installed in OleSetMenuDescriptor.
 */
LRESULT CALLBACK OLEMenu_GetMsgProc(INT code, WPARAM wParam, LPARAM lParam)
{
  LPMSG pMsg = NULL;
  HOLEMENU hOleMenu = 0;
  OleMenuDescriptor *pOleMenuDescriptor = NULL;
  OleMenuHookItem *pHookItem = NULL;
  WORD wCode;
  
    dprintf(("OLE32: OLEMenu_GetMsgProc %i, %04x, %08x", code, wParam, (unsigned)lParam ));

  /* Check if we're being asked to process a  messages */
  if ( HC_ACTION != code )
    goto NEXTHOOK;
      
  /* Retrieve the current message being dispatched from lParam */
  pMsg = (LPMSG)lParam;

  /* Check if the message is destined for a window we are interested in:
   * If the window has an OLEMenu property we may need to dispatch
   * the menu message to its active objects window instead. */

  hOleMenu = (HOLEMENU)GetPropA( pMsg->hwnd, "PROP_OLEMenuDescriptor" );
  if ( !hOleMenu )
    goto NEXTHOOK;

  /* Process menu messages */
  switch( pMsg->message )
  {
    case WM_COMMAND:
    {
      wCode = HIWORD(pMsg->wParam);  /* Get notification code */
      if ( wCode )
        goto NEXTHOOK;  /* Not a menu message */
      break;
    }
    default:
      goto NEXTHOOK;
  }

  /* Get the menu descriptor */
  pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
  if ( !pOleMenuDescriptor ) /* Bad descriptor! */
    goto NEXTHOOK;

  /* If the message was for the server dispatch it accordingly */
  if ( pOleMenuDescriptor->bIsServerItem )
  {
    /* Change the hWnd in the message to the active objects hWnd.
     * The message loop which reads this message will automatically
     * dispatch it to the embedded objects window. */
    pMsg->hwnd = pOleMenuDescriptor->hwndActiveObject;
  }
    
NEXTHOOK:
  if ( pOleMenuDescriptor )
    GlobalUnlock( hOleMenu );
  
  /* Lookup the hook item for the current thread */
  if ( !( pHookItem = OLEMenu_IsHookInstalled( GetCurrentThreadId(), NULL ) ) )
  {
    /* This should never fail!! */
    dprintf(("Warning: Could not retrieve hHook for current thread!\n" ));
    return FALSE;
  }
  
  /* Pass on the message to the next hooker */
  return CallNextHookEx( pHookItem->GetMsg_hHook, code, wParam, lParam );
}

