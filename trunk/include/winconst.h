/* $Id: winconst.h,v 1.1 1999-08-20 20:11:06 sandervl Exp $ */

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

/* Window extended styles */
#define WS_EX_DLGMODALFRAME_W    0x00000001L
#define WS_EX_DRAGDETECT_W       0x00000002L
#define WS_EX_NOPARENTNOTIFY_W   0x00000004L
#define WS_EX_TOPMOST_W          0x00000008L
#define WS_EX_ACCEPTFILES_W      0x00000010L
#define WS_EX_TRANSPARENT_W      0x00000020L
