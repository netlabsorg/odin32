/* $Id: icmpapi.h,v 1.1 2000-05-19 18:44:44 jeroen Exp $ */

/* ICMP API header file */
#ifndef _ICMPAPI_INCLUDED_H
#define _ICMPAPI_INCLUDED_H

#ifdef __cplusplus
extern "C" {
#endif

HANDLE WINAPI IcmpCreateFile(VOID);

BOOL   WINAPI IcmpCloseHandle(HANDLE IcmpHandle);

DWORD WINAPI IcmpSendEcho(HANDLE IcmpHandle,
                          IPAddr DestAddress,
                          LPVOID RequestData,
                          WORD RequestSize,
                          PIP_OPTION_INFORMATION RequestOptions,
                          LPVOID ReplyBuffer,
                          DWORD ReplySize,
                          DWORD Timeout);

#ifdef __cplusplus
}
#endif

#endif                                 /* _ICMP_INCLUDED_H                 */

