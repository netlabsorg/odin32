/* $Id: edit.cpp,v 1.43 2001-07-08 15:51:41 sandervl Exp $ */
/*
 *      Edit control
 *
 *      Copyright  David W. Metcalfe, 1994
 *      Copyright  William Magro, 1995, 1996
 *      Copyright  Frans van Dorsselaer, 1996, 1997
 *
 *      Copyright  1999 Christoph Bratschi
 *
 * Corel version: 20000513
 * (WINE version: 991212)
 *
 * Status:  complete
 * Version: 5.00
 */

/* CB: todo
  - EN_ALIGN_LTR_EC, EN_ALIGN_RTL_EC -> undocumented notifications
  - text alignment for single and multi line (ES_LEFT, ES_RIGHT, ES_CENTER)
    new in Win98, Win2k: for single line too
  - WinNT/Win2k: higher size limits (single: 0x7FFFFFFE, multi: none)
    SvL: Limits removed. EM_SETTEXTLIMIT has no effect in NT4, SP6 (EM_GETTEXTLIMIT
         only returns that value); limits are simply ignored, no EN_MAXTEXT is ever sent)
  - too many redraws and recalculations!
*/

#include <stdlib.h>
#include <os2win.h>
#include <string.h>
#include "controls.h"
#include "combo.h"

#define DBG_LOCALLOG    DBG_edit
#include "dbglocal.h"

#ifdef DEBUG
char *GetMsgText(int Msg);
#endif

#define BUFLIMIT_MULTI          65534   /* maximum buffer size (not including '\0')
                                           FIXME: BTW, Win95 specs say 65535 (do you dare ???) */
#define BUFLIMIT_SINGLE         32766   /* maximum buffer size (not including '\0') */

#ifdef __WIN32OS2__
#define BUFLIMIT_SINGLE_NT	0x7FFFFFFF
#endif
//#define BUFLIMIT_MULTI  0xFFFFFFFE
//#define BUFLIMIT_SINGLE 0x7FFFFFFF

#define BUFSTART_MULTI          1024    /* starting size */
#define BUFSTART_SINGLE         256     /* starting size */
#define GROWLENGTH              64      /* buffers grow by this much */
#define HSCROLL_FRACTION        3       /* scroll window by 1/3 width */

/*
 *      extra flags for EDITSTATE.flags field
 */
#define EF_MODIFIED             0x0001  /* text has been modified */
#define EF_FOCUSED              0x0002  /* we have input focus */
#define EF_UPDATE               0x0004  /* notify parent of changed state on next WM_PAINT */
#define EF_VSCROLL_TRACK        0x0008  /* don't SetScrollPos() since we are tracking the thumb */
#define EF_HSCROLL_TRACK        0x0010  /* don't SetScrollPos() since we are tracking the thumb */
#define EF_VSCROLL_HACK         0x0020  /* we already have informed the user of the hacked handler */
#define EF_HSCROLL_HACK         0x0040  /* we already have informed the user of the hacked handler */
#define EF_AFTER_WRAP           0x0080  /* the caret is displayed after the last character of a
                                           wrapped line, instead of in front of the next character */
#define EF_USE_SOFTBRK          0x0100  /* Enable soft breaks in text. */

typedef enum
{
        END_0 = 0,      /* line ends with terminating '\0' character */
        END_WRAP,       /* line is wrapped */
        END_HARD,       /* line ends with a hard return '\r\n' */
        END_SOFT        /* line ends with a soft return '\r\r\n' */
} LINE_END;

typedef struct tagLINEDEF {
        INT length;             /* bruto length of a line in bytes */
        INT net_length; /* netto length of a line in visible characters */
        LINE_END ending;
        INT width;              /* width of the line in pixels */
        struct tagLINEDEF *next;
} LINEDEF;

typedef struct
{
        HANDLE heap;                    /* our own heap */
        LPSTR text;                     /* the actual contents of the control */
        INT buffer_size;                /* the size of the buffer */
        INT buffer_limit;               /* the maximum size to which the buffer may grow */
        HFONT font;                     /* NULL means standard system font */
        INT x_offset;                   /* scroll offset        for multi lines this is in pixels
                                                                for single lines it's in characters */
        INT line_height;                /* height of a screen line in pixels */
        INT char_width;         /* average character width in pixels */
        DWORD style;                    /* sane version of wnd->dwStyle */
        WORD flags;                     /* flags that are not in es->style or wnd->flags (EF_XXX) */
        INT undo_insert_count;  /* number of characters inserted in sequence */
        INT undo_position;              /* character index of the insertion and deletion */
        LPSTR undo_text;                /* deleted text */
        INT undo_buffer_size;           /* size of the deleted text buffer */
        INT selection_start;            /* == selection_end if no selection */
        INT selection_end;              /* == current caret position */
        CHAR password_char;             /* == 0 if no password char, and for multi line controls */
        INT left_margin;                /* in pixels */
        INT right_margin;               /* in pixels */
        RECT format_rect;
        INT region_posx;                /* Position of cursor relative to region: */
        INT region_posy;                /* -1: to left, 0: within, 1: to right */
        EDITWORDBREAKPROCA word_break_procA;
        INT line_count;         /* number of lines */
        INT y_offset;                   /* scroll offset in number of lines */
        BOOL bCaptureState; /* flag indicating whether mouse was captured */
        BOOL bEnableState;             /* flag keeping the enable state */
	HWND hwndListBox;              /* handle of ComboBox's listbox or NULL */
        /*
         *      only for multi line controls
         */
        INT lock_count;         /* amount of re-entries in the EditWndProc */
        INT tabs_count;
        LPINT tabs;
        INT text_width;         /* width of the widest line in pixels */
        LINEDEF *first_line_def;        /* linked list of (soft) linebreaks */
        HLOCAL hloc;          /* for controls receiving EM_GETHANDLE */
} EDITSTATE;


#define SWAP_INT32(x,y) do { INT temp = (INT)(x); (x) = (INT)(y); (y) = temp; } while(0)
#define ORDER_INT(x,y) do { if ((INT)(y) < (INT)(x)) SWAP_INT32((x),(y)); } while(0)

#define SWAP_UINT32(x,y) do { UINT temp = (UINT)(x); (x) = (UINT)(y); (y) = temp; } while(0)
#define ORDER_UINT(x,y) do { if ((UINT)(y) < (UINT)(x)) SWAP_UINT32((x),(y)); } while(0)

/* used for disabled or read-only edit control */
#define EDIT_SEND_CTLCOLORSTATIC(hwnd,hdc) \
        (SendMessageA(GetParent(hwnd), WM_CTLCOLORSTATIC, \
                        (WPARAM)(hdc), (LPARAM)hwnd))
#define EDIT_SEND_CTLCOLOR(hwnd,hdc) \
        (SendMessageA(GetParent(hwnd), WM_CTLCOLOREDIT, \
                        (WPARAM)(hdc), (LPARAM)hwnd))
#define EDIT_NOTIFY_PARENT(hwnd, wNotifyCode) \
        (SendMessageA(GetParent(hwnd), WM_COMMAND, \
                     MAKEWPARAM(GetWindowLongA(hwnd,GWL_ID), wNotifyCode), (LPARAM)hwnd))

/*********************************************************************
 *
 *      Declarations
 *
 */

/*
 *      These functions have trivial implementations
 *      We still like to call them internally
 *      "static inline" makes them more like macro's
 */
static inline BOOL      EDIT_EM_CanUndo(HWND hwnd, EDITSTATE *es);
static inline void      EDIT_EM_EmptyUndoBuffer(HWND hwnd, EDITSTATE *es);
static inline void      EDIT_WM_Clear(HWND hwnd, EDITSTATE *es);
static inline void      EDIT_WM_Cut(HWND hwnd, EDITSTATE *es);

/*
 *      Helper functions only valid for one type of control
 */
static void     EDIT_BuildLineDefs_ML(HWND hwnd,EDITSTATE *es);
static LPSTR    EDIT_GetPasswordPointer_SL(HWND hwnd, EDITSTATE *es);
static void     EDIT_MoveDown_ML(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MovePageDown_ML(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MovePageUp_ML(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MoveUp_ML(HWND hwnd, EDITSTATE *es, BOOL extend);
static VOID     EDIT_UpdateScrollBars(HWND hwnd,EDITSTATE *es,BOOL updateHorz,BOOL updateVert);
/*
 *      Helper functions valid for both single line _and_ multi line controls
 */
static INT      EDIT_CallWordBreakProc(HWND hwnd, EDITSTATE *es, INT start, INT index, INT count, INT action);
static INT      EDIT_CharFromPos(HWND hwnd, EDITSTATE *es, INT x, INT y, LPBOOL after_wrap);
static void     EDIT_ConfinePoint(HWND hwnd, EDITSTATE *es, LPINT x, LPINT y);
static void     EDIT_GetLineRect(HWND hwnd,HDC dc,EDITSTATE *es, INT line, INT scol, INT ecol, LPRECT rc);
static void     EDIT_InvalidateText(HWND hwnd, EDITSTATE *es, INT start, INT end);
static void     EDIT_LockBuffer(HWND hwnd, EDITSTATE *es);
static BOOL     EDIT_MakeFit(HWND hwnd, EDITSTATE *es, INT size);
static BOOL     EDIT_MakeUndoFit(HWND hwnd, EDITSTATE *es, INT size);
static void     EDIT_MoveBackward(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MoveEnd(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MoveForward(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MoveHome(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MoveWordBackward(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_MoveWordForward(HWND hwnd, EDITSTATE *es, BOOL extend);
static void     EDIT_PaintLine(HWND hwnd, EDITSTATE *es, HDC hdc, INT line, BOOL rev);
static VOID     EDIT_PaintText(HWND hwnd, EDITSTATE *es, HDC hdc, INT x, INT y, INT line, INT col, INT count, BOOL rev);
static void     EDIT_SetCaretPos(HWND hwnd, EDITSTATE *es, INT pos, BOOL after_wrap);
static void     EDIT_SetRectNP(HWND hwnd, EDITSTATE *es, LPRECT lprc);
static void     EDIT_UnlockBuffer(HWND hwnd, EDITSTATE *es, BOOL force);
static INT      EDIT_WordBreakProc(LPSTR s, INT index, INT count, INT action);
static VOID     EDIT_Draw(HWND hwnd,EDITSTATE *es,HDC hdc);
static VOID     EDIT_Refresh(HWND hwnd,EDITSTATE *es,BOOL useCache);

/*
 *      EM_XXX message handlers
 */
static LRESULT  EDIT_EM_CharFromPos(HWND hwnd, EDITSTATE *es, INT x, INT y);
static BOOL     EDIT_EM_FmtLines(HWND hwnd, EDITSTATE *es, BOOL add_eol);
static INT      EDIT_EM_GetFirstVisibleLine(HWND hwnd,EDITSTATE *es);
static HLOCAL   EDIT_EM_GetHandle(HWND hwnd, EDITSTATE *es);
static INT      EDIT_EM_GetLimitText(HWND hwnd,EDITSTATE *es);
static INT      EDIT_EM_GetLine(HWND hwnd, EDITSTATE *es, INT line, LPSTR lpch);
static INT      EDIT_EM_GetLineCount(HWND hwnd,EDITSTATE *es);
static LONG     EDIT_EM_GetMargins(HWND hwnd,EDITSTATE *es);
static BOOL     EDIT_EM_GetModify(HWND hwnd,EDITSTATE *es);
static CHAR     EDIT_EM_GetPasswordChar(HWND hwnd,EDITSTATE *es);
static VOID     EDIT_EM_GetRect(HWND hwnd,EDITSTATE *es,LPRECT lprc);
static LRESULT  EDIT_EM_GetSel(HWND hwnd, EDITSTATE *es, LPUINT start, LPUINT end);
static LRESULT  EDIT_EM_GetThumb(HWND hwnd, EDITSTATE *es);
static PVOID    EDIT_EM_GetWordbreakProc(HWND hwnd,EDITSTATE *es);
static INT      EDIT_EM_LineFromChar(HWND hwnd, EDITSTATE *es, INT index);
static INT      EDIT_EM_LineIndex(HWND hwnd, EDITSTATE *es, INT line);
static INT      EDIT_EM_LineLength(HWND hwnd, EDITSTATE *es, INT index);
static BOOL     EDIT_EM_LineScroll(HWND hwnd, EDITSTATE *es, INT dx, INT dy);
static LRESULT  EDIT_EM_PosFromChar(HWND hwnd,HDC dc, EDITSTATE *es, INT index, BOOL after_wrap);
static void     EDIT_EM_ReplaceSel(HWND hwnd, EDITSTATE *es, BOOL can_undo, LPCSTR lpsz_replace);
static LRESULT  EDIT_EM_Scroll(HWND hwnd, EDITSTATE *es, INT action);
static void     EDIT_EM_ScrollCaret(HWND hwnd, EDITSTATE *es);
static void     EDIT_EM_SetHandle(HWND hwnd, EDITSTATE *es, HLOCAL hloc);
static void     EDIT_EM_SetLimitText(HWND hwnd, EDITSTATE *es, INT limit);
static void     EDIT_EM_SetMargins(HWND hwnd, EDITSTATE *es, INT action, INT left, INT right);
static void     EDIT_EM_SetModify(HWND hwnd,EDITSTATE *es,BOOL fModified);
static void     EDIT_EM_SetPasswordChar(HWND hwnd, EDITSTATE *es, CHAR c);
static BOOL     EDIT_EM_SetReadOnly(HWND hwnd,EDITSTATE *es,BOOL fReadOnly);
static void     EDIT_EM_SetRect(HWND hwnd,EDITSTATE *es,LPRECT lprc);
static void     EDIT_EM_SetRectNP(HWND hwnd,EDITSTATE *es,LPRECT lprc);
static void     EDIT_EM_SetSel(HWND hwnd, EDITSTATE *es, UINT start, UINT end, BOOL after_wrap);
static BOOL     EDIT_EM_SetTabStops(HWND hwnd, EDITSTATE *es, INT count, LPINT tabs);
static void     EDIT_EM_SetWordBreakProc(HWND hwnd, EDITSTATE *es, EDITWORDBREAKPROCA wbp);
static BOOL     EDIT_EM_Undo(HWND hwnd, EDITSTATE *es);
static LRESULT  EDIT_EM_SetIMEStatus(HWND hwnd,EDITSTATE *es,WPARAM wParam,LPARAM lParam);
static LRESULT  EDIT_EM_GetIMEStatus(HWND hwnd,EDITSTATE *es,WPARAM wParam,LPARAM lParam);
/*
 *      WM_XXX message handlers
 */
static void     EDIT_WM_Char(HWND hwnd, EDITSTATE *es, CHAR c, DWORD key_data);
static void     EDIT_WM_Command(HWND hwnd, EDITSTATE *es, INT code, INT id, HWND conrtol);
static void     EDIT_WM_ContextMenu(HWND hwnd, EDITSTATE *es, HWND hwndBtn, INT x, INT y);
static void     EDIT_WM_Copy(HWND hwnd, EDITSTATE *es);
static LRESULT  EDIT_WM_Create(HWND hwnd, EDITSTATE *es, LPCREATESTRUCTA cs);
static void     EDIT_WM_Destroy(HWND hwnd, EDITSTATE *es);
static LRESULT  EDIT_WM_EraseBkGnd(HWND hwnd, EDITSTATE *es, HDC dc);
static INT      EDIT_WM_GetText(HWND hwnd, EDITSTATE *es, INT count, LPSTR text);
static LRESULT  EDIT_WM_HScroll(HWND hwnd, EDITSTATE *es, INT action, INT pos, HWND scroll_bar);
static LRESULT  EDIT_WM_KeyDown(HWND hwnd, EDITSTATE *es, INT key, DWORD key_data);
static LRESULT  EDIT_WM_KillFocus(HWND hwnd, EDITSTATE *es, HWND window_getting_focus);
static LRESULT  EDIT_WM_LButtonDblClk(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y);
static LRESULT  EDIT_WM_LButtonDown(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y);
static LRESULT  EDIT_WM_LButtonUp(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y);
static LRESULT  EDIT_WM_CaptureChanged(HWND hwnd,EDITSTATE *es);
static LRESULT  EDIT_WM_MouseMove(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y);
static LRESULT  EDIT_WM_NCCreate(HWND hwnd, LPCREATESTRUCTA cs);
static void     EDIT_WM_Paint(HWND hwnd, EDITSTATE *es,WPARAM wParam);
static void     EDIT_WM_Paste(HWND hwnd, EDITSTATE *es);
static void     EDIT_WM_SetFocus(HWND hwnd, EDITSTATE *es, HWND window_losing_focus);
static void     EDIT_WM_SetFont(HWND hwnd, EDITSTATE *es, HFONT font, BOOL redraw);
static void     EDIT_WM_SetText(HWND hwnd, EDITSTATE *es, LPCSTR text);
static void     EDIT_WM_Size(HWND hwnd, EDITSTATE *es, UINT action, INT width, INT height);
static LRESULT  EDIT_WM_SysKeyDown(HWND hwnd, EDITSTATE *es, INT key, DWORD key_data);
static void     EDIT_WM_Timer(HWND hwnd, EDITSTATE *es, INT id, TIMERPROC timer_proc);
static LRESULT  EDIT_WM_VScroll(HWND hwnd, EDITSTATE *es, INT action, INT pos, HWND scroll_bar);
static LRESULT EDIT_WM_MouseWheel(HWND hwnd,EDITSTATE *es,WPARAM wParam,LPARAM lParam);

/*********************************************************************
 *
 *      EM_CANUNDO
 *
 */
static inline BOOL EDIT_EM_CanUndo(HWND hwnd, EDITSTATE *es)
{
        return (es->undo_insert_count || lstrlenA(es->undo_text));
}


/*********************************************************************
 *
 *      EM_EMPTYUNDOBUFFER
 *
 */
static inline void EDIT_EM_EmptyUndoBuffer(HWND hwnd, EDITSTATE *es)
{
        es->undo_insert_count = 0;
        *es->undo_text = '\0';
}


/*********************************************************************
 *
 *      WM_CLEAR
 *
 */
static inline void EDIT_WM_Clear(HWND hwnd, EDITSTATE *es)
{
        EDIT_EM_ReplaceSel(hwnd, es, TRUE, "");
        if (es->flags & EF_UPDATE) {
                es->flags &= ~EF_UPDATE;
                EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
        }
}


/*********************************************************************
 *
 *      WM_CUT
 *
 */
static inline void EDIT_WM_Cut(HWND hwnd, EDITSTATE *es)
{
        EDIT_WM_Copy(hwnd, es);
        EDIT_WM_Clear(hwnd, es);
}


/*********************************************************************
 *
 *      EditWndProc()
 *
 *      The messages are in the order of the actual integer values
 *      (which can be found in include/windows.h)
 *      Whereever possible the 16 bit versions are converted to
 *      the 32 bit ones, so that we can 'fall through' to the
 *      helper functions.  These are mostly 32 bit (with a few
 *      exceptions, clearly indicated by a '16' extension to their
 *      names).
 *
 */
LRESULT WINAPI EditWndProc( HWND hwnd, UINT msg,
                            WPARAM wParam, LPARAM lParam )
{
        EDITSTATE *es = (EDITSTATE*)GetInfoPtr(hwnd);
        LRESULT result = 0;

//      dprintf(("EditWndProc hwnd: %04x, msg %s, wp %08x lp %08lx\n",
//               hwnd, GetMsgText(msg), wParam, lParam));

        switch (msg) {
        case WM_DESTROY:
                //DPRINTF_EDIT_MSG32("WM_DESTROY");
                EDIT_WM_Destroy(hwnd, es);
                result = 0;
                goto END;

        case WM_NCCREATE:
                //DPRINTF_EDIT_MSG32("WM_NCCREATE");
                result = EDIT_WM_NCCreate(hwnd, (LPCREATESTRUCTA)lParam);
                goto END;
        }

        if (!es)
        {
            result = DefWindowProcA(hwnd, msg, wParam, lParam);
            goto END;
        }


        EDIT_LockBuffer(hwnd, es);
        switch (msg) {
        case EM_GETSEL:
                //DPRINTF_EDIT_MSG32("EM_GETSEL");
                result = EDIT_EM_GetSel(hwnd, es, (LPUINT)wParam, (LPUINT)lParam);
                break;

        case EM_SETSEL:
                //DPRINTF_EDIT_MSG32("EM_SETSEL");
                EDIT_EM_SetSel(hwnd, es, wParam, lParam, FALSE);
                EDIT_EM_ScrollCaret(hwnd,es);
                result = 1;
                break;

        case EM_GETRECT:
                //DPRINTF_EDIT_MSG32("EM_GETRECT");
                EDIT_EM_GetRect(hwnd,es,(LPRECT)lParam);
                break;

        case EM_SETRECT:
                //DPRINTF_EDIT_MSG32("EM_SETRECT");
                EDIT_EM_SetRect(hwnd,es,(LPRECT)lParam);
                break;

        case EM_SETRECTNP:
                //DPRINTF_EDIT_MSG32("EM_SETRECTNP");
                EDIT_EM_SetRectNP(hwnd,es,(LPRECT)lParam);
                break;

        case EM_SCROLL:
                //DPRINTF_EDIT_MSG32("EM_SCROLL");
                result = EDIT_EM_Scroll(hwnd, es, (INT)wParam);
                break;

        case EM_LINESCROLL:
                //DPRINTF_EDIT_MSG32("EM_LINESCROLL");
                result = (LRESULT)EDIT_EM_LineScroll(hwnd, es, (INT)wParam, (INT)lParam);
                break;

        case EM_SCROLLCARET:
                //DPRINTF_EDIT_MSG32("EM_SCROLLCARET");
                EDIT_EM_ScrollCaret(hwnd, es);
                result = 1;
                break;

        case EM_GETMODIFY:
                //DPRINTF_EDIT_MSG32("EM_GETMODIFY");
                result = (LRESULT)EDIT_EM_GetModify(hwnd,es);
                break;

        case EM_SETMODIFY:
                //DPRINTF_EDIT_MSG32("EM_SETMODIFY");
                EDIT_EM_SetModify(hwnd,es,(BOOL)wParam);
                break;

        case EM_GETLINECOUNT:
                //DPRINTF_EDIT_MSG32("EM_GETLINECOUNT");
                result = (LRESULT)EDIT_EM_GetLineCount(hwnd,es);
                break;

        case EM_LINEINDEX:
                //DPRINTF_EDIT_MSG32("EM_LINEINDEX");
                result = (LRESULT)EDIT_EM_LineIndex(hwnd, es, (INT)wParam);
                break;

        case EM_SETHANDLE:
                //DPRINTF_EDIT_MSG32("EM_SETHANDLE");
                EDIT_EM_SetHandle(hwnd, es, (HLOCAL)wParam);
                break;

        case EM_GETHANDLE:
                //DPRINTF_EDIT_MSG32("EM_GETHANDLE");
                result = (LRESULT)EDIT_EM_GetHandle(hwnd, es);
                break;

        case EM_GETTHUMB:
                //DPRINTF_EDIT_MSG32("EM_GETTHUMB");
                result = EDIT_EM_GetThumb(hwnd, es);
                break;

        /* messages 0x00bf and 0x00c0 missing from specs */

        case WM_USER+15:
                //DPRINTF_EDIT_MSG16("undocumented WM_USER+15, please report");
                /* fall through */
        case 0x00bf:
                //DPRINTF_EDIT_MSG32("undocumented 0x00bf, please report");
                result = DefWindowProcA(hwnd, msg, wParam, lParam);
                break;

        case WM_USER+16:
                //DPRINTF_EDIT_MSG16("undocumented WM_USER+16, please report");
                /* fall through */
        case 0x00c0:
                //DPRINTF_EDIT_MSG32("undocumented 0x00c0, please report");
                result = DefWindowProcA(hwnd, msg, wParam, lParam);
                break;

        case EM_LINELENGTH:
                //DPRINTF_EDIT_MSG32("EM_LINELENGTH");
                result = (LRESULT)EDIT_EM_LineLength(hwnd, es, (INT)wParam);
                break;

        case EM_REPLACESEL:
                //DPRINTF_EDIT_MSG32("EM_REPLACESEL");
                EDIT_EM_ReplaceSel(hwnd, es, (BOOL)wParam, (LPCSTR)lParam);
                result = 1;
                break;

        /* message 0x00c3 missing from specs */

        case WM_USER+19:
                //DPRINTF_EDIT_MSG16("undocumented WM_USER+19, please report");
                /* fall through */
        case 0x00c3:
                //DPRINTF_EDIT_MSG32("undocumented 0x00c3, please report");
                result = DefWindowProcA(hwnd, msg, wParam, lParam);
                break;

        case EM_GETLINE:
                //DPRINTF_EDIT_MSG32("EM_GETLINE");
                result = (LRESULT)EDIT_EM_GetLine(hwnd, es, (INT)wParam, (LPSTR)lParam);
                break;

        case EM_SETLIMITTEXT:
                //DPRINTF_EDIT_MSG32("EM_SETLIMITTEXT");
                EDIT_EM_SetLimitText(hwnd, es, (INT)wParam);
                break;

        case EM_CANUNDO:
                //DPRINTF_EDIT_MSG32("EM_CANUNDO");
                result = (LRESULT)EDIT_EM_CanUndo(hwnd, es);
                break;

        case EM_UNDO:
                /* fall through */
        case WM_UNDO:
                //DPRINTF_EDIT_MSG32("EM_UNDO / WM_UNDO");
                result = (LRESULT)EDIT_EM_Undo(hwnd, es);
                break;

        case EM_FMTLINES:
                //DPRINTF_EDIT_MSG32("EM_FMTLINES");
                result = (LRESULT)EDIT_EM_FmtLines(hwnd, es, (BOOL)wParam);
                break;

        case EM_LINEFROMCHAR:
                //DPRINTF_EDIT_MSG32("EM_LINEFROMCHAR");
                result = (LRESULT)EDIT_EM_LineFromChar(hwnd, es, (INT)wParam);
                break;

        /* message 0x00ca missing from specs */

        case WM_USER+26:
                //DPRINTF_EDIT_MSG16("undocumented WM_USER+26, please report");
                /* fall through */
        case 0x00ca:
                //DPRINTF_EDIT_MSG32("undocumented 0x00ca, please report");
                result = DefWindowProcA(hwnd, msg, wParam, lParam);
                break;

        case EM_SETTABSTOPS:
                //DPRINTF_EDIT_MSG32("EM_SETTABSTOPS");
                result = (LRESULT)EDIT_EM_SetTabStops(hwnd, es, (INT)wParam, (LPINT)lParam);
                break;

        case EM_SETPASSWORDCHAR:
                //DPRINTF_EDIT_MSG32("EM_SETPASSWORDCHAR");
                EDIT_EM_SetPasswordChar(hwnd, es, (CHAR)wParam);
                break;

        case EM_EMPTYUNDOBUFFER:
                //DPRINTF_EDIT_MSG32("EM_EMPTYUNDOBUFFER");
                EDIT_EM_EmptyUndoBuffer(hwnd, es);
                break;

        case EM_GETFIRSTVISIBLELINE:
                //DPRINTF_EDIT_MSG32("EM_GETFIRSTVISIBLELINE");
                result = (LRESULT)EDIT_EM_GetFirstVisibleLine(hwnd,es);
                break;

        case EM_SETREADONLY:
                //DPRINTF_EDIT_MSG32("EM_SETREADONLY");
                result = (LRESULT)EDIT_EM_SetReadOnly(hwnd,es,(BOOL)wParam);
                break;

        case EM_SETWORDBREAKPROC:
                //DPRINTF_EDIT_MSG32("EM_SETWORDBREAKPROC");
                EDIT_EM_SetWordBreakProc(hwnd, es, (EDITWORDBREAKPROCA)lParam);
                break;

        case EM_GETWORDBREAKPROC:
                //DPRINTF_EDIT_MSG32("EM_GETWORDBREAKPROC");
                result = (LRESULT)EDIT_EM_GetWordbreakProc(hwnd,es);
                break;

        case EM_GETPASSWORDCHAR:
                //DPRINTF_EDIT_MSG32("EM_GETPASSWORDCHAR");
                result = (LRESULT)EDIT_EM_GetPasswordChar(hwnd,es);
                break;

        /* The following EM_xxx are new to win95 and don't exist for 16 bit */

        case EM_SETMARGINS:
                //DPRINTF_EDIT_MSG32("EM_SETMARGINS");
                EDIT_EM_SetMargins(hwnd, es, (INT)wParam, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case EM_GETMARGINS:
                //DPRINTF_EDIT_MSG32("EM_GETMARGINS");
                result = EDIT_EM_GetMargins(hwnd,es);
                break;

        case EM_GETLIMITTEXT:
                //DPRINTF_EDIT_MSG32("EM_GETLIMITTEXT");
                result = (LRESULT)EDIT_EM_GetLimitText(hwnd,es);
                break;

        case EM_POSFROMCHAR:
                //DPRINTF_EDIT_MSG32("EM_POSFROMCHAR");
                result = EDIT_EM_PosFromChar(hwnd,0, es, (INT)wParam, FALSE);
                break;

        case EM_CHARFROMPOS:
                //DPRINTF_EDIT_MSG32("EM_CHARFROMPOS");
                result = EDIT_EM_CharFromPos(hwnd, es, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case EM_SETIMESTATUS:
                result = EDIT_EM_SetIMEStatus(hwnd,es,wParam,lParam);
                break;

        case EM_GETIMESTATUS:
                result = EDIT_EM_GetIMEStatus(hwnd,es,wParam,lParam);
                break;

        case WM_GETDLGCODE:
                //DPRINTF_EDIT_MSG32("WM_GETDLGCODE");
                result = DLGC_HASSETSEL | DLGC_WANTCHARS | DLGC_WANTARROWS;

       		if (lParam && (((LPMSG)lParam)->message == WM_KEYDOWN))
		{
		   int vk = (int)((LPMSG)lParam)->wParam;

		   if ((GetWindowLongA(hwnd,GWL_STYLE) & ES_WANTRETURN) && vk == VK_RETURN)
		   {
		      result |= DLGC_WANTMESSAGE;
		   }
		   else if (es->hwndListBox && (vk == VK_RETURN || vk == VK_ESCAPE))
		   {
		      if (SendMessageA(GetParent(hwnd), CB_GETDROPPEDSTATE, 0, 0))
		         result |= DLGC_WANTMESSAGE;
		   }
		}
		break;

        case WM_CHAR:
                //DPRINTF_EDIT_MSG32("WM_CHAR");
		if (((CHAR)wParam == VK_RETURN || (CHAR)wParam == VK_ESCAPE) && es->hwndListBox)
		{
		   HWND hwndParent = GetParent(hwnd);
		
		   if (SendMessageA(hwndParent, CB_GETDROPPEDSTATE, 0, 0))
		      SendMessageA(hwndParent, WM_KEYDOWN, wParam, 0);
		   break;
		}
                EDIT_WM_Char(hwnd, es, (CHAR)wParam, (DWORD)lParam);
                break;

        case WM_CLEAR:
                //DPRINTF_EDIT_MSG32("WM_CLEAR");
                EDIT_WM_Clear(hwnd, es);
                break;

        case WM_COMMAND:
                //DPRINTF_EDIT_MSG32("WM_COMMAND");
                EDIT_WM_Command(hwnd, es, HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
                break;

        case WM_CONTEXTMENU:
                //DPRINTF_EDIT_MSG32("WM_CONTEXTMENU");
                EDIT_WM_ContextMenu(hwnd, es, (HWND)wParam, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case WM_COPY:
                //DPRINTF_EDIT_MSG32("WM_COPY");
                EDIT_WM_Copy(hwnd, es);
                break;

        case WM_CREATE:
                //DPRINTF_EDIT_MSG32("WM_CREATE");
                result = EDIT_WM_Create(hwnd, es, (LPCREATESTRUCTA)lParam);
                break;

        case WM_CUT:
                //DPRINTF_EDIT_MSG32("WM_CUT");
                EDIT_WM_Cut(hwnd, es);
                break;

        case WM_ENABLE:
                //DPRINTF_EDIT_MSG32("WM_ENABLE");
                es->bEnableState = (BOOL)wParam;
                EDIT_Refresh(hwnd,es,FALSE);
                break;

        case WM_ERASEBKGND:
                //DPRINTF_EDIT_MSG32("WM_ERASEBKGND");
                result = EDIT_WM_EraseBkGnd(hwnd, es, (HDC)wParam);
                break;

        case WM_GETFONT:
                //DPRINTF_EDIT_MSG32("WM_GETFONT");
                result = (LRESULT)es->font;
                break;

        case WM_GETTEXT:
                //DPRINTF_EDIT_MSG32("WM_GETTEXT");
                result = (LRESULT)EDIT_WM_GetText(hwnd, es, (INT)wParam, (LPSTR)lParam);
                break;

        case WM_GETTEXTLENGTH:
                //DPRINTF_EDIT_MSG32("WM_GETTEXTLENGTH");
                result = lstrlenA(es->text);
                break;

        case WM_HSCROLL:
                //DPRINTF_EDIT_MSG32("WM_HSCROLL");
                result = EDIT_WM_HScroll(hwnd, es, LOWORD(wParam), SHIWORD(wParam), (HWND)lParam);
                break;

        case WM_KEYDOWN:
                //DPRINTF_EDIT_MSG32("WM_KEYDOWN");
                result = EDIT_WM_KeyDown(hwnd, es, (INT)wParam, (DWORD)lParam);
                break;

        case WM_KILLFOCUS:
                //DPRINTF_EDIT_MSG32("WM_KILLFOCUS");
                result = EDIT_WM_KillFocus(hwnd, es, (HWND)wParam);
                break;

        case WM_LBUTTONDBLCLK:
                //DPRINTF_EDIT_MSG32("WM_LBUTTONDBLCLK");
                result = EDIT_WM_LButtonDblClk(hwnd, es, (DWORD)wParam, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case WM_LBUTTONDOWN:
                //DPRINTF_EDIT_MSG32("WM_LBUTTONDOWN");
                result = EDIT_WM_LButtonDown(hwnd, es, (DWORD)wParam, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case WM_LBUTTONUP:
                //DPRINTF_EDIT_MSG32("WM_LBUTTONUP");
                result = EDIT_WM_LButtonUp(hwnd, es, (DWORD)wParam, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case WM_CAPTURECHANGED:
                result = EDIT_WM_CaptureChanged(hwnd,es);
                break;

        case WM_MOUSEACTIVATE:
                /*
                 *      FIXME: maybe DefWindowProc() screws up, but it seems that
                 *              modalless dialog boxes need this.  If we don't do this, the focus
                 *              will _not_ be set by DefWindowProc() for edit controls in a
                 *              modalless dialog box ???
                 */
                //DPRINTF_EDIT_MSG32("WM_MOUSEACTIVATE");
                result = MA_ACTIVATE;
                break;

        case WM_MOUSEMOVE:
                /*
                 *      DPRINTF_EDIT_MSG32("WM_MOUSEMOVE");
                 */
                result = EDIT_WM_MouseMove(hwnd, es, (DWORD)wParam, SLOWORD(lParam), SHIWORD(lParam));
                break;

        case WM_PAINT:
                //DPRINTF_EDIT_MSG32("WM_PAINT");
                EDIT_WM_Paint(hwnd, es,wParam);
                break;

        case WM_PASTE:
                //DPRINTF_EDIT_MSG32("WM_PASTE");
                EDIT_WM_Paste(hwnd, es);
                break;

        case WM_SETFOCUS:
                //DPRINTF_EDIT_MSG32("WM_SETFOCUS");
                EDIT_WM_SetFocus(hwnd, es, (HWND)wParam);
                break;

        case WM_SETFONT:
                //DPRINTF_EDIT_MSG32("WM_SETFONT");
                EDIT_WM_SetFont(hwnd, es, (HFONT)wParam, LOWORD(lParam) != 0);
                break;

        case WM_SETTEXT:
                //DPRINTF_EDIT_MSG32("WM_SETTEXT");
                EDIT_WM_SetText(hwnd, es, (LPCSTR)lParam);
                result = TRUE;
                break;

        case WM_SIZE:
                //DPRINTF_EDIT_MSG32("WM_SIZE");
                EDIT_WM_Size(hwnd, es, (UINT)wParam, LOWORD(lParam), HIWORD(lParam));
                break;

        case WM_SYSKEYDOWN:
                //DPRINTF_EDIT_MSG32("WM_SYSKEYDOWN");
                result = EDIT_WM_SysKeyDown(hwnd, es, (INT)wParam, (DWORD)lParam);
                break;

        case WM_TIMER:
                //DPRINTF_EDIT_MSG32("WM_TIMER");
                EDIT_WM_Timer(hwnd, es, (INT)wParam, (TIMERPROC)lParam);
                break;

        case WM_VSCROLL:
                //DPRINTF_EDIT_MSG32("WM_VSCROLL");
                result = EDIT_WM_VScroll(hwnd, es, LOWORD(wParam), SHIWORD(wParam), (HWND)(lParam));
                break;

        case WM_MOUSEWHEEL:
                result = EDIT_WM_MouseWheel(hwnd,es,wParam,lParam);
                break;

        default:
                result = DefWindowProcA(hwnd, msg, wParam, lParam);
                break;
        }
        EDIT_UnlockBuffer(hwnd, es, FALSE);
    END:
        return result;

}


/*********************************************************************
 *
 *      EDIT_BuildLineDefs_ML
 *
 *      Build linked list of text lines.
 *      Lines can end with '\0' (last line), a character (if it is wrapped),
 *      a soft return '\r\r\n' or a hard return '\r\n'
 *
 */
static void EDIT_BuildLineDefs_ML(HWND hwnd,EDITSTATE *es)
{
        HDC hdc = 0;
        HFONT old_font = 0;
        LPSTR start, cp;
        INT fw;
        LINEDEF *current_def;
        LINEDEF **previous_next;

        current_def = es->first_line_def;
        do {
                LINEDEF *next_def = current_def->next;
                HeapFree(es->heap, 0, current_def);
                current_def = next_def;
        } while (current_def);
        es->line_count = 0;
        es->text_width = 0;

        fw = es->format_rect.right - es->format_rect.left;
        start = es->text;
        previous_next = &es->first_line_def;
        do {
                current_def = (LINEDEF*)HeapAlloc(es->heap, 0, sizeof(LINEDEF));
                current_def->next = NULL;
                cp = start;
                while (*cp) {
                        if ((*cp == '\r') && (*(cp + 1) == '\n'))
                                break;
                        cp++;
                }
                if (!(*cp)) {
                        current_def->ending = END_0;
                        current_def->net_length = lstrlenA(start);
                } else if ((cp > start) && (*(cp - 1) == '\r')) {
                        current_def->ending = END_SOFT;
                        current_def->net_length = cp - start - 1;
                } else {
                        current_def->ending = END_HARD;
                        current_def->net_length = cp - start;
                }

                if (!hdc)
                {
                  hdc = GetDC(hwnd);
                  if (es->font)
                    old_font = SelectObject(hdc, es->font);
                }

                current_def->width = (INT)LOWORD(GetTabbedTextExtentA(hdc,
                                        start, current_def->net_length,
                                        es->tabs_count, es->tabs));
                /* FIXME: check here for lines that are too wide even in AUTOHSCROLL (> 32767 ???) */
                if ((!(es->style & ES_AUTOHSCROLL)) && (current_def->width > fw)) {
                        INT next = 0;
                        INT prev;
                        do {
                                prev = next;
                                next = EDIT_CallWordBreakProc(hwnd, es, start - es->text,
                                                prev + 1, current_def->net_length, WB_RIGHT);
                                current_def->width = (INT)LOWORD(GetTabbedTextExtentA(hdc,
                                                        start, next, es->tabs_count, es->tabs));
                        } while (current_def->width <= fw);
                        if (!prev) {
                                next = 0;
                                do {
                                        prev = next;
                                        next++;
                                        current_def->width = (INT)LOWORD(GetTabbedTextExtentA(hdc,
                                                                start, next, es->tabs_count, es->tabs));
                                } while (current_def->width <= fw);
                                if (!prev)
                                        prev = 1;
                        }
                        current_def->net_length = prev;
                        current_def->ending = END_WRAP;
                        current_def->width = (INT)LOWORD(GetTabbedTextExtentA(hdc, start,
                                                current_def->net_length, es->tabs_count, es->tabs));
                }
                switch (current_def->ending) {
                case END_SOFT:
                        current_def->length = current_def->net_length + 3;
                        break;
                case END_HARD:
                        current_def->length = current_def->net_length + 2;
                        break;
                case END_WRAP:
                case END_0:
                        current_def->length = current_def->net_length;
                        break;
                }
                es->text_width = MAX(es->text_width, current_def->width);
                start += current_def->length;
                *previous_next = current_def;
                previous_next = &current_def->next;
                es->line_count++;
        } while (current_def->ending != END_0);
        if (hdc)
        {
          if (es->font)
                  SelectObject(hdc, old_font);
          ReleaseDC(hwnd, hdc);
        }
        EDIT_UpdateScrollBars(hwnd,es,TRUE,TRUE);
}


/*********************************************************************
 *
 *      EDIT_CallWordBreakProc
 *
 *      Call appropriate WordBreakProc (internal or external).
 *
 *      Note: The "start" argument should always be an index refering
 *              to es->text.  The actual wordbreak proc might be
 *              16 bit, so we can't always pass any 32 bit LPSTR.
 *              Hence we assume that es->text is the buffer that holds
 *              the string under examination (we can decide this for ourselves).
 *
 */
static INT EDIT_CallWordBreakProc(HWND hwnd, EDITSTATE *es, INT start, INT index, INT count, INT action)
{
        if (es->word_break_procA)
        {
            //TRACE_(relay)("(wordbrk=%p,str='%s',idx=%d,cnt=%d,act=%d)\n",
            //               es->word_break_proc32A, es->text + start, index,
            //               count, action );
            return (INT)es->word_break_procA( es->text + start, index,
                                                  count, action );
        }
        else
            return EDIT_WordBreakProc(es->text + start, index, count, action);
}


/*********************************************************************
 *
 *      EDIT_CharFromPos
 *
 *      Beware: This is not the function called on EM_CHARFROMPOS
 *              The position _can_ be outside the formatting / client
 *              rectangle
 *              The return value is only the character index
 *
 */
static INT EDIT_CharFromPos(HWND hwnd, EDITSTATE *es, INT x, INT y, LPBOOL after_wrap)
{
        INT index;
        HDC dc = 0;
        HFONT old_font = 0;

        if (es->style & ES_MULTILINE) {
                INT line = (y - es->format_rect.top) / es->line_height + es->y_offset;
                INT line_index = 0;
                LINEDEF *line_def = es->first_line_def;
                INT low, high;
                while ((line > 0) && line_def->next) {
                        line_index += line_def->length;
                        line_def = line_def->next;
                        line--;
                }
                x += es->x_offset - es->format_rect.left;
                if (x >= line_def->width) {
                        if (after_wrap)
                                *after_wrap = (line_def->ending == END_WRAP);
                        return line_index + line_def->net_length;
                }
                if (x <= 0) {
                        if (after_wrap)
                                *after_wrap = FALSE;
                        return line_index;
                }
                low = line_index + 1;
                high = line_index + line_def->net_length + 1;
                if (low < high-1)
                {
                  if (!dc)
                  {
                    dc = GetDC(hwnd);
                    if (es->font)
                      old_font = SelectObject(dc, es->font);
                  }

                  while (low < high-1)
                  {
                    INT mid = (low + high) / 2;
                    if (LOWORD(GetTabbedTextExtentA(dc, es->text + line_index,mid - line_index, es->tabs_count, es->tabs)) > x) high = mid;
                    else low = mid;
                  }
                }
                index = low;

                if (after_wrap)
                        *after_wrap = ((index == line_index + line_def->net_length) &&
                                                        (line_def->ending == END_WRAP));
        } else {
                LPSTR text;
                SIZE size;
                if (after_wrap)
                        *after_wrap = FALSE;
                x -= es->format_rect.left;
                if (!x)
                        return es->x_offset;
                text = EDIT_GetPasswordPointer_SL(hwnd, es);
                if (x < 0)
                {
                  INT low = 0;
                  INT high = es->x_offset;

                  if (low < high-1)
                  {
                    if (!dc)
                    {
                      dc = GetDC(hwnd);
                      if (es->font)
                        old_font = SelectObject(dc, es->font);
                    }

                    while (low < high-1)
                    {
                      INT mid = (low + high) / 2;

                      GetTextExtentPoint32A( dc, text + mid,
                                             es->x_offset - mid, &size );
                      if (size.cx > -x) low = mid;
                      else high = mid;
                    }
                  }
                  index = low;
                } else
                {
                  INT low = es->x_offset;
                  INT high = lstrlenA(es->text) + 1;

                  if (low < high-1)
                  {
                    if (!dc)
                    {
                      dc = GetDC(hwnd);
                      if (es->font)
                        old_font = SelectObject(dc, es->font);
                    }

                    while (low < high-1)
                    {
                      INT mid = (low + high) / 2;

                      GetTextExtentPoint32A( dc, text + es->x_offset,
                                             mid - es->x_offset, &size );
                      if (size.cx > x) high = mid;
                      else low = mid;
                    }
                  }
                  index = low;
                }
                if (es->style & ES_PASSWORD)
                        HeapFree(es->heap, 0 ,text);
        }
        if (dc)
        {
          if (es->font)
            SelectObject(dc, old_font);
          ReleaseDC(hwnd, dc);
        }
        return index;
}


/*********************************************************************
 *
 *      EDIT_ConfinePoint
 *
 *      adjusts the point to be within the formatting rectangle
 *      (so CharFromPos returns the nearest _visible_ character)
 *
 */
static void EDIT_ConfinePoint(HWND hwnd, EDITSTATE *es, LPINT x, LPINT y)
{
        *x = MIN(MAX(*x, es->format_rect.left), es->format_rect.right - 1);
        *y = MIN(MAX(*y, es->format_rect.top), es->format_rect.bottom - 1);
}


/*********************************************************************
 *
 *      EDIT_GetLineRect
 *
 *      Calculates the bounding rectangle for a line from a starting
 *      column to an ending column.
 *
 */
static void EDIT_GetLineRect(HWND hwnd,HDC dc,EDITSTATE *es, INT line, INT scol, INT ecol, LPRECT rc)
{
        INT line_index =  EDIT_EM_LineIndex(hwnd, es, line);

        if (es->style & ES_MULTILINE)
                rc->top = es->format_rect.top + (line - es->y_offset) * es->line_height;
        else
                rc->top = es->format_rect.top;
        rc->bottom = rc->top + es->line_height;

        rc->left = (scol == 0) ? es->format_rect.left : SLOWORD(EDIT_EM_PosFromChar(hwnd,dc, es, line_index + scol, TRUE));
        rc->right = (ecol == -1) ? es->format_rect.right : SLOWORD(EDIT_EM_PosFromChar(hwnd,dc, es, line_index + ecol, TRUE))+1;
}


/*********************************************************************
 *
 *      EDIT_GetPasswordPointer_SL
 *
 *      note: caller should free the (optionally) allocated buffer
 *
 */
static LPSTR EDIT_GetPasswordPointer_SL(HWND hwnd, EDITSTATE *es)
{
        if (es->style & ES_PASSWORD) {
                INT len = lstrlenA(es->text);
                LPSTR text = (LPSTR)HeapAlloc(es->heap, 0, len + 1);
                RtlFillMemory(text, len, es->password_char);
                text[len] = '\0';
                return text;
        } else
                return es->text;
}


/*********************************************************************
 *
 *      EDIT_LockBuffer
 *
 *      This acts as a LOCAL_Lock(), but it locks only once.  This way
 *      you can call it whenever you like, without unlocking.
 *
 */
static void EDIT_LockBuffer(HWND hwnd, EDITSTATE *es)
{
        if (!es) {
                //ERR_(edit)("no EDITSTATE ... please report\n");
                return;
        }
        if (!(es->style & ES_MULTILINE))
                return;
        if (!es->text) {
                if (es->hloc)
                        es->text = (char*)LocalLock(es->hloc);
                else {
                        //ERR_(edit)("no buffer ... please report\n");
                        return;
                }
        }
        es->lock_count++;
}


/*********************************************************************
 *
 *      EDIT_SL_InvalidateText
 *
 *      Called from EDIT_InvalidateText().
 *      Does the job for single-line controls only.
 *
 */
static void EDIT_SL_InvalidateText(HWND hwnd, EDITSTATE *es, INT start, INT end)
{
        RECT line_rect;
        RECT rc;

        EDIT_GetLineRect(hwnd,0, es, 0, start, end, &line_rect);

        if (IntersectRect(&rc, &line_rect, &es->format_rect))
        {
          HideCaret(hwnd);
          InvalidateRect(hwnd, &rc, TRUE);
          ShowCaret(hwnd);
        }
}


/*********************************************************************
 *
 *      EDIT_ML_InvalidateText
 *
 *      Called from EDIT_InvalidateText().
 *      Does the job for multi-line controls only.
 *
 */
static void EDIT_ML_InvalidateText(HWND hwnd, EDITSTATE *es, INT start, INT end)
{
        INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
        INT sl = EDIT_EM_LineFromChar(hwnd, es, start);
        INT el = EDIT_EM_LineFromChar(hwnd, es, end);
        INT sc;
        INT ec;
        RECT rc1;
        RECT rcWnd;
        RECT rcLine;
        RECT rcUpdate;
        INT l;

        if ((el < es->y_offset) || (sl > es->y_offset + vlc))
                return;

        sc = start - EDIT_EM_LineIndex(hwnd, es, sl);
        ec = end - EDIT_EM_LineIndex(hwnd, es, el);
        if (sl < es->y_offset) {
                sl = es->y_offset;
                sc = 0;
        }
        if (el > es->y_offset + vlc) {
                el = es->y_offset + vlc;
                ec = EDIT_EM_LineLength(hwnd, es, EDIT_EM_LineIndex(hwnd, es, el));
        }
        GetClientRect(hwnd, &rc1);
        IntersectRect(&rcWnd, &rc1, &es->format_rect);
        HideCaret(hwnd);
        if (sl == el) {
                EDIT_GetLineRect(hwnd,0, es, sl, sc, ec, &rcLine);

                if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
                        InvalidateRect(hwnd, &rcUpdate, TRUE);
        } else {
                EDIT_GetLineRect(hwnd,0, es, sl, sc,
                                EDIT_EM_LineLength(hwnd, es,
                                        EDIT_EM_LineIndex(hwnd, es, sl)),
                                &rcLine);
                if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
                        InvalidateRect(hwnd, &rcUpdate, TRUE);
                for (l = sl + 1 ; l < el ; l++) {
                        EDIT_GetLineRect(hwnd,0, es, l, 0,
                                EDIT_EM_LineLength(hwnd, es,
                                        EDIT_EM_LineIndex(hwnd, es, l)),
                                &rcLine);
                        if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
                                InvalidateRect(hwnd, &rcUpdate, TRUE);
                }
                EDIT_GetLineRect(hwnd,0, es, el, 0, ec, &rcLine);
                if (IntersectRect(&rcUpdate, &rcWnd, &rcLine))
                        InvalidateRect(hwnd, &rcUpdate, TRUE);
        }
        ShowCaret(hwnd);
}


/*********************************************************************
 *
 *      EDIT_InvalidateText
 *
 *      Invalidate the text from offset start upto, but not including,
 *      offset end.  Useful for (re)painting the selection.
 *      Regions outside the linewidth are not invalidated.
 *      end == -1 means end == TextLength.
 *      start and end need not be ordered.
 *
 */
static void EDIT_InvalidateText(HWND hwnd, EDITSTATE *es, INT start, INT end)
{
        if (end == start)
                return;

        if (end == -1)
                end = lstrlenA(es->text);

        ORDER_INT(start, end);

        if (es->style & ES_MULTILINE)
                EDIT_ML_InvalidateText(hwnd, es, start, end);
        else
                EDIT_SL_InvalidateText(hwnd, es, start, end);
}


/*********************************************************************
 *
 *      EDIT_MakeFit
 *
 *      Try to fit size + 1 bytes in the buffer.  Constrain to limits.
 *
 */
static BOOL EDIT_MakeFit(HWND hwnd, EDITSTATE *es, INT size)
{
        HLOCAL hNew32;

#ifndef __WIN32OS2__
        if (size > es->buffer_limit) {
                EDIT_NOTIFY_PARENT(hwnd, EN_MAXTEXT);
                return FALSE;
        }
#endif

        if (size <= es->buffer_size)
                return TRUE;
        size = ((size / GROWLENGTH) + 1) * GROWLENGTH;

#ifndef __WIN32OS2__
        if (size > es->buffer_limit)
                size = es->buffer_limit;
#endif

        //TRACE_(edit)("trying to ReAlloc to %d+1\n", size);

        EDIT_UnlockBuffer(hwnd, es, TRUE);
        if (es->text) {
                es->text = (char*)HeapReAlloc(es->heap, 0, es->text, size + 1);
                if (es->text)
#ifdef __WIN32OS2__
                        es->buffer_size = HeapSize(es->heap, 0, es->text) - 1;
#else
                        es->buffer_size = MIN(HeapSize(es->heap, 0, es->text) - 1, es->buffer_limit);
#endif
                else
                        es->buffer_size = 0;
        } else if (es->hloc) {
                hNew32 = LocalReAlloc(es->hloc, size + 1, 0);
                if (hNew32) {
                        //TRACE_(edit)("Old 32 bit handle %08x, new handle %08x\n", es->hloc32, hNew32);
                        es->hloc = hNew32;
#ifdef __WIN32OS2__
                        es->buffer_size = LocalSize(es->hloc) - 1;
#else
                        es->buffer_size = MIN(LocalSize(es->hloc) - 1, es->buffer_limit);
#endif
                }
        }
        if (es->buffer_size < size) {
                EDIT_LockBuffer(hwnd, es);
                //WARN_(edit)("FAILED !  We now have %d+1\n", es->buffer_size);
                EDIT_NOTIFY_PARENT(hwnd, EN_ERRSPACE);
                return FALSE;
        } else {
                EDIT_LockBuffer(hwnd, es);
                //TRACE_(edit)("We now have %d+1\n", es->buffer_size);
                return TRUE;
        }
}


/*********************************************************************
 *
 *      EDIT_MakeUndoFit
 *
 *      Try to fit size + 1 bytes in the undo buffer.
 *
 */
static BOOL EDIT_MakeUndoFit(HWND hwnd, EDITSTATE *es, INT size)
{
        if (size <= es->undo_buffer_size)
                return TRUE;
        size = ((size / GROWLENGTH) + 1) * GROWLENGTH;

        //TRACE_(edit)("trying to ReAlloc to %d+1\n", size);
        es->undo_text = (char*)HeapReAlloc(es->heap, 0, es->undo_text, size + 1);
        if (es->undo_text) {
                es->undo_buffer_size = HeapSize(es->heap, 0, es->undo_text) - 1;
                if (es->undo_buffer_size < size) {
                        //WARN_(edit)("FAILED !  We now have %d+1\n", es->undo_buffer_size);
                        return FALSE;
                }
                return TRUE;
        }
        return FALSE;
}


/*********************************************************************
 *
 *      EDIT_MoveBackward
 *
 */
static void EDIT_MoveBackward(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT e = es->selection_end;

        if (e) {
                e--;
                if ((es->style & ES_MULTILINE) && e &&
                                (es->text[e - 1] == '\r') && (es->text[e] == '\n')) {
                        e--;
                        if (e && (es->text[e - 1] == '\r'))
                                e--;
                }
        }
        EDIT_EM_SetSel(hwnd, es, extend ? es->selection_start : e, e, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveDown_ML
 *
 *      Only for multi line controls
 *      Move the caret one line down, on a column with the nearest
 *      x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MoveDown_ML(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
        LRESULT pos = EDIT_EM_PosFromChar(hwnd,0, es, e, after_wrap);
        INT x = SLOWORD(pos);
        INT y = SHIWORD(pos);

        e = EDIT_CharFromPos(hwnd, es, x, y + es->line_height, &after_wrap);
        if (!extend)
                s = e;
        EDIT_EM_SetSel(hwnd, es, s, e, after_wrap);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveEnd
 *
 */
static void EDIT_MoveEnd(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        BOOL after_wrap = FALSE;
        INT e;

        /* Pass a high value in x to make sure of receiving the en of the line */
        if (es->style & ES_MULTILINE)
                e = EDIT_CharFromPos(hwnd, es, 0x3fffffff,
                        HIWORD(EDIT_EM_PosFromChar(hwnd,0, es, es->selection_end, es->flags & EF_AFTER_WRAP)), &after_wrap);
        else
                e = lstrlenA(es->text);
        EDIT_EM_SetSel(hwnd, es, extend ? es->selection_start : e, e, after_wrap);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveForward
 *
 */
static void EDIT_MoveForward(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT e = es->selection_end;

        if (es->text[e]) {
                e++;
                if ((es->style & ES_MULTILINE) && (es->text[e - 1] == '\r')) {
                        if (es->text[e] == '\n')
                                e++;
                        else if ((es->text[e] == '\r') && (es->text[e + 1] == '\n'))
                                e += 2;
                }
        }
        EDIT_EM_SetSel(hwnd, es, extend ? es->selection_start : e, e, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveHome
 *
 *      Home key: move to beginning of line.
 *
 */
static void EDIT_MoveHome(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT e;

        /* Pass the x_offset in x to make sure of receiving the first position of the line */
        if (es->style & ES_MULTILINE)
                e = EDIT_CharFromPos(hwnd, es, -es->x_offset,
                        HIWORD(EDIT_EM_PosFromChar(hwnd,0, es, es->selection_end, es->flags & EF_AFTER_WRAP)), NULL);
        else
                e = 0;
        EDIT_EM_SetSel(hwnd, es, extend ? es->selection_start : e, e, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MovePageDown_ML
 *
 *      Only for multi line controls
 *      Move the caret one page down, on a column with the nearest
 *      x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MovePageDown_ML(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
        LRESULT pos = EDIT_EM_PosFromChar(hwnd,0, es, e, after_wrap);
        INT x = SLOWORD(pos);
        INT y = SHIWORD(pos);

        e = EDIT_CharFromPos(hwnd, es, x,
                y + (es->format_rect.bottom - es->format_rect.top),
                &after_wrap);
        if (!extend)
                s = e;
        EDIT_EM_SetSel(hwnd, es, s, e, after_wrap);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MovePageUp_ML
 *
 *      Only for multi line controls
 *      Move the caret one page up, on a column with the nearest
 *      x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MovePageUp_ML(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
        LRESULT pos = EDIT_EM_PosFromChar(hwnd,0, es, e, after_wrap);
        INT x = SLOWORD(pos);
        INT y = SHIWORD(pos);

        e = EDIT_CharFromPos(hwnd, es, x,
                y - (es->format_rect.bottom - es->format_rect.top),
                &after_wrap);
        if (!extend)
                s = e;
        EDIT_EM_SetSel(hwnd, es, s, e, after_wrap);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveUp_ML
 *
 *      Only for multi line controls
 *      Move the caret one line up, on a column with the nearest
 *      x coordinate on the screen (might be a different column).
 *
 */
static void EDIT_MoveUp_ML(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        BOOL after_wrap = (es->flags & EF_AFTER_WRAP);
        LRESULT pos = EDIT_EM_PosFromChar(hwnd,0, es, e, after_wrap);
        INT x = SLOWORD(pos);
        INT y = SHIWORD(pos);

        e = EDIT_CharFromPos(hwnd, es, x, y - es->line_height, &after_wrap);
        if (!extend)
                s = e;
        EDIT_EM_SetSel(hwnd, es, s, e, after_wrap);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveWordBackward
 *
 */
static void EDIT_MoveWordBackward(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        INT l;
        INT ll;
        INT li;

        l = EDIT_EM_LineFromChar(hwnd, es, e);
        ll = EDIT_EM_LineLength(hwnd, es, e);
        li = EDIT_EM_LineIndex(hwnd, es, l);
        if (e - li == 0) {
                if (l) {
                        li = EDIT_EM_LineIndex(hwnd, es, l - 1);
                        e = li + EDIT_EM_LineLength(hwnd, es, li);
                }
        } else {
                e = li + (INT)EDIT_CallWordBreakProc(hwnd, es,
                                li, e - li, ll, WB_LEFT);
        }
        if (!extend)
                s = e;
        EDIT_EM_SetSel(hwnd, es, s, e, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_MoveWordForward
 *
 */
static void EDIT_MoveWordForward(HWND hwnd, EDITSTATE *es, BOOL extend)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        INT l;
        INT ll;
        INT li;

        l = EDIT_EM_LineFromChar(hwnd, es, e);
        ll = EDIT_EM_LineLength(hwnd, es, e);
        li = EDIT_EM_LineIndex(hwnd, es, l);
        if (e - li == ll) {
                if ((es->style & ES_MULTILINE) && (l != es->line_count - 1))
                        e = EDIT_EM_LineIndex(hwnd, es, l + 1);
        } else {
                e = li + EDIT_CallWordBreakProc(hwnd, es,
                                li, e - li + 1, ll, WB_RIGHT);
        }
        if (!extend)
                s = e;
        EDIT_EM_SetSel(hwnd, es, s, e, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
}


/*********************************************************************
 *
 *      EDIT_PaintLine
 *
 */
static void EDIT_PaintLine(HWND hwnd, EDITSTATE *es, HDC dc, INT line, BOOL rev)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        INT li;
        INT ll;
        INT x;
        INT y;
        LRESULT pos;

        if (es->style & ES_MULTILINE) {
                INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
                if ((line < es->y_offset) || (line > es->y_offset + vlc) || (line >= es->line_count))
                        return;
        } else if (line)
                return;

        //TRACE_(edit)("line=%d\n", line);

        pos = EDIT_EM_PosFromChar(hwnd,dc, es, EDIT_EM_LineIndex(hwnd, es, line), FALSE);
        x = SLOWORD(pos);
        y = SHIWORD(pos);
        li = EDIT_EM_LineIndex(hwnd, es, line);
        ll = EDIT_EM_LineLength(hwnd, es, li);
        s = es->selection_start;
        e = es->selection_end;
        ORDER_INT(s, e);
        s = MIN(li + ll, MAX(li, s));
        e = MIN(li + ll, MAX(li, e));

        if (rev && (s != e) && ((es->flags & EF_FOCUSED) || (es->style & ES_NOHIDESEL)))
        {
          HRGN oldRgn,newRgn,combRgn;
          RECT rect;
          //CB: OS/2 has problems with relative string positions (i.e. Communicator)
          //    fix: always calculate string from starting point, tab bugs fixed too
          //    otherwise we have 'dancing characters'

          if (!(es->style & ES_MULTILINE))
          {
            SIZE size;

            rect.top = y;
            rect.bottom = y+es->line_height;
            GetTextExtentPoint32A(dc,es->text+li,s-li,&size);
            rect.left = x+size.cx;
            GetTextExtentPoint32A(dc,es->text+li,e-li,&size);
            rect.right = x+size.cx;

            oldRgn = CreateRectRgnIndirect(&rect); //dummy parameter
            GetClipRgn(dc,oldRgn);
            newRgn = CreateRectRgnIndirect(&rect);
            combRgn = CreateRectRgnIndirect(&rect); //dummy parameter
            CombineRgn(combRgn,oldRgn,newRgn,RGN_XOR);
            SelectClipRgn(dc,combRgn);
            EDIT_PaintText(hwnd,es,dc,x,y,line,0,ll,FALSE);
            CombineRgn(combRgn,oldRgn,newRgn,RGN_AND);
            SelectClipRgn(dc,combRgn);
            EDIT_PaintText(hwnd,es,dc,x,y,line,0,e-li,TRUE);
            DeleteObject(oldRgn);
            DeleteObject(newRgn);
            DeleteObject(combRgn);
          } else
          {
            rect.top = y;
            rect.bottom = y+es->line_height;
            rect.left = x+LOWORD(TabbedTextOutA(dc,x,y,es->text+li,s-li,es->tabs_count,es->tabs,es->format_rect.left-es->x_offset));
            rect.right = x+LOWORD(TabbedTextOutA(dc,x,y,es->text+li,e-li,es->tabs_count,es->tabs,es->format_rect.left-es->x_offset));

            oldRgn = CreateRectRgnIndirect(&rect); //dummy parameter
            GetClipRgn(dc,oldRgn);
            newRgn = CreateRectRgnIndirect(&rect);
            combRgn = CreateRectRgnIndirect(&rect); //dummy parameter
            CombineRgn(combRgn,oldRgn,newRgn,RGN_XOR);
            CombineRgn(combRgn,oldRgn,combRgn,RGN_AND);
            SelectClipRgn(dc,combRgn);
            EDIT_PaintText(hwnd,es,dc,x,y,line,0,ll,FALSE);
            CombineRgn(combRgn,oldRgn,newRgn,RGN_AND);
            SelectClipRgn(dc,combRgn);
            EDIT_PaintText(hwnd,es,dc,x,y,line,0,e-li,TRUE);
            SelectClipRgn(dc,oldRgn);
            DeleteObject(oldRgn);
            DeleteObject(newRgn);
            DeleteObject(combRgn);
          }
        } else  EDIT_PaintText(hwnd, es, dc, x, y, line, 0, ll, FALSE);
}


/*********************************************************************
 *
 *      EDIT_PaintText
 *
 */
static VOID EDIT_PaintText(HWND hwnd, EDITSTATE *es, HDC dc, INT x, INT y, INT line, INT col, INT count, BOOL rev)
{
        COLORREF BkColor;
        COLORREF TextColor;
        INT li;

        if (!count)
                return;
        BkColor = GetBkColor(dc);
        TextColor = GetTextColor(dc);
        if (rev)
        {
                SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
                SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
        }
        li = EDIT_EM_LineIndex(hwnd, es, line);
        if (es->style & ES_MULTILINE)
        {
                TabbedTextOutA(dc, x, y, es->text + li + col, count,
                               es->tabs_count, es->tabs, es->format_rect.left - es->x_offset);
        } else
        {
          LPSTR text = EDIT_GetPasswordPointer_SL(hwnd, es);
          POINT pt;

          TextOutA(dc,x,y,text+li+col,count);
          if (es->style & ES_PASSWORD)
            HeapFree(es->heap, 0, text);
        }
        if (rev)
        {
                SetBkColor(dc, BkColor);
                SetTextColor(dc, TextColor);
        }
}


/*********************************************************************
 *
 *      EDIT_SetCaretPos
 *
 */
static void EDIT_SetCaretPos(HWND hwnd, EDITSTATE *es, INT pos,
                             BOOL after_wrap)
{
        LRESULT res = EDIT_EM_PosFromChar(hwnd,0, es, pos, after_wrap);
        INT x = SLOWORD(res);
        INT y = SHIWORD(res);

        if(x < es->format_rect.left)
                x = es->format_rect.left;
        if(x > es->format_rect.right - 2)
                x = es->format_rect.right - 2;
        if(y > es->format_rect.bottom)
                y = es->format_rect.bottom;
        if(y < es->format_rect.top)
                y = es->format_rect.top;
        SetCaretPos(x, y);
        return;
}


/*********************************************************************
 *
 *      EDIT_SetRectNP
 *
 *      note:   this is not (exactly) the handler called on EM_SETRECTNP
 *              it is also used to set the rect of a single line control
 *
 */
static void EDIT_SetRectNP(HWND hwnd, EDITSTATE *es, LPRECT rc)
{
        CopyRect(&es->format_rect, rc);
        if (es->style & WS_BORDER)
        {
          INT bw = GetSystemMetrics(SM_CXBORDER)+1,bh = GetSystemMetrics(SM_CYBORDER)+1;

          es->format_rect.left += bw;
          es->format_rect.top += bh;
          es->format_rect.right -= bw;
          es->format_rect.bottom -= bh;
        }
        es->format_rect.left += es->left_margin;
        es->format_rect.right -= es->right_margin;
        es->format_rect.right = MAX(es->format_rect.right, es->format_rect.left + es->char_width);
        if (es->style & ES_MULTILINE)
                es->format_rect.bottom = es->format_rect.top +
                        MAX(1, (es->format_rect.bottom - es->format_rect.top) / es->line_height) * es->line_height;
        else
                es->format_rect.bottom = es->format_rect.top + es->line_height;
        if ((es->style & ES_MULTILINE) && !(es->style & ES_AUTOHSCROLL))
                EDIT_BuildLineDefs_ML(hwnd,es);
        EDIT_UpdateScrollBars(hwnd,es,TRUE,TRUE);
}


/*********************************************************************
 *
 *      EDIT_UnlockBuffer
 *
 */
static void EDIT_UnlockBuffer(HWND hwnd, EDITSTATE *es, BOOL force)
{
        if (!es) {
                //ERR_(edit)("no EDITSTATE ... please report\n");
                return;
        }
        if (!(es->style & ES_MULTILINE))
                return;
        if (!es->lock_count) {
                //ERR_(edit)("lock_count == 0 ... please report\n");
                return;
        }
        if (!es->text) {
                //ERR_(edit)("es->text == 0 ... please report\n");
                return;
        }
        if (force || (es->lock_count == 1)) {
                if (es->hloc) {
                        LocalUnlock(es->hloc);
                        es->text = NULL;
                }
        }
        es->lock_count--;
}


/*********************************************************************
 *
 *      EDIT_WordBreakProc
 *
 *      Find the beginning of words.
 *      Note:   unlike the specs for a WordBreakProc, this function only
 *              allows to be called without linebreaks between s[0] upto
 *              s[count - 1].  Remember it is only called
 *              internally, so we can decide this for ourselves.
 *
 */
static INT EDIT_WordBreakProc(LPSTR s, INT index, INT count, INT action)
{
        INT ret = 0;

        //TRACE_(edit)("s=%p, index=%u, count=%u, action=%d\n",
        //             s, index, count, action);

        switch (action) {
        case WB_LEFT:
                if (!count)
                        break;
                if (index)
                        index--;
                if (s[index] == ' ') {
                        while (index && (s[index] == ' '))
                                index--;
                        if (index) {
                                while (index && (s[index] != ' '))
                                        index--;
                                if (s[index] == ' ')
                                        index++;
                        }
                } else {
                        while (index && (s[index] != ' '))
                                index--;
                        if (s[index] == ' ')
                                index++;
                }
                ret = index;
                break;
        case WB_RIGHT:
                if (!count)
                        break;
                if (index)
                        index--;
                if (s[index] == ' ')
                        while ((index < count) && (s[index] == ' ')) index++;
                else {
                        while (s[index] && (s[index] != ' ') && (index < count))
                                index++;
                        while ((s[index] == ' ') && (index < count)) index++;
                }
                ret = index;
                break;
        case WB_ISDELIMITER:
                ret = (s[index] == ' ');
                break;
        default:
                //ERR_(edit)("unknown action code, please report !\n");
                break;
        }
        return ret;
}


/*********************************************************************
 *
 *      EM_CHARFROMPOS
 *
 *      returns line number (not index) in high-order word of result.
 *      NB : Q137805 is unclear about this. POINT * pointer in lParam apply
 *      to Richedit, not to the edit control. Original documentation is valid.
 *
 */
static LRESULT EDIT_EM_CharFromPos(HWND hwnd, EDITSTATE *es, INT x, INT y)
{
        POINT pt;
        INT index;

        pt.x = x;
        pt.y = y;

        if (!PtInRect(&es->format_rect,pt)) return -1;

        index = EDIT_CharFromPos(hwnd, es, x, y, NULL);
        return MAKELONG(index, EDIT_EM_LineFromChar(hwnd, es, index));
}


/*********************************************************************
 *
 *      EM_FMTLINES
 *
 * Enable or disable soft breaks.
 */
static BOOL EDIT_EM_FmtLines(HWND hwnd, EDITSTATE *es, BOOL add_eol)
{
        es->flags &= ~EF_USE_SOFTBRK;
        if (add_eol) {
                es->flags |= EF_USE_SOFTBRK;
                dprintf(("EDIT: EM_FMTLINES: soft break enabled, not implemented\n"));
        }
        return add_eol;
}

static INT EDIT_EM_GetFirstVisibleLine(HWND hwnd,EDITSTATE *es)
{
  return (es->style & ES_MULTILINE) ? es->y_offset : es->x_offset;
}

/*********************************************************************
 *
 *      EM_GETHANDLE
 *
 *      Hopefully this won't fire back at us.
 *      We always start with a fixed buffer in our own heap.
 *      However, with this message a 32 bit application requests
 *      a handle to 32 bit moveable local heap memory, where it expects
 *      to find the text.
 *      It's a pity that from this moment on we have to use this
 *      local heap, because applications may rely on the handle
 *      in the future.
 *
 *      In this function we'll try to switch to local heap.
 *
 */
static HLOCAL EDIT_EM_GetHandle(HWND hwnd, EDITSTATE *es)
{
        HLOCAL newBuf;
        LPSTR newText;
        INT newSize;

        if (!(es->style & ES_MULTILINE))
                return 0;

        if (es->hloc)
                return es->hloc;

        if (!(newBuf = LocalAlloc(LMEM_MOVEABLE, lstrlenA(es->text) + 1))) {
                //ERR_(edit)("could not allocate new 32 bit buffer\n");
                return 0;
        }
#ifdef __WIN32OS2__
        newSize = LocalSize(newBuf) - 1;
#else
        newSize = MIN(LocalSize(newBuf) - 1, es->buffer_limit);
#endif
        if (!(newText = (char*)LocalLock(newBuf))) {
                //ERR_(edit)("could not lock new 32 bit buffer\n");
                LocalFree(newBuf);
                return 0;
        }
        lstrcpyA(newText, es->text);
        EDIT_UnlockBuffer(hwnd, es, TRUE);
        if (es->text)
                HeapFree(es->heap, 0, es->text);
        es->hloc = newBuf;
        es->buffer_size = newSize;
        es->text = newText;
        EDIT_LockBuffer(hwnd, es);
        //TRACE_(edit)("switched to 32 bit local heap\n");

        return es->hloc;
}

static INT EDIT_EM_GetLimitText(HWND hwnd,EDITSTATE *es)
{
  return es->buffer_limit;
}

/*********************************************************************
 *
 *      EM_GETLINE
 *
 */
static INT EDIT_EM_GetLine(HWND hwnd, EDITSTATE *es, INT line, LPSTR lpch)
{
        LPSTR src;
        INT len;
        INT i;

        if (!lpch || (*(WORD*)lpch == 0)) return 0;

        if (es->style & ES_MULTILINE) {
                if (line >= es->line_count)
                        return 0;
        } else
                line = 0;
        i = EDIT_EM_LineIndex(hwnd, es, line);
        src = es->text + i;
        len = MIN(*(WORD *)lpch, EDIT_EM_LineLength(hwnd, es, i));
        for (i = 0 ; i < len ; i++) {
                *lpch = *src;
                src++;
                lpch++;
        }
        //SvL: Terminate string
        *lpch = 0;
        return (LRESULT)len;
}

static INT EDIT_EM_GetLineCount(HWND hwnd,EDITSTATE *es)
{
  return (es->style & ES_MULTILINE) ? es->line_count : 1;
}

static LONG EDIT_EM_GetMargins(HWND hwnd,EDITSTATE *es)
{
  return MAKELONG(es->left_margin, es->right_margin);
}

static BOOL EDIT_EM_GetModify(HWND hwnd,EDITSTATE *es)
{
  return ((es->flags & EF_MODIFIED) != 0);
}

static CHAR EDIT_EM_GetPasswordChar(HWND hwnd,EDITSTATE *es)
{
  return es->password_char;
}

static VOID EDIT_EM_GetRect(HWND hwnd,EDITSTATE *es,LPRECT lprc)
{
  if (lprc) CopyRect(lprc,&es->format_rect);
}

/*********************************************************************
 *
 *      EM_GETSEL
 *
 */
static LRESULT EDIT_EM_GetSel(HWND hwnd, EDITSTATE *es, LPUINT start, LPUINT end)
{
        UINT s = es->selection_start;
        UINT e = es->selection_end;

        ORDER_UINT(s, e);

        if (start)
                *start = s;
        if (end)
                *end = e;
        return MAKELONG(s, e);
}

/*********************************************************************
 *
 *      EM_GETTHUMB
 */
static LRESULT EDIT_EM_GetThumb(HWND hwnd, EDITSTATE *es)
{
  SCROLLINFO si;

  if (!(es->style & ES_MULTILINE)) return 0;

  si.cbSize = sizeof(si);
  si.fMask  = SIF_TRACKPOS;
  return GetScrollInfo(hwnd,SB_VERT,&si) ? si.nTrackPos:0;
}

static PVOID EDIT_EM_GetWordbreakProc(HWND hwnd,EDITSTATE *es)
{
  return es->word_break_procA;
}

/*********************************************************************
 *
 *      EM_LINEFROMCHAR
 *
 */
static INT EDIT_EM_LineFromChar(HWND hwnd, EDITSTATE *es, INT index)
{
        INT line;
        LINEDEF *line_def;

        if (!(es->style & ES_MULTILINE))
                return 0;
        if (index > lstrlenA(es->text))
                return es->line_count - 1;
        if (index == -1)
                index = MIN(es->selection_start, es->selection_end); //selection_end == caret pos

        line = 0;
        line_def = es->first_line_def;
        index -= line_def->length;
        while ((index >= 0) && line_def->next) {
                line++;
                line_def = line_def->next;
                index -= line_def->length;
        }
        return line;
}


/*********************************************************************
 *
 *      EM_LINEINDEX
 *
 */
static INT EDIT_EM_LineIndex(HWND hwnd, EDITSTATE *es, INT line)
{
        INT line_index;
        LINEDEF *line_def;

        if (!(es->style & ES_MULTILINE))
                return 0;
        if (line >= es->line_count)
                return -1;

        line_index = 0;
        line_def = es->first_line_def;
        if (line == -1) {
                INT index = es->selection_end - line_def->length;
                while ((index >= 0) && line_def->next) {
                        line_index += line_def->length;
                        line_def = line_def->next;
                        index -= line_def->length;
                }
        } else {
                while (line > 0) {
                        line_index += line_def->length;
                        line_def = line_def->next;
                        line--;
                }
        }
        return line_index;
}


/*********************************************************************
 *
 *      EM_LINELENGTH
 *
 */
static INT EDIT_EM_LineLength(HWND hwnd, EDITSTATE *es, INT index)
{
        LINEDEF *line_def;

        if (!(es->style & ES_MULTILINE))
                return lstrlenA(es->text);

        if (index == -1)
        {
          INT sl = EDIT_EM_LineFromChar(hwnd,es,MIN(es->selection_start,es->selection_end));
          INT el = EDIT_EM_LineFromChar(hwnd,es,MAX(es->selection_start,es->selection_end));

          if (sl == el)
            return EDIT_EM_LineLength(hwnd,es,sl)+es->selection_start-es->selection_end;
          else
            return es->selection_start+EDIT_EM_LineLength(hwnd,es,el)-es->selection_end;
        }
        line_def = es->first_line_def;
        index -= line_def->length;
        while ((index >= 0) && line_def->next) {
                line_def = line_def->next;
                index -= line_def->length;
        }
        return line_def->net_length;
}

static VOID EDIT_UpdateScrollBars(HWND hwnd,EDITSTATE *es,BOOL updateHorz,BOOL updateVert)
{
  if (updateHorz && (es->style & WS_HSCROLL) && !(es->flags & EF_HSCROLL_TRACK))
  {
    SCROLLINFO si;
    INT fw = es->format_rect.right - es->format_rect.left;

    si.cbSize       = sizeof(SCROLLINFO);
    si.fMask        = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
    si.nMin         = 0;
    si.nMax         = es->text_width + fw - 1;
    si.nPage        = fw;
    si.nPos         = es->x_offset;
    SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
  }

  if (updateVert && (es->style & WS_VSCROLL) && !(es->flags & EF_VSCROLL_TRACK))
  {
    INT vlc = (es->format_rect.bottom-es->format_rect.top)/es->line_height;
    SCROLLINFO si;

    si.cbSize       = sizeof(SCROLLINFO);
    si.fMask        = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
    si.nMin         = 0;
    si.nMax         = es->line_count + vlc - 2;
    si.nPage        = vlc;
    si.nPos         = es->y_offset;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
  }
}


/*********************************************************************
 *
 *      EM_LINESCROLL
 *
 *      FIXME: dx is in average character widths
 *              However, we assume it is in pixels when we use this
 *              function internally
 *
 */
static BOOL EDIT_EM_LineScroll(HWND hwnd, EDITSTATE *es, INT dx, INT dy)
{
        INT nyoff;
        INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;

        if (!(es->style & ES_MULTILINE))
                return FALSE;

        if (-dx > es->x_offset)
                dx = -es->x_offset;
        if (dx > es->text_width - es->x_offset)
                dx = es->text_width - es->x_offset;
        nyoff = MAX(0, es->y_offset + dy);

        //SvL: If nyoff > nr lines in control -> last line in control
        //     window should show last line of control
        //     Wine code shows last line at the top of the control
        //     (Quake 3 startup edit control shows the problem)
        if (nyoff >= es->line_count) {
                if(es->line_count <= vlc) {
                        nyoff = es->y_offset;   //no need to scroll
                }
                else    nyoff = es->line_count - vlc - 1;
        }
        dy = (es->y_offset - nyoff) * es->line_height;
        if (dx || dy)
        {
                RECT rc1;
                RECT rc;

                if (dx && !(es->flags & EF_HSCROLL_TRACK))
                  EDIT_NOTIFY_PARENT(hwnd, EN_HSCROLL);
                if (dy && !(es->flags & EF_VSCROLL_TRACK))
                  EDIT_NOTIFY_PARENT(hwnd, EN_VSCROLL);

                GetClientRect(hwnd, &rc1);
                IntersectRect(&rc, &rc1, &es->format_rect);

                ScrollWindowEx(hwnd,-dx,dy,NULL,&rc,(HRGN)NULL,NULL,SW_INVALIDATE);
                es->y_offset = nyoff;
                es->x_offset += dx;
                EDIT_UpdateScrollBars(hwnd,es,dx,dy);
        }

        return TRUE;
}


/*********************************************************************
 *
 *      EM_POSFROMCHAR
 *
 */
static LRESULT EDIT_EM_PosFromChar(HWND hwnd,HDC dc, EDITSTATE *es, INT index, BOOL after_wrap)
{
        INT len = lstrlenA(es->text);
        INT l;
        INT li;
        INT x;
        INT y = 0;
        BOOL createdDC = FALSE;
        HFONT old_font = 0;
        SIZE size;

        index = MIN(index, len);
        if (!dc)
        {
          dc = GetDC(hwnd);
          if (es->font)
            old_font = SelectObject(dc, es->font);
          createdDC = TRUE;
        }
        if (es->style & ES_MULTILINE) {
                l = EDIT_EM_LineFromChar(hwnd, es, index);
                y = (l - es->y_offset) * es->line_height;
                li = EDIT_EM_LineIndex(hwnd, es, l);
                if (after_wrap && (li == index) && l) {
                        INT l2 = l - 1;
                        LINEDEF *line_def = es->first_line_def;
                        while (l2) {
                                line_def = line_def->next;
                                l2--;
                        }
                        if (line_def->ending == END_WRAP) {
                                l--;
                                y -= es->line_height;
                                li = EDIT_EM_LineIndex(hwnd, es, l);
                        }
                }
                x = LOWORD(GetTabbedTextExtentA(dc, es->text + li, index - li,
                                es->tabs_count, es->tabs)) - es->x_offset;
        } else
        {
          LPSTR text = EDIT_GetPasswordPointer_SL(hwnd, es);

          GetTextExtentPoint32A(dc,text,index,&size);
          x = size.cx;
          if (es->x_offset)
          {
            GetTextExtentPoint32A(dc,text,es->x_offset,&size);
            x -= size.cx;
          }
          y = 0;
          if (es->style & ES_PASSWORD)
            HeapFree(es->heap, 0 ,text);
        }
        x += es->format_rect.left;
        y += es->format_rect.top;
        if (createdDC)
        {
          if (es->font)
            SelectObject(dc, old_font);
          ReleaseDC(hwnd, dc);
        }
        return MAKELONG((INT16)x, (INT16)y);
}

BOOL EDIT_CheckNumber(CHAR *text)
{
  if (!text) return TRUE;

  while (text[0] != 0)
  {
    if ((BYTE)text[0] < '0' || (BYTE)text[0] > '9') return FALSE;
    text++;
  }

  return TRUE;
}

/*********************************************************************
 *
 *      EM_REPLACESEL
 *
 */
static void EDIT_EM_ReplaceSel(HWND hwnd, EDITSTATE *es, BOOL can_undo, LPCSTR lpsz_replace)
{
        INT strl = lstrlenA(lpsz_replace);
        INT tl = lstrlenA(es->text);
        INT utl;
        UINT s;
        UINT e;
        INT i;
        LPSTR p;

        s = es->selection_start;
        e = es->selection_end;

        if ((s == e) && !strl)
                return;

        ORDER_UINT(s, e);

        if (!EDIT_MakeFit(hwnd, es, tl - (e - s) + strl))
                return;

        if (e != s) {
                /* there is something to be deleted */
                if (can_undo) {
                        utl = lstrlenA(es->undo_text);
                        if (!es->undo_insert_count && (*es->undo_text && (s == es->undo_position))) {
                                /* undo-buffer is extended to the right */
                                EDIT_MakeUndoFit(hwnd, es, utl + e - s);
                                lstrcpynA(es->undo_text + utl, es->text + s, e - s + 1);
                        } else if (!es->undo_insert_count && (*es->undo_text && (e == es->undo_position))) {
                                /* undo-buffer is extended to the left */
                                EDIT_MakeUndoFit(hwnd, es, utl + e - s);
                                for (p = es->undo_text + utl ; p >= es->undo_text ; p--)
                                        p[e - s] = p[0];
                                for (i = 0 , p = es->undo_text ; i < e - s ; i++)
                                        p[i] = (es->text + s)[i];
                                es->undo_position = s;
                        } else {
                                /* new undo-buffer */
                                EDIT_MakeUndoFit(hwnd, es, e - s);
                                lstrcpynA(es->undo_text, es->text + s, e - s + 1);
                                es->undo_position = s;
                        }
                        /* any deletion makes the old insertion-undo invalid */
                        es->undo_insert_count = 0;
                } else
                        EDIT_EM_EmptyUndoBuffer(hwnd, es);

                /* now delete */
                lstrcpyA(es->text + s, es->text + e);
        }
        if (strl)
        {
          if ((es->style & ES_NUMBER) && !EDIT_CheckNumber((CHAR*)lpsz_replace))
            MessageBeep(MB_ICONEXCLAMATION);
          else
          {
            /* there is an insertion */
            if (can_undo) {
              if ((s == es->undo_position) ||
                  ((es->undo_insert_count) &&
                  (s == es->undo_position + es->undo_insert_count)))
                  /*
                   * insertion is new and at delete position or
                   * an extension to either left or right
                   */
                  es->undo_insert_count += strl;
              else {
                /* new insertion undo */
                es->undo_position = s;
                es->undo_insert_count = strl;
                /* new insertion makes old delete-buffer invalid */
                *es->undo_text = '\0';
              }
            } else
              EDIT_EM_EmptyUndoBuffer(hwnd, es);


            /* now insert */
            tl = lstrlenA(es->text);
            for (p = es->text + tl ; p >= es->text + s ; p--)
            p[strl] = p[0];
            for (i = 0 , p = es->text + s ; i < strl ; i++)
              p[i] = lpsz_replace[i];

            if (es->style & ES_OEMCONVERT)
            {
              CHAR *text = (LPSTR)HeapAlloc(es->heap,0,strl);

              CharToOemBuffA(lpsz_replace,text,strl);
              OemToCharBuffA(text,p,strl);
              HeapFree(es->heap,0,text);
            }

            if(es->style & ES_UPPERCASE)
              CharUpperBuffA(p, strl);
            else if(es->style & ES_LOWERCASE)
              CharLowerBuffA(p, strl);

            s += strl;
          }
        }
        /* FIXME: really inefficient */
        if (es->style & ES_MULTILINE)
                EDIT_BuildLineDefs_ML(hwnd,es);

        EDIT_EM_SetSel(hwnd, es, s, s, FALSE);

        es->flags |= EF_MODIFIED;
        es->flags |= EF_UPDATE;
        EDIT_EM_ScrollCaret(hwnd, es);

        /* FIXME: really inefficient */
        EDIT_Refresh(hwnd,es,TRUE);
}


/*********************************************************************
 *
 *      EM_SCROLL
 *
 */
static LRESULT EDIT_EM_Scroll(HWND hwnd, EDITSTATE *es, INT action)
{
        INT dy;

        if (!(es->style & ES_MULTILINE))
                return (LRESULT)FALSE;

        dy = 0;

        switch (action) {
        case SB_LINEUP:
                if (es->y_offset)
                        dy = -1;
                break;
        case SB_LINEDOWN:
                if (es->y_offset < es->line_count - 1)
                        dy = 1;
                break;
        case SB_PAGEUP:
                if (es->y_offset)
                        dy = -(es->format_rect.bottom - es->format_rect.top) / es->line_height;
                break;
        case SB_PAGEDOWN:
                if (es->y_offset < es->line_count - 1)
                        dy = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
                break;
        default:
                return (LRESULT)FALSE;
        }
        if (dy) {
                EDIT_NOTIFY_PARENT(hwnd, EN_VSCROLL);
                EDIT_EM_LineScroll(hwnd, es, 0, dy);
        }
        return MAKELONG((INT16)dy, (BOOL16)TRUE);
}


/*********************************************************************
 *
 *      EM_SCROLLCARET
 *
 */
static void EDIT_EM_ScrollCaret(HWND hwnd, EDITSTATE *es)
{
        if (es->style & ES_MULTILINE) {
                INT l;
                INT li;
                INT vlc;
                INT ww;
                INT cw = es->char_width;
                INT x;
                INT dy = 0;
                INT dx = 0;

                l = EDIT_EM_LineFromChar(hwnd, es, es->selection_end);
                li = EDIT_EM_LineIndex(hwnd, es, l);
                x = SLOWORD(EDIT_EM_PosFromChar(hwnd,0, es, es->selection_end, es->flags & EF_AFTER_WRAP));
                vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;
                if (l >= es->y_offset + vlc)
                        dy = l - vlc + 1 - es->y_offset;
                if (l < es->y_offset)
                        dy = l - es->y_offset;
                ww = es->format_rect.right - es->format_rect.left;
                if (x < es->format_rect.left)
                        dx = x - es->format_rect.left - ww / HSCROLL_FRACTION / cw * cw;
                if (x > es->format_rect.right)
                        dx = x - es->format_rect.left - (HSCROLL_FRACTION - 1) * ww / HSCROLL_FRACTION / cw * cw;
                if (dy || dx)
                        EDIT_EM_LineScroll(hwnd, es, dx, dy);
        } else {
                INT x;
                INT goal;
                INT format_width;

                if (!(es->style & ES_AUTOHSCROLL))
                        return;

                x = SLOWORD(EDIT_EM_PosFromChar(hwnd,0, es, es->selection_end, FALSE));
                format_width = es->format_rect.right - es->format_rect.left;
                if (x < es->format_rect.left)
                {
                        goal = es->format_rect.left + format_width / HSCROLL_FRACTION;
                        do {
                                es->x_offset--;
                                x = SLOWORD(EDIT_EM_PosFromChar(hwnd,0, es, es->selection_end, FALSE));
                        } while ((x < goal) && es->x_offset);
                        /* FIXME: use ScrollWindow() somehow to improve performance */
                        EDIT_Refresh(hwnd,es,TRUE);
                        EDIT_UpdateScrollBars(hwnd,es,TRUE,FALSE);
                } else if (x > es->format_rect.right)
                {
                        INT x_last;
                        INT len = lstrlenA(es->text);
                        goal = es->format_rect.right - format_width / HSCROLL_FRACTION;
                        do {
                                es->x_offset++;
                                x = SLOWORD(EDIT_EM_PosFromChar(hwnd,0, es, es->selection_end, FALSE));
                                x_last = SLOWORD(EDIT_EM_PosFromChar(hwnd,0, es, len, FALSE));
                        } while ((x > goal) && (x_last > es->format_rect.right));
                        /* FIXME: use ScrollWindow() somehow to improve performance */
                        EDIT_Refresh(hwnd,es,TRUE);
                        EDIT_UpdateScrollBars(hwnd,es,TRUE,FALSE);
                }
        }
}


/*********************************************************************
 *
 *      EM_SETHANDLE
 *
 */
static void EDIT_EM_SetHandle(HWND hwnd, EDITSTATE *es, HLOCAL hloc)
{
        if (!(es->style & ES_MULTILINE))
                return;

        if (!hloc) {
                //WARN_(edit)("called with NULL handle\n");
                return;
        }

        EDIT_UnlockBuffer(hwnd, es, TRUE);
        /*
         *      old buffer is freed by caller, unless
         *      it is still in our own heap.  (in that case
         *      we free it, correcting the buggy caller.)
         */
        if (es->text)
                HeapFree(es->heap, 0, es->text);

        es->hloc = hloc;
        es->text = NULL;
        es->buffer_size = LocalSize(es->hloc) - 1;
        EDIT_LockBuffer(hwnd, es);

        if (es->text && (es->text[0] != 0))
        {
          if (es->style & ES_NUMBER)
          {
            //CB: todo
          }

          if (es->style & ES_OEMCONVERT)
          {
            INT len = lstrlenA(es->text);
            CHAR *text = (LPSTR)HeapAlloc(es->heap,0,len);

            CharToOemBuffA(es->text,text,len);
            OemToCharBuffA(text,es->text,len);
            HeapFree(es->heap,0,text);
          }

          if(es->style & ES_UPPERCASE)
            CharUpperA(es->text);
          else if(es->style & ES_LOWERCASE)
            CharLowerA(es->text);
        }

        es->x_offset = es->y_offset = 0;
        es->selection_start = es->selection_end = 0;
        EDIT_EM_EmptyUndoBuffer(hwnd, es);
        es->flags &= ~EF_MODIFIED;
        es->flags &= ~EF_UPDATE;
        EDIT_BuildLineDefs_ML(hwnd,es);
        EDIT_Refresh(hwnd,es,FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
        EDIT_UpdateScrollBars(hwnd,es,TRUE,TRUE);
}

/*********************************************************************
 *
 *      EM_SETLIMITTEXT
 *
 *      FIXME: in WinNT maxsize is 0x7FFFFFFF / 0xFFFFFFFF
 *      However, the windows version is not complied to yet in all of edit.c
 *
 */
static void EDIT_EM_SetLimitText(HWND hwnd, EDITSTATE *es, INT limit)
{
        if (es->style & ES_MULTILINE) {
                if (limit)
#ifdef __WIN32OS2__
                        es->buffer_limit = limit;
#else
                        es->buffer_limit = MIN(limit, BUFLIMIT_MULTI);
#endif
                else
                        es->buffer_limit = BUFLIMIT_MULTI;
        } else {
                if (limit)
#ifdef __WIN32OS2__
                        es->buffer_limit = MIN(limit, BUFLIMIT_SINGLE_NT);
#else
                        es->buffer_limit = MIN(limit, BUFLIMIT_SINGLE);
#endif
                else
                        es->buffer_limit = BUFLIMIT_SINGLE;
        }
}


/*********************************************************************
 *
 *      EM_SETMARGINS
 *
 * EC_USEFONTINFO is used as a left or right value i.e. lParam and not as an
 * action wParam despite what the docs say. EC_USEFONTINFO means one third
 * of the char's width, according to the new docs.
 *
 */
static void EDIT_EM_SetMargins(HWND hwnd, EDITSTATE *es, INT action,
                               INT left, INT right)
{
        RECT r;
        INT oldLeft = es->left_margin,oldRight = es->right_margin;

        if (action & EC_LEFTMARGIN) {
                if (left != EC_USEFONTINFO)
                        es->left_margin = left;
                else
                        es->left_margin = es->char_width / 3;
        }

        if (action & EC_RIGHTMARGIN) {
                if (right != EC_USEFONTINFO)
                        es->right_margin = right;
                else
                        es->right_margin = es->char_width / 3;
        }
        //TRACE_(edit)("left=%d, right=%d\n", es->left_margin, es->right_margin);

        if ((oldLeft != es->left_margin) || (oldRight != es->right_margin))
        {
          GetClientRect(hwnd, &r);
          EDIT_SetRectNP(hwnd, es, &r);
          if (es->style & ES_MULTILINE)
                  EDIT_BuildLineDefs_ML(hwnd,es);

          EDIT_Refresh(hwnd,es,FALSE);
          if (es->flags & EF_FOCUSED) {
                  DestroyCaret();
                  CreateCaret(hwnd, 0, 2, es->line_height);
                  EDIT_SetCaretPos(hwnd, es, es->selection_end,
                                   es->flags & EF_AFTER_WRAP);
                  ShowCaret(hwnd);
          }
        }
}

static void EDIT_EM_SetModify(HWND hwnd,EDITSTATE *es,BOOL fModified)
{
  if (fModified)
    es->flags |= EF_MODIFIED;
  else
    es->flags &= ~(EF_MODIFIED | EF_UPDATE);  /* reset pending updates */
}

/*********************************************************************
 *
 *      EM_SETPASSWORDCHAR
 *
 */
static void EDIT_EM_SetPasswordChar(HWND hwnd, EDITSTATE *es, CHAR c)
{
        if (es->style & ES_MULTILINE)
                return;

        if (es->password_char == c)
                return;

        es->password_char = c;
        if (c) {
                SetWindowLongA(hwnd,GWL_STYLE,GetWindowLongA(hwnd,GWL_STYLE) | ES_PASSWORD);
                es->style |= ES_PASSWORD;
        } else {
                SetWindowLongA(hwnd,GWL_STYLE,GetWindowLongA(hwnd,GWL_STYLE) & ~ES_PASSWORD);
                es->style &= ~ES_PASSWORD;
        }
        EDIT_Refresh(hwnd,es,FALSE);
}

static BOOL EDIT_EM_SetReadOnly(HWND hwnd,EDITSTATE *es,BOOL fReadOnly)
{
  if (fReadOnly)
  {
    SetWindowLongA(hwnd,GWL_STYLE,GetWindowLongA(hwnd,GWL_STYLE) | ES_READONLY);
    es->style |= ES_READONLY;
  } else
  {
    SetWindowLongA(hwnd,GWL_STYLE,GetWindowLongA(hwnd,GWL_STYLE) & ~ES_READONLY);
    es->style &= ~ES_READONLY;
  }

  return TRUE;
}

static void EDIT_EM_SetRect(HWND hwnd,EDITSTATE *es,LPRECT lprc)
{
  if ((es->style & ES_MULTILINE) && lprc)
  {
    EDIT_SetRectNP(hwnd,es,lprc);
    EDIT_Refresh(hwnd,es,FALSE);
  }
}

static void EDIT_EM_SetRectNP(HWND hwnd,EDITSTATE *es,LPRECT lprc)
{
  if ((es->style & ES_MULTILINE) && lprc)
    EDIT_SetRectNP(hwnd,es,lprc);
}

/*********************************************************************
 *
 *      EDIT_EM_SetSel
 *
 *      note:   unlike the specs say: the order of start and end
 *              _is_ preserved in Windows.  (i.e. start can be > end)
 *              In other words: this handler is OK
 *
 */
static void EDIT_EM_SetSel(HWND hwnd, EDITSTATE *es, UINT start, UINT end, BOOL after_wrap)
{
        UINT old_start = es->selection_start;
        UINT old_end = es->selection_end;
        UINT len = lstrlenA(es->text);

        if (start == -1) {
                start = es->selection_end;
                end = es->selection_end;
        } else {
                start = MIN(start, len);
                end = MIN(end, len);
        }
        es->selection_start = start;
        es->selection_end = end;
        if (after_wrap)
                es->flags |= EF_AFTER_WRAP;
        else
                es->flags &= ~EF_AFTER_WRAP;
        if (es->flags & EF_FOCUSED)
                EDIT_SetCaretPos(hwnd, es, end, after_wrap);
/* This is little  bit more efficient than before, not sure if it can be improved. FIXME? */
        ORDER_UINT(start, end);
        ORDER_UINT(end, old_end);
        ORDER_UINT(start, old_start);
        ORDER_UINT(old_start, old_end);
        if ((start == old_start) && (end == old_end)) return;
        if (end != old_start)
        {
/*
 * One can also do
 *          ORDER_UINT32(end, old_start);
 *          EDIT_InvalidateText(wnd, es, start, end);
 *          EDIT_InvalidateText(wnd, es, old_start, old_end);
 * in place of the following if statement.
 */
            if (old_start > end )
            {
                EDIT_InvalidateText(hwnd, es, start, end);
                EDIT_InvalidateText(hwnd, es, old_start, old_end);
            }
            else
            {
                EDIT_InvalidateText(hwnd, es, start, old_start);
                EDIT_InvalidateText(hwnd, es, end, old_end);
            }
        }
        else EDIT_InvalidateText(hwnd, es, start, old_end);
}


/*********************************************************************
 *
 *      EM_SETTABSTOPS
 *
 */
static BOOL EDIT_EM_SetTabStops(HWND hwnd, EDITSTATE *es, INT count, LPINT tabs)
{
        if (!(es->style & ES_MULTILINE))
                return FALSE;
        if (es->tabs)
                HeapFree(es->heap, 0, es->tabs);
        es->tabs_count = count;
        if (!count)
                es->tabs = NULL;
        else {
                es->tabs = (INT*)HeapAlloc(es->heap, 0, count * sizeof(INT));
                memcpy(es->tabs, tabs, count * sizeof(INT));
        }
        return TRUE;
}


/*********************************************************************
 *
 *      EM_SETWORDBREAKPROC
 *
 */
static void EDIT_EM_SetWordBreakProc(HWND hwnd, EDITSTATE *es, EDITWORDBREAKPROCA wbp)
{
        if (es->word_break_procA == wbp)
                return;

        es->word_break_procA = wbp;
        if ((es->style & ES_MULTILINE) && !(es->style & ES_AUTOHSCROLL)) {
                EDIT_BuildLineDefs_ML(hwnd,es);
                EDIT_Refresh(hwnd,es,FALSE);
        }
}


/*********************************************************************
 *
 *      EM_UNDO / WM_UNDO
 *
 */
static BOOL EDIT_EM_Undo(HWND hwnd, EDITSTATE *es)
{
        INT ulength = lstrlenA(es->undo_text);
        LPSTR utext = (LPSTR)HeapAlloc(es->heap, 0, ulength + 1);

        lstrcpyA(utext, es->undo_text);

        //TRACE_(edit)("before UNDO:insertion length = %d, deletion buffer = %s\n",
        //             es->undo_insert_count, utext);

        EDIT_EM_SetSel(hwnd, es, es->undo_position, es->undo_position + es->undo_insert_count, FALSE);
        EDIT_EM_EmptyUndoBuffer(hwnd, es);
        EDIT_EM_ReplaceSel(hwnd, es, TRUE, utext);
        EDIT_EM_SetSel(hwnd, es, es->undo_position, es->undo_position + es->undo_insert_count, FALSE);
        HeapFree(es->heap, 0, utext);

        //TRACE_(edit)("after UNDO:insertion length = %d, deletion buffer = %s\n",
        //                es->undo_insert_count, es->undo_text);

        if (es->flags & EF_UPDATE) {
                es->flags &= ~EF_UPDATE;
                EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
        }

        return TRUE;
}

static LRESULT EDIT_EM_SetIMEStatus(HWND hwnd,EDITSTATE *es,WPARAM wParam,LPARAM lParam)
{
  if (wParam == EMSIS_COMPOSITIONSTRING)
  {
    //CB: todo
  }

  return 0;
}

static LRESULT EDIT_EM_GetIMEStatus(HWND hwnd,EDITSTATE *es,WPARAM wParam,LPARAM lParam)
{
  //CB: todo

  return 0; //default
}

/*********************************************************************
 *
 *      WM_CHAR
 *
 */
static void EDIT_WM_Char(HWND hwnd, EDITSTATE *es, CHAR c, DWORD key_data)
{
        BOOL control = GetKeyState(VK_CONTROL) & 0x8000;
        switch (c) {
        case '\r':
            /* If the edit doesn't want the return and it's not a multiline edit, do nothing */
            if(!(es->style & ES_MULTILINE) && !(es->style & ES_WANTRETURN))
            {
              MessageBeep(MB_ICONEXCLAMATION);
              break;
            }
        case '\n':
                if (es->style & ES_MULTILINE) {
                        if (es->style & ES_READONLY) {
                                EDIT_MoveHome(hwnd, es, FALSE);
                                EDIT_MoveDown_ML(hwnd, es, FALSE);
                        } else
                        {
                                EDIT_EM_ReplaceSel(hwnd, es, TRUE, "\r\n");
                                if (es->flags & EF_UPDATE) {
                                        es->flags &= ~EF_UPDATE;
                                        EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
                                }
                        }
                }
                break;
        case '\t':
                if ((es->style & ES_MULTILINE) && !(es->style & ES_READONLY))
                {
                        EDIT_EM_ReplaceSel(hwnd, es, TRUE, "\t");
                        if (es->flags & EF_UPDATE) {
                                es->flags &= ~EF_UPDATE;
                                EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
                        }
                }
                break;
        case VK_BACK:
                if (!(es->style & ES_READONLY) && !control) {
                        if (es->selection_start != es->selection_end)
                                EDIT_WM_Clear(hwnd, es);
                        else {
                                /* delete character left of caret */
                                EDIT_EM_SetSel(hwnd, es, -1, 0, FALSE);
                                EDIT_MoveBackward(hwnd, es, TRUE);
                                EDIT_WM_Clear(hwnd, es);
                        }
                }
                break;
//CB: are these three keys documented or Linux style???
        case 0x03: /* ^C */
                SendMessageA(hwnd, WM_COPY, 0, 0);
                break;
        case 0x16: /* ^V */
                SendMessageA(hwnd, WM_PASTE, 0, 0);
                break;
        case 0x18: /* ^X */
                SendMessageA(hwnd, WM_CUT, 0, 0);
                break;

        default:
                if (!(es->style & ES_READONLY) && ((BYTE)c >= ' ') && (c != 127))
                {
                  char str[2];

                  if (es->style & ES_NUMBER)
                  {
                    	if (((BYTE)c < '0') || ((BYTE)c > '9')) {
				MessageBeep(MB_ICONEXCLAMATION);
                    		return;
			}
                  }
                  str[0] = c;
                  str[1] = '\0';
                  EDIT_EM_ReplaceSel(hwnd, es, TRUE, str);
                  if (es->flags & EF_UPDATE)
                  {
                    es->flags &= ~EF_UPDATE;
                    EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
                  }
                } else MessageBeep(MB_ICONEXCLAMATION);
                break;
        }
}


/*********************************************************************
 *
 *      WM_COMMAND
 *
 */
static void EDIT_WM_Command(HWND hwnd, EDITSTATE *es, INT code, INT id, HWND control)
{
        if (code || control)
                return;

        switch (id) {
                case EM_UNDO:
                        EDIT_EM_Undo(hwnd, es);
                        break;
                case WM_CUT:
                        EDIT_WM_Cut(hwnd, es);
                        break;
                case WM_COPY:
                        EDIT_WM_Copy(hwnd, es);
                        break;
                case WM_PASTE:
                        EDIT_WM_Paste(hwnd, es);
                        break;
                case WM_CLEAR:
                        EDIT_WM_Clear(hwnd, es);
                        break;
                case EM_SETSEL:
                        EDIT_EM_SetSel(hwnd, es, 0, -1, FALSE);
                        EDIT_EM_ScrollCaret(hwnd, es);
                        break;
                default:
                        //ERR_(edit)("unknown menu item, please report\n");
                        break;
        }
}


/*********************************************************************
 *
 *      WM_CONTEXTMENU
 *
 *      Note: the resource files resource/sysres_??.rc cannot define a
 *              single popup menu.  Hence we use a (dummy) menubar
 *              containing the single popup menu as its first item.
 *
 *      FIXME: the message identifiers have been chosen arbitrarily,
 *              hence we use MF_BYPOSITION.
 *              We might as well use the "real" values (anybody knows ?)
 *              The menu definition is in resources/sysres_??.rc.
 *              Once these are OK, we better use MF_BYCOMMAND here
 *              (as we do in EDIT_WM_Command()).
 *
 */
static void EDIT_WM_ContextMenu(HWND hwnd, EDITSTATE *es, HWND hwndBtn, INT x, INT y)
{
        HMENU menu = LoadMenuA(GetModuleHandleA("USER32"), "EDITMENU");
        HMENU popup = GetSubMenu(menu, 0);
        UINT start = es->selection_start;
        UINT end = es->selection_end;

        ORDER_UINT(start, end);

        /* undo */
        EnableMenuItem(popup, 0, MF_BYPOSITION | (EDIT_EM_CanUndo(hwnd, es) ? MF_ENABLED : MF_GRAYED));
        /* cut */
        EnableMenuItem(popup, 2, MF_BYPOSITION | ((end - start) && !(es->style & (ES_PASSWORD | ES_READONLY)) ? MF_ENABLED : MF_GRAYED));
        /* copy */
        EnableMenuItem(popup, 3, MF_BYPOSITION | ((end - start) && !(es->style & ES_PASSWORD) ? MF_ENABLED : MF_GRAYED));
        /* paste */
        EnableMenuItem(popup, 4, MF_BYPOSITION | (IsClipboardFormatAvailable(CF_TEXT) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
        /* delete */
        EnableMenuItem(popup, 5, MF_BYPOSITION | ((end - start) && !(es->style & ES_READONLY) ? MF_ENABLED : MF_GRAYED));
        /* select all */
        EnableMenuItem(popup, 7, MF_BYPOSITION | (start || (end != lstrlenA(es->text)) ? MF_ENABLED : MF_GRAYED));

        TrackPopupMenu(popup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hwnd, NULL);
        DestroyMenu(menu);
}


/*********************************************************************
 *
 *      WM_COPY
 *
 */
static void EDIT_WM_Copy(HWND hwnd, EDITSTATE *es)
{
        INT s = es->selection_start;
        INT e = es->selection_end;
        HGLOBAL hdst;
        LPSTR dst;

        if (e == s)
                return;
        ORDER_INT(s, e);
        hdst = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(e - s + 1));
        dst = (LPSTR)GlobalLock(hdst);
        lstrcpynA(dst, es->text + s, e - s + 1);
        GlobalUnlock(hdst);
        OpenClipboard(hwnd);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hdst);
        CloseClipboard();
}


/*********************************************************************
 *
 *      WM_CREATE
 *
 */
static LRESULT EDIT_WM_Create(HWND hwnd, EDITSTATE *es, LPCREATESTRUCTA cs)
{
       /*
        *       To initialize some final structure members, we call some helper
        *       functions.  However, since the EDITSTATE is not consistent (i.e.
        *       not fully initialized), we should be very careful which
        *       functions can be called, and in what order.
        */
        EDIT_WM_SetFont(hwnd, es, 0, FALSE);
        EDIT_EM_EmptyUndoBuffer(hwnd, es);
       if (cs->lpszName && *(cs->lpszName) != '\0') {
           EDIT_EM_ReplaceSel(hwnd, es, FALSE, cs->lpszName);
           /* if we insert text to the editline, the text scrolls out
            * of the window, as the caret is placed after the insert
            * pos normally; thus we reset es->selection... to 0 and
            * update caret
            */
           es->selection_start = es->selection_end = 0;
           EDIT_EM_ScrollCaret(hwnd, es);
           if (es->flags & EF_UPDATE) {
                es->flags &= ~EF_UPDATE;
                EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
           }
       }
       return 0;
}


/*********************************************************************
 *
 *      WM_DESTROY
 *
 */
static void EDIT_WM_Destroy(HWND hwnd, EDITSTATE *es)
{
        if (!es) /* Protect against multiple destroy messages */
            return;

        if (es->hloc) {
                while (LocalUnlock(es->hloc)) ;
                LocalFree(es->hloc);
        }

        HeapDestroy(es->heap);
        HeapFree(GetProcessHeap(), 0, es);
        SetInfoPtr(hwnd,0);
}


/*********************************************************************
 *
 *      WM_ERASEBKGND
 *
 */
static LRESULT EDIT_WM_EraseBkGnd(HWND hwnd, EDITSTATE *es, HDC dc)
{
        HBRUSH brush;
        RECT rc;

        HideCaret(hwnd);

        if (!es->bEnableState || (es->style & ES_READONLY))
                brush = (HBRUSH)EDIT_SEND_CTLCOLORSTATIC(hwnd, dc);
        else
                brush = (HBRUSH)EDIT_SEND_CTLCOLOR(hwnd, dc);

        if (!brush)
                brush = (HBRUSH)GetStockObject(WHITE_BRUSH);

        GetClientRect(hwnd, &rc);
        IntersectClipRect(dc, rc.left, rc.top, rc.right, rc.bottom);
        GetClipBox(dc, &rc);
        /*
         *      FIXME:  specs say that we should UnrealizeObject() the brush,
         *              but the specs of UnrealizeObject() say that we shouldn't
         *              unrealize a stock object.  The default brush that
         *              DefWndProc() returns is ... a stock object.
         */
        FillRect(dc, &rc, brush);

        ShowCaret(hwnd);

        return -1;
}


/*********************************************************************
 *
 *      WM_GETTEXT
 *
 */
static INT EDIT_WM_GetText(HWND hwnd, EDITSTATE *es, INT count, LPSTR text)
{
  INT len;

  if (es->text == NULL)  // the only case of failure i can imagine
    return 0;

  len = min(count, lstrlenA(es->text)+1); // determine length
  lstrcpynA(text, es->text, len);       // copy as much as possible
  return len;
}


/*********************************************************************
 *
 *      EDIT_HScroll_Hack
 *
 *      16 bit notepad needs this.  Actually it is not _our_ hack,
 *      it is notepad's.  Notepad is sending us scrollbar messages with
 *      undocumented parameters without us even having a scrollbar ... !?!?
 *
 */
static LRESULT EDIT_HScroll_Hack(HWND hwnd, EDITSTATE *es, INT action, INT pos, HWND scroll_bar)
{
        INT dx = 0;
        INT fw = es->format_rect.right - es->format_rect.left;
        LRESULT ret = 0;

        if (!(es->flags & EF_HSCROLL_HACK)) {
                //ERR_(edit)("hacked WM_HSCROLL handler invoked\n");
                //ERR_(edit)("      if you are _not_ running 16 bit notepad, please report\n");
                //ERR_(edit)("      (this message is only displayed once per edit control)\n");
                es->flags |= EF_HSCROLL_HACK;
        }

        switch (action) {
        case SB_LINELEFT:
                if (es->x_offset)
                        dx = -es->char_width;
                break;
        case SB_LINERIGHT:
                if (es->x_offset < es->text_width)
                        dx = es->char_width;
                break;
        case SB_PAGELEFT:
                if (es->x_offset)
                        dx = -fw / HSCROLL_FRACTION / es->char_width * es->char_width;
                break;
        case SB_PAGERIGHT:
                if (es->x_offset < es->text_width)
                        dx = fw / HSCROLL_FRACTION / es->char_width * es->char_width;
                break;
        case SB_LEFT:
                if (es->x_offset)
                        dx = -es->x_offset;
                break;
        case SB_RIGHT:
                if (es->x_offset < es->text_width)
                        dx = es->text_width - es->x_offset;
                break;
        case SB_THUMBTRACK:
                es->flags |= EF_HSCROLL_TRACK;
                dx = pos * es->text_width / 100 - es->x_offset;
                break;
        case SB_THUMBPOSITION:
                es->flags &= ~EF_HSCROLL_TRACK;
                if (!(dx = pos * es->text_width / 100 - es->x_offset))
                        EDIT_NOTIFY_PARENT(hwnd, EN_HSCROLL);
                break;
        case SB_ENDSCROLL:
                break;

        default:
                //ERR_(edit)("undocumented (hacked) WM_HSCROLL parameter, please report\n");
                return 0;
        }
        if (dx)
                EDIT_EM_LineScroll(hwnd, es, dx, 0);
        return ret;
}


/*********************************************************************
 *
 *      WM_HSCROLL
 *
 */
static LRESULT EDIT_WM_HScroll(HWND hwnd, EDITSTATE *es, INT action, INT pos, HWND scroll_bar)
{
        INT dx;
        INT fw;

        if (!(es->style & ES_MULTILINE))
                return 0;

        if (!(es->style & ES_AUTOHSCROLL))
                return 0;

        if (!(es->style & WS_HSCROLL))
                return EDIT_HScroll_Hack(hwnd, es, action, pos, scroll_bar);

        dx = 0;
        fw = es->format_rect.right - es->format_rect.left;
        switch (action) {
        case SB_LINELEFT:
                if (es->x_offset)
                        dx = -es->char_width;
                break;
        case SB_LINERIGHT:
                if (es->x_offset < es->text_width)
                        dx = es->char_width;
                break;
        case SB_PAGELEFT:
                if (es->x_offset)
                        dx = -fw / HSCROLL_FRACTION / es->char_width * es->char_width;
                break;
        case SB_PAGERIGHT:
                if (es->x_offset < es->text_width)
                        dx = fw / HSCROLL_FRACTION / es->char_width * es->char_width;
                break;
        case SB_LEFT:
                if (es->x_offset)
                        dx = -es->x_offset;
                break;
        case SB_RIGHT:
                if (es->x_offset < es->text_width)
                        dx = es->text_width - es->x_offset;
                break;
        case SB_THUMBTRACK:
                es->flags |= EF_HSCROLL_TRACK;
                dx = pos - es->x_offset;
                break;
        case SB_THUMBPOSITION:
                es->flags &= ~EF_HSCROLL_TRACK;
                if (!(dx = pos - es->x_offset)) {
                        SetScrollPos(hwnd, SB_HORZ, pos, TRUE);
                        EDIT_NOTIFY_PARENT(hwnd, EN_HSCROLL);
                }
                break;
        case SB_ENDSCROLL:
                break;

        default:
                //ERR_(edit)("undocumented WM_HSCROLL parameter, please report\n");
                return 0;
        }
        if (dx)
                EDIT_EM_LineScroll(hwnd, es, dx, 0);
        return 0;
}


/*********************************************************************
 *
 *      EDIT_CheckCombo
 *
 */
static BOOL EDIT_CheckCombo(HWND hwnd, EDITSTATE *es, UINT msg, INT key, DWORD key_data)
{
   HWND hLBox = es->hwndListBox;
   HWND hCombo;
   BOOL bDropped;
   int  nEUI;

   if (!hLBox)
      return FALSE;

   hCombo   = GetParent(hwnd);
   bDropped = TRUE;
   nEUI     = 0;

   //TRACE_(combo)("[%04x]: handling msg %04x (%04x)\n",
   //    		     wnd->hwndSelf, (UINT16)msg, (UINT16)key);

   if (key == VK_UP || key == VK_DOWN)
   {
      if (SendMessageA(hCombo, CB_GETEXTENDEDUI, 0, 0))
         nEUI = 1;

      if (msg == WM_KEYDOWN || nEUI)
          bDropped = (BOOL)SendMessageA(hCombo, CB_GETDROPPEDSTATE, 0, 0);
   }

   switch (msg)
   {
      case WM_KEYDOWN:
         if (!bDropped && nEUI && (key == VK_UP || key == VK_DOWN))
         {
            /* make sure ComboLBox pops up */
            SendMessageA(hCombo, CB_SETEXTENDEDUI, FALSE, 0);
            key = VK_F4;
            nEUI = 2;
         }

         SendMessageA(hLBox, WM_KEYDOWN, (WPARAM)key, 0);
         break;

      case WM_SYSKEYDOWN: /* Handle Alt+up/down arrows */
         if (nEUI)
            SendMessageA(hCombo, CB_SHOWDROPDOWN, bDropped ? FALSE : TRUE, 0);
         else
            SendMessageA(hLBox, WM_KEYDOWN, (WPARAM)VK_F4, 0);
         break;
   }

   if(nEUI == 2)
      SendMessageA(hCombo, CB_SETEXTENDEDUI, TRUE, 0);

   return TRUE;
}

/*********************************************************************
 *
 *      WM_KEYDOWN
 *
 *      Handling of special keys that don't produce a WM_CHAR
 *      (i.e. non-printable keys) & Backspace & Delete
 *
 */
static LRESULT EDIT_WM_KeyDown(HWND hwnd, EDITSTATE *es, INT key, DWORD key_data)
{
        BOOL shift;
        BOOL control;

        if (GetKeyState(VK_MENU) & 0x8000)
                return 0;

        shift = GetKeyState(VK_SHIFT) & 0x8000;
        control = GetKeyState(VK_CONTROL) & 0x8000;

        switch (key) {
        case VK_F4:
        case VK_UP:
                if (EDIT_CheckCombo(hwnd,es, WM_KEYDOWN, key, key_data) || key == VK_F4)
                        break;
                /* fall through */
        case VK_LEFT:
                if ((es->style & ES_MULTILINE) && (key == VK_UP))
                        EDIT_MoveUp_ML(hwnd, es, shift);
                else
                        if (control)
                                EDIT_MoveWordBackward(hwnd, es, shift);
                        else
                                EDIT_MoveBackward(hwnd, es, shift);
                break;
        case VK_DOWN:
                if (EDIT_CheckCombo(hwnd,es, WM_KEYDOWN, key, key_data))
                        break;
                /* fall through */
        case VK_RIGHT:
                if ((es->style & ES_MULTILINE) && (key == VK_DOWN))
                        EDIT_MoveDown_ML(hwnd, es, shift);
                else if (control)
                        EDIT_MoveWordForward(hwnd, es, shift);
                else
                        EDIT_MoveForward(hwnd, es, shift);
                break;
        case VK_HOME:
                EDIT_MoveHome(hwnd, es, shift);
                break;
        case VK_END:
                EDIT_MoveEnd(hwnd, es, shift);
                break;
        case VK_PRIOR:
                if (es->style & ES_MULTILINE)
                        EDIT_MovePageUp_ML(hwnd, es, shift);
		else
			EDIT_CheckCombo(hwnd, es, WM_KEYDOWN, key, key_data);
                break;
        case VK_NEXT:
                if (es->style & ES_MULTILINE)
                        EDIT_MovePageDown_ML(hwnd, es, shift);
		else
			EDIT_CheckCombo(hwnd, es, WM_KEYDOWN, key, key_data);
                break;
        case VK_DELETE:
                if (!(es->style & ES_READONLY) && !(shift && control)) {
                        if (es->selection_start != es->selection_end) {
                                if (shift)
                                        EDIT_WM_Cut(hwnd, es);
                                else
                                        EDIT_WM_Clear(hwnd, es);
                        } else {
                                if (shift) {
                                        /* delete character left of caret */
                                        EDIT_EM_SetSel(hwnd, es, -1, 0, FALSE);
                                        EDIT_MoveBackward(hwnd, es, TRUE);
                                        EDIT_WM_Clear(hwnd, es);
                                } else if (control) {
                                        /* delete to end of line */
                                        EDIT_EM_SetSel(hwnd, es, -1, 0, FALSE);
                                        EDIT_MoveEnd(hwnd, es, TRUE);
                                        EDIT_WM_Clear(hwnd, es);
                                } else {
                                        /* delete character right of caret */
                                        EDIT_EM_SetSel(hwnd, es, -1, 0, FALSE);
                                        EDIT_MoveForward(hwnd, es, TRUE);
                                        EDIT_WM_Clear(hwnd, es);
                                }
                        }
                }
                break;
        case VK_INSERT:
                if (shift) {
                        if (!(es->style & ES_READONLY))
                                EDIT_WM_Paste(hwnd, es);
                } else if (control)
                        EDIT_WM_Copy(hwnd, es);
                break;
        case VK_RETURN:
            /* If the edit doesn't want the return send a message to the default object */
            if(!(es->style & ES_WANTRETURN))
            {
                HWND hwndParent = GetParent(hwnd);
                DWORD dw = SendMessageA( hwndParent, DM_GETDEFID, 0, 0 );
                if (HIWORD(dw) == DC_HASDEFID)
                {
                    SendMessageA( hwndParent, WM_COMMAND,
                                  MAKEWPARAM( LOWORD(dw), BN_CLICKED ),
                              (LPARAM)GetDlgItem( hwndParent, LOWORD(dw) ) );
                }
            }
            break;
        }
        return 0;
}


/*********************************************************************
 *
 *      WM_KILLFOCUS
 *
 */
static LRESULT EDIT_WM_KillFocus(HWND hwnd, EDITSTATE *es, HWND window_getting_focus)
{
        es->flags &= ~EF_FOCUSED;
        DestroyCaret();
        if(!(es->style & ES_NOHIDESEL))
                EDIT_InvalidateText(hwnd, es, es->selection_start, es->selection_end);
        EDIT_NOTIFY_PARENT(hwnd, EN_KILLFOCUS);
        return 0;
}


/*********************************************************************
 *
 *      WM_LBUTTONDBLCLK
 *
 *      The caret position has been set on the WM_LBUTTONDOWN message
 *
 */
static LRESULT EDIT_WM_LButtonDblClk(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y)
{
        INT s;
        INT e = es->selection_end;
        INT l;
        INT li;
        INT ll;

        if (!(es->flags & EF_FOCUSED))
                return 0;

        l = EDIT_EM_LineFromChar(hwnd, es, e);
        li = EDIT_EM_LineIndex(hwnd, es, l);
        ll = EDIT_EM_LineLength(hwnd, es, e);
        s = li + EDIT_CallWordBreakProc (hwnd, es, li, e - li, ll, WB_LEFT);
        e = li + EDIT_CallWordBreakProc(hwnd, es, li, e - li, ll, WB_RIGHT);
        EDIT_EM_SetSel(hwnd, es, s, e, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
        return 0;
}


/*********************************************************************
 *
 *      WM_LBUTTONDOWN
 *
 */
static LRESULT EDIT_WM_LButtonDown(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y)
{
        INT e;
        BOOL after_wrap;

        if (!(es->flags & EF_FOCUSED))
          SetFocus(hwnd);

        es->bCaptureState = TRUE;
        SetCapture(hwnd);
        EDIT_ConfinePoint(hwnd, es, &x, &y);
        e = EDIT_CharFromPos(hwnd, es, x, y, &after_wrap);
        EDIT_EM_SetSel(hwnd, es, (keys & MK_SHIFT) ? es->selection_start : e, e, after_wrap);
        EDIT_EM_ScrollCaret(hwnd, es);
        es->region_posx = es->region_posy = 0;
        SetTimer(hwnd, 0, 100, NULL);
        return 0;
}


/*********************************************************************
 *
 *      WM_LBUTTONUP
 *
 */
static LRESULT EDIT_WM_LButtonUp(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y)
{
  if (es->bCaptureState)
  {
    KillTimer(hwnd,0);
    ReleaseCapture();
  }
  es->bCaptureState = FALSE;

  return 0;
}

static LRESULT EDIT_WM_CaptureChanged(HWND hwnd,EDITSTATE *es)
{
  if (es->bCaptureState) KillTimer(hwnd,0);
  es->bCaptureState = FALSE;

  return 0;
}

/*********************************************************************
 *
 *      WM_MOUSEMOVE
 *
 */
static LRESULT EDIT_WM_MouseMove(HWND hwnd, EDITSTATE *es, DWORD keys, INT x, INT y)
{
        INT e;
        BOOL after_wrap;
        INT prex, prey;

        if (!es->bCaptureState) return 0;

        /*
         *      FIXME: gotta do some scrolling if outside client
         *              area.  Maybe reset the timer ?
         */
        prex = x; prey = y;
        EDIT_ConfinePoint(hwnd, es, &x, &y);
        es->region_posx = (prex < x) ? -1 : ((prex > x) ? 1 : 0);
        es->region_posy = (prey < y) ? -1 : ((prey > y) ? 1 : 0);
        e = EDIT_CharFromPos(hwnd, es, x, y, &after_wrap);
        EDIT_EM_SetSel(hwnd, es, es->selection_start, e, after_wrap);

        return 0;
}


/*********************************************************************
 *
 *      WM_NCCREATE
 *
 */
static LRESULT EDIT_WM_NCCreate(HWND hwnd, LPCREATESTRUCTA cs)
{
        EDITSTATE *es;

        if (!(es = (EDITSTATE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*es))))
                return FALSE;
        SetInfoPtr(hwnd,(DWORD)es);

       /*
        *      Note: since the EDITSTATE has not been fully initialized yet,
        *            we can't use any API calls that may send
        *            WM_XXX messages before WM_NCCREATE is completed.
        */

        if (!(es->heap = HeapCreate(0, 0x10000, 0)))
                return FALSE;
        es->style = cs->style;

        es->bEnableState = !(cs->style & WS_DISABLED);

        /*
         * In Win95 look and feel, the WS_BORDER style is replaced by the
         * WS_EX_CLIENTEDGE style for the edit control. This gives the edit
         * control a non client area.
         */
        if (es->style & WS_BORDER)
        {
          es->style      &= ~WS_BORDER;
          SetWindowLongA(hwnd,GWL_STYLE,GetWindowLongA(hwnd,GWL_STYLE) & ~WS_BORDER);
          SetWindowLongA(hwnd,GWL_EXSTYLE,GetWindowLongA(hwnd,GWL_EXSTYLE) | WS_EX_CLIENTEDGE);
        }

	if (es->style & ES_COMBO)
	   es->hwndListBox = GetDlgItem(cs->hwndParent, ID_CB_LISTBOX);

        if (es->style & ES_MULTILINE) {
                es->buffer_size = BUFSTART_MULTI;
                es->buffer_limit = BUFLIMIT_MULTI;
                if (es->style & WS_VSCROLL)
                        es->style |= ES_AUTOVSCROLL;
                if (es->style & WS_HSCROLL)
                        es->style |= ES_AUTOHSCROLL;
                es->style &= ~ES_PASSWORD;
                if ((es->style & ES_CENTER) || (es->style & ES_RIGHT)) {
                        if (es->style & ES_RIGHT)
                                es->style &= ~ES_CENTER;
                        es->style &= ~WS_HSCROLL;
                        es->style &= ~ES_AUTOHSCROLL;
                }

                /* FIXME: for now, all multi line controls are AUTOVSCROLL */
                es->style |= ES_AUTOVSCROLL;
        } else {
                es->buffer_size = BUFSTART_SINGLE;
                es->buffer_limit = BUFLIMIT_SINGLE;
                es->style &= ~ES_CENTER;
                es->style &= ~ES_RIGHT;
                es->style &= ~WS_HSCROLL;
                es->style &= ~WS_VSCROLL;
                es->style &= ~ES_AUTOVSCROLL;
                es->style &= ~ES_WANTRETURN;
                if (es->style & ES_UPPERCASE) {
                        es->style &= ~ES_LOWERCASE;
                        es->style &= ~ES_NUMBER;
                } else if (es->style & ES_LOWERCASE)
                        es->style &= ~ES_NUMBER;
                if (es->style & ES_PASSWORD)
                        es->password_char = '*';

                /* FIXME: for now, all single line controls are AUTOHSCROLL */
                es->style |= ES_AUTOHSCROLL;
        }
        if (!(es->text = (char*)HeapAlloc(es->heap, 0, es->buffer_size + 1)))
                return FALSE;
        es->buffer_size = HeapSize(es->heap, 0, es->text) - 1;
        if (!(es->undo_text = (char*)HeapAlloc(es->heap, 0, es->buffer_size + 1)))
                return FALSE;
        es->undo_buffer_size = HeapSize(es->heap, 0, es->undo_text) - 1;
        *es->text = '\0';
        if (es->style & ES_MULTILINE)
                if (!(es->first_line_def = (LINEDEF*)HeapAlloc(es->heap, HEAP_ZERO_MEMORY, sizeof(LINEDEF))))
                        return FALSE;
        es->line_count = 1;

        return TRUE;
}

static VOID EDIT_Draw(HWND hwnd,EDITSTATE *es,HDC hdc,BOOL eraseBkGnd)
{
  INT i;
  HFONT old_font = 0;
  RECT rc;
  RECT rcLine;
  RECT rcRgn;
  BOOL rev = es->bEnableState && ((es->flags & EF_FOCUSED) || (es->style & ES_NOHIDESEL));

  HideCaret(hwnd);

  if (eraseBkGnd)
  {
    HBRUSH brush;

    if (!es->bEnableState || (es->style & ES_READONLY))
      brush = (HBRUSH)EDIT_SEND_CTLCOLORSTATIC(hwnd, hdc);
    else
      brush = (HBRUSH)EDIT_SEND_CTLCOLOR(hwnd, hdc);

    if (!brush)
      brush = (HBRUSH)GetStockObject(WHITE_BRUSH);

    GetClientRect(hwnd, &rc);
    /*
     *      FIXME:  specs say that we should UnrealizeObject() the brush,
     *              but the specs of UnrealizeObject() say that we shouldn't
     *              unrealize a stock object.  The default brush that
     *              DefWndProc() returns is ... a stock object.
     */
    FillRect(hdc, &rc, brush);
  }

  if(es->style & WS_BORDER)
  {
    GetClientRect(hwnd, &rc);
    if(es->style & ES_MULTILINE)
    {
      if(es->style & WS_HSCROLL) rc.bottom++;
      if(es->style & WS_VSCROLL) rc.right++;
    }
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
  }

  IntersectClipRect(hdc, es->format_rect.left,
                        es->format_rect.top,
                        es->format_rect.right,
                        es->format_rect.bottom);
  if (es->style & ES_MULTILINE)
  {
    GetClientRect(hwnd, &rc);
    IntersectClipRect(hdc, rc.left, rc.top, rc.right, rc.bottom);
  }
  if (es->font) old_font = SelectObject(hdc, es->font);
  if (!es->bEnableState || (es->style & ES_READONLY))
    EDIT_SEND_CTLCOLORSTATIC(hwnd, hdc);
  else
    EDIT_SEND_CTLCOLOR(hwnd, hdc);
  if (!es->bEnableState)
    SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
  GetClipBox(hdc, &rcRgn);
  if (es->style & ES_MULTILINE)
  {
    INT vlc = (es->format_rect.bottom - es->format_rect.top) / es->line_height;

    for (i = es->y_offset ; i <= MIN(es->y_offset + vlc, es->y_offset + es->line_count - 1) ; i++)
    {
      EDIT_GetLineRect(hwnd,hdc, es, i, 0, -1, &rcLine);
      if (IntersectRect(&rc, &rcRgn, &rcLine))
        EDIT_PaintLine(hwnd, es, hdc, i, rev);
    }
  } else
  {
    EDIT_GetLineRect(hwnd,hdc, es, 0, 0, -1, &rcLine);
    if (IntersectRect(&rc, &rcRgn, &rcLine))
      EDIT_PaintLine(hwnd, es, hdc, 0, rev);
  }
  if (es->font) SelectObject(hdc, old_font);
  if (es->flags & EF_FOCUSED)
    EDIT_SetCaretPos(hwnd, es, es->selection_end,es->flags & EF_AFTER_WRAP);

  ShowCaret(hwnd);
}

static VOID EDIT_Refresh(HWND hwnd,EDITSTATE *es,BOOL useCache)
{
  HDC hdc,hdcCompatible;
  HBITMAP bitmap,oldbmp;
  RECT rect;

  if (es->flags & EF_UPDATE)
  {
    //CB: don't reset flag or EN_CHANGE is lost!
    //es->flags &= ~EF_UPDATE;
    EDIT_NOTIFY_PARENT(hwnd,EN_UPDATE);
  }

  if (!IsWindowVisible(hwnd)) return;

  if (!useCache)
  {
    InvalidateRect(hwnd,NULL,TRUE);
    return;
  }

  //CB: original control redraws many times, cache drawing
  HideCaret(hwnd);
  GetClientRect(hwnd,&rect);
  hdc = GetDC(hwnd);
  hdcCompatible = CreateCompatibleDC(hdc);
  bitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
  oldbmp = SelectObject(hdcCompatible,bitmap);
  EDIT_Draw(hwnd,es,hdcCompatible,TRUE);
  SelectClipRgn(hdcCompatible,0);
  BitBlt(hdc,0,0,rect.right,rect.bottom,hdcCompatible,0,0,SRCCOPY);
  SelectObject(hdcCompatible,oldbmp);
  DeleteObject(bitmap);
  DeleteDC(hdcCompatible);
  ReleaseDC(hwnd,hdc);
  ShowCaret(hwnd);
}

/*********************************************************************
 *
 *      WM_PAINT
 *
 */
static void EDIT_WM_Paint(HWND hwnd, EDITSTATE *es,WPARAM wParam)
{
  PAINTSTRUCT ps;
  HDC hdc;

  if (!wParam) hdc = BeginPaint(hwnd, &ps);
  else hdc = (HDC) wParam;

  EDIT_Draw(hwnd,es,hdc,FALSE);

  if (!wParam) EndPaint(hwnd, &ps);
}


/*********************************************************************
 *
 *      WM_PASTE
 *
 */
static void EDIT_WM_Paste(HWND hwnd, EDITSTATE *es)
{
        HGLOBAL hsrc;
        LPSTR src;

        OpenClipboard(hwnd);
        hsrc = GetClipboardData(CF_TEXT);
        if (hsrc) {
                src = (LPSTR)GlobalLock(hsrc);
                EDIT_EM_ReplaceSel(hwnd, es, TRUE, src);
                GlobalUnlock(hsrc);

                if (es->flags & EF_UPDATE) {
                        es->flags &= ~EF_UPDATE;
                        EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
                }
        }
        CloseClipboard();
}


/*********************************************************************
 *
 *      WM_SETFOCUS
 *
 */
static void EDIT_WM_SetFocus(HWND hwnd, EDITSTATE *es, HWND window_losing_focus)
{
        es->flags |= EF_FOCUSED;
        CreateCaret(hwnd, 0, 2, es->line_height);
        EDIT_SetCaretPos(hwnd, es, es->selection_end,
                         es->flags & EF_AFTER_WRAP);
        if(!(es->style & ES_NOHIDESEL))
                EDIT_InvalidateText(hwnd, es, es->selection_start, es->selection_end);
        ShowCaret(hwnd);
        EDIT_NOTIFY_PARENT(hwnd, EN_SETFOCUS);
}

/*********************************************************************
 *
 *      WM_SETFONT
 *
 * With Win95 look the margins are set to default font value unless
 * the system font (font == 0) is being set, in which case they are left
 * unchanged.
 *
 */
static void EDIT_WM_SetFont(HWND hwnd, EDITSTATE *es, HFONT font, BOOL redraw)
{
        TEXTMETRICA tm;
        HDC dc;
        HFONT old_font = 0;
        RECT r;

        dc = GetDC(hwnd);
        if (font)
                old_font = SelectObject(dc, font);
        if (!GetTextMetricsA(dc, &tm))
        {
          if (font) SelectObject(dc,old_font);
          ReleaseDC(hwnd,dc);

          return;
        }
        es->font = font;
        es->line_height = tm.tmHeight;

        es->char_width = tm.tmAveCharWidth;
        if (font)
                SelectObject(dc, old_font);
        ReleaseDC(hwnd, dc);
        if (font)
                EDIT_EM_SetMargins(hwnd, es, EC_LEFTMARGIN | EC_RIGHTMARGIN,
                                   EC_USEFONTINFO, EC_USEFONTINFO);
        /* Force the recalculation of the format rect for each font change */
        GetClientRect(hwnd, &r);
        EDIT_SetRectNP(hwnd, es, &r);
        if (es->style & ES_MULTILINE)
                EDIT_BuildLineDefs_ML(hwnd,es);

        if (redraw)
                EDIT_Refresh(hwnd,es,FALSE);
        if (es->flags & EF_FOCUSED) {
                DestroyCaret();
                CreateCaret(hwnd, 0, 2, es->line_height);
                EDIT_SetCaretPos(hwnd, es, es->selection_end,
                                 es->flags & EF_AFTER_WRAP);
                ShowCaret(hwnd);
        }
}


/*********************************************************************
 *
 *      WM_SETTEXT
 *
 * NOTES
 *  For multiline controls (ES_MULTILINE), reception of WM_SETTEXT triggers:
 *  The modified flag is reset. No notifications are sent.
 *
 *  For single-line controls, reception of WM_SETTEXT triggers:
 *  The modified flag is reset. EN_UPDATE and EN_CHANGE notifications are sent.
 *
 */
static void EDIT_WM_SetText(HWND hwnd, EDITSTATE *es, LPCSTR text)
{
#ifdef __WIN32OS2__
        //SvL: Acrobat Distiller keeps on sending WM_SETTEXT with the same
        //     string in responds to a WM_COMMAND with EN_UPDATE -> stack
        //     overflow (TODO: Need to check if this behaviour is correct compared to NT)
        if(text && es->text) {
		if(!strcmp(es->text, text)) {
			return;
		}
        }
#endif
        es->selection_start = 0;
        es->selection_end = lstrlenA(es->text);
        if (es->flags & EF_FOCUSED)
          EDIT_SetCaretPos(hwnd, es, es->selection_end, FALSE);

        if (text) {
                //TRACE_(edit)("\t'%s'\n", text);
                EDIT_EM_ReplaceSel(hwnd, es, FALSE, text);
        } else {
                //TRACE_(edit)("\t<NULL>\n");
                EDIT_EM_ReplaceSel(hwnd, es, FALSE, "");
        }
        es->x_offset = 0;
        if (es->style & ES_MULTILINE) {
                es->flags &= ~EF_UPDATE;
        } else {
                es->flags |= EF_UPDATE;
        }
        es->flags &= ~EF_MODIFIED;
        EDIT_EM_SetSel(hwnd, es, 0, 0, FALSE);
        EDIT_EM_ScrollCaret(hwnd, es);
        EDIT_UpdateScrollBars(hwnd,es,TRUE,TRUE);
        if (es->flags & EF_UPDATE)
        {
          EDIT_NOTIFY_PARENT(hwnd,EN_UPDATE);
          /* EN_CHANGE notification need to be sent too
             Windows doc says it's only done after the display,
             the doc is WRONG. EN_CHANGE notification is sent
             while processing WM_SETTEXT */
          EDIT_NOTIFY_PARENT(hwnd, EN_CHANGE);
          es->flags &= ~EF_UPDATE;
        }
}


/*********************************************************************
 *
 *      WM_SIZE
 *
 */
static void EDIT_WM_Size(HWND hwnd, EDITSTATE *es, UINT action, INT width, INT height)
{
        if ((action == SIZE_MAXIMIZED) || (action == SIZE_RESTORED)) {
                RECT rc;
                SetRect(&rc, 0, 0, width, height);
                EDIT_SetRectNP(hwnd, es, &rc);
                EDIT_Refresh(hwnd,es,FALSE);
        }
}


/*********************************************************************
 *
 *      WM_SYSKEYDOWN
 *
 */
static LRESULT EDIT_WM_SysKeyDown(HWND hwnd, EDITSTATE *es, INT key, DWORD key_data)
{
        if ((key == VK_BACK) && (key_data & 0x2000)) {
                if (EDIT_EM_CanUndo(hwnd, es))
                        EDIT_EM_Undo(hwnd, es);
                return 0;
        } else if ((key == VK_UP) || (key == VK_DOWN))
        {
                if (EDIT_CheckCombo(hwnd,es, WM_SYSKEYDOWN, key, key_data))
                        return 0;
         }
        return DefWindowProcA(hwnd, WM_SYSKEYDOWN, (WPARAM)key, (LPARAM)key_data);
}


/*********************************************************************
 *
 *      WM_TIMER
 *
 */
static void EDIT_WM_Timer(HWND hwnd, EDITSTATE *es, INT id, TIMERPROC timer_proc)
{
        if (es->region_posx < 0) {
                EDIT_MoveBackward(hwnd, es, TRUE);
        } else if (es->region_posx > 0) {
                EDIT_MoveForward(hwnd, es, TRUE);
        }

        if (!(es->style & ES_MULTILINE)) return;

        if (es->region_posy < 0)
        {
          EDIT_MoveUp_ML(hwnd,es,TRUE);
        } else if (es->region_posy > 0)
        {
          EDIT_MoveDown_ML(hwnd,es,TRUE);
        }
}


/*********************************************************************
 *
 *      EDIT_VScroll_Hack
 *
 *      16 bit notepad needs this.  Actually it is not _our_ hack,
 *      it is notepad's.  Notepad is sending us scrollbar messages with
 *      undocumented parameters without us even having a scrollbar ... !?!?
 *
 */
static LRESULT EDIT_VScroll_Hack(HWND hwnd, EDITSTATE *es, INT action, INT pos, HWND scroll_bar)
{
        INT dy = 0;
        LRESULT ret = 0;

        if (!(es->flags & EF_VSCROLL_HACK)) {
                //ERR_(edit)("hacked WM_VSCROLL handler invoked\n");
                //ERR_(edit)("      if you are _not_ running 16 bit notepad, please report\n");
                //ERR_(edit)("      (this message is only displayed once per edit control)\n");
                es->flags |= EF_VSCROLL_HACK;
        }

        switch (action) {
        case SB_LINEUP:
        case SB_LINEDOWN:
        case SB_PAGEUP:
        case SB_PAGEDOWN:
                EDIT_EM_Scroll(hwnd, es, action);
                return 0;
        case SB_TOP:
                dy = -es->y_offset;
                break;
        case SB_BOTTOM:
                dy = es->line_count - 1 - es->y_offset;
                break;
        case SB_THUMBTRACK:
                es->flags |= EF_VSCROLL_TRACK;
                dy = (pos * (es->line_count - 1) + 50) / 100 - es->y_offset;
                break;
        case SB_THUMBPOSITION:
                es->flags &= ~EF_VSCROLL_TRACK;
                if (!(dy = (pos * (es->line_count - 1) + 50) / 100 - es->y_offset))
                        EDIT_NOTIFY_PARENT(hwnd, EN_VSCROLL);
                break;
        case SB_ENDSCROLL:
                break;

        default:
                //ERR_(edit)("undocumented (hacked) WM_VSCROLL parameter, please report\n");
                return 0;
        }
        if (dy)
                EDIT_EM_LineScroll(hwnd, es, 0, dy);
        return ret;
}


/*********************************************************************
 *
 *      WM_VSCROLL
 *
 */
static LRESULT EDIT_WM_VScroll(HWND hwnd, EDITSTATE *es, INT action, INT pos, HWND scroll_bar)
{
        INT dy;

        if (!(es->style & ES_MULTILINE))
                return 0;

        if (!(es->style & ES_AUTOVSCROLL))
                return 0;

        if (!(es->style & WS_VSCROLL))
                return EDIT_VScroll_Hack(hwnd, es, action, pos, scroll_bar);

        dy = 0;
        switch (action) {
        case SB_LINEUP:
        case SB_LINEDOWN:
        case SB_PAGEUP:
        case SB_PAGEDOWN:
                EDIT_EM_Scroll(hwnd, es, action);
                return 0;

        case SB_TOP:
                dy = -es->y_offset;
                break;
        case SB_BOTTOM:
                dy = es->line_count - 1 - es->y_offset;
                break;
        case SB_THUMBTRACK:
                es->flags |= EF_VSCROLL_TRACK;
                dy = pos - es->y_offset;
                break;
        case SB_THUMBPOSITION:
                es->flags &= ~EF_VSCROLL_TRACK;
                if (!(dy = pos - es->y_offset)) {
                        SetScrollPos(hwnd, SB_VERT, pos, TRUE);
                        EDIT_NOTIFY_PARENT(hwnd, EN_VSCROLL);
                }
                break;
        case SB_ENDSCROLL:
                break;

        default:
                //ERR_(edit)("undocumented WM_VSCROLL action %d, please report\n",
                //        action);
                return 0;
        }
        if (dy)
                EDIT_EM_LineScroll(hwnd, es, 0, dy);
        return 0;
}

static LRESULT EDIT_WM_MouseWheel(HWND hwnd,EDITSTATE *es,WPARAM wParam,LPARAM lParam)
{
  short gcWheelDelta = 0;
  UINT pulScrollLines = 3;
  SystemParametersInfoW(SPI_GETWHEELSCROLLLINES,0, &pulScrollLines, 0);

  if (wParam & (MK_SHIFT | MK_CONTROL))
    return DefWindowProcA(hwnd,WM_MOUSEWHEEL, wParam, lParam);
  gcWheelDelta -= (short) HIWORD(wParam);
  if (abs(gcWheelDelta) >= WHEEL_DELTA && pulScrollLines)
  {
    int cLineScroll= (int) min((UINT) es->line_count, pulScrollLines);

    cLineScroll *= (gcWheelDelta / WHEEL_DELTA);
    return EDIT_EM_LineScroll(hwnd, es, 0, cLineScroll);
  }

  return 0;
}

BOOL EDIT_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(EDITCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_DBLCLKS;
    wndClass.lpfnWndProc   = (WNDPROC)EditWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(VOID*);
    wndClass.hCursor       = LoadCursorA(0,IDC_IBEAMA);
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = EDITCLASSNAME;

    return RegisterClassA(&wndClass);
}

BOOL EDIT_Unregister()
{
    if (GlobalFindAtomA(EDITCLASSNAME))
        return UnregisterClassA(EDITCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
