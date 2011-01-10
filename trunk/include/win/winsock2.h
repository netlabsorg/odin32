/*
 * Winsock 2 definitions - used for ws2_32.dll
 *
 * FIXME: Still missing required Winsock 2 definitions.
 */

#ifndef __WINSOCK2API__
#define __WINSOCK2API__

#include <winsock.h>

/* proper 4-byte packing */
#include <pshpack4.h>

#define SO_GROUP_ID       0x2001
#define SO_GROUP_PRIORITY 0x2002
#define SO_MAX_MSG_SIZE   0x2003
#define SO_PROTOCOL_INFOA 0x2004
#define SO_PROTOCOL_INFOW 0x2005
#define SO_PROTOCOL_INFO WINELIB_NAME_AW(SO_PROTOCOL_INFO)

#define PVD_CONFIG            0x3001
#define SO_CONDITIONAL_ACCEPT 0x3002


/* option flags per socket */

#define FD_MAX_EVENTS                   10
#define FD_ALL_EVENTS                   ((1 << FD_MAX_EVENTS) - 1)
#define FD_READ_BIT                     0
#define FD_READ                         (1 << FD_READ_BIT)
#define FD_WRITE_BIT                    1
#define FD_WRITE                        (1 << FD_WRITE_BIT)
#define FD_OOB_BIT                      2
#define FD_OOB                          (1 << FD_OOB_BIT)
#define FD_ACCEPT_BIT                   3
#define FD_ACCEPT                       (1 << FD_ACCEPT_BIT)
#define FD_CONNECT_BIT	                4
#define FD_CONNECT                      (1 << FD_CONNECT_BIT)
#define FD_CLOSE_BIT	                5
#define FD_CLOSE                        (1 << FD_CLOSE_BIT)
#define FD_QOS_BIT                      6
#define FD_QOS                          (1 << FD_QOS_BIT)
#define FD_GROUP_QOS_BIT                7
#define FD_GROUP_QOS                    (1 << FD_GROUP_QOS_BIT)
#define FD_ROUTING_INTERFACE_CHANGE_BIT 8
#define FD_ROUTING_INTERFACE_CHANGE     (1 << FD_ROUTING_INTERFACE_CHANGE_BIT)
#define FD_ADDRESS_LIST_CHANGE_BIT      9
#define FD_ADDRESS_LIST_CHANGE          (1 << FD_ADDRESS_LIST_CHANGE_BIT)

/* Constants for LPCONDITIONPROC */
#define CF_ACCEPT                  0x0000
#define CF_REJECT                  0x0001
#define CF_DEFER                   0x0002

/* Constants for shutdown() */
#define SD_RECEIVE                 0x00
#define SD_SEND                    0x01
#define SD_BOTH                    0x02

/*
 * Constants for WSAIoctl()
 */
#define IOC_UNIX                      0x00000000
#define IOC_WS2                       0x08000000
#define IOC_PROTOCOL                  0x10000000
#define IOC_VENDOR                    0x18000000
#define _WSAIO(x,y)                   (IOC_VOID|(x)|(y))
#define _WSAIOR(x,y)                  (IOC_OUT|(x)|(y))
#define _WSAIOW(x,y)                  (IOC_IN|(x)|(y))
#define _WSAIORW(x,y)                 (IOC_INOUT|(x)|(y))
#define SIO_ASSOCIATE_HANDLE          _WSAIOW(IOC_WS2,1)
#define SIO_ENABLE_CIRCULAR_QUEUEING  _WSAIO(IOC_WS2,2)
#define SIO_FIND_ROUTE                _WSAIOR(IOC_WS2,3)
#define SIO_FLUSH                     _WSAIO(IOC_WS2,4)
#define SIO_GET_BROADCAST_ADDRESS     _WSAIOR(IOC_WS2,5)
#define SIO_GET_EXTENSION_FUNCTION_POINTER  _WSAIORW(IOC_WS2,6)
#define SIO_GET_QOS                   _WSAIORW(IOC_WS2,7)
#define SIO_GET_GROUP_QOS             _WSAIORW(IOC_WS2,8)
#define SIO_MULTIPOINT_LOOPBACK       _WSAIOW(IOC_WS2,9)
#define SIO_MULTICAST_SCOPE           _WSAIOW(IOC_WS2,10)
#define SIO_SET_QOS                   _WSAIOW(IOC_WS2,11)
#define SIO_SET_GROUP_QOS             _WSAIOW(IOC_WS2,12)
#define SIO_TRANSLATE_HANDLE          _WSAIORW(IOC_WS2,13)
#define SIO_ROUTING_INTERFACE_QUERY   _WSAIORW(IOC_WS2,20)
#define SIO_ROUTING_INTERFACE_CHANGE  _WSAIOW(IOC_WS2,21)
#define SIO_ADDRESS_LIST_QUERY        _WSAIOR(IOC_WS2,22)
#define SIO_ADDRESS_LIST_CHANGE       _WSAIO(IOC_WS2,23)
#define SIO_QUERY_TARGET_PNP_HANDLE   _WSAIOR(IOC_W32,24)
#define SIO_GET_INTERFACE_LIST        WS_IOR ('t', 127, u_long)

/* Unfortunately the sockaddr_in6 structure doesn't
   seem to be defined in a standard place, even across
   different Linux distributions.  Until IPv6 support settles
   down, let's do our own here so the sockaddr_gen
   union is the correct size.*/
#ifdef s6_addr
#undef s6_addr
#endif

struct ws_in_addr6
{
   unsigned char s6_addr[16];   /* IPv6 address */
};
struct ws_sockaddr_in6
{
   short   sin6_family;            /* AF_INET6 */
   u_short sin6_port;              /* Transport level port number */
   u_long  sin6_flowinfo;          /* IPv6 flow information */
   struct  ws_in_addr6 sin6_addr;  /* IPv6 address */
};

struct ws_in_addr
{
        union {
                struct { BYTE   s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { WORD   s_w1,s_w2; } S_un_w;
                UINT S_addr;
        } S_un;
#define ws_addr  S_un.S_addr		/* can be used for most tcp & ip code */
#define ws_host  S_un.S_un_b.s_b2	/* host on imp */
#define ws_net   S_un.S_un_b.s_b1	/* network */
#define ws_imp   S_un.S_un_w.s_w2	/* imp */
#define ws_impno S_un.S_un_b.s_b4	/* imp # */
#define ws_lh    S_un.S_un_b.s_b3	/* logical host */
};

struct ws_sockaddr_in
{
        SHORT		sin_family;
        WORD 		sin_port;
        struct ws_in_addr sin_addr;
        BYTE    	sin_zero[8];
};

typedef union sockaddr_gen
{
   struct sockaddr Address;
   struct ws_sockaddr_in  AddressIn;
   struct ws_sockaddr_in6 AddressIn6;
} sockaddr_gen;

/* ws_hostent, ws_protoent, ws_servent, ws_netent are 4-byte aligned here ! */

typedef struct ws_hostent32
{
        char    *h_name;        /* official name of host */
        char    **h_aliases;    /* alias list */
        short   h_addrtype;     /* host address type */
        short   h_length;       /* length of address */
        char    **h_addr_list;  /* list of addresses from name server */
} _ws_hostent32;

typedef struct ws_protoent32
{
        char    *p_name;        /* official protocol name */
        char    **p_aliases;    /* alias list */
        short   p_proto;        /* protocol # */
} _ws_protoent32;

typedef struct ws_servent32
{
        char    *s_name;        /* official service name */
        char    **s_aliases;    /* alias list */
        short   s_port;         /* port # */
        char    *s_proto;       /* protocol to use */
} _ws_servent32;

typedef struct ws_netent32
{
        char    *n_name;        /* official name of net */
        char    **n_aliases;    /* alias list */
        short   n_addrtype;     /* net address type */
        u_long  n_net;          /* network # */
} _ws_netent32;

/* Structure to keep interface specific information */
typedef struct _INTERFACE_INFO
{
   u_long        iiFlags;             /* Interface flags */
   sockaddr_gen  iiAddress;           /* Interface address */
   sockaddr_gen  iiBroadcastAddress;  /* Broadcast address */
   sockaddr_gen  iiNetmask;           /* Network mask */
} INTERFACE_INFO, * LPINTERFACE_INFO;

/* Possible flags for the  iiFlags - bitmask  */
#ifndef HAVE_NET_IF_H
#  define IFF_UP                0x00000001 /* Interface is up */
#  define IFF_BROADCAST         0x00000002 /* Broadcast is  supported */
#  define IFF_LOOPBACK          0x00000004 /* this is loopback interface */
#  define IFF_POINTTOPOINT      0x00000008 /* this is point-to-point interface */
#  define IFF_MULTICAST         0x00000010 /* multicast is supported */
#endif

#ifndef GUID_DEFINED
#include "guiddef.h"
#endif

#define MAX_PROTOCOL_CHAIN 7
#define BASE_PROTOCOL      1
#define LAYERED_PROTOCOL   0

typedef struct _WSAPROTOCOLCHAIN
{
    int ChainLen;                                 /* the length of the chain,     */
                                                  /* length = 0 means layered protocol, */
                                                  /* length = 1 means base protocol, */
                                                  /* length > 1 means protocol chain */
    DWORD ChainEntries[MAX_PROTOCOL_CHAIN];       /* a list of dwCatalogEntryIds */
} WSAPROTOCOLCHAIN, * LPWSAPROTOCOLCHAIN;
#define WSAPROTOCOL_LEN  255

typedef struct _WSAPROTOCOL_INFOA
{
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    CHAR   szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOA, * LPWSAPROTOCOL_INFOA;

typedef struct _WSAPROTOCOL_INFOW
{
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    WCHAR szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOW, * LPWSAPROTOCOL_INFOW;

typedef struct _WSANETWORKEVENTS
{
  long lNetworkEvents;
  int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

typedef struct _WSABUF
{
    ULONG len;
    CHAR* buf;
} WSABUF, *LPWSABUF;

typedef struct _OVERLAPPED *  LPWSAOVERLAPPED;
typedef HANDLE WSAEVENT;

#define WSA_IO_PENDING             (ERROR_IO_PENDING)
#define WSA_IO_INCOMPLETE          (ERROR_IO_INCOMPLETE)
#define WSA_INVALID_HANDLE         (ERROR_INVALID_HANDLE)
#define WSA_INVALID_PARAMETER      (ERROR_INVALID_PARAMETER)
#define WSA_NOT_ENOUGH_MEMORY      (ERROR_NOT_ENOUGH_MEMORY)
#define WSA_OPERATION_ABORTED      (ERROR_OPERATION_ABORTED)

#define WSA_INVALID_EVENT          ((WSAEVENT)NULL)
#define WSA_MAXIMUM_WAIT_EVENTS    (MAXIMUM_WAIT_OBJECTS)
#define WSA_WAIT_FAILED            ((DWORD)-1L)
#define WSA_WAIT_EVENT_0           (WAIT_OBJECT_0)
#define WSA_WAIT_IO_COMPLETION     (WAIT_IO_COMPLETION)
#define WSA_WAIT_TIMEOUT           (WAIT_TIMEOUT)
#define WSA_INFINITE               (INFINITE)

typedef unsigned int   GROUP;
#define SG_UNCONSTRAINED_GROUP   0x01
#define SG_CONSTRAINED_GROUP     0x02

#define WSAAPI  FAR PASCAL

/*
 * FLOWSPEC and SERVICETYPE should eventually move to qos.h
 */

typedef ULONG   SERVICETYPE;

typedef struct _FLOWSPEC {
       unsigned int      TokenRate;
       unsigned int      TokenBucketSize;
       unsigned int      PeakBandwidth;
       unsigned int      Latency;
       unsigned int      DelayVariation;
       SERVICETYPE       ServiceType;
       unsigned int      MaxSduSize;
       unsigned int      MinimumPolicedSize;
   } FLOWSPEC, *PFLOWSPEC, *LPFLOWSPEC;

typedef struct _QUALITYOFSERVICE {
        FLOWSPEC           SendingFlowspec;
        FLOWSPEC           ReceivingFlowspec;
        WSABUF             ProviderSpecific;
   } QOS, *LPQOS;

typedef int (* CALLBACK LPCONDITIONPROC)
(
    LPWSABUF lpCallerId,
    LPWSABUF lpCallerData,
    LPQOS lpSQOS,
    LPQOS lpGQOS,
    LPWSABUF lpCalleeId,
    LPWSABUF lpCalleeData,
    GROUP *g,
    DWORD dwCallbackData
);

typedef void (* CALLBACK LPWSAOVERLAPPED_COMPLETION_ROUTINE)
(
     DWORD dwError,
     DWORD cbTransferred,
     LPWSAOVERLAPPED lpOverlapped,
     DWORD dwFlags
);

#ifndef _tagBLOB_DEFINED
#define _tagBLOB_DEFINED
#define _BLOB_DEFINED
#define _LPBLOB_DEFINED
typedef struct _BLOB {
        ULONG   cbSize;
        BYTE   *pBlobData;
} BLOB, *LPBLOB;
#endif

#ifndef __CSADDR_DEFINED__
#define __CSADDR_DEFINED__

typedef struct _SOCKET_ADDRESS {
        LPSOCKADDR      lpSockaddr;
        INT             iSockaddrLength;
} SOCKET_ADDRESS, *PSOCKET_ADDRESS, *LPSOCKET_ADDRESS;

typedef struct _CSADDR_INFO {
        SOCKET_ADDRESS  LocalAddr;
        SOCKET_ADDRESS  RemoteAddr;
        INT             iSocketType;
        INT             iProtocol;
} CSADDR_INFO, *PCSADDR_INFO, *LPCSADDR_INFO;
#endif

/*socket address list */
typedef struct _SOCKET_ADDRESS_LIST {
        INT             iAddressCount;
        SOCKET_ADDRESS  Address[1];
} SOCKET_ADDRESS_LIST, *LPSOCKET_ADDRESS_LIST;

/*   addressfamily protocol pairs */
typedef struct _AFPROTOCOLS {
        INT     iAddressFamily;
        INT     iProtocol;
} AFPROTOCOLS, *PAFPROTOCOLS, *LPAFPROTOCOLS;

/* client query definitions */
typedef enum _WSAEcomparator {
        COMP_EQUAL = 0,
        COMP_NOTLESS
} WSAECOMPARATOR, *PWSAECOMPARATOR, *LPWSAECOMPARATOR;

typedef struct _WSAVersion {
        DWORD           dwVersion;
        WSAECOMPARATOR  ecHow;
} WSAVERSION, *PWSAVERSION, *LPWSAVERSION;


typedef struct _WSAQuerySetA {
        DWORD           dwSize;
        LPSTR           lpszServiceInstanceName;
        LPGUID          lpServiceClassId;
        LPWSAVERSION    lpVersion;
        LPSTR           lpszComment;
        DWORD           dwNameSpace;
        LPGUID          lpNSProviderId;
        LPSTR           lpszContext;
        DWORD           dwNumberOfProtocols;
        LPAFPROTOCOLS   lpafpProtocols;
        LPSTR           lpszQueryString;
        DWORD           dwNumberOfCsAddrs;
        LPCSADDR_INFO   lpcsaBuffer;
        DWORD           dwOutputFlags;
        LPBLOB          lpBlob;
} WSAQUERYSETA, *PWSAQUERYSETA, *LPWSAQUERYSETA;

typedef struct _WSAQuerySetW {
        DWORD           dwSize;
        LPWSTR          lpszServiceInstanceName;
        LPGUID          lpServiceClassId;
        LPWSAVERSION    lpVersion;
        LPWSTR          lpszComment;
        DWORD           dwNameSpace;
        LPGUID          lpNSProviderId;
        LPWSTR          lpszContext;
        DWORD           dwNumberOfProtocols;
        LPAFPROTOCOLS   lpafpProtocols;
        LPWSTR          lpszQueryString;
        DWORD           dwNumberOfCsAddrs;
        LPCSADDR_INFO   lpcsaBuffer;
        DWORD           dwOutputFlags;
        LPBLOB          lpBlob;
} WSAQUERYSETW, *PWSAQUERYSETW, *LPWSAQUERYSETW;

DECL_WINELIB_TYPE_AW(WSAQUERYSET)
DECL_WINELIB_TYPE_AW(PWSAQUERYSET)
DECL_WINELIB_TYPE_AW(LPWSAQUERYSET)

/* Function declarations */
int WINAPI WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject, LPWSANETWORKEVENTS lpNetworkEvents);
int WINAPI WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvents);
WSAEVENT WINAPI WSACreateEvent(void);
BOOL WINAPI WSACloseEvent(WSAEVENT event);
SOCKET WINAPI WSASocketA(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOA lpProtocolInfo,
                         GROUP g, DWORD dwFlags);
INT WINAPI WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                   LPDWORD lpNumberOfBytesSent, DWORD dwFlags,
                   LPWSAOVERLAPPED lpOverlapped,
                   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
INT WINAPI WSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                     LPDWORD lpNumberOfBytesSent, DWORD dwFlags,
                     const struct sockaddr *to, int tolen,
                     LPWSAOVERLAPPED lpOverlapped,
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI WSASendDisconnect(SOCKET s, LPWSABUF lpOutboundDisconnectData);
int WINAPI WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer,
                    DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer,
                    LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI WSARecv (SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                    LPDWORD lpNumberOfBytesReceived, LPDWORD lpFlags,
	                LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
INT WINAPI WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                       LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom,
                       LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped,
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
INT WINAPI WSALookupServiceBeginA( LPWSAQUERYSETA, DWORD, LPHANDLE);
INT WINAPI WSALookupServiceBeginW( LPWSAQUERYSETW, DWORD, LPHANDLE);
#define    WSALookupServiceBegin WINELIB_NAME_AW(WSALookupServiceBegin)

extern INT WINAPI ioctlsocket(SOCKET s, LONG cmd, ULONG *argp);

#include <poppack.h>

#endif
