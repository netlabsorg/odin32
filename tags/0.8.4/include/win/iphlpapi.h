/* IPHLPAPI.H */

#ifndef IP_HLP_API_H
#define IP_HLP_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <iprtrmib.h>

DWORD WINAPI GetIpAddrTable(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize,
                              BOOL bOrder);

DWORD WINAPI GetIfTable(PMIB_IFTABLE pIfTable, PULONG pdwSize, BOOL bOrder);

DWORD WINAPI GetFriendlyIfIndex(DWORD IfIndex);

DWORD WINAPI GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);

DWORD WINAPI GetNetworkParams(PFIXED_INFO pFixedInfo, PULONG pOutBufLen);

#ifdef __cplusplus
}
#endif

#endif /* IP_HLP_API_H */
