/* $Id: cursorgrp.cpp,v 1.1 1999-09-06 02:20:04 bird Exp $ */

/*
 * PE2LX cursor group code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "pe2lx.h"
#include "cursor.h"
#include "cursorgrp.h"



//******************************************************************************
//******************************************************************************
BOOL ShowGroupCursor(LXHeaderSuper &OS2Exe, int id, CursorHeader *chdr, int size)
{
    BITMAPARRAYFILEHEADER *pCursorgrp;
    int rc;
    ULONG ulSize;

    ltassert((ULONG)chdr > MINPTR && (ULONG)chdr + size < MAXPTR);

    pCursorgrp = (BITMAPARRAYFILEHEADER*)ConvertGroupCursor(chdr, size, (PULONG)SSToDS(&ulSize));
    ltasserthp(pCursorgrp);

    rc = OS2Exe.StoreResource(id, RT_POINTER, ulSize, (char *)pCursorgrp);
    free((void*)pCursorgrp);
    return rc;
}
//******************************************************************************
//******************************************************************************
void *ConvertGroupCursor(CursorHeader *chdr, int size, PULONG pulSize)
{
    CursorResDir           *rdir = (CursorResDir *)(chdr + 1);
    int                    rc;
    int                    i;
    ULONG                  groupsize = 0;
    BITMAPARRAYFILEHEADER *bafh;
    BITMAPARRAYFILEHEADER *orgbafh;
    OS2Cursor             *Cursor;

    ltassert((ULONG)chdr > MINPTR && (ULONG)chdr + size < MAXPTR);

    cout << "Cursor Group type :" << chdr->wType << endl;
    cout << "Cursor Group count:" << chdr->cwCount << endl;

    /* calc size of cursor group */
    for (i = 0; i < chdr->cwCount; i++)
    {
        cout << "Cursor id: " << rdir->wNameOrdinal << endl;
        cout << "Width:   " << (int)rdir->wWidth << endl;
        cout << "Height:  " << (int)rdir->wHeight << endl;
        cout << "Bits:    " << rdir->wBitCount << endl;
        cout << "ResBytes:" << rdir->lBytesInRes << endl;
        Cursor     = OS2Cursor::GetCursor(rdir->wNameOrdinal);
        if (Cursor == NULL)
        {
            cout << "Can't find Cursor!(1)" << endl;
            rdir++;
            continue; //KSO: or return FALSE;??
        }
        groupsize += Cursor->QueryCursorSize();
        rdir++;
    }
    *pulSize = groupsize+chdr->cwCount*sizeof(BITMAPARRAYFILEHEADER);
    orgbafh = bafh = (BITMAPARRAYFILEHEADER*)malloc(*pulSize);
    ltassert(bafh != NULL);

    /* copy the cursor bitmaps which this group consists of */
    rdir = (CursorResDir *)(chdr + 1);
    for (i=0; i < chdr->cwCount; i++)
    {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
        Cursor          = OS2Cursor::GetCursor(rdir->wNameOrdinal);
        if (Cursor == NULL)
        {
            cout << "Can't find Cursor!(2)" << endl;
            rdir++;
            continue;
        }
        if (i != chdr->cwCount -1)
            bafh->offNext = (int)&bafh->bfh - (int)orgbafh + Cursor->QueryCursorSize();
        else
            bafh->offNext   = 0;

        Cursor->SetCursorHdrOffset((int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER)-sizeof(BITMAPFILEHEADER));

        memcpy((char *)&bafh->bfh, (char *)Cursor->GetCursorHeader(rdir->wWidth, rdir->wHeight, rdir->wPlanes, rdir->wBitCount),
               Cursor->QueryCursorSize());
        bafh = (BITMAPARRAYFILEHEADER *)((int)&bafh->bfh + Cursor->QueryCursorSize());
        rdir++;
    }

    return(void*)orgbafh;
}


ULONG QuerySizeGroupCursor(CursorHeader *chdr, int size)
{
    CursorResDir           *rdir = (CursorResDir *)(chdr + 1);
    int                    i;
    int                    groupsize = 0;
    OS2Cursor             *Cursor;

    ltassert((ULONG)chdr > MINPTR && (ULONG)chdr + size < MAXPTR);

    /* calc size of cursor group */
    for (i = 0; i < chdr->cwCount; i++)
    {
        Cursor = OS2Cursor::GetCursor(rdir->wNameOrdinal);
        if (Cursor == NULL)
        {
            cout << "QuerySizeGroupCursor: Can't find Cursor!" << endl;
            rdir++;
            continue; //KSO: or return FALSE;??
        }
        groupsize += Cursor->QueryCursorSize();
        rdir++;
    }

    return groupsize+chdr->cwCount*sizeof(BITMAPARRAYFILEHEADER);
}
