/* $Id: krnlImportTable.c,v 1.3 2002-12-16 00:46:23 bird Exp $
 *
 * krnlImportTable - the import table.
 *
 * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FALSE   0
#define TRUE    1


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "krnlImportTable.h"


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
#pragma info(notrd)
IMPORTKRNLSYM aImportTab[NBR_OF_KRNLIMPORTS] =
{/* iOrdinal    iObject   achName                      offObject  usSel  chOpcode    */
 /*       fFound    cchName                    achExtra    ulAddress  cProlog  fType */
    /* Ordinals 1-999 is reserved for kKrnlLib helper functions */
    /* 16-bit procs - starts at ordinal 1000 */
    {1000, FALSE, -1, 11, "f_FuStrLenZ",          "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    {1001, FALSE, -1, 10, "f_FuStrLen",           "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    {1002, FALSE, -1,  8, "f_FuBuff",             "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    {1003, FALSE, -1, 13, "h_POST_SIGNAL",        "",    -1,  -1,  -1,  -1, 0, EPT_PROCH16},
    {1004, FALSE, -1, 12, "dh_SendEvent",         "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},
    {1005, FALSE, -1,  6, "RASRST",               "",    -1,  -1,  -1,  -1, 0, EPT_PROC16},

    /* 32-bit procs - starts at ordinal 1200 */
    {1500, FALSE, -1, 12, "_ldrOpenPath",         "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32 | EPT_WRAPPED}, /* Must be [0]! See importTabInit. */
    {1501, FALSE, -1,  8, "_ldrRead",             "@24", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1502, FALSE, -1,  8, "_ldrOpen",             "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1503, FALSE, -1,  9, "_ldrClose",            "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1504, FALSE, -1, 12, "_LDRQAppType",         "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1505, FALSE, -1, 20, "_ldrEnum32bitRelRecs", "@24", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1506, FALSE, -1, 14, "_ldrFindModule",       "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1507, FALSE, -1, 21, "_ldrCheckInternalName","@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1508, FALSE, -1, 11, "g_tkExecPgm",          "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1509, FALSE, -1, 15, "_tkStartProcess",      "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1510, FALSE, -1, 12, "_LDRClearSem",         "@0",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1511, FALSE, -1, 21, "_ldrASMpMTEFromHandle","@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1512, FALSE, -1, 21, "_ldrValidateMteHandle","@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1513, FALSE, -1, 13, "_ldrTransPath",        "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1514, FALSE, -1, 15, "_ldrGetFileName",      "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1515, FALSE, -1, 15, "_ldrUCaseString",      "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1516, FALSE, -1, 11, "_VMAllocMem",          "@36", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1517, FALSE, -1, 10, "_VMFreeMem",           "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1518, FALSE, -1, 11, "_VMGetOwner",          "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1519, FALSE, -1, 16, "_VMObjHandleInfo",     "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1520, FALSE, -1, 16, "_VMMapDebugAlias",     "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1521, FALSE, -1, 21, "_VMCreatePseudoHandle","@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1522, FALSE, -1, 19, "_VMFreePseudoHandle",  "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1523, FALSE, -1, 21, "_KSEMRequestExclusive","@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1524, FALSE, -1, 18, "_KSEMRequestShared",   "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1525, FALSE, -1, 17, "_KSEMRequestMutex",    "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1526, FALSE, -1, 12, "_KSEMRelease",         "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1527, FALSE, -1, 17, "_KSEMReleaseMutex",    "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1528, FALSE, -1, 10, "_KSEMQuery",           "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1529, FALSE, -1, 15, "_KSEMQueryMutex",      "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1530, FALSE, -1, 15, "_KSEMResetEvent",      "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1531, FALSE, -1, 14, "_KSEMPostEvent",       "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1532, FALSE, -1, 14, "_KSEMWaitEvent",       "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1533, FALSE, -1, 12, "_TKPidToPTDA",         "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1534, FALSE, -1, 12, "_TKScanTasks",         "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1535, FALSE, -1,  9, "_TKSuBuff",            "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1536, FALSE, -1,  9, "_TKFuBuff",            "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1537, FALSE, -1, 11, "_TKFuBufLen",          "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1538, FALSE, -1, 11, "_TKSuFuBuff",          "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1539, FALSE, -1, 14, "_TKForceThread",       "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1540, FALSE, -1, 12, "_TKForceTask",         "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1541, FALSE, -1, 14, "_TKGetPriority",       "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1542, FALSE, -1,  8, "_TKSleep",             "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1543, FALSE, -1,  9, "_TKWakeup",            "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1544, FALSE, -1, 13, "_TKWakeThread",        "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1545, FALSE, -1, 14, "_TKQueryWakeup",       "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1546, FALSE, -1, 12, "_SftFileSize",         "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1547, FALSE, -1, 12, "_PGPhysAvail",         "@0",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1548, FALSE, -1, 14, "_PGPhysPresent",       "@0",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1549, FALSE, -1, 17, "_vmRecalcShrBound",    "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1550, FALSE, -1, 15, "KMEnterKmodeSEF",      "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1551, FALSE, -1, 15, "KMExitKmodeSEF8",      "",    -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1552, FALSE, -1, 14, "SecPathFromSFN",       "",    -1,  -1,  -1,  -1, 0, EPT_PROCNR32},
    {1553, FALSE, -1,  9, "_KSEMInit",            "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1554, FALSE, -1, 10, "_IOSftOpen",           "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1555, FALSE, -1, 11, "_IOSftClose",          "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1556, FALSE, -1, 15, "_IOSftTransPath",      "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1557, FALSE, -1, 12, "_IOSftReadAt",         "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1558, FALSE, -1, 13, "_IOSftWriteAt",        "@20", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1559, FALSE, -1, 14, "_ldrSetVMflags",       "@16", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1560, FALSE, -1, 13, "_ldrGetOrdNum",        "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1561, FALSE, -1, 18, "_ldrWasLoadModuled",   "@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1562, FALSE, -1, 15, "_LDRGetProcAddr",      "@24", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1563, FALSE, -1, 12, "_LDRFreeTask",         "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1554, FALSE, -1, 13, "_SELVirtToLin",        "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1565, FALSE, -1, 19, "_SELConvertToLinear",  "@8",  -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1566, FALSE, -1, 21, "_SELConvertToSelector","@12", -1,  -1,  -1,  -1, 0, EPT_PROC32},
    {1567, FALSE, -1, 12, "_SELAllocGDT",         "@4",  -1,  -1,  -1,  -1, 0, EPT_PROC32},

    /* 32-bit vars - starts at ordinal 2000 */
    {2000, FALSE, -1, 16, "_ldrpFileNameBuf",     "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2001, FALSE, -1,  7, "_LdrSem",              "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2002, FALSE, -1, 11, "_LDRLibPath",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2003, FALSE, -1,  6, "_mte_h",               "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2004, FALSE, -1,  9, "_global_h",            "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2005, FALSE, -1,  9, "_global_l",            "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2006, FALSE, -1, 11, "_specific_h",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2007, FALSE, -1, 11, "_specific_l",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2008, FALSE, -1, 10, "_program_h",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2009, FALSE, -1, 10, "_program_l",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2010, FALSE, -1, 11, "_SMcDFInuse",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2011, FALSE, -1, 11, "_smFileSize",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2012, FALSE, -1, 11, "_SMswapping",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2013, FALSE, -1, 12, "_smcBrokenDF",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2014, FALSE, -1, 12, "_pgPhysPages",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2015, FALSE, -1, 13, "_SMcInMemFile",        "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2016, FALSE, -1, 13, "_SMCFGMinFree",        "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2017, FALSE, -1, 13, "_smcGrowFails",        "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2018, FALSE, -1, 14, "_PGSwapEnabled",       "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2019, FALSE, -1, 14, "_pgcPageFaults",       "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2020, FALSE, -1, 14, "_SMCFGSwapSize",       "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2021, FALSE, -1, 16, "_pgResidentPages",     "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2022, FALSE, -1, 17, "_pgSwappablePages",    "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2023, FALSE, -1,  8, "_ahvmShr",             "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2024, FALSE, -1,  8, "_ahvmSys",             "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2025, FALSE, -1, 19, "_pgDiscardableInmem",  "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2026, FALSE, -1, 19, "_pgDiscardablePages",  "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2027, FALSE, -1, 10, "_SMMinFree",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2028, FALSE, -1, 20, "_pgcPageFaultsActive", "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2029, FALSE, -1, 10, "_pgPhysMax",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR32},
    {2030, FALSE, -1,  9, "_ahvmhShr",            "",    -1,  -1,  -1,  -1, 0, EPT_VARNR32},
    {2031, FALSE, -1, 20, "_VirtualAddressLimit", "",    -1,  -1,  -1,  -1, 0, EPT_VARNR32},

    /* 16-bit vars - starts at ordinal 2500. */
    {2500, FALSE, -1,  8, "_pTCBCur",             "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2501, FALSE, -1,  9, "_pPTDACur",            "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2502, FALSE, -1, 10, "ptda_start",           "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2503, FALSE, -1, 12, "ptda_environ",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2504, FALSE, -1, 12, "ptda_ptdasem",         "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2505, FALSE, -1, 11, "ptda_handle",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2506, FALSE, -1, 11, "ptda_module",          "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},
    {2507, FALSE, -1, 18, "ptda_pBeginLIBPATH",   "",    -1,  -1,  -1,  -1, 0, EPT_VAR16},

    /* 16-bit Org* starts at ordinal 3000. */
    /* 32-bit Org* starts at ordinal 3500. */
};

