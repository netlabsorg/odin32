#ifndef __IPHLWRAP_H__
#define __IPHLWRAP_H__

#include <sys\socket.h>
#include <sys\ioctl.h>
#include <netdb.h>
#include <netinet\in.h>
#include <netinet\tcp.h>
#include <nerrno.h>

int _System os2_ioctl (int, unsigned long, char *, int);

inline int _os2_ioctl (int a, unsigned long b, char *c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = os2_ioctl(a, b, c, d);
    SetFS(sel);

    return yyrc;
}

#undef  os2_ioctl
#define os2_ioctl _os2_ioctl

inline int _ioctl(int a, int b, char *c, int d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = ioctl(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  ioctl
#define ioctl _ioctl

inline int _res_init(void)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = res_init();
    SetFS(sel);

    return yyrc;
} 
#undef  res_init
#define res_init _res_init

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

#endif  


