/* $Id: mkcalltab.c,v 1.8 2001-07-31 21:35:30 bird Exp $
 *
 * Description:     Generates the calltab.asm from aImportTab.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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

/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
extern int  kstrlen(const char *psz);

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void syntax(void);
int GenerateCalltab(void);
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
    int i;

    /*
     * Write Start of file.
     */
    printf16("; $Id: mkcalltab.c,v 1.8 2001-07-31 21:35:30 bird Exp $\n"
             ";\n"
             "; Autogenerated calltab file.\n"
             ";\n"
             "; Generate: mkcalltab.exe > ldr\calltab.asm\n"
             ";\n"
             "; Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)\n"
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
             "\n"
             "\n"
             ";\n"
             "; Exported symbols\n"
             ";\n"
             "    public callTab\n"
             "    public callTab16\n"
             "    public auFuncs\n"
             "\n"
             "\n"
             ";\n"
             "; Constants\n"
             ";\n"
             "OVERLOAD32_PROLOG     EQU 14h\n"
             "OVERLOAD16_PROLOG     EQU 14h\n"
             "IMPORT32_PROLOG       EQU 08h\n"
             "NRIMPORT32_PROLOG     EQU 06h\n"
             "IMPORT16_PROLOG       EQU 08h\n"
             "NRIMPORT16_PROLOG     EQU 07h\n"
             "IMPORTH16_PROLOG      EQU 08h\n"
             "IMPORTHNR16_PROLOG    EQU 07h\n"
             "\n"
             "\n"
             ";\n"
             "; Macros\n"
             ";\n"
             "\n"
             "; Macro which makes a function overload calltable entry\n"
             "FnOverload32Entry macro fnname\n"
             "    public fnname\n"
             "    fnname proc near\n"
             "        db OVERLOAD32_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "endm\n"
             "\n"
             "; Macro which makes a function overload calltable entry\n"
             "FnOverload16Entry macro fnname\n"
             "    public fnname\n"
             "    public &fnname&_off\n"
             "    public &fnname&_sel\n"
             "    fnname proc near\n"
             "        db OVERLOAD16_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "    &fnname&_off         dw  0\n"
             "    &fnname&_sel         dw  0\n"
             "endm\n"
             "\n"
             "; Macro which makes a function calltable entry\n"
             "FnImport32Entry macro fnname\n"
             "    public fnname\n"
             "    fnname proc near\n"
             "        db IMPORT32_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "endm\n"
             "\n");
    printf16("; Macro which makes a function calltable entry\n"
             "FnNRImport32Entry macro fnname\n"
             "    public fnname\n"
             "    fnname proc near\n"
             "        db NRIMPORT32_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "    f&fnname&            dw  0\n"
             "endm\n"
             "\n"
             "; Macro which makes a function calltable entry\n"
             "FnImport16Entry macro fnname\n"
             "    public fnname\n"
             "    fnname proc near\n"
             "        db IMPORT16_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "endm\n"
             "\n"
             "; Macro which makes a function calltable entry\n"
             "FnHybrid16Entry macro fnname\n"
             "    public fnname\n"
             "    fnname proc near\n"
             "        db IMPORTH16_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "endm\n"
             "\n"
             "; Macro which makes a function calltable entry\n"
             "FnNRImport16Entry macro fnname\n"
             "    public fnname\n"
             "    fnname proc near\n"
             "        db NRIMPORT16_PROLOG dup(0cch)\n"
             "    fnname endp\n"
             "    f&fnname&            db  0\n"
             "endm\n"
             "\n");
    printf16("; Macro which makes a variable calltable entry.\n"
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
             "CODE32 segment\n"
             "    ;assume cs:CODE32, ds:flat, ss:nothing\n"
             "    assume ds:flat, ss:nothing\n"
             ";\n"
             "; callTab is an array of function prologs with a jump to the real function\n"
             "; and pointers to real variables.\n"
             ";\n"
             "; Imported and Overrided OS/2 kernel functions are called thru this table.\n"
             "; Imported OS/2 kernel variables are accessed thru this table.\n"
             ";\n"
             "callTab:\n");

    /*
     * Process aImportTab - all but 16-bit overrides.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        if (!EPT16Proc(aImportTab[i]) && !EPTProcImportHybrid(aImportTab[i]))
        {
            char *pszMacro = (aImportTab[i].fType & EPT_VARIMPORT) ?
                                    "VariableEntry"
                                : (  (aImportTab[i].fType & EPT_PROCIMPORT) ?
                                   ( (aImportTab[i].fType & EPT_NOT_REQ) ?
                                    (EPT16BitEntry(aImportTab[i]) ? "FnNRImport16Entry" : "FnNRImport32Entry")
                                :   (EPT16BitEntry(aImportTab[i]) ? "FnImport16Entry"   : "FnImport32Entry"))
                                :   (EPT16BitEntry(aImportTab[i]) ? "FnOverload16Entry" : "FnOverload32Entry"));
            char *pszName = &aImportTab[i].achName[(aImportTab[i].fType & EPT_VARIMPORT) && aImportTab[i].achName[0] == '_'];
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
             "CODE32 ENDS\n"
             "\n"
             "\n"
             "CODE16 SEGMENT\n"
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
        if (EPT16Proc(aImportTab[i]))
        {
            printf16("FnOverload16Entry<%s>%*.s;%d\n",
                     aImportTab[i].achName,
                     45 - 2 - 17 - aImportTab[i].cchName,
                     "",
                     i);
        }
        else if (EPT16ProcImportH(aImportTab[i]))
        {
            printf16("FnHybrid16Entry<%s>%*.s;%d\n",
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
             "CODE16 ENDS\n"
             "\n"
             "\n"
             "DATA32 SEGMENT\n"
             "    assume ds:nothing, es:nothing\n"
             "\n"
             ";\n"
             "; Function table for overloader and nop functions.\n"
             "; Used by importTabInit() in d32init.c.\n"
             ";\n"
             "auFuncs:\n"
             );

    /*
     * Process aImportTab to generate auFuncs.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        char *  pszSeg      = (EPT32BitEntry(aImportTab[i]) ? "32" : "16");
        char *  pszPrefix   = (aImportTab[i].achName[0] == '_' ? "_" : "");
        char *  pszName     = (*pszPrefix != '\0' ? &aImportTab[i].achName[1] : &aImportTab[i].achName[0]);

        if (EPT16Proc(aImportTab[i]) && !EPTProcImportHybrid(aImportTab[i]))
        {   /* Overload procedure (Overloader function) */
            #if 1
            printf16("    extrn %smy%s%s : FAR\n"
                     "    dw  offset CODE%s:%smy%s%s\n"
                     "    dw  seg CODE%s:%smy%s%s\n",
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra,
                     pszSeg,
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra,
                     pszSeg,
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra);
            #else
            printf16("    extrn %smy%s%s : FAR\n"
                     "    dd  far ptr CODE%s:%smy%s%s\n",
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra,
                     pszSeg,
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra);
            #endif
        }
        else if (!(aImportTab[i].fType & (EPT_PROCIMPORT | EPT_PROCIMPORTH |EPT_VARIMPORT | EPT_NOT_REQ)))
        {   /* Overload procedure (Overloader function) */
            printf16("    extrn %smy%s%s : NEAR\n"
                     "    dd  offset FLAT:CODE%s:%smy%s%s\n",
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra,
                     pszSeg,
                     pszPrefix,
                     pszName,
                     aImportTab[i].achExtra);
        }
        else if ((aImportTab[i].fType & (EPT_NOT_REQ | EPT_VARIMPORT | EPT_PROCIMPORT | EPT_PROCIMPORTH)) == (EPT_NOT_REQ | EPT_PROCIMPORT))
        {   /* Not required imported function (NOP function) */
            printf16("    extrn %snop%s%s : NEAR\n"
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
     * Write End of file.
     */
    printf16("\n"
             "DATA32 ENDS\n"
             "\n"
             "END\n");


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
    printf16("/* $Id: mkcalltab.c,v 1.8 2001-07-31 21:35:30 bird Exp $\n"
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
             "#include \"dev32hlp.h\"\n"
             "#include \"probkrnl.h\"\n"
             "#include \"ldr.h\"\n"
             "#include \"test.h\"\n"
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
                                ((aImportTab[i].fType & EPT_VARIMPORT) ? 4 : 2)
                            :   ((aImportTab[i].fType & EPT_VARIMPORT) ? 3 : 1);
        const char *psz = aImportTab[i].achName[0] == '_' ?
                                &aImportTab[i].achName[1]
                            :   &aImportTab[i].achName[0];

        /*
         * Variable or Function?
         */
        printf16("    {(unsigned)%sfake%s,%*.s%d}%s\n",
                 (aImportTab[i].fType & EPT_VARIMPORT) ? "&" : "",
                 psz,
                 45 - 21 - kstrlen(psz) - ((aImportTab[i].fType & EPT_VARIMPORT) == EPT_VARIMPORT),
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
