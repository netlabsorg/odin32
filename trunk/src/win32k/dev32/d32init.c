/* $Id: d32init.c,v 1.1 1999-09-06 02:19:56 bird Exp $
 *
 * d32init.c - 32-bits init routines.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define MAXSIZE_PROLOG 0x10             /* Note that this must be synced with */
                                        /* the one used in calltab.asm.       */

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define LDR_INCL_INITONLY

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <string.h>

#include "OS2Krnl.h"
#include "options.h"
#include "dev1632.h"
#include "dev32.h"
#include "probkrnl.h"
#include "log.h"
#include "asmutils.h"
#include "cout.h"
#include "malloc.h"
#include "ldr.h"
#include "ldrCalls.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int  interpretFunctionProlog(char *p);
static int  procInit(void);


/* externs located in 16-bit data segement */
extern ULONG  _TKSSBase16;
extern USHORT _R0FlatCS16;
extern USHORT _R0FlatDS16;


/* extern(s) located in calltab.asm */
extern char callTab[NUMBER_OF_PROCS][MAXSIZE_PROLOG];


/**
 * Ring-0, 32-bit, init function.
 * @returns   Status word.
 * @param     pRpInit  Pointer init request packet.
 */
USHORT _loadds _Far32 _Pascal R0Init32(RP32INIT *pRpInit)
{
    char *pszTmp2;
    char *pszTmp;

    TKSSBase32 = _TKSSBase16;

    SET_OPTIONS_TO_DEFAULT(options);

    /*---------------------*/
    /* commandline options */
    /*---------------------*/
    kprintf(("Options start\n"));
    pszTmp = strpbrk(pRpInit->InitArgs, "-/");
    while (pszTmp != NULL)
    {
        char cch;
        pszTmp++; //skip [-/]
        cch = strlen(pszTmp);
        switch (*pszTmp)
        {
            case 'C': /* -C[1|2] - com-port no */
                switch (pszTmp[1])
                {
                    case '1':
                        options.usCom = OUTPUT_COM1;
                        break;
                    case '2':
                        options.usCom = OUTPUT_COM2;
                        break;
                }
                break;

            case 'L': /*-L:[Y|N or ]*/
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1 && (*pszTmp2 == '=' || *pszTmp2 == ':' || *pszTmp2 == ' ' && (pszTmp2[1] != '-' || pszTmp2[1] != '/')))
                {
                    switch (pszTmp2[1])
                    {
                        case 'D': /*disable*/
                            options.fLogging = FALSE;
                            break;
                        case 'E': /*enable*/
                            options.fLogging = TRUE;
                            break;
                        case 'Y': /*yes*/
                            options.fLogging = TRUE;
                            break;
                        case 'N': /*no*/
                            options.fLogging = FALSE;
                            break;
                    }
                }
                else
                    options.fLogging = TRUE;
                break;


            case 'Q': /* quiet initialization */
                options.fQuiet = TRUE;
                break;

            case 'S': /* SMP kernel */
                options.fKernel = KF_SMP;
                break;

            case 'V': /* verbose initialization */
                options.fQuiet = TRUE;
                break;

            case 'U': /* UNI kernel */
                options.fKernel = KF_SMP;
                break;
        }
        pszTmp = strpbrk(pszTmp, "-/");
    }

    /* Transfer version and build number from 16-bit probkrnl.c */
    options.ulBuild    = _ulBuild;
    options.usVerMajor = _usVerMajor;
    options.usVerMinor = _usVerMinor;

    /* log option summary */
    kprintf(("Options - Summary\n"));
    kprintf(("\tKernel: ver%d.%d  build %d  type %s\n",
                options.usVerMajor,
                options.usVerMinor,
                options.ulBuild,
                (options.fKernel & KF_SMP) ? "SMP" : "UNI"
              ));
    kprintf(("\tCom port no.%d\n", options.usCom));
    if (options.fQuiet)
        kprintf(("\tQuiet init\n"));
    else
        kprintf(("\tVerbose init\n"));
    if (options.fLogging)
        kprintf(("\tlogging enabled\n"));
    else
        kprintf(("\tlogging disabled\n"));
    /* end option summary */


    /*
     * init sub-parts
     */

    /* heap */
    if (heapInit(HEAP_SIZE) != NO_ERROR)
        return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

    /* cout init */
    coutInit(options.usCom);

    /* loader */
    if (ldrInit() != NO_ERROR)
        return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

    /* functionoverrides */
    if (procInit() != NO_ERROR)
        return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

    return STATUS_DONE;
}


/**
 * Verifies the aProcTab.
 * @returns   0 if ok. !0 if not ok.
 * @remark    Called from IOCtl.
 */
USHORT _loadds _Far32 _Pascal VerifyProcTab32(void)
{
    int i;
    int cb;

    /* verify */
    for (i = 0; i < NUMBER_OF_PROCS; i++)
    {
        /* verify that it is found */
        if (!_aProcTab[i].fFound)
        {
            kprintf(("VerifyProcTab32: procedure no.%d was not found!\n", i));
            return 1;
        }

        /* verify read/writeable. - NOT IMPLEMENTED (yet) */

        /* verify known function prolog. (only EPT_PROC) */
        if (_aProcTab[i].fType == EPT_PROC)
        {
            if ((cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress)) <= 0 && cb + 5 >= MAXSIZE_PROLOG)
            {
                kprintf(("VerifyProcTab32: verify failed for procedure no.%d\n",i));
                return 2;
            }
        }
        else
        {
            kprintf(("VerifyProcTab32: only EPT_PROC is implemented\n",i));
            return 3;
        }
    }

    return 0;
}


/**
 * Get kernel OTEs
 * @returns   0 if ok. !0 on failiure.
 * @param     pOTEBuf  Pointer to output buffer.
 * @remark    Called from IOCtl.
 */
USHORT _loadds _Far32 _Pascal GetOTEs32(PKRNLOBJTABLE pOTEBuf)
{
    PMTE    pMTE;
    PSMTE   pSMTE;
    POTE    pOTE;
    int     i;
    USHORT  usRc;

    pMTE = GetOS2KrnlMTE();
    if (pMTE != NULL)
    {
        pSMTE = pMTE->mte_swapmte;
        if (pSMTE != NULL)
        {
            pOTEBuf->cObjects = pSMTE->smte_objcnt;
            if (pSMTE->smte_objcnt <= MAXKRNLOBJECTS)
            {
                pOTE = pSMTE->smte_objtab;
                if (pOTE != NULL)
                {
                    for (i = 0; i < pOTEBuf->cObjects; i++)
                        memcpy((void*)&pOTEBuf->aObjects[i], &pOTE[i], sizeof(OTE));
                    usRc = 0;
                }
                else
                    usRc = 4;
            }
            else
                usRc = 3;
        }
        else
            usRc = 2;
    }
    else
        usRc = 1;

    if (usRc != 0)
        kprintf(("GetOTEs32: failed. usRc = %d\n", usRc));

    return usRc;
}


/**
 * Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     p  Pointer to prolog.
 */
static int interpretFunctionProlog(char *p)
{
    int length;
    int rc;

    /*
     * check for the well known prolog (the only that is supported now)
     * which is:
     *     push ebp
     *     mov ebp,esp
     */

    if (p[0] == 0x55 && p[1] == 0x8b && p[2] == 0xec)
    {
        rc = 3;
        while (rc < 5)
        {
            /*
             * This is not at all very stable or correct - but it works
             * for the current functions.
             * There will never be any doubt when something goes wrong!
             */
            switch(p[rc])
            {
                case 0x33: /* xor (ldrClose, ldrOpen) */
                    rc +=2;
                    break;
                case 0x8d: /* lea (ldrRead) */
                    rc += 3;
                    break;
                case 0x83: /* sub (LDRQAppType) */
                    rc += 3;
                    break;
                default:
                    kprintf(("interpretFunctionProlog: unknown instruction 0x%x \n", p[rc]));
                    return 0;
            }
        }
    }
    else
        rc = 0;

    return rc;
}



/**
 * Initiates the overrided functions.
 * @returns   NO_ERROR on success. !0 on error.
 */
static int procInit(void)
{
    int i;
    int cb;

    /* verify */
    for (i = 0; i < NUMBER_OF_PROCS; i++)
    {
        if ((cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress)) <= 0 && cb + 5 >= MAXSIZE_PROLOG)
        {
            kprintf(("rehookFunctions: verify failed for procedure no.%d\n", i));
            return 1;
        }
    }

    /* rehook */
    for (i = 0; i < NUMBER_OF_PROCS; i++)
    {
        cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress);
        if (cb > 0 && cb + 5 < MAXSIZE_PROLOG)
        {
            char *pMy;
            switch (i)
            {
                case iLDRREAD:          pMy = (char*)myldrRead; break;
                case iLDROPEN:          pMy = (char*)myldrOpen; break;
                case iLDRCLOSE:         pMy = (char*)myldrClose; break;
                case iLDRQAPPTYPE:      pMy = (char*)myLDRQAppType; break;

                default:
                    kprintf(("rehookFunctions: Someone has added function without updating the switch! i=%d\n", i));
                    Int3();
                    return 2;
            }

            /* copy function prolog */
            memcpy(callTab[i], (char*)_aProcTab[i].ulAddress, cb);

            /* jump from calltab to original function */
            callTab[i][cb] = 0xE9; /* jmp */
            *(unsigned*)&callTab[i][cb+1] = _aProcTab[i].ulAddress + cb - (unsigned)&callTab[i][cb+5];

            /* jump from original function to my function - an cli could be needed here */
            *(char*)_aProcTab[i].ulAddress = 0xE9; /* jmp */
            *(unsigned*)(_aProcTab[i].ulAddress + 1) = (unsigned)pMy - (_aProcTab[i].ulAddress + 5);
        }
        else
        {
            /* !fatal! - this could never happen really... */
            kprintf(("rehookFunctions: FATAL verify failed for procedure no.%d when rehooking it!\n",i));
            Int3(); /* ipe - later! */
            return 1;
        }
        i++;
    }

    return NO_ERROR;
}

