/* $Id: oleDrag.cpp,v 1.1 2001-04-26 19:26:14 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * OLE Drag Drop functions.
 * 
 * 5/9/99
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

// ======================================================================
// Local Data
// ======================================================================

typedef struct tagDropTargetNode
{
    HWND			hwndTarget;
    IDropTarget *		dropTarget;
    struct tagDropTargetNode *	prevDropTarget;
    struct tagDropTargetNode *	nextDropTarget;
} DropTargetNode;

typedef struct tagTrackerWindowInfo
{
    IDataObject *		dataObject;
    IDropSource *		dropSource;
    DWORD       		dwOKEffect;
    DWORD*      		pdwEffect;
    BOOL      			trackingDone;
    HRESULT     		returnValue;

    BOOL      			escPressed;
    HWND      			curDragTargetHWND;
    IDropTarget *		curDragTarget;
} TrackerWindowInfo;

/*
 * Name of our registered window class.
 */
static const char OLEDD_DRAGTRACKERCLASS[] = "WineDragDropTracker32";

/*
 * This is the head of the Drop target container.
 */
static DropTargetNode * targetListHead = NULL;

// ======================================================================
// Prototypes.
// ======================================================================

extern void            OLEDD_Initialize();
extern void            OLEDD_UnInitialize();
static void            OLEDD_InsertDropTarget(
			 DropTargetNode* nodeToAdd);
static DropTargetNode* OLEDD_ExtractDropTarget(
                         HWND hwndOfTarget);
static DropTargetNode* OLEDD_FindDropTarget(
                         HWND hwndOfTarget);
static LRESULT WIN32API  OLEDD_DragTrackerWindowProc(
			 HWND   hwnd, 
			 UINT   uMsg,
			 WPARAM wParam, 
			 LPARAM   lParam);
static void OLEDD_TrackMouseMove(
                         TrackerWindowInfo* trackerInfo,
			 POINT            mousePos,
			 DWORD              keyState);
static void OLEDD_TrackStateChange(
                         TrackerWindowInfo* trackerInfo,
			 POINT            mousePos,
			 DWORD              keyState);
static DWORD OLEDD_GetButtonState();

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// RegisterDragDrop()
// ----------------------------------------------------------------------
HRESULT WIN32API RegisterDragDrop
   (HWND		hwnd,
    LPDROPTARGET	pDropTarget)
{
    dprintf(("OLE32: RegisterDragDrop"));

    DropTargetNode *	dropTargetInfo;

    // First, check if the window is already registered.
    dropTargetInfo = OLEDD_FindDropTarget(hwnd);
    if (dropTargetInfo != NULL)
	return DRAGDROP_E_ALREADYREGISTERED;

    // If it's not there, we can add it. We first create a node for it.
    dropTargetInfo = (DropTargetNode *)HeapAlloc(GetProcessHeap(), 0, sizeof(DropTargetNode));

    if (dropTargetInfo == NULL)
	return E_OUTOFMEMORY;

    dropTargetInfo->hwndTarget     = hwnd;
    dropTargetInfo->prevDropTarget = NULL;
    dropTargetInfo->nextDropTarget = NULL;

    // Don't forget that this is an interface pointer, need to nail it down since
    // we keep a copy of it.
    dropTargetInfo->dropTarget  = pDropTarget;
    IDropTarget_AddRef(dropTargetInfo->dropTarget);

    OLEDD_InsertDropTarget(dropTargetInfo);

    return S_OK;
}

// ----------------------------------------------------------------------
// RevokeDragDrop()
// ----------------------------------------------------------------------
HRESULT WIN32API RevokeDragDrop
    (HWND		hwnd)
{
    dprintf(("OLE32: RevokeDragDrop"));

    DropTargetNode *	dropTargetInfo;

    // First, check if the window is already registered.
    dropTargetInfo = OLEDD_ExtractDropTarget(hwnd);
    if (dropTargetInfo == NULL)
	return DRAGDROP_E_NOTREGISTERED;

    // If it's in there, clean-up it's used memory and references
    IDropTarget_Release(dropTargetInfo->dropTarget);
    HeapFree(GetProcessHeap(), 0, dropTargetInfo);  

    return S_OK;
}

// ----------------------------------------------------------------------
// DoDragDrop()
// ----------------------------------------------------------------------
HRESULT WIN32API DoDragDrop
   (IDataObject *	pDataObject,  // ptr to the data obj
    IDropSource *	pDropSource,  // ptr to the source obj
    DWORD       	dwOKEffect,   // effects allowed by the source
    DWORD *		pdwEffect)    // ptr to effects of the source
{
    dprintf(("OLE32: DoDragDrop"));

    TrackerWindowInfo 	trackerInfo;
    HWND            	hwndTrackWindow;
    MSG             	msg;

    // Setup the drag n drop tracking window.
    trackerInfo.dataObject        = pDataObject;
    trackerInfo.dropSource        = pDropSource;
    trackerInfo.dwOKEffect        = dwOKEffect;
    trackerInfo.pdwEffect         = pdwEffect;
    trackerInfo.trackingDone      = FALSE;
    trackerInfo.escPressed        = FALSE;
    trackerInfo.curDragTargetHWND = 0;
    trackerInfo.curDragTarget     = 0;

    hwndTrackWindow = CreateWindowA(OLEDD_DRAGTRACKERCLASS,
				    "TrackerWindow",
				    WS_POPUP,
				    CW_USEDEFAULT, CW_USEDEFAULT,
				    CW_USEDEFAULT, CW_USEDEFAULT,
				    0,
				    0,
				    0,
				    (LPVOID)&trackerInfo);

    if (hwndTrackWindow != 0)
    {
	// Capture the mouse input
	SetCapture(hwndTrackWindow);

	// Pump messages. All mouse input should go the the capture window.
	while (!trackerInfo.trackingDone && GetMessageA(&msg, 0, 0, 0) )
	{
	    if ( (msg.message >= WM_KEYFIRST) && (msg.message <= WM_KEYFIRST) )
	    {
		// When keyboard messages are sent to windows on this thread, we
		// want to ignore notify the drop source that the state changed.
		// in the case of the Escape key, we also notify the drop source
		// we give it a special meaning.
		if ( (msg.message == WM_KEYDOWN) && (msg.wParam == VK_ESCAPE) )
		    trackerInfo.escPressed = TRUE;

		// Notify the drop source.
		OLEDD_TrackStateChange(&trackerInfo, msg.pt, OLEDD_GetButtonState());
	    }
	    else
	    {
		// Dispatch the messages only when it's not a keyboard message.
		DispatchMessageA(&msg);
	    }
	}

	// Destroy the temporary window.
	DestroyWindow(hwndTrackWindow);

	return trackerInfo.returnValue;
    }

    return E_FAIL;
}

// ----------------------------------------------------------------------
// OLEDD_Initialize()
// ----------------------------------------------------------------------
extern void OLEDD_Initialize()
{
    dprintf(("OLE32: OLEDD_Initialize"));

    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)OLEDD_DragTrackerWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(TrackerWindowInfo*);
    wndClass.hCursor       = 0;
    wndClass.hbrBackground = 0;
    wndClass.lpszClassName = OLEDD_DRAGTRACKERCLASS;
 
    RegisterClassA (&wndClass);
}

// ----------------------------------------------------------------------
// OLEDD_UnInitialize()
// ----------------------------------------------------------------------
extern void OLEDD_UnInitialize()
{
    dprintf(("OLE32: OLEDD_UnInitialize"));

    // Simply empty the list.
    while (targetListHead!=NULL)
    {
	RevokeDragDrop(targetListHead->hwndTarget);
    }
}

// ======================================================================
// Private functions.
// ======================================================================

/***
 * OLEDD_InsertDropTarget()
 *
 * Insert the target node in the tree.
 */
static void OLEDD_InsertDropTarget(DropTargetNode* nodeToAdd)
{
  DropTargetNode*  curNode;
  DropTargetNode** parentNodeLink;

  /*
   * Iterate the tree to find the insertion point.
   */
  curNode        = targetListHead;
  parentNodeLink = &targetListHead;

  while (curNode!=NULL)
  {
    if (nodeToAdd->hwndTarget<curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a smaller HWND, go left
       */
      parentNodeLink = &curNode->prevDropTarget;
      curNode        =  curNode->prevDropTarget;
    }
    else if (nodeToAdd->hwndTarget>curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a larger HWND, go right
       */
      parentNodeLink = &curNode->nextDropTarget;
      curNode        =  curNode->nextDropTarget;
    }
    else
    {
      /*
       * The item was found in the list. It shouldn't have been there
       */
      assert(FALSE);
      return;
    }
  }

  /*
   * If we get here, we have found a spot for our item. The parentNodeLink
   * pointer points to the pointer that we have to modify. 
   * The curNode should be NULL. We just have to establish the link and Voila!
   */
  assert(curNode==NULL);
  assert(parentNodeLink!=NULL);
  assert(*parentNodeLink==NULL);

  *parentNodeLink=nodeToAdd;
}

/***
 * OLEDD_ExtractDropTarget()
 *
 * Removes the target node from the tree.
 */
static DropTargetNode* OLEDD_ExtractDropTarget(HWND hwndOfTarget)
{
  DropTargetNode*  curNode;
  DropTargetNode** parentNodeLink;

  /*
   * Iterate the tree to find the insertion point.
   */
  curNode        = targetListHead;
  parentNodeLink = &targetListHead;

  while (curNode!=NULL)
  {
    if (hwndOfTarget<curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a smaller HWND, go left
       */
      parentNodeLink = &curNode->prevDropTarget;
      curNode        =  curNode->prevDropTarget;
    }
    else if (hwndOfTarget>curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a larger HWND, go right
       */
      parentNodeLink = &curNode->nextDropTarget;
      curNode        =  curNode->nextDropTarget;
    }
    else
    {
      /*
       * The item was found in the list. Detach it from it's parent and 
       * re-insert it's kids in the tree.
       */
      assert(parentNodeLink!=NULL);
      assert(*parentNodeLink==curNode);

      /*
       * We arbitrately re-attach the left sub-tree to the parent.
       */
      *parentNodeLink = curNode->prevDropTarget;

      /*
       * And we re-insert the right subtree
       */
      if (curNode->nextDropTarget!=NULL)
      {
	OLEDD_InsertDropTarget(curNode->nextDropTarget);
      }

      /*
       * The node we found is still a valid node once we complete
       * the unlinking of the kids.
       */
      curNode->nextDropTarget=NULL;
      curNode->prevDropTarget=NULL;

      return curNode;
    }
  }

  /*
   * If we get here, the node is not in the tree
   */
  return NULL;
}

/***
 * OLEDD_FindDropTarget()
 *
 * Finds information about the drop target.
 */
static DropTargetNode* OLEDD_FindDropTarget(HWND hwndOfTarget)
{
  DropTargetNode*  curNode;

  /*
   * Iterate the tree to find the HWND value.
   */
  curNode        = targetListHead;

  while (curNode!=NULL)
  {
    if (hwndOfTarget<curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a smaller HWND, go left
       */
      curNode =  curNode->prevDropTarget;
    }
    else if (hwndOfTarget>curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a larger HWND, go right
       */
      curNode =  curNode->nextDropTarget;
    }
    else
    {
      /*
       * The item was found in the list.
       */
      return curNode;
    }
  }

  /*
   * If we get here, the item is not in the list
   */
  return NULL;
}

/***
 * OLEDD_DragTrackerWindowProc()
 *
 * This method is the WindowProcedure of the drag n drop tracking
 * window. During a drag n Drop operation, an invisible window is created
 * to receive the user input and act upon it. This procedure is in charge
 * of this behavior.
 */
static LRESULT WIN32API OLEDD_DragTrackerWindowProc(
			 HWND   hwnd, 
			 UINT   uMsg,
			 WPARAM wParam, 
			 LPARAM   lParam)
{
  switch (uMsg)
  {
    case WM_CREATE:
    {
      LPCREATESTRUCTA createStruct = (LPCREATESTRUCTA)lParam;

      SetWindowLongA(hwnd, 0, (LONG)createStruct->lpCreateParams); 

      
      break;
    }
    case WM_MOUSEMOVE:
    {
      TrackerWindowInfo* trackerInfo = (TrackerWindowInfo*)GetWindowLongA(hwnd, 0);
      POINT            mousePos;

      /*
       * Get the current mouse position in screen coordinates.
       */
      mousePos.x = LOWORD(lParam);
      mousePos.y = HIWORD(lParam);
      ClientToScreen(hwnd, &mousePos);

      /*
       * Track the movement of the mouse.
       */
      OLEDD_TrackMouseMove(trackerInfo, mousePos, wParam);

      break;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      TrackerWindowInfo* trackerInfo = (TrackerWindowInfo*)GetWindowLongA(hwnd, 0);
      POINT            mousePos;

      /*
       * Get the current mouse position in screen coordinates.
       */
      mousePos.x = LOWORD(lParam);
      mousePos.y = HIWORD(lParam);
      ClientToScreen(hwnd, &mousePos);

      /*
       * Notify everyone that the button state changed
       * TODO: Check if the "escape" key was pressed.
       */
      OLEDD_TrackStateChange(trackerInfo, mousePos, wParam);

      break;
    }
  }

  /*
   * This is a window proc after all. Let's call the default.
   */
  return DefWindowProcA (hwnd, uMsg, wParam, lParam);
}

/***
 * OLEDD_TrackMouseMove()
 *
 * This method is invoked while a drag and drop operation is in effect.
 * it will generate the appropriate callbacks in the drop source
 * and drop target. It will also provide the expected feedback to
 * the user.
 *
 * params:
 *    trackerInfo - Pointer to the structure identifying the
 *                  drag & drop operation that is currently
 *                  active.
 *    mousePos    - Current position of the mouse in screen
 *                  coordinates.
 *    keyState    - Contains the state of the shift keys and the
 *                  mouse buttons (MK_LBUTTON and the like)
 */
static void OLEDD_TrackMouseMove(
  TrackerWindowInfo* trackerInfo,
  POINT            mousePos,
  DWORD              keyState)
{
  HWND   hwndNewTarget = 0;
  HRESULT  hr = S_OK;

  /*
   * Get the handle of the window under the mouse
   */
  hwndNewTarget = WindowFromPoint(mousePos);

  /*
   * Every time, we re-initialize the effects passed to the
   * IDropTarget to the effects allowed by the source.
   */
  *trackerInfo->pdwEffect = trackerInfo->dwOKEffect;

  /*
   * If we are hovering over the same target as before, send the
   * DragOver notification
   */
  if ( (trackerInfo->curDragTarget != 0) && 
       (trackerInfo->curDragTargetHWND==hwndNewTarget) )
  {
    POINTL  mousePosParam;
    
    /*
     * The documentation tells me that the coordinate should be in the target
     * window's coordinate space. However, the tests I made tell me the
     * coordinates should be in screen coordinates.
     */
    mousePosParam.x = mousePos.x;
    mousePosParam.y = mousePos.y;
    
    IDropTarget_DragOver(trackerInfo->curDragTarget,
			 keyState,
			 mousePosParam,
			 trackerInfo->pdwEffect);
  }
  else
  {
    DropTargetNode* newDropTargetNode = 0;
    
    /*
     * If we changed window, we have to notify our old target and check for
     * the new one.
     */
    if (trackerInfo->curDragTarget!=0)
    {
      IDropTarget_DragLeave(trackerInfo->curDragTarget);
    }
    
    /*
     * Make sure we're hovering over a window.
     */
    if (hwndNewTarget!=0)
    {
      /*
       * Find-out if there is a drag target under the mouse
       */
      newDropTargetNode = OLEDD_FindDropTarget(hwndNewTarget);
      
      trackerInfo->curDragTargetHWND = hwndNewTarget;
      trackerInfo->curDragTarget     = newDropTargetNode ? newDropTargetNode->dropTarget : 0;
      
      /*
       * If there is, notify it that we just dragged-in
       */
      if (trackerInfo->curDragTarget!=0)
      {
	POINTL  mousePosParam;
	
	/*
	 * The documentation tells me that the coordinate should be in the target
	 * window's coordinate space. However, the tests I made tell me the
	 * coordinates should be in screen coordinates.
	 */
	mousePosParam.x = mousePos.x;
	mousePosParam.y = mousePos.y;
	
	IDropTarget_DragEnter(trackerInfo->curDragTarget,
			      trackerInfo->dataObject,
			      keyState,
			      mousePosParam,
			      trackerInfo->pdwEffect);
      }
    }
    else
    {
      /*
       * The mouse is not over a window so we don't track anything.
       */
      trackerInfo->curDragTargetHWND = 0;
      trackerInfo->curDragTarget     = 0;
    }
  }

  /*
   * Now that we have done that, we have to tell the source to give 
   * us feedback on the work being done by the target.  If we don't 
   * have a target, simulate no effect.
   */
  if (trackerInfo->curDragTarget==0)
  {
    *trackerInfo->pdwEffect = DROPEFFECT_NONE;
  }

  hr = IDropSource_GiveFeedback(trackerInfo->dropSource,
  				*trackerInfo->pdwEffect);

  /*
   * When we ask for feedback from the drop source, sometimes it will
   * do all the necessary work and sometimes it will not handle it
   * when that's the case, we must display the standard drag and drop
   * cursors.
   */
  if (hr==DRAGDROP_S_USEDEFAULTCURSORS)
  {
    if ( (*trackerInfo->pdwEffect & DROPEFFECT_MOVE) ||
	 (*trackerInfo->pdwEffect & DROPEFFECT_COPY) ||
	 (*trackerInfo->pdwEffect & DROPEFFECT_LINK) )
    {
      SetCursor(LoadCursorA(0, IDC_SIZEALLA));
    }
    else
    {
      SetCursor(LoadCursorA(0, IDC_NOA));
    }
  }  
}

/***
 * OLEDD_TrackStateChange()
 *
 * This method is invoked while a drag and drop operation is in effect.
 * It is used to notify the drop target/drop source callbacks when
 * the state of the keyboard or mouse button change.
 *
 * params:
 *    trackerInfo - Pointer to the structure identifying the
 *                  drag & drop operation that is currently
 *                  active.
 *    mousePos    - Current position of the mouse in screen
 *                  coordinates.
 *    keyState    - Contains the state of the shift keys and the
 *                  mouse buttons (MK_LBUTTON and the like)
 */
static void OLEDD_TrackStateChange(
  TrackerWindowInfo* trackerInfo,
  POINT            mousePos,
  DWORD              keyState)
{
  /*
   * Ask the drop source what to do with the operation.
   */
  trackerInfo->returnValue = IDropSource_QueryContinueDrag(
			       trackerInfo->dropSource,
			       trackerInfo->escPressed, 
			       keyState);
  
  /*
   * All the return valued will stop the operation except the S_OK
   * return value.
   */
  if (trackerInfo->returnValue!=S_OK)
  {
    /*
     * Make sure the message loop in DoDragDrop stops
     */
    trackerInfo->trackingDone = TRUE;

    /*
     * Release the mouse in case the drop target decides to show a popup 
     * or a menu or something.
     */
    ReleaseCapture();
    
    /*
     * If we end-up over a target, drop the object in the target or 
     * inform the target that the operation was cancelled.
     */
    if (trackerInfo->curDragTarget!=0)
    {
      switch (trackerInfo->returnValue)
      {
	/*
	 * If the source wants us to complete the operation, we tell 
	 * the drop target that we just dropped the object in it.
	 */
        case DRAGDROP_S_DROP:
	{
	  POINTL  mousePosParam;
	
	  /*
	   * The documentation tells me that the coordinate should be 
	   * in the target window's coordinate space. However, the tests
	   * I made tell me the coordinates should be in screen coordinates.
	   */
	  mousePosParam.x = mousePos.x;
	  mousePosParam.y = mousePos.y;
	  
	  IDropTarget_Drop(trackerInfo->curDragTarget,
			   trackerInfo->dataObject,
			   keyState,
			   mousePosParam,
			   trackerInfo->pdwEffect);
	  break;
	}
	/*
	 * If the source told us that we should cancel, fool the drop 
	 * target by telling it that the mouse left it's window.
	 */
        case DRAGDROP_S_CANCEL:
	  IDropTarget_DragLeave(trackerInfo->curDragTarget);
	  break;
      }
    }
  }
}

/***
 * OLEDD_GetButtonState()
 *
 * This method will use the current state of the keyboard to build
 * a button state mask equivalent to the one passed in the
 * WM_MOUSEMOVE wParam.
 */
static DWORD OLEDD_GetButtonState()
{
  BYTE  keyboardState[256];
  DWORD keyMask = 0;

  GetKeyboardState(keyboardState);

  if ( (keyboardState[VK_SHIFT] & 0x80) !=0)
    keyMask |= MK_SHIFT;

  if ( (keyboardState[VK_CONTROL] & 0x80) !=0)
    keyMask |= MK_CONTROL;

  if ( (keyboardState[VK_LBUTTON] & 0x80) !=0)
    keyMask |= MK_LBUTTON;

  if ( (keyboardState[VK_RBUTTON] & 0x80) !=0)
    keyMask |= MK_RBUTTON;

  if ( (keyboardState[VK_MBUTTON] & 0x80) !=0)
    keyMask |= MK_MBUTTON;

  return keyMask;
}

