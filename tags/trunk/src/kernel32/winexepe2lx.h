/* $Id: winexepe2lx.h,v 1.4 2001-07-08 02:49:47 bird Exp $ */

/*
 * Win32 PE2LX Exe class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
    Win32Pe2LxExe(HINSTANCE hinstance, BOOL fWin32k);
    virtual ~Win32Pe2LxExe();
    BOOL        init();
    static BOOL earlyInit();

public:
    static BOOL fEarlyInit;
};

#endif //__WINEXEPE2LX_H__
