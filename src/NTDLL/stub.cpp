/* $Id: stub.cpp,v 1.1 2002-05-16 12:16:48 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 * Win32 NT Runtime / NTDLL for OS/2
 */

#include <stdlib.h>
#include <string.h>

#include "ntdll.h"

//*****************************************************************************
//*****************************************************************************
NTSTATUS NtAllocateVirtualMemory(DWORD x1, DWORD x2, DWORD x3)
{

}
//*****************************************************************************
//*****************************************************************************
NTSTATUS NtQueryVirtualMemory(DWORD x1, DWORD x2, DWORD x3)
{
    dprintf(("NtQueryVirtualMemory NOT IMPLEMENTED"));
    return STATUS_SUCCESS;
}
//*****************************************************************************
//*****************************************************************************
NTSTATUS NtCreateProfile(DWORD x1)
{

}

NtStopProfile(DWORD x1)
{
}
