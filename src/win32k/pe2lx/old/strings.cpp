/* $Id: strings.cpp,v 1.1 1999-10-06 18:50:56 bird Exp $ */

/*
 * PE2LX String conversion
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
#include "strings.h"


//******************************************************************************
//Save as RCDATA and make sure the kernel API's don't use the Open32 string
//retrieval stuff
//OS/2 strings are stored in a similar way as windows strings with the
//important difference that OS/2 strings can only be 256 bytes long
//whereas windows strings can be up to 64k bytes long
//******************************************************************************
BOOL ShowStrings(LXHeaderSuper &OS2Exe, int id, char *data, int size, int cp)
{
    USHORT *len;
    int i;
    BOOL rc = TRUE;
    ltassert((ULONG)data > MINPTR && (ULONG)data+size < MAXPTR)

    cout << "String Table resource with id " << id << ", size " << size << endl;

    len = (USHORT *)data;
    for (i=0;i<16 && rc;i++)
    {
        cout << "String " << (id-1)*16+i << " "<< *len << " " << UnicodeToAscii(*len, len+1, cp);
        rc = OS2Exe.StoreResource((id-1)*16+i, RT_RCDATA, (*len)*2 + 2, (char *)len);
        len += *len + 1;
    }
    return rc;
}
//******************************************************************************
//******************************************************************************
