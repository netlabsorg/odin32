#ifdef __WIN32OS2__
#define INCL_DOSPROCESS
#define INCL_DOSEXCEPTIONS
#define INCL_ERRORS
#define INCL_DOSSEMAPHORES
#include <os2wrap2.h>
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#ifdef __WIN32OS2__
#include <odinlx.h>
#include <excpt.h>
#endif

#ifdef __WIN32OS2__

void os2_PrintMemLayout (os2_PVOID pAddr)
{
    os2_APIRET arc;
    os2_ULONG sz, flags;
    os2_PVOID pBase, pCur;

    printf ("Layout of memory region containing %x:\n", pAddr);

    // find the base address
    sz = 0x1000;
    pBase = (os2_PVOID)(((os2_ULONG) pAddr) & ~0xFFF);

    do
    {
        arc = DosQueryMem (pBase, &sz, &flags);
        if (arc)
        {
            printf ("DosQueryMem(%x) failed with %d\n", pBase, arc);
            return;
        }
        if (flags & os2_PAG_BASE)
        {
            break;
        }

        pBase = (os2_PVOID) ((os2_ULONG) pBase - 0x1000);
    }
    while (1);

    pCur = pBase;
    do
    {
        sz = ~0;
        arc = DosQueryMem (pCur, &sz, &flags);
        if (arc)
        {
            printf ("DosQueryMem(%x) failed with %d\n", pCur, arc);
            return;
        }
        if (pCur != pBase && (flags & (os2_PAG_BASE | os2_PAG_FREE)))
        {
            break;
        }

        printf (" %08X-%08X (%08X): Flags %08X\n",
                pCur, (ULONG) pCur + sz - 1, sz, flags);

        pCur = (os2_PVOID) ((os2_ULONG) pCur + sz);
    }
    while (1);
}

#endif

SYSTEM_INFO sSysInfo;

void PrintMemLayout (LPVOID pAddr)
{
    MEMORY_BASIC_INFORMATION sMemInfo;
    if (VirtualQuery (pAddr, &sMemInfo, sizeof (sMemInfo)))
    {
        printf ("Layout of memory region containing %x:\n", pAddr);

        LPVOID pBase = sMemInfo.AllocationBase;
        DWORD dwSize = 0;

        LPVOID pStart = pBase;
        do
        {
            if (VirtualQuery (pStart, &sMemInfo, sizeof (sMemInfo)))
            {
                if (sMemInfo.AllocationBase != pBase)
                    break;
                printf (" %08X-%08X (%08X): Protect %08X, State %08X, Type %08X\n",
                        sMemInfo.BaseAddress, (DWORD)sMemInfo.BaseAddress + sMemInfo.RegionSize - 1,
                        sMemInfo.RegionSize, sMemInfo.Protect, sMemInfo.State, sMemInfo.Type);
                pStart = (LPVOID)((DWORD)sMemInfo.BaseAddress + sMemInfo.RegionSize);
                dwSize += sMemInfo.RegionSize;
            }
            else
            {
                printf ("VirtualQuery(%x) failed with %d\n", pStart, GetLastError());
                return;
            }
        }
        while (1);
    }
    else
    {
        printf ("VirtualQuery(%x) failed with %d\n", pAddr, GetLastError());
    }
}

void Alloc4KOnStack(int nTimes)
{
    char buf[4096 - 12];
    printf ("buf %08X\n", &buf);
    if (--nTimes)
        Alloc4KOnStack (nTimes);
}

DWORD ExceptionFilter (DWORD ExceptionCode)
{
    printf ("Exception %08X\n", ExceptionCode);

    if (ExceptionCode == EXCEPTION_STACK_OVERFLOW)
        return EXCEPTION_EXECUTE_HANDLER;
    if (ExceptionCode == EXCEPTION_GUARD_PAGE)
        return EXCEPTION_EXECUTE_HANDLER;
    return EXCEPTION_CONTINUE_SEARCH;
}

DWORD WINAPI ThreadProc (LPVOID lpParameter)
{
    DWORD dwStackSize = (DWORD) lpParameter;

#ifdef _MSC_VER
    DWORD dwStackTop = __readfsdword (0x04);
    DWORD dwStackBottom = __readfsdword (0x08);
#else
    DWORD dwStackTop;
    DWORD dwStackBottom;
    __asm__ ("movl %%fs:0x04, %0" : "=r" (dwStackTop));
    __asm__ ("movl %%fs:0x08, %0" : "=r" (dwStackBottom));
#endif

    printf ("Thread: dwStackTop %x, dwStackBottom %x\n", dwStackTop, dwStackBottom);

    ///////////////////////////////////////////////////

    printf ("\nTEST 1 (STACK)\n");

    DWORD dwStackBase = dwStackTop - dwStackSize;
    DWORD dwYellowZoneSize = sSysInfo.dwPageSize * 3;

#if 1
    if (VirtualAlloc ((LPVOID) dwStackBase, dwYellowZoneSize,
                      MEM_COMMIT, PAGE_READWRITE))
    {
        DWORD dwOldFlags;
        if (!VirtualProtect ((LPVOID) dwStackBase, dwYellowZoneSize,
                             PAGE_READWRITE | PAGE_GUARD, &dwOldFlags))
        {
            printf ("VirtualProtect(%x) failed with %d\n", dwStackBase,
                    GetLastError());
        }
    }
    else
    {
        printf ("VirtualAlloc(%x) failed with %d\n", dwStackBase,
                GetLastError());
    }

    PrintMemLayout (&dwStackTop);

    {
        __try
        {
            //Alloc4KOnStack (15);

            printf ("Writing to %08X...\n", dwStackBase + sSysInfo.dwPageSize * 3 - 4);
            *((LPDWORD)(dwStackBase + sSysInfo.dwPageSize * 3 - 4)) = 0;
        }
        __except (ExceptionFilter (GetExceptionCode()))
        {
        }
    }
#endif

    PrintMemLayout (&dwStackTop);

    ///////////////////////////////////////////////////

    printf ("\nTEST 2 (PAGE_GUARD)\n");

    LPVOID pNonStack = VirtualAlloc (NULL, sSysInfo.dwPageSize,
                                     MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD);
    if (!pNonStack)
    {
        printf ("VirtualAlloc(0) failed with %d\n", GetLastError());
        return 0;
    }

    PrintMemLayout (pNonStack);
    os2_PrintMemLayout (pNonStack);

    {
        __try
        {
            *((LPDWORD)(pNonStack)) = 0;
        }
        __except (ExceptionFilter (GetExceptionCode()))
        {
        }
    }

    PrintMemLayout (pNonStack);
    os2_PrintMemLayout (pNonStack);

    VirtualFree (pNonStack, 0, MEM_RELEASE);

    return 0;
}

int main()
{
#ifdef __WIN32OS2__
    EnableSEH();
#endif

    //setbuf (stdout, NULL);

    GetSystemInfo (&sSysInfo);

    printf ("This computer has page size %d.\n", sSysInfo.dwPageSize);

    DWORD dwStackSize = 64 * 1024;

    HANDLE hThread = CreateThread (NULL, dwStackSize, ThreadProc, (LPVOID) dwStackSize,
                                   STACK_SIZE_PARAM_IS_A_RESERVATION,
                                   NULL);
    if (hThread == NULL)
    {
        printf ("CreateThread failed with %d\n", GetLastError());
        //getc (stdin);
        return 1;
    }

    printf ("Waiting for thread...\n");
    DWORD rc = WaitForSingleObject (hThread, INFINITE);
    printf ("Wait returned %d\n", rc);

    //getc (stdin);
    return 0;
}
