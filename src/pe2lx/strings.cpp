/* $Id: strings.cpp,v 1.3 1999-06-10 17:08:55 phaller Exp $ */

/*
 * PE2LX String conversion
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "lx.h"
#include "strings.h"
#include "misc.h"

//******************************************************************************
//Save as RCDATA and make sure the kernel API's don't use the Open32 string
//retrieval stuff
//OS/2 strings are stored in a similar way as windows strings with the
//important difference that OS/2 strings can only be 256 bytes long
//whereas windows strings can be up to 64k bytes long
//******************************************************************************
void ShowStrings(int id, char *data, int size)
{
USHORT *len;
int i;

  cout << "String Table resource with id " << id << ", size " << size << endl;

  len = (USHORT *)data;
  for(i=0;i<16;i++) {
//      if(*len != 0) {//skip NULL strings
                cout << "String " << (id-1)*16+i << " " << UnicodeToAscii((*len), len+1) << endl;
                OS2Exe.StoreResource((id-1)*16+i, RT_RCDATA, (*len)*2 + 2, (char *)len);
//      }
        len += *len + 1;
  }
}
//******************************************************************************
//******************************************************************************
