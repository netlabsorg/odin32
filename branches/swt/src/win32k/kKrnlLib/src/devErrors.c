/* $Id: devErrors.c,v 1.2 2002-12-16 01:53:55 bird Exp $
 *
 * Error message from rc.
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
*   Header Files                                                               *
*******************************************************************************/
#include "devErrors.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * Error Messages.
 */
static struct
{
    short       sErr;
    const char *pszMsg;
} aErrorMsgs[] =
{
    {ERROR_PROB_KRNL_OPEN_FAILED,       "(Krnl) Failed to open kernel file."},
    {ERROR_PROB_KRNL_SEEK_SIZE,         "(Krnl) Failed to seek to end to of file."},
    {ERROR_PROB_KRNL_SEEK_FIRST,        "(Krnl) Failed to start of file."},
    {ERROR_PROB_KRNL_READ_FIRST,        "(Krnl) Failed to read (first)."},
    {ERROR_PROB_KRNL_READ_NEXT,         "(Krnl) Failed to read."},
    {ERROR_PROB_KRNL_TAG_NOT_FOUND,     "(Krnl) Build level tag was not found."},
    {ERROR_PROB_KRNL_INV_SIGANTURE,     "(Krnl) Invalid build level signature."},
    {ERROR_PROB_KRNL_INV_BUILD_NBR,     "(Krnl) Invalid build level number."},
    {ERROR_PROB_KRNL_BUILD_VERSION,     "(Krnl) Invalid build level version."},
    {ERROR_PROB_KRNL_MZ_SEEK,           "(Krnl) Failed to seek to start of file. (MZ)"},
    {ERROR_PROB_KRNL_MZ_READ,           "(Krnl) Failed to read MZ header."},
    {ERROR_PROB_KRNL_NEOFF_INVALID,     "(Krnl) Invalid new-header offset in MZ header."},
    {ERROR_PROB_KRNL_NEOFF_SEEK,        "(Krnl) Failed to seek to new-header offset."},
    {ERROR_PROB_KRNL_LX_READ,           "(Krnl) Failed to read LX header."},
    {ERROR_PROB_KRNL_LX_SIGNATURE,      "(Krnl) Invalid LX header signature."},
    {ERROR_PROB_KRNL_OBJECT_CNT,        "(Krnl) Object count don't match the running kernel."},
    {ERROR_PROB_KRNL_OBJECT_CNR_10,     "(Krnl) Less than 10 objects - not a valid kernel file!"},
    {ERROR_PROB_KRNL_OTE_SEEK,          "(Krnl) Failed to seek to OTEs."},
    {ERROR_PROB_KRNL_OTE_READ,          "(Krnl) Failed to read OTEs."},
    {ERROR_PROB_KRNL_OTE_SIZE_MIS,      "(Krnl) Size of a OTE didn't match the running kernel."},

    /*
     * ProbeSymFile error messages + some extra ones.
     */
    {ERROR_D16_THUNKING_FAILED,         "(Init) Request packed thunking failed."},
    {ERROR_D16_OPEN_DEV_FAILED,         "(Init) Failed to open helper driver ($KrnlHlp)."},
    {ERROR_D16_IOCTL_FAILED,            "(Init) IOCtl call to helper driver ($KrnlHlp) failed."},

    {ERROR_PROB_SYM_FILE_NOT_FOUND,     "(Sym) Symbol file was not found."},
    {ERROR_PROB_SYM_READERROR,          "(Sym) Read failed."},
    {ERROR_PROB_SYM_INVALID_MOD_NAME,   "(Sym) Invalid module name (not OS2KRNL)."},
    {ERROR_PROB_SYM_SEGS_NE_OBJS,       "(Sym) Number of segments don't match the object count of the kernel."},
    {ERROR_PROB_SYM_SEG_DEF_SEEK,       "(Sym) Failed to seek to a segment definition."},
    {ERROR_PROB_SYM_SEG_DEF_READ,       "(Sym) Failed to read a segment definition."},
    {ERROR_PROB_SYM_IMPORTS_NOTFOUND,   "(Sym) Some of the imports wasn't found."},

    {ERROR_D32_GETOS2KRNL_FAILED   ,    "(KrnlInfo) Failed to get the kernel MTE."},
    {ERROR_D32_NO_SWAPMTE          ,    "(KrnlInfo) No swap MTE pointer in kernel MTE."},
    {ERROR_D32_TOO_MANY_OBJECTS    ,    "(KrnlInfo) Too many kernel objects."},
    {ERROR_D32_NO_OBJECT_TABLE     ,    "(KrnlInfo) No object table pointer.."},
    {ERROR_D32_BUILD_INFO_NOT_FOUND,    "(KrnlInfo) Didn't find build info."},
    {ERROR_D32_INVALID_BUILD       ,    "(KrnlInfo) Invalid build info."},

    {ERROR_D32_VERIFY_FAILED  ,         "(InitR0) Reverify of kernel entrypoints failed."},
    {ERROR_D32_IPE            ,         "(InitR0) Internal Processing Error."},
    {ERROR_D32_HEAPINIT_FAILED,         "(InitR0) Heap Init failed."},
    {ERROR_D32_LDR_INIT_FAILED,         "(InitR0) Loader init failed."},

    {ERROR_D32_PROC_NOT_FOUND     ,     "(Verify32) Procedure not found."},
    {ERROR_D32_INVALID_OBJ_OR_ADDR,     "(Verify32) Invalid object or address."},
    {ERROR_D32_INVALID_ADDRESS    ,     "(Verify32) Invalid address."},
    {ERROR_D32_TOO_INVALID_PROLOG ,     "(Verify32) Invalid procedure prolog."},
    {ERROR_D32_NOT_IMPLEMENTED    ,     "(Verify32) Entrytype is not implemented."},

    {ERROR_D32_SYMDB_NOT_FOUND,         "(SymDB32) Not found."},
};


/**
 * Get the message text for an error message.
 * @returns Pointer to error text. NULL if not found.
 * @param   sErr  Error code id.
 * @status  completely implemented.
 */
const char *devGetErrorMsg(int sErr)
{
    int i;
    for (i = 0; i < sizeof(aErrorMsgs) / sizeof(aErrorMsgs[0]); i++)
    {
        if (aErrorMsgs[i].sErr == sErr)
            return aErrorMsgs[i].pszMsg;
    }
    return (const char *)0;
}

