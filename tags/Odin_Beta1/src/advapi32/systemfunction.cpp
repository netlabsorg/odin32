/* $Id: systemfunction.cpp,v 1.1 2001-05-19 11:13:05 sandervl Exp $ */
/*
 * undocumented SystemFunctionXXX functions
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>

ODINDEBUGCHANNEL(ADVAPI32-SYSFUNC)

//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction001(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction001 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction002(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction002 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction003(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction003 %x %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction004(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction004 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction005(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction005 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction006(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction006 %x %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction007(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction007 %x %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction008(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction008 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction009(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction009 %x %x %x", arg1, arg2, arg3));
    return SystemFunction008(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction010(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction010 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction011(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction011 %x %x %x", arg1, arg2, arg3));
    return SystemFunction010(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction012(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction012 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction013(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction013 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction014(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction014 %x %x %x", arg1, arg2, arg3));
    return SystemFunction012(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction015(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction015 %x %x %x", arg1, arg2, arg3));
    return SystemFunction013(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction016(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction016 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction017(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction017 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction018(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction018 %x %x %x", arg1, arg2, arg3));
    return SystemFunction016(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction019(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction019 %x %x %x", arg1, arg2, arg3));
    return SystemFunction017(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction020(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction020 %x %x %x", arg1, arg2, arg3));
    return SystemFunction012(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction021(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction021 %x %x %x", arg1, arg2, arg3));
    return SystemFunction013(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction022(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction022 %x %x %x", arg1, arg2, arg3));
    return SystemFunction020(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction023(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction023 %x %x %x", arg1, arg2, arg3));
    return SystemFunction021(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction024(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction024 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction025(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction025 %x %x %x", arg1, arg2, arg3));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction026(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction026 %x %x %x", arg1, arg2, arg3));
    return SystemFunction024(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction027(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction027 %x %x %x", arg1, arg2, arg3));
    return SystemFunction025(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction028(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction028 %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction029(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction029 %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//calls RtlCompareMemory
//******************************************************************************
DWORD WINAPI SystemFunction030(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction030 %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//looks the same as 030
//******************************************************************************
DWORD WINAPI SystemFunction031(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction031 %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction032(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction032 %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI SystemFunction033(DWORD arg1, DWORD arg2)
{
    dprintf(("NOT IMPLEMENTED: SystemFunction033 %x %x", arg1, arg2));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}
//******************************************************************************
//******************************************************************************
