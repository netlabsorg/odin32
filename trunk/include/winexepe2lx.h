/* $Id: winexepe2lx.h,v 1.2 1999-10-14 01:39:13 bird Exp $ */

/*
 * Win32 PE2LX Exe class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WINEXEPE2LX_H__
#define __WINEXEPE2LX_H__

#include <winexebase.h>
#include <winimagepe2lx.h>


/**
 * Class for executables converted by pe2lx or win32k
 * @shortdesc   Class for executables converted by pe2lx or win32k
 * @author      Sander van Leeuwen, knut st. osmundsen
 * @approval    -
 */
class Win32Pe2LxExe : public Win32Pe2LxImage, public Win32ExeBase
{
public:
    /** @cat Constructor/Destructor */
    Win32Pe2LxExe(HINSTANCE hinstance, BOOL fWin32k) throw(ULONG);
    virtual ~Win32Pe2LxExe();
};


#endif //__WINEXEPE2LX_H__
