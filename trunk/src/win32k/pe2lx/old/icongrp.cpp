/* $Id: icongrp.cpp,v 1.1 1999-10-06 18:50:52 bird Exp $ */

/*
 * PE2LX Icon group code
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
#include <pe2lx.h>
#include "icon.h"
#include "icongrp.h"


BOOL ShowGroupIcon(LXHeaderSuper &OS2Exe, int id, IconHeader *ihdr, int size)
{
    BOOL rc = FALSE;
    void *pConverted;
    ULONG ulSize;

    ltasserthp(ihdr);

    pConverted = ConvertGroupIcon(OS2Exe, id, ihdr, size, (PULONG)SSToDS(&ulSize));
    if (pConverted != NULL)
    {
        rc = OS2Exe.StoreResource(id, RT_POINTER, ulSize, (char*)pConverted);
        free(pConverted);
    }
    else
        cout << "ShowGroupIcon: convert failed" << endl;

    return rc;
}



void *ConvertGroupIcon(LXHeaderSuper &OS2Exe, int id, IconHeader *ihdr, int size, PULONG pulSize)
{
    ResourceDirectory *rdir = (ResourceDirectory *)(ihdr + 1);
    int i, groupsize = 0;
    BITMAPARRAYFILEHEADER *bafh, *orgbafh;
    OS2Icon *icon;
    int rc;

    ltassert(ihdr);
    cout << "Icon Group id   :" << id << endl;
    cout << "Icon Group type :" << ihdr->wType << endl;
    cout << "Icon Group count:" << ihdr->wCount << endl;
    for (i=0;i<ihdr->wCount;i++)
    {
        cout << "Icon    :" << rdir->wNameOrdinal << endl;
        cout << "Width   :" << (int)rdir->bWidth << endl;
        cout << "Height  :" << (int)rdir->bHeight << endl;
        cout << "Colors  :" << (int)rdir->bColorCount << endl;
        cout << "Bits    :" << rdir->wBitCount << endl;
        cout << "ResBytes:" << rdir->lBytesInRes << endl;
        icon       = OS2Icon::GetIcon(rdir->wNameOrdinal);
        if (icon)
            groupsize += icon->QueryIconSize();
        rdir++;
    }

    ////@KSO: since the iconsize includes all bytes from bafh->bfh let us not count it twice
    //bafh = (BITMAPARRAYFILEHEADER *)malloc(groupsize+ihdr->wCount*(sizeof(BITMAPARRAYFILEHEADER)-sizeof(BITMAPFILEHEADER)));

    bafh = (BITMAPARRAYFILEHEADER *)malloc(groupsize+ihdr->wCount*sizeof(BITMAPARRAYFILEHEADER));
    memset((void*)bafh, 0xaa, groupsize+ihdr->wCount*sizeof(BITMAPARRAYFILEHEADER));        //@KSO: this makes comparing converteded files easier

    ltassert(bafh != NULL);

    orgbafh = bafh;

    rdir = (ResourceDirectory *)(ihdr + 1);
    for (i=0;i<ihdr->wCount;i++)
    {
        bafh->usType    = BFT_BITMAPARRAY;
        bafh->cbSize    = sizeof(BITMAPARRAYFILEHEADER);
        bafh->cxDisplay = 0;
        bafh->cyDisplay = 0;
        icon            = OS2Icon::GetIcon(rdir->wNameOrdinal);
        if (icon == NULL)
        {
            cout << "Can't find icon!" << endl;
            rdir++;
            continue;
        }
        if (i < ihdr->wCount - 1)
            bafh->offNext = (int)&bafh->bfh - (int)orgbafh + icon->QueryIconSize();
        else
            bafh->offNext = 0;

        icon->SetIconHdrOffset((int)bafh - (int)orgbafh + sizeof(BITMAPARRAYFILEHEADER)-sizeof(BITMAPFILEHEADER));

        memcpy((char *)&bafh->bfh, (char *)icon->GetIconHeader(), icon->QueryIconSize());
        bafh = (BITMAPARRAYFILEHEADER *)((int)&bafh->bfh + icon->QueryIconSize());
        rdir++;
    }

    ////@KSO: since the iconsize includes all bytes from bafh->bfh let us not count it twice
    //*pulSize = groupsize+ihdr->wCount*(sizeof(BITMAPARRAYFILEHEADER)-sizeof(BITMAPFILEHEADER));

    *pulSize = groupsize+ihdr->wCount*(sizeof(BITMAPARRAYFILEHEADER));
    return (void*)orgbafh;
}



ULONG QuerySizeGroupIcon(IconHeader *ihdr, int size)
{
    ResourceDirectory *rdir = (ResourceDirectory *)(ihdr + 1);
    int i, groupsize = 0;
    OS2Icon *icon;
    int rc;

    ltassert(ihdr);
    for ( i = 0; i < ihdr->wCount; i++)
    {
        icon = OS2Icon::GetIcon(rdir->wNameOrdinal);
        if (icon)
            groupsize += icon->QueryIconSize();
        rdir++;
    }

    return groupsize+ihdr->wCount*(sizeof(BITMAPARRAYFILEHEADER));
}

