/* $Id: myldrRead.cpp,v 1.1 1999-09-06 02:20:01 bird Exp $
 *
 * myldrRead - _ldrRead.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "pefile.h"
#include "lx.h"
#include "log.h"
#include "dev32.h"
#include "ldr.h"
#include "ldrCalls.h"



ULONG LDRCALL myldrRead(
                       SFN hFile,
                       ULONG ulOffset,
                       PVOID pBuffer,
                       ULONG ulFlags ,
                       ULONG ulBytesToRead,
                       PMTE pMTE
                       )
{
    ULONG   rc;
    ULONG   ulRead;
    int     i;
    PPENODE pNode;

    /* check if this is an PE-file */
    if (GetState(hFile) == HSTATE_PE)
    {
        pNode = getNodePtr(hFile);
        if (pNode != NULL)
        {
            /* I would love to have a pointer to the MTE */
            if (pNode->pMTE == NULL && pMTE != NULL)
                pNode->pMTE == pMTE;

            /*debug*/
            if (ulFlags != 0)
                kprintf(("_ldrRead: Warning ulFlags = 0x%x (!= 0)\n", ulFlags));
            rc = pNode->lxfile.read(ulOffset, pBuffer, ulBytesToRead, (PULONG)SSToDS(&ulRead), ulFlags);
            return rc;
        }
        else
            kprintf(("_ldrRead:  DON'T PANIC! - I can't get Node ptr!\n"));
    }


    rc = _ldrRead( hFile, ulOffset, pBuffer, ulFlags, ulBytesToRead, pMTE );

//  kprintf(("_ldrRead:  hF=%+04x off=%+08x pB=%+08x fl=%+08x cb=%+04x pMTE=%+08x rc=%d\n",hFile,ulOffset,pBuffer,ulFlags,ulBytesToRead,pMTE,rc));

    /* check if file is uncertain - try make it certain */
    if (GetState(hFile) == HSTATE_CHECK)
    {
        i = findUncertainEntry(hFile);
        if (!ahUncertain[i].fMZ && ulOffset == 0 && ulBytesToRead >= 0x40)
        {
            /* Stage 1: */
            if (((PUSHORT)pBuffer)[0] == IMAGE_DOS_SIGNATURE)
            {   /* MZ */
                ahUncertain[i].offsetNEHdr = *(PULONG)((ULONG)pBuffer+0x3c); /*Need more verification???*/
                ahUncertain[i].fMZ = 1;
                //kprintf(("_ldrRead:  Yeah! Detected MZ file - offset of NEHdr=%#x.\n", ahUncertain[i].offsetNEHdr));
            }
            else
            {   /* CERTAIN: not a PE-file */
                SetState(hFile, HSTATE_NOT_PE);
                freeUncertainEntry(hFile);
            }
        }
        else
        {
            if (ahUncertain[i].fMZ && ulOffset == ahUncertain[i].offsetNEHdr && ulBytesToRead == sizeof(struct e32_exe))
            {
                /* Stage 2: */
                if (LXFile::isPEFile((PIMAGE_SIG_FILE_HEADER)pBuffer))
                {
                    kprintf(("_ldrRead:  Yeah! Detected PE file.\n"));
                    if (!fQAppType)
                    {
                        pNode = allocateNode();
                        if (pNode != NULL)
                        {
                            /* setup node */
                            pNode->hFile = hFile;   //key - but yes it is duplicateded in lxfile
                            pNode->pMTE = pMTE;
                            insertNode(pNode);
                            SetState(hFile, HSTATE_PE);
                            pNode->lxfile.SetModuleName(ahUncertain[i].pszName);

                            /* initiate lxfile */
                            if (pNode->lxfile.init(hFile, (PIMAGE_SIG_FILE_HEADER)pBuffer, ahUncertain[i].offsetNEHdr))
                            {
                                /* init ok - do read */
                                ULONG read;
                                /*debug*/
                                if (ulFlags != 0)
                                    kprintf(("_ldrRead: Warning ulFlags = 0x%x (!= 0)\n", ulFlags));
                                rc = pNode->lxfile.read(ulOffset, pBuffer, ulBytesToRead, (PULONG)SSToDS(&read), ulFlags);
                            }
                            else
                            {
                                /* init failed - complain */
                                kprintf(("_ldrRead:  opps! lxfile.init() failed!\n"));
                            }
                        }
                        else
                        {
                            /* to not enter an unstable state - free all and mark as no pefile*/
                            SetState(hFile, HSTATE_NOT_PE);
                            kprintf(("_ldrRead:  oops! could not allocate a Node for this file - why?\n"));
                        }
                    }
                    else
                    {
                        kprintf(("_ldrRead: Building an QAppType-Header!\n"));
                        LXFile::buildQAppTypeHeader((struct e32_exe*)pBuffer);
                        SetState(hFile, HSTATE_NOT_PE);
                    }
                }
                else
                {
                    /* CERTAIN: not a pefile! */
                    SetState(hFile,HSTATE_NOT_PE);
                }
                freeUncertainEntry(hFile);
            }
        }
    }

    return rc;
}
