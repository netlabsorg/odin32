/*
 * IPHLPAPI: the Vista era "netioapi" functions on top of GetIfTable/GetIpAddrTable.
 *
 * OpenJDK 21's java.net.NetworkInterface (net.dll) enumerates interfaces with
 * GetIfTable2, GetIfEntry2, GetUnicastIpAddressTable, GetAnycastIpAddressTable,
 * ConvertInterfaceLuidToNameW, ConvertInterfaceNameToLuidW, ConvertLengthToIpv4Mask
 * and FreeMibTable.  Only IPv4 interfaces exist here.  The interface LUID is the
 * interface index plus one (OS/2 numbers the interfaces from 0, Windows never uses 0).  The structures are laid out by hand with the offsets of the
 * 32 bit Windows SDK (MIB_IF_ROW2 is 1352 bytes, MIB_UNICASTIPADDRESS_ROW 80,
 * MIB_ANYCASTIPADDRESS_ROW 48, the table rows start at offset 8).
 */
#include <os2win.h>
#include <string.h>
#include <stdlib.h>
#include <iprtrmib.h>

extern "C" DWORD WINAPI GetIfTable(PMIB_IFTABLE pIfTable, PULONG pdwSize, BOOL bOrder);
extern "C" DWORD WINAPI GetIpAddrTable(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize, BOOL bOrder);

#define IFROW2_SIZE        1352
#define IFROW2_LUID        0
#define IFROW2_INDEX       8
#define IFROW2_DESCRIPTION 542     /* WCHAR[257] */
#define IFROW2_PHYSLEN     1056
#define IFROW2_PHYSADDR    1060    /* UCHAR[32] */
#define IFROW2_MTU         1124
#define IFROW2_TYPE        1128
#define IFROW2_ACCESSTYPE  1144
#define IFROW2_OPERSTATUS  1156
#define IFROW2_ADMINSTATUS 1160
#define IFROW2_MEDIACONN   1164
#define IFROW2_TXSPEED     1192
#define IFROW2_RXSPEED     1200

#define UNIROW_SIZE        80
#define UNIROW_ADDRESS     0       /* SOCKADDR_INET, 28 bytes */
#define UNIROW_LUID        32
#define UNIROW_INDEX       40
#define UNIROW_PREFIXORG   44
#define UNIROW_SUFFIXORG   48
#define UNIROW_VALIDLIFE   52
#define UNIROW_PREFLIFE    56
#define UNIROW_PREFIXLEN   60
#define UNIROW_DADSTATE    64
#define IpDadStatePreferred 4

#define TABLE_ROWS         8

static BYTE *getIfBuffer(PULONG pcb)
{
    ULONG   cb = 0;
    BYTE   *buf;
    DWORD   rc = GetIfTable(NULL, &cb, FALSE);
    if(rc != ERROR_BUFFER_OVERFLOW && rc != ERROR_INSUFFICIENT_BUFFER)
        return NULL;
    buf = (BYTE *)malloc(cb);
    if(buf == NULL)
        return NULL;
    if(GetIfTable((PMIB_IFTABLE)buf, &cb, FALSE) != NO_ERROR) {
        free(buf);
        return NULL;
    }
    *pcb = cb;
    return buf;
}

static void fillIfRow(BYTE *row, const MIB_IFROW *src)
{
    ULONG i, len;
    memset(row, 0, IFROW2_SIZE);
    *(ULONGLONG *)(row + IFROW2_LUID) = (ULONGLONG)src->dwIndex + 1;
    *(DWORD *)(row + IFROW2_INDEX)    = src->dwIndex + 1;
    len = src->dwDescrLen < 256 ? src->dwDescrLen : 256;
    for(i = 0; i < len; i++)
        ((WCHAR *)(row + IFROW2_DESCRIPTION))[i] = src->bDescr[i];
    len = src->dwPhysAddrLen < 32 ? src->dwPhysAddrLen : 32;
    *(DWORD *)(row + IFROW2_PHYSLEN) = len;
    memcpy(row + IFROW2_PHYSADDR, src->bPhysAddr, len);
    *(DWORD *)(row + IFROW2_MTU)     = src->dwMtu;
    *(DWORD *)(row + IFROW2_TYPE)    = src->dwType;
    /* NET_IF_ACCESS_LOOPBACK 1, NET_IF_ACCESS_BROADCAST 2 */
    *(DWORD *)(row + IFROW2_ACCESSTYPE) = (src->dwType == 24) ? 1 : 2;
    /* IfOperStatusUp 1, IfOperStatusDown 2; NET_IF_ADMIN_STATUS_UP 1, DOWN 2 */
    *(DWORD *)(row + IFROW2_OPERSTATUS) =
        (src->dwOperStatus == MIB_IF_OPER_STATUS_OPERATIONAL) ? 1 : 2;
    *(DWORD *)(row + IFROW2_ADMINSTATUS) =
        (src->dwAdminStatus == MIB_IF_ADMIN_STATUS_UP) ? 1 : 2;
    /* MediaConnectStateConnected 1, Disconnected 2 */
    *(DWORD *)(row + IFROW2_MEDIACONN) =
        (src->dwOperStatus == MIB_IF_OPER_STATUS_OPERATIONAL) ? 1 : 2;
    *(ULONGLONG *)(row + IFROW2_TXSPEED) = (ULONGLONG)src->dwSpeed;
    *(ULONGLONG *)(row + IFROW2_RXSPEED) = (ULONGLONG)src->dwSpeed;
}

extern "C" {

ULONG WINAPI GetIfTable2(PVOID *ppTable)
{
    ULONG cb = 0, i, n;
    BYTE *buf, *tab;
    PMIB_IFTABLE ift;

    if(ppTable == NULL)
        return ERROR_INVALID_PARAMETER;
    buf = getIfBuffer(&cb);
    if(buf == NULL)
        return ERROR_NO_DATA;
    ift = (PMIB_IFTABLE)buf;
    n = ift->dwNumEntries;
    tab = (BYTE *)calloc(1, TABLE_ROWS + (size_t)n * IFROW2_SIZE);
    if(tab == NULL) {
        free(buf);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    *(ULONG *)tab = n;
    for(i = 0; i < n; i++)
        fillIfRow(tab + TABLE_ROWS + (size_t)i * IFROW2_SIZE, &ift->table[i]);
    free(buf);
    *ppTable = tab;
    return NO_ERROR;
}

/* the row must hold InterfaceIndex or InterfaceLuid */
ULONG WINAPI GetIfEntry2(PVOID pRow)
{
    BYTE *row = (BYTE *)pRow, *buf;
    ULONG cb = 0, i, n;
    DWORD index;
    PMIB_IFTABLE ift;

    if(row == NULL)
        return ERROR_INVALID_PARAMETER;
    index = *(DWORD *)(row + IFROW2_INDEX);
    if(index == 0)
        index = (DWORD)*(ULONGLONG *)(row + IFROW2_LUID);
    if(index == 0)
        return ERROR_INVALID_PARAMETER;
    buf = getIfBuffer(&cb);
    if(buf == NULL)
        return ERROR_NO_DATA;
    ift = (PMIB_IFTABLE)buf;
    n = ift->dwNumEntries;
    for(i = 0; i < n; i++) {
        if(ift->table[i].dwIndex + 1 == index) {
            fillIfRow(row, &ift->table[i]);
            free(buf);
            return NO_ERROR;
        }
    }
    free(buf);
    return ERROR_FILE_NOT_FOUND;
}

ULONG WINAPI GetUnicastIpAddressTable(USHORT family, PVOID *ppTable)
{
    ULONG cb = 0, n, i;
    BYTE *tab;
    PMIB_IPADDRTABLE at;
    DWORD rc;

    if(ppTable == NULL)
        return ERROR_INVALID_PARAMETER;
    *ppTable = NULL;
    if(family == 23 /* AF_INET6 */) {
        tab = (BYTE *)calloc(1, TABLE_ROWS);
        if(tab == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;
        *ppTable = tab;
        return NO_ERROR;
    }
    rc = GetIpAddrTable(NULL, &cb, FALSE);
    if(rc != ERROR_BUFFER_OVERFLOW && rc != ERROR_INSUFFICIENT_BUFFER)
        cb = sizeof(MIB_IPADDRTABLE);
    at = (PMIB_IPADDRTABLE)calloc(1, cb < sizeof(MIB_IPADDRTABLE) ? sizeof(MIB_IPADDRTABLE) : cb);
    if(at == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    if(rc == ERROR_BUFFER_OVERFLOW || rc == ERROR_INSUFFICIENT_BUFFER) {
        if(GetIpAddrTable(at, &cb, FALSE) != NO_ERROR)
            at->dwNumEntries = 0;
    } else {
        at->dwNumEntries = 0;
    }
    n = at->dwNumEntries;
    tab = (BYTE *)calloc(1, TABLE_ROWS + (size_t)n * UNIROW_SIZE);
    if(tab == NULL) {
        free(at);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    *(ULONG *)tab = n;
    for(i = 0; i < n; i++) {
        BYTE *r = tab + TABLE_ROWS + (size_t)i * UNIROW_SIZE;
        ULONG mask = at->table[i].dwMask, len = 0;
        /* SOCKADDR_IN: family(2) port(2) addr(4); dwAddr is in network byte order */
        *(USHORT *)(r + UNIROW_ADDRESS)     = 2; /* AF_INET */
        *(DWORD *)(r + UNIROW_ADDRESS + 4)  = at->table[i].dwAddr;
        *(ULONGLONG *)(r + UNIROW_LUID)     = (ULONGLONG)at->table[i].dwIndex + 1;
        *(DWORD *)(r + UNIROW_INDEX)        = at->table[i].dwIndex + 1;
        *(DWORD *)(r + UNIROW_VALIDLIFE)    = 0xFFFFFFFF;
        *(DWORD *)(r + UNIROW_PREFLIFE)     = 0xFFFFFFFF;
        /* a netmask is contiguous: its prefix length is the number of one bits in any byte order */
        while(mask) { len += mask & 1; mask >>= 1; }
        r[UNIROW_PREFIXLEN]                 = (BYTE)len;
        *(DWORD *)(r + UNIROW_DADSTATE)     = IpDadStatePreferred;
    }
    free(at);
    *ppTable = tab;
    return NO_ERROR;
}

ULONG WINAPI GetAnycastIpAddressTable(USHORT family, PVOID *ppTable)
{
    BYTE *tab;
    if(ppTable == NULL)
        return ERROR_INVALID_PARAMETER;
    tab = (BYTE *)calloc(1, TABLE_ROWS);
    if(tab == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;
    *ppTable = tab;
    return NO_ERROR;
}

VOID WINAPI FreeMibTable(PVOID pTable)
{
    free(pTable);
}

ULONG WINAPI ConvertInterfaceLuidToNameW(const ULONGLONG *luid, LPWSTR name, ULONG cch)
{
    ULONG cb = 0, i, n, len;
    BYTE *buf;
    PMIB_IFTABLE ift;
    DWORD index;

    if(luid == NULL || name == NULL)
        return ERROR_INVALID_PARAMETER;
    index = (DWORD)*luid;
    buf = getIfBuffer(&cb);
    if(buf == NULL)
        return ERROR_NO_DATA;
    ift = (PMIB_IFTABLE)buf;
    n = ift->dwNumEntries;
    for(i = 0; i < n; i++) {
        if(ift->table[i].dwIndex + 1 == index) {
            len = lstrlenW(ift->table[i].wszName);
            if(len + 1 > cch) {
                free(buf);
                return ERROR_NOT_ENOUGH_MEMORY;
            }
            memcpy(name, ift->table[i].wszName, (len + 1) * sizeof(WCHAR));
            free(buf);
            return NO_ERROR;
        }
    }
    free(buf);
    return ERROR_FILE_NOT_FOUND;
}

ULONG WINAPI ConvertInterfaceNameToLuidW(LPCWSTR name, ULONGLONG *luid)
{
    ULONG cb = 0, i, n;
    BYTE *buf;
    PMIB_IFTABLE ift;

    if(name == NULL || luid == NULL)
        return ERROR_INVALID_PARAMETER;
    buf = getIfBuffer(&cb);
    if(buf == NULL)
        return ERROR_INVALID_NAME;
    ift = (PMIB_IFTABLE)buf;
    n = ift->dwNumEntries;
    for(i = 0; i < n; i++) {
        if(lstrcmpiW(ift->table[i].wszName, name) == 0) {
            *luid = (ULONGLONG)ift->table[i].dwIndex + 1;
            free(buf);
            return NO_ERROR;
        }
    }
    free(buf);
    return ERROR_INVALID_NAME;
}

ULONG WINAPI ConvertLengthToIpv4Mask(ULONG len, PULONG pmask)
{
    ULONG host;
    if(pmask == NULL || len > 32)
        return ERROR_INVALID_PARAMETER;
    host = len == 0 ? 0 : (0xFFFFFFFFUL << (32 - len));
    /* network byte order */
    *pmask = ((host & 0xFF) << 24) | ((host & 0xFF00) << 8) | ((host >> 8) & 0xFF00) | (host >> 24);
    return NO_ERROR;
}

} // extern "C"

/* Change notifications: interfaces and addresses do not change while a program runs here, so a
 * notification is registered (and can be cancelled) but never delivered. */
extern "C" {

DWORD WINAPI NotifyIpInterfaceChange(USHORT family, PVOID callback, PVOID context, BOOLEAN initialNotification,
                                     HANDLE *notificationHandle)
{
    if(!callback || !notificationHandle)
        return ERROR_INVALID_PARAMETER;
    *notificationHandle = (HANDLE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, 16);
    return *notificationHandle ? NO_ERROR : ERROR_NOT_ENOUGH_MEMORY;
}

DWORD WINAPI NotifyUnicastIpAddressChange(USHORT family, PVOID callback, PVOID context, BOOLEAN initialNotification,
                                          HANDLE *notificationHandle)
{
    return NotifyIpInterfaceChange(family, callback, context, initialNotification, notificationHandle);
}

DWORD WINAPI CancelMibChangeNotify2(HANDLE notificationHandle)
{
    if(!notificationHandle)
        return ERROR_INVALID_PARAMETER;
    GlobalFree((HGLOBAL)notificationHandle);
    return NO_ERROR;
}

DWORD WINAPI NotifyAddrChange(HANDLE *handle, LPOVERLAPPED overlapped)
{
    if(overlapped) {
        if(handle)
            *handle = NULL;
        return ERROR_IO_PENDING;       /* never completes */
    }
    for(;;)
        Sleep(60000);                   /* blocks forever, like an unchanged network */
    return NO_ERROR;
}

DWORD WINAPI NotifyRouteChange(HANDLE *handle, LPOVERLAPPED overlapped)
{
    return NotifyAddrChange(handle, overlapped);
}

BOOL WINAPI CancelIPChangeNotify(LPOVERLAPPED overlapped)
{
    return TRUE;
}

} // extern "C"
