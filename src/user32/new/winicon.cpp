/* $Id: winicon.cpp,v 1.1 1999-07-25 17:47:25 sandervl Exp $ */
/*
 * Win32 Icon Code for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include "icon.h"

//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIcon( HINSTANCE arg1, INT arg2, INT arg3, BYTE arg4, BYTE arg5, LPCVOID arg6, LPCVOID arg7)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIcon\n");
#endif
    return O32_CreateIcon(arg1, arg2, arg3, arg4, arg5, (const BYTE *)arg6, (const BYTE *)arg7);
}
//******************************************************************************
//ASSERT dwVer == win31 (ok according to SDK docs)
//******************************************************************************
HICON WIN32API CreateIconFromResource(PBYTE presbits,  UINT dwResSize,
                                      BOOL  fIcon,     DWORD dwVer)
{
 HICON hicon;
 DWORD OS2ResSize = 0;
 PBYTE OS2Icon    = ConvertWin32Icon(presbits, dwResSize, &OS2ResSize);

    hicon = O32_CreateIconFromResource(OS2Icon, OS2ResSize, fIcon, dwVer);
#ifdef DEBUG
    WriteLog("USER32:  CreateIconFromResource returned %X (%X)\n", hicon, GetLastError());
#endif
    if(OS2Icon)
        FreeIcon(OS2Icon);

    return(hicon);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconFromResourceEx(PBYTE presbits,  UINT dwResSize,
                                        BOOL  fIcon,     DWORD dwVer,
                                        int   cxDesired, int cyDesired,
                                        UINT  Flags)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIconFromResourceEx %X %d %d %X %d %d %X, not completely supported!\n", presbits, dwResSize, fIcon, dwVer, cxDesired, cyDesired, Flags);
#endif
    return CreateIconFromResource(presbits, dwResSize, fIcon, dwVer);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CreateIconIndirect(LPICONINFO arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateIconIndirect\n");
#endif
    return O32_CreateIconIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyIcon( HICON arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyIcon\n");
#endif
    return O32_DestroyIcon(arg1);
}
//******************************************************************************
//******************************************************************************
HICON WIN32API CopyIcon( HICON arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  CopyIcon\n");
#endif
    return O32_CopyIcon(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetIconInfo( HICON arg1, LPICONINFO  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetIconInfo\n");
#endif
    return O32_GetIconInfo(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
