/* 
 * OLEAUT32 
 * 
 * Copyright 1999 Sander van Leeuwen (OS/2 Port 990815)
 *
 * Based on Wine code: (ole\compobj.c)
 * 
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1998	Justin Bradford
 *      Copyright 1999  Francis Beaudet
 *  	Copyright 1999  Sylvain St-Germain
 *
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 * TODO: WINE_StringFromCLSID should be imported from ole32.dll
 * TODO: OaBuildVersion has to be changed (as well as GetVersion in kernel32)
 */

#include "oleaut32.h"
#ifdef DEBUG
#define DEBUG_RUNTIME
#endif

#include <debugdefs.h>


/******************************************************************************
 *		WINE_StringFromCLSID	[Internal]
 * Converts a GUID into the respective string representation.
 *
 * NOTES
 *
 * RETURNS
 *	the string representation and OLESTATUS
 */
HRESULT WINE_StringFromCLSID(
	const CLSID *id,	/* [in] GUID to be converted */
	LPSTR idstr		/* [out] pointer to buffer to contain converted guid */
) {
  static const char *hex = "0123456789ABCDEF";
  char *s;
  int	i;

  if (!id)
	{ dprintf(("called with id=Null\n"));
	  *idstr = 0x00;
	  return E_FAIL;
	}
	
  sprintf(idstr, "{%08lX-%04X-%04X-%02X%02X-",
	  id->Data1, id->Data2, id->Data3,
	  id->Data4[0], id->Data4[1]);
  s = &idstr[25];

  /* 6 hex bytes */
  for (i = 2; i < 8; i++) {
    *s++ = hex[id->Data4[i]>>4];
    *s++ = hex[id->Data4[i] & 0xf];
  }

  *s++ = '}';
  *s++ = '\0';

  dprintf(("%p->%s\n", id, idstr));

  return OLE_OK;
}


/***********************************************************************
 *           OaBuildVersion           [OLEAUT32.170]
 */
UINT WINAPI OaBuildVersion()
{
#if 1
    return 0x141016;
#else
    WINDOWS_VERSION ver = VERSION_GetVersion();

    FIXME("Please report to a.mohr@mailto.de if you get version error messages !\n");
    switch(VersionData[ver].getVersion32)
    {
        case 0x80000a03: /* Win 3.1 */
		return 0x140fd1; /* from Win32s 1.1e */
        case 0xc0000004: /* Win 95 */
	 case 0xc0000a04: /* Win 98: verified same as Win95 */
		return 0x1e10a9; /* some older version: 0x0a0bd3 */
        case 0x04213303: /* NT 3.51 */
		FIXME("NT 3.51 version value unknown !\n");
		return 0x1e10a9; /* value borrowed from Win95 */
        case 0x05650004: /* NT 4.0 */
		return 0x141016;
	default:
		return 0x0;
    }
#endif
}
