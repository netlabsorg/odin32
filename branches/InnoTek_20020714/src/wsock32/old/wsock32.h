/* $Id: wsock32.h,v 1.1 1999-12-07 20:19:59 achimha Exp $ */

/* WSOCK32.H--definitions & conversions for Odin's wsock32.dll.
 * Unused/unneeded Microsoft declarations removed.
 *
 * This header file corresponds to version 1.1 of the Windows Sockets specification.
 *
 * This file includes parts which are Copyright (c) 1982-1986 Regents
 * of the University of California.  All rights reserved.  The
 * Berkeley Software License Agreement specifies the terms and
 * conditions for redistribution.
 *
 */

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_


#ifdef MAKELONG
#  undef MAKELONG
#endif

#ifdef MAKEWORD
#  undef MAKEWORD
#endif

#ifdef LOBYTE
#  undef LOBYTE
#endif

#ifdef LOWORD
#  undef LOWORD
#endif

#ifdef HIBYTE
#  undef HIBYTE
#endif

#ifdef HIWORD
#  undef HIWORD
#endif

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))



/*
 * Basic system type definitions, taken from the BSD file sys/types.h.
 */

#ifdef __WATCOM__
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
  typedef int ( WIN32API *PROC)();
#else
  typedef int (*  WIN32API PROC)();
#endif

//typedef unsigned short  WORD;
//typedef unsigned long   DWORD;
//typedef ULONG HANDLE, *PHANDLE, *LPHANDLE, *SPHANDLE;

/*
 * The new type to be used in all
 * instances which refer to sockets.
 */
typedef u_int           SOCKET;



/*
 * Select uses arrays of SOCKETs.  These macros manipulate such
 * arrays.  FD_SETSIZE may be defined by the user before including
 * this file, but the default here should be >= 64.
 *
 * CAVEAT IMPLEMENTOR and USER: THESE MACROS AND TYPES MUST BE
 * INCLUDED IN WINSOCK.H EXACTLY AS SHOWN HERE.
 */
#ifndef FD_SETSIZE
#define FD_SETSIZE      64
#endif /* FD_SETSIZE */

typedef struct Wfd_set {
        u_int   fd_count;               /* how many are SET? */
        SOCKET  fd_array[FD_SETSIZE];   /* an array of SOCKETs */
} Wfd_set;



#ifdef __cplusplus
extern "C" {
#endif

extern int PASCAL  __WSAFDIsSet(SOCKET, Wfd_set  *);

#ifdef __cplusplus
}
#endif

/*
#define FD_CLR(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((Wfd_set  *)(set))->fd_count ; __i++) { \
        if (((Wfd_set  *)(set))->fd_array[__i] == fd) { \
            while (__i < ((Wfd_set  *)(set))->fd_count-1) { \
                ((Wfd_set  *)(set))->fd_array[__i] = \
                    ((Wfd_set  *)(set))->fd_array[__i+1]; \
                __i++; \
            } \
            ((Wfd_set  *)(set))->fd_count--; \
            break; \
        } \
    } \
} while(0)

#define FD_SET(fd, set) do { \
    if (((Wfd_set  *)(set))->fd_count < FD_SETSIZE) \
        ((Wfd_set  *)(set))->fd_array[((Wfd_set  *)(set))->fd_count++]=(fd);\
} while(0)

#define FD_ZERO(set) (((Wfd_set  *)(set))->fd_count=0)

#define FD_ISSET(fd, set) __WSAFDIsSet((SOCKET)(fd), (Wfd_set  *)(set))
*/

/*
 * Structure used in select() call, taken from the BSD file sys/time.h.
 */
struct Wtimeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define Wtimercmp(tvp, uvp, cmp) \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0

/*
 * Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
 *
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#define IOCPARM_MASK    0x7f            /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000      /* no parameters */
#define IOC_OUT         0x40000000      /* copy out parameters */
#define IOC_IN          0x80000000      /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define WFIONREAD    _IOR('f', 127, u_long) /* get # bytes to read */
#define WFIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
#define WFIOASYNC    _IOW('f', 125, u_long) /* set/clear async i/o */

/* Socket I/O Controls */
#define WSIOCSHIWAT  _IOW('s',  0, u_long)  /* set high watermark */
#define WSIOCGHIWAT  _IOR('s',  1, u_long)  /* get high watermark */
#define WSIOCSLOWAT  _IOW('s',  2, u_long)  /* set low watermark */
#define WSIOCGLOWAT  _IOR('s',  3, u_long)  /* get low watermark */
#define WSIOCATMARK  _IOR('s',  7, u_long)  /* at oob mark? */

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, taken from the BSD file netinet/in.h.
 */

/*
 * Protocols
 */
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_ICMP            1               /* control message protocol */
//#define IPPROTO_GGP             2               /* gateway^2 (deprecated) */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_PUP             12              /* pup */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_IDP             22              /* xns idp */
#define IPPROTO_ND              77              /* UNOFFICIAL net disk proto */

#define IPPROTO_RAW             255             /* raw IP packet */
#define IPPROTO_MAX             256

/*
 * Port/socket numbers: network standard functions
 */
#define IPPORT_ECHO             7
#define IPPORT_DISCARD          9
#define IPPORT_SYSTAT           11
#define IPPORT_DAYTIME          13
#define IPPORT_NETSTAT          15
#define IPPORT_FTP              21
#define IPPORT_TELNET           23
#define IPPORT_SMTP             25
#define IPPORT_TIMESERVER       37
#define IPPORT_NAMESERVER       42
#define IPPORT_WHOIS            43
#define IPPORT_MTP              57

/*
 * Port/socket numbers: host specific functions
 */
#define IPPORT_TFTP             69
#define IPPORT_RJE              77
#define IPPORT_FINGER           79
#define IPPORT_TTYLINK          87
#define IPPORT_SUPDUP           95

/*
 * UNIX TCP sockets
 */
#define IPPORT_EXECSERVER       512
#define IPPORT_LOGINSERVER      513
#define IPPORT_CMDSERVER        514
#define IPPORT_EFSSERVER        520

/*
 * UNIX UDP sockets
 */
#define IPPORT_BIFFUDP          512
#define IPPORT_WHOSERVER        513
#define IPPORT_ROUTESERVER      520
                                        /* 520+1 also used */

/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 */
#define IPPORT_RESERVED         1024

/*
 * Link numbers
 */
#define IMPLINK_IP              155
#define IMPLINK_LOWEXPER        156
#define IMPLINK_HIGHEXPER       158

/*
 * Internet address (old style... should be updated)
 */
struct Win_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
#define s_addr  S_un.S_addr
                                /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
                                /* host on imp */
#define s_net   S_un.S_un_b.s_b1
                                /* network */
#define s_imp   S_un.S_un_w.s_w2
                                /* imp */
#define s_impno S_un.S_un_b.s_b4
                                /* imp # */
#define s_lh    S_un.S_un_b.s_b3
                                /* logical host */
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define WIN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define WIN_CLASSA_NET           0xff000000
#define WIN_CLASSA_NSHIFT        24
#define WIN_CLASSA_HOST          0x00ffffff
#define WIN_CLASSA_MAX           128

#define WIN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define WIN_CLASSB_NET           0xffff0000
#define WIN_CLASSB_NSHIFT        16
#define WIN_CLASSB_HOST          0x0000ffff
#define WIN_CLASSB_MAX           65536

#define WIN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)
#define WIN_CLASSC_NET           0xffffff00
#define WIN_CLASSC_NSHIFT        8
#define WIN_CLASSC_HOST          0x000000ff

#define WINADDR_ANY              (u_long)0x00000000
#define WINADDR_LOOPBACK         0x7f000001
#define WINADDR_BROADCAST        (u_long)0xffffffff
#define WINADDR_NONE             0xffffffff

/*
 * Socket address, internet style.
 */
struct Wsockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128

typedef struct WSAData {
        WORD                    wVersion;
        WORD                    wHighVersion;
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char  *              lpVendorInfo;
} WSADATA;

typedef WSADATA  *LPWSADATA;

/*
 * Options for use with [gs]etsockopt at the IP level.
 */
#define IP_OPTIONS          1           /* set/get IP per-packet options    */

/*
 * Definitions related to sockets: types, address families, options,
 * taken from the BSD file sys/socket.h.
 */


/*
 * Types
 */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

/*
 * Option flags per-socket.
 */
#define SO_DEBUG        0x0001          /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002          /* socket has had listen() */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */
#define SO_BROADCAST    0x0020          /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040          /* bypass hardware when possible */
#define SO_LINGER       0x0080          /* linger on close if data present */
#define SO_OOBINLINE    0x0100          /* leave received OOB data in line */

#define SO_DONTLINGER   (u_int)(~SO_LINGER)

/*
 * Additional options.
 */
#define SO_SNDBUF       0x1001          /* send buffer size */
#define SO_RCVBUF       0x1002          /* receive buffer size */
#define SO_SNDLOWAT     0x1003          /* send low-water mark */
#define SO_RCVLOWAT     0x1004          /* receive low-water mark */
#define SO_SNDTIMEO     0x1005          /* send timeout */
#define SO_RCVTIMEO     0x1006          /* receive timeout */
#define SO_ERROR        0x1007          /* get error status and clear */
#define SO_TYPE         0x1008          /* get socket type */


/*
 * TCP options.
 */
#define TCP_NODELAY     0x0001
#define TCP_BSDURGENT   0x7000

/*
 * Address families.
 */
#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
//#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_VOICEVIEW    18              /* VoiceView */

//#define AF_MAX          19

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct Wsockaddr {
        u_short sa_family;              /* address family */
        char    sa_data[14];            /* up to 14 bytes of direct address */
};

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct Wsockproto {
        u_short sp_family;              /* address family */
        u_short sp_protocol;            /* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#define PF_UNSPEC       AF_UNSPEC
#define PF_UNIX         AF_UNIX
#define PF_INET         AF_INET
#define PF_IMPLINK      AF_IMPLINK
#define PF_PUP          AF_PUP
#define PF_CHAOS        AF_CHAOS
#define PF_NS           AF_NS
#define PF_IPX          AF_IPX
#define PF_ISO          AF_ISO
#define PF_OSI          AF_OSI
#define PF_ECMA         AF_ECMA
#define PF_DATAKIT      AF_DATAKIT
#define PF_CCITT        AF_CCITT
#define PF_SNA          AF_SNA
#define PF_DECnet       AF_DECnet
#define PF_DLI          AF_DLI
#define PF_LAT          AF_LAT
#define PF_HYLINK       AF_HYLINK
#define PF_APPLETALK    AF_APPLETALK
#define PF_VOICEVIEW    AF_VOICEVIEW

#define PF_MAX          AF_MAX

/*
 * Structure used for manipulating linger option.
 */
struct  Wlinger {
        u_short l_onoff;                /* option on/off */
        u_short l_linger;               /* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET      0xffff          /* options for socket level */

/*
 * Maximum queue length specifiable by listen.
 */
#define SOMAXCONN       5

#define MSG_OOB         0x1             /* process out-of-band data */
#define MSG_PEEK        0x2             /* peek at incoming message */
#define MSG_DONTROUTE   0x4             /* send without using routing tables */

#define MSG_MAXIOVLEN   16

#define MSG_PARTIAL     0x8000          /* partial send or recv for message xport */

/*
 * Define constant based on rfc883, used by gethostbyxxxx() calls.
 */
#define MAXGETHOSTSTRUCT        1024

/*
 * Define flags to be used with the WSAAsyncSelect() call.
 */
#define FD_READ         0x01
#define FD_WRITE        0x02
#define FD_OOB          0x04
#define FD_ACCEPT       0x08
#define FD_CONNECT      0x10
#define FD_CLOSE        0x20


/* Socket function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

SOCKET  WIN32API  OS2accept (SOCKET s, struct sockaddr *addr, int *addrlen);
int     WIN32API  OS2bind (SOCKET s, const struct sockaddr  *addr, int namelen);
int     WIN32API  OS2closesocket (SOCKET s);
int     WIN32API  OS2connect (SOCKET s, const struct sockaddr  *name, int namelen);
int     WIN32API  OS2ioctlsocket (SOCKET s, long cmd, u_long  *argp);
int     WIN32API  OS2getpeername (SOCKET s, struct sockaddr  *name,  int  * namelen);
int     WIN32API  OS2getsockname (SOCKET s, struct sockaddr  *name, int  * namelen);
int     WIN32API  OS2getsockopt (SOCKET s, int level, int optname, char  * optval, int  *optlen);
u_long  WIN32API  OS2htonl (u_long hostlong);
u_short WIN32API  OS2htons (u_short hostshort);
u_long  WIN32API  OS2inet_addr (const char  * cp);
char  * WIN32API  OS2inet_ntoa (struct in_addr in);
int     WIN32API  OS2listen (SOCKET s, int backlog);
u_long  WIN32API  OS2ntohl (u_long netlong);
u_short WIN32API  OS2ntohs (u_short netshort);
int     WIN32API  OS2recv (SOCKET s, char  * buf, int len, int flags);
int     WIN32API  OS2recvfrom (SOCKET s, char  * buf, int len, int flags, struct sockaddr  *from, int  * fromlen);
int     WIN32API  OS2select (int nfds, Wfd_set  *readfds, Wfd_set  *writefds, Wfd_set  *exceptfds, const struct Wtimeval  *timeout);
int     WIN32API  OS2send (SOCKET s, const char  * buf, int len, int flags);
int     WIN32API  OS2sendto (SOCKET s, const char  * buf, int len, int flags, const struct sockaddr  *to, int tolen);
int     WIN32API  OS2setsockopt (SOCKET s, int level, int optname, const char  * optval, int optlen);
int     WIN32API  OS2shutdown (SOCKET s, int how);
SOCKET  WIN32API  OS2socket (int af, int type, int protocol);

/* Database function prototypes */
struct Whostent  *  WIN32API OS2gethostbyaddr(const char  * addr, int len, int type);
struct Whostent  *  WIN32API OS2gethostbyname(const char  * name);
int                 WIN32API OS2gethostname (char  * name, int namelen);
struct Wservent  *  WIN32API OS2getservbyport(int port, const char  * proto);
struct Wservent  *  WIN32API OS2getservbyname(const char  * name, const char  * proto);
struct Wprotoent  * WIN32API OS2getprotobynumber(int proto);
struct Wprotoent  * WIN32API OS2getprotobyname(const char  * name);

/* Microsoft Windows Extension function prototypes */
int    WIN32API  OS2WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData);
int    WIN32API  OS2WSACleanup(void);
BOOL   WIN32API  OS2WSAIsBlocking(void);
int    WIN32API  OS2WSAUnhookBlockingHook(void);
PROC   WIN32API  OS2WSASetBlockingHook(PROC lpBlockFunc);
int    WIN32API  OS2WSACancelBlockingCall(void);
HANDLE WIN32API  OS2WSAAsyncGetServByName(HWND hWnd, u_int wMsg, const char  * name, const char  * proto, char  * buf, int buflen);
HANDLE WIN32API  OS2WSAAsyncGetServByPort(HWND hWnd, u_int wMsg, int port, const char  * proto, char  * buf, int buflen);
HANDLE WIN32API  OS2WSAAsyncGetProtoByName(HWND hWnd, u_int wMsg, const char  * name, char  * buf, int buflen);
HANDLE WIN32API  OS2WSAAsyncGetProtoByNumber(HWND hWnd, u_int wMsg, int number, char  * buf, int buflen);
HANDLE WIN32API  OS2WSAAsyncGetHostByName(HWND hWnd, u_int wMsg, const char  * name, char  * buf, int buflen);
HANDLE WIN32API  OS2WSAAsyncGetHostByAddr(HWND hWnd, u_int wMsg, const char  * addr, int len, int type, char  * buf, int buflen);
int    WIN32API  OS2WSACancelAsyncRequest(HANDLE hAsyncTaskHandle);
int    WIN32API  OS2WSAAsyncSelect(SOCKET s, HWND hWnd, u_int wMsg, long lEvent);
int    WIN32API  OS2WSARecvEx (SOCKET s, char  * buf, int len, int  *flags);


typedef struct _TRANSMIT_FILE_BUFFERS {
    PVOID Head;
    DWORD HeadLength;
    PVOID Tail;
    DWORD TailLength;
} TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, *LPTRANSMIT_FILE_BUFFERS;

typedef struct _OVERLAPPED {
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;


BOOL WIN32API OS2TransmitFile (SOCKET                  hSocket,
                               HANDLE                  hFile,
                               DWORD                   nNumberOfBytesToWrite,
                               DWORD                   nNumberOfBytesPerSend,
                               LPOVERLAPPED            lpOverlapped,
                               LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
                               DWORD                   dwReserved);

#ifdef __cplusplus
}
#endif

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr *LPSOCKADDR;

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in *LPSOCKADDR_IN;

typedef struct linger LINGER;
typedef struct linger *PLINGER;
typedef struct linger *LPLINGER;

typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr *LPIN_ADDR;

typedef struct Wfd_set WFD_SET;
typedef struct Wfd_set *PWFD_SET;
typedef struct Wfd_set *LPWFD_SET;

typedef struct Whostent WHOSTENT;
typedef struct Whostent *PWHOSTENT;
typedef struct Whostent *LPWHOSTENT;

typedef struct Wservent WSERVENT;
typedef struct Wservent *PWSERVENT;
typedef struct Wservent *LPWSERVENT;

typedef struct Wprotoent WPROTOENT;
typedef struct Wprotoent *PWPROTOENT;
typedef struct Wprotoent *LPWPROTOENT;

typedef struct Wtimeval WTIMEVAL;
typedef struct Wtimeval *PWTIMEVAL;
typedef struct Wtimeval *LPWTIMEVAL;

/*
 * Windows message parameter composition and decomposition
 * macros.
 *
 * WSAMAKEASYNCREPLY is intended for use by the Windows Sockets implementation
 * when constructing the response to a WSAAsyncGetXByY() routine.
 */
#define OS2WSAMAKEASYNCREPLY(buflen,error)     MAKELONG(buflen,error)
/*
 * WSAMAKESELECTREPLY is intended for use by the Windows Sockets implementation
 * when constructing the response to WSAAsyncSelect().
 */
#define OS2WSAMAKESELECTREPLY(event,error)     MAKELONG(event,error)
/*
 * WSAGETASYNCBUFLEN is intended for use by the Windows Sockets application
 * to extract the buffer length from the lParam in the response
 * to a WSAGetXByY().
 */
#define OS2WSAGETASYNCBUFLEN(lParam)           LOWORD(lParam)
/*
 * WSAGETASYNCERROR is intended for use by the Windows Sockets application
 * to extract the error code from the lParam in the response
 * to a WSAGetXByY().
 */
#define OS2WSAGETASYNCERROR(lParam)            HIWORD(lParam)
/*
 * WSAGETSELECTEVENT is intended for use by the Windows Sockets application
 * to extract the event code from the lParam in the response
 * to a WSAAsyncSelect().
 */
#define OS2WSAGETSELECTEVENT(lParam)           LOWORD(lParam)
/*
 * WSAGETSELECTERROR is intended for use by the Windows Sockets application
 * to extract the error code from the lParam in the response
 * to a WSAAsyncSelect().
 */
#define OS2WSAGETSELECTERROR(lParam)           HIWORD(lParam)

#endif  /* _WINSOCKAPI_ */


