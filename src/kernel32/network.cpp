/* $Id: network.cpp,v 1.1 1999-05-24 20:19:47 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 Network apis
 *
 * Copyright 1998 Peter Fitzsimmons
 *
 */
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#include <os2.h>
#include <pmwsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "unicode.h"

//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerNameA(LPSTR name, LPDWORD size)
{
#if 1   /*PLF Wed  98-03-18 23:38:28*/
//*plf removing need for wsock32 call.
// This should be done with UPM anyway.
return FALSE;
#else
  if (-1==gethostname(name,*size))
    return FALSE;

  *size = strlen(name);
  return TRUE;
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerNameW(LPSTR name, LPDWORD size)
{
#if 1   /*PLF Wed  98-03-18 23:38:28*/
//*plf removing need for wsock32 call.
// This should be done with UPM anyway.
return FALSE;
#else
  if (-1==OS2gethostname(name,*size))
    return FALSE;
  @@@PH do unicode conversion

  *size = strlen(name);
  return TRUE;
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetComputerName32W(LPWSTR name, LPDWORD size)
{
 LPSTR nameA = (LPSTR)malloc(*size);
 BOOL  ret = GetComputerNameA(nameA,size);

    if (ret)
      AsciiToUnicode(nameA, name);

    free(nameA);
    /* FIXME : size correct? */
    return ret;
}
//******************************************************************************
//******************************************************************************

