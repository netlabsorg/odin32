/* $Id: krnlLoadKernelSym.c,v 1.2 2002-12-19 01:49:08 bird Exp $
 *
 * Description:   Autoprobes the os2krnl file and os2krnl[*].sym files.
 *
 *                How this used to works:
 *                1. parses the device-line parameters and collects some "SysInfo".
 *                2. gets the kernel object table and kernel info like build no. (kKrnlHlp)
 *                3. if non-debug kernel the symbol database is scanned to get the syms
 *                4. if Syms not found THEN locates and scans the symbol-file(s) for the
 *                   entrypoints which are wanted.
 *                5. the entry points are verified. (kKrnlHlp)
 *                6. finished.
 *
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

#define NOFUNCTIONNAME
#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlLoadKernelSym.c,v 1.2 2002-12-19 01:49:08 bird Exp $";
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <kLib/format/SYMdbg.h>
#include <kLib/kDevHlp.h>
#include <kLib/kLog.h>
#include "devErrors.h"
#include "options.h"
#include "krnlImportTable.h"
#define INCL_OS2KRNL_LDR
#include "OS2Krnl.h"
#include "krnlPrivate.h"



#include <string.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/**
 * Common symbol file locations.
 */
static char *apszSym[] =
{
    "c:\\os2krnl.sym",                              /* usual for debugkernel */
    "c:\\os2\\pdpsi\\pmdf\\warp4\\os2krnlr.sym",    /* warp 4 */
    "c:\\os2\\pdpsi\\pmdf\\warp4\\os2krnld.sym",    /* warp 4 */
    "c:\\os2\\pdpsi\\pmdf\\warp4\\os2krnlb.sym",    /* warp 4 */
    "c:\\os2\\pdpsi\\pmdf\\warp45_u\\os2krnlr.sym", /* warp 45 */
    "c:\\os2\\pdpsi\\pmdf\\warp45_u\\os2krnld.sym", /* warp 45 */
    "c:\\os2\\pdpsi\\pmdf\\warp45_u\\os2krnlb.sym", /* warp 45 */
    "c:\\os2\\pdpsi\\pmdf\\warp45_s\\os2krnlr.sym", /* warp 45 */
    "c:\\os2\\pdpsi\\pmdf\\warp45_s\\os2krnld.sym", /* warp 45 */
    "c:\\os2\\pdpsi\\pmdf\\warp45_s\\os2krnlb.sym", /* warp 45 */
    "c:\\os2\\system\\pmdf\\os2krnlr.sym",          /* warp 3 ?*/
    "c:\\os2\\system\\pmdf\\os2krnld.sym",          /* warp 3 ?*/
    "c:\\os2\\system\\pmdf\\os2krnlb.sym",          /* warp 3 ?*/
    "c:\\os2\\system\\trace\\os2krnl.sym",          /* warp 3 ?*/
    "c:\\os2krnlr.sym",                             /* custom */
    "c:\\os2krnlb.sym",                             /* custom */
    "c:\\os2krnld.sym",                             /* custom */
    NULL
};


/**
 * This function will loop thru all possible kernel
 * sym files and try match them with the loaded kernel.
 *
 * @returns 0 on success.
 *          The kernel import table contains valid data.
 * @returns error code on failure. Upper part contains failed proc.
 *          The kernel import table contains invalid data.
 * @sketch
 * @modifies szSymbolFile, aImportTab.
 *
 */
int krnlLoadKernelSym(void)
{
    int         rc;
    int         i;
    PGINFOSEG   pGIS;
    char        chBootDrive;

    /*
     * Determin boot drive and update the table.
     * Note! Make sure the strings are not readonly const!
     */
    pGIS = kDH_GetDOSVar(DHGETDOSV_SYSINFOSEG, 0);
    if (!pGIS)
        return ERROR_PROB_KRNL_GIS;
    dprintf(("BootDrive: %d\n", pGIS->bootdrive));

    chBootDrive = (char)(pGIS->bootdrive + 'a' - 1);
    for (i = 0; apszSym[i] != NULL; i++)
        apszSym[i][0] = chBootDrive;

    /*
     * If symfile was specified, we will try read it.
     */
    if (szSymbolFile[0] != '\0')
    {
        rc = krnlLoadKernelSymFile(szSymbolFile, pKrnlOTE, cKernelObjects);
        if (!rc)
            rc = krnlVerifyImportTab();

        if (rc)
        {
            dprintf(("Warning: Invalid symbol file specified. rc=%x ordinal=%d\n"
                     "         Tries defaults.\n",
                     rc,
                     (unsigned)rc >> ERROR_D32_PROC_SHIFT));
        }
    }

    /*
     * Try the know locations.
     */
    else
    {
        rc = ERROR_PROB_SYM_FILE_NOT_FOUND;
        for (i = 0; apszSym[i] != NULL; i++)
        {
            int rc2 = krnlLoadKernelSymFile(apszSym[i], pKrnlOTE, cKernelObjects);
            if (!rc2)
            {
                rc2 = krnlVerifyImportTab();
                if (!rc || rc == ERROR_PROB_SYM_FILE_NOT_FOUND)
                {
                    strcpy(szSymbolFile, apszSym[i]);
                    rc = rc2;
                    if (!rc)
                        break;
                }
            }
        }
    }

    return rc;
}


