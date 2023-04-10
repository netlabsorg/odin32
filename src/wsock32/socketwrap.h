#ifndef __SOCKETWRAP_H__
#define __SOCKETWRAP_H__

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <nerrno.h>

inline int _accept(int a, struct sockaddr *b, socklen_t *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = accept(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  accept
#define accept _accept

inline void _addsockettolist(int a)
{
 USHORT sel = RestoreOS2FS();

    addsockettolist(a);
    SetFS(sel);
}

#undef  addsockettolist
#define addsockettolist _addsockettolist

inline int _bind(int a, struct sockaddr *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = bind(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  bind
#define bind _bind

inline int _connect(int a, struct sockaddr *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = connect(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  connect
#define connect _connect

inline int _gethostid()
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = gethostid();
    SetFS(sel);

    return yyrc;
}

#undef  gethostid
#define gethostid _gethostid

inline int _getpeername(int a, struct sockaddr *b, socklen_t *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getpeername(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  getpeername
#define getpeername _getpeername

inline int _getsockname(int a, struct sockaddr *b, socklen_t *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getsockname(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  getsockname
#define getsockname _getsockname

inline int _getsockopt(int a, int b, socklen_t c, char *d, socklen_t *e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getsockopt(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  getsockopt
#define getsockopt _getsockopt

inline int _os2_ioctl(int a, int b, char *c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = os2_ioctl(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  ioctl
#define ioctl _os2_ioctl

inline int _listen(int a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = listen(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  listen
#define listen _listen

inline int _recvmsg(int a, struct msghdr *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = recvmsg(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  recvmsg
#define recvmsg _recvmsg

inline int _recv(int a, char *b, int c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = recv(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  recv
#define recv _recv

inline int _recvfrom(int a, char *b, int c, int d, struct sockaddr *e, socklen_t *f)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = recvfrom(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
}

#undef  recvfrom
#define recvfrom _recvfrom

inline int _removesocketfromlist(int a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = removesocketfromlist(a);
    SetFS(sel);

    return yyrc;
}

#undef  removesocketfromlist
#define removesocketfromlist _removesocketfromlist

inline int _os2_select(int *a, int b, int c, int d, long e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = os2_select(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  select
#define select _os2_select

inline int _send(int a, char *b, int c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = send(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  send
#define send _send

inline int _sendmsg(int a, struct msghdr *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = sendmsg(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  sendmsg
#define sendmsg _sendmsg

inline int _sendto(int a, char *b, int c, int d, struct sockaddr *e, int f)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = sendto(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
}

#undef  sendto
#define sendto _sendto

inline int _setsockopt(int a, int b, int c, char *d, int e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = setsockopt(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
}

#undef  setsockopt
#define setsockopt _setsockopt

inline int _sock_init()
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = sock_init();
    SetFS(sel);

    return yyrc;
}

#undef  sock_init
#define sock_init _sock_init

inline int _sock_errno()
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = sock_errno();
    SetFS(sel);

    return yyrc;
}

#undef  sock_errno
#define sock_errno _sock_errno

inline void _psock_errno(char *a)
{
 USHORT sel = RestoreOS2FS();

    psock_errno(a);
    SetFS(sel);
}

#undef  psock_errno
#define psock_errno _psock_errno

inline int _socket(int a, int b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = socket(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  socket
#define socket _socket

inline int _soclose(int a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = soclose(a);
    SetFS(sel);

    return yyrc;
}

#undef  soclose
#define soclose _soclose

inline int _so_cancel(int a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = so_cancel(a);
    SetFS(sel);

    return yyrc;
}

#undef  so_cancel
#define so_cancel _so_cancel

inline int _readv(int a, struct iovec *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = readv(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  readv
#define readv _readv

inline int _writev(int a, struct iovec *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = writev(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  writev
#define writev _writev

inline int _shutdown(int a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = shutdown(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  shutdown
#define shutdown _shutdown

inline int _Raccept(int a, struct sockaddr *b, int *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Raccept(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  Raccept
#define Raccept _Raccept

inline int _Rbind(int a, struct sockaddr_in *b, int c, struct sockaddr_in *d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Rbind(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  Rbind
#define Rbind _Rbind

inline int _Rconnect(int a, const struct sockaddr *b, int c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Rconnect(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  Rconnect
#define Rconnect _Rconnect

inline int _Rgetsockname(int a, struct sockaddr *b, int *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Rgetsockname(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  Rgetsockname
#define Rgetsockname _Rgetsockname

inline int _Rlisten(int a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Rlisten(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  Rlisten
#define Rlisten _Rlisten

inline int _gethostname(char *a, int b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = gethostname(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  gethostname
#define gethostname _gethostname

inline struct hostent *_gethostbyname(char *a)
{
 struct hostent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = gethostbyname(a);
    SetFS(sel);

    return yyrc;
}

#undef  gethostbyname
#define gethostbyname _gethostbyname

inline struct hostent *_gethostbyaddr(char *a, int b, int c)
{
 struct hostent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = gethostbyaddr(a, b, c);
    SetFS(sel);

    return yyrc;
}

#undef  gethostbyaddr
#define gethostbyaddr _gethostbyaddr

inline struct netent *_getnetbyname(char *a)
{
 struct netent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getnetbyname(a);
    SetFS(sel);

    return yyrc;
}

#undef  getnetbyname
#define getnetbyname _getnetbyname

inline struct netent *_getnetbyaddr(unsigned a, int b)
{
 struct netent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getnetbyaddr(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  getnetbyaddr
#define getnetbyaddr _getnetbyaddr

inline struct servent *_getservbyname(char *a, char *b)
{
 struct servent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getservbyname(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  getservbyname
#define getservbyname _getservbyname

inline struct servent *_getservbyport(int a, char *b)
{
 struct servent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getservbyport(a, b);
    SetFS(sel);

    return yyrc;
}

#undef  getservbyport
#define getservbyport _getservbyport

inline struct protoent *_getprotobyname(char *a)
{
 struct protoent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getprotobyname(a);
    SetFS(sel);

    return yyrc;
}

#undef  getprotobyname
#define getprotobyname _getprotobyname

inline struct protoent *_getprotobynumber(int a)
{
 struct protoent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getprotobynumber(a);
    SetFS(sel);

    return yyrc;
}

#undef  getprotobynumber
#define getprotobynumber _getprotobynumber

inline void _sethostent(int a)
{
 USHORT sel = RestoreOS2FS();

    sethostent(a);
    SetFS(sel);
}

#undef  sethostent
#define sethostent _sethostent

inline struct hostent *_gethostent()
{
 struct hostent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = gethostent();
    SetFS(sel);

    return yyrc;
}

#undef  gethostent
#define gethostent _gethostent

inline void _endhostent()
{
 USHORT sel = RestoreOS2FS();

    endhostent();
    SetFS(sel);
}

#undef  endhostent
#define endhostent _endhostent

inline void _setnetent(int a)
{
 USHORT sel = RestoreOS2FS();

    setnetent(a);
    SetFS(sel);
}

#undef  setnetent
#define setnetent _setnetent

inline struct netent *_getnetent()
{
 struct netent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getnetent();
    SetFS(sel);

    return yyrc;
}

#undef  getnetent
#define getnetent _getnetent

inline void _endnetent()
{
 USHORT sel = RestoreOS2FS();

    endnetent();
    SetFS(sel);
}

#undef  endnetent
#define endnetent _endnetent

inline void _setprotoent(int a)
{
 USHORT sel = RestoreOS2FS();

    setprotoent(a);
    SetFS(sel);
}

#undef  setprotoent
#define setprotoent _setprotoent

inline struct protoent *_getprotoent()
{
 struct protoent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getprotoent();
    SetFS(sel);

    return yyrc;
}

#undef  getprotoent
#define getprotoent _getprotoent

inline void _endprotoent()
{
 USHORT sel = RestoreOS2FS();

    endprotoent();
    SetFS(sel);
}

#undef  endprotoent
#define endprotoent _endprotoent

inline void _setservent(int a)
{
 USHORT sel = RestoreOS2FS();

    setservent(a);
    SetFS(sel);
}

#undef  setservent
#define setservent _setservent

inline struct servent *_getservent()
{
 struct servent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = getservent();
    SetFS(sel);

    return yyrc;
}

#undef  getservent
#define getservent _getservent

inline void _endservent()
{
 USHORT sel = RestoreOS2FS();

    endservent();
    SetFS(sel);
}

#undef  endservent
#define endservent _endservent

inline int _tcp_h_errno()
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = tcp_h_errno();
    SetFS(sel);

    return yyrc;
}

#undef  tcp_h_errno
#define tcp_h_errno _tcp_h_errno

inline struct hostent *_Rgethostbyname(char *a)
{
 struct hostent * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = Rgethostbyname(a);
    SetFS(sel);

    return yyrc;
}

#undef  Rgethostbyname
#define Rgethostbyname _Rgethostbyname


#endif  //__SOCKETWRAP_H__


