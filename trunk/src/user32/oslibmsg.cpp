/* $Id: oslibmsg.cpp,v 1.74 2003-08-06 11:00:44 sandervl Exp $ */
/*
 * Window message translation functions for OS/2
 *
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * TODO: Some messages that are sent to the frame window are directly passed on to the client
 *       -> Get/PeekMessage never gets them as we return a dummy message for non-client windows
 *       (i.e. menu WM_COMMAND messages)
 *
 * TODO: Filter translation isn't correct! (for posted messages or messages that don't have
 *       a PM version)
 *
 * TODO: Flaw in our message handling; we don't handle posted/sent (by the app)
 *       system messages properly if removed from the queue with PeekMessage.
 *       e.g.
 *       PostMessage(WM_KEYDOWN)
 *       PeekMessage(any, PM_NOREMOVE)
 *       ...
 *       PeekMessage(WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE)
 *
 *       So what we really need is a complete win to os2 message translation
 *       in Post/SendMessage. Quite a lot of work though...
 *
 */
#define  INCL_WIN
#define  INCL_PM
#define  INCL_DOSPROCESS
#define  INCL_WINMESSAGEMGR
#define  INCL_DOSSEMAPHORES
#define  INCL_DOSERRORS
#include <os2wrap.h>
#include <odinwrap.h>
#include <string.h>
#include <misc.h>
#include "oslibmsg.h"
#include <winconst.h>
#include <win32api.h>
#include <winuser32.h>
#include "oslibutil.h"
#include "timer.h"
#include <thread.h>
#include <wprocess.h>
#include <winnls.h>
#include "pmwindow.h"
#include "oslibwin.h"
#include <win\hook.h>
#include <winscan.h>
#include <winkeyboard.h>
#include "user32api.h"

#define DBG_LOCALLOG    DBG_oslibmsg
#include "dbglocal.h"


ODINDEBUGCHANNEL(USER32-OSLIBMSG)



typedef BOOL (EXPENTRY FNTRANS)(MSG *, QMSG *);
typedef FNTRANS *PFNTRANS;

typedef struct
{
   ULONG   msgOS2;
   ULONG   msgWin32;
// PFNTRANS toOS2;
// PFNTRANS toWIN32;
} MSGTRANSTAB, *PMSGTRANSTAB;

//NOTE: Must be ordered by win32 message id!!
MSGTRANSTAB MsgTransTab[] = {
   WM_NULL,          WINWM_NULL,
   WM_CREATE,        WINWM_CREATE,
   WM_DESTROY,       WINWM_DESTROY,
   WM_MOVE,          WINWM_MOVE,            //TODO: Sent directly
   WM_SIZE,          WINWM_SIZE,            //TODO: Sent directly
   WM_ACTIVATE,      WINWM_ACTIVATE,
   WM_SETFOCUS,      WINWM_SETFOCUS,
   WM_SETFOCUS,      WINWM_KILLFOCUS,
   WM_ENABLE,        WINWM_ENABLE,
   WM_PAINT,         WINWM_PAINT,
   WM_CLOSE,         WINWM_CLOSE,
   WM_QUIT,          WINWM_QUIT,
   WM_SHOW,          WINWM_SHOWWINDOW,

   WM_HITTEST,       WINWM_NCHITTEST,

   //todo: not always right if mouse msg turns out to be for the client window
   WM_MOUSEMOVE,     WINWM_NCMOUSEMOVE,
   WM_BUTTON1DOWN,   WINWM_NCLBUTTONDOWN,
   WM_BUTTON1UP,     WINWM_NCLBUTTONUP,
   WM_BUTTON1DBLCLK, WINWM_NCLBUTTONDBLCLK,
   WM_BUTTON2DOWN,   WINWM_NCRBUTTONDOWN,
   WM_BUTTON2UP,     WINWM_NCRBUTTONUP,
   WM_BUTTON2DBLCLK, WINWM_NCRBUTTONDBLCLK,
   WM_BUTTON3DOWN,   WINWM_NCMBUTTONDOWN,
   WM_BUTTON3UP,     WINWM_NCMBUTTONUP,
   WM_BUTTON3DBLCLK, WINWM_NCMBUTTONDBLCLK,

   //TODO: Needs better translation!
   WM_CHAR,          WINWM_KEYDOWN,   //WM_KEYFIRST
   WM_CHAR,          WINWM_KEYUP,
   WM_CHAR,          WINWM_CHAR,
   WM_CHAR,          WINWM_DEADCHAR,
   WM_CHAR,          WINWM_SYSKEYDOWN,
   WM_CHAR,          WINWM_SYSKEYUP,
   WM_CHAR,          WINWM_SYSCHAR,
   WM_CHAR,          WINWM_SYSDEADCHAR,
   WM_CHAR,          WINWM_KEYLAST,

   //
   WM_TIMER,         WINWM_TIMER,

   //
   //todo: not always right if mouse msg turns out to be for the nonclient window
   WM_MOUSEMOVE,     WINWM_MOUSEMOVE,    //WM_MOUSEFIRST
   WM_BUTTON1DOWN,   WINWM_LBUTTONDOWN,
   WM_BUTTON1UP,     WINWM_LBUTTONUP,
   WM_BUTTON1DBLCLK, WINWM_LBUTTONDBLCLK,
   WM_BUTTON2DOWN,   WINWM_RBUTTONDOWN,
   WM_BUTTON2UP,     WINWM_RBUTTONUP,
   WM_BUTTON2DBLCLK, WINWM_RBUTTONDBLCLK,
   WM_BUTTON3DOWN,   WINWM_MBUTTONDOWN,
   WM_BUTTON3UP,     WINWM_MBUTTONUP,
   WM_BUTTON3DBLCLK, WINWM_MBUTTONDBLCLK,
   WM_BUTTON3DBLCLK, WINWM_MOUSEWHEEL,    //WM_MOUSELAST
   999999999,        999999999,
};
#define MAX_MSGTRANSTAB (sizeof(MsgTransTab)/sizeof(MsgTransTab[0]))

//******************************************************************************
//******************************************************************************
void WinToOS2MsgTranslate(MSG *winMsg, QMSG *os2Msg, BOOL isUnicode)
{
  dprintf(("WinToOS2MsgTranslate not implemented"));
//  memcpy(os2Msg, winMsg, sizeof(MSG));
//  os2Msg->hwnd = Win32ToOS2Handle(winMsg->hwnd);
//  os2Msg->reserved = 0;
}
//******************************************************************************
//TODO: NOT COMPLETE nor 100% CORRECT!!!
//If both the minimum & maximum message are unknown, the result can be wrong (max > min)!
//******************************************************************************
ULONG TranslateWinMsg(ULONG msg, BOOL fMinFilter, BOOL fExactMatch = FALSE)
{
    if(msg == 0)
        return 0;

    if(msg >= WINWM_USER)
        return msg + WIN32APP_POSTMSG;

    for(int i=0;i<MAX_MSGTRANSTAB;i++)
    {
        if(fExactMatch) {
            if(MsgTransTab[i].msgWin32 == msg)
                return MsgTransTab[i].msgOS2;
        }
        else {
            if(fMinFilter && MsgTransTab[i].msgWin32 >= msg) {
                return MsgTransTab[i].msgOS2;
            }
            else
            if(!fMinFilter && MsgTransTab[i].msgWin32 >= msg) {
                if(MsgTransTab[i].msgWin32 == msg)
                        return MsgTransTab[i].msgOS2;
                else    return MsgTransTab[i-1].msgOS2;
            }
        }
    }

    //not found, get everything
    dprintf2(("WARNING: TranslateWinMsg: message %x not found", msg));
    return 0;
}
//******************************************************************************
//******************************************************************************
void OSLibWinPostQuitMessage(ULONG nExitCode)
{
 APIRET rc;

  //NOTE: mp2 must always be zero or else we won't be able to distinguish
  //      between the WM_QUIT sent by us and the one sent by the window list!!
  rc = WinPostQueueMsg(NULLHANDLE, WM_QUIT, MPFROMLONG(nExitCode), 0);
  dprintf(("WinPostQueueMsg %d returned %d", nExitCode, rc));
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinDispatchMsg(MSG *msg, BOOL isUnicode)
{
 TEB  *teb;
 QMSG  os2msg;
 LONG  rc;

  teb = GetThreadTEB();
  if(teb == NULL) {
        DebugInt3();
        return FALSE;
  }

  //TODO: What to do if app changed msg? (translate)
  //  WinToOS2MsgTranslate(msg, &qmsg, isUnicode);

  if(!memcmp(msg, &teb->o.odin.winmsg, sizeof(MSG)) || msg->hwnd == 0) {
        memcpy(&os2msg, &teb->o.odin.os2msg, sizeof(QMSG));
        teb->o.odin.os2msg.time = -1;
        teb->o.odin.winmsg.time = -1;
        if(msg->hwnd) {
                teb->o.odin.nrOfMsgs = 1;
                teb->o.odin.msgstate++; //odd -> next call to our PM window handler should dispatch the translated msg
                memcpy(&teb->o.odin.msg, msg, sizeof(MSG));
        }
        if(os2msg.hwnd || os2msg.msg == WM_QUIT) {
                memset(&teb->o.odin.os2msg, 0, sizeof(teb->o.odin.os2msg));
                memset(&teb->o.odin.winmsg, 0, sizeof(teb->o.odin.winmsg));
                return (LONG)WinDispatchMsg(teb->o.odin.hab, &os2msg);
        }
        //SvL: Don't dispatch messages sent by PostThreadMessage (correct??)
        //     Or WM_TIMER msgs with no window handle or timer proc
        return 0;

  }
  else {//is this allowed?
//        dprintf(("WARNING: OSLibWinDispatchMsg: called with own message!"));
        return isUnicode ? SendMessageW(msg->hwnd, msg->message, msg->wParam, msg->lParam) :
                           SendMessageA(msg->hwnd, msg->message, msg->wParam, msg->lParam);
  }
}
//******************************************************************************
// ReturnQueuedWMCHAR:
//
// Check for a queued WM_CHAR message (e.g. inserted by TranslateMessage)
//******************************************************************************
BOOL ReturnQueuedWMCHAR(LPMSG pMsg, TEB *teb, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax, 
                        BOOL isUnicode, BOOL fRemove)
{
    if(teb->o.odin.fTranslated && (!hwnd || hwnd == teb->o.odin.msgWCHAR.hwnd))
    {
        dprintf(("Return queued WM_CHAR message hwnd=%x msg=%d wParam=%x lParam=%x", teb->o.odin.msgWCHAR.hwnd, teb->o.odin.msgWCHAR.message, teb->o.odin.msgWCHAR.wParam, teb->o.odin.msgWCHAR.lParam));

        if(uMsgFilterMin) {
            if(teb->o.odin.msgWCHAR.message < uMsgFilterMin)
                return FALSE;
        }
        if(uMsgFilterMax) {
            if(teb->o.odin.msgWCHAR.message > uMsgFilterMax)
                return FALSE;
        }
    
        if(fRemove & PM_REMOVE_W) {
            teb->o.odin.fTranslated = FALSE;
            teb->o.odin.os2msg.msg  = 0;
            teb->o.odin.os2msg.hwnd = 0;
        }
        memcpy(pMsg, &teb->o.odin.msgWCHAR, sizeof(MSG));
        //After SetFocus(0), all keystrokes are converted in WM_SYS*
        if(pMsg->message == WINWM_CHAR && fIgnoreKeystrokes) {
            pMsg->message = WINWM_SYSCHAR;
        }

        if(!IsWindow(pMsg->hwnd)) {
            //could be a queued char message for a window that was just destroyed
            //when that's the case, we ignore it (MFC assertions are triggered by this)
            teb->o.odin.fTranslated = FALSE;
            teb->o.odin.os2msg.msg  = 0;
            teb->o.odin.os2msg.hwnd = 0;
            return FALSE;
        }

        // @@@PH verify this
        // if this is a keyup or keydown message, we've got to
        // call the keyboard hook here
        // send keyboard messages to the registered hooks
        if(fRemove & PM_REMOVE_W) 
        {
            switch (pMsg->message)
            {
            case WINWM_KEYDOWN:
            case WINWM_KEYUP:
            case WINWM_SYSKEYDOWN:
            case WINWM_SYSKEYUP:
                // only supposed to be called upon WM_KEYDOWN
                // and WM_KEYUP according to docs.
                if(ProcessKbdHook(pMsg, fRemove))
                    return FALSE;
                break;
            }
        }
    
        //GetMessageW and PeekMessageW expect the character code in UTF-16
        //(we save it in ascii format)
        if(isUnicode && (pMsg->message == WINWM_CHAR ||  pMsg->message == WINWM_SYSCHAR))
        {
            CHAR  charA;
            WCHAR charW;
    
            charA = pMsg->wParam;
            MultiByteToWideChar(CP_ACP, 0, &charA, 1, &charW, 1);
            pMsg->wParam= charW;
            dprintf(("ReturnQueuedWMCHAR: Convert to Unicode src=%x res=%x", charA, charW));
        }
        return TRUE;
    }
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                    BOOL isUnicode)
{
    BOOL rc, eaten;
    TEB  *teb;
    QMSG  os2msg;
    HWND  hwndOS2 = 0;
    ULONG filtermin, filtermax;

    if(hwnd) {
        hwndOS2 = Win32ToOS2Handle(hwnd);
        if(hwndOS2 == NULL) {
                    memset(pMsg, 0, sizeof(MSG));
                    dprintf(("GetMsg: window %x NOT FOUND!", hwnd));
                    SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
                    return TRUE;
            }
    }

    teb = GetThreadTEB();
    if(teb == NULL) {
        DebugInt3();
        return TRUE;
    }

    //check for a queued WM_CHAR message (e.g. inserted by TranslateMessage)
    if(ReturnQueuedWMCHAR(pMsg, teb, hwnd, uMsgFilterMin, uMsgFilterMax, isUnicode, PM_REMOVE_W) == TRUE)
    {
        return (pMsg->message != WINWM_QUIT);
    }

continuegetmsg:
    if(hwnd) {
        filtermin = TranslateWinMsg(uMsgFilterMin, TRUE);
        filtermax = TranslateWinMsg(uMsgFilterMax, FALSE);
        if(filtermin > filtermax) {
            ULONG tmp = filtermin;
            filtermin = filtermax;
            filtermax = filtermin;
        }
        do {
            WinWaitMsg(teb->o.odin.hab, filtermin, filtermax);
            rc = OSLibWinPeekMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, PM_REMOVE_W, isUnicode);
        }
        while(rc == FALSE);

        return (pMsg->message != WINWM_QUIT);
    }
    else
    {
        filtermin = TranslateWinMsg(uMsgFilterMin, TRUE);
        filtermax = TranslateWinMsg(uMsgFilterMax, FALSE);
        if(filtermin > filtermax) {
            ULONG tmp = filtermin;
            filtermin = filtermax;
            filtermax = filtermin;
        }
        rc = WinGetMsg(teb->o.odin.hab, &os2msg, 0, filtermin, filtermax);
        if (os2msg.msg == WM_QUIT && ((ULONG)os2msg.mp2 != 0) ) {
            // Don't return FALSE when the window list sends us
            // a WM_QUIT message, improper killing can lead to
            // application crashes.
            // In the WM_QUIT handler in pmwindow we send a WM_CLOSE
            // in this case. When the app calls PostQuitMessage (mp2 == 0),
            // then we handle it the normal way
            rc = 1;
        }
    }
    if(OS2ToWinMsgTranslate((PVOID)teb, &os2msg, pMsg, isUnicode, MSG_REMOVE) == FALSE) {
        //dispatch untranslated message immediately
        WinDispatchMsg(teb->o.odin.hab, &os2msg);
        //and get the next one
        return OSLibWinGetMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, isUnicode);
    }

    memcpy(&teb->o.odin.os2msg, &os2msg, sizeof(QMSG));
    memcpy(&teb->o.odin.winmsg, pMsg, sizeof(MSG));

    // send keyboard messages to the registered hooks
    switch (pMsg->message)
    {
    case WINWM_KEYDOWN:
    case WINWM_KEYUP:
    case WINWM_SYSKEYDOWN:
    case WINWM_SYSKEYUP:
        // only supposed to be called upon WM_KEYDOWN
        // and WM_KEYUP according to docs.
        if(ProcessKbdHook(pMsg, TRUE))
            goto continuegetmsg;
        break;
    case WINWM_CHAR:
    case WINWM_SYSCHAR:
        // prevent from calling wrong DispatchMsg() (DBCS generated WM_CHAR)
        memset( &teb->o.odin.winmsg, 0, sizeof( MSG ));
        break;
    }
    return rc;
}


//******************************************************************************
//PeekMessage retrieves only messages associated with the window identified by the
//hwnd parameter or any of its children as specified by the IsChild function, and within
//the range of message values given by the uMsgFilterMin and uMsgFilterMax
//parameters. If hwnd is NULL, PeekMessage retrieves messages for any window that
//belongs to the current thread making the call. (PeekMessage does not retrieve
//messages for windows that belong to other threads.) If hwnd is -1, PeekMessage only
//returns messages with a hwnd value of NULL, as posted by the PostAppMessage
//function. If uMsgFilterMin and uMsgFilterMax are both zero, PeekMessage returns all
//available messages (no range filtering is performed).
//TODO: Not working as specified right now!
//******************************************************************************
BOOL OSLibWinPeekMsg(LPMSG pMsg, HWND hwnd, UINT uMsgFilterMin, UINT uMsgFilterMax,
                     DWORD fRemove, BOOL isUnicode)
{
    BOOL  rc, eaten;
    TEB  *teb;
    QMSG  os2msg;
    HWND  hwndOS2 = 0;

    if(uMsgFilterMin > uMsgFilterMax) {
        //TODO: is this correct behaviour?
        dprintf(("!ERROR!: invalid message filter range!!!"));
        SetLastError(ERROR_INVALID_PARAMETER_W);
        return FALSE;
    }
    if(hwnd && hwnd != -1) {
        hwndOS2 = Win32ToOS2Handle(hwnd);
        if(hwndOS2 == NULL) {
            dprintf(("PeekMsg: window %x NOT FOUND!", hwnd));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE_W);
            return FALSE;
        }
    }

    teb = GetThreadTEB();
    if(teb == NULL) {
        DebugInt3();
        return FALSE;
    }

    //check for a queued WM_CHAR message (e.g. inserted by TranslateMessage)
    if(ReturnQueuedWMCHAR(pMsg, teb, hwnd, uMsgFilterMin, uMsgFilterMax, isUnicode, fRemove) == TRUE)
    {
        return TRUE;
    }

continuepeekmsg:
    if(uMsgFilterMin && uMsgFilterMax)
    {   //we can't use the PM message filter (since the message nrs aren't similar), so we must
        //filter each message seperately
        //to do so, we will translate each win32 message in the filter range and call WinPeekMsg
        ULONG ulPMFilter;

        for(int i=0;i<uMsgFilterMax-uMsgFilterMin+1;i++) 
        {
            rc = 0;

            ulPMFilter = TranslateWinMsg(uMsgFilterMin+i, TRUE, TRUE);
            if(ulPMFilter) {
                rc = WinPeekMsg(teb->o.odin.hab, &os2msg, hwndOS2, ulPMFilter, ulPMFilter,
                                (fRemove & PM_REMOVE_W) ? PM_REMOVE : PM_NOREMOVE);
                //Sadly indeed WinPeekMsg sometimes does not filter well!
                if (rc && (os2msg.msg != ulPMFilter)) {// drop this message
                   dprintf(("WARNING: WinPeekMsg returns %x even though we filter for %x", os2msg.msg, ulPMFilter));
                   rc = 0;
                }
            }
            if(rc) {
                break;
            }
        }
    }
    else {
        rc = WinPeekMsg(teb->o.odin.hab, &os2msg, hwndOS2, 0, 0, (fRemove & PM_REMOVE_W) ? PM_REMOVE : PM_NOREMOVE);
    }
    if(rc == FALSE) {
        return FALSE;
    }

    // @@@PH
    // warning - OS2ToWinMsgTranslate might insert additional messages
    // into the queue
    if(OS2ToWinMsgTranslate((PVOID)teb, &os2msg, pMsg, isUnicode, (fRemove & PM_REMOVE_W) ? MSG_REMOVE : MSG_NOREMOVE) == FALSE)
    {
         //unused PM message; dispatch immediately and grab next one
        dprintf2(("OSLibWinPeekMsg: Untranslated message; dispatched immediately"));
        if(!(fRemove & PM_REMOVE_W)) {
            rc = WinPeekMsg(teb->o.odin.hab, &os2msg, os2msg.hwnd, os2msg.msg,
                            os2msg.msg, PM_REMOVE);
        }
        WinDispatchMsg(teb->o.odin.hab, &os2msg);
        return OSLibWinPeekMsg(pMsg, hwnd, uMsgFilterMin, uMsgFilterMax, fRemove, isUnicode);
    }
    //TODO: This is not safe! There's no guarantee this message will be dispatched and it might overwrite a previous message
    if(fRemove & PM_REMOVE_W) {
        memcpy(&teb->o.odin.os2msg, &os2msg, sizeof(QMSG));
        memcpy(&teb->o.odin.winmsg, pMsg, sizeof(MSG));
    }

    // send keyboard messages to the registered hooks
    if(fRemove & PM_REMOVE_W) {
        switch (pMsg->message)
        {
        case WINWM_KEYDOWN:
        case WINWM_KEYUP:
        case WINWM_SYSKEYDOWN:
        case WINWM_SYSKEYUP:
            // only supposed to be called upon WM_KEYDOWN
            // and WM_KEYUP according to docs.
            if(ProcessKbdHook(pMsg, fRemove))
                goto continuepeekmsg;
            break;
        case WINWM_CHAR:
        case WINWM_SYSCHAR:
            // prevent from calling wrong DispatchMsg() (DBCS generated WM_CHAR)
            memset( &teb->o.odin.winmsg, 0, sizeof( MSG ));
            break;
        }
    }

    return rc;
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinQueryMsgTime()
{
    return WinQueryMsgTime(GetThreadHAB());
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinWaitMessage()
{
    return WinWaitMsg(GetThreadHAB(), 0, 0);
}
//******************************************************************************
//TODO: QS_HOTKEY
//******************************************************************************
ULONG OSLibWinQueryQueueStatus()
{
    ULONG statusOS2, statusWin32 = 0;

    statusOS2 = WinQueryQueueStatus(HWND_DESKTOP);

    if(statusOS2 & QS_KEY)
        statusWin32 |= QS_KEY_W;
    if(statusOS2 & QS_MOUSEBUTTON)
        statusWin32 |= QS_MOUSEBUTTON_W;
    if(statusOS2 & QS_MOUSEMOVE)
        statusWin32 |= QS_MOUSEMOVE_W;
    if(statusOS2 & QS_TIMER)
        statusWin32 |= QS_TIMER_W;
    if(statusOS2 & QS_PAINT)
        statusWin32 |= QS_PAINT_W;
    if(statusOS2 & QS_POSTMSG)
        statusWin32 |= QS_POSTMESSAGE_W;
    if(statusOS2 & QS_SENDMSG)
        statusWin32 |= QS_SENDMESSAGE_W;

    return statusWin32;
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinInSendMessage()
{
    return WinInSendMsg(GetThreadHAB());
}
//******************************************************************************
//******************************************************************************
DWORD OSLibWinGetMessagePos()
{
    APIRET rc;
    POINTL ptl;

    rc = WinQueryMsgPos(GetThreadHAB(), &ptl);
    if(!rc) {
        return 0;
    }
    //convert to windows coordinates
    return MAKEULONG(ptl.x,mapScreenY(ptl.y));
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinGetMessageTime()
{
    return (LONG)WinQueryMsgTime(GetThreadHAB());
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinReplyMessage(ULONG result)
{
    return (BOOL)WinReplyMsg( NULLHANDLE, NULLHANDLE, HMQ_CURRENT, (MRESULT)result);
}

//******************************************************************************

/**
 * Send and Post message helper for packing down interprocess and interthread messages.
 *
 * @returns Pointer to packet on success. (shared memory)
 * @returns NULL on failure with SendMessage return code suggestion in *pRc if pRc is set.
 * @param   hwndOdin    Odin window handle. (NULL allowed)
 * @param   hwndOS2     OS/2 window handle.
 * @param   msg         Message id.
 * @param   wParam      Message param.
 * @param   lParam      Message param.
 * @param   fUnicode    Unicode or ansi indicator.
 * @param   pRc         Where to store SendMessage return code. Optional.
 * @author  knut st. osmundsen<bird@anduin.net>
 */
void * OSLibPackMessage(HWND hwndOdin, HWND hwndOS2, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode, PULONG pRc)
{
    POSTMSG_PACKET  * pMsgPacket;

    /*
     * Pack message by id.
     */
    switch (msg)
    {
        /*
         * lParam = PCOPYDATASTRUCT.
         *      Must move this to shared memory together with any
         *      data it's pointing at.
         *
         *      We put everything into the package that might ease cleanup...
         *      WARNING! Currently there are cleanup hacks which works with acrobat.
         */
        case WINWM_COPYDATA:
        {
            PCOPYDATASTRUCT_W pOrg = (PCOPYDATASTRUCT_W)lParam;
            dprintf(("user32::oslibmsg::OSLibPackMessage - WM_COPYDATA: lParam=%#x  dwData=%#x cbData=%d lpData=%#x",
                     pOrg, pOrg ? pOrg->dwData : -1, pOrg ? pOrg->cbData : -1, pOrg ? pOrg->lpData : (LPVOID)-1));

            /*
             * Calc packet size.
             */
            unsigned cb = sizeof(POSTMSG_PACKET);
            if (pOrg)
            {
                cb += sizeof(COPYDATASTRUCT_W);
                if (pOrg->lpData && pOrg->cbData)
                    cb += 16 + pOrg->cbData; //add 16 Bytes for safty and alignment.
            }

            /*
             * Allocate packet.
             */
            pMsgPacket = (POSTMSG_PACKET *)_smalloc(cb);
            if (!pMsgPacket)
            {
                dprintf(("user32::oslibmsg::OSLibPackMessage - WM_COPYDATA: failed to allocate %d shared bytes for packing", cb));
                DebugInt3();
                if (pRc)
                    *pRc = FALSE;
                //@todo figure out which error to set. This is plain guesswork!
                SetLastError(ERROR_NOT_ENOUGH_MEMORY_W);
                break;
            }

            /*
             * Initialize packet.
             */
            PCOPYDATASTRUCT_W pNew = (PCOPYDATASTRUCT_W)(pMsgPacket + 1);
            pMsgPacket->wParam = wParam;
            pMsgPacket->lParam = (LPARAM)pNew;
            *pNew = *pOrg;
            if (pNew->cbData && pNew->lpData)
            {
                pNew->lpData = (LPVOID)(((unsigned)(pNew + 1) + 15) & ~15); //16byte align for safty.
                //@todo what about a safe_memcpy?
                memcpy(pNew->lpData, pOrg->lpData, pNew->cbData);
            }

            /* done! */
            dprintf(("user32::oslibmsg::OSLibPackMessage - WM_COPYDATA: Packed down %d bytes at %#x (pMsgPacket)",
                     cb, pMsgPacket));
            break;
        }


        /*
         * Default packing
         */
        default:
        {
            pMsgPacket = (POSTMSG_PACKET *)_smalloc(sizeof(POSTMSG_PACKET));
            if (!pMsgPacket)
            {
                dprintf(("user32::oslibmsg::OSLibPackMessage - allocated packet structure is NULL"));
                if (pRc)
                {   // Can't find any better return code than 0 :/
                    *pRc = 0;
                }
                DebugInt3();
                break;
            }
            pMsgPacket->wParam = wParam;
            pMsgPacket->lParam = lParam;
        }

    }

    return pMsgPacket;
}


/**
 * Send an inter thread/proces message.
 *
 * @returns
 * @param   hwnd        OS/2 hwnd.
 * @param   msg         Odin message id.
 * @param   wParam      Message param.
 * @param   lParam      Message param.
 * @param   fUnicode    Unicode indicator.
 */
ULONG OSLibSendMessage(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode)
{
    ULONG           rc;                 /* return code on packing failure */
    void *          pvMsgPacket;        /* packed message (shared memory) */

    /*
     * Call message packer.
     */
    pvMsgPacket = OSLibPackMessage(NULLHANDLE, hwnd, msg, wParam, lParam, fUnicode, &rc);
    if (!pvMsgPacket)
    {
        dprintf(("user32::oslibmsg::OSLibSendMessage - Failed to pack message !!"));
        DebugInt3();
        return rc;
    }

    return (ULONG)WinSendMsg(hwnd, WIN32APP_POSTMSG+msg, (MPARAM)((fUnicode) ? WIN32MSG_MAGICW : WIN32MSG_MAGICA), pvMsgPacket);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibSendWinMessage(HWND hwnd, ULONG winmsg)
{
    return (BOOL)WinSendMsg(Win32ToOS2Handle(hwnd), TranslateWinMsg(winmsg, TRUE), 0, 0);
}
//******************************************************************************
//******************************************************************************
ULONG OSLibWinBroadcastMsg(ULONG msg, ULONG wParam, ULONG lParam, BOOL fSend)
{
    #ifdef DEBUG
    if (msg == WINWM_COPYDATA)
    {
        dprintf(("user32::oslibmsg::OSLibWinBroadcastMsg - WM_COPYDATA will not work outside this process !!!"));
        DebugInt3();
    }
    #endif
    return WinBroadcastMsg(HWND_DESKTOP, WIN32APP_POSTMSG+msg, (MPARAM)wParam, (MPARAM)lParam,
                           (fSend) ? BMSG_SEND : BMSG_POST);
}


/**
 * Post a message.
 *
 * @returns Success indicator.
 *
 * @param   hwndWin32   Odin window handle.
 * @param   hwndOS2     OS/2 window handle
 * @param   msg         Odin message id.
 * @param   wParam      Message param.
 * @param   lParam      Message param.
 * @param   fUnicode    Unicode indicator.
 */
BOOL OSLibPostMessage(HWND hwndWin32, HWND hwndOS2, ULONG msg, ULONG wParam, ULONG lParam, BOOL fUnicode)
{
    void *          pvMsgPacket;        /* packed message (shared memory) */

    /*
     * Call message packer.
     */
    pvMsgPacket = OSLibPackMessage(hwndWin32, hwndOS2, msg, wParam, lParam, fUnicode, NULL);
    if (!pvMsgPacket)
    {
        dprintf(("user32::oslibmsg::OSLibPostMessage - Failed to pack message !!"));
        DebugInt3();
        return FALSE;
    }

    /*
     * Post the message.
     * Signal the receiver for if it's doing MsgWaitForMultipleObjects() at the time.
     */
    TEB *teb = GetTEBFromThreadId(GetWindowThreadProcessId(hwndWin32, NULL));
    BOOL ret = WinPostMsg(hwndOS2,
                          WIN32APP_POSTMSG+msg,
                          (MPARAM)((fUnicode) ? WIN32MSG_MAGICW : WIN32MSG_MAGICA),
                          pvMsgPacket);
    if (teb && (teb->o.odin.dwWakeMask & QS_POSTMESSAGE_W))
    {
        //thread is blocked in MsgWaitForMultipleObjects waiting for
        //posted messages
        dprintf(("PostMessage: Wake up thread %x which is blocked in MsgWaitForMultipleObjects", teb->o.odin.threadId));
        SetEvent(teb->o.odin.hPostMsgEvent);
    }
    return ret;
}
//******************************************************************************
//Direct posting of messages that must remain invisible to the win32 app
//******************************************************************************
BOOL OSLibPostMessageDirect(HWND hwnd, ULONG msg, ULONG wParam, ULONG lParam)
{
    return WinPostMsg(hwnd, msg, (MPARAM)wParam, (MPARAM)lParam);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibPostThreadMessage(ULONG threadid, UINT msg, WPARAM wParam, LPARAM lParam, BOOL fUnicode)
{
    TEB *teb = GetTEBFromThreadId(threadid);
    POSTMSG_PACKET *packet = (POSTMSG_PACKET *)_smalloc(sizeof(POSTMSG_PACKET));
    BOOL ret;

    if (NULL == packet)
    {
        dprintf(("user32::oslibmsg::OSLibPostMessage - allocated packet structure is NULL"));

        DebugInt3();
        // PH: we can provide a correct returncode
        return FALSE;
    }

    if(teb == NULL) {
        dprintf(("OSLibPostThreadMessage: thread %x not found!", threadid));
        return FALSE;
    }
    dprintf(("PostThreadMessageA %x %x %x %x -> hmq %x", threadid, msg, wParam, lParam, teb->o.odin.hmq));
    packet->wParam   = wParam;
    packet->lParam   = lParam;

    ret = WinPostQueueMsg((HMQ)teb->o.odin.hmq, WIN32APP_POSTMSG+msg,
                          (MPARAM)((fUnicode) ? WIN32MSG_MAGICW : WIN32MSG_MAGICA),
                          (MPARAM)packet);

    if(ret == FALSE)
    {
       SetLastError(ERROR_INVALID_PARAMETER_W);
       return FALSE;
    }

    if(teb->o.odin.dwWakeMask & QS_POSTMESSAGE_W) {
        //thread is blocked in MsgWaitForMultipleObjects waiting for
        //posted messages
        dprintf(("PostMessage: Wake up thread %x which is blocked in MsgWaitForMultipleObjects", teb->o.odin.threadId));
        SetEvent(teb->o.odin.hPostMsgEvent);
    }

    SetLastError(ERROR_SUCCESS_W);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
DWORD GetThreadMessageExtraInfo()
{
    TEB *teb;

    teb = GetThreadTEB();
    if(teb)
    {
        return teb->o.odin.dwMsgExtraInfo;
    }
    dprintf(("GetThreadMessageExtraInfo: teb == NULL!!"));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD SetThreadMessageExtraInfo(DWORD lParam)
{
    TEB *teb;

    teb = GetThreadTEB();
    if(teb)
    {
         teb->o.odin.dwMsgExtraInfo = lParam;
    }
    else dprintf(("SetThreadMessageExtraInfo: teb == NULL!!"));
    return 0;
}
//******************************************************************************
//******************************************************************************
