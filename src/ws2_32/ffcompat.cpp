/*
 * WS2_32: functions imported by Mozilla Firefox 160+ that Odin lacked.
 * WSAStringToAddressA converts an IPv4 address text; the Winsock service provider
 * calls (WSCEnumProtocols, WSCGetProviderPath) are not supported.
 */
#include <odin.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

#define FF_WSAEINVAL        10022
#define FF_WSAEAFNOSUPPORT  10047

extern "C" {

INT WINAPI WSAStringToAddressA(LPSTR str, INT family, PVOID protoInfo, PVOID addr, LPINT len)
{
    BYTE  ip[4];
    char  buf[64], *p, *e;
    int   i;
    WORD  port = 0;
    BYTE *sa = (BYTE *)addr;

    if(str == NULL || addr == NULL || len == NULL) {
        WSASetLastError(FF_WSAEINVAL);
        return -1;
    }
    if(family != 2 /* AF_INET */) {
        WSASetLastError(FF_WSAEAFNOSUPPORT);
        return -1;
    }
    if(*len < 16) {
        *len = 16;
        WSASetLastError(FF_WSAEINVAL);
        return -1;
    }
    strncpy(buf, str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    e = strchr(buf, ':');
    if(e) {
        *e++ = 0;
        port = (WORD)atoi(e);
    }
    p = buf;
    for(i = 0; i < 4; i++) {
        long v = strtol(p, &e, 10);
        if(e == p || v < 0 || v > 255 || (i < 3 && *e != '.') || (i == 3 && *e != 0)) {
            WSASetLastError(FF_WSAEINVAL);
            return -1;
        }
        ip[i] = (BYTE)v;
        p = e + 1;
    }
    memset(sa, 0, 16);
    sa[0] = 2;                      /* sin_family = AF_INET */
    sa[2] = (BYTE)(port >> 8);      /* network order port */
    sa[3] = (BYTE)(port & 0xFF);
    memcpy(sa + 4, ip, 4);
    *len = 16;
    return 0;
}

INT WINAPI WSCEnumProtocols(LPINT protocols, PVOID buffer, LPDWORD len, LPINT error)
{
    if(error)
        *error = FF_WSAEINVAL;
    return -1;
}

INT WINAPI WSCGetProviderPath(PVOID providerId, LPWSTR path, LPINT pathLen, LPINT error)
{
    if(error)
        *error = FF_WSAEINVAL;
    return -1;
}

} // extern "C"
