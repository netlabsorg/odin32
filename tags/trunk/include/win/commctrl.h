/*
 * Win32 common controls include file
 *
 * Copyright (C) 1999 Achim Hasenmueller
 *
 * Based on the work of the WINE group (www.winehq.com)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _H_COMMCTRL
#define _H_COMMCTRL

#include "windef.h"
#include "winuser.h"

/* common control styles */
#define CCS_TOP             0x00000001L
#define CCS_NOMOVEY         0x00000002L
#define CCS_BOTTOM          0x00000003L
#define CCS_NORESIZE        0x00000004L
#define CCS_NOPARENTALIGN   0x00000008L
#define CCS_ADJUSTABLE      0x00000020L
#define CCS_NODIVIDER       0x00000040L
#define CCS_VERT            0x00000080L
#define CCS_LEFT            (CCS_VERT|CCS_TOP)
#define CCS_RIGHT           (CCS_VERT|CCS_BOTTOM)
#define CCS_NOMOVEX         (CCS_VERT|CCS_NOMOVEY)


/* common control shared messages */
#define CCM_FIRST            0x2000

#define CCM_SETBKCOLOR       (CCM_FIRST+1)     /* lParam = bkColor */
#define CCM_SETCOLORSCHEME   (CCM_FIRST+2)
#define CCM_GETCOLORSCHEME   (CCM_FIRST+3)
#define CCM_GETDROPTARGET    (CCM_FIRST+4)
#define CCM_SETUNICODEFORMAT (CCM_FIRST+5)
#define CCM_GETUNICODEFORMAT (CCM_FIRST+6) 

/* common notification codes (WM_NOTIFY)*/
#define NM_FIRST                (0U-  0U)
#define NM_LAST                 (0U- 99U)
#define NM_OUTOFMEMORY          (NM_FIRST-1)
#define NM_CLICK                (NM_FIRST-2)
#define NM_DBLCLK               (NM_FIRST-3)
#define NM_RETURN               (NM_FIRST-4)
#define NM_RCLICK               (NM_FIRST-5)
#define NM_RDBLCLK              (NM_FIRST-6)
#define NM_SETFOCUS             (NM_FIRST-7)
#define NM_KILLFOCUS            (NM_FIRST-8)
#define NM_CUSTOMDRAW           (NM_FIRST-12)
#define NM_HOVER                (NM_FIRST-13)
#define NM_NCHITTEST            (NM_FIRST-14)
#define NM_KEYDOWN              (NM_FIRST-15)
#define NM_RELEASEDCAPTURE      (NM_FIRST-16)
#define NM_SETCURSOR            (NM_FIRST-17)
#define NM_CHAR                 (NM_FIRST-18)
#define NM_TOOLTIPSCREATED      (NM_FIRST-19)
#define NM_UPDOWN                NMUPDOWN

/* callback constants */
#define LPSTR_TEXTCALLBACKA    ((LPSTR)-1L)
#define LPSTR_TEXTCALLBACKW    ((LPWSTR)-1L)
#define LPSTR_TEXTCALLBACK WINELIB_NAME_AW(LPSTR_TEXTCALLBACK)

#define I_IMAGECALLBACK          (-1)
#define I_INDENTCALLBACK         (-1)
#define I_CHILDRENCALLBACK       (-1)


/* owner drawn types */
#define ODT_HEADER      100
#define ODT_TAB         101
#define ODT_LISTVIEW    102

/* common notification structures */
typedef struct tagNMTOOLTIPSCREATED
{
    NMHDR  hdr;
    HWND hwndToolTips;
} NMTOOLTIPSCREATED, *LPNMTOOLTIPSCREATED;


#ifndef CCSIZEOF_STRUCT
#define CCSIZEOF_STRUCT(name, member) \
    (((INT)((LPBYTE)(&((name*)0)->member)-((LPBYTE)((name*)0))))+ \
    sizeof(((name*)0)->member))
#endif

/* Custom Draw messages */

#define CDRF_DODEFAULT          0x0
#define CDRF_NEWFONT            0x00000002
#define CDRF_SKIPDEFAULT        0x00000004
#define CDRF_NOTIFYPOSTPAINT    0x00000010
#define CDRF_NOTIFYITEMDRAW     0x00000020
#define CDRF_NOTIFYSUBITEMDRAW  0x00000020
#define CDRF_NOTIFYPOSTERASE    0x00000040
/* #define CDRF_NOTIFYITEMERASE    0x00000080          obsolete ? */


/* drawstage flags */

#define CDDS_PREPAINT           1
#define CDDS_POSTPAINT          2
#define CDDS_PREERASE           3
#define CDDS_POSTERASE          4

#define CDDS_ITEM				0x00010000
#define CDDS_ITEMPREPAINT		(CDDS_ITEM | CDDS_PREPAINT)
#define CDDS_ITEMPOSTPAINT		(CDDS_ITEM | CDDS_POSTPAINT)
#define CDDS_ITEMPREERASE		(CDDS_ITEM | CDDS_PREERASE)
#define CDDS_ITEMPOSTERASE		(CDDS_ITEM | CDDS_POSTERASE)
#define CDDS_SUBITEM            0x00020000

/* itemState flags */

#define CDIS_SELECTED	 	0x0001
#define CDIS_GRAYED			0x0002
#define CDIS_DISABLED		0x0004
#define CDIS_CHECKED		0x0008
#define CDIS_FOCUS			0x0010
#define CDIS_DEFAULT		0x0020
#define CDIS_HOT			0x0040
#define CDIS_MARKED         0x0080
#define CDIS_INDETERMINATE  0x0100


typedef struct tagNMCUSTOMDRAWINFO
{
	NMHDR	hdr;
	DWORD	dwDrawStage;
	HDC	hdc;
	RECT	rc;
	DWORD	dwItemSpec; 
	UINT	uItemState;
	LPARAM	lItemlParam;
} NMCUSTOMDRAW, *LPNMCUSTOMDRAW;

typedef struct tagNMTTCUSTOMDRAW
{
    NMCUSTOMDRAW nmcd;
    UINT       uDrawFlags;
} NMTTCUSTOMDRAW, *LPNMTTCUSTOMDRAW;



/* ImageList */
struct _IMAGELIST;
typedef struct _IMAGELIST *HIMAGELIST;

#define CLR_NONE         0xFFFFFFFF
#define CLR_DEFAULT      0xFF000000
#define CLR_HILIGHT      CLR_DEFAULT

#define ILC_MASK         0x0001
#define ILC_COLOR        0x0000
#define ILC_COLORDDB     0x00FE
#define ILC_COLOR4       0x0004
#define ILC_COLOR8       0x0008
#define ILC_COLOR16      0x0010
#define ILC_COLOR24      0x0018
#define ILC_COLOR32      0x0020
#define ILC_PALETTE      0x0800  /* no longer supported by M$ */

#define ILD_NORMAL       0x0000
#define ILD_TRANSPARENT  0x0001
#define ILD_BLEND25      0x0002
#define ILD_BLEND50      0x0004
#define ILD_MASK         0x0010
#define ILD_IMAGE        0x0020
#define ILD_ROP          0x0040
#define ILD_OVERLAYMASK  0x0F00

#define ILD_SELECTED     ILD_BLEND50
#define ILD_FOCUS        ILD_BLEND25
#define ILD_BLEND        ILD_BLEND50

#define INDEXTOOVERLAYMASK(i)  ((i)<<8)

#define ILCF_MOVE        (0x00000000)
#define ILCF_SWAP        (0x00000001)


typedef struct _IMAGEINFO
{
    HBITMAP hbmImage;
    HBITMAP hbmMask;
    INT     Unused1;
    INT     Unused2;
    RECT    rcImage;
} IMAGEINFO;


typedef struct _IMAGELISTDRAWPARAMS
{
    DWORD       cbSize;
    HIMAGELIST  himl;
    INT       i;
    HDC       hdcDst;
    INT       x;
    INT       y;
    INT       cx;
    INT       cy;
    INT       xBitmap;  /* x offest from the upperleft of bitmap */
    INT       yBitmap;  /* y offset from the upperleft of bitmap */
    COLORREF    rgbBk;
    COLORREF    rgbFg;
    UINT      fStyle;
    DWORD       dwRop;
} IMAGELISTDRAWPARAMS, *LPIMAGELISTDRAWPARAMS;

 
INT      WINAPI ImageList_Add(HIMAGELIST,HBITMAP,HBITMAP);
INT      WINAPI ImageList_AddIcon (HIMAGELIST, HICON);
INT      WINAPI ImageList_AddMasked(HIMAGELIST,HBITMAP,COLORREF);
BOOL     WINAPI ImageList_BeginDrag(HIMAGELIST,INT,INT,INT);
BOOL     WINAPI ImageList_Copy(HIMAGELIST,INT,HIMAGELIST,INT,INT);
HIMAGELIST WINAPI ImageList_Create(INT,INT,UINT,INT,INT);
BOOL     WINAPI ImageList_Destroy(HIMAGELIST);
BOOL     WINAPI ImageList_DragEnter(HWND,INT,INT);
BOOL     WINAPI ImageList_DragLeave(HWND); 
BOOL     WINAPI ImageList_DragMove(INT,INT);
BOOL     WINAPI ImageList_DragShowNolock (BOOL);
BOOL     WINAPI ImageList_Draw(HIMAGELIST,INT,HDC,INT,INT,UINT);
BOOL     WINAPI ImageList_DrawEx(HIMAGELIST,INT,HDC,INT,INT,INT,
                                   INT,COLORREF,COLORREF,UINT);
BOOL     WINAPI ImageList_DrawIndirect(IMAGELISTDRAWPARAMS*); 
HIMAGELIST WINAPI ImageList_Duplicate(HIMAGELIST);
BOOL     WINAPI ImageList_EndDrag(VOID);
COLORREF   WINAPI ImageList_GetBkColor(HIMAGELIST);
HIMAGELIST WINAPI ImageList_GetDragImage(POINT*,POINT*);
HICON    WINAPI ImageList_GetIcon(HIMAGELIST,INT,UINT);
BOOL     WINAPI ImageList_GetIconSize(HIMAGELIST,INT*,INT*);
INT      WINAPI ImageList_GetImageCount(HIMAGELIST);
BOOL     WINAPI ImageList_GetImageInfo(HIMAGELIST,INT,IMAGEINFO*);
BOOL     WINAPI ImageList_GetImageRect(HIMAGELIST,INT,LPRECT);
HIMAGELIST WINAPI ImageList_LoadImageA(HINSTANCE,LPCSTR,INT,INT,
                                         COLORREF,UINT,UINT);
HIMAGELIST WINAPI ImageList_LoadImageW(HINSTANCE,LPCWSTR,INT,INT,
                                         COLORREF,UINT,UINT);
#define    ImageList_LoadImage WINELIB_NAME_AW(ImageList_LoadImage)
HIMAGELIST WINAPI ImageList_Merge(HIMAGELIST,INT,HIMAGELIST,INT,INT,INT);
#ifdef __IStream_INTREFACE_DEFINED__
HIMAGELIST WINAPI ImageList_Read(LPSTREAM);
#endif
BOOL     WINAPI ImageList_Remove(HIMAGELIST,INT);
BOOL     WINAPI ImageList_Replace(HIMAGELIST,INT,HBITMAP,HBITMAP);
INT      WINAPI ImageList_ReplaceIcon(HIMAGELIST,INT,HICON);
COLORREF   WINAPI ImageList_SetBkColor(HIMAGELIST,COLORREF);
BOOL     WINAPI ImageList_SetDragCursorImage(HIMAGELIST,INT,INT,INT);

BOOL     WINAPI ImageList_SetIconSize(HIMAGELIST,INT,INT);
BOOL     WINAPI ImageList_SetImageCount(HIMAGELIST,INT);
BOOL     WINAPI ImageList_SetOverlayImage(HIMAGELIST,INT,INT);
//BOOL     WINAPI ImageList_Write(HIMAGELIST, LPSTREAM);

#define ImageList_ExtractIcon(hi,himl,i) ImageList_GetIcon(himl,i,0)
#define ImageList_LoadBitmap(hi,lpbmp,cx,cGrow,crMask) \
  ImageList_LoadImage(hi,lpbmp,cx,cGrow,crMask,IMAGE_BITMAP,0)
#define ImageList_RemoveAll(himl) ImageList_Remove(himl,-1)




/* PROGRESS BAR CONTROL */

#define PROGRESS_CLASSA   "msctls_progress32"
#define PROGRESS_CLASSW  L"msctls_progress32"
#define PROGRESS_CLASS    WINELIB_NAME_AW(PROGRESS_CLASS)

#define PBM_SETRANGE        (WM_USER+1)
#define PBM_SETPOS          (WM_USER+2)
#define PBM_DELTAPOS        (WM_USER+3)
#define PBM_SETSTEP         (WM_USER+4)
#define PBM_STEPIT          (WM_USER+5)
#define PBM_SETRANGE32      (WM_USER+6)
#define PBM_GETRANGE        (WM_USER+7)
#define PBM_GETPOS          (WM_USER+8)
#define PBM_SETBARCOLOR     (WM_USER+9)
#define PBM_SETBKCOLOR      CCM_SETBKCOLOR

#define PBS_SMOOTH          0x01
#define PBS_VERTICAL        0x04

typedef struct
{
    INT iLow;
    INT iHigh;
} PBRANGE, *PPBRANGE;


/* StatusWindow */

#define STATUSCLASSNAME16	"msctls_statusbar"
#define STATUSCLASSNAMEA	"msctls_statusbar32"
/* Does not work. gcc creates 4 byte wide strings.
 * #define STATUSCLASSNAME32W	L"msctls_statusbar32"
 */
static const WCHAR	_scn32w[] = {
'm','s','c','t','l','s','_','s','t','a','t','u','s','b','a','r','3','2',0
};
#define STATUSCLASSNAMEW	_scn32w
#define STATUSCLASSNAME		WINELIB_NAME_AW(STATUSCLASSNAME)

#define SBT_NOBORDERS		0x0100
#define SBT_POPOUT		0x0200
#define SBT_RTLREADING		0x0400  /* not supported */
#define SBT_TOOLTIPS		0x0800
#define SBT_OWNERDRAW		0x1000

#define SBARS_SIZEGRIP		0x0100

#define SB_SETTEXTA		(WM_USER+1)
#define SB_SETTEXTW		(WM_USER+11)
#define SB_SETTEXT		WINELIB_NAME_AW(SB_SETTEXT)
#define SB_GETTEXTA		(WM_USER+2)
#define SB_GETTEXTW		(WM_USER+13)
#define SB_GETTEXT		WINELIB_NAME_AW(SB_GETTEXT)
#define SB_GETTEXTLENGTHA	(WM_USER+3)
#define SB_GETTEXTLENGTHW	(WM_USER+12)
#define SB_GETTEXTLENGTH	WINELIB_NAME_AW(SB_GETTEXTLENGTH)
#define SB_SETPARTS		(WM_USER+4)
#define SB_GETPARTS		(WM_USER+6)
#define SB_GETBORDERS		(WM_USER+7)
#define SB_SETMINHEIGHT		(WM_USER+8)
#define SB_SIMPLE		(WM_USER+9)
#define SB_GETRECT		(WM_USER+10)
#define SB_ISSIMPLE		(WM_USER+14)
#define SB_SETICON		(WM_USER+15)
#define SB_SETTIPTEXTA	(WM_USER+16)
#define SB_SETTIPTEXTW	(WM_USER+17)
#define SB_SETTIPTEXT		WINELIB_NAME_AW(SB_SETTIPTEXT)
#define SB_GETTIPTEXTA	(WM_USER+18)
#define SB_GETTIPTEXTW	(WM_USER+19)
#define SB_GETTIPTEXT		WINELIB_NAME_AW(SB_GETTIPTEXT)
#define SB_GETICON		(WM_USER+20)
#define SB_SETBKCOLOR		CCM_SETBKCOLOR   /* lParam = bkColor */
#define SB_GETUNICODEFORMAT	CCM_GETUNICODEFORMAT
#define SB_SETUNICODEFORMAT	CCM_SETUNICODEFORMAT

#define SBN_FIRST		(0U-880U)
#define SBN_LAST		(0U-899U)
#define SBN_SIMPLEMODECHANGE	(SBN_FIRST-0)

HWND WINAPI CreateStatusWindowA (INT, LPCSTR, HWND, UINT);
HWND WINAPI CreateStatusWindowW (INT, LPCWSTR, HWND, UINT);
#define CreateStatusWindow WINELIB_NAME_AW(CreateStatusWindow)
VOID WINAPI DrawStatusTextA (HDC, LPRECT, LPCSTR, UINT);
VOID WINAPI DrawStatusTextW (HDC, LPRECT, LPCWSTR, UINT);
#define DrawStatusText WINELIB_NAME_AW(DrawStatusText)
VOID WINAPI MenuHelp (UINT, WPARAM, LPARAM, HMENU,
                      HINSTANCE, HWND, LPUINT);


/* Toolbar */

#define TOOLBARCLASSNAME16      "ToolbarWindow" 
#define TOOLBARCLASSNAMEW     "ToolbarWindow32" 
#define TOOLBARCLASSNAMEA     "ToolbarWindow32" 
#define TOOLBARCLASSNAME WINELIB_NAME_AW(TOOLBARCLASSNAME)

#define CMB_MASKED              0x02 
 
#define TBSTATE_CHECKED         0x01 
#define TBSTATE_PRESSED         0x02 
#define TBSTATE_ENABLED         0x04 
#define TBSTATE_HIDDEN          0x08 
#define TBSTATE_INDETERMINATE   0x10 
#define TBSTATE_WRAP            0x20 
#define TBSTATE_ELLIPSES        0x40
#define TBSTATE_MARKED          0x80 
 
#define TBSTYLE_BUTTON          0x00 
#define TBSTYLE_SEP             0x01 
#define TBSTYLE_CHECK           0x02 
#define TBSTYLE_GROUP           0x04 
#define TBSTYLE_CHECKGROUP      (TBSTYLE_GROUP | TBSTYLE_CHECK) 
#define TBSTYLE_DROPDOWN        0x08 
 
#define TBSTYLE_TOOLTIPS        0x0100 
#define TBSTYLE_WRAPABLE        0x0200 
#define TBSTYLE_ALTDRAG         0x0400 
#define TBSTYLE_FLAT            0x0800 
#define TBSTYLE_LIST            0x1000 
#define TBSTYLE_CUSTOMERASE     0x2000 

#define TBIF_IMAGE              0x00000001
#define TBIF_TEXT               0x00000002
#define TBIF_STATE              0x00000004
#define TBIF_STYLE              0x00000008
#define TBIF_LPARAM             0x00000010
#define TBIF_COMMAND            0x00000020
#define TBIF_SIZE               0x00000040

#define TBBF_LARGE		0x0001 

#define TB_ENABLEBUTTON          (WM_USER+1)
#define TB_CHECKBUTTON           (WM_USER+2)
#define TB_PRESSBUTTON           (WM_USER+3)
#define TB_HIDEBUTTON            (WM_USER+4)
#define TB_INDETERMINATE         (WM_USER+5)
#define TB_ISBUTTONENABLED       (WM_USER+9) 
#define TB_ISBUTTONCHECKED       (WM_USER+10) 
#define TB_ISBUTTONPRESSED       (WM_USER+11) 
#define TB_ISBUTTONHIDDEN        (WM_USER+12) 
#define TB_ISBUTTONINDETERMINATE (WM_USER+13)
#define TB_ISBUTTONHIGHLIGHTED   (WM_USER+14)
#define TB_SETSTATE              (WM_USER+17)
#define TB_GETSTATE              (WM_USER+18)
#define TB_ADDBITMAP             (WM_USER+19)
#define TB_ADDBUTTONSA         (WM_USER+20)
#define TB_ADDBUTTONSW         (WM_USER+68)
#define TB_ADDBUTTONS WINELIB_NAME_AW(TB_ADDBUTTONS)
#define TB_HITTEST               (WM_USER+69)
#define TB_INSERTBUTTONA       (WM_USER+21)
#define TB_INSERTBUTTONW       (WM_USER+67)
#define TB_INSERTBUTTON WINELIB_NAME_AW(TB_INSERTBUTTON)
#define TB_DELETEBUTTON          (WM_USER+22)
#define TB_GETBUTTON             (WM_USER+23)
#define TB_BUTTONCOUNT           (WM_USER+24)
#define TB_COMMANDTOINDEX        (WM_USER+25)
#define TB_SAVERESTOREA        (WM_USER+26)
#define TB_SAVERESTOREW        (WM_USER+76)
#define TB_SAVERESTORE WINELIB_NAME_AW(TB_SAVERESTORE)
#define TB_CUSTOMIZE             (WM_USER+27)
#define TB_ADDSTRINGA          (WM_USER+28) 
#define TB_ADDSTRINGW          (WM_USER+77) 
#define TB_ADDSTRING WINELIB_NAME_AW(TB_ADDSTRING)
#define TB_GETITEMRECT           (WM_USER+29)
#define TB_BUTTONSTRUCTSIZE      (WM_USER+30)
#define TB_SETBUTTONSIZE         (WM_USER+31)
#define TB_SETBITMAPSIZE         (WM_USER+32)
#define TB_AUTOSIZE              (WM_USER+33)
#define TB_GETTOOLTIPS           (WM_USER+35)
#define TB_SETTOOLTIPS           (WM_USER+36)
#define TB_SETPARENT             (WM_USER+37)
#define TB_SETROWS               (WM_USER+39)
#define TB_GETROWS               (WM_USER+40)
#define TB_GETBITMAPFLAGS        (WM_USER+41)
#define TB_SETCMDID              (WM_USER+42)
#define TB_CHANGEBITMAP          (WM_USER+43)
#define TB_GETBITMAP             (WM_USER+44)
#define TB_GETBUTTONTEXTA      (WM_USER+45)
#define TB_GETBUTTONTEXTW      (WM_USER+75)
#define TB_GETBUTTONTEXT WINELIB_NAME_AW(TB_GETBUTTONTEXT)
#define TB_REPLACEBITMAP         (WM_USER+46)
#define TB_SETINDENT             (WM_USER+47)
#define TB_SETIMAGELIST          (WM_USER+48)
#define TB_GETIMAGELIST          (WM_USER+49)
#define TB_LOADIMAGES            (WM_USER+50)
#define TB_GETRECT               (WM_USER+51) /* wParam is the Cmd instead of index */
#define TB_SETHOTIMAGELIST       (WM_USER+52)
#define TB_GETHOTIMAGELIST       (WM_USER+53)
#define TB_SETDISABLEDIMAGELIST  (WM_USER+54)
#define TB_GETDISABLEDIMAGELIST  (WM_USER+55)
#define TB_SETSTYLE              (WM_USER+56)
#define TB_GETSTYLE              (WM_USER+57)
#define TB_GETBUTTONSIZE         (WM_USER+58)
#define TB_SETBUTTONWIDTH        (WM_USER+59)
#define TB_SETMAXTEXTROWS        (WM_USER+60)
#define TB_GETTEXTROWS           (WM_USER+61)
#define TB_GETOBJECT             (WM_USER+62)
#define TB_GETBUTTONINFOW      (WM_USER+63)
#define TB_GETBUTTONINFOA      (WM_USER+65)
#define TB_GETBUTTONINFO WINELIB_NAME_AW(TB_GETBUTTONINFO)
#define TB_SETBUTTONINFOW      (WM_USER+64)
#define TB_SETBUTTONINFOA      (WM_USER+66)
#define TB_SETBUTTONINFO WINELIB_NAME_AW(TB_SETBUTTONINFO)
#define TB_SETDRAWTEXTFLAGS      (WM_USER+70)
#define TB_GETHOTITEM            (WM_USER+71)
#define TB_SETHOTITEM            (WM_USER+72)
#define TB_SETANCHORHIGHLIGHT    (WM_USER+73)
#define TB_GETANCHORHIGHLIGHT    (WM_USER+74)
#define TB_MAPACCELERATORA     (WM_USER+78)
#define TB_MAPACCELERATORW     (WM_USER+90)
#define TB_MAPACCELERATOR WINELIB_NAME_AW(TB_MAPACCELERATOR)
#define TB_GETINSERTMARK         (WM_USER+79)
#define TB_SETINSERTMARK         (WM_USER+80)
#define TB_INSERTMARKHITTEST     (WM_USER+81)
#define TB_MOVEBUTTON            (WM_USER+82)
#define TB_GETMAXSIZE            (WM_USER+83)
#define TB_SETEXTENDEDSTYLE      (WM_USER+84)
#define TB_GETEXTENDEDSTYLE      (WM_USER+85)
#define TB_GETPADDING            (WM_USER+86)
#define TB_SETPADDING            (WM_USER+87)
#define TB_SETINSERTMARKCOLOR    (WM_USER+88)
#define TB_GETINSERTMARKCOLOR    (WM_USER+89)
#define TB_SETCOLORSCHEME        CCM_SETCOLORSCHEME
#define TB_GETCOLORSCHEME        CCM_GETCOLORSCHEME
#define TB_SETUNICODEFORMAT      CCM_SETUNICODEFORMAT
#define TB_GETUNICODEFORMAT      CCM_GETUNICODEFORMAT

#define TBN_FIRST               (0U-700U)
#define TBN_LAST                (0U-720U)
#define TBN_GETBUTTONINFOA    (TBN_FIRST-0)
#define TBN_GETBUTTONINFOW    (TBN_FIRST-20)
#define TBN_GETBUTTONINFO WINELIB_NAME_AW(TBN_GETBUTTONINFO)
#define TBN_GETINFOTIPA       (TBN_FIRST-18)
#define TBN_GETINFOTIPW       (TBN_FIRST-19)
#define TBN_GETINFOTIP WINELIB_NAME_AW(TBN_GETINFOTIP)


/* This is just for old CreateToolbar. */
/* Don't use it in new programs. */
typedef struct _OLDTBBUTTON {
    INT iBitmap;
    INT idCommand;
    BYTE  fsState;
    BYTE  fsStyle;
    BYTE  bReserved[2];
    DWORD dwData;
} OLDTBBUTTON, *POLDTBBUTTON, *LPOLDTBBUTTON;
typedef const OLDTBBUTTON *LPCOLDTBBUTTON;


typedef struct _TBBUTTON {
    INT iBitmap;
    INT idCommand;
    BYTE  fsState;
    BYTE  fsStyle;
    BYTE  bReserved[2];
    DWORD dwData;
    INT iString;
} TBBUTTON, *PTBBUTTON, *LPTBBUTTON;
typedef const TBBUTTON *LPCTBBUTTON;


typedef struct _COLORMAP {
    COLORREF from;
    COLORREF to;
} COLORMAP, *LPCOLORMAP;

typedef struct tagTBADDBITMAP {
    HINSTANCE hInst;
    UINT      nID;
} TBADDBITMAP, *LPTBADDBITMAP;

#define HINST_COMMCTRL         ((HINSTANCE)-1)
#define IDB_STD_SMALL_COLOR     0
#define IDB_STD_LARGE_COLOR     1
#define IDB_VIEW_SMALL_COLOR    4
#define IDB_VIEW_LARGE_COLOR    5
#define IDB_HIST_SMALL_COLOR    8
#define IDB_HIST_LARGE_COLOR    9

#define STD_CUT                 0
#define STD_COPY                1
#define STD_PASTE               2
#define STD_UNDO                3
#define STD_REDOW               4
#define STD_DELETE              5
#define STD_FILENEW             6
#define STD_FILEOPEN            7
#define STD_FILESAVE            8
#define STD_PRINTPRE            9
#define STD_PROPERTIES          10
#define STD_HELP                11
#define STD_FIND                12
#define STD_REPLACE             13
#define STD_PRINT               14

#define VIEW_LARGEICONS         0
#define VIEW_SMALLICONS         1
#define VIEW_LIST               2
#define VIEW_DETAILS            3
#define VIEW_SORTNAME           4
#define VIEW_SORTSIZE           5
#define VIEW_SORTDATE           6
#define VIEW_SORTTYPE           7
#define VIEW_PARENTFOLDER       8
#define VIEW_NETCONNECT         9
#define VIEW_NETDISCONNECT      10
#define VIEW_NEWFOLDER          11

typedef struct tagTBSAVEPARAMSA {
    HKEY   hkr;
    LPCSTR pszSubKey;
    LPCSTR pszValueName;
} TBSAVEPARAMSA, *LPTBSAVEPARAMSA;

typedef struct tagTBSAVEPARAMSW {
    HKEY   hkr;
    LPCWSTR pszSubKey;
    LPCWSTR pszValueName;
} TBSAVEPARAMSAW, *LPTBSAVEPARAMSAW;

#define TBSAVEPARAMS   WINELIB_NAME_AW(TBSAVEPARAMS)
#define LPTBSAVEPARAMS WINELIB_NAME_AW(LPTBSAVEPARAMS)

typedef struct
{
    UINT cbSize;
    DWORD  dwMask;
    INT  idCommand;
    INT  iImage;
    BYTE   fsState;
    BYTE   fsStyle;
    WORD   cx;
    DWORD  lParam;
    LPSTR  pszText;
    INT  cchText;
} TBBUTTONINFOA, *LPTBBUTTONINFOA;

typedef struct
{
    UINT cbSize;
    DWORD  dwMask;
    INT  idCommand;
    INT  iImage;
    BYTE   fsState;
    BYTE   fsStyle;
    WORD   cx;
    DWORD  lParam;
    LPWSTR pszText;
    INT  cchText;
} TBBUTTONINFOW, *LPTBBUTTONINFOW;

#define TBBUTTONINFO   WINELIB_NAME_AW(TBBUTTONINFO)
#define LPTBBUTTONINFO WINELIB_NAME_AW(LPTBBUTTONINFO)

typedef struct tagNMTBGETINFOTIPA
{
    NMHDR  hdr;
    LPSTR  pszText;
    INT  cchTextMax;
    INT  iItem;
    LPARAM lParam;
} NMTBGETINFOTIPA, *LPNMTBGETINFOTIPA;

typedef struct tagNMTBGETINFOTIPW
{
    NMHDR  hdr;
    LPWSTR pszText;
    INT  cchTextMax;
    INT  iItem;
    LPARAM lParam;
} NMTBGETINFOTIPW, *LPNMTBGETINFOTIPW;

#define NMTBGETINFOTIP   WINELIB_NAME_AW(NMTBGETINFOFTIP)
#define LPNMTBGETINFOTIP WINELIB_NAME_AW(LPNMTBGETINFOTIP)

typedef struct
{
	HINSTANCE hInstOld;
	UINT      nIDOld;
	HINSTANCE hInstNew;
	UINT      nIDNew;
	INT       nButtons;
} TBREPLACEBITMAP, *LPTBREPLACEBITMAP;

HWND WINAPI
CreateToolbar(HWND, DWORD, UINT, INT, HINSTANCE,
              UINT, LPCOLDTBBUTTON, INT); 
 
HWND WINAPI
CreateToolbarEx(HWND, DWORD, UINT, INT,
                HINSTANCE, UINT, LPCTBBUTTON, 
                INT, INT, INT, INT, INT, UINT); 

HBITMAP WINAPI
CreateMappedBitmap (HINSTANCE, INT, UINT, LPCOLORMAP, INT); 



/* Tool tips */

#define TOOLTIPS_CLASS16        "tooltips_class"
#define TOOLTIPS_CLASSA       "tooltips_class32"
#define TOOLTIPS_CLASS32W       L"tooltips_class32"
#define TOOLTIPS_CLASS          WINELIB_NAME_AW(TOOLTIPS_CLASS)

#define INFOTIPSIZE             1024
 
#define TTS_ALWAYSTIP           0x01
#define TTS_NOPREFIX            0x02

#define TTF_IDISHWND            0x0001
#define TTF_CENTERTIP           0x0002
#define TTF_RTLREADING          0x0004
#define TTF_SUBCLASS            0x0010
#define TTF_TRACK               0x0020
#define TTF_ABSOLUTE            0x0080
#define TTF_TRANSPARENT         0x0100
#define TTF_DI_SETITEM          0x8000  /* valid only on the TTN_NEEDTEXT callback */


#define TTDT_AUTOMATIC          0
#define TTDT_RESHOW             1
#define TTDT_AUTOPOP            2
#define TTDT_INITIAL            3


#define TTM_ACTIVATE            (WM_USER+1)
#define TTM_SETDELAYTIME        (WM_USER+3)
#define TTM_ADDTOOLA          (WM_USER+4)
#define TTM_ADDTOOLW          (WM_USER+50)
#define TTM_ADDTOOL WINELIB_NAME_AW(TTM_ADDTOOL)
#define TTM_DELTOOLA          (WM_USER+5)
#define TTM_DELTOOLW          (WM_USER+51)
#define TTM_DELTOOL WINELIB_NAME_AW(TTM_DELTOOL)
#define TTM_NEWTOOLRECTA      (WM_USER+6)
#define TTM_NEWTOOLRECTW      (WM_USER+52)
#define TTM_NEWTOOLRECT WINELIB_NAME_AW(TTM_NEWTOOLRECT)
#define TTM_RELAYEVENT          (WM_USER+7)
#define TTM_GETTOOLINFOA      (WM_USER+8)
#define TTM_GETTOOLINFOW      (WM_USER+53)
#define TTM_GETTOOLINFO WINELIB_NAME_AW(TTM_GETTOOLINFO)
#define TTM_SETTOOLINFOA      (WM_USER+9)
#define TTM_SETTOOLINFOW      (WM_USER+54)
#define TTM_SETTOOLINFO WINELIB_NAME_AW(TTM_SETTOOLINFO)
#define TTM_HITTESTA          (WM_USER+10)
#define TTM_HITTESTW          (WM_USER+55)
#define TTM_HITTEST WINELIB_NAME_AW(TTM_HITTEST)
#define TTM_GETTEXTA          (WM_USER+11)
#define TTM_GETTEXTW          (WM_USER+56)
#define TTM_GETTEXT WINELIB_NAME_AW(TTM_GETTEXT)
#define TTM_UPDATETIPTEXTA    (WM_USER+12)
#define TTM_UPDATETIPTEXTW    (WM_USER+57)
#define TTM_UPDATETIPTEXT WINELIB_NAME_AW(TTM_UPDATETIPTEXT)
#define TTM_GETTOOLCOUNT        (WM_USER+13)
#define TTM_ENUMTOOLSA        (WM_USER+14)
#define TTM_ENUMTOOLSW        (WM_USER+58)
#define TTM_ENUMTOOLS WINELIB_NAME_AW(TTM_ENUMTOOLS)
#define TTM_GETCURRENTTOOLA   (WM_USER+15)
#define TTM_GETCURRENTTOOLW   (WM_USER+59)
#define TTM_GETCURRENTTOOL WINELIB_NAME_AW(TTM_GETCURRENTTOOL)
#define TTM_WINDOWFROMPOINT     (WM_USER+16)
#define TTM_TRACKACTIVATE       (WM_USER+17)
#define TTM_TRACKPOSITION       (WM_USER+18)
#define TTM_SETTIPBKCOLOR       (WM_USER+19)
#define TTM_SETTIPTEXTCOLOR     (WM_USER+20)
#define TTM_GETDELAYTIME        (WM_USER+21)
#define TTM_GETTIPBKCOLOR       (WM_USER+22)
#define TTM_GETTIPTEXTCOLOR     (WM_USER+23)
#define TTM_SETMAXTIPWIDTH      (WM_USER+24)
#define TTM_GETMAXTIPWIDTH      (WM_USER+25)
#define TTM_SETMARGIN           (WM_USER+26)
#define TTM_GETMARGIN           (WM_USER+27)
#define TTM_POP                 (WM_USER+28)
#define TTM_UPDATE              (WM_USER+29)


#define TTN_FIRST               (0U-520U)
#define TTN_LAST                (0U-549U)
#define TTN_GETDISPINFOA      (TTN_FIRST-0)
#define TTN_GETDISPINFOW      (TTN_FIRST-10)
#define TTN_GETDISPINFO WINELIB_NAME_AW(TTN_GETDISPINFO)
#define TTN_SHOW                (TTN_FIRST-1)
#define TTN_POP                 (TTN_FIRST-2)

#define TTN_NEEDTEXT TTN_GETDISPINFO
#define TTN_NEEDTEXTA TTN_GETDISPINFOA
#define TTN_NEEDTEXTW TTN_GETDISPINFOW

typedef struct tagTOOLINFOA {
    UINT cbSize;
    UINT uFlags;
    HWND hwnd;
    UINT uId;
    RECT rect;
    HINSTANCE hinst;
    LPSTR lpszText;
    LPARAM lParam;
} TTTOOLINFOA, *LPTOOLINFOA, *PTOOLINFOA, *LPTTTOOLINFOA;

typedef struct tagTOOLINFOW {
    UINT cbSize;
    UINT uFlags;
    HWND hwnd;
    UINT uId;
    RECT rect;
    HINSTANCE hinst;
    LPWSTR lpszText;
    LPARAM lParam;
} TTTOOLINFOW, *LPTOOLINFOW, *PTOOLINFOW, *LPTTTOOLINFOW;

#define TTTOOLINFO WINELIB_NAME_AW(TTTOOLINFO)
#define TOOLINFO WINELIB_NAME_AW(TTTOOLINFO)
#define PTOOLINFO WINELIB_NAME_AW(PTOOLINFO)
#define LPTTTOOLINFO WINELIB_NAME_AW(LPTTTOOLINFO)
#define LPTOOLINFO WINELIB_NAME_AW(LPTOOLINFO)

#define TTTOOLINFO_V1_SIZEA CCSIZEOF_STRUCT(TTTOOLINFOA, lpszText)
#define TTTOOLINFO_V1_SIZEW CCSIZEOF_STRUCT(TTTOOLINFOW, lpszText)
#define TTTOOLINFO_V1_SIZE WINELIB_NAME_AW(TTTOOLINFO_V1_SIZE)

typedef struct _TT_HITTESTINFOA
{
    HWND        hwnd;
    POINT       pt;
    TTTOOLINFOA ti;
} TTHITTESTINFOA, *LPTTHITTESTINFOA;

typedef struct _TT_HITTESTINFOW
{
    HWND        hwnd;
    POINT       pt;
    TTTOOLINFOW ti;
} TTHITTESTINFOW, *LPTTHITTESTINFOW;

#define TTHITTESTINFO WINELIB_NAME_AW(TTHITTESTINFO)
#define LPTTHITTESTINFO WINELIB_NAME_AW(LPTTHITTESTINFO)

typedef struct tagNMTTDISPINFOA
{
    NMHDR hdr;
    LPSTR lpszText;
    CHAR  szText[80];
    HINSTANCE hinst;
    UINT      uFlags;
    LPARAM      lParam;
} NMTTDISPINFOA, *LPNMTTDISPINFOA;

typedef struct tagNMTTDISPINFOW
{
    NMHDR       hdr;
    LPWSTR      lpszText;
    WCHAR       szText[80];
    HINSTANCE hinst;
    UINT      uFlags;
    LPARAM      lParam;
} NMTTDISPINFOW, *LPNMTTDISPINFOW;

#define NMTTDISPINFO WINELIB_NAME_AW(NMTTDISPINFO)
#define LPNMTTDISPINFO WINELIB_NAME_AW(LPNMTTDISPINFO)

#define NMTTDISPINFO_V1_SIZEA CCSIZEOF_STRUCT(NMTTDISPINFOA, uFlags)
#define NMTTDISPINFO_V1_SIZEW CCSIZEOF_STRUCT(NMTTDISPINFOW, uFlags)
#define NMTTDISPINFO_V1_SIZE WINELIB_NAME_AW(NMTTDISPINFO_V1_SIZE)

#define TOOLTIPTEXTW    NMTTDISPINFOW
#define TOOLTIPTEXTA    NMTTDISPINFOA
#define TOOLTIPTEXT     NMTTDISPINFO
#define LPTOOLTIPTEXTW  LPNMTTDISPINFOW
#define LPTOOLTIPTEXTA  LPNMTTDISPINFOA
#define LPTOOLTIPTEXT   LPNMTTDISPINFO





#endif /* _H_COMMCTRL */