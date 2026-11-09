/*
 * NTDLL functions imported by Mozilla Firefox 160+ (firefox.exe, mozglue.dll, xul.dll,
 * plugin-container.exe, the Microsoft C runtime).
 *
 *  - the C runtime block functions (memcpy, memmove, memset, memcmp): real ntdll
 *    exports them and Firefox's launcher links to them there.
 *  - NtWaitForKeyedEvent / NtReleaseKeyedEvent: real implementation on top of events
 *    (a waiter registers its key, a releaser wakes the waiter with the same key and
 *    waits for one to arrive, as the kernel does).
 *  - NtQueryVirtualMemory (MemoryBasicInformation) and NtReadVirtualMemory for the
 *    current process, NtUnmapViewOfSection, NtSetEventBoostPriority.
 *  - the rest report "not implemented" or do nothing.
 */
#include <windows.h>
#include <string.h>

typedef LONG NTSTATUS;

#define FF_STATUS_SUCCESS            ((NTSTATUS)0x00000000)
#define FF_STATUS_TIMEOUT            ((NTSTATUS)0x00000102)
#define FF_STATUS_NOT_IMPLEMENTED    ((NTSTATUS)0xC0000002)
#define FF_STATUS_INVALID_INFO_CLASS ((NTSTATUS)0xC0000003)
#define FF_STATUS_INVALID_PARAMETER  ((NTSTATUS)0xC000000D)
#define FF_STATUS_ACCESS_VIOLATION   ((NTSTATUS)0xC0000005)
#define FF_STATUS_INVALID_HANDLE     ((NTSTATUS)0xC0000008)
#define FF_STATUS_NO_MEMORY          ((NTSTATUS)0xC0000017)

typedef struct {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} FF_UNICODE_STRING;

/*****************************************************************************
 * C runtime block functions
 *****************************************************************************/
void * __cdecl FF_memcpy(void *d, const void *s, size_t n)
{
    return memcpy(d, s, n);
}

void * __cdecl FF_memmove(void *d, const void *s, size_t n)
{
    return memmove(d, s, n);
}

void * __cdecl FF_memset(void *d, int c, size_t n)
{
    return memset(d, c, n);
}

int __cdecl FF_memcmp(const void *a, const void *b, size_t n)
{
    return memcmp(a, b, n);
}

/*****************************************************************************
 * Keyed events
 *****************************************************************************/
#define FF_MAX_KEYWAITERS 128

typedef struct {
    PVOID  key;
    HANDLE event;
    int    used;
} FF_KEYWAITER;

static FF_KEYWAITER     keyWaiters[FF_MAX_KEYWAITERS];
static volatile LONG    keyLock = 0;

static void keyLockEnter(void)
{
    while(InterlockedExchange((LPLONG)&keyLock, 1))
        Sleep(0);
}

static void keyLockLeave(void)
{
    InterlockedExchange((LPLONG)&keyLock, 0);
}

static DWORD ffTimeoutMs(const LARGE_INTEGER *t)
{
    LONGLONG q;

    if(t == NULL)
        return INFINITE;
    q = ((LONGLONG)t->HighPart << 32) | t->LowPart;
    if(q < 0) {                                  /* relative, 100ns units */
        LONGLONG ms = (-q) / 10000;
        return ms > 0x7FFFFFFE ? 0x7FFFFFFE : (DWORD)ms;
    }
    return 0;
}

NTSTATUS WINAPI FF_NtWaitForKeyedEvent(HANDLE keyedEvent, PVOID key, BOOLEAN alertable,
                                       const LARGE_INTEGER *timeout)
{
    int    i, slot = -1;
    DWORD  rc;
    HANDLE ev = CreateEventA(NULL, FALSE, FALSE, NULL);

    if(ev == NULL)
        return FF_STATUS_NO_MEMORY;
    keyLockEnter();
    for(i = 0; i < FF_MAX_KEYWAITERS; i++) {
        if(!keyWaiters[i].used) {
            keyWaiters[i].used  = 1;
            keyWaiters[i].key   = key;
            keyWaiters[i].event = ev;
            slot = i;
            break;
        }
    }
    keyLockLeave();
    if(slot < 0) {
        CloseHandle(ev);
        return FF_STATUS_NO_MEMORY;
    }
    rc = WaitForSingleObject(ev, ffTimeoutMs(timeout));
    if(rc != WAIT_OBJECT_0) {
        keyLockEnter();
        if(keyWaiters[slot].used && keyWaiters[slot].event == ev) {
            keyWaiters[slot].used = 0;           /* timed out, nobody released us */
            keyLockLeave();
            CloseHandle(ev);
            return FF_STATUS_TIMEOUT;
        }
        keyLockLeave();                          /* released just now: slot freed by the releaser */
    }
    CloseHandle(ev);
    return FF_STATUS_SUCCESS;
}

NTSTATUS WINAPI FF_NtReleaseKeyedEvent(HANDLE keyedEvent, PVOID key, BOOLEAN alertable,
                                       const LARGE_INTEGER *timeout)
{
    DWORD start = GetTickCount(), limit = ffTimeoutMs(timeout);
    int   i;

    for(;;) {
        keyLockEnter();
        for(i = 0; i < FF_MAX_KEYWAITERS; i++) {
            if(keyWaiters[i].used && keyWaiters[i].key == key) {
                HANDLE ev = keyWaiters[i].event;
                keyWaiters[i].used = 0;
                keyLockLeave();
                SetEvent(ev);
                return FF_STATUS_SUCCESS;
            }
        }
        keyLockLeave();
        if(limit != INFINITE && GetTickCount() - start >= limit)
            return FF_STATUS_TIMEOUT;
        Sleep(1);
    }
}

/*****************************************************************************
 * Virtual memory of the current process
 *****************************************************************************/
static int ffIsSelf(HANDLE h)
{
    return h == (HANDLE)-1 || h == GetCurrentProcess() || h == NULL;
}

NTSTATUS WINAPI FF_NtQueryVirtualMemory(HANDLE process, PVOID address, ULONG infoClass,
                                        PVOID buffer, ULONG len, PULONG retLen)
{
    if(!ffIsSelf(process))
        return FF_STATUS_NOT_IMPLEMENTED;
    if(infoClass != 0 /* MemoryBasicInformation */)
        return FF_STATUS_INVALID_INFO_CLASS;
    if(len < sizeof(MEMORY_BASIC_INFORMATION))
        return FF_STATUS_INVALID_PARAMETER;
    if(VirtualQuery(address, (PMEMORY_BASIC_INFORMATION)buffer, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
        return FF_STATUS_INVALID_PARAMETER;
    if(retLen)
        *retLen = sizeof(MEMORY_BASIC_INFORMATION);
    return FF_STATUS_SUCCESS;
}

NTSTATUS WINAPI FF_NtReadVirtualMemory(HANDLE process, PVOID base, PVOID buffer, ULONG size, PULONG done)
{
    if(!ffIsSelf(process))
        return FF_STATUS_NOT_IMPLEMENTED;
    if(IsBadReadPtr(base, size))
        return FF_STATUS_ACCESS_VIOLATION;
    memcpy(buffer, base, size);
    if(done)
        *done = size;
    return FF_STATUS_SUCCESS;
}

NTSTATUS WINAPI FF_NtUnmapViewOfSection(HANDLE process, PVOID base)
{
    if(!ffIsSelf(process))
        return FF_STATUS_NOT_IMPLEMENTED;
    return UnmapViewOfFile(base) ? FF_STATUS_SUCCESS : FF_STATUS_INVALID_PARAMETER;
}

NTSTATUS WINAPI FF_NtSetEventBoostPriority(HANDLE event)
{
    return SetEvent(event) ? FF_STATUS_SUCCESS : FF_STATUS_INVALID_HANDLE;
}

NTSTATUS WINAPI FF_NtCancelIoFile(HANDLE file, PVOID ioStatus)
{
    if(ioStatus) {
        ((PDWORD)ioStatus)[0] = 0;
        ((PDWORD)ioStatus)[1] = 0;
    }
    return FF_STATUS_SUCCESS;
}

NTSTATUS WINAPI FF_NtRenameKey(HANDLE key, PVOID newName)
{
    return FF_STATUS_NOT_IMPLEMENTED;
}

/*****************************************************************************
 * Rtl
 *****************************************************************************/
VOID WINAPI FF_RtlCaptureContext(PCONTEXT ctx)
{
    if(ctx)
        memset(ctx, 0, sizeof(CONTEXT));
}

NTSTATUS WINAPI FF_RtlDuplicateUnicodeString(ULONG flags, const FF_UNICODE_STRING *src, FF_UNICODE_STRING *dst)
{
    if(src == NULL || dst == NULL)
        return FF_STATUS_INVALID_PARAMETER;
    if(src->Length == 0 && !(flags & 2)) {
        dst->Length = 0;
        dst->MaximumLength = 0;
        dst->Buffer = NULL;
        return FF_STATUS_SUCCESS;
    }
    dst->MaximumLength = src->Length + ((flags & 1) ? sizeof(WCHAR) : 0);
    dst->Buffer = (PWSTR)HeapAlloc(GetProcessHeap(), 0, dst->MaximumLength ? dst->MaximumLength : sizeof(WCHAR));
    if(dst->Buffer == NULL)
        return FF_STATUS_NO_MEMORY;
    memcpy(dst->Buffer, src->Buffer, src->Length);
    if(flags & 1)
        dst->Buffer[src->Length / sizeof(WCHAR)] = 0;
    dst->Length = src->Length;
    return FF_STATUS_SUCCESS;
}

typedef struct {
    ULONG dwOSVersionInfoSize, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId;
    WCHAR szCSDVersion[128];
} FF_OSVERSIONINFOW;

NTSTATUS WINAPI FF_RtlGetVersion(FF_OSVERSIONINFOW *vi)
{
    OSVERSIONINFOA a;

    if(vi == NULL)
        return FF_STATUS_INVALID_PARAMETER;
    memset(&a, 0, sizeof(a));
    a.dwOSVersionInfoSize = sizeof(a);
    GetVersionExA(&a);
    vi->dwMajorVersion = a.dwMajorVersion;
    vi->dwMinorVersion = a.dwMinorVersion;
    vi->dwBuildNumber  = a.dwBuildNumber;
    vi->dwPlatformId   = a.dwPlatformId;
    vi->szCSDVersion[0] = 0;
    return FF_STATUS_SUCCESS;
}

ULONG WINAPI FF_RtlGetLastWin32Error(void)
{
    return GetLastError();
}

VOID WINAPI FF_RtlSetLastWin32Error(ULONG err)
{
    SetLastError(err);
}

USHORT WINAPI FF_RtlCaptureStackBackTrace(ULONG skip, ULONG count, PVOID *frames, PULONG hash)
{
    if(hash)
        *hash = 0;
    return 0;
}

/*****************************************************************************
 * RtlImageDirectoryEntryToData: address (and size) of a data directory of a PE image
 *****************************************************************************/
PVOID WINAPI FF_RtlImageDirectoryEntryToData(PVOID base, BOOLEAN mappedAsImage, USHORT entry, PULONG size)
{
    PIMAGE_DOS_HEADER     dos = (PIMAGE_DOS_HEADER)((ULONG)base & ~1u);
    PIMAGE_NT_HEADERS     nt;
    PIMAGE_SECTION_HEADER sec;
    DWORD                 rva, i;

    if(dos == NULL || dos->e_magic != IMAGE_DOS_SIGNATURE)
        return NULL;
    nt = (PIMAGE_NT_HEADERS)((BYTE *)dos + dos->e_lfanew);
    if(nt->Signature != IMAGE_NT_SIGNATURE || entry >= nt->OptionalHeader.NumberOfRvaAndSizes)
        return NULL;
    rva = nt->OptionalHeader.DataDirectory[entry].VirtualAddress;
    if(rva == 0)
        return NULL;
    if(size)
        *size = nt->OptionalHeader.DataDirectory[entry].Size;
    if(mappedAsImage || rva < nt->OptionalHeader.SizeOfHeaders)
        return (BYTE *)dos + rva;
    sec = (PIMAGE_SECTION_HEADER)((BYTE *)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader);
    for(i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++) {
        if(rva >= sec->VirtualAddress && rva < sec->VirtualAddress + sec->SizeOfRawData)
            return (BYTE *)dos + sec->PointerToRawData + (rva - sec->VirtualAddress);
    }
    return NULL;
}


/* Keyed events are kept in one table that does not depend on the handle, so a keyed event handle
 * only has to be a valid, closeable handle. */
NTSTATUS WINAPI FF_NtCreateKeyedEvent(PHANDLE handle, ACCESS_MASK access, PVOID attributes, ULONG flags)
{
    if(!handle)
        return 0xC000000D;      /* STATUS_INVALID_PARAMETER */
    *handle = CreateEventA(NULL, TRUE, FALSE, NULL);
    return *handle ? 0 : 0xC0000017;    /* STATUS_NO_MEMORY */
}

NTSTATUS WINAPI FF_NtOpenKeyedEvent(PHANDLE handle, ACCESS_MASK access, PVOID attributes)
{
    return FF_NtCreateKeyedEvent(handle, access, attributes, 0);
}

/*****************************************************************************
 * Loader helpers used by Supermium's Win7 compatibility layer (pwrp_k32.dll)
 *****************************************************************************/
typedef struct {
    USHORT Length;
    USHORT MaximumLength;
    PSTR   Buffer;
} FF_ANSI_STRING;

NTSTATUS WINAPI FF_LdrLoadDll(LPCWSTR path, PULONG flags, FF_UNICODE_STRING *name, HMODULE *handle)
{
    WCHAR buf[MAX_PATH];
    ULONG n;
    HMODULE h;
    if(!name || !name->Buffer || !handle)
        return FF_STATUS_INVALID_PARAMETER;
    n = name->Length / sizeof(WCHAR);
    if(n >= MAX_PATH)
        return FF_STATUS_INVALID_PARAMETER;
    memcpy(buf, name->Buffer, n * sizeof(WCHAR));
    buf[n] = 0;
    h = LoadLibraryW(buf);
    if(!h) {
        *handle = NULL;
        return (NTSTATUS)0xC0000135;        /* STATUS_DLL_NOT_FOUND */
    }
    *handle = h;
    return FF_STATUS_SUCCESS;
}

NTSTATUS WINAPI FF_LdrGetProcedureAddress(HMODULE module, FF_ANSI_STRING *name, ULONG ordinal, PVOID *address)
{
    char buf[256];
    FARPROC p;
    if(!module || !address)
        return FF_STATUS_INVALID_PARAMETER;
    if(name && name->Buffer) {
        ULONG n = name->Length;
        if(n >= sizeof(buf))
            return FF_STATUS_INVALID_PARAMETER;
        memcpy(buf, name->Buffer, n);
        buf[n] = 0;
        p = GetProcAddress(module, buf);
    }
    else
        p = GetProcAddress(module, (LPCSTR)(ULONG_PTR)(ordinal & 0xFFFF));
    if(!p) {
        *address = NULL;
        return (NTSTATUS)0xC000007A;        /* STATUS_PROCEDURE_NOT_FOUND */
    }
    *address = (PVOID)p;
    return FF_STATUS_SUCCESS;
}

NTSTATUS WINAPI FF_NtAllocateVirtualMemory(HANDLE process, PVOID *base, ULONG zeroBits, PULONG size, ULONG type, ULONG protect)
{
    PVOID p;
    if(!base || !size)
        return FF_STATUS_INVALID_PARAMETER;
    if(process != (HANDLE)-1 && process != GetCurrentProcess())
        return FF_STATUS_NOT_IMPLEMENTED;
    p = VirtualAlloc(*base, *size, type, protect);
    if(!p)
        return FF_STATUS_NO_MEMORY;
    *base = p;
    return FF_STATUS_SUCCESS;
}

/* base address of the image that contains pc */
PVOID WINAPI FF_RtlPcToFileHeader(PVOID pc, PVOID *base)
{
    MEMORY_BASIC_INFORMATION mbi;
    PVOID b = NULL;
    if(VirtualQuery(pc, &mbi, sizeof(mbi)))
        b = mbi.AllocationBase;
    if(base)
        *base = b;
    return b;
}

VOID WINAPI FF_RtlGetCallersAddress(PVOID *caller, PVOID *callersCaller)
{
    if(caller) *caller = NULL;
    if(callersCaller) *callersCaller = NULL;
}

/* ntdll exports the C runtime's strstr (Supermium's compatibility layer resolves it by name) */
char * __cdecl FF_strstr(const char *s, const char *find)
{
    size_t n;
    if(!*find)
        return (char *)s;
    n = strlen(find);
    for(; *s; s++) {
        if(*s == *find && !strncmp(s, find, n))
            return (char *)s;
    }
    return NULL;
}
