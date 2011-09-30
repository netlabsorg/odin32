
/*
 *@@sourcefile xsystray.h:
 *      Extended system tray widget for XCenter/eCenter.
 *
 *      Public API.
 *
 *      This file contains the public API to the Extended system tray widget.
 *      This API is used by applications to add and remove icons from the
 *      special notification area on the desktop provided by the Extended
 *      system tray widget.
 *
 *      Refer to the "API" file for more details about the public API.
 *
 *      Copyright (C) 2009-2011 Dmitriy Kuminov
 *
 *      This file is part of the Extended system tray widget source package.
 *      Extended system tray widget is free software; you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License as
 *      published by the Free Software Foundation, in version 2 as it comes in
 *      the "COPYING" file of the Extended system tray widget distribution. This
 *      program is distributed in the hope that it will be useful, but WITHOUT
 *      ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *      FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *      more details.
 */

#ifndef XSYSTRAY_API_HEADER_INCLUDED
#define XSYSTRAY_API_HEADER_INCLUDED

#if __cplusplus
extern "C" {
#endif

// Use #define XSTAPI_FPTRS before including this header to declare funciton
// pointers instead of functions (useful for dynamic linking to the DLL).
// Use #define XSTAPI_FPTRS_STATIC to declare static pointers. Note that all
// pointers must be initialized with a valid function address before calling!

#ifdef XSTAPI_IMPL
# define XSTAPI(rt,fn) rt __declspec(dllexport) EXPENTRY fn
#else
# if defined(XSTAPI_FPTRS)
#  define XSTAPI(rt,fn) rt (* EXPENTRY fn)
# elif defined(XSTAPI_FPTRS_STATIC)
#  define XSTAPI(rt,fn) static rt (* EXPENTRY fn)
# else
#  define XSTAPI(rt,fn) rt EXPENTRY fn
# endif
#endif

// notification code constants for the notification messages sent by the system
// tray (refer to xstAddSysTrayIcon() for details)
#define XST_IN_MOUSE    0x0001
#define XST_IN_CONTEXT  0x0002
#define XST_IN_WHEEL    0x0003

// structure for XST_IN_MOUSE
typedef struct
{
    ULONG  ulMouseMsg;
           // mouse message (one of WM_BUTTONxyyy)
    POINTS ptsPointerPos;
           // global pointer position at the time of the mouse event
    USHORT fsHitTestRes;
           // hit-test result (see WM_BUTTONxyyy description in PM)
    USHORT fsFlags;
           // keyboard control codes (see WM_BUTTONxyyy description in PM)

} XSTMOUSEMSG, *PXSTMOUSEMSG;

// structure for XST_IN_CONTEXT
typedef struct
{
    POINTS ptsPointerPos;
           // global pointer position at the time of the mouse event
    USHORT fPointer;
           // input device flag (see WM_CONTEXTMENU description in PM)

} XSTCONTEXTMSG, *PXSTCONTEXTMSG;

// structure for XST_IN_WHEEL
typedef struct
{
    ULONG  ulWheelMsg;
           // mouse message (one of WM_HSCROLL or WM_VSCROLL)
    POINTS ptsPointerPos;
           // global pointer position at the time of the mouse event
    USHORT usCmd;
           // command (see WM_HSCROLL/WM_VSCROLL description in PM)

} XSTWHEELMSG, *PXSTWHEELMSG;

/*
 *@@ xstQuerySysTrayVersion:
 *
 *      Returns the version of the Extended system tray in the variables pointed
 *      to by arguments. Any argument may be NULL in which case the
 *      corresponding component of the version is not returned.
 *
 *      Returns TRUE on success and FALSE if the Extended system tray is not
 *      installed or not operational.
 *
 *      NOTE: When the Extended system tray is started up or gets enabled after
 *      being temporarily disabled, it sends a message with the ID returned by
 *      xstGetSysTrayCreatedMsgId() to all top-level WC_FRAME windows on the
 *      desktop to let them add tray icons if they need.
 */

XSTAPI(BOOL, xstQuerySysTrayVersion)
    (PULONG pulMajor,     // out: major version number
     PULONG pulMinor,     // out: minor version number
     PULONG pulRevision); // out: revision number

/*
 *@@ xstAddSysTrayIcon:
 *
 *      Adds an icon for the given window handle to the system tray. The icon ID
 *      is used to distinguish between several icons for the same window handle.
 *      If the icon with the specified ID already exists in the system tray, it
 *      will be replaced.
 *
 *      Returns TRUE on success and FALSE otherwise.
 *
 *      The specified window handle receives notification messages about icon
 *      events using the message ID specified by the ulMsgId parameter. The
 *      layout of the message parameters is as follows:
 *
 *          param1
 *              USHORT  usIconID        icon ID
 *              USHORT  usNotifyCode    notify code, one of XST_IN_ constants
 *
 *          param2
 *              PVOID   pData           notify code specific data (see below)
 *
 *      The following notify codes are currently recognized:
 *
 *          XST_IN_MOUSE:
 *              Mouse event in the icon area. Currently, only mouse click
 *              messages are recognized. param2 is a pointer to the XSTMOUSEMSG
 *              structure containing full mouse message details.
 *
 *          XST_IN_CONTEXT:
 *              Context menu event in the icon area. param2 is a pointer to the
 *              XSTCONTEXTMSG structure containing full message details.
 *
 *          XST_IN_WHEEL:
 *              Mouse wheel event in the icon area. param2 is a pointer to the
 *              XSTWHEELTMSG structure containing full message details.
 *
 *      NOTE: The maximum tooltip text length (including terminating null) is
 *      limited to a value returned by xstGetSysTrayMaxTextLen(). If the
 *      supplied string is longer, it will be truncated.
 */

XSTAPI(BOOL, xstAddSysTrayIcon)
    (HWND hwnd,       // in: window handle associated with the icon
     USHORT usId,     // in: icon ID to add
     HPOINTER hIcon,  // in: icon handle
     PCSZ pcszToolTip,// in: tooltip text
     ULONG ulMsgId,   // in: message ID for notifications
     ULONG ulFlags);  // in: flags (not currently used, must be 0)

/*
 *@@ xstReplaceSysTrayIcon:
 *
 *      Replaces the existing icon previously added by xstAddSysTrayIcon() with
 *      a new icon.
 *
 *      Returns TRUE on success and FALSE otherwise.
 */

XSTAPI(BOOL, xstReplaceSysTrayIcon)
    (HWND hwnd,       // in: window handle associated with the icon
     USHORT usId,     // in: icon ID to change
     HPOINTER hIcon); // in: new icon handle

/*
 *@@ xstRemoveSysTrayIcon:
 *
 *      Removes the icon previously added by xstAddSysTrayIcon() from the system
 *      tray.
 *
 *      Returns TRUE on success and FALSE otherwise.
 */

XSTAPI(BOOL, xstRemoveSysTrayIcon)
    (HWND hwnd,    // in: window handle associated with the icon
     USHORT usId); // in: icon ID to remove

/*
 *@@ xstSetSysTrayIconToolTip:
 *
 *      Sets the tooltip text for the given icon in the system tray. This text
 *      is shown when the mouse pointer is held still over the icon area for
 *      some time.
 *
 *      If pszText is NULL, the tooltip text is reset and will not be shown next
 *      time. The old tooltip is hidden if it is being currently shown.
 *
 *      Returns TRUE on success and FALSE otherwise.
 *
 *      NOTE: The maximum tooltip text length (including terminating null) is
 *      limited to a value returned by xstGetSysTrayMaxTextLen(). If the
 *      supplied string is longer, it will be truncated.
 */

XSTAPI(BOOL, xstSetSysTrayIconToolTip)
    (HWND hwnd,         // in: window handle associated with the icon
     USHORT usId,       // in: icon ID to set the tooltip for
     PCSZ pcszToolTip); // in: tooltip text

/*
 *@@ xstShowSysTrayIconBalloon:
 *
 *      Not implemented yet.
 */

XSTAPI(BOOL, xstShowSysTrayIconBalloon)(HWND hwnd, USHORT usId, PCSZ pcszTitle,
                                        PCSZ pcszText, ULONG ulFlags,
                                        ULONG ulTimeout);

/*
 *@@ xstHideSysTrayIconBalloon:
 *
 *      Not implemented yet.
 */

XSTAPI(BOOL, xstHideSysTrayIconBalloon)(HWND hwnd, USHORT usId);

/*
 *@@ xstQuerySysTrayIconRect:
 *
 *      Obtains a rectangle occupied by the given icon (in screen coordinates,
 *      top right corner exclusive).
 *
 *      Returns TRUE on success and FALSE otherwise.
 */

XSTAPI(BOOL, xstQuerySysTrayIconRect)
    (HWND hwnd,        // in: window handle associated with the icon
     USHORT usId,      // in: icon ID to query the rectangle for
     PRECTL prclRect); // out: where to store the returned rectangle

/*
 *@@ xstGetSysTrayCreatedMsgId:
 *
 *      Returns the ID of the message that is sent by the Extended system tray
 *      to all top-level WC_FRAME windows on the desktop to let them add tray
 *      icons if they need.
 *
 *      NOTE: The returned value never changes until reboot so it is a good
 *      practice to cache it instead of calling this function each time from the
 *      window procedure of every involved window.
 */

XSTAPI(ULONG, xstGetSysTrayCreatedMsgId)();

/*
 *@@ xstGetSysTrayMaxTextLen:
 *
 *      Returns the maximum length of the text (in bytes, including the
 *      terminating null) that can be shown in the tooltop of the icon in the
 *      system tray. You can use the returned value to determine the maximum
 *      length of the string passed as pszText to xstSetSysTrayIconToolTip().
 *
 *      The returned value also defines the maximum length of both the title and
 *      the text (including terminating nulls) of the icon's balloon for the
 *      xstShowSysTrayIconBalloon() call.
 *
 *      Returns TRUE on success and FALSE otherwise.
 *
 *      NOTE: The returned value never changes until reboot so it is a good
 *      practice to cache it instead of calling this function each time.
 */

XSTAPI(ULONG, xstGetSysTrayMaxTextLen)();

#if __cplusplus
}
#endif

#endif // XSYSTRAY_API_HEADER_INCLUDED

