/* $Id: Dialog.h,v 1.3 2001-02-17 20:25:44 bird Exp $
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
 * The Win32kCC (main) notebook dialog.
 */
#define DL_WIN32KCC                 1001
#define DL_WIN32KCC_OLD             1000
#define DL_WIN32KCC_NTBK            1002

/*
 * The Logging Page.
 */
#define DL_LOGGING_PAGE             1100
#define GR_LOGGING                  1101
#define CB_LOGGING_ENABLED          1102
#define RB_LOGGING_COM1             1103
#define RB_LOGGING_COM2             1104
#define RB_LOGGING_COM3             1105
#define RB_LOGGING_COM4             1106

/*
 * The Loader Page.
 */
#define DL_LOADERS_PAGE             1200
#define GR_LOADERS                  1201
#define CB_LDR_DISABLE_ALL          1202

#define GR_LDR_PE                   1210
#define RB_LDR_PE_PURE              1211
#define RB_LDR_PE_MIXED             1212
#define RB_LDR_PE_PE                1213
#define RB_LDR_PE_NOT               1214
#define TX_LDR_PE_INFOLEVEL         1215
#define SB_LDR_PE_INFOLEVEL         1216
#define TX_LDR_PE_MODULES           1217
#define TX_LDR_PE_MODULES_VAL       1218
#define CK_LDR_PE_ONEOBJECT         1219

#define GR_LDR_ELF                  1240
#define CB_LDR_ELF_ENABLED          1241
#define TX_LDR_ELF_INFOLEVEL        1242
#define SB_LDR_ELF_INFOLEVEL        1243
#define TX_LDR_ELF_MODULES          1244
#define TX_LDR_ELF_MODULES_VAL      1245

#define CB_LDR_SHELL_SCRIPTS        1260
#define CB_LDR_JAVA                 1270
#define CB_LDR_REXX                 1280

/*
 * The Heaps Page.
 */
#define DL_HEAPS_PAGE               1300
#define GR_HEAPS                    1301

#define GR_HEAP_RES                 1310
#define TX_HEAP_RES_MAX             1311
#define SB_HEAP_RES_MAX             1312
#define TX_HEAP_RES_INIT            1313
#define TX_HEAP_RES_INIT_VAL        1314
#define TX_HEAP_RES_USED            1315
#define TX_HEAP_RES_USED_VAL        1316
#define TX_HEAP_RES_FREE            1317
#define TX_HEAP_RES_FREE_VAL        1318
#define TX_HEAP_RES_SIZE            1319
#define TX_HEAP_RES_SIZE_VAL        1320
#define TX_HEAP_RES_USED_BLOCKS     1321
#define TX_HEAP_RES_USED_BLOCKS_VAL 1322
#define TX_HEAP_RES_FREE_BLOCKS     1323
#define TX_HEAP_RES_FREE_BLOCKS_VAL 1324

#define GR_HEAP_SWP                 1340
#define TX_HEAP_SWP_MAX             1341
#define SB_HEAP_SWP_MAX             1342
#define TX_HEAP_SWP_INIT            1343
#define TX_HEAP_SWP_INIT_VAL        1344
#define TX_HEAP_SWP_USED            1345
#define TX_HEAP_SWP_USED_VAL        1346
#define TX_HEAP_SWP_FREE            1347
#define TX_HEAP_SWP_FREE_VAL        1348
#define TX_HEAP_SWP_SIZE            1349
#define TX_HEAP_SWP_SIZE_VAL        1350
#define TX_HEAP_SWP_USED_BLOCKS     1351
#define TX_HEAP_SWP_USED_BLOCKS_VAL 1352
#define TX_HEAP_SWP_FREE_BLOCKS     1353
#define TX_HEAP_SWP_FREE_BLOCKS_VAL 1354

/*
 * The Status Page.
 */
#define PG_WIN32K_INFO_PAGE         1400
#define GR_W32K                     1401
#define TX_W32K_VERSION             1402
#define TX_W32K_VERSION_VAL         1403
#define TX_W32K_BUILD_DATETIME      1404
#define TX_W32K_BUILD_DATETIME_VAL  1405
#define TX_W32K_SYMBOLFILE          1406
#define TX_W32K_SYMBOLFILE_VAL      1407
#define TX_W32K_KERNELBUILD         1408
#define TX_W32K_KERNELBUILD_VAL     1409

/*
 * The Loader/OS extention Page
 */
#define DL_LDRFIX_PAGE              1500
#define GR_LDRFIX                   1501
#define CB_LDRFIX_DLLFIXES          1502
#define CB_LDRFIX_EXEFIXES          1503
#define CB_LDRFIX_FORCEPRELOAD      1504

/*
 * The MemInfo Page.
 */
#define DL_MEMINFO_PAGE             1600
#define GR_MEMINFO                  1601

#define GR_MEMINFO_SWAP             1610
#define TX_MEMINFO_SWAP_SIZE        1611
#define TX_MEMINFO_SWAP_AVAIL       1612
#define TX_MEMINFO_SWAP_USED        1613
#define TX_MEMINFO_SWAP_MINFREE     1614
#define TX_MEMINFO_SWAP_CFG_MINFREE 1615
#define TX_MEMINFO_SWAP_CFG_SIZE    1616
#define TX_MEMINFO_SWAP_BROKEN_DFS  1617
#define TX_MEMINFO_SWAP_GROW_FAILS  1618
#define TX_MEMINFO_SWAP_DFS_IN_MEMFILE 1619

#define GR_MEMINFO_PHYSPAGE         1640
#define TX_MEMINFO_PHYS_SIZE        1641
#define TX_MEMINFO_PHYS_AVAIL       1642
#define TX_MEMINFO_PHYS_USED        1643
#define TX_MEMINFO_PAGE_ENABLED     1651
#define TX_MEMINFO_PAGE_FAULTS      1652
#define TX_MEMINFO_PAGE_FAULTS_ACTIVE 1653
#define TX_MEMINFO_PAGE_PHYSPAGES   1654
#define TX_MEMINFO_PAGE_RESPAGES    1655
#define TX_MEMINFO_PAGE_SWAPPAGES   1656
#define TX_MEMINFO_PAGE_DISCPAGES   1657
#define TX_MEMINFO_PAGE_DISCINMEM   1658

#define GR_MEMINFO_VM               1680
#define TX_MEMINFO_VM_ADDRESSLIMIT  1681
#define TX_MEMINFO_VM_SHARED_MIN    1682
#define TX_MEMINFO_VM_SHARED_MAX    1683
#define TX_MEMINFO_VM_PRIVATE_MAX   1684
#define TX_MEMINFO_VM_SYSTEM_MIN    1685
#define TX_MEMINFO_VM_SYSTEM_MAX    1686
#define TX_MEMINFO_VM_PRIVATE_HMAX  1687
#define TX_MEMINFO_VM_SHARED_HMIN   1688
#define TX_MEMINFO_VM_SHARED_HMAX   1689
