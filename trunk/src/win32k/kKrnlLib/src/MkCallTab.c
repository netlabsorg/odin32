/* $Id: MkCallTab.c,v 1.1 2002-03-10 02:45:48 bird Exp $
 *
 * Description:     Generates the calltab.asm from aImportTab.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* Disable logging */
#define NOLOGGING 1

#define fclose(a) DosClose(a)
#define SEEK_SET FILE_BEGIN
#define SEEK_END FILE_END


/*
 * Ordinal Ranges - Org exports.
 */
#define ORD_ORGDISPLACEMENT 1200        /* Displacement of the Org ordinals */
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

#include <strat2.h>
#include <reqpkt.h>

#include "devSegDf.h"
#undef  DATA16_INIT
#define DATA16_INIT
#undef  CODE16_INIT
#define CODE16_INIT
#include "os2krnl.h"                    /* must be included before dev1632.h! */
#include "probkrnl.h"
#include "dev1632.h"
#include "vprntf16.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* dummy replacement for SymDB.c */
KRNLDBENTRY   DATA16_INIT    aKrnlSymDB[] = {{0}};

struct Entry
{
    int     iOrdinal;
    char *  szName;
    char *  szNameExp;
    int     fFlags;
} aEntries[] =
{   /* ord, name                       nameexp                          Flags */
    {   1, "_options.ulBuild",         "ulKernelBuild",                 EXP_ABSOLUTE32},
    {   2, "_options.fKernel",         "flKernel",                      EXP_ABSOLUTE32},
    {   3, "_TKSSBase16",              "TKSSBase",                      EXP_ABSOLUTE32},
    {   4, "_TKSSBase16",              "_TKSSBase",                     EXP_ABSOLUTE32},
    {   5, "_R0FlatCS16",              "R0FlatCS",                      EXP_ABSOLUTE16},
    {   6, "_R0FlatDS16",              "R0FlatDS",                      EXP_ABSOLUTE16},

    /* 16-bit funcions */
    {   7, "_printf16",                "_printf16",                     EXP_16},
    {   8, "_vprintf16",               "_vprintf16",                    EXP_16},

    /* clib functions */
    {  10, "GetCS",                     "GetCS",                        EXP_32},
    {  11, "GetDS",                     "GetDS",                        EXP_32},
    {  12, "GetES",                     "GetES",                        EXP_32},
    {  13, "GetFS",                     "GetFS",                        EXP_32},
    {  14, "GetGS",                     "GetGS",                        EXP_32},
    {  15, "GetSS",                     "GetSS",                        EXP_32},
    {  16, "DisableInterrupts",         "DisableInterrupts",            EXP_32},
    {  17, "EnableInterrupts",          "EnableInterrupts",             EXP_32},

    {  20, "AVLStrRemove"              , "AVLStrRemove"               , EXP_32},
    {  21, "AVLStrInsert"              , "AVLStrInsert"               , EXP_32},
    {  22, "AVLStrGet"                 , "AVLStrGet"                  , EXP_32},
    {  23, "AVLStrBeginEnumTree"       , "AVLStrBeginEnumTree"        , EXP_32},
    {  24, "AVLStrGetNextNode"         , "AVLStrGetNextNode"          , EXP_32},
    {  25, "AVLStrDoWithAll"           , "AVLStrDoWithAll"            , EXP_32},
    {  26, "AVLStrIRemove"             , "AVLStrIRemove"              , EXP_32},
    {  27, "AVLStrIInsert"             , "AVLStrIInsert"              , EXP_32},
    {  28, "AVLStrIGet"                , "AVLStrIGet"                 , EXP_32},
    {  29, "AVLStrIBeginEnumTree"      , "AVLStrIBeginEnumTree"       , EXP_32},
    {  30, "AVLStrIGetNextNode"        , "AVLStrIGetNextNode"         , EXP_32},
    {  31, "AVLStrIDoWithAll"          , "AVLStrIDoWithAll"           , EXP_32},
    {  32, "AVLPVRemove"               , "AVLPVRemove"                , EXP_32},
    {  33, "AVLPVInsert"               , "AVLPVInsert"                , EXP_32},
    {  34, "AVLPVGet"                  , "AVLPVGet"                   , EXP_32},
    {  35, "AVLPVGetWithParent"        , "AVLPVGetWithParent"         , EXP_32},
    {  36, "AVLPVGetWithAdjacentNodes" , "AVLPVGetWithAdjacentNodes"  , EXP_32},
    {  37, "AVLPVGetBestFit"           , "AVLPVGetBestFit"            , EXP_32},
    {  38, "AVLPVRemoveBestFit"        , "AVLPVRemoveBestFit"         , EXP_32},
    {  39, "AVLPVBeginEnumTree"        , "AVLPVBeginEnumTree"         , EXP_32},
    {  40, "AVLPVGetNextNode"          , "AVLPVGetNextNode"           , EXP_32},
    {  41, "AVLPVDoWithAll"            , "AVLPVDoWithAll"             , EXP_32},
    {  42, "AVLULMRemove"              , "AVLULMRemove"               , EXP_32},
    {  43, "AVLULMRemove2"             , "AVLULMRemove2"              , EXP_32},
    {  44, "AVLULMInsert"              , "AVLULMInsert"               , EXP_32},
    {  45, "AVLULMGet"                 , "AVLULMGet"                  , EXP_32},
    {  46, "AVLULMGet2"                , "AVLULMGet2"                 , EXP_32},
    {  47, "AVLULMGetWithParent"       , "AVLULMGetWithParent"        , EXP_32},
    {  48, "AVLULMGetWithAdjacentNodes", "AVLULMGetWithAdjacentNodes" , EXP_32},
    {  49, "AVLULMGetBestFit"          , "AVLULMGetBestFit"           , EXP_32},
    {  50, "AVLULMRemoveBestFit"       , "AVLULMRemoveBestFit"        , EXP_32},
    {  51, "AVLULMBeginEnumTree"       , "AVLULMBeginEnumTree"        , EXP_32},
    {  52, "AVLULMGetNextNode"         , "AVLULMGetNextNode"          , EXP_32},
    {  53, "AVLULMDoWithAll"           , "AVLULMDoWithAll"            , EXP_32},

    {  60, "__nw__FUi"                 , "__nw__FUi"                  , EXP_32},
    {  61, "__nw__FUiPv"               , "__nw__FUiPv"                , EXP_32},
    {  62, "__vn__FUi"                 , "__vn__FUi"                  , EXP_32},
    {  63, "__vn__FUiPv"               , "__vn__FUiPv"                , EXP_32},
    {  64, "__dl__FPv"                 , "__dl__FPv"                  , EXP_32},
    {  65, "__vd__FPv"                 , "__vd__FPv"                  , EXP_32},

    {  66, "__nw__FUiPCcT1"            , "__nw__FUiPCcT1"             , EXP_32},
    {  67, "__nw__FUiPCcT1Pv"          , "__nw__FUiPCcT1Pv"           , EXP_32},
    {  68, "__vn__FUiPCcT1"            , "__vn__FUiPCcT1"             , EXP_32},
    {  69, "__vn__FUiPCcT1Pv"          , "__vn__FUiPCcT1Pv"           , EXP_32},
    {  70, "__dl__FPvPCcUi"            , "__dl__FPvPCcUi"             , EXP_32},
    {  71, "__vd__FPvPCcUi"            , "__vd__FPvPCcUi"             , EXP_32},

    {  72, "malloc"                    , "malloc"                     , EXP_32},
    {  73, "realloc"                   , "realloc"                    , EXP_32},
    {  74, "free"                      , "free"                       , EXP_32},
    {  75, "_msize"                    , "_msize"                     , EXP_32},
    {  76, "_memfree"                  , "_memfree"                   , EXP_32},
    {  77, "_heap_check"               , "_heap_check"                , EXP_32},
    {  78, "smalloc"                    , "smalloc"                   , EXP_32},
    {  79, "srealloc"                   , "srealloc"                  , EXP_32},
    {  80, "sfree"                      , "sfree"                     , EXP_32},
    {  81, "_swp_msize"                 , "_swp_msize"                , EXP_32},
    {  82, "_swp_memfree"               , "_swp_memfree"              , EXP_32},
    {  83, "_swp_heap_check"            , "_swp_heap_check"           , EXP_32},
    {  84, "_swp_heapmin"               , "_swp_heapmin"              , EXP_32},
    {  85, "_swp_state"                 , "_swp_state"                , EXP_32},
    {  86, "rmalloc"                    , "rmalloc"                   , EXP_32},
    {  87, "rrealloc"                   , "rrealloc"                  , EXP_32},
    {  88, "rfree"                      , "rfree"                     , EXP_32},
    {  89, "_res_msize"                 , "_res_msize"                , EXP_32},
    {  90, "_res_memfree"               , "_res_memfree"              , EXP_32},
    {  91, "_res_heap_check"            , "_res_heap_check"           , EXP_32},
    {  92, "_res_heapmin"               , "_res_heapmin"              , EXP_32},
    {  93, "_res_state"                 , "_res_state"                , EXP_32},

    { 100, "stricmp"                    , "stricmp"                   , EXP_32},
    { 101, "strnicmp"                   , "strnicmp"                  , EXP_32},

    { 102, "vsprintf"                   , "vsprintf"                  , EXP_32},
    { 103, "sprintf"                    , "sprintf"                   , EXP_32},

    { 104, "vprintf"                    , "vprintf"                   , EXP_32},
    { 105, "vprintf2"                   , "vprintf2"                  , EXP_32},
    { 106, "vprintf"                    , "_vprintfieee"              , EXP_32},
    { 107, "printf"                     , "printf"                    , EXP_32},
    { 108, "printf"                     , "_printfieee"               , EXP_32},
    { 109, "printf"                     , "_printf_ansi"              , EXP_32},

    { 110, "abort"                      , "abort"                     , EXP_32},
    { 111, "abort"                      , "__PureVirtualCalled"       , EXP_32},
    { 112, "_fltused"                   , "_fltused"                  , EXP_32},

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

    /* devhelps */
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
};



/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
extern int  kstrlen(const char *psz);
extern int  kstrcpy(char *psz1, const char *psz2);
extern char*kstrcat(char * pszTarget, const char * pszSource);
const char *kstrchr(const char *psz, int ch);

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
 * strchr implementation.
 * @returns Pointer to first occurence of ch in psz.
 * @param   psz     String to search.
 * @param   ch      Char to search for.
 */
const char *kstrchr(const char *psz, int ch)
{
    while (*psz != '\0' && *psz != (char)ch)
        psz++;
    return *psz == (char)ch ? psz : NULL;
}


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
    else
    {
        syntax();
        return -2;
    }

    return 0;
}


/**
 * Display syntax.
 */
void syntax(void)
{
    printf16("Incorrect parameter!\n"
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
    printf16("; $Id: MkCallTab.c,v 1.1 2002-03-10 02:45:48 bird Exp $\n"
             ";\n"
             "; Autogenerated calltab file.\n"
             ";\n"
             "; Generate: mkcalltab.exe > ldr\calltab.asm\n"
             ";\n"
             "; Copyright (c) 1998-2001 knut st. osmundsen (kosmunds@csc.com)\n"
             ";\n"
             "; Project Odin Software License can be found in LICENSE.TXT\n"
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
    printf16("; Macro which makes a function calltable entry\n"
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
            printf16("%s<%s%s>%*.s;%d\n",
                     pszMacro,
                     pszName,
                     pszExtra,
                     45 - 2 - kstrlen(pszMacro) - aImportTab[i].cchName - kstrlen(pszExtra),
                     "",
                     i);
        }
    }

    /*
     * Write End of Calltab and start of callTab16.
     */
    printf16("\n"
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
            printf16("FnProc16Entry<%s>%*.s;%d\n",
                     aImportTab[i].achName,
                     45 - 2 - 17 - aImportTab[i].cchName,
                     "",
                     i);
        }
    }


    /*
     * Write End of Calltab and start of auFuncs.
     */
    printf16("\n"
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

            printf16(EPT16BitEntry(aImportTab[i])
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
            printf16("    dd  0h\n");
    }


    /*
     * MTE.
     */
    printf16("\n"
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
    printf16(";\n"
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
    printf16("dd  0                                  ; ULONG   smte_impproc;       /* 48  Offset of Imp Procedure Name Tab - POINTER */\n"
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
    printf16("\n"
             ";\n"
             "; Filename\n"
             ";\n"
             "KKL_szFn db \"kKrnlLib.dll\",0\n"
             "\n");

    /*
     * Object table.
     */
    printf16("\n"
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

    printf16("\n"
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
    printf16(";obj 3\n"
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
     * Resident name table.
     */
    printf16("\n"
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
        printf16(
            "db %d, '%s'\n"
            "dw %d\n",
            kstrlen(szNameExp), szNameExp, iOrd);
    }
    #endif
    printf16("db 0 ; terminator\n"
             "public KKL_ResNameTabEND\n"
             "KKL_ResNameTabEND label byte\n"
             "\n"
             "DATA32 ends\n"
             "\n"
             "\n");

    /*
     * Entry table.
     */
    printf16("KKLENTRY segment\n"
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
            printf16("\n"
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
            printf16("\n"
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
                        if (!kstrchr(szName, '.'))
                            printf16("  extrn %s:NEAR\n",
                                     szName);
                        break;
                    case EXP_ABSOLUTE16:
                    case EXP_16:
                        if (!kstrchr(szName, '.'))
                            printf16("  extrn %s:FAR\n",
                                     szName);
                        break;
                }
            }
            printf16("\n"
                     "%s ENDS\n",
                     aszSeg2[fType]);
        }


        /*
         * Write bundle header.
         */
        printf16("\n"
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
            printf16("  db 1\n");
            switch (fType)
            {
                case EXP_ABSOLUTE32:
                case EXP_32:
                case EXP_ORGPROC32:
                    printf16(
                         "  dd offset FLAT:%s\n",
                         szName);
                    break;
                case EXP_ABSOLUTE16:
                case EXP_16:
                case EXP_ORGPROC16:
                    printf16(
                         "  dw offset %s\n",
                         szName,
                         szName);
                    break;

                case EXP_PROC16:
                case EXP_VAR16:
                    printf16(
                         "  dw %d ; %s\n",
                         iIndex,
                         szName);
                    break;
                case EXP_PROC32:
                case EXP_VAR32:
                    printf16(
                         "  dd %d ; %s\n",
                         iIndex,
                         szName);
                    break;
            }
        } /* for */

        /*
         * Write bundle fixups.
         */
        printf16("KKLENTRY ends\n"
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
    printf16("KKLENTRY segment\n"
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
    printf16("\n"
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
            kstrcpy(pszName, aEntries[i].szName);
            kstrcpy(pszNameExp, aEntries[i].szNameExp);
            *pfType = aEntries[i].fFlags;
            *piIndex = i;
        }
        else if (aEntries[i].iOrdinal == iOrdRet)
        {
            printf16("Fatal Error: Duplicate ordinal detected (ord=%d)\n", iOrdRet);
            DosExit(EXIT_PROCESS, iOrdRet);
        }
    }

    /* search import table */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (aImportTab[i].iOrdinal > iOrd && aImportTab[i].iOrdinal < iOrdRet)
        {
            iOrdRet = aImportTab[i].iOrdinal;
            kstrcpy(pszName, aImportTab[i].achName);
            kstrcat(pszName, EPTWrapped(aImportTab[i]) ? "_wrapped" : aImportTab[i].achExtra);
            kstrcpy(pszNameExp, aImportTab[i].achName);
            kstrcat(pszNameExp, aImportTab[i].achExtra);
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
                kstrcpy(pszName, aImportTab[i].achName);
                kstrcat(pszName, EPTWrapped(aImportTab[i]) ? "_wrapped" : aImportTab[i].achExtra);

                if (aImportTab[i].achName[0] == '_')
                {
                    kstrcpy(pszNameExp, "_Org");
                    kstrcpy(pszNameExp + 4, &aImportTab[i].achName[1]);
                }
                else
                {
                    kstrcpy(pszNameExp, "Org");
                    kstrcpy(pszNameExp + 3, &aImportTab[i].achName[0]);
                }
                kstrcat(pszNameExp, aImportTab[i].achExtra);

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

    printf16(";/* $Id: MkCallTab.c,v 1.1 2002-03-10 02:45:48 bird Exp $\n"
             "; *\n"
             "; * Autogenerated .c file.\n"
             "; *\n"
             "; * Generate: mkcalltab.exe deffile > kKrnlLibImp.def\n"
             "; *\n"
             "; * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)\n"
             "; *\n"
             "; * Project Odin Software License can be found in LICENSE.TXT\n"
             "; *\n"
             "; */\n"
             "LIBRARY KKRNLLIB\n"
             "EXPORTS\n");

    iOrdMax = findMaxOrdinal();
    for (iOrd = findNextOrdinal(0, &szName[0], &szNameExp[0], NULL, NULL);
         iOrd < iOrdMax;
         iOrd = findNextOrdinal(iOrd, &szName[0], &szNameExp[0], NULL, NULL))
    {
        printf16("    %30s @%d\n", szNameExp, iOrd);
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
    printf16("/* $Id: MkCallTab.c,v 1.1 2002-03-10 02:45:48 bird Exp $\n"
             " *\n"
             " * Autogenerated TstFakers.c file.\n"
             " *\n"
             " * Generate: mkcalltab.exe fake > test\TstFakers.c\n"
             " *\n"
             " * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)\n"
             " *\n"
             " * Project Odin Software License can be found in LICENSE.TXT\n"
             " *\n"
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
             "/*******************************************************************************\n"
             "*   Header Files                                                               *\n"
             "*******************************************************************************/\n"
             "#include <os2.h>\n"
             "\n"
             "#include \"devSegDf.h\"\n"
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
        printf16("    {(unsigned)%sfake%s,%*.s%d}%s\n",
                 (aImportTab[i].fType & EPT_VAR) ? "&" : "",
                 psz,
                 45 - 21 - kstrlen(psz) - ((aImportTab[i].fType & EPT_VAR) == EPT_VAR),
                 "",
                 iSeg,
                 i + 1 == NBR_OF_KRNLIMPORTS ? "" : ",");
    }

    /*
     * Write End of file.
     */
    printf16("};\n"
             "\n");

    return 0;
}

