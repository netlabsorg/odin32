/* $Id: krnlImportTable.c,v 1.1 2002-03-10 02:45:55 bird Exp $
 *
 * krnlImportTable - the import table (move from ProbKrnl).
 *
 * Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FALSE 0
#define TRUE 0

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "devSegDf.h"
#include "ProbKrnl.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * aImportTab defines the imported and overloaded OS/2 kernel functions.
 * IMPORTANT: aImportTab has three sibling arrays, two in dev32\d32init.c (aulProc
 *            and aTstFakers), and the calltab.asm, which must match entry by entry.
 *  - obsolete warning -
 *            When adding/removing/shuffling items in aImportTab, aulProc and
 *            calltab.asm has to be updated immediately!
 *            Use the mkcalltab.exe to generate calltab.asm and aTstFakers.
 *  - obsolete warning -
 *            We'll now generate both of these files from this table.
 *
 */
IMPORTKRNLSYM DATA16_GLOBAL aImportTab[NBR_OF_KRNLIMPORTS] =
{/* iOrdinal    iObject   achName                      offObject  usSel  chOpcode    */
 /*       fFound    cchName                    achExtra    ulAddress  cProlog  fType */
    /* 16-bit procs - starts at ordinal 400 */
    { 400, FALSE, -1, 11, "f_FuStrLenZ",          "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    { 401, FALSE, -1, 10, "f_FuStrLen",           "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    { 402, FALSE, -1,  8, "f_FuBuff",             "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    { 403, FALSE, -1, 13, "h_POST_SIGNAL",        "",    -1,  -1,  -1,  -1, 0, EPT_PROCH16},
    { 404, FALSE, -1, 12, "dh_SendEvent",         "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    { 405, FALSE, -1,  6, "RASRST",               "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    /* 32-bit procs - starts at ordinal 500 */
    { 500, FALSE, -1, 12, "_ldrOpenPath",         "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32 | EPT_WRAPPED}, /* Must be [0]! See importTabInit. */
    { 501, FALSE, -1,  8, "_ldrRead",             "@24", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 502, FALSE, -1,  8, "_ldrOpen",             "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 503, FALSE, -1,  9, "_ldrClose",            "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 504, FALSE, -1, 12, "_LDRQAppType",         "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 505, FALSE, -1, 20, "_ldrEnum32bitRelRecs", "@24", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 506, FALSE, -1, 14, "_ldrFindModule",       "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 507, FALSE, -1, 21, "_ldrCheckInternalName","@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 508, FALSE, -1, 11, "g_tkExecPgm",          "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 509, FALSE, -1, 15, "_tkStartProcess",      "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 510, FALSE, -1, 12, "_LDRClearSem",         "@0",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 511, FALSE, -1, 21, "_ldrASMpMTEFromHandle","@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 512, FALSE, -1, 21, "_ldrValidateMteHandle","@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 513, FALSE, -1, 13, "_ldrTransPath",        "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 514, FALSE, -1, 15, "_ldrGetFileName",      "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 515, FALSE, -1, 15, "_ldrUCaseString",      "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 516, FALSE, -1, 11, "_VMAllocMem",          "@36", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 517, FALSE, -1, 10, "_VMFreeMem",           "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 518, FALSE, -1, 11, "_VMGetOwner",          "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 519, FALSE, -1, 16, "_VMObjHandleInfo",     "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 520, FALSE, -1, 16, "_VMMapDebugAlias",     "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 521, FALSE, -1, 21, "_VMCreatePseudoHandle","@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 522, FALSE, -1, 19, "_VMFreePseudoHandle",  "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 523, FALSE, -1, 21, "_KSEMRequestExclusive","@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 524, FALSE, -1, 18, "_KSEMRequestShared",   "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 525, FALSE, -1, 17, "_KSEMRequestMutex",    "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 526, FALSE, -1, 12, "_KSEMRelease",         "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 527, FALSE, -1, 17, "_KSEMReleaseMutex",    "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 528, FALSE, -1, 10, "_KSEMQuery",           "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 529, FALSE, -1, 15, "_KSEMQueryMutex",      "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 530, FALSE, -1, 15, "_KSEMResetEvent",      "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 531, FALSE, -1, 14, "_KSEMPostEvent",       "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 532, FALSE, -1, 14, "_KSEMWaitEvent",       "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 533, FALSE, -1, 12, "_TKPidToPTDA",         "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 534, FALSE, -1, 12, "_TKScanTasks",         "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 535, FALSE, -1,  9, "_TKSuBuff",            "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 536, FALSE, -1,  9, "_TKFuBuff",            "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 537, FALSE, -1, 11, "_TKFuBufLen",          "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 538, FALSE, -1, 11, "_TKSuFuBuff",          "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 539, FALSE, -1, 14, "_TKForceThread",       "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 540, FALSE, -1, 12, "_TKForceTask",         "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 541, FALSE, -1, 14, "_TKGetPriority",       "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 542, FALSE, -1,  8, "_TKSleep",             "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 543, FALSE, -1,  9, "_TKWakeup",            "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 544, FALSE, -1, 13, "_TKWakeThread",        "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 545, FALSE, -1, 14, "_TKQueryWakeup",       "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 546, FALSE, -1, 12, "_SftFileSize",         "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 547, FALSE, -1, 12, "_PGPhysAvail",         "@0",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 548, FALSE, -1, 14, "_PGPhysPresent",       "@0",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 549, FALSE, -1, 17, "_vmRecalcShrBound",    "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 550, FALSE, -1, 15, "KMEnterKmodeSEF",      "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 551, FALSE, -1, 15, "KMExitKmodeSEF8",      "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 552, FALSE, -1, 14, "SecPathFromSFN",       "",    -1,  -1,  -1,  -1, 0, EPT_PROCNR32},
    { 553, FALSE, -1,  9, "_KSEMInit",            "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 554, FALSE, -1, 10, "_IOSftOpen",           "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 555, FALSE, -1, 11, "_IOSftClose",          "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 556, FALSE, -1, 15, "_IOSftTransPath",      "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 557, FALSE, -1, 12, "_IOSftReadAt",         "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 558, FALSE, -1, 13, "_IOSftWriteAt",        "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 559, FALSE, -1, 14, "_ldrSetVMflags",       "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 560, FALSE, -1, 13, "_ldrGetOrdNum",        "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 561, FALSE, -1, 18, "_ldrWasLoadModuled",   "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 562, FALSE, -1, 15, "_LDRGetProcAddr",      "@24", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 563, FALSE, -1, 12, "_LDRFreeTask",         "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 554, FALSE, -1, 13, "_SELVirtToLin",        "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 565, FALSE, -1, 19, "_SELConvertToLinear",  "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 566, FALSE, -1, 21, "_SELConvertToSelector","@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    { 567, FALSE, -1, 12, "_SELAllocGDT",         "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},

    /* 32-bit vars - starts at ordinal 900 */
    { 900, FALSE, -1, 16, "_ldrpFileNameBuf",     "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 901, FALSE, -1,  7, "_LdrSem",              "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 902, FALSE, -1, 11, "_LDRLibPath",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 903, FALSE, -1,  6, "_mte_h",               "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 904, FALSE, -1,  9, "_global_h",            "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 905, FALSE, -1,  9, "_global_l",            "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 906, FALSE, -1, 11, "_specific_h",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 907, FALSE, -1, 11, "_specific_l",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 908, FALSE, -1, 10, "_program_h",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 909, FALSE, -1, 10, "_program_l",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 910, FALSE, -1, 11, "_SMcDFInuse",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 911, FALSE, -1, 11, "_smFileSize",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 912, FALSE, -1, 11, "_SMswapping",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 913, FALSE, -1, 12, "_smcBrokenDF",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 914, FALSE, -1, 12, "_pgPhysPages",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 915, FALSE, -1, 13, "_SMcInMemFile",        "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 916, FALSE, -1, 13, "_SMCFGMinFree",        "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 917, FALSE, -1, 13, "_smcGrowFails",        "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 918, FALSE, -1, 14, "_PGSwapEnabled",       "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 919, FALSE, -1, 14, "_pgcPageFaults",       "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 920, FALSE, -1, 14, "_SMCFGSwapSize",       "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 921, FALSE, -1, 16, "_pgResidentPages",     "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 922, FALSE, -1, 17, "_pgSwappablePages",    "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 923, FALSE, -1,  8, "_ahvmShr",             "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 924, FALSE, -1,  8, "_ahvmSys",             "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 925, FALSE, -1, 19, "_pgDiscardableInmem",  "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 926, FALSE, -1, 19, "_pgDiscardablePages",  "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 927, FALSE, -1, 10, "_SMMinFree",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 928, FALSE, -1, 20, "_pgcPageFaultsActive", "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 929, FALSE, -1, 10, "_pgPhysMax",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    { 930, FALSE, -1,  9, "_ahvmhShr",            "",    -1,  -1,  -1,  -1, 0, EPT_VARNR32},
    { 931, FALSE, -1, 20, "_VirtualAddressLimit", "",    -1,  -1,  -1,  -1, 0, EPT_VARNR32},
    /* 16-bit vars - starts at ordinal 1100. */
    {1100, FALSE, -1,  8, "_pTCBCur",             "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1101, FALSE, -1,  9, "_pPTDACur",            "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1102, FALSE, -1, 10, "ptda_start",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1103, FALSE, -1, 12, "ptda_environ",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1104, FALSE, -1, 12, "ptda_ptdasem",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1105, FALSE, -1, 11, "ptda_handle",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1106, FALSE, -1, 11, "ptda_module",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {1107, FALSE, -1, 18, "ptda_pBeginLIBPATH",   "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},

    /* 16-bit Org* starts at ordinal 1500. */
    /* 32-bit Org* starts at ordinal 1700. */
};

