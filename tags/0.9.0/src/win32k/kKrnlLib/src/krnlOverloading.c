/* $Id: krnlOverloading.c,v 1.3 2002-12-19 01:49:09 bird Exp $
 *
 * Interfaces for overloading kernel routines.
 *
 * Copyright (c) 2001-2001 knut st. osmundsen <bird@anduin.net>
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

#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlOverloading.c,v 1.3 2002-12-19 01:49:09 bird Exp $";
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <kLib/kTypes.h>

#define INCL_OS2KRNL_ALL
#include "OS2Krnl.h"

#define INCL_KKL_MISC
#define INCL_KKL_LOG
#define INCL_KKL_FUNC
#include "kKrnlLib.h"

#include "krnlImportTable.h"
#include "krnlPrivate.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int krnlValidate32(ULONG ulFuncAddr);
int krnlValidate16(ULONG ulFuncFarAddr);


/**
 * Validates a function address.
 *
 * @returns Index of the import function into aImportTab if found.
 *          -1 if not found.
 * @param   ulFuncAdd   32-bit Address of the function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
int krnlValidate32(ULONG ulFuncAddr)
{
    KLOGENTRY1("int","ULONG ulFuncAddr", ulFuncAddr);
    int i;

    /* search thru the importtab for the function */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        if (ulFuncAddr == aImportTab[i].ulAddress)
        {
            KLOGEXIT(aImportTab[i].fFound && !EPTVar(aImportTab[i]) ? i : -1);
            return aImportTab[i].fFound && !EPTVar(aImportTab[i]) ? i : -1;
        }

    /* not found */
    KLOGEXIT(-1);
    return -1;
}


/**
 * Validates a function address.
 *
 * @returns Index of the import function into aImportTab if found.
 *          -1 if not found.
 * @param   ulFuncFarAddr   16:16 far address of the function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
int krnlValidate16(ULONG ulFuncFarAddr)
{
    KLOGENTRY1("int","ULONG ulFuncFarAddr", ulFuncFarAddr);
    int i;

    /* search thru the importtab for the function */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
        if (    aImportTab[i].offObject == (USHORT)ulFuncFarAddr
            &&  aImportTab[i].usSel == (USHORT)(ulFuncFarAddr >> 16))
        {
            KLOGEXIT(aImportTab[i].fFound && !EPTVar(aImportTab[i]) ? i : -1);
            return aImportTab[i].fFound && !EPTVar(aImportTab[i]) ? i : -1;
        }

    /* not found */
    KLOGEXIT(-1);
    return -1;
}


/**
 * Find the original prolog (in the callTabs) for a function.
 * @returns Pointer to the prolog if found.
 *          NULL if not found.
 * @param   iFunc   Function index int aImportTab.
 */
char * krnlFindOrgProlog(int iFunc)
{
    KLOGENTRY1("char *","int iFunc", iFunc);
    int     i;
    char *  pch16 = &callTab16[0];
    char *  pch32 = &callTab[0];

    /*
     * Validate input.
     */
    if (iFunc >= NBR_OF_KRNLIMPORTS)
    {
        kprintf(("invalid function index %d.\n", iFunc));
        KLOGEXIT(NULL);
        return NULL;
    }
    if (EPTVar(aImportTab[i]))
    {
        kprintf(("tried to get prolog for a variable iFunc=%d.\n", iFunc));
        KLOGEXIT(NULL);
        return NULL;
    }

    /*
     * Walk thru all entries to this entry.
     */
    for (i = 0; i < iFunc; i++)
    {
        switch (aImportTab[i].fType & ~EPT_WRAPPED)
        {
            /*
             * 32-bit procedure.
             */
            case EPT_PROCNR32:     /* Not required */
            case EPT_PROC32:
                pch32 += OVERLOAD32_ENTRY;
                break;

            /*
             * 16-bit procedure overload.
             */
            case EPT_PROCNR16:    /* Not required */
            case EPT_PROC16:
                pch16 += OVERLOAD16_ENTRY;
                break;

            /*
             * 16-bit imported hybrid procedure.
             */
            case EPT_PROCH16:
                pch16 += IMPORTH16_ENTRY;
                break;

            /*
             * 16/32-bit importe variable.
             */
            case EPT_VARNR32:
            case EPT_VARNR16:
            case EPT_VAR32:
            case EPT_VAR16:
                pch32 += VARIMPORT_ENTRY;
                break;

            default:
                kprintf(("unsupported type. (procedure no.%d)\n", i));
                INT3(); /* ipe - later! */
                KLOGEXIT(NULL);
                return NULL;
        } /* switch - type */
    }

    KLOGEXIT((EPT32BitEntry(aImportTab[i])) ? pch32 : pch16);
    return (EPT32BitEntry(aImportTab[i])) ? pch32 : pch16;
}



/**
 * Overloads a 32-bit kernel function which is exported by us.
 * @returns Success indicator.
 * @param   ulFuncAddr          Address (32-bit) of the function to overload.
 *                              This have to be the one not prefixed by Org.
 * @param   ulOverloaderAddr    Address (32-bit) of the overloading function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
KBOOL kKLOverload32(ULONG ulFuncAddr, ULONG ulOverloaderAddr)
{
    KLOGENTRY2("KBOOL","ULONG ulFuncAddr, ULONG ulOverloaderAddr", ulFuncAddr, ulOverloaderAddr);
    int     iFunc;

    /*
     * Validate the function address and get it's aImportTab index.
     */
    iFunc = krnlValidate32(ulFuncAddr);
    if (iFunc < 0)
    {
        kprintf(("Invalid function address. ulFuncAddr = %x\n", ulFuncAddr));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    IMPORT_LOCK();

    /*
     * Check that the function is not allready overloaded.
     */
    if (*(char*)ulFuncAddr == 0xe9) /* not jmp */
    {
        IMPORT_UNLOCK();
        kprintf(("Function allready overloaded. ulFuncAddr = %x %s\n",
                 ulFuncAddr, aImportTab[iFunc].achName));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    /*
     * Overload the function.
     */
    krnlLockedWrite(ulFuncAddr, 0xe9, ulOverloaderAddr - ulFuncAddr + 5);

    IMPORT_UNLOCK();

    KLOGEXIT(TRUE);
    return TRUE;
}


/**
 * Restores the original function prolog for an overloaded
 * function.
 * @returns Success indicator.
 * @param   ulFuncAddr          The address (32-bit) of the kernel function.
 *                              (Not the Org prefixed.)
 * @param   ulOverloaderAddr    The address (32-bit) of the function which _is_
 *                              currently overloading the function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
KBOOL kKLRestore32(ULONG ulFuncAddr, ULONG ulOverloaderAddr)
{
    KLOGENTRY2("KBOOL","ULONG ulFuncAddr, ULONG ulOverloaderAddr", ulFuncAddr, ulOverloaderAddr);
    int     iFunc;
    char *  pchOrgProlog;

    /*
     * Validate the function address and get it's aImportTab index.
     */
    iFunc = krnlValidate32(ulFuncAddr);
    if (iFunc < 0)
    {
        kprintf(("Invalid function address. ulFuncAddr = %x\n", ulFuncAddr));
        KLOGEXIT(FALSE);
        return FALSE;
    }
    pchOrgProlog = krnlFindOrgProlog(iFunc);
    if (!pchOrgProlog)
    {
        kprintf(("Internal error! Failed to find orgprolog. ulFuncAddr = %x (%s)\n",
                 ulFuncAddr, aImportTab[iFunc].achName));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    IMPORT_LOCK();

    /*
     * Check that the function is overloaded.
     */
    if (*(char*)ulFuncAddr != 0xe9) /* jmp */
    {
        IMPORT_UNLOCK();
        kprintf(("Function not overloaded. ulFuncAddr = %x %s\n",
                 ulFuncAddr, aImportTab[iFunc].achName));
        KLOGEXIT(FALSE);
        return FALSE;
    }
    if (*(unsigned long *)(ulFuncAddr + 1) != (ulOverloaderAddr - ulFuncAddr + 5)) /* jmp */
    {
        IMPORT_UNLOCK();
        kprintf(("Function was overloaded by caller. ulFuncAddr = %x (%s) Realoverloader = %x\n",
                 ulFuncAddr, aImportTab[iFunc].achName, *(unsigned long *)(ulFuncAddr + 1) + ulFuncAddr - 5));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    /*
     * Restore function prolog.
     */
    krnlLockedWrite(ulFuncAddr, *pchOrgProlog, *(unsigned long *)(void*)(pchOrgProlog + 1));

    IMPORT_UNLOCK();

    KLOGEXIT(TRUE);
    return TRUE;
}


/**
 * Overloads a 16-bit kernel function.
 * @returns Success indicator.
 * @param   ulFuncFarAddr           Far address of the kernel function.
 *                                  (Not the Org prefixed one.)
 * @param   ulOverloaderFarAddr     Far address of the overloader function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
KBOOL kKLOverload16(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr)
{
    KLOGENTRY2("KBOOL","ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr", ulFuncFarAddr, ulOverloaderFarAddr);
    int     iFunc;

    /*
     * Validate the function address and get it's aImportTab index.
     */
    iFunc = krnlValidate16(ulFuncFarAddr);
    if (iFunc < 0)
    {
        kprintf(("Invalid function address. ulFuncFarAddr = %04x:%04x\n",
                 ulFuncFarAddr >> 16, (USHORT)ulFuncFarAddr));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    IMPORT_LOCK();

    /*
     * Check that the function is not allready overloaded.
     */
    if (*(char *)aImportTab[iFunc].ulAddress == 0xea) /* not jmp */
    {
        IMPORT_UNLOCK();
        kprintf(("Function allready overloaded. ulFuncFarAddr = %04x:%04x %s\n",
                 ulFuncFarAddr >> 16, (USHORT)ulFuncFarAddr, aImportTab[iFunc].achName));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    /*
     * Overload the function.
     */
    krnlLockedWrite(aImportTab[iFunc].ulAddress, 0xea, ulOverloaderFarAddr);

    IMPORT_UNLOCK();

    KLOGEXIT(TRUE);
    return TRUE;
}


/**
 * Restores a overloaded 16-bit kernel function.
 * @returns Success indicator.
 * @param   ulFuncFarAddr           Far address of the kernel function.
 *                                  (Not the Org prefixed one.)
 * @param   ulOverloaderFarAddr     Far address of the overloader function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark  Currently not supported.
 */
KBOOL kKLRestore16(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr)
{
    KLOGENTRY2("KBOOL","ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr", ulFuncFarAddr, ulOverloaderFarAddr);
    int     iFunc;
    char *  pchOrgProlog;

    /*
     * Validate the function address and get it's aImportTab index.
     */
    iFunc = krnlValidate16(ulFuncFarAddr);
    if (iFunc < 0)
    {
        kprintf(("Invalid function address. ulFuncFarAddr = %04x:%04x\n",
                 ulFuncFarAddr >> 16, (USHORT)ulFuncFarAddr));
        KLOGEXIT(FALSE);
        return FALSE;
    }
    pchOrgProlog = krnlFindOrgProlog(iFunc);
    if (!pchOrgProlog)
    {
        kprintf(("Internal error! Failed to find orgprolog. ulFuncFarAddr = %04x:%04x (%s)\n",
                 ulFuncFarAddr >> 16, (USHORT)ulFuncFarAddr, aImportTab[iFunc].achName));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    IMPORT_LOCK();

    /*
     * Check that the function is overloaded.
     */
    if (*(char*)aImportTab[iFunc].ulAddress != 0xea) /* jmp far */
    {
        IMPORT_UNLOCK();
        kprintf(("Function not overloaded. ulFuncFarAddr = %04x:%04x %s\n",
                 ulFuncFarAddr >> 16, (USHORT)ulFuncFarAddr, aImportTab[iFunc].achName));
        KLOGEXIT(FALSE);
        return FALSE;
    }
    if (*(unsigned long *)(aImportTab[iFunc].ulAddress + 1) != ulOverloaderFarAddr)
    {
        IMPORT_UNLOCK();
        kprintf(("Function was overloaded by caller. ulFuncAddr = %04x:%04x (%s) Realoverloader = %04x:%04x\n",
                 ulFuncFarAddr >> 16, (USHORT)ulFuncFarAddr, aImportTab[iFunc].achName,
                 *(unsigned short *)(aImportTab[iFunc].ulAddress + 3), *(unsigned short *)(aImportTab[iFunc].ulAddress + 1)));
        KLOGEXIT(FALSE);
        return FALSE;
    }

    /*
     * Restore function prolog.
     */
    krnlLockedWrite(aImportTab[iFunc].ulAddress, *pchOrgProlog, *(unsigned long *)(void*)(pchOrgProlog + 1));

    IMPORT_UNLOCK();

    KLOGEXIT(TRUE);
    return TRUE;
}


/**
 * Overloads a hybrid 16-bit kernel function.
 * @returns Success indicator.
 * @param   ulFuncFarAddr           Far address of the kernel function.
 *                                  (Not the Org prefixed one.)
 * @param   ulOverloaderFarAddr     Far address of the overloader function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark  Currently not supported.
 */
KBOOL kKLOverload16H(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr)
{
    KLOGENTRY2("KBOOL","ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr", ulFuncFarAddr, ulOverloaderFarAddr);
    kprintf(("not supported currently\n"));
    KNOREF(ulFuncFarAddr);
    KNOREF(ulOverloaderFarAddr);
    KLOGEXIT(FALSE);
    return FALSE;
}


/**
 * Restores a overloaded hybrid 16-bit function.
 * @returns Success indicator.
 * @param   ulFuncFarAddr           Far address of the kernel function.
 *                                  (Not the Org prefixed one.)
 * @param   ulOverloaderFarAddr     Far address of the overloader function.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark  Currently not supported.
 */
KBOOL kKLRestore16H(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr)
{
    KLOGENTRY2("KBOOL","ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr", ulFuncFarAddr, ulOverloaderFarAddr);
    kprintf(("not supported currently\n"));
    KNOREF(ulFuncFarAddr);
    KNOREF(ulOverloaderFarAddr);
    KLOGEXIT(FALSE);
    return FALSE;
}

