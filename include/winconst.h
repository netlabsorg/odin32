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
#define WH_KEYBOARD_LL        13
#define WH_MOUSE_LL           14
#define WH_MAX_W              14

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

//Parameters for CombineRgn
#define RGN_AND_W           1
#define RGN_OR_W            2
#define RGN_XOR_W           3
#define RGN_DIFF_W          4
#define RGN_COPY_W          5


#define PIPE_ACCESS_INBOUND_W         0x00000001
#define PIPE_ACCESS_OUTBOUND_W        0x00000002
#define PIPE_ACCESS_DUPLEX_W          0x00000003

#define PIPE_WAIT_W                   0x00000000
#define PIPE_NOWAIT_W                 0x00000001
#define PIPE_READMODE_BYTE_W          0x00000000
#define PIPE_READMODE_MESSAGE_W       0x00000002
#define PIPE_TYPE_BYTE_W              0x00000000
#define PIPE_TYPE_MESSAGE_W           0x00000004


#define GENERIC_READ_W               0x80000000
#define GENERIC_WRITE_W              0x40000000

#define FILE_SHARE_READ_W         0x00000001L
#define FILE_SHARE_WRITE_W        0x00000002L

#define CREATE_NEW_W              1
#define CREATE_ALWAYS_W           2
#define OPEN_EXISTING_W           3
#define OPEN_ALWAYS_W             4
#define TRUNCATE_EXISTING_W       5

#define FILE_ATTRIBUTE_READONLY_W         0x00000001L
#define FILE_ATTRIBUTE_HIDDEN_W           0x00000002L
#define FILE_ATTRIBUTE_SYSTEM_W           0x00000004L
#define FILE_ATTRIBUTE_DIRECTORY_W        0x00000010L
#define FILE_ATTRIBUTE_ARCHIVE_W          0x00000020L
#define FILE_ATTRIBUTE_NORMAL_W           0x00000080L
#define FILE_ATTRIBUTE_TEMPORARY_W        0x00000100L

#define FILE_FLAG_WRITE_THROUGH_W    0x80000000UL
#define FILE_FLAG_OVERLAPPED_W       0x40000000L
#define FILE_FLAG_NO_BUFFERING_W     0x20000000L
#define FILE_FLAG_RANDOM_ACCESS_W    0x10000000L
#define FILE_FLAG_SEQUENTIAL_SCAN_W  0x08000000L
#define FILE_FLAG_DELETE_ON_CLOSE_W  0x04000000L
#define FILE_FLAG_BACKUP_SEMANTICS_W 0x02000000L
#define FILE_FLAG_POSIX_SEMANTICS_W  0x01000000L


#define NO_ERROR_W                    0
#define ERROR_SUCCESS_W               0
#define ERROR_INVALID_FUNCTION_W      1
#define ERROR_FILE_NOT_FOUND_W        2
#define ERROR_PATH_NOT_FOUND_W        3
#define ERROR_TOO_MANY_OPEN_FILES_W   4
#define ERROR_ACCESS_DENIED_W         5
#define ERROR_INVALID_HANDLE_W        6
#define ERROR_ARENA_TRASHED_W         7
#define ERROR_NOT_ENOUGH_MEMORY_W     8
#define ERROR_INVALID_BLOCK_W         9
#define ERROR_BAD_ENVIRONMENT_W       10
#define ERROR_BAD_FORMAT_W            11
#define ERROR_INVALID_ACCESS_W        12
#define ERROR_INVALID_DATA_W          13
#define ERROR_OUTOFMEMORY_W           14
#define ERROR_INVALID_DRIVE_W         15
#define ERROR_CURRENT_DIRECTORY_W     16
#define ERROR_NOT_SAME_DEVICE_W       17
#define ERROR_NO_MORE_FILES_W         18
#define ERROR_WRITE_PROTECT_W         19
#define ERROR_BAD_UNIT_W              20
#define ERROR_NOT_READY_W             21
#define ERROR_BAD_COMMAND_W           22
#define ERROR_CRC_W                   23
#define ERROR_BAD_LENGTH_W            24
#define ERROR_SEEK_W                  25
#define ERROR_NOT_DOS_DISK_W          26
#define ERROR_SECTOR_NOT_FOUND_W      27
#define ERROR_WRITE_FAULT_W           29
#define ERROR_READ_FAULT_W            30
#define ERROR_GEN_FAILURE_W           31
#define ERROR_SHARING_VIOLATION_W     32
#define ERROR_LOCK_VIOLATION_W        33
#define ERROR_WRONG_DISK_W            34
#define ERROR_SHARING_BUFFER_EXCEEDED_W 36
#define ERROR_HANDLE_EOF_W            38
#define ERROR_HANDLE_DISK_FULL_W      39
#define ERROR_NOT_SUPPORTED_W         50
#define ERROR_REM_NOT_LIST_W          51
#define ERROR_DUP_NAME_W              52
#define ERROR_BAD_NETPATH_W           53
#define ERROR_NETWORK_BUSY_W          54
#define ERROR_DEV_NOT_EXIST_W         55
#define ERROR_ADAP_HDW_ERR_W          57
#define ERROR_BAD_NET_RESP_W          58
#define ERROR_UNEXP_NET_ERR_W         59
#define ERROR_BAD_REM_ADAP_W          60
#define ERROR_PRINTQ_FULL_W           61
#define ERROR_NO_SPOOL_SPACE_W        62
#define ERROR_PRINT_CANCELLED_W       63
#define ERROR_NETNAME_DELETED_W       64
#define ERROR_NETWORK_ACCESS_DENIED_W 65
#define ERROR_BAD_DEV_TYPE_W          66
#define ERROR_BAD_NET_NAME_W          67
#define ERROR_TOO_MANY_NAMES_W        68
#define ERROR_TOO_MANY_SESS_W         69
#define ERROR_SHARING_PAUSED_W        70
#define ERROR_REQ_NOT_ACCEP_W         71
#define ERROR_REDIR_PAUSED_W          72
#define ERROR_FILE_EXISTS_W           80
#define ERROR_CANNOT_MAKE_W           82
#define ERROR_FAIL_I24_W              83
#define ERROR_OUT_OF_STRUCTURES_W     84
#define ERROR_ALREADY_ASSIGNED_W      85
#define ERROR_INVALID_PASSWORD_W      86
#define ERROR_INVALID_PARAMETER_W     87
#define ERROR_NET_WRITE_FAULT_W       88
#define ERROR_NO_PROC_SLOTS_W         89
#define ERROR_TOO_MANY_SEMAPHORES_W   100
#define ERROR_EXCL_SEM_ALREADY_OWNED_W 101
#define ERROR_SEM_IS_SET_W            102
#define ERROR_TOO_MANY_SEM_REQUESTS_W 103
#define ERROR_INVALID_AT_INTERRUPT_TIME_W 104
#define ERROR_SEM_OWNER_DIED_W        105
#define ERROR_SEM_USER_LIMIT_W        106
#define ERROR_DISK_CHANGE_W           107
#define ERROR_DRIVE_LOCKED_W          108
#define ERROR_BROKEN_PIPE_W           109
#define ERROR_OPEN_FAILED_W           110
#define ERROR_BUFFER_OVERFLOW_W       111
#define ERROR_DISK_FULL_W             112
#define ERROR_NO_MORE_SEARCH_HANDLES_W 113
#define ERROR_INVALID_TARGET_HANDLE_W 114
#define ERROR_INVALID_CATEGORY_W      117
#define ERROR_INVALID_VERIFY_SWITCH_W 118
#define ERROR_BAD_DRIVER_LEVEL_W      119
#define ERROR_CALL_NOT_IMPLEMENTED_W  120
#define ERROR_SEM_TIMEOUT_W           121
#define ERROR_INSUFFICIENT_BUFFER_W   122
#define ERROR_INVALID_NAME_W          123
#define ERROR_INVALID_LEVEL_W         124
#define ERROR_NO_VOLUME_LABEL_W       125
#define ERROR_MOD_NOT_FOUND_W         126
#define ERROR_PROC_NOT_FOUND_W        127
#define ERROR_WAIT_NO_CHILDREN_W      128
#define ERROR_CHILD_NOT_COMPLETE_W    129
#define ERROR_DIRECT_ACCESS_HANDLE_W  130
#define ERROR_NEGATIVE_SEEK_W         131
#define ERROR_SEEK_ON_DEVICE_W        132
#define ERROR_IS_JOIN_TARGET_W        133
#define ERROR_IS_JOINED_W             134
#define ERROR_IS_SUBSTED_W            135
#define ERROR_NOT_JOINED_W            136
#define ERROR_NOT_SUBSTED_W           137
#define ERROR_JOIN_TO_JOIN_W          138
#define ERROR_SUBST_TO_SUBST_W        139
#define ERROR_JOIN_TO_SUBST_W         140
#define ERROR_SUBST_TO_JOIN_W         141
#define ERROR_BUSY_DRIVE_W            142
#define ERROR_SAME_DRIVE_W            143
#define ERROR_DIR_NOT_ROOT_W          144
#define ERROR_DIR_NOT_EMPTY_W         145
#define ERROR_IS_SUBST_PATH_W         146
#define ERROR_IS_JOIN_PATH_W          147
#define ERROR_PATH_BUSY_W             148
#define ERROR_IS_SUBST_TARGET_W       149
#define ERROR_SYSTEM_TRACE_W          150
#define ERROR_INVALID_EVENT_COUNT_W   151
#define ERROR_TOO_MANY_MUXWAITERS_W   152
#define ERROR_INVALID_LIST_FORMAT_W   153
#define ERROR_LABEL_TOO_LONG_W        154
#define ERROR_TOO_MANY_TCBS_W         155
#define ERROR_SIGNAL_REFUSED_W        156
#define ERROR_DISCARDED_W             157
#define ERROR_NOT_LOCKED_W            158
#define ERROR_BAD_THREADID_ADDR_W     159
#define ERROR_BAD_ARGUMENTS_W         160
#define ERROR_BAD_PATHNAME_W          161
#define ERROR_SIGNAL_PENDING_W        162
#define ERROR_MAX_THRDS_REACHED_W     164
#define ERROR_LOCK_FAILED_W           167
#define ERROR_BUSY_W                  170
#define ERROR_CANCEL_VIOLATION_W      173
#define ERROR_ATOMIC_LOCKS_NOT_SUPPORTED_W 174
#define ERROR_INVALID_SEGMENT_NUMBER_W 180
#define ERROR_INVALID_ORDINAL_W       182
#define ERROR_ALREADY_EXISTS_W        183
#define ERROR_INVALID_FLAG_NUMBER_W   186
#define ERROR_SEM_NOT_FOUND_W         187
#define ERROR_INVALID_STARTING_CODESEG_W 188
#define ERROR_INVALID_STACKSEG_W      189
#define ERROR_INVALID_MODULETYPE_W    190
#define ERROR_INVALID_EXE_SIGNATURE_W 191
#define ERROR_EXE_MARKED_INVALID_W    192
#define ERROR_BAD_EXE_FORMAT_W        193
#define ERROR_ITERATED_DATA_EXCEEDS_64k_W 194
#define ERROR_INVALID_MINALLOCSIZE_W  195
#define ERROR_DYNLINK_FROM_INVALID_RING_W 196
#define ERROR_IOPL_NOT_ENABLED_W      197
#define ERROR_INVALID_SEGDPL_W        198
#define ERROR_AUTODATASEG_EXCEEDS_64k_W 199
#define ERROR_RING2SEG_MUST_BE_MOVABLE_W 200
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM_W 201
#define ERROR_INFLOOP_IN_RELOC_CHAIN_W 202
#define ERROR_ENVVAR_NOT_FOUND_W      203
#define ERROR_NO_SIGNAL_SENT_W        205
#define ERROR_FILENAME_EXCED_RANGE_W  206
#define ERROR_RING2_STACK_IN_USE_W    207
#define ERROR_META_EXPANSION_TOO_LONG_W 208
#define ERROR_INVALID_SIGNAL_NUMBER_W 209
#define ERROR_THREAD_1_INACTIVE_W     210
#define ERROR_LOCKED_W                212
#define ERROR_TOO_MANY_MODULES_W      214
#define ERROR_NESTING_NOT_ALLOWED_W   215
#define ERROR_EXE_MACHINE_TYPE_MISMATCH_W 216
#define ERROR_BAD_PIPE_W              230
#define ERROR_PIPE_BUSY_W             231
#define ERROR_NO_DATA_W               232
#define ERROR_PIPE_NOT_CONNECTED_W    233
#define ERROR_MORE_DATA_W             234
#define ERROR_VC_DISCONNECTED_W       240
#define ERROR_INVALID_EA_NAME_W       254
#define ERROR_EA_LIST_INCONSISTENT_W  255
#define ERROR_NO_MORE_ITEMS_W         259
#define ERROR_CANNOT_COPY_W           266
#define ERROR_DIRECTORY_W             267
#define ERROR_EAS_DIDNT_FIT_W         275
#define ERROR_EA_FILE_CORRUPT_W       276
#define ERROR_EA_TABLE_FULL_W         277
#define ERROR_INVALID_EA_HANDLE_W     278
#define ERROR_EAS_NOT_SUPPORTED_W     282
#define ERROR_NOT_OWNER_W             288
#define ERROR_TOO_MANY_POSTS_W        298
#define ERROR_PARTIAL_COPY_W          299
#define ERROR_OPLOCK_NOT_GRANTED_W    300
#define ERROR_INVALID_OPLOCK_PROTOCOL_W 301
#define ERROR_MR_MID_NOT_FOUND_W      317
#define ERROR_INVALID_ADDRESS_W       487
#define ERROR_ARITHMETIC_OVERFLOW_W   534
#define ERROR_PIPE_CONNECTED_W        535
#define ERROR_PIPE_LISTENING_W        536
#define ERROR_EA_ACCESS_DENIED_W      994
#define ERROR_OPERATION_ABORTED_W     995
#define ERROR_IO_INCOMPLETE_W         996
#define ERROR_IO_PENDING_W            997
#define ERROR_NOACCESS_W              998
#define ERROR_SWAPERROR_W             999
#define ERROR_STACK_OVERFLOW_W                  1001L
#define ERROR_INVALID_MESSAGE_W                 1002L
#define ERROR_CAN_NOT_COMPLETE_W                1003L
#define ERROR_INVALID_FLAGS_W                   1004L
#define ERROR_UNRECOGNIZED_VOLUME_W             1005L
#define ERROR_FILE_INVALID_W                    1006L
#define ERROR_FULLSCREEN_MODE_W                 1007L
#define ERROR_NO_TOKEN_W                        1008L
#define ERROR_BADDB_W                           1009L
#define ERROR_BADKEY_W                          1010L
#define ERROR_CANTOPEN_W                        1011L
#define ERROR_CANTREAD_W                        1012L
#define ERROR_CANTWRITE_W                       1013L
#define ERROR_REGISTRY_RECOVERED_W              1014L
#define ERROR_REGISTRY_CORRUPT_W                1015L
#define ERROR_REGISTRY_IO_FAILED_W              1016L
#define ERROR_NOT_REGISTRY_FILE_W               1017L
#define ERROR_KEY_DELETED_W                     1018L
#define ERROR_NO_LOG_SPACE_W                    1019L
#define ERROR_KEY_HAS_CHILDREN_W                1020L
#define ERROR_CHILD_MUST_BE_VOLATILE_W          1021L
#define ERROR_NOTIFY_ENUM_DIR_W                 1022L
#define ERROR_DEPENDENT_SERVICES_RUNNING_W      1051L
#define ERROR_INVALID_SERVICE_CONTROL_W         1052L
#define ERROR_SERVICE_REQUEST_TIMEOUT_W         1053L
#define ERROR_SERVICE_NO_THREAD_W               1054L
#define ERROR_SERVICE_DATABASE_LOCKED_W         1055L
#define ERROR_SERVICE_ALREADY_RUNNING_W         1056L
#define ERROR_INVALID_SERVICE_ACCOUNT_W         1057L
#define ERROR_SERVICE_DISABLED_W                1058L
#define ERROR_CIRCULAR_DEPENDENCY_W             1059L
#define ERROR_SERVICE_DOES_NOT_EXIST_W          1060L
#define ERROR_SERVICE_CANNOT_ACCEPT_CTRL_W      1061L
#define ERROR_SERVICE_NOT_ACTIVE_W              1062L
#define ERROR_FAILED_SERVICE_CONTROLLER_CONNECT_W 1063L
#define ERROR_EXCEPTION_IN_SERVICE_W            1064L
#define ERROR_DATABASE_DOES_NOT_EXIST_W         1065L
#define ERROR_SERVICE_SPECIFIC_ERROR_W          1066L
#define ERROR_PROCESS_ABORTED_W                 1067L
#define ERROR_SERVICE_DEPENDENCY_FAIL_W         1068L
#define ERROR_SERVICE_LOGON_FAILED_W            1069L
#define ERROR_SERVICE_START_HANG_W              1070L
#define ERROR_INVALID_SERVICE_LOCK_W            1071L
#define ERROR_SERVICE_MARKED_FOR_DELETE_W       1072L
#define ERROR_SERVICE_EXISTS_W                  1073L
#define ERROR_ALREADY_RUNNING_LKG_W             1074L
#define ERROR_SERVICE_DEPENDENCY_DELETED_W      1075L
#define ERROR_BOOT_ALREADY_ACCEPTED_W           1076L
#define ERROR_SERVICE_NEVER_STARTED_W           1077L
#define ERROR_DUPLICATE_SERVICE_NAME_W          1078L
#define ERROR_END_OF_MEDIA_W                    1100L
#define ERROR_FILEMARK_DETECTED_W               1101L
#define ERROR_BEGINNING_OF_MEDIA_W              1102L
#define ERROR_SETMARK_DETECTED_W                1103L
#define ERROR_NO_DATA_DETECTED_W                1104L
#define ERROR_PARTITION_FAILURE_W               1105L
#define ERROR_INVALID_BLOCK_LENGTH_W            1106L
#define ERROR_DEVICE_NOT_PARTITIONED_W          1107L
#define ERROR_UNABLE_TO_LOCK_MEDIA_W            1108L
#define ERROR_UNABLE_TO_UNLOAD_MEDIA_W          1109L
#define ERROR_MEDIA_CHANGED_W                   1110L
#define ERROR_BUS_RESET_W                       1111L
#define ERROR_NO_MEDIA_IN_DRIVE_W               1112L
#define ERROR_NO_UNICODE_TRANSLATION_W          1113L
#define ERROR_DLL_INIT_FAILED_W                 1114L
#define ERROR_SHUTDOWN_IN_PROGRESS_W            1115L
#define ERROR_NO_SHUTDOWN_IN_PROGRESS_W         1116L
#define ERROR_IO_DEVICE_W                       1117L
#define ERROR_SERIAL_NO_DEVICE_W                1118L
#define ERROR_IRQ_BUSY_W                        1119L
#define ERROR_MORE_WRITES_W                     1120L
#define ERROR_COUNTER_TIMEOUT_W                 1121L
#define ERROR_FLOPPY_ID_MARK_NOT_FOUND_W        1122L
#define ERROR_FLOPPY_WRONG_CYLINDER_W           1123L
#define ERROR_FLOPPY_UNKNOWN_ERROR_W            1124L
#define ERROR_FLOPPY_BAD_REGISTERS_W            1125L
#define ERROR_DISK_RECALIBRATE_FAILED_W         1126L
#define ERROR_DISK_OPERATION_FAILED_W           1127L
#define ERROR_DISK_RESET_FAILED_W               1128L
#define ERROR_EOM_OVERFLOW_W                    1129L
#define ERROR_NOT_ENOUGH_SERVER_MEMORY_W        1130L
#define ERROR_POSSIBLE_DEADLOCK_W               1131L
#define ERROR_MAPPED_ALIGNMENT_W                1132L
#define ERROR_BAD_DEVICE_W                      1200L
#define ERROR_CONNECTION_UNAVAIL_W              1201L
#define ERROR_DEVICE_ALREADY_REMEMBERED_W       1202L
#define ERROR_NO_NET_OR_BAD_PATH_W              1203L
#define ERROR_BAD_PROVIDER_W                    1204L
#define ERROR_CANNOT_OPEN_PROFILE_W             1205L
#define ERROR_BAD_PROFILE_W                     1206L
#define ERROR_NOT_CONTAINER_W                   1207L
#define ERROR_EXTENDED_ERROR_W                  1208L
#define ERROR_INVALID_GROUPNAME_W               1209L
#define ERROR_INVALID_COMPUTERNAME_W            1210L
#define ERROR_INVALID_EVENTNAME_W               1211L
#define ERROR_INVALID_DOMAINNAME_W              1212L
#define ERROR_INVALID_SERVICENAME_W             1213L
#define ERROR_INVALID_NETNAME_W                 1214L
#define ERROR_INVALID_SHARENAME_W               1215L
#define ERROR_INVALID_PASSWORDNAME_W            1216L
#define ERROR_INVALID_MESSAGENAME_W             1217L
#define ERROR_INVALID_MESSAGEDEST_W             1218L
#define ERROR_SESSION_CREDENTIAL_CONFLICT_W     1219L
#define ERROR_REMOTE_SESSION_LIMIT_EXCEEDED_W   1220L
#define ERROR_DUP_DOMAINNAME_W                  1221L
#define ERROR_NO_NETWORK_W                      1222L
#define ERROR_CANCELLED_W                       1223L
#define ERROR_USER_MAPPED_FILE_W                1224L
#define ERROR_CONNECTION_REFUSED_W              1225L
#define ERROR_GRACEFUL_DISCONNECT_W             1226L
#define ERROR_ADDRESS_ALREADY_ASSOCIATED_W      1227L
#define ERROR_ADDRESS_NOT_ASSOCIATED_W          1228L
#define ERROR_CONNECTION_INVALID_W              1229L
#define ERROR_CONNECTION_ACTIVE_W               1230L
#define ERROR_NETWORK_UNREACHABLE_W             1231L
#define ERROR_HOST_UNREACHABLE_W                1232L
#define ERROR_PROTOCOL_UNREACHABLE_W            1233L
#define ERROR_PORT_UNREACHABLE_W                1234L
#define ERROR_REQUEST_ABORTED_W                 1235L
#define ERROR_CONNECTION_ABORTED_W              1236L
#define ERROR_RETRY_W                           1237L
#define ERROR_CONNECTION_COUNT_LIMIT_W          1238L
#define ERROR_LOGIN_TIME_RESTRICTION_W          1239L
#define ERROR_LOGIN_WKSTA_RESTRICTION_W         1240L
#define ERROR_INCORRECT_ADDRESS_W               1241L
#define ERROR_ALREADY_REGISTERED_W              1242L
#define ERROR_SERVICE_NOT_FOUND_W               1243L
#define ERROR_NOT_AUTHENTICATED_W             1244L
#define ERROR_NOT_LOGGED_ON_W                   1245L
#define ERROR_CONTINUE_W                        1246L
#define ERROR_ALREADY_INITIALIZED_W             1247L
#define ERROR_NO_MORE_DEVICES_W                 1248L
#define ERROR_NOT_ALL_ASSIGNED_W                1300L
#define ERROR_SOME_NOT_MAPPED_W                 1301L
#define ERROR_NO_QUOTAS_FOR_ACCOUNT_W           1302L
#define ERROR_LOCAL_USER_SESSION_KEY_W          1303L
#define ERROR_NULL_LM_PASSWORD_W                1304L
#define ERROR_UNKNOWN_REVISION_W                1305L
#define ERROR_REVISION_MISMATCH_W               1306L
#define ERROR_INVALID_OWNER_W                   1307L
#define ERROR_INVALID_PRIMARY_GROUP_W           1308L
#define ERROR_NO_IMPERSONATION_TOKEN_W          1309L
#define ERROR_CANT_DISABLE_MANDATORY_W          1310L
#define ERROR_NO_LOGON_SERVERS_W                1311L
#define ERROR_NO_SUCH_LOGON_SESSION_W           1312L
#define ERROR_NO_SUCH_PRIVILEGE_W               1313L
#define ERROR_PRIVILEGE_NOT_HELD_W              1314L
#define ERROR_INVALID_ACCOUNT_NAME_W            1315L
#define ERROR_USER_EXISTS_W                     1316L
#define ERROR_NO_SUCH_USER_W                    1317L
#define ERROR_GROUP_EXISTS_W                    1318L
#define ERROR_NO_SUCH_GROUP_W                   1319L
#define ERROR_MEMBER_IN_GROUP_W                 1320L
#define ERROR_MEMBER_NOT_IN_GROUP_W             1321L
#define ERROR_LAST_ADMIN_W                      1322L
#define ERROR_WRONG_PASSWORD_W                  1323L
#define ERROR_ILL_FORMED_PASSWORD_W             1324L
#define ERROR_PASSWORD_RESTRICTION_W            1325L
#define ERROR_LOGON_FAILURE_W                   1326L
#define ERROR_ACCOUNT_RESTRICTION_W             1327L
#define ERROR_INVALID_LOGON_HOURS_W             1328L
#define ERROR_INVALID_WORKSTATION_W             1329L
#define ERROR_PASSWORD_EXPIRED_W                1330L
#define ERROR_ACCOUNT_DISABLED_W                1331L
#define ERROR_NONE_MAPPED_W                     1332L
#define ERROR_TOO_MANY_LUIDS_REQUESTED_W        1333L
#define ERROR_LUIDS_EXHAUSTED_W                 1334L
#define ERROR_INVALID_SUB_AUTHORITY_W           1335L
#define ERROR_INVALID_ACL_W                     1336L
#define ERROR_INVALID_SID_W                     1337L
#define ERROR_INVALID_SECURITY_DESCR_W          1338L
#define ERROR_BAD_INHERITANCE_ACL_W             1340L
#define ERROR_SERVER_DISABLED_W                 1341L
#define ERROR_SERVER_NOT_DISABLED_W             1342L
#define ERROR_INVALID_ID_AUTHORITY_W            1343L
#define ERROR_ALLOTTED_SPACE_EXCEEDED_W         1344L
#define ERROR_INVALID_GROUP_ATTRIBUTES_W        1345L
#define ERROR_BAD_IMPERSONATION_LEVEL_W         1346L
#define ERROR_CANT_OPEN_ANONYMOUS_W             1347L
#define ERROR_BAD_VALIDATION_CLASS_W            1348L
#define ERROR_BAD_TOKEN_TYPE_W                  1349L
#define ERROR_NO_SECURITY_ON_OBJECT_W           1350L
#define ERROR_CANT_ACCESS_DOMAIN_INFO_W         1351L
#define ERROR_INVALID_SERVER_STATE_W            1352L
#define ERROR_INVALID_DOMAIN_STATE_W            1353L
#define ERROR_INVALID_DOMAIN_ROLE_W             1354L
#define ERROR_NO_SUCH_DOMAIN_W                  1355L
#define ERROR_DOMAIN_EXISTS_W                   1356L
#define ERROR_DOMAIN_LIMIT_EXCEEDED_W           1357L
#define ERROR_INTERNAL_DB_CORRUPTION_W          1358L
#define ERROR_INTERNAL_ERROR_W                  1359L
#define ERROR_GENERIC_NOT_MAPPED_W              1360L
#define ERROR_BAD_DESCRIPTOR_FORMAT_W           1361L
#define ERROR_NOT_LOGON_PROCESS_W               1362L
#define ERROR_LOGON_SESSION_EXISTS_W            1363L
#define ERROR_NO_SUCH_PACKAGE_W                 1364L
#define ERROR_BAD_LOGON_SESSION_STATE_W         1365L
#define ERROR_LOGON_SESSION_COLLISION_W         1366L
#define ERROR_INVALID_LOGON_TYPE_W              1367L
#define ERROR_CANNOT_IMPERSONATE_W              1368L
#define ERROR_RXACT_INVALID_STATE_W             1369L
#define ERROR_RXACT_COMMIT_FAILURE_W            1370L
#define ERROR_SPECIAL_ACCOUNT_W                 1371L
#define ERROR_SPECIAL_GROUP_W                   1372L
#define ERROR_SPECIAL_USER_W                    1373L
#define ERROR_MEMBERS_PRIMARY_GROUP_W           1374L
#define ERROR_TOKEN_ALREADY_IN_USE_W            1375L
#define ERROR_NO_SUCH_ALIAS_W                   1376L
#define ERROR_MEMBER_NOT_IN_ALIAS_W             1377L
#define ERROR_MEMBER_IN_ALIAS_W                 1378L
#define ERROR_ALIAS_EXISTS_W                    1379L
#define ERROR_LOGON_NOT_GRANTED_W               1380L
#define ERROR_TOO_MANY_SECRETS_W                1381L
#define ERROR_SECRET_TOO_LONG_W                 1382L
#define ERROR_INTERNAL_DB_ERROR_W               1383L
#define ERROR_TOO_MANY_CONTEXT_IDS_W            1384L
#define ERROR_LOGON_TYPE_NOT_GRANTED_W          1385L
#define ERROR_NT_CROSS_ENCRYPTION_REQUIRED_W    1386L
#define ERROR_NO_SUCH_MEMBER_W                  1387L
#define ERROR_INVALID_MEMBER_W                  1388L
#define ERROR_TOO_MANY_SIDS_W                   1389L
#define ERROR_LM_CROSS_ENCRYPTION_REQUIRED_W    1390L
#define ERROR_NO_INHERITANCE_W                  1391L
#define ERROR_FILE_CORRUPT_W                    1392L
#define ERROR_DISK_CORRUPT_W                    1393L
#define ERROR_NO_USER_SESSION_KEY_W             1394L
#define ERROR_INVALID_WINDOW_HANDLE_W           1400L
#define ERROR_INVALID_MENU_HANDLE_W             1401L
#define ERROR_INVALID_CURSOR_HANDLE_W           1402L
#define ERROR_INVALID_ACCEL_HANDLE_W            1403L
#define ERROR_INVALID_HOOK_HANDLE_W             1404L
#define ERROR_INVALID_DWP_HANDLE_W              1405L
#define ERROR_TLW_WITH_WSCHILD_W                1406L
#define ERROR_CANNOT_FIND_WND_CLASS_W           1407L
#define ERROR_WINDOW_OF_OTHER_THREAD_W          1408L
#define ERROR_HOTKEY_ALREADY_REGISTERED_W       1409L
#define ERROR_CLASS_ALREADY_EXISTS_W            1410L
#define ERROR_CLASS_DOES_NOT_EXIST_W            1411L
#define ERROR_CLASS_HAS_WINDOWS_W               1412L
#define ERROR_INVALID_INDEX_W                   1413L
#define ERROR_INVALID_ICON_HANDLE_W             1414L
#define ERROR_PRIVATE_DIALOG_INDEX_W            1415L
#define ERROR_LISTBOX_ID_NOT_FOUND_W            1416L
#define ERROR_NO_WILDCARD_CHARACTERS_W          1417L
#define ERROR_CLIPBOARD_NOT_OPEN_W              1418L
#define ERROR_HOTKEY_NOT_REGISTERED_W           1419L
#define ERROR_WINDOW_NOT_DIALOG_W               1420L
#define ERROR_CONTROL_ID_NOT_FOUND_W            1421L
#define ERROR_INVALID_COMBOBOX_MESSAGE_W        1422L
#define ERROR_WINDOW_NOT_COMBOBOX_W             1423L
#define ERROR_INVALID_EDIT_HEIGHT_W             1424L
#define ERROR_DC_NOT_FOUND_W                    1425L
#define ERROR_INVALID_HOOK_FILTER_W             1426L
#define ERROR_INVALID_FILTER_PROC_W             1427L
#define ERROR_HOOK_NEEDS_HMOD_W                 1428L
#define ERROR_GLOBAL_ONLY_HOOK_W                1429L
#define ERROR_JOURNAL_HOOK_SET_W                1430L
#define ERROR_HOOK_NOT_INSTALLED_W              1431L
#define ERROR_INVALID_LB_MESSAGE_W              1432L
#define ERROR_SETCOUNT_ON_BAD_LB_W              1433L
#define ERROR_LB_WITHOUT_TABSTOPS_W             1434L
#define ERROR_DESTROY_OBJECT_OF_OTHER_THREAD_W  1435L
#define ERROR_CHILD_WINDOW_MENU_W               1436L
#define ERROR_NO_SYSTEM_MENU_W                  1437L
#define ERROR_INVALID_MSGBOX_STYLE_W            1438L
#define ERROR_INVALID_SPI_VALUE_W               1439L
#define ERROR_SCREEN_ALREADY_LOCKED_W           1440L
#define ERROR_HWNDS_HAVE_DIFF_PARENT_W          1441L
#define ERROR_NOT_CHILD_WINDOW_W                1442L
#define ERROR_INVALID_GW_COMMAND_W              1443L
#define ERROR_INVALID_THREAD_ID_W               1444L
#define ERROR_NON_MDICHILD_WINDOW_W             1445L
#define ERROR_POPUP_ALREADY_ACTIVE_W            1446L
#define ERROR_NO_SCROLLBARS_W                   1447L
#define ERROR_INVALID_SCROLLBAR_RANGE_W         1448L
#define ERROR_INVALID_SHOWWIN_COMMAND_W         1449L
#define ERROR_COMMITMENT_LIMIT_W                1455L
#define ERROR_EVENTLOG_FILE_CORRUPT_W           1500L
#define ERROR_EVENTLOG_CANT_START_W             1501L
#define ERROR_LOG_FILE_FULL_W                   1502L
#define ERROR_EVENTLOG_FILE_CHANGED_W           1503L
#define ERROR_INVALID_USER_BUFFER_W             1784L
#define ERROR_UNRECOGNIZED_MEDIA_W              1785L
#define ERROR_NO_TRUST_LSA_SECRET_W             1786L
#define ERROR_NO_TRUST_SAM_ACCOUNT_W            1787L
#define ERROR_TRUSTED_DOMAIN_FAILURE_W          1788L
#define ERROR_TRUSTED_RELATIONSHIP_FAILURE_W    1789L
#define ERROR_TRUST_FAILURE_W                   1790L
#define ERROR_NETLOGON_NOT_STARTED_W            1792L
#define ERROR_ACCOUNT_EXPIRED_W                 1793L
#define ERROR_REDIRECTOR_HAS_OPEN_HANDLES_W     1794L
#define ERROR_PRINTER_DRIVER_ALREADY_INSTALLED_W 1795L
#define ERROR_UNKNOWN_PORT_W                    1796L
#define ERROR_UNKNOWN_PRINTER_DRIVER_W          1797L
#define ERROR_UNKNOWN_PRINTPROCESSOR_W          1798L
#define ERROR_INVALID_SEPARATOR_FILE_W          1799L
#define ERROR_INVALID_PRIORITY_W                1800L
#define ERROR_INVALID_PRINTER_NAME_W            1801L
#define ERROR_PRINTER_ALREADY_EXISTS_W          1802L
#define ERROR_INVALID_PRINTER_COMMAND_W         1803L
#define ERROR_INVALID_DATATYPE_W                1804L
#define ERROR_INVALID_ENVIRONMENT_W             1805L
#define ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT_W 1807L
#define ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT_W 1808L
#define ERROR_NOLOGON_SERVER_TRUST_ACCOUNT_W    1809L
#define ERROR_DOMAIN_TRUST_INCONSISTENT_W       1810L
#define ERROR_SERVER_HAS_OPEN_HANDLES_W         1811L
#define ERROR_RESOURCE_DATA_NOT_FOUND_W         1812L
#define ERROR_RESOURCE_TYPE_NOT_FOUND_W         1813L
#define ERROR_RESOURCE_NAME_NOT_FOUND_W         1814L
#define ERROR_RESOURCE_LANG_NOT_FOUND_W         1815L
#define ERROR_NOT_ENOUGH_QUOTA_W                1816L
#define ERROR_INVALID_TIME_W                    1901L
#define ERROR_INVALID_FORM_NAME_W               1902L
#define ERROR_INVALID_FORM_SIZE_W               1903L
#define ERROR_ALREADY_WAITING_W                 1904L
#define ERROR_PRINTER_DELETED_W                 1905L
#define ERROR_INVALID_PRINTER_STATE_W           1906L
#define ERROR_PASSWORD_MUST_CHANGE_W            1907L
#define ERROR_DOMAIN_CONTROLLER_NOT_FOUND_W     1908L
#define ERROR_ACCOUNT_LOCKED_OUT_W              1909L
#define ERROR_INVALID_PIXEL_FORMAT_W            2000L
#define ERROR_BAD_DRIVER_W                      2001L
#define ERROR_INVALID_WINDOW_STYLE_W            2002L
#define ERROR_METAFILE_NOT_SUPPORTED_W          2003L
#define ERROR_TRANSFORM_NOT_SUPPORTED_W         2004L
#define ERROR_CLIPPING_NOT_SUPPORTED_W          2005L
#define ERROR_BAD_USERNAME_W                    2202L
#define ERROR_NOT_CONNECTED_W                   2250L
#define ERROR_OPEN_FILES_W                      2401L
#define ERROR_ACTIVE_CONNECTIONS_W              2402L
#define ERROR_DEVICE_IN_USE_W                   2404L
#define ERROR_UNKNOWN_PRINT_MONITOR_W           3000L
#define ERROR_PRINTER_DRIVER_IN_USE_W           3001L
#define ERROR_SPOOL_FILE_NOT_FOUND_W            3002L
#define ERROR_SPL_NO_STARTDOC_W                 3003L
#define ERROR_SPL_NO_ADDJOB_W                   3004L
#define ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED_W 3005L
#define ERROR_PRINT_MONITOR_ALREADY_INSTALLED_W 3006L
#define ERROR_WINS_INTERNAL_W                   4000L
#define ERROR_CAN_NOT_DEL_LOCAL_WINS_W          4001L
#define ERROR_STATIC_INIT_W                     4002L
#define ERROR_INC_BACKUP_W                      4003L
#define ERROR_FULL_BACKUP_W                     4004L
#define ERROR_REC_NON_EXISTENT_W                4005L
#define ERROR_RPL_NOT_ALLOWED_W                 4006L

/* Access rights */

#define DELETE_W                     0x00010000
#define READ_CONTROL_W               0x00020000
#define WRITE_DAC_W                  0x00040000
#define WRITE_OWNER_W                0x00080000
#define SYNCHRONIZE_W                0x00100000
#define STANDARD_RIGHTS_REQUIRED_W   0x000f0000

#define STANDARD_RIGHTS_READ_W       READ_CONTROL_W
#define STANDARD_RIGHTS_WRITE_W      READ_CONTROL_W
#define STANDARD_RIGHTS_EXECUTE_W    READ_CONTROL_W

#define STANDARD_RIGHTS_ALL_W        0x001f0000

#define SPECIFIC_RIGHTS_ALL_W        0x0000ffff

#define EVENT_MODIFY_STATE_W         0x0002
#define EVENT_ALL_ACCESS_W           (STANDARD_RIGHTS_REQUIRED_W|SYNCHRONIZE_W|0x3)

#define SEMAPHORE_MODIFY_STATE_W     0x0002
#define SEMAPHORE_ALL_ACCESS_W       (STANDARD_RIGHTS_REQUIRED_W|SYNCHRONIZE_W|0x3)

#define MUTEX_MODIFY_STATE_W         0x0001
#define MUTEX_ALL_ACCESS_W           (STANDARD_RIGHTS_REQUIRED_W|SYNCHRONIZE_W|0x1)

#define MAXIMUM_ALLOWED_W            0x02000000
#define ACCESS_SYSTEM_SECURITY_W     0x01000000

#ifndef LOWORD
#define LOWORD(l)              ((WORD)(DWORD)(l))
#endif
#ifndef HIWORD
#define HIWORD(l)              ((WORD)((DWORD)(l) >> 16))
#endif

  /* biCompression */
#define BI_RGB_W           0
#define BI_RLE8_W          1
#define BI_RLE4_W          2
#define BI_BITFIELDS_W     3

  /* Polygon modes */
#define ALTERNATE_W         1
#define WINDING_W           2

/* Regions */
#define ERROR_W             0
#define NULLREGION_W        1
#define SIMPLEREGION_W      2
#define COMPLEXREGION_W     3

#define RGN_AND_W           1
#define RGN_OR_W            2
#define RGN_XOR_W           3
#define RGN_DIFF_W          4
#define RGN_COPY_W          5


#define PAGE_NOACCESS_W                 0x01
#define PAGE_READONLY_W                 0x02
#define PAGE_READWRITE_W                0x04
#define PAGE_WRITECOPY_W                0x08
#define PAGE_EXECUTE_W                  0x10
#define PAGE_EXECUTE_READ_W             0x20
#define PAGE_EXECUTE_READWRITE_W        0x40
#define PAGE_EXECUTE_WRITECOPY_W        0x80
#define PAGE_GUARD_W                    0x100
#define PAGE_NOCACHE_W                  0x200

#define MEM_COMMIT_W                    0x00001000
#define MEM_RESERVE_W                   0x00002000
#define MEM_DECOMMIT_W                  0x00004000
#define MEM_RELEASE_W                   0x00008000
#define MEM_FREE_W                      0x00010000
#define MEM_PRIVATE_W                   0x00020000
#define MEM_MAPPED_W                    0x00040000
#define MEM_TOP_DOWN_W                  0x00100000

#define SEC_FILE_W                      0x00800000
#define SEC_IMAGE_W                     0x01000000
#define SEC_RESERVE_W                   0x04000000
#define SEC_COMMIT_W                    0x08000000
#define SEC_NOCACHE_W                   0x10000000

#define FILE_MAP_COPY_W                 0x00000001
#define FILE_MAP_WRITE_W                0x00000002
#define FILE_MAP_READ_W                 0x00000004
#define FILE_MAP_ALL_ACCESS_W           0x000f001f

#define SND_SYNC_W              0x0000  /* play synchronously (default) */
#define SND_ASYNC_W             0x0001  /* play asynchronously */
#define SND_NODEFAULT_W         0x0002  /* don't use default sound */
#define SND_MEMORY_W            0x0004  /* lpszSoundName points to a memory file */
#define SND_LOOP_W              0x0008  /* loop the sound until next sndPlaySound */
#define SND_NOSTOP_W            0x0010  /* don't stop any currently playing sound */

#define SND_NOWAIT_W            0x00002000L /* don't wait if the driver is busy */
#define SND_ALIAS_W             0x00010000L /* name is a registry alias */
#define SND_ALIAS_ID_W          0x00110000L /* alias is a predefined ID */
#define SND_FILENAME_W          0x00020000L /* name is file name */
#define SND_RESOURCE_W          0x00040004L /* name is resource name or atom */
#define SND_PURGE_W             0x00000040L /* purge all sounds */
#define SND_APPLICATION_W       0x00000080L /* look for application specific association */


#define REG_NONE                         0  /* no type */
#define REG_SZ                           1  /* string type (ASCII) */
#define REG_EXPAND_SZ                    2  /* string, includes %ENVVAR% (expanded by caller) (ASCII) */
#define REG_BINARY                       3  /* binary format, callerspecific */
                                /* YES, REG_DWORD == REG_DWORD_LITTLE_ENDIAN */
#define REG_DWORD                        4  /* DWORD in little endian format */
#define REG_DWORD_LITTLE_ENDIAN          4  /* DWORD in little endian format */
#define REG_DWORD_BIG_ENDIAN             5  /* DWORD in big endian format  */
#define REG_LINK                         6  /* symbolic link (UNICODE) */
#define REG_MULTI_SZ                     7  /* multiple strings, delimited by \0, terminated by \0\0 (ASCII) */
#define REG_RESOURCE_LIST                8  /* resource list? huh? */
#define REG_FULL_RESOURCE_DESCRIPTOR     9  /* full resource descriptor? huh? */

#define KEY_QUERY_VALUE         0x00000001
#define KEY_SET_VALUE           0x00000002
#define KEY_CREATE_SUB_KEY      0x00000004
#define KEY_ENUMERATE_SUB_KEYS  0x00000008
#define KEY_NOTIFY              0x00000010
#define KEY_CREATE_LINK         0x00000020

#define KEY_READ                (STANDARD_RIGHTS_READ_W|  \
                                 KEY_QUERY_VALUE|       \
                                 KEY_ENUMERATE_SUB_KEYS|\
                                 KEY_NOTIFY             \
                                )
#define KEY_WRITE               (STANDARD_RIGHTS_WRITE_W| \
                                 KEY_SET_VALUE|         \
                                 KEY_CREATE_SUB_KEY     \
                                )
#define KEY_EXECUTE             KEY_READ
#define KEY_ALL_ACCESS          (STANDARD_RIGHTS_ALL_W|   \
                                 KEY_READ|KEY_WRITE|    \
                                 KEY_CREATE_LINK        \
                                )

#define HKEY_CLASSES_ROOT       ((HKEY) 0x80000000)
#define HKEY_CURRENT_USER       ((HKEY) 0x80000001)
#define HKEY_LOCAL_MACHINE      ((HKEY) 0x80000002)
#define HKEY_USERS              ((HKEY) 0x80000003)
#define HKEY_PERFORMANCE_DATA   ((HKEY) 0x80000004)
#define HKEY_CURRENT_CONFIG     ((HKEY) 0x80000005)
#define HKEY_DYN_DATA           ((HKEY) 0x80000006)

/* Virtual key codes */
#define VK_LBUTTON_W          0x01
#define VK_RBUTTON_W          0x02
#define VK_CANCEL_W           0x03
#define VK_MBUTTON_W          0x04
/*                          0x05-0x07  Undefined */
#define VK_BACK_W             0x08
#define VK_TAB_W              0x09
/*                          0x0A-0x0B  Undefined */
#define VK_CLEAR_W            0x0C
#define VK_RETURN_W           0x0D
/*                          0x0E-0x0F  Undefined */
#define VK_SHIFT_W            0x10
#define VK_CONTROL_W          0x11
#define VK_MENU_W             0x12
#define VK_PAUSE_W            0x13
#define VK_CAPITAL_W          0x14
/*                          0x15-0x19  Reserved for Kanji systems */
/*                          0x1A       Undefined */
#define VK_ESCAPE_W           0x1B
/*                          0x1C-0x1F  Reserved for Kanji systems */
#define VK_SPACE_W            0x20
#define VK_PRIOR_W            0x21
#define VK_NEXT_W             0x22
#define VK_END_W              0x23
#define VK_HOME_W             0x24
#define VK_LEFT_W             0x25
#define VK_UP_W               0x26
#define VK_RIGHT_W            0x27
#define VK_DOWN_W             0x28
#define VK_SELECT_W           0x29
#define VK_PRINT_W            0x2A /* OEM specific in Windows 3.1 SDK */
#define VK_EXECUTE_W          0x2B
#define VK_SNAPSHOT_W         0x2C
#define VK_INSERT_W           0x2D
#define VK_DELETE_W           0x2E
#define VK_HELP_W             0x2F
#define VK_0_W                0x30
#define VK_1_W                0x31
#define VK_2_W                0x32
#define VK_3_W                0x33
#define VK_4_W                0x34
#define VK_5_W                0x35
#define VK_6_W                0x36
#define VK_7_W                0x37
#define VK_8_W                0x38
#define VK_9_W                0x39
/*                          0x3A-0x40  Undefined */
#define VK_A_W                0x41
#define VK_B_W                0x42
#define VK_C_W                0x43
#define VK_D_W                0x44
#define VK_E_W                0x45
#define VK_F_W                0x46
#define VK_G_W                0x47
#define VK_H_W                0x48
#define VK_I_W                0x49
#define VK_J_W                0x4A
#define VK_K_W                0x4B
#define VK_L_W                0x4C
#define VK_M_W                0x4D
#define VK_N_W                0x4E
#define VK_O_W                0x4F
#define VK_P_W                0x50
#define VK_Q_W                0x51
#define VK_R_W                0x52
#define VK_S_W                0x53
#define VK_T_W                0x54
#define VK_U_W                0x55
#define VK_V_W                0x56
#define VK_W_W                0x57
#define VK_X_W                0x58
#define VK_Y_W                0x59
#define VK_Z_W                0x5A

#define VK_LWIN_W             0x5B
#define VK_RWIN_W             0x5C
#define VK_APPS_W             0x5D
/*                          0x5E-0x5F Unassigned */
#define VK_NUMPAD0_W          0x60
#define VK_NUMPAD1_W          0x61
#define VK_NUMPAD2_W          0x62
#define VK_NUMPAD3_W          0x63
#define VK_NUMPAD4_W          0x64
#define VK_NUMPAD5_W          0x65
#define VK_NUMPAD6_W          0x66
#define VK_NUMPAD7_W          0x67
#define VK_NUMPAD8_W          0x68
#define VK_NUMPAD9_W          0x69
#define VK_MULTIPLY_W         0x6A
#define VK_ADD_W              0x6B
#define VK_SEPARATOR_W        0x6C
#define VK_SUBTRACT_W         0x6D
#define VK_DECIMAL_W          0x6E
#define VK_DIVIDE_W           0x6F
#define VK_F1_W               0x70
#define VK_F2_W               0x71
#define VK_F3_W               0x72
#define VK_F4_W               0x73
#define VK_F5_W               0x74
#define VK_F6_W               0x75
#define VK_F7_W               0x76
#define VK_F8_W               0x77
#define VK_F9_W               0x78
#define VK_F10_W              0x79
#define VK_F11_W              0x7A
#define VK_F12_W              0x7B
#define VK_F13_W              0x7C
#define VK_F14_W              0x7D
#define VK_F15_W              0x7E
#define VK_F16_W              0x7F
#define VK_F17_W              0x80
#define VK_F18_W              0x81
#define VK_F19_W              0x82
#define VK_F20_W              0x83
#define VK_F21_W              0x84
#define VK_F22_W              0x85
#define VK_F23_W              0x86
#define VK_F24_W              0x87
/*                          0x88-0x8F  Unassigned */
#define VK_NUMLOCK_W          0x90
#define VK_SCROLL_W           0x91
/*                          0x92-0x9F  Unassigned */
/*
 * differencing between right and left shift/control/alt key.
 * Used only by GetAsyncKeyState() and GetKeyState().
 */
#define VK_LSHIFT_W           0xA0
#define VK_RSHIFT_W           0xA1
#define VK_LCONTROL_W         0xA2
#define VK_RCONTROL_W         0xA3
#define VK_LMENU_W            0xA4
#define VK_RMENU_W            0xA5
/*                          0xA6-0xB9  Unassigned */
#define VK_OEM_1_W            0xBA
#define VK_OEM_PLUS_W         0xBB
#define VK_OEM_COMMA_W        0xBC
#define VK_OEM_MINUS_W        0xBD
#define VK_OEM_PERIOD_W       0xBE
#define VK_OEM_2_W            0xBF
#define VK_OEM_3_W            0xC0
/*                          0xC1-0xDA  Unassigned */
#define VK_OEM_4_W            0xDB
#define VK_OEM_5_W            0xDC
#define VK_OEM_6_W            0xDD
#define VK_OEM_7_W            0xDE
/*                          0xDF-0xE4  OEM specific */

#define VK_PROCESSKEY_W       0xE5

/*                          0xE6       OEM specific */
/*                          0xE7-0xE8  Unassigned */
/*                          0xE9-0xF5  OEM specific */

#define VK_ATTN_W             0xF6
#define VK_CRSEL_W            0xF7
#define VK_EXSEL_W            0xF8
#define VK_EREOF_W            0xF9
#define VK_PLAY_W             0xFA
#define VK_ZOOM_W             0xFB
#define VK_NONAME_W           0xFC
#define VK_PA1_W              0xFD
#define VK_OEM_CLEAR_W        0xFE

/**************************************************************/
/* The following vkey definitions are undocumented in Windows */
/**************************************************************/
#define VK_SEMICOLON_W       0xBA
#define VK_EQUAL_W           0xBB
#define VK_COMMA_W           0xBC
#define VK_HYPHEN_W          0xBD
#define VK_PERIOD_W          0xBE
#define VK_SLASH_W           0xBF
#define VK_GRAVE_W           0xC0
#define VK_BRACKETLEFT_W     0xDB
#define VK_BACKSLASH_W       0xDC
#define VK_BRACKETRIGHT_W    0xDD
#define VK_QUOTESINGLE_W     0xDE
#define VK_EXTRA_W           0xE2

/* Heap flags */

#define HEAP_NO_SERIALIZE               0x00000001
#define HEAP_GROWABLE                   0x00000002
#define HEAP_GENERATE_EXCEPTIONS        0x00000004
#define HEAP_ZERO_MEMORY                0x00000008
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020
#define HEAP_FREE_CHECKING_ENABLED      0x00000040
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080
#define HEAP_CREATE_ALIGN_16            0x00010000
#define HEAP_CREATE_ENABLE_TRACING      0x00020000

/* FindFirstFile */

#define INVALID_HANDLE_VALUE_W  ((HANDLE) -1)

  /* StretchBlt() modes */
#define BLACKONWHITE_W         1
#define WHITEONBLACK_W         2
#define COLORONCOLOR_W	       3
#define HALFTONE_W             4
#define MAXSTRETCHBLTMODE_W    4

    #define    NTRT_NEWRESOURCE      0x2000
    #define    NTRT_ERROR            0x7fff
    #define    NTRT_CURSOR           1
    #define    NTRT_BITMAP           2
    #define    NTRT_ICON             3
    #define    NTRT_MENU             4
    #define    NTRT_DIALOG           5
    #define    NTRT_STRING           6
    #define    NTRT_FONTDIR          7
    #define    NTRT_FONT             8
    #define    NTRT_ACCELERATORS     9
    #define    NTRT_RCDATA           10
    #define    NTRT_MESSAGETABLE     11
    #define    NTRT_GROUP_CURSOR     12
    #define    NTRT_GROUP_ICON       14
    #define    NTRT_VERSION          16
    #define    NTRT_NEWBITMAP        (NTRT_BITMAP|NTRT_NEWRESOURCE)
    #define    NTRT_NEWMENU          (NTRT_MENU|NTRT_NEWRESOURCE)
    #define    NTRT_NEWDIALOG        (NTRT_DIALOG|NTRT_NEWRESOURCE)


/* Device parameters for GetDeviceCaps() */
#define DRIVERVERSION_W     0
#define TECHNOLOGY_W        2
#define HORZSIZE_W          4
#define VERTSIZE_W          6
#define HORZRES_W           8
#define VERTRES_W           10
#define BITSPIXEL_W         12
#define PLANES_W            14
#define NUMBRUSHES_W        16
#define NUMPENS_W           18
#define NUMMARKERS_W        20
#define NUMFONTS_W          22
#define NUMCOLORS_W         24
#define PDEVICESIZE_W       26
#define CURVECAPS_W         28
#define LINECAPS_W          30
#define POLYGONALCAPS_W     32
#define TEXTCAPS_W          34
#define CLIPCAPS_W          36
#define RASTERCAPS_W        38
#define ASPECTX_W           40
#define ASPECTY_W           42
#define ASPECTXY_W          44
#define LOGPIXELSX_W        88
#define LOGPIXELSY_W        90
#define SIZEPALETTE_W       104
#define NUMRESERVED_W       106
#define COLORRES_W          108
#define PHYSICALWIDTH_W     110 
#define PHYSICALHEIGHT_W    111 
#define PHYSICALOFFSETX_W   112 
#define PHYSICALOFFSETY_W   113 
#define SCALINGFACTORX_W    114 
#define SCALINGFACTORY_W    115 

#define OF_READ_W               0x0000
#define OF_WRITE_W              0x0001
#define OF_READWRITE_W          0x0002
#define OF_SHARE_COMPAT_W       0x0000
#define OF_SHARE_EXCLUSIVE_W    0x0010
#define OF_SHARE_DENY_WRITE_W   0x0020
#define OF_SHARE_DENY_READ_W    0x0030
#define OF_SHARE_DENY_NONE_W    0x0040
#define OF_PARSE_W              0x0100
#define OF_DELETE_W             0x0200
#define OF_VERIFY_W             0x0400   /* Used with OF_REOPEN */
#define OF_SEARCH_W             0x0400   /* Used without OF_REOPEN */
#define OF_CANCEL_W             0x0800
#define OF_CREATE_W             0x1000
#define OF_PROMPT_W             0x2000
#define OF_EXIST_W              0x4000
#define OF_REOPEN_W             0x8000

#define HFILE_ERROR_W     	((HFILE)-1)

// LockFileEx flags

#define LOCKFILE_FAIL_IMMEDIATELY_W   0x00000001
#define LOCKFILE_EXCLUSIVE_LOCK_W     0x00000002

#define FILE_BEGIN_W              0
#define FILE_CURRENT_W            1
#define FILE_END_W                2

/* Graphics Modes */
#define GM_COMPATIBLE_W     1
#define GM_ADVANCED_W       2
#define GM_LAST_W           2

#define	RDH_RECTANGLES_W  1

//nls
#define MAKELANGID(p, s)                 ((((WORD)(s))<<10) | (WORD)(p))

#define LANG_NEUTRAL                     0x00

/* Sublanguage definitions */
#define SUBLANG_NEUTRAL                  0x00    /* language neutral */
#define SUBLANG_DEFAULT                  0x01    /* user default */
#define SUBLANG_SYS_DEFAULT              0x02    /* system default */


/* Flags for ModifyWorldTransform */
#define MWT_IDENTITY_W      1
#define MWT_LEFTMULTIPLY_W  2
#define MWT_RIGHTMULTIPLY_W 3

  /* Map modes */
#define MM_TEXT_W		  1
#define MM_LOMETRIC_W	  2
#define MM_HIMETRIC_W	  3
#define MM_LOENGLISH_W	  4
#define MM_HIENGLISH_W	  5
#define MM_TWIPS_W	  6
#define MM_ISOTROPIC_W	  7
#define MM_ANISOTROPIC_W  8


#define DRIVE_UNKNOWN_W              0
#define DRIVE_NO_ROOT_DIR_W          1
#define DRIVE_CANNOTDETERMINE_W      0
#define DRIVE_DOESNOTEXIST_W         1
#define DRIVE_REMOVABLE_W            2
#define DRIVE_FIXED_W                3
#define DRIVE_REMOTE_W               4
/* Win32 additions */
#define DRIVE_CDROM_W                5
#define DRIVE_RAMDISK_W              6


/* GetWindow() constants */
#define GW_HWNDFIRST_W    0
#define GW_HWNDLAST_W     1
#define GW_HWNDNEXT_W     2
#define GW_HWNDPREV_W     3
#define GW_OWNER_W        4
#define GW_CHILD_W        5

/* WM_NCCALCSIZE return flags */
#define WVR_ALIGNTOP_W        0x0010
#define WVR_ALIGNLEFT_W       0x0020
#define WVR_ALIGNBOTTOM_W     0x0040
#define WVR_ALIGNRIGHT_W      0x0080
#define WVR_HREDRAW_W         0x0100
#define WVR_VREDRAW_W         0x0200
#define WVR_REDRAW_W          (WVR_HREDRAW_W | WVR_VREDRAW_W)
#define WVR_VALIDRECTS_W      0x0400


#define STATUS_WAIT_0_W                    0x00000000
#define STATUS_ABANDONED_WAIT_0_W          0x00000080
#define STATUS_USER_APC_W                  0x000000C0
#define STATUS_TIMEOUT_W                   0x00000102

#define WAIT_FAILED_W             0xffffffff
#define WAIT_OBJECT_0_W           0
#define WAIT_ABANDONED_W          STATUS_ABANDONED_WAIT_0_W
#define WAIT_ABANDONED_0_W        STATUS_ABANDONED_WAIT_0_W
#define WAIT_IO_COMPLETION_W      STATUS_USER_APC_W
#define WAIT_TIMEOUT_W            STATUS_TIMEOUT_W
