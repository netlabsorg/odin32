/* $Id: Dialog.h,v 1.2 2001-02-11 23:41:46 bird Exp $
 *
 * Dialog IDs.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
#define PB_UPD_CONFIGSYS            0xfffc


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

#define GR_LDR_PE                   1040
#define RB_LDR_PE_PURE              1041
#define RB_LDR_PE_MIXED             1042
#define RB_LDR_PE_PE                1043
#define RB_LDR_PE_NOT               1044
#define TX_LDR_PE_INFOLEVEL         1045
#define SB_LDR_PE_INFOLEVEL         1046
#define TX_LDR_PE_MODULES           1047
#define TX_LDR_PE_MODULES_VAL       1048
#define CK_LDR_PE_ONEOBJECT         1049

#define GR_LDR_ELF                  1060
#define CB_LDR_ELF_ENABLED          1061
#define TX_LDR_ELF_INFOLEVEL        1062
#define SB_LDR_ELF_INFOLEVEL        1063
#define TX_LDR_ELF_MODULES          1064
#define TX_LDR_ELF_MODULES_VAL      1065

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
#define TX_HEAP_RES_FREE            1167
#define TX_HEAP_RES_FREE_VAL        1168
#define TX_HEAP_RES_SIZE            1169
#define TX_HEAP_RES_SIZE_VAL        1170
#define TX_HEAP_RES_USED_BLOCKS     1171
#define TX_HEAP_RES_USED_BLOCKS_VAL 1172
#define TX_HEAP_RES_FREE_BLOCKS     1173
#define TX_HEAP_RES_FREE_BLOCKS_VAL 1174


#define GR_HEAP_SWP                 1180
#define TX_HEAP_SWP_MAX             1181
#define SB_HEAP_SWP_MAX             1182
#define TX_HEAP_SWP_INIT            1183
#define TX_HEAP_SWP_INIT_VAL        1184
#define TX_HEAP_SWP_USED            1185
#define TX_HEAP_SWP_USED_VAL        1186
#define TX_HEAP_SWP_FREE            1187
#define TX_HEAP_SWP_FREE_VAL        1188
#define TX_HEAP_SWP_SIZE            1189
#define TX_HEAP_SWP_SIZE_VAL        1190
#define TX_HEAP_SWP_USED_BLOCKS     1191
#define TX_HEAP_SWP_USED_BLOCKS_VAL 1192
#define TX_HEAP_SWP_FREE_BLOCKS     1193
#define TX_HEAP_SWP_FREE_BLOCKS_VAL 1194

#define GR_W32K                     1200
#define TX_W32K_VERSION             1201
#define TX_W32K_VERSION_VAL         1202
#define TX_W32K_BUILD_DATETIME      1203
#define TX_W32K_BUILD_DATETIME_VAL  1204
#define TX_W32K_SYMBOLFILE          1205
#define TX_W32K_SYMBOLFILE_VAL      1206
#define TX_W32K_KERNELBUILD         1207
#define TX_W32K_KERNELBUILD_VAL     1208

#define GR_LDRFIX                   1250
#define CB_LDRFIX_DLLFIXES          1251
#define CB_LDRFIX_EXEFIXES          1252
#define CB_LDRFIX_FORCEPRELOAD      1253

