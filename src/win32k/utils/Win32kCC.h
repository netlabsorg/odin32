/* $Id: Win32kCC.h,v 1.1.2.1 2000-09-02 20:47:26 bird Exp $
 *
 *
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * General PushButton ID(s)
 */
#define PB_APPLY                    0xfffe
#define PB_REFRESH                  0xfffd


/*
 * String IDs.
 */
#define  IDS_ERR_DIALOGLOAD             1
#define  IDS_ERR_MALLOC_FAILED          2
#define  IDS_ERR_SET_INSTACEDATA        3
#define  IDS_ERR_WIN32K_NOT_LOADED      4
#define  IDS_ERR_WIN32K_OPEN_FAILED     5
#define  IDS_ERR_QUERYOPTIONSTATUS      6
#define  IDS_ERR_NO_COM_RADIOBUTTON     7
#define  IDS_ERR_NO_PE_RADIOBUTTON      8
#define  IDS_ERR_INVALID_INFOLEVEL      9
#define  IDS_ERR_INVALID_MAXHEAPSIZE   10
#define  IDS_ERR_SETPOPTIONS           11

/*
 * Message IDs.
 */
#define  IDM_INFO_DIRTY             1


/*
 * The Win32kCC (main) dialog.
 */
#define DL_WIN32KCC                 1000

#define GR_LOGGING                  1010
#define CB_LOGGING_ENABLED          1011
#define RB_LOGGING_COM1             1012
#define RB_LOGGING_COM2             1013
#define RB_LOGGING_COM3             1014
#define RB_LOGGING_COM4             1015


#define GR_LOADERS                  1030
#define CB_LDR_DISABLE_ALL          1031

#define RB_LDR_PE                   1040
#define RB_LDR_PE_PURE              1041
#define RB_LDR_PE_MIXED             1042
#define RB_LDR_PE_PE                1043
#define RB_LDR_PE_NOT               1044
#define TX_LDR_PE_INFOLEVEL         1045
#define SB_LDR_PE_INFOLEVEL         1046

#define GR_LDR_ELF                  1060
#define CB_LDR_ELF_ENABLED          1061
#define TX_LDR_ELF_INFOLEVEL        1062
#define SB_LDR_ELF_INFOLEVEL        1063

#define CB_LDR_SHELL_SCRIPTS        1070
#define CB_LDR_JAVA                 1080
#define CB_LDR_REXX                 1090


#define GR_HEAPS                    1150

#define GR_HEAP_RES                 1160
#define TX_HEAP_RES_MAX             1161
#define SB_HEAP_RES_MAX             1162
#define TX_HEAP_RES_INIT            1163
#define TX_HEAP_RES_INIT_VAL        1164
#define TX_HEAP_RES_USED            1165
#define TX_HEAP_RES_USED_VAL        1166

#define GR_HEAP_SWP                 1170
#define TX_HEAP_SWP_MAX             1171
#define SB_HEAP_SWP_MAX             1172
#define TX_HEAP_SWP_INIT            1173
#define TX_HEAP_SWP_INIT_VAL        1174
#define TX_HEAP_SWP_USED            1175
#define TX_HEAP_SWP_USED_VAL        1176

