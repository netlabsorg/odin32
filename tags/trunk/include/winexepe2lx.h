/* $Id: winexepe2lx.h,v 1.1 1999-09-15 23:29:37 sandervl Exp $ */

/*
 * Win32 PE2LX Exe class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINEXEPE2LX_H__
#define __WINEXEPE2LX_H__

#include <winexebase.h>
#include <winimagepe2lx.h>

//Class for executables converted by pe2lx or win32k
class Win32Pe2LxExe : public Win32Pe2LxImage, public Win32ExeBase
{
public:
        Win32Pe2LxExe(HINSTANCE hinstance, int NameTableId, int Win32TableId);
virtual ~Win32Pe2LxExe();

protected:
private:
};

#endif //__WINEXEPE2LX_H__
