/* $Id: winfakepeldr.h,v 1.1 2000-03-10 16:12:02 sandervl Exp $ */

/*
 * Win32 PE loader fake image class (RSRC_LOAD)
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINFAKEPELDR_H__
#define __WINFAKEPELDR_H__

#include <winimagepeldr.h>

//Class for images that are loaded for only retrieving their resources
class Win32PeLdrRsrcImg : public Win32PeLdrImage
{
public:
	Win32PeLdrRsrcImg(char *szFileName);
virtual ~Win32PeLdrRsrcImg();

virtual	ULONG     getApi(char *name);
virtual ULONG     getApi(int ordinal);

virtual BOOL  isDll();

protected:
private:
};

#endif //__WINFAKEPELDR_H__
