/* $Id: MkCallTab.c,v 1.7 2002-12-16 00:51:33 bird Exp $
 *
 * Generates the calltab.asm from aImportTab.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen <bird@anduin.net>
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
/* Disable logging */
#define NOLOGGING 1


/*
 * Ordinal Ranges - Org exports.
 */
#define ORD_ORGDISPLACEMENT 2000        /* Displacement of the Org ordinals */
                                        /* from the ones in aImportTab.     */

/*
 * Include configuration/fixes.
 */
#define WORD unsigned short int
#define DWORD unsigned long int

#define INCL_BASE
#define INCL_DOS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_LDR

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

//#include <strat2.h>
//#include <reqpkt.h>

#include "krnlImportTable.h"
#include "os2krnl.h"                    /* must be included before dev1632.h! */
#include "dev1632.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* dummy replacement for SymDB.c */
KRNLDBENTRY     aKrnlSymDB[] = {{0}};

struct Entry
{
    int     iOrdinal;
    char *  szName;
    char *  szNameExp;
    int     fFlags;
} aEntries[] =
{   /* ord, name                       nameexp                          Flags */
    {   1, "ulkKrnlLibBuild",          "ulkKrnlLibBuild",               EXP_ABSOLUTE32},
    {   2, "_ulKernelBuild",           "ulKernelBuild",                 EXP_ABSOLUTE32},
    {   3, "_fKernel",                 "flKernel",                      EXP_ABSOLUTE32},
    {   4, "_pulTKSSBase32",           "TKSSBase",                      EXP_ABSOLUTE32},
    {   5, "_pulTKSSBase32",           "_TKSSBase",                     EXP_ABSOLUTE32},
    {   6, "_R0FlatCS16",              "R0FlatCS",                      EXP_ABSOLUTE16},
    {   7, "_R0FlatDS16",              "R0FlatDS",                      EXP_ABSOLUTE16},

    /* 16-bit functions */
#if 0 /* FIXME: not now */
    {  30, "_printf16",                "_printf16",                     EXP_16},
    {  31, "_vprintf16",               "_vprintf16",                    EXP_16},
    {  32, "_kargncpy",                "_kargncpy",                     EXP_16},
    {  33, "_fopen",                   "_fopen",                        EXP_16},
    {  34, "_kstrchr",                 "_kstrchr",                      EXP_16},
    {  35, "_fread",                   "_fread",                        EXP_16},
    {  36, "_fseek",                   "_fseek",                        EXP_16},
    {  37, "_fsize",                   "_fsize",                        EXP_16},
    {  38, "_feof",                    "_feof",                         EXP_16},
    {  39, "_fgets",                   "_fgets",                        EXP_16},
    {  40, "_kstrstr",                 "_kstrstr",                      EXP_16},
    {  41, "_kstrncmp",                "_kstrncmp",                     EXP_16},
    {  42, "_kstrlen",                 "_kstrlen",                      EXP_16},
    {  43, "_kstrcpy",                 "_kstrcpy",                      EXP_16},
    {  44, "_kstrncpy",                "_kstrncpy",                     EXP_16},
    {  45, "_kstrcat",                 "_kstrcat",                      EXP_16},
    {  46, "_kstrtok",                 "_kstrtok",                      EXP_16},
#endif

    /* 16-bit crt functions */
    {  60, "_inp",                     "_inp",                          EXP_16},
    {  61, "_outp",                    "_outp",                         EXP_16},
    {  62, "__aNFaldiv",               "__aNFaldiv",                    EXP_16},
    {  63, "__aNFalmul",               "__aNFalmul",                    EXP_16},
    {  64, "__aNFalrem",               "__aNFalrem",                    EXP_16},
    {  65, "__aNFauldiv",              "__aNFauldiv",                   EXP_16},
    {  66, "__aNFaulmul",              "__aNFaulmul",                   EXP_16},
    {  67, "__aNlmul",                 "__aNlmul",                      EXP_16},
    {  68, "__aNldiv",                 "__aNldiv",                      EXP_16},
    {  69, "__aNlrem",                 "__aNlrem",                      EXP_16},
    {  70, "__aNlshl",                 "__aNlshl",                      EXP_16},
    {  71, "__aNuldiv",                "__aNuldiv",                     EXP_16},
    {  72, "__aNulmul",                "__aNulmul",                     EXP_16},
    {  73, "__aNulrem",                "__aNulrem",                     EXP_16},
    {  74, "__fmemcpy",                "__fmemcpy",                     EXP_16},

    /* 16-bit crt variables */
    /*{  99, "__AHINCR",                 "__AHINCR",                      EXP_16}, - doesn't work I'm afraid...*/


    /* clib functions */
    { 100, "__nw__FUi"                 , "__nw__FUi"                  , EXP_32},
    { 101, "__nw__FUiPv"               , "__nw__FUiPv"                , EXP_32},
    { 102, "__vn__FUi"                 , "__vn__FUi"                  , EXP_32},
    { 103, "__vn__FUiPv"               , "__vn__FUiPv"                , EXP_32},
    { 104, "__dl__FPv"                 , "__dl__FPv"                  , EXP_32},
    { 105, "__vd__FPv"                 , "__vd__FPv"                  , EXP_32},

    { 106, "__nw__FUiPCcT1"            , "__nw__FUiPCcT1"             , EXP_32},
    { 107, "__nw__FUiPCcT1Pv"          , "__nw__FUiPCcT1Pv"           , EXP_32},
    { 108, "__vn__FUiPCcT1"            , "__vn__FUiPCcT1"             , EXP_32},
    { 109, "__vn__FUiPCcT1Pv"          , "__vn__FUiPCcT1Pv"           , EXP_32},
    { 110, "__dl__FPvPCcUi"            , "__dl__FPvPCcUi"             , EXP_32},
    { 111, "__vd__FPvPCcUi"            , "__vd__FPvPCcUi"             , EXP_32},

    { 112, "malloc"                    , "malloc"                     , EXP_32},
    { 113, "realloc"                   , "realloc"                    , EXP_32},
    { 114, "free"                      , "free"                       , EXP_32},
    { 115, "_msize"                    , "_msize"                     , EXP_32},
    { 116, "_memfree"                  , "_memfree"                   , EXP_32},
    { 117, "_heap_check"               , "_heap_check"                , EXP_32},
    { 118, "smalloc"                    , "smalloc"                   , EXP_32},
    { 119, "srealloc"                   , "srealloc"                  , EXP_32},
    { 120, "sfree"                      , "sfree"                     , EXP_32},
    { 121, "_swp_msize"                 , "_swp_msize"                , EXP_32},
    { 122, "_swp_memfree"               , "_swp_memfree"              , EXP_32},
    { 123, "_swp_heap_check"            , "_swp_heap_check"           , EXP_32},
    { 124, "_swp_heapmin"               , "_swp_heapmin"              , EXP_32},
    { 125, "_swp_state"                 , "_swp_state"                , EXP_32},
    { 126, "rmalloc"                    , "rmalloc"                   , EXP_32},
    { 127, "rrealloc"                   , "rrealloc"                  , EXP_32},
    { 128, "rfree"                      , "rfree"                     , EXP_32},
    { 129, "_res_msize"                 , "_res_msize"                , EXP_32},
    { 130, "_res_memfree"               , "_res_memfree"              , EXP_32},
    { 131, "_res_heap_check"            , "_res_heap_check"           , EXP_32},
    { 132, "_res_heapmin"               , "_res_heapmin"              , EXP_32},
    { 133, "_res_state"                 , "_res_state"                , EXP_32},

    { 170, "stricmp"                    , "stricmp"                   , EXP_32},
    { 171, "strnicmp"                   , "strnicmp"                  , EXP_32},

    { 172, "vsprintf"                   , "vsprintf"                  , EXP_32},
    { 173, "sprintf"                    , "sprintf"                   , EXP_32},

    { 174, "vprintf"                    , "vprintf"                   , EXP_32},
    { 175, "vprintf2"                   , "vprintf2"                  , EXP_32},
    { 176, "vprintf"                    , "_vprintfieee"              , EXP_32},
    { 177, "printf"                     , "printf"                    , EXP_32},
    { 178, "printf"                     , "_printfieee"               , EXP_32},
    { 179, "printf"                     , "_printf_ansi"              , EXP_32},

    { 180, "abort"                      , "abort"                     , EXP_32},
    { 181, "abort"                      , "__PureVirtualCalled"       , EXP_32},
    { 182, "_fltused"                   , "_fltused"                  , EXP_32},

    /* overload functions (190-199) */
    { 190, "kKLOverload32"              , "kKLOverload32"             , EXP_32},
    { 191, "kKLRestore32"               , "kKLRestore32"              , EXP_32},
    { 192, "kKLOverload16"              , "kKLOverload16"             , EXP_32},
    { 193, "kKLRestore16"               , "kKLRestore16"              , EXP_32},
    { 194, "kKLOverload16H"             , "kKLOverload16H"            , EXP_32},
    { 195, "kKLRestore16H"              , "kKLRestore16H"             , EXP_32},

    /* crt functions - adds as needed. */
    { 200, "memchr"                     , "memchr"                    , EXP_32},
    { 201, "memcmp"                     , "memcmp"                    , EXP_32},
    { 202, "memcpy"                     , "memcpy"                    , EXP_32},
    { 203, "memmove"                    , "memmove"                   , EXP_32},
    { 204, "memset"                     , "memset"                    , EXP_32},
    { 205, "strcat"                     , "strcat"                    , EXP_32},
    { 206, "strchr"                     , "strchr"                    , EXP_32},
    { 207, "strcmp"                     , "strcmp"                    , EXP_32},
    { 208, "strcpy"                     , "strcpy"                    , EXP_32},
    { 209, "strlen"                     , "strlen"                    , EXP_32},
    { 210, "strncat"                    , "strncat"                   , EXP_32},
    { 211, "strncmp"                    , "strncmp"                   , EXP_32},
    { 212, "strncpy"                    , "strncpy"                   , EXP_32},
    { 213, "strpbrk"                    , "strpbrk"                   , EXP_32},
    { 214, "strrchr"                    , "strrchr"                   , EXP_32},
    { 215, "strstr"                     , "strstr"                    , EXP_32},

    /* 32-bits devhelps */
    { 300, "D32Hlp_VirtToLin"           , "D32Hlp_VirtToLin"          , EXP_32},
    { 301, "D32Hlp_VirtToLin2"          , "D32Hlp_VirtToLin2"         , EXP_32},
    { 302, "D32Hlp_VirtToLin3"          , "D32Hlp_VirtToLin3"         , EXP_32},
    { 303, "D32Hlp_GetDOSVar"           , "D32Hlp_GetDOSVar"          , EXP_32},
    { 304, "D32Hlp_Yield"               , "D32Hlp_Yield"              , EXP_32},
    { 305, "D32Hlp_VMAlloc"             , "D32Hlp_VMAlloc"            , EXP_32},
    { 306, "D32Hlp_VMFree"              , "D32Hlp_VMFree"             , EXP_32},
    { 307, "D32Hlp_VMSetMem"            , "D32Hlp_VMSetMem"           , EXP_32},
    { 308, "D32Hlp_VMLock"              , "D32Hlp_VMLock"             , EXP_32},
    { 309, "D32Hlp_VMLock2"             , "D32Hlp_VMLock2"            , EXP_32},
    { 310, "D32Hlp_VMUnLock"            , "D32Hlp_VMUnLock"           , EXP_32},

    /* 16-bit devhelps */
    { 400, "DEVHELP_VIRTTOLIN"          , "DEVHELP_VIRTTOLIN"         , EXP_16},
    { 401, "DEVHELP_GETDOSVAR"          , "DEVHELP_GETDOSVAR"         , EXP_16},
    { 402, "DEVHELP_VMUNLOCK"           , "DEVHELP_VMUNLOCK"          , EXP_16},
    { 403, "DEVHELP_VMLOCK"             , "DEVHELP_VMLOCK"            , EXP_16},
    { 404, "DEVHELP_SAVE_MESSAGE"       , "DEVHELP_SAVE_MESSAGE"      , EXP_16},
    { 405, "DEVHELP_VIRTTOLIN"          , "DEVHELP_VIRTTOLIN"         , EXP_16},
    { 406, "DEVHELP_LINTOPAGELIST"      , "DEVHELP_LINTOPAGELIST"     , EXP_16},
    { 407, "DEVHELP_VERIFYACCESS"       , "DEVHELP_VERIFYACCESS"      , EXP_16},

    /* Access functions */
    { 500, "ptdaGet_ptda_environ"       , "ptdaGet_ptda_environ"      , EXP_32},
    { 501, "ptdaGet_ptda_handle"        , "ptdaGet_ptda_handle"       , EXP_32},
    { 502, "ptdaGet_ptda_module"        , "ptdaGet_ptda_module"       , EXP_32},
    { 503, "ptda_ptda_ptdasem"          , "ptda_ptda_ptdasem"         , EXP_32},
    { 504, "ptdaGet_ptda_pBeginLIBPATH" , "ptdaGet_ptda_pBeginLIBPATH", EXP_32},
    { 505, "ptdaGet_ptda_pEndLIBPATH"   , "ptdaGet_ptda_pEndLIBPATH"  , EXP_32},
    { 506, "tcbGetTCBFailErr"           , "tcbGetTCBFailErr"          , EXP_32},
    { 507, "tcbSetTCBFailErr"           , "tcbSetTCBFailErr"          , EXP_32},
    { 508, "GetEnv"                     , "GetEnv"                    , EXP_32},
    { 509, "ScanEnv"                    , "ScanEnv"                   , EXP_32},

    /* kLib functions */
    { 600, "kAssertMsg"                 , "kAssertMsg"                , EXP_32},
    { 601, "kLogFixPrintf1"             , "kLogFixPrintf1"            , EXP_32},
    { 602, "kLogFixPrintf2"             , "kLogFixPrintf2"            , EXP_32},
    { 603, "kLogv"                      , "kLogv"                     , EXP_32},
    { 604, "kLogExit"                   , "kLogExit"                  , EXP_32},
    { 605, "kLogEntry"                  , "kLogEntry"                 , EXP_32},
    { 606, "kLogStop"                   , "kLogStop"                  , EXP_32},
    { 607, "kLogStart"                  , "kLogStart"                 , EXP_32},
    { 608, "kLogTermMod"                , "kLogTermMod"               , EXP_32},
    { 609, "kLogInitMod"                , "kLogInitMod"               , EXP_32},
    { 610, "kLog"                       , "kLog"                      , EXP_32},
    { 611, "kLogInit"                   , "kLogInit"                  , EXP_32},
    { 612, "kLIFOPop"                   , "kLIFOPop"                  , EXP_32},
    { 613, "kLIFOPush"                  , "kLIFOPush"                 , EXP_32},
    { 614, "kFIFORemove"                , "kFIFORemove"               , EXP_32},
    { 615, "kFIFOInsert"                , "kFIFOInsert"               , EXP_32},
    { 616, "kFIFOAtomicRemove"          , "kFIFOAtomicRemove"         , EXP_32},
    { 617, "kFIFOAtomicInsert"          , "kFIFOAtomicInsert"         , EXP_32},
    { 618, "kLIFOAtomicPop"             , "kLIFOAtomicPop"            , EXP_32},
    { 619, "kLIFOAtomicPush"            , "kLIFOAtomicPush"           , EXP_32},
    { 620, "kAtomicSetEqual"            , "kAtomicSetEqual"           , EXP_32},
    { 621, "kAtomicSet"                 , "kAtomicSet"                , EXP_32},
    { 622, "kAtomicIncWrap"             , "kAtomicIncWrap"            , EXP_32},
    { 623, "kAtomicInc"                 , "kAtomicInc"                , EXP_32},
    { 624, "kAtomicDec"                 , "kAtomicDec"                , EXP_32},
    { 625, "kGetSS"                     , "kGetSS"                    , EXP_32},
    { 626, "kGetGS"                     , "kGetGS"                    , EXP_32},
    { 627, "kGetFS"                     , "kGetFS"                    , EXP_32},
    { 628, "kGetES"                     , "kGetES"                    , EXP_32},
    { 629, "kGetDS"                     , "kGetDS"                    , EXP_32},
    { 630, "kGetCS"                     , "kGetCS"                    , EXP_32},
    { 631, "AVLStrRemove"              , "AVLStrRemove"               , EXP_32},
    { 632, "AVLStrInsert"              , "AVLStrInsert"               , EXP_32},
    { 633, "AVLStrGet"                 , "AVLStrGet"                  , EXP_32},
    { 634, "AVLStrBeginEnumTree"       , "AVLStrBeginEnumTree"        , EXP_32},
    { 635, "AVLStrGetNextNode"         , "AVLStrGetNextNode"          , EXP_32},
    { 636, "AVLStrDoWithAll"           , "AVLStrDoWithAll"            , EXP_32},
    { 637, "AVLStrIRemove"             , "AVLStrIRemove"              , EXP_32},
    { 638, "AVLStrIInsert"             , "AVLStrIInsert"              , EXP_32},
    { 639, "AVLStrIGet"                , "AVLStrIGet"                 , EXP_32},
    { 640, "AVLStrIBeginEnumTree"      , "AVLStrIBeginEnumTree"       , EXP_32},
    { 641, "AVLStrIGetNextNode"        , "AVLStrIGetNextNode"         , EXP_32},
    { 642, "AVLStrIDoWithAll"          , "AVLStrIDoWithAll"           , EXP_32},
    { 643, "AVLPVRemove"               , "AVLPVRemove"                , EXP_32},
    { 644, "AVLPVInsert"               , "AVLPVInsert"                , EXP_32},
    { 645, "AVLPVGet"                  , "AVLPVGet"                   , EXP_32},
    { 646, "AVLPVGetWithParent"        , "AVLPVGetWithParent"         , EXP_32},
    { 647, "AVLPVGetWithAdjacentNodes" , "AVLPVGetWithAdjacentNodes"  , EXP_32},
    { 648, "AVLPVGetBestFit"           , "AVLPVGetBestFit"            , EXP_32},
    { 649, "AVLPVRemoveBestFit"        , "AVLPVRemoveBestFit"         , EXP_32},
    { 650, "AVLPVBeginEnumTree"        , "AVLPVBeginEnumTree"         , EXP_32},
    { 651, "AVLPVGetNextNode"          , "AVLPVGetNextNode"           , EXP_32},
    { 652, "AVLPVDoWithAll"            , "AVLPVDoWithAll"             , EXP_32},
    { 653, "AVLULRemove"               , "AVLULRemove"                , EXP_32},
    { 654, "AVLULInsert"               , "AVLULInsert"                , EXP_32},
    { 655, "AVLULGet"                  , "AVLULGet"                   , EXP_32},
    { 656, "AVLULGetWithParent"        , "AVLULGetWithParent"         , EXP_32},
    { 657, "AVLULGetWithAdjacentNodes" , "AVLULGetWithAdjacentNodes"  , EXP_32},
    { 658, "AVLULGetBestFit"           , "AVLULGetBestFit"            , EXP_32},
    { 659, "AVLULRemoveBestFit"        , "AVLULRemoveBestFit"         , EXP_32},
    { 660, "AVLULBeginEnumTree"        , "AVLULBeginEnumTree"         , EXP_32},
    { 661, "AVLULGetNextNode"          , "AVLULGetNextNode"           , EXP_32},
    { 662, "AVLULDoWithAll"            , "AVLULDoWithAll"             , EXP_32},
    { 663, "AVLULMRemove"              , "AVLULMRemove"               , EXP_32},
    { 664, "AVLULMRemove2"             , "AVLULMRemove2"              , EXP_32},
    { 665, "AVLULMInsert"              , "AVLULMInsert"               , EXP_32},
    { 666, "AVLULMGet"                 , "AVLULMGet"                  , EXP_32},
    { 667, "AVLULMGet2"                , "AVLULMGet2"                 , EXP_32},
    { 668, "AVLULMGetWithParent"       , "AVLULMGetWithParent"        , EXP_32},
    { 669, "AVLULMGetWithAdjacentNodes", "AVLULMGetWithAdjacentNodes" , EXP_32},
    { 670, "AVLULMGetBestFit"          , "AVLULMGetBestFit"           , EXP_32},
    { 671, "AVLULMRemoveBestFit"       , "AVLULMRemoveBestFit"        , EXP_32},
    { 672, "AVLULMBeginEnumTree"       , "AVLULMBeginEnumTree"        , EXP_32},
    { 673, "AVLULMGetNextNode"         , "AVLULMGetNextNode"          , EXP_32},
    { 674, "AVLULMDoWithAll"           , "AVLULMDoWithAll"            , EXP_32},
};





/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void syntax(void);
int GenerateCalltab(void);
int findMaxOrdinal(void);
int findNextOrdinal(int iOrd, char *pszName, char *pszNameExp, int *pfType, int *piIndex);
int GenerateDefFile(void);
int GenerateTstFakers(void);


/**
 * MkCalltab program.
 *
 * Output to stdout the calltab assembly code.
 *
 */
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        syntax();
        return -1;
    }
    if (argv[1][0] == 'c')
        return GenerateCalltab();
    else if (argv[1][0] == 'd')
        return GenerateDefFile();
    else if (argv[1][0] == 't')
        return GenerateTstFakers();

    /* failure */
    syntax();
    return -2;
}


/**
 * Display syntax.
 */
void syntax(void)
{
    fprintf(stderr,
            "Incorrect parameter!\n"
            "Syntax: mkcalltab.exe <tab>\n"
            "   Where <tab> is either calltab or tstfakers.\n"
           );
}


/**
 * Generate calltab.asm.
 * It's contents is written to stdout.
 */
int GenerateCalltab(void)
{
    int     i;
    int     iOrd;
    int     iOrdMax;
    int     iPrevBundle;
    int     fType;
    int     iIndex;
    char    szName[64];
    char    szNameExp[64];


    /*
     * Write Start of file.
     */
    printf("; $Id: MkCallTab.c,v 1.7 2002-12-16 00:51:33 bird Exp $\n"
           ";\n"
           "; Autogenerated calltab file.\n"
           ";\n"
           "; Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>\n"
           ";\n"
           ";\n"
           "; This file is part of kKrnlLib.\n"
           ";\n"
           "; kKrnlLib is free software; you can redistribute it and/or modify\n"
           "; it under the terms of the GNU General Public License as published by\n"
           "; the Free Software Foundation; either version 2 of the License, or\n"
           "; (at your option) any later version.\n"
           ";\n"
           "; kKrnlLib is distributed in the hope that it will be useful,\n"
           "; but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           "; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
           "; GNU General Public License for more details.\n"
           ";\n"
           "; You should have received a copy of the GNU General Public License\n"
           "; along with kKrnlLib; if not, write to the Free Software\n"
           "; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
           ";\n"
           "    .386p\n"
           "\n"
           "\n"
           ";\n"
           "; Include files\n"
           ";\n"
           "    include devsegdf.inc\n"
           "    include options.inc\n"
           "\n"
           "\n"
           ";\n"
           "; Exported symbols\n"
           ";\n"
           "    public callTab\n"
           "    public callTabEND\n"
           "    public callTab16\n"
           "    public callTab16END\n"
           "    public auNopFuncs\n"
           "\n"
           "    extrn _aImportTab:BYTE\n"
           "    extrn _options:kKLOptions\n"
           "\n"
           "\n"
           ";\n"
           "; Constants\n"
           ";\n"
           "PROC32_PROLOG         EQU 14h\n"
           "PROC16_PROLOG         EQU 14h\n"
           "IMPORTH16_PROLOG      EQU 08h\n"
           "IMPORTHNR16_PROLOG    EQU 07h\n"
           "\n"
           "\n"
           ";\n"
           "; Macros\n"
           ";\n"
           "\n"
           "; Macro which makes a function overload calltable entry\n"
           "FnProc32Entry macro fnname\n"
           "    public fnname\n"
           "    fnname proc near\n"
           "        db PROC32_PROLOG dup(0cch)\n"
           "    fnname endp\n"
           "endm\n"
           "\n"
           "; Macro which makes a function overload calltable entry\n"
           "FnProc16Entry macro fnname\n"
           "    public fnname\n"
           "    public &fnname&_off\n"
           "    public &fnname&_sel\n"
           "    fnname proc near\n"
           "        db PROC16_PROLOG dup(0cch)\n"
           "    fnname endp\n"
           "    &fnname&_off         dw  0\n"
           "    &fnname&_sel         dw  0\n"
           "endm\n"
           "\n");
    printf("; Macro which makes a function calltable entry\n"
           "FnProcNR32Entry macro fnname\n"
           "    f&fnname&            dd  0\n"
           "    FnProc32Entry<fnname>\n"
           "endm\n"
           "\n"
           "; Macro which makes a function calltable entry\n"
           "FnProcNR16Entry macro fnname\n"
           "    f&fnname&            dd  0\n"
           "    FnProc16Entry<fnname>\n"
           "endm\n"
           "\n"
           "; Macro which makes a variable calltable entry.\n"
           "VariableEntry macro varname\n"
           "    public p&varname&\n"
           "    p&varname&           dd  0\n"
           "    public &varname&_offObject\n"
           "    &varname&_offObject  dd  0\n"
           "    public _fp&varname&\n"
           "    _fp&varname&         dd  0\n"
           "    public &varname&_sel\n"
           "    &varname&_sel        dw  0\n"
           "                         dw  0cch ;alignment\n"
           "endm\n"
           "\n"
           "\n"
           "\n"
           "DATA32 segment\n"
           "    ;assume cs:DATA32, ds:flat, ss:nothing\n"
           "    assume ds:flat, ss:nothing\n"
           ";\n"
           "; callTab is an array of function prologs with a jump to the real function\n"
           "; and pointers to real variables.\n"
           ";\n"
           "; Overrided OS/2 kernel functions are called thru this table.\n"
           ";\n"
           "db 'callTab',0\n"
           "callTab:\n");

    /*
     * Process aImportTab - all but 16-bit overrides.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (!EPT16Proc(aImportTab[i]) && !EPTProcHybrid(aImportTab[i]))
        {
            char *pszMacro = (aImportTab[i].fType & EPT_VAR)
                                ? "VariableEntry"
                                : ( (aImportTab[i].fType & EPT_NOT_REQ)
                                  ? (EPT16BitEntry(aImportTab[i]) ? "FnProcNR16Entry" : "FnProcNR32Entry")
                                  : (EPT16BitEntry(aImportTab[i]) ? "FnProc16Entry"   : "FnProc32Entry"));
            char *pszName = &aImportTab[i].achName[(aImportTab[i].fType & EPT_VAR) && aImportTab[i].achName[0] == '_'];
            char *pszExtra = (aImportTab[i].fType & EPT_WRAPPED) ? "_wrapped" : aImportTab[i].achExtra;

            /*
             * Variable or Function?
             */
            printf("%s<%s%s>%*.s;%d\n",
                   pszMacro,
                   pszName,
                   pszExtra,
                   45 - 2 - strlen(pszMacro) - aImportTab[i].cchName - strlen(pszExtra),
                   "",
                   i);
            if ((aImportTab[i].fType & EPT_WRAPPED))
            {
                printf("extrn %s%s:NEAR\n",
                       pszName,
                       aImportTab[i].achExtra);
            }
        }
    }

    /*
     * Write End of Calltab and start of callTab16.
     */
    printf("\n"
           "callTabEND db 'callTabEND',0,0\n"
           "DATA32 ENDS\n"
           "\n"
           "\n"
           "CALLTAB16 segment para public 'CALLTAB16_DATA' use16\n"
           "    assume ds:nothing, ss:nothing\n"
           ";\n"
           "; callTab16 is an array of function prologs with a jump to the real function\n"
           "; for 16-bit overrides.\n"
           ";\n"
           "; Overrided 16-bit OS/2 kernel functions are called thru this table.\n"
           ";\n"
           "callTab16:\n"
           );

    /*
     * Process aImportTab - 16 bit overrides.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (EPT16Proc(aImportTab[i]) || EPT16ProcH(aImportTab[i]))
        {
            printf("FnProc16Entry<%s>%*.s;%d\n",
                   aImportTab[i].achName,
                   45 - 2 - 17 - aImportTab[i].cchName,
                   "",
                   i);
        }
    }


    /*
     * Write End of Calltab and start of auFuncs.
     */
    printf("\n"
           "callTab16END db 'callTab16END',0,0,0,0\n"
           "CALLTAB16 ENDS\n"
           "\n"
           "\n"
           "DATA32 segment\n"
           "    assume ds:nothing, es:nothing\n"
           "\n"
           ";\n"
           "; Function table for nop functions.\n"
           "; Used by krnlInitImports() in krnlInit.c.\n"
           ";\n"
           "auNopFuncs:\n"
           );

    /*
     * Process aImportTab to generate auFuncs.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if ((aImportTab[i].fType & (EPT_NOT_REQ | EPT_VAR | EPT_PROC | EPT_PROCH)) == (EPT_NOT_REQ | EPT_PROC))
        {   /* Not required imported function (NOP function) */
            char *  pszSeg      = (EPT32BitEntry(aImportTab[i]) ? "32" : "16");
            char *  pszPrefix   = (aImportTab[i].achName[0] == '_' ? "_" : "");
            char *  pszName     = (*pszPrefix != '\0' ? &aImportTab[i].achName[1] : &aImportTab[i].achName[0]);

            printf(EPT16BitEntry(aImportTab[i])
                   ?  "    extrn %snop%s%s : NEAR\n"
                      "    dw  seg CODE%s:%snop%s%s\n"
                      "    dw  offset CODE%s:%snop%s%s\n"
                   :  "    extrn %snop%s%s : NEAR\n"
                      "    dd  offset FLAT:CODE%s:%snop%s%s\n",
                   pszPrefix,
                   pszName,
                   aImportTab[i].achExtra,
                   pszSeg,
                   pszPrefix,
                   pszName,
                   aImportTab[i].achExtra);
        }
        else
            printf("    dd  0h\n");
    }


    /*
     * MTE.
     */
    printf("\n"
           "\n"
           ";\n"
           "; kKrnlLib MTE\n"
           ";\n"
           "public kKrnlLibMTE\n"
           "kKrnlLibMTE label byte\n"
           "dw  2                                  ; mte_flags2; (MTEFORMATLX)\n"
           "dw  0                                  ; mte_handle;\n"
           "dd  offset FLAT:KKL_SwapMTE            ; mte_swapmte;\n"
           "dd  0                                  ; mte_link;\n"
           "dd  0001b180h                          ; mte_flags1; (LIBRARYMOD | LDRINVALID | CLASS_GLOBAL | MTE_MEDIAFIXED | MTEPROCESSED | VDDMOD)\n"
           "dd  0                                  ; mte_impmodcnt;\n"
           "dw  0                                  ; mte_sfn;\n"
           "dw  1                                  ; mte_usecnt;\n"
           "db  \"KKRNLLIB\"                         ; mte_modname[8];\n"
           "dd  0                                  ; evt. new fields\n"
           "dd  0                                  ; evt. new fields\n"
           "\n"
           "\n");

    /*
     * SMTE.
     */
    printf(";\n"
           "; kKrnlLIB SwapMTE\n"
           ";\n"
           "public KKL_SwapMTE\n"
           "KKL_SwapMTE label byte\n"
           "dd  0                                  ; ULONG   smte_mpages;        /* 00  Module # pages */\n"
           "dd  0                                  ; ULONG   smte_startobj;      /* 04  Object # for instruction */\n"
           "dd  0                                  ; ULONG   smte_eip;           /* 08  Extended instruction pointer */\n"
           "dd  0                                  ; ULONG   smte_stackobj;      /* 0c  Object # for stack pointer */\n"
           "dd  0                                  ; ULONG   smte_esp;           /* 10  Extended stack pointer */\n"
           "dd  0                                  ; ULONG   smte_pageshift;     /* 14  Page alignment shift in .EXE */\n"
           "dd  0                                  ; ULONG   smte_fixupsize;     /* 18  Fixup section size */\n"
           "dd  offset FLAT:KKL_ObjTab             ; POTE    smte_objtab;        /* 1c  Object table offset - POINTER */\n"
           "dd  5                                  ; ULONG   smte_objcnt;        /* 20  Number of objects in module */\n"
           "dd  0                                  ; ULONG   smte_objmap;        /* 24  Object page map offset - POINTER */\n"
           "dd  0                                  ; ULONG   smte_itermap;       /* 28  Object iterated data map offset */\n"
           "dd  0                                  ; ULONG   smte_rsrctab;       /* 2c  Offset of Resource Table */\n"
           "dd  0                                  ; ULONG   smte_rsrccnt;       /* 30  Number of resource entries */\n"
           "dd  offset FLAT:KKL_ResNameTab         ; ULONG   smte_restab;        /* 34  Offset of resident name table - POINTER */\n"
           "dd  offset FLAT:KKL_EntryTab           ; ULONG   smte_enttab;        /* 38  Offset of Entry Table - POINTER */\n"
           "dd  0                                  ; ULONG   smte_fpagetab;      /* 3c  Offset of Fixup Page Table - POINTER */\n"
           "dd  0                                  ; ULONG   smte_frectab;       /* 40  Offset of Fixup Record Table - POINTER */\n"
           "dd  0                                  ; ULONG   smte_impmod;        /* 44  Offset of Import Module Name Table - POINTER */\n");
    printf("dd  0                                  ; ULONG   smte_impproc;       /* 48  Offset of Imp Procedure Name Tab - POINTER */\n"
           "dd  0                                  ; ULONG   smte_datapage;      /* 4c  Offset of Enumerated Data Pages */\n"
           "dd  0                                  ; ULONG   smte_nrestab;       /* 50  Offset of Non-resident Names Table */\n"
           "dd  0                                  ; ULONG   smte_cbnrestab;     /* 54  Size of Non-resident Name Table */\n"
           "dd  0                                  ; ULONG   smte_autods;        /* 58  Object # for automatic data object */\n"
           "dd  0                                  ; ULONG   smte_debuginfo;     /* 5c  Offset of the debugging info */\n"
           "dd  0                                  ; ULONG   smte_debuglen;      /* 60  The len of the debug info in */\n"
           "dd  0                                  ; ULONG   smte_heapsize;      /* 64  use for converted 16-bit modules */\n"
           "dd  offset FLAT:KKL_szFn               ; PCHAR   smte_path;          /* 68  full pathname - POINTER */\n"
           "dd  0                                  ; USHORT  smte_semcount;      /* 6c  Count of threads waiting on MTE semaphore. 0 => semaphore is free */\n"
           "dd  0                                  ; USHORT  smte_semowner;      /* 6e  Slot number of the owner of MTE semahore */\n"
           "dd  0                                  ; ULONG   smte_pfilecache;    /* 70  Pointer to file cache for Dos32CacheModule */\n"
           "dd  0                                  ; ULONG   smte_stacksize;     /* 74  Thread 1 Stack size from the exe header */\n"
           "dd  0                                  ; USHORT  smte_alignshift;    /* 78  use for converted 16-bit modules */\n"
           "dd  0                                  ; USHORT  smte_NEexpver;      /* 7a  expver from NE header */\n"
           "dd  12                                 ; USHORT  smte_pathlen;       /* 7c  length of full pathname */\n"
           "dd  0                                  ; USHORT  smte_NEexetype;     /* 7e  exetype from NE header */\n"
           "dd  0                                  ; USHORT  smte_csegpack;      /* 80  count of segs to pack */\n"
           "\n");

    /*
     * Filename.
     */
    printf("\n"
           ";\n"
           "; Filename\n"
           ";\n"
           "KKL_szFn db \"kKrnlLib.dll\",0\n"
           "\n");

    /*
     * Object table.
     */
    printf("\n"
           ";\n"
           "; Externals\n"
           ";\n"
           "extrn DATA16START:BYTE\n"
           "extrn DATA16_GLOBALEND:BYTE\n"
           "extrn CODE16START:BYTE\n"
           "extrn CODE16END:BYTE\n"
           "extrn CODE32START:BYTE\n"
           "extrn CODE32END:BYTE\n"
           "extrn DATA32START:BYTE\n"
           "extrn _end:BYTE\n"
           );

    printf("\n"
           ";\n"
           "; Object table\n"
           ";\n"
           "public KKL_ObjTab\n"
           "KKL_ObjTab label byte\n"
           ";obj 1\n"
           "   dd offset FLAT:DATA16:DATA16_GLOBALEND;o32_size; /* Object virtual size */\n"
           "   dd offset FLAT:DATA16:DATA16START    ; o32_base; /* Object base virtual address */\n"
           "   dd 1003h                       ; o32_flags;      /* Attribute flags */                     \n"
           "   dd 0                           ; o32_pagemap;    /* Object page map index */               \n"
           "   dd 0                           ; o32_mapsize;    /* Number of entries in object page map */\n"
           "   dw seg DATA16                  ; ote_sel;        /* Object Selector */\n"
           "   dw 0                           ; ote_hob;        /* Object Handle */\n"
           ";obj 2\n"
           "   dd offset FLAT:CODE16:CODE16END      ; o32_size; /* Object virtual size */\n"
           "   dd offset FLAT:CODE16:CODE16START    ; o32_base; /* Object base virtual address */\n"
           "   dd 1005h                       ; o32_flags;      /* Attribute flags */                     \n"
           "   dd 0                           ; o32_pagemap;    /* Object page map index */               \n"
           "   dd 0                           ; o32_mapsize;    /* Number of entries in object page map */\n"
           "   dw seg CODE16                  ; ote_sel;        /* Object Selector */\n"
           "   dw 0                           ; ote_hob;        /* Object Handle */\n");
    printf(";obj 3\n"
           "   dd offset FLAT:CODE32:CODE32END      ; o32_size; /* Object virtual size */\n"
           "   dd offset FLAT:CODE32:CODE32START    ; o32_base; /* Object base virtual address */\n"
           "   dd 2005h                       ; o32_flags;      /* Attribute flags */                     \n"
           "   dd 0                           ; o32_pagemap;    /* Object page map index */               \n"
           "   dd 0                           ; o32_mapsize;    /* Number of entries in object page map */\n"
           "   dw seg FLAT:CODE32             ; ote_sel;        /* Object Selector */\n"
           "   dw 0                           ; ote_hob;        /* Object Handle */\n"
           ";obj 4\n"
           "   dd offset FLAT:DATA32:_end     ; o32_size; /* Object virtual size */\n"
           "   dd offset FLAT:DATA32:DATA32START    ; o32_base; /* Object base virtual address */\n"
           "   dd 2003h                       ; o32_flags;      /* Attribute flags */                     \n"
           "   dd 0                           ; o32_pagemap;    /* Object page map index */               \n"
           "   dd 0                           ; o32_mapsize;    /* Number of entries in object page map */\n"
           "   dw seg FLAT:DATA32             ; ote_sel;        /* Object Selector */\n"
           "   dw 0                           ; ote_hob;        /* Object Handle */\n"
           ";obj 5\n"
           "   dd offset FLAT:CALLTAB16:callTab16END; o32_size; /* Object virtual size */\n"
           "   dd offset FLAT:CALLTAB16:callTab16   ; o32_base; /* Object base virtual address */\n"
           "   dd 1005h                       ; o32_flags;      /* Attribute flags */                     \n"
           "   dd 0                           ; o32_pagemap;    /* Object page map index */               \n"
           "   dd 0                           ; o32_mapsize;    /* Number of entries in object page map */\n"
           "   dw seg FLAT:CALLTAB16          ; ote_sel;        /* Object Selector */\n"
           "   dw 0                           ; ote_hob;        /* Object Handle */\n"
           "\n"
           "public KKL_ObjTab_DosCalls\n"
           "KKL_ObjTab_DosCalls label byte\n"
           "db (20 * (4 * 6)) dup(0) ; 4*6 = sizeof(o32_obj); 20 = max objects;\n"
           "\n"
           "\n");

    /*
     * Resent name table.
     */
    printf("\n"
           ";\n"
           "; Resident name table\n"
           ";\n"
           "public KKL_ResNameTab\n"
           "KKL_ResNameTab label byte\n"
           "db 8,\"KKRNLLIB\"\n"
           "dw 0 ; ordinal #\n");
    #ifdef DEBUG
    iOrdMax = findMaxOrdinal();
    for (iOrd = findNextOrdinal(0, &szName[0], &szNameExp[0], NULL, NULL);
         iOrd < iOrdMax;
         iOrd = findNextOrdinal(iOrd, &szName[0], &szNameExp[0], NULL, NULL))
    {
        printf("db %d, '%s'\n"
               "dw %d\n",
               strlen(szNameExp), szNameExp, iOrd);
    }
    #endif
    printf("db 0 ; terminator\n"
           "public KKL_ResNameTabEND\n"
           "KKL_ResNameTabEND label byte\n"
           "\n"
           "DATA32 ends\n"
           "\n"
           "\n");

    /*
     * Entry table.
     */
    printf("KKLENTRY segment\n"
           "public KKL_EntryTab\n"
           "KKL_EntryTab label byte\n"
           "KKLENTRY ends\n"
           "\n"
           "KKLENTRYFIXUPS segment\n"
           "public KKL_EntryTabFixups\n"
           "KKL_EntryTabFixups label byte\n"
           "KKLENTRYFIXUPS ends\n"
           "\n");
    iOrdMax = findMaxOrdinal();
    for (iOrd = findNextOrdinal(0, &szName[0], &szNameExp[0], &fType, &iIndex), iPrevBundle = 0;
         iOrd < iOrdMax;
         iOrd = findNextOrdinal(iPrevBundle, &szName[0], &szNameExp[0], &fType, &iIndex)
         )
    {
        int iBundle;
        static char aszType[11][16] =
        {
            "Invalid",
            "Absolute16",               /* EXP_ABSOLUTE16  1  */
            "Absolute32",               /* EXP_ABSOLUTE32  2  */
            "32-bit",                   /* EXP_32          3  */
            "16-bit",                   /* EXP_16          4  */
            "32-bit KrnlProc",          /* EXP_PROC32      5  */
            "32-bit KrnlVar",           /* EXP_VAR32       6  */
            "16-bit KrnlProc",          /* EXP_VAR16       7  */
            "16-bit KrnlVar",           /* EXP_PROC16      8  */
            "32-bit",                   /* EXP_ORGPROC32   9  */
            "16-bit"                    /* EXP_ORGPROC16  10  */
        };
        static char aszSeg[11][16] =
        {
            "FATALERROR",
            "DATA16",                   /* EXP_ABSOLUTE16  1  */
            "FLAT:DATA32",              /* EXP_ABSOLUTE32  2  */
            "FLAT:CODE32",              /* EXP_32          3  */
            "CODE16",                   /* EXP_16          4  */
            "FLAT:CODE32",              /* EXP_PROC32      5  */
            "FLAT:DATA32",              /* EXP_VAR32       6  */
            "CODE16",                   /* EXP_VAR16       7  */
            "DATA16",                   /* EXP_PROC16      8  */
            "FLAT:CODE32",              /* EXP_ORGPROC32   9  */
            "CODE16"                    /* EXP_ORGPROC16  10  */
        };
        static char aszSeg2[11][16] =
        {
            "FATALERROR",
            "DATA16",                   /* EXP_ABSOLUTE16  1  */
            "DATA32",                   /* EXP_ABSOLUTE32  2  */
            "CODE32",                   /* EXP_32          3  */
            "CODE16",                   /* EXP_16          4  */
            "CODE32",                   /* EXP_PROC32      5  */
            "DATA32",                   /* EXP_VAR32       6  */
            "CODE16",                   /* EXP_VAR16       7  */
            "DATA16",                   /* EXP_PROC16      8  */
            "CODE32",                   /* EXP_ORGPROC32   9  */
            "CODE16"                    /* EXP_ORGPROC16  10  */
        };



        /*
         * New bundle.
         *      Find it's size by enumerating ordinals until not sequential or different type.
         *      16-bits exports need one bundle each.
         */
        for (iBundle = 0; iBundle < 254; iBundle++)
        {
            char    szDummy1[64];
            char    szDummy2[64];
            int     fType2;
            int     iIndexTmp;

            if (    (iBundle + iOrd + 1) != findNextOrdinal(iOrd + iBundle, szDummy1, szDummy2, &fType2, &iIndexTmp)
                ||  fType2 != fType
                ||  fType == EXP_PROC16
                ||  fType == EXP_VAR16)
                break;
        }
        iBundle++;                      /* make it a count. */

        /*
         * Need skip bundle?
         */
        while (iOrd > iPrevBundle + 1)
        {
            int cSkips = iOrd - iPrevBundle >= 255 ? 255 : iOrd - iPrevBundle - 1;
            printf("\n"
                   "KKLENTRY segment\n"
                   "db %d ; skips ordinals %d to %d\n"
                   "db 0\n"
                   "KKLENTRY ends\n",
                   cSkips,
                   iPrevBundle + 1,
                   iPrevBundle + cSkips);
            iPrevBundle += cSkips;
        }

        /*
         * Write extrn statements.
         */
        if (fType < EXP_IMPORT_FIRST)
        {
            printf("\n"
                   "%s segment\n",
                   aszSeg2[fType]);
            for (i = iOrd; i < iBundle + iOrd; i++)
            {
                int fType;
                int iIndex;

                findNextOrdinal(i - 1, szName, szNameExp, &fType, &iIndex);
                switch (fType)
                {
                    case EXP_ABSOLUTE32:
                    case EXP_32:
                        if (!strchr(szName, '.'))
                            printf("  extrn %s:NEAR\n",
                                   szName);
                        break;
                    case EXP_ABSOLUTE16:
                    case EXP_16:
                        if (!strchr(szName, '.'))
                            printf("  extrn %s:FAR\n",
                                   szName);
                        break;
                }
            }
            printf("\n"
                   "%s ENDS\n",
                   aszSeg2[fType]);
        }


        /*
         * Write bundle header.
         */
        printf("\n"
               "KKLENTRY segment\n"
               "bundle%d label byte\n"
               "db %d ; ordinal %d to %d\n"
               "db %d ; %s\n"
               "dw seg %s ; object\n",
               iOrd,
               iBundle,
               iOrd,
               iOrd + iBundle - 1,
               fType == EXP_ABSOLUTE16 || fType == EXP_16 || fType == EXP_VAR16 || fType == EXP_PROC16 || fType == EXP_ORGPROC16
                  ? 1
                  : 3,
               aszType[fType],
               aszSeg[fType]
               );
        for (i = iOrd; i < iBundle + iOrd; i++)
        {
            int fType;
            int iIndex;

            findNextOrdinal(i - 1, szName, szNameExp, &fType, &iIndex);
            printf("  db 1\n");
            switch (fType)
            {
                case EXP_ABSOLUTE32:
                case EXP_32:
                case EXP_ORGPROC32:
                    printf("  dd offset FLAT:%s\n",
                           szName);
                    break;
                case EXP_ABSOLUTE16:
                case EXP_16:
                case EXP_ORGPROC16:
                    printf("  dw offset %s\n",
                           szName,
                           szName);
                    break;

                case EXP_PROC16:
                case EXP_VAR16:
                    printf("  dw %d ; %s\n",
                           iIndex,
                           szName);
                    break;
                case EXP_PROC32:
                case EXP_VAR32:
                    printf("  dd %d ; %s\n",
                           iIndex,
                           szName);
                    break;
            }
        } /* for */

        /*
         * Write bundle fixups.
         */
        printf("KKLENTRY ends\n"
               "KKLENTRYFIXUPS segment\n"
               "db %d ; type\n"
               "dw offset FLAT:bundle%d - offset FLAT:KKL_EntryTab ; bundle offset\n"
               "KKLENTRYFIXUPS ends\n",
               fType,
               iOrd);

        iPrevBundle += iBundle;
    }

    /*
     * Write stop bundles.
     */
    printf("KKLENTRY segment\n"
           "db 0  ; end bundle\n"
           "dw 0  ; end bundle\n"
           "public KKL_EntryTabEND\n"
           "KKL_EntryTabEND db 0ffh\n"
           "KKLENTRY ends\n"
           "KKLENTRYFIXUPS segment\n"
           "db 0 ; end type\n"
           "KKLENTRYFIXUPS ends\n");

    /*
     * End of file
     */
    printf("\n"
           "end\n"
           "\n");

    return 0;
}

/**
 * Find the highest ordinal value.
 * @returns Max ordinal value.
 */
int findMaxOrdinal(void)
{
    int i;
    int iOrdMax = 0;

    /* search KKL export table */
    for (i = 0; i < sizeof(aEntries) / sizeof(aEntries[0]); i++)
    {
        if (aEntries[i].iOrdinal > iOrdMax)
            iOrdMax = aEntries[i].iOrdinal;
    }

    /* search import table */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (EPTVar(aImportTab[i]))
        {
            if (aImportTab[i].iOrdinal > iOrdMax)
                iOrdMax = aImportTab[i].iOrdinal;
        }
        else
        {
            if (aImportTab[i].iOrdinal + ORD_ORGDISPLACEMENT > iOrdMax)
                iOrdMax = aImportTab[i].iOrdinal + ORD_ORGDISPLACEMENT;
        }
    }

    return iOrdMax;
}

/**
 * Finds the next ordinal.
 * @returns Next ordinal number.
 * @param   iOrd        The current ordinal number. (0 for the first call)
 * @param   pszName     Buffer to hold the internal name.
 * @param   pszNameExp  Buffer to hold the exported name.
 * @param   pfType      Export type.
 * @param   piIndex     Index into the table (use fType to determin table).
 */
int findNextOrdinal(int iOrd, char *pszName, char *pszNameExp, int *pfType, int *piIndex)
{
    int i;
    int iOrdRet = 0x7fff;
    int iIndexDummy;
    int fTypeDummy;

    /* Check for NULLs. */
    if (pfType == NULL)
        pfType = &fTypeDummy;
    if (piIndex == NULL)
        piIndex = &iIndexDummy;

    /* search KKL export table */
    for (i = 0; i < sizeof(aEntries) / sizeof(aEntries[0]); i++)
    {
        if (aEntries[i].iOrdinal > iOrd && aEntries[i].iOrdinal < iOrdRet)
        {
            iOrdRet = aEntries[i].iOrdinal;
            strcpy(pszName, aEntries[i].szName);
            strcpy(pszNameExp, aEntries[i].szNameExp);
            *pfType = aEntries[i].fFlags;
            *piIndex = i;
        }
        else if (aEntries[i].iOrdinal == iOrdRet)
        {
            fprintf(stderr, "Fatal Error: Duplicate ordinal detected (ord=%d)\n", iOrdRet);
            exit(iOrdRet);
        }
    }

    /* search import table */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (aImportTab[i].iOrdinal > iOrd && aImportTab[i].iOrdinal < iOrdRet)
        {
            iOrdRet = aImportTab[i].iOrdinal;
            strcpy(pszName, aImportTab[i].achName);
            strcat(pszName, /*EPTWrapped(aImportTab[i]) ? "_wrapped" :*/ aImportTab[i].achExtra);
            strcpy(pszNameExp, aImportTab[i].achName);
            strcat(pszNameExp, aImportTab[i].achExtra);
            if (EPTVar(aImportTab[i]))
                *pfType = EPT16BitEntry(aImportTab[i]) ? EXP_VAR16 : EXP_VAR32;
            else
                *pfType = EPT16BitEntry(aImportTab[i]) ? EXP_PROC16 : EXP_PROC32;
            *piIndex = i;
        }
    }


    /*
     * Search for Org exports.
     */
    if (iOrdRet > ORD_ORGDISPLACEMENT)
    {
        /*
         * Search for
         */
        for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        {
            if (   aImportTab[i].iOrdinal + ORD_ORGDISPLACEMENT > iOrd
                && aImportTab[i].iOrdinal + ORD_ORGDISPLACEMENT < iOrdRet
                && !EPTVar(aImportTab[i]))
            {
                iOrdRet = aImportTab[i].iOrdinal + ORD_ORGDISPLACEMENT;
                strcpy(pszName, aImportTab[i].achName);
                strcat(pszName, /*EPTWrapped(aImportTab[i]) ? "_wrapped" :*/ aImportTab[i].achExtra);

                if (aImportTab[i].achName[0] == '_')
                {
                    strcpy(pszNameExp, "_Org");
                    strcpy(pszNameExp + 4, &aImportTab[i].achName[1]);
                }
                else
                {
                    strcpy(pszNameExp, "Org");
                    strcpy(pszNameExp + 3, &aImportTab[i].achName[0]);
                }
                strcat(pszNameExp, aImportTab[i].achExtra);

                *pfType = EPT16BitEntry(aImportTab[i]) ? EXP_ORGPROC16 : EXP_ORGPROC32;
                *piIndex = -1;
            }
        }
    }

    return iOrdRet;
}


/**
 * Generates the .Def file used for generating the implib.
 * It's contents is written to stdout.
 */
int GenerateDefFile(void)
{
    int     iOrd;
    int     iOrdMax;
    char    szName[64];
    char    szNameExp[64];

    printf("; $Id: MkCallTab.c,v 1.7 2002-12-16 00:51:33 bird Exp $\n"
           ";\n"
           "; Autogenerated import definition file.\n"
           ";\n"
           "; Generate: mkcalltab.exe def > kKrnlLibImplib.def\n"
           ";\n"
           "; Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>\n"
           ";\n"
           ";\n"
           "; This file is part of kKrnlLib.\n"
           ";\n"
           "; kKrnlLib is free software; you can redistribute it and/or modify\n"
           "; it under the terms of the GNU General Public License as published by\n"
           "; the Free Software Foundation; either version 2 of the License, or\n"
           "; (at your option) any later version.\n"
           ";\n"
           "; kKrnlLib is distributed in the hope that it will be useful,\n"
           "; but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           "; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
           "; GNU General Public License for more details.\n"
           ";\n"
           "; You should have received a copy of the GNU General Public License\n"
           "; along with kKrnlLib; if not, write to the Free Software\n"
           "; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
           ";\n"
           "LIBRARY KKRNLLIB\n"
           "EXPORTS\n");

    iOrdMax = findMaxOrdinal();
    for (iOrd = findNextOrdinal(0, &szName[0], &szNameExp[0], NULL, NULL);
         iOrd < iOrdMax;
         iOrd = findNextOrdinal(iOrd, &szName[0], &szNameExp[0], NULL, NULL))
    {
        printf("    %30s @%d\n", szNameExp, iOrd);
    }
    return 0;
}


/**
 * Generate the (test\)TstFakers.c file.
 * It's contents is written to stdout.
 */
int GenerateTstFakers(void)
{
    int i;

    /*
     * Write Start of file.
     */
    printf("/* $Id: MkCallTab.c,v 1.7 2002-12-16 00:51:33 bird Exp $\n"
           " *\n"
           " * Autogenerated TstFakers file.\n"
           " *\n"
           " * Generate: mkcalltab.exe fake > test\\TstFakers.c\n"
           " *\n"
           " * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>\n"
           " *\n"
           " *\n"
           " * This file is part of kKrnlLib.\n"
           " *\n"
           " * kKrnlLib is free software; you can redistribute it and/or modify\n"
           " * it under the terms of the GNU General Public License as published by\n"
           " * the Free Software Foundation; either version 2 of the License, or\n"
           " * (at your option) any later version.\n"
           " *\n"
           " * kKrnlLib is distributed in the hope that it will be useful,\n"
           " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
           " * GNU General Public License for more details.\n"
           " *\n"
           " * You should have received a copy of the GNU General Public License\n"
           " * along with kKrnlLib; if not, write to the Free Software\n"
           " * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
           " */\n"
           "\n"
           "\n"
           "/*******************************************************************************\n"
           "*   Defined Constants And Macros                                               *\n"
           "*******************************************************************************/\n"
           "#define INCL_NOPMAPI\n"
           "#define LDR_INCL_INITONLY\n"
           "#define INCL_OS2KRNL_ALL\n"
           "\n"
           "\n"
           "/*******************************************************************************\n"
           "*   Header Files                                                               *\n"
           "*******************************************************************************/\n"
           "#include <os2.h>\n"
           "\n"
           "#include \"OS2Krnl.h\"\n"
           "#include \"dev1632.h\"\n"
           "#include \"dev32.h\"\n"
           "#include \"dev32Hlp.h\"\n"
           "#include \"ProbKrnl.h\"\n"
           "#include \"testcase.h\"\n"
           "\n"
           "\n"
           "TSTFAKER aTstFakers[NBR_OF_KRNLIMPORTS] =\n"
           "{\n");

    /*
     * Process aImportTab.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        int     iSeg = EPT16BitEntry(aImportTab[i]) ?
                                ((aImportTab[i].fType & EPT_VAR) ? 4 : 2)
                            :   ((aImportTab[i].fType & EPT_VAR) ? 3 : 1);
        const char *psz = aImportTab[i].achName[0] == '_' ?
                                &aImportTab[i].achName[1]
                            :   &aImportTab[i].achName[0];

        /*
         * Variable or Function?
         */
        printf("    {(unsigned)%sfake%s,%*.s%d}%s\n",
               (aImportTab[i].fType & EPT_VAR) ? "&" : "",
               psz,
               45 - 21 - strlen(psz) - ((aImportTab[i].fType & EPT_VAR) == EPT_VAR),
               "",
               iSeg,
               i + 1 == NBR_OF_KRNLIMPORTS ? "" : ",");
    }

    /*
     * Write End of file.
     */
    printf("};\n"
           "\n");

    return 0;
}

