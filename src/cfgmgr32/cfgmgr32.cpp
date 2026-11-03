/*
 * cfgmgr32 - device configuration manager stub (Mozilla Firefox asks for the
 * device ids of audio and graphics devices); OS/2 has none, so the calls fail.
 */
#include <os2win.h>

#define CR_FAILURE 0x13

extern "C" {

DWORD WINAPI CM_Get_Device_IDW(DWORD devInst, LPWSTR buffer, ULONG len, ULONG flags)
{
    return CR_FAILURE;
}

DWORD WINAPI CM_Get_Device_Interface_List_SizeW(PULONG len, PVOID guid, LPWSTR deviceId, ULONG flags)
{
    return CR_FAILURE;
}

} // extern "C"
