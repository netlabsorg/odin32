/* $Id: winconst.h,v 1.19 2000-01-02 22:10:19 sandervl Exp $ */

/*
 * Win32 constants
 *
 * Copyright (C) 1999 Sander van Leeuwen
 *
 */

/* Predefined resources */
#define IDI_APPLICATION_W     32512
#define IDI_HAND_W            32513
#define IDI_QUESTION_W        32514
#define IDI_EXCLAMATION_W     32515
#define IDI_ASTERISK_W        32516
#define IDI_WINLOGO_W         32517

#define IDC_ARROW_W           32512
#define IDC_IBEAM_W           32513
#define IDC_WAIT_W            32514
#define IDC_CROSS_W           32515
#define IDC_UPARROW_W         32516
#define IDC_SIZE_W            32640
#define IDC_ICON_W            32641
#define IDC_SIZENWSE_W        32642
#define IDC_SIZENESW_W        32643
#define IDC_SIZEWE_W          32644
#define IDC_SIZENS_W          32645
#define IDC_SIZEALL_W         32646
#define IDC_NO_W              32648
#define IDC_APPSTARTING_W     32650
#define IDC_HELP_W            32651


#define OBM_CLOSE_W           32754
#define OBM_UPARROW_W         32753
#define OBM_DNARROW_W         32752
#define OBM_RGARROW_W         32751
#define OBM_LFARROW_W         32750
#define OBM_REDUCE_W          32749
#define OBM_ZOOM_W            32748
#define OBM_RESTORE_W         32747
#define OBM_REDUCED_W         32746
#define OBM_ZOOMD_W           32745
#define OBM_RESTORED_W        32744
#define OBM_UPARROWD_W        32743
#define OBM_DNARROWD_W        32742
#define OBM_RGARROWD_W        32741
#define OBM_LFARROWD_W        32740
#define OBM_MNARROW_W         32739
#define OBM_COMBO_W           32738
#define OBM_UPARROWI_W        32737
#define OBM_DNARROWI_W        32736
#define OBM_RGARROWI_W        32735
#define OBM_LFARROWI_W        32734

#define OBM_OLD_CLOSE_W       32767
#define OBM_SIZE_W            32766
#define OBM_OLD_UPARROW_W     32765
#define OBM_OLD_DNARROW_W     32764
#define OBM_OLD_RGARROW_W     32763
#define OBM_OLD_LFARROW_W     32762
#define OBM_BTSIZE_W          32761
#define OBM_CHECK_W           32760
#define OBM_CHECKBOXES_W      32759
#define OBM_BTNCORNERS_W      32758
#define OBM_OLD_REDUCE_W      32757
#define OBM_OLD_ZOOM_W        32756
#define OBM_OLD_RESTORE_W     32755

/* Window Styles */
#define WS_OVERLAPPED_W    0x00000000L
#define WS_POPUP_W         0x80000000L
#define WS_CHILD_W         0x40000000L
#define WS_MINIMIZE_W      0x20000000L
#define WS_VISIBLE_W       0x10000000L
#define WS_DISABLED_W      0x08000000L
#define WS_CLIPSIBLINGS_W  0x04000000L
#define WS_CLIPCHILDREN_W  0x02000000L
#define WS_MAXIMIZE_W      0x01000000L
#define WS_CAPTION_W       0x00C00000L
#define WS_BORDER_W        0x00800000L
#define WS_DLGFRAME_W      0x00400000L
#define WS_VSCROLL_W       0x00200000L
#define WS_HSCROLL_W       0x00100000L
#define WS_SYSMENU_W       0x00080000L
#define WS_THICKFRAME_W    0x00040000L
#define WS_GROUP_W         0x00020000L
#define WS_TABSTOP_W       0x00010000L
#define WS_MINIMIZEBOX_W   0x00020000L
#define WS_MAXIMIZEBOX_W   0x00010000L
#define WS_TILED_W         WS_OVERLAPPED_W
#define WS_ICONIC_W        WS_MINIMIZE_W
#define WS_SIZEBOX_W       WS_THICKFRAME_W
#define WS_OVERLAPPEDWINDOW_W (WS_OVERLAPPED_W | WS_CAPTION_W | WS_SYSMENU_W | WS_THICKFRAME_W| WS_MINIMIZEBOX_W | WS_MAXIMIZEBOX_W)
#define WS_POPUPWINDOW_W (WS_POPUP_W | WS_BORDER_W | WS_SYSMENU_W)
#define WS_CHILDWINDOW_W (WS_CHILD_W)
#define WS_TILEDWINDOW_W (WS_OVERLAPPEDWINDOW_W)

/* Window extended styles */
#define WS_EX_DLGMODALFRAME_W    0x00000001L
#define WS_EX_DRAGDETECT_W       0x00000002L
#define WS_EX_NOPARENTNOTIFY_W   0x00000004L
#define WS_EX_TOPMOST_W          0x00000008L
#define WS_EX_ACCEPTFILES_W      0x00000010L
#define WS_EX_TRANSPARENT_W      0x00000020L
#define WS_EX_MDICHILD_W         0x00000040L
#define WS_EX_TOOLWINDOW_W       0x00000080L
#define WS_EX_WINDOWEDGE_W       0x00000100L
#define WS_EX_CLIENTEDGE_W       0x00000200L
#define WS_EX_CONTEXTHELP_W      0x00000400L
#define WS_EX_RIGHT_W            0x00001000L
#define WS_EX_LEFT_W             0x00000000L
#define WS_EX_RTLREADING_W       0x00002000L
#define WS_EX_LTRREADING_W       0x00000000L
#define WS_EX_LEFTSCROLLBAR_W    0x00004000L
#define WS_EX_RIGHTSCROLLBAR_W   0x00000000L
#define WS_EX_CONTROLPARENT_W    0x00010000L
#define WS_EX_STATICEDGE_W       0x00020000L
#define WS_EX_APPWINDOW_W        0x00040000L

#define WS_EX_OVERLAPPEDWINDOW_W (WS_EX_WINDOWEDGE_W|WS_EX_CLIENTEDGE_W)
#define WS_EX_PALETTEWINDOW_W    (WS_EX_WINDOWEDGE_W|WS_EX_TOOLWINDOW_W|WS_EX_TOPMOST_W)

/* Class Styles
 */
 #define CS_VREDRAW_W           0x0001
 #define CS_HREDRAW_W           0x0002
 #define CS_KEYCVTWINDOW_W      0x0004
 #define CS_DBLCLKS_W           0x0008
 #define CS_OWNDC_W             0x0020
 #define CS_CLASSDC_W           0x0040
 #define CS_PARENTDC_W          0x0080
 #define CS_NOKEYCVT_W          0x0100
 #define CS_NOCLOSE_W           0x0200
 #define CS_SAVEBITS_W          0x0800
 #define CS_BYTEALIGNCLIENT_W   0x1000
 #define CS_BYTEALIGNWINDOW_W   0x2000
 #define CS_GLOBALCLASS_W       0x4000

#define DUPLICATE_CLOSE_SOURCE          0x00000001
#define DUPLICATE_SAME_ACCESS           0x00000002

#define HANDLE_FLAG_INHERIT             0x00000001
#define HANDLE_FLAG_PROTECT_FROM_CLOSE  0x00000002

/* SetWindowPos() and WINDOWPOS flags */
#define SWP_NOSIZE_W          0x0001
#define SWP_NOMOVE_W          0x0002
#define SWP_NOZORDER_W        0x0004
#define SWP_NOREDRAW_W        0x0008
#define SWP_NOACTIVATE_W      0x0010
#define SWP_FRAMECHANGED_W    0x0020  /* The frame changed: send WM_NCCALCSIZE */
#define SWP_SHOWWINDOW_W      0x0040
#define SWP_HIDEWINDOW_W      0x0080
#define SWP_NOCOPYBITS_W      0x0100
#define SWP_NOOWNERZORDER_W   0x0200  /* Don't do owner Z ordering */

#define SWP_DRAWFRAME_W       SWP_FRAMECHANGED_W
#define SWP_NOREPOSITION_W    SWP_NOOWNERZORDER_W

#define SWP_NOSENDCHANGING_W  0x0400
#define SWP_DEFERERASE_W      0x2000

/* undocumented SWP flags - from SDK 3.1 */
#define SWP_NOCLIENTSIZE_W    0x0800
#define SWP_NOCLIENTMOVE_W    0x1000

#define HWND_DESKTOP_W        ((HWND)0)
#define HWND_BROADCAST_W      ((HWND)0xffff)

/* SetWindowPos() hwndInsertAfter field values */
#define HWND_TOP_W            ((HWND)0)
#define HWND_BOTTOM_W         ((HWND)1)
#define HWND_TOPMOST_W        ((HWND)-1)
#define HWND_NOTOPMOST_W      ((HWND)-2)


/* Argument 1 passed to the DllEntryProc. */
#define DLL_PROCESS_DETACH      0       /* detach process (unload library) */
#define DLL_PROCESS_ATTACH      1       /* attach process (load library) */
#define DLL_THREAD_ATTACH       2       /* attach new thread */
#define DLL_THREAD_DETACH       3       /* detach thread */

/* Regions */
#define ERROR_W           0
#define NULLREGION_W      1
#define SIMPLEREGION_W    2
#define COMPLEXREGION_W   3

/* ShowWindow() codes */
#define SW_HIDE_W             0
#define SW_SHOWNORMAL_W       1
#define SW_NORMAL_W           1
#define SW_SHOWMINIMIZED_W    2
#define SW_SHOWMAXIMIZED_W    3
#define SW_MAXIMIZE_W         3
#define SW_SHOWNOACTIVATE_W   4
#define SW_SHOW_W             5
#define SW_MINIMIZE_W         6
#define SW_SHOWMINNOACTIVE_W  7
#define SW_SHOWNA_W           8
#define SW_RESTORE_W          9
#define SW_SHOWDEFAULT_W      10
#define SW_MAX_W              10
#define SW_NORMALNA_W         0xCC        /* undoc. flag in MinMaximize */


/* scroll messages */
#define WM_HSCROLL_W          0x0114
#define WM_VSCROLL_W          0x0115

/* WM_H/VSCROLL commands */
#define SB_LINEUP_W           0
#define SB_LINELEFT_W         0
#define SB_LINEDOWN_W         1
#define SB_LINERIGHT_W        1
#define SB_PAGEUP_W           2
#define SB_PAGELEFT_W         2
#define SB_PAGEDOWN_W         3
#define SB_PAGERIGHT_W        3
#define SB_THUMBPOSITION_W    4
#define SB_THUMBTRACK_W       5
#define SB_TOP_W              6
#define SB_LEFT_W             6
#define SB_BOTTOM_W           7
#define SB_RIGHT_W            7
#define SB_ENDSCROLL_W        8

/* WM_SYSCOMMAND parameters */
#define SC_SIZE_W         0xf000
#define SC_MOVE_W         0xf010
#define SC_MINIMIZE_W     0xf020
#define SC_MAXIMIZE_W     0xf030
#define SC_NEXTWINDOW_W   0xf040
#define SC_PREVWINDOW_W   0xf050
#define SC_CLOSE_W        0xf060
#define SC_VSCROLL_W      0xf070
#define SC_HSCROLL_W      0xf080
#define SC_MOUSEMENU_W    0xf090
#define SC_KEYMENU_W      0xf100
#define SC_ARRANGE_W      0xf110
#define SC_RESTORE_W      0xf120
#define SC_TASKLIST_W     0xf130
#define SC_SCREENSAVE_W   0xf140
#define SC_HOTKEY_W       0xf150

/* Key status flags for mouse events */
#define MK_LBUTTON_W        0x0001
#define MK_RBUTTON_W        0x0002
#define MK_SHIFT_W          0x0004
#define MK_CONTROL_W        0x0008
#define MK_MBUTTON_W        0x0010


/* Queue status flags */
#define QS_KEY_W          0x0001
#define QS_MOUSEMOVE_W    0x0002
#define QS_MOUSEBUTTON_W  0x0004
#define QS_MOUSE_W        (QS_MOUSEMOVE_W | QS_MOUSEBUTTON_W)
#define QS_POSTMESSAGE_W  0x0008
#define QS_TIMER_W        0x0010
#define QS_PAINT_W        0x0020
#define QS_SENDMESSAGE_W  0x0040
#define QS_HOTKEY_W       0x0080
#define QS_INPUT_W        (QS_MOUSE_W | QS_KEY_W)
#define QS_ALLEVENTS_W    (QS_INPUT_W | QS_POSTMESSAGE_W | QS_TIMER_W | QS_PAINT_W | QS_HOTKEY_W)
#define QS_ALLINPUT_W     (QS_ALLEVENTS_W | QS_SENDMESSAGE_W)


#define ERROR_INVALID_WINDOW_HANDLE_W           1400L

  /* Hook values */
#define WH_MIN_W              (-1)
#define WH_MSGFILTER_W        (-1)
#define WH_JOURNALRECORD_W    0
#define WH_JOURNALPLAYBACK_W  1
#define WH_KEYBOARD_W         2
#define WH_GETMESSAGE_W       3
#define WH_CALLWNDPROC_W      4
#define WH_CBT_W              5
#define WH_SYSMSGFILTER_W     6
#define WH_MOUSE_W            7
#define WH_HARDWARE_W         8
#define WH_DEBUG_W            9
#define WH_SHELL_W            10
#define WH_FOREGROUNDIDLE_W   11
#define WH_CALLWNDPROCRET_W   12
#define WH_MAX_W              12

#define WH_MINHOOK_W          WH_MIN_W
#define WH_MAXHOOK_W          WH_MAX_W
#ifndef WH_NB_HOOKS
#define WH_NB_HOOKS           (WH_MAXHOOK_W-WH_MINHOOK_W+1)
#endif

#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

/* WM_ACTIVATE wParam values */
#define WA_INACTIVE_W             0
#define WA_ACTIVE_W               1
#define WA_CLICKACTIVE_W          2


/* WM_NCHITTEST return codes */
#define HTERROR_W             (-2)
#define HTTRANSPARENT_W       (-1)
#define HTNOWHERE_W           0
#define HTCLIENT_W            1
#define HTCAPTION_W           2
#define HTSYSMENU_W           3
#define HTSIZE_W              4
#define HTMENU_W              5
#define HTHSCROLL_W           6
#define HTVSCROLL_W           7
#define HTMINBUTTON_W         8
#define HTMAXBUTTON_W         9
#define HTLEFT_W              10
#define HTRIGHT_W             11
#define HTTOP_W               12
#define HTTOPLEFT_W           13
#define HTTOPRIGHT_W          14
#define HTBOTTOM_W            15
#define HTBOTTOMLEFT_W        16
#define HTBOTTOMRIGHT_W       17
#define HTBORDER_W            18
#define HTGROWBOX_W           HTSIZE_W
#define HTREDUCE_W            HTMINBUTTON_W
#define HTZOOM_W              HTMAXBUTTON_W
#define HTOBJECT_W            19
#define HTCLOSE_W             20
#define HTHELP_W              21
#define HTSIZEFIRST_W         HTLEFT_W
#define HTSIZELAST_W          HTBOTTOMRIGHT_W


//class styles
#define CS_VREDRAW_W          0x0001
#define CS_HREDRAW_W          0x0002
#define CS_KEYCVTWINDOW_W     0x0004
#define CS_DBLCLKS_W          0x0008
#define CS_OWNDC_W            0x0020
#define CS_CLASSDC_W          0x0040
#define CS_PARENTDC_W         0x0080
#define CS_NOKEYCVT_W         0x0100
#define CS_NOCLOSE_W          0x0200
#define CS_SAVEBITS_W         0x0800
#define CS_BYTEALIGNCLIENT_W  0x1000
#define CS_BYTEALIGNWINDOW_W  0x2000
#define CS_GLOBALCLASS_W      0x4000


/* Offsets for GetClassLong() and GetClassWord() */
#define GCL_MENUNAME_W        (-8)
#define GCW_HBRBACKGROUND_W   (-10)
#define GCL_HBRBACKGROUND_W   GCW_HBRBACKGROUND_W
#define GCW_HCURSOR_W         (-12)
#define GCL_HCURSOR_W         GCW_HCURSOR_W
#define GCW_HICON_W           (-14)
#define GCL_HICON_W           GCW_HICON_W
#define GCW_HMODULE_W         (-16)
#define GCL_HMODULE_W         GCW_HMODULE_W
#define GCW_CBWNDEXTRA_W      (-18)
#define GCL_CBWNDEXTRA_W      GCW_CBWNDEXTRA_W
#define GCW_CBCLSEXTRA_W      (-20)
#define GCL_CBCLSEXTRA_W      GCW_CBCLSEXTRA_W
#define GCL_WNDPROC_W         (-24)
#define GCW_STYLE_W           (-26)
#define GCL_STYLE_W           GCW_STYLE_W
#define GCW_ATOM_W            (-32)
#define GCW_HICONSM_W         (-34)
#define GCL_HICONSM_W         GCW_HICONSM_W

/* PeekMessage() options */
#define PM_NOREMOVE_W     0x0000
#define PM_REMOVE_W       0x0001
#define PM_NOYIELD_W      0x0002

#define EXCEPTION_DEBUG_EVENT_W       1
#define CREATE_THREAD_DEBUG_EVENT_W   2
#define CREATE_PROCESS_DEBUG_EVENT_W  3
#define EXIT_THREAD_DEBUG_EVENT_W     4
#define EXIT_PROCESS_DEBUG_EVENT_W    5
#define LOAD_DLL_DEBUG_EVENT_W        6
#define UNLOAD_DLL_DEBUG_EVENT_W      7
#define OUTPUT_DEBUG_STRING_EVENT_W   8
#define RIP_EVENT_W                   9

//Region return values
#define ERROR_W             0
#define NULLREGION_W        1
#define SIMPLEREGION_W      2
#define COMPLEXREGION_W     3

//Parameters for CombineRgn
#define RGN_AND_W           1
#define RGN_OR_W            2
#define RGN_XOR_W           3
#define RGN_DIFF_W          4
#define RGN_COPY_W          5
