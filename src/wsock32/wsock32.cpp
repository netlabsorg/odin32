/* $Id: wsock32.cpp,v 1.2 1999-06-10 16:11:08 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 SOCK32 for OS/2
 *
 * 1998/08/25 Vince Vielhaber
 *
 * @(#) wsock32.c     	1.0.0   1998/08/25 VV initial release
 *                      1.0.1   1999/04/27 VV cleanup and implement select.
 *
 */

#define INCL_DOSPROCESS     /* Process and thread values */
#define INCL_DOSFILEMGR     /* for DosRead and DosWrite */
#define INCL_DOSQUEUES      /* for unnamed pipes */
#define INCL_DOSERRORS      /* DOS error values          */
#define INCL_WINMESSAGEMGR  /* Window Message Functions     */
#define INCL_WINMENUS       /* Window Menu Functions        */

/* this is for IBM TCP/IP 5.0 headers as present in the current Warp 4 toolkit */
#define TCPV40HDRS 1

#define VV_BSD_SELECT       /* undefine this if it interferes with other routines */

#ifdef VV_BSD_SELECT
# define BSD_SELECT
#endif

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#ifdef VV_BSD_SELECT
# include <sys/select.h>
#endif

#include <sys/time.h>
#include <win32type.h>
#include "wsock32.h"
#include "misc.h"


#define FAR
//#define WIN32API APIENTRY

static WHOSTENT whsnt;
static WSERVENT wsvnt;
static WPROTOENT wptnt;

 /*+-------------------------------------------------------------------+*/
 /*| _CRT_init is the C run-time environment initialization function.  |*/
 /*|It will return 0 to indicate success and -1 to indicate failure.   |*/
 /*+-------------------------------------------------------------------+*/
 
int _CRT_init (void);
 
 /*+-------------------------------------------------------------------+*/
 /*| _CRT_term is the C run-time environment termination function.     |*/
 /*+-------------------------------------------------------------------+*/
 
void _CRT_term (unsigned long);

void _System AsyncLoop(ULONG);
void CheckThreads(AsyncStatus *);
void NotifyApp(int,AsyncStatus *);
int Notify(AsyncStatus *,int);

int NotifyWSA(HWND hw,u_int msg,UINT wp,LONG lp);

void _System WSAFunct(ULONG); // for the wsa database calls
void SetErrForDatabaseCalls(void);


size_t nSize;
int *pArray;

int MsgSent;
int LocalErrorNumber = 0;


TID tidAsyncLoop = 0;     /* ID of AsyncSelect (AsyncLoop) thread */

/*

 
typedef struct AsyncStatus {
    HWND hwnd;     // owner's hwindow
    u_int msg;     // message to send when event occurs
    long event;    // event that may occur
    SOCKET socket; // the socket
    int status;    // blocking yes/no
    TID threadID;  // Thread ID for async
    int MsgStat;   // has message been sent yet?
    struct AsyncStatus *Next; // pointer to next AsyncStatus in the list
    struct AsyncStatus *Prev; // pointer to previous AsyncStatus in the list
} AsyncStatus;
 
 
 
*/

AsyncStatus *TopASY = 0;


#define BLOCKING 0
#define NONBLOCKING 1



typedef struct WSAStruct {
    int CallingWhat;
    HWND hw;
    u_int msg;
    char *carg1;
    char *carg2;
    int iarg1;
    int iarg2;
    char *buf;
    int buflen;
} WSAStruct;


#define GETSERVBYNAME    1
#define GETSERVBYPORT    2
#define GETPROTOBYNAME   3
#define GETPROTOBYNUMBER 4
#define GETHOSTBYNAME    5
#define GETHOSTBYADDR    6




typedef struct PipeStruct {
    AsyncStatus as;
    int MsgLoop;
    HFILE rcv;
    HFILE snd;
} PipeStruct;

PipeStruct PS;



#ifdef FD_CLR
#undef FD_CLR
#define FD_CLR(x,y) WFD_CLR(x,y)
#undef FD_SET
#define FD_SET(x,y) WFD_SET(x,y)
#undef FD_ZERO
#define FD_ZERO(x) WFD_ZERO(x)
#undef FD_ISSET
#define FD_ISSET(x,y) WFD_SET(x,y)
#endif






int WIN32API OS2__WSAFDIsSet(SOCKET fd, Wfd_set FAR *set) 
{
int i = set->fd_count;

#ifdef DEBUG
    WriteLog("WSOCK32: __WSAFDIsSet\n");
#endif
 
     while (i--)
         if (set->fd_array[i] == fd)
             return 1;
 
return 0;
}



AsyncStatus * FindASY(SOCKET s)
{
AsyncStatus *as;

    for(as = TopASY; as; as = as->Next)
        if(as->socket == s)
            return as;

return 0;
}



SOCKET WIN32API OS2accept (SOCKET s, struct sockaddr *addr,
                        int *addrlen) {
#ifdef DEBUG
    WriteLog("WSOCK32: accept\n");
#endif

    return accept(s,addr,addrlen);
}

int WIN32API OS2bind (SOCKET s, const struct sockaddr *addr, int namelen)
{
 int rc;

#ifdef DEBUG
    WriteLog("WSOCK32: bind\n");
#endif

    rc = bind(s,(struct sockaddr *)addr,namelen);
    dprintf(("bind returned %X\n", rc));
    return(rc);
}

int WIN32API OS2closesocket (SOCKET s)
{
AsyncStatus *as;
#ifdef DEBUG
    WriteLog("WSOCK32: closesocket\n");
#endif

    as = FindASY(s);
    if(as == NULL) {
        LocalErrorNumber = 10038;
        return -1;
    }

    CheckThreads(as);

#ifdef DEBUG
    WriteLog("WSOCK32: closesocket\n");
#endif
    if(as->Prev && as->Next)
        as->Prev->Prev = as->Next->Next; // I SURE HOPE THIS IS RIGHT!!!!!!!!
    free(as);
    
    return soclose((int)s);
}

int WIN32API OS2connect (SOCKET s, const struct sockaddr *name, int namelen)
{
#ifdef DEBUG
//char message[512];
//struct Wsockaddr_in *xname;

  WriteLog("WSOCK32: connect\n");


#endif

    return connect(s,(struct sockaddr *)name,namelen);
}

int WIN32API OS2ioctlsocket (SOCKET s, long cmd, u_long *argp)
{
 int rc;

#ifdef DEBUG
    WriteLog("WSOCK32: ioctlsocket unimplemented\n");
#endif

    rc = ioctl(s, cmd, (char *)argp, 4);
    dprintf(("ioctl returned %X\n", rc));
    return(rc);
}

int WIN32API OS2getpeername (SOCKET s, struct sockaddr *name,
                          int * namelen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: getpeername\n");
#endif

    return getpeername(s,name,namelen);
}

int WIN32API OS2getsockname (SOCKET s, struct sockaddr *name,
                          int * namelen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: getsockname\n");
#endif

    return getsockname(s,name,namelen);
}

int WIN32API OS2getsockopt (SOCKET s, int level, int optname,
                         char * optval, int *optlen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: getsockopt\n");
#endif

    return getsockopt(s,level,optname,optval,optlen);
}

u_long WIN32API OS2htonl (u_long hostlong)
{
#ifdef DEBUG
    WriteLog("WSOCK32: htonl\n");
#endif

    return htonl(hostlong);
}

u_short WIN32API OS2htons (u_short hostshort)
{
#ifdef DEBUG
    WriteLog("WSOCK32: htons\n");
#endif

    return htons(hostshort);
}

unsigned long WIN32API OS2inet_addr (const char * cp)
{
#ifdef DEBUG
    WriteLog("WSOCK32: inet_addr\n");
#endif

    return inet_addr((char *)cp);
}

char * WIN32API OS2inet_ntoa (struct in_addr in)
{
#ifdef DEBUG
    WriteLog("WSOCK32: inet_ntoa\n");
#endif

    return inet_ntoa(in);
}

int WIN32API OS2listen (SOCKET s, int backlog)
{
#ifdef DEBUG
    WriteLog("WSOCK32: listen\n");
#endif

    return listen(s,backlog);
}

u_long WIN32API OS2ntohl (u_long netlong)
{
#ifdef DEBUG
    WriteLog("WSOCK32: ntohl\n");
#endif

    return ntohl(netlong);
}

u_short WIN32API OS2ntohs (u_short netshort)
{
#ifdef DEBUG
    WriteLog("WSOCK32: ntohs\n");
#endif

    return ntohs(netshort);
}

int WIN32API OS2recv (SOCKET s, char * buf, int len, int flags)
{
unsigned long ii;
int xx,yy;
char buff[200];
AsyncStatus *as;
#ifdef DEBUG
    WriteLog("WSOCK32: recv socket: %u\n",(unsigned int)s);
    WriteLog("WSOCK32: recv len: %d\n",len);
    WriteLog("WSOCK32: recv flags: %d\n",flags);

#endif

    PS.MsgLoop = 0;

    as = FindASY(s);

    if(as != NULL) as->MsgStat = 2;

    
    xx = recv(s,buf,len,flags);

return xx;
}



int WIN32API OS2recvfrom (SOCKET s, char * buf, int len, int flags,
                       struct sockaddr *from, int * fromlen)
{
 int rc;

#ifdef DEBUG
    WriteLog("WSOCK32: recvfrom\n");
#endif

    rc = recvfrom(s,buf,len,flags,from,fromlen);
    dprintf(("recvfrom returned %X\n", rc));
    return(rc);
}



#ifdef VV_BSD_SELECT


int WIN32API OS2select (int nfds, Wfd_set *readfds, Wfd_set *writefds,
                     Wfd_set *exceptfds, const struct Wtimeval *timeout)
{
#ifdef DEBUG
    WriteLog("WSOCK32: select\n");
#endif


    return select(nfds,(fd_set *)readfds,(fd_set *)writefds,(fd_set *)exceptfds,(timeval *)timeout);
    
}



#else


int WIN32API OS2select (int nfds, Wfd_set *readfds, Wfd_set *writefds,
                     Wfd_set *exceptfds, const struct Wtimeval *timeout)
{
#ifdef DEBUG
    WriteLog("WSOCK32: select\n");
#endif

    // NEED TO DO THIS ONE!!!!!
    
    return 0;
}
#endif



int WIN32API OS2send (SOCKET s, const char * buf, int len, int flags)
{
#ifdef DEBUG
    WriteLog("WSOCK32: send socket: %u\n",(unsigned int)s);
//    WriteLog("WSOCK32: send buf: %s\n",buf);
    WriteLog("WSOCK32: send len: %d\n",len);
    WriteLog("WSOCK32: send flags: %d\n",flags);
#endif

    return send(s,(char *)buf,len,flags);
}

int WIN32API OS2sendto (SOCKET s, const char * buf, int len, int flags,
                     const struct sockaddr *to, int tolen)
{
 int rc;

#ifdef DEBUG
    WriteLog("WSOCK32: sendto\n");
#endif

    rc = sendto(s,(char *)buf,len,flags,(struct sockaddr *)to,tolen);
    dprintf(("sendto returned %X\n", rc));
    return(rc);
}

int WIN32API OS2setsockopt (SOCKET s, int level, int optname,
                         const char * optval, int optlen)
{
struct Wlinger *yy;
struct linger xx;

#ifdef DEBUG
    WriteLog("WSOCK32: setsockopt\n");
#endif

    if(level == SOL_SOCKET && optname == SO_LINGER) {
	yy = (struct Wlinger *)optval;
	xx.l_onoff = (int)yy->l_onoff;
	xx.l_linger = (int)yy->l_linger;

        return setsockopt(s,level,optname,(char *)&xx,optlen);
    } 

    else return setsockopt(s,level,optname,(char *)optval,optlen);

}

int WIN32API OS2shutdown (SOCKET s, int how)
{
#ifdef DEBUG
    WriteLog("WSOCK32: shutdown\n");
#endif

    return shutdown(s,how);
}

SOCKET WIN32API OS2socket (int af, int type, int protocol)
{
SOCKET s;
AsyncStatus *as;
#ifdef DEBUG
    WriteLog("WSOCK32: socket\n");
#endif

    s = (SOCKET)socket(af,type,protocol);

    if(s > 0) {
        as = (AsyncStatus *)malloc(sizeof(AsyncStatus));
        if(as != NULL) {
           as->hwnd = (HWND)0;
           as->msg = 0;
           as->event = 0L;
           as->socket = s;
           as->status = BLOCKING;
           as->threadID = 0;
           as->MsgStat = 0;
           as->Next = TopASY;
           as->Prev = NULL;
           if(TopASY) TopASY->Prev = as;
           TopASY = as;
        } else {
            soclose(s);
            return -1;
        }
    }
    dprintf(("WSOCK32: socket returned %X\n", s));

return s;
}

/* Database function prototypes */


void SetErrForDatabaseCalls(void)
{

    switch(h_errno) {
        case 1:
            OS2WSASetLastError(11001); // host not found
            break;
        case 2:
            OS2WSASetLastError(11002); // try again later
            break;
        case 3:
            OS2WSASetLastError(11003); // No recovery
            break;
        case 4:
            OS2WSASetLastError(11004); // No address or no data
            break;
        default:
            OS2WSASetLastError(0); // unknown error and should never get here
#ifdef DEBUG
WriteLog("WSOCK32: Unknown error condition: %d\n",h_errno);
#endif
            break;
    }

return;
}




WHOSTENT * WIN32API OS2gethostbyaddr(const char * addr,
                                        int len, int type)
{
WHOSTENT *yy;
struct hostent *xx;
#ifdef DEBUG
    WriteLog("WSOCK32: gethostbyaddr\n");
#endif

    xx = gethostbyaddr((char *)addr,len,type);

    if(xx == NULL) {
        SetErrForDatabaseCalls();
        return (WHOSTENT *)NULL;
    }

    whsnt.h_name = xx->h_name;
    whsnt.h_aliases = xx->h_aliases;
    whsnt.h_addrtype = (short)xx->h_addrtype;
    whsnt.h_length = (short)xx->h_length;
    whsnt.h_addr_list = xx->h_addr_list;

return &whsnt;
}




WHOSTENT * WIN32API OS2gethostbyname(const char * name)
{
WHOSTENT *yy;
struct hostent *xx;
#ifdef DEBUG
    WriteLog("WSOCK32: gethostbyname: %s\n",name);
#endif

    xx = gethostbyname((char *)name);

    if(xx == NULL) {
        SetErrForDatabaseCalls();
        return (WHOSTENT *)NULL;
    }

    whsnt.h_name = xx->h_name;
    whsnt.h_aliases = xx->h_aliases;
    whsnt.h_addrtype = (short)xx->h_addrtype;
    whsnt.h_length = (short)xx->h_length;
    whsnt.h_addr_list = xx->h_addr_list;
  
return &whsnt;    
}






int WIN32API _OS2gethostname (char * name, int namelen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: _gethostname\n");
#endif

    return gethostname(name,namelen);
}



int WIN32API OS2gethostname (char * name, int namelen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: gethostname\n");
#endif

    return gethostname(name,namelen);
}

WSERVENT * WIN32API OS2getservbyport(int port, const char * proto)
{
struct servent *xx;
#ifdef DEBUG
    WriteLog("WSOCK32: getservbyport\n");
#endif

    xx = getservbyport(port,(char *)proto);

    if(xx == NULL) { // this call doesn't generate an error message
        return (WSERVENT *)NULL;
    }

    wsvnt.s_name = xx->s_name;
    wsvnt.s_aliases = xx->s_aliases;
    wsvnt.s_port = (short)xx->s_port;
    wsvnt.s_proto = xx->s_proto;

return &wsvnt;
}

WSERVENT * WIN32API OS2getservbyname(const char * name,
                                        const char * proto)
{
WSERVENT *yy;
struct servent *xx;
#ifdef DEBUG
    WriteLog("WSOCK32: getservbyname\n");
#endif

    xx = getservbyname((char *)name,(char *)proto);

    if(xx == NULL) { // this call doesn't generate an error message
      return (WSERVENT *)NULL;
    }

    wsvnt.s_name = xx->s_name;
    wsvnt.s_aliases = xx->s_aliases;
    wsvnt.s_port = (short)xx->s_port;
    wsvnt.s_proto = xx->s_proto;

return &wsvnt;
}

WPROTOENT * WIN32API OS2getprotobynumber(int proto)
{
struct protoent *xx;
#ifdef DEBUG
    WriteLog("WSOCK32: getprotobynumber\n");
#endif

    xx = getprotobynumber(proto);

    if(xx == NULL) { // this call doesn't generate an error message
        return (WPROTOENT *)NULL;
    }

    wptnt.p_name = xx->p_name;
    wptnt.p_aliases = xx->p_aliases;
    wptnt.p_proto = (short)xx->p_proto;

return &wptnt;
}

WPROTOENT * WIN32API OS2getprotobyname(const char * name)
{
struct protoent *xx;
#ifdef DEBUG
    WriteLog("WSOCK32: getprotobyname\n");
#endif

    xx = getprotobyname((char *)name);

    if(xx == NULL) { // this call doesn't generate an error message
        return (WPROTOENT *)NULL;
    }

    wptnt.p_name = xx->p_name;
    wptnt.p_aliases = xx->p_aliases;
    wptnt.p_proto = (short)xx->p_proto;

return &wptnt;
}




int WIN32API OS2WSAStartup(USHORT wVersionRequired, LPWSADATA lpWsaData)
{
APIRET rc;
int ii;
#ifdef DEBUG
    WriteLog("WSOCK32: WSAStartup\n");
#endif
  /* Make sure that the version requested is >= 1.1.   */
  /* The low byte is the major version and the high    */
  /* byte is the minor version.                        */
  
  if ( LOBYTE( wVersionRequired ) < 1 ||
       ( LOBYTE( wVersionRequired ) == 1 &&
         HIBYTE( wVersionRequired ) < 1 )) {
      return WSAVERNOTSUPPORTED;
  }
  
  /* Since we only support 1.1, set both wVersion and  */
  /* wHighVersion to 1.1.                              */
  
  lpWsaData->wVersion = MAKEWORD( 1, 1 );
  lpWsaData->wHighVersion = MAKEWORD( 1, 1 );
  strcpy(lpWsaData->szDescription,"Win32OS2 WSOCK32.DLL Ver. 1.1");
  lpWsaData->iMaxUdpDg = 32767;
  lpWsaData->iMaxSockets = 2048;
  strcpy(lpWsaData->szSystemStatus,"No Status");

  LocalErrorNumber = 0;

  if(sock_init() == 0) {
#ifdef DEBUG
    WriteLog("WSOCK32: WSAStartup sock_init returned 0\n");
#endif
      return 0;
  }
  else ii = sock_errno();
#ifdef DEBUG
    WriteLog("WSOCK32: WSAStartup exiting: %d\n",ii);
#endif
return ii;
}






int WIN32API OS2WSACleanup(void)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSACleanup\n");
#endif

    CheckThreads((AsyncStatus *)NULL);
    
    return 0;
}

void WIN32API OS2WSASetLastError(int iError)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSASetLastError(%d)\n",iError);
#endif

    LocalErrorNumber = iError;
    
    return; 
}

int WIN32API OS2WSAGetLastError(void)
{
int ii;
#ifdef DEBUG
    WriteLog("WSOCK32: WSAGetLastError\n");
#endif

    if(LocalErrorNumber == 0) {
        ii = sock_errno(); // WSAGetLastError();
#ifdef DEBUG
    WriteLog("WSOCK32: WSAGetLastError: %d\n",ii);
#endif
	return ii;
    }
    else {
#ifdef DEBUG
    WriteLog("WSOCK32: WSAGetLastError: %d\n",LocalErrorNumber);
#endif
	return LocalErrorNumber;
    }
}

BOOL WIN32API OS2WSAIsBlocking(void)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAIsBlocking unimplemented\n");
#endif

    return -5000; //WSAIsBlocking();
}

int WIN32API OS2WSAUnhookBlockingHook(void)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAUnhookBlockingHook unimplemented\n");
#endif

    return -5000; //WSAUnhookBlockingHook();
}

FARPROC WIN32API OS2WSASetBlockingHook(FARPROC lpBlockFunc)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSASetBlockingHook Unimplemented\n");
#endif

    return (FARPROC)NULL;
}

int WIN32API OS2WSACancelBlockingCall(void)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSACancelBlockingCall unimplemented\n");
#endif

    return -5000; //WSACancelBlockingCall();
}





// The following 6 calls need to start a new thread, perform
// the operation, copy ALL the info to the buffer provided then
// notify the sender.


LHANDLE WIN32API OS2WSAAsyncGetServByName(HWND hWnd, u_int wMsg,
                                        const char * name,
                                        const char * proto,
                                        char * buf, int buflen)
{
WSAStruct *wsa;
PFNTHREAD   pfnAsyncThread = &WSAFunct; /* Address of thread program   */
ULONG       ulThreadParm = 100;          /* Parameter to thread routine     */
APIRET      rc           = NO_ERROR;     /* Return code */
TID         tid;


OS2WSASetLastError(WSAEWOULDBLOCK);
return 0;

#ifdef DEBUG
WriteLog("WSOCK32: WSAAsyncGetServByName.  name: %s, proto: %s \n",name,proto);
#endif

    wsa = (WSAStruct *)malloc(sizeof(WSAStruct));
    if(wsa == NULL) {
        OS2WSASetLastError(WSAEWOULDBLOCK);
        return 0;
    }

    wsa->CallingWhat = GETSERVBYNAME;
    wsa->hw = hWnd;
    wsa->msg = wMsg;
    wsa->carg1 = strdup(name);
    wsa->carg2 = strdup(proto);
    wsa->buf = buf;
    wsa->buflen = buflen;

    ulThreadParm = (ULONG)wsa;
    
    rc = DosCreateThread(&tid,           /* Thread ID (returned by function)  */
                         pfnAsyncThread, /* Address of thread program         */
                         ulThreadParm,   /* Parameter passed to ThreadProc    */
                         CREATE_READY |  /* Thread is ready when created      */
                         STACK_SPARSE,   /* Do not pre-commit stack pages     */
                         8192L);         /* Stack size, rounded to page bdy   */
    if (rc != NO_ERROR) {
#ifdef DEBUG
        WriteLog("WSOCK32: DosCreateThread error in WSAAsyncGetServByName: return code = %u\n", rc);
#endif
        OS2WSASetLastError(rc);
        free(wsa);
        return 0;
    }

#ifdef DEBUG
    WriteLog("WSOCK32 THREAD: DosCreateThread's tid: %lu, ThreadParm = %p\n",(unsigned long)tid,ulThreadParm);
    WriteLog("WSOCK32 THREAD: hwnd: %p\n",wsa->hw);
#endif

    return (LHANDLE)tid; //WSAAsyncGetServByName(hWnd,wMsg,name,proto,buf,buflen);
}

LHANDLE WIN32API OS2WSAAsyncGetServByPort(HWND hWnd, u_int wMsg, int port,
                                        const char * proto, char * buf,
                                        int buflen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAAsyncGetServByPort unimplemented\n");
#endif

    return -5000; //WSAAsyncGetServByPort(hWnd,wMsg,port,proto,buf,buflen);
}

LHANDLE WIN32API OS2WSAAsyncGetProtoByName(HWND hWnd, u_int wMsg,
                                         const char * name, char * buf,
                                         int buflen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAAsyncGetProtoByName unimplemented\n");
#endif

    return -5000; //WSAAsyncGetProtoByName(hWnd,wMsg,name,buf,buflen);
}

LHANDLE WIN32API OS2WSAAsyncGetProtoByNumber(HWND hWnd, u_int wMsg,
                                           int number, char * buf,
                                           int buflen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAAsyncGetProtoByNumber unimplemented\n");
#endif

    return -5000; //WSAAsyncGetProtoByNumber(hWnd,wMsg,number,buf,buflen);
}

LHANDLE WIN32API OS2WSAAsyncGetHostByName(HWND hWnd, u_int wMsg,
                                        const char * name, char * buf,
                                        int buflen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAAsyncGetHostByName unimplemented\n");
#endif

    return -5000; //WSAAsyncGetHostByName(hWnd,wMsg,name,buf,buflen);
}

LHANDLE WIN32API OS2WSAAsyncGetHostByAddr(HWND hWnd, u_int wMsg,
                                        const char * addr, int len, int type,
                                        char * buf, int buflen)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSAAsyncGetHostByAddr unimplemented\n");
#endif




    
    return -5000; //WSAAsyncGetHostByAddr(hWnd,wMsg,addr,len,type,buf,buflen);
}







int WIN32API OS2WSACancelAsyncRequest(LHANDLE hAsyncTaskHandle)
{
TID tid;
APIRET rc;
#ifdef DEBUG
    WriteLog("WSOCK32: WSACancelAsyncRequest unimplemented\n");
#endif

    rc = 0;
    
    tid = (LHANDLE)hAsyncTaskHandle;

    if(tid == 0) rc = WSAEINVAL;

    if(!rc) rc = DosKillThread(tid);

    switch(rc) {
        case 0: // SUCCESS!!
            return 0;
        case 170:
            rc = WSAEINPROGRESS;
            break;
        case 309:
            rc = WSAEINVAL;
            break;
        default:
            rc = -5000;
            break;
    } // end switch

    OS2WSASetLastError(rc);
    return SOCKET_ERROR; // ERROR!
}





int WIN32API OS2WSAAsyncSelect(SOCKET s, HWND hWnd, u_int wMsg,
                            long lEvent)
{
PFNTHREAD   pfnAsyncThread = &AsyncLoop; /* Address of thread program   */
ULONG       ulThreadParm = 100;          /* Parameter to thread routine     */
APIRET      rc           = NO_ERROR;     /* Return code                     */
unsigned long ii;
AsyncStatus *as;
#ifdef DEBUG
char buf[150];
sprintf(buf,"WSOCK32: WSAAsyncSelect\n         Message: %x\n         Event: %ld\n         hwindow: %x\n",
        wMsg,lEvent,(HWND)hWnd);
    WriteLog(buf);
#endif

    as = FindASY(s);

    if(as == NULL) return 0;
    
    CheckThreads(as);

    as->hwnd = hWnd;
    as->msg = wMsg;
    as->event = lEvent;

    ulThreadParm = (ULONG)as;
    
    rc = DosCreateThread(&(as->threadID),  /* Thread ID (returned by function)  */
                         pfnAsyncThread, /* Address of thread program         */
                         ulThreadParm,   /* Parameter passed to ThreadProc    */
                         CREATE_READY |  /* Thread is ready when created      */
                         STACK_SPARSE,   /* Do not pre-commit stack pages     */
                         8192L);         /* Stack size, rounded to page bdy   */
    if (rc != NO_ERROR) {
#ifdef DEBUG
        WriteLog("WSOCK32: DosCreateThread error: return code = %u\n", rc);
#endif
        OS2WSASetLastError(rc);
        return 0;
    }

    return 1; //WSAAsyncSelect(s,hWnd,wMsg,lEvent);
}



void _System AsyncLoop(ULONG ASP)
{
int socks[1],r,w,e,rc,ii;
AsyncStatus *as;

    as = (AsyncStatus *)ASP;

    r = w = e = 0;
    if(as->event & FD_READ) r = 1;
    if(as->event & FD_WRITE) w = 1;
    if(as->event & FD_OOB) e = 1;

    socks[0] = (int)as->socket;

    if((r+w+e) == 0) {
#ifdef DEBUG
        WriteLog("WSOCK32: Turning off async\n");
#endif
        ii = 0;
        rc = ioctl(socks[0],FIONBIO,(char *)&ii,sizeof(ii));
        as->threadID = 0;
        as->hwnd = 0;
        as->msg = 0;
        as->event = 0;
        as->status = BLOCKING;
        return;
    } // end if
    else
    {
#ifdef DEBUG
        WriteLog("WSOCK32: Setting up non-blocking sockets\n");
#endif
        ii = 1;
        rc = ioctl(socks[0],FIONBIO,(char *)&ii,sizeof(ii));
        if(rc != 0) {
#ifdef DEBUG
	    WriteLog("WSOCK32: ioctl failed trying to non-block.\n");
#endif
	    return;
        } 
        as->status = NONBLOCKING;
    } // end else

    do {
        rc = select(socks[0],(fd_set*)&r,0,0,0); // ioctl may be better for this.
        if(rc > 0) {
            rc = ioctl(socks[0],FIONREAD,(char *)&ii,sizeof(ii));
            if(rc == 0 && ii > 0) {
            /* data is ready */
                NotifyApp(FD_READ,as);
//#ifdef DEBUG
//                WriteLog("WSOCK32: Data Waiting\n");
//#endif
            }
        }
        if(rc < 0) {
            rc = sock_errno();
            /* something ain't right */
            if(rc == 10038) { // Connection closed
                NotifyApp(FD_CLOSE,as);
                DosSleep(500);
                return;
            }
#ifdef DEBUG
            WriteLog("WSOCK32: Select error: %d\n",rc);
#endif
        } // end if
        DosSleep(50);
    } while(1);

return;
}



void CheckThreads(AsyncStatus *as)
{
AsyncStatus *asy;

    if(as != NULL)
        if(as->threadID != 0) DosKillThread(as->threadID);

    for(asy = TopASY; asy; asy = asy->Next)
        if(asy->threadID != 0) DosKillThread(asy->threadID);
    
return;
}



void NotifyApp(int xx,AsyncStatus *as)
{
    BOOL    fResult;        /* message-posted indicator             */
    unsigned long ii;


//#ifdef DEBUG
//    WriteLog("WSOCK32: Notifying the caller.  rc = %d\n",xx);
//#endif

    if(as->MsgStat == 0) {
        fResult = Notify(as,xx);
#ifdef DEBUG
    WriteLog("WSOCK32: Notify returns: %d\n",fResult);
#endif
    } // end if

    if(as->MsgStat == 2) as->MsgStat = 0;
    else as->MsgStat = 1;

return;
}



void _System WSAFunct(ULONG xx)
{
WSAStruct *wsa;
WSERVENT *ooo;
char *yy;
int ii;
size_t ss;
UINT wp;
LONG lp;
int id = *_threadid;

    wsa = (WSAStruct *)xx;

#ifdef DEBUG
    WriteLog("WSOCK32: WSAFunct: xx = %p, hwnd = %p\n",xx,wsa->hw);
    WriteLog("WSOCK32: WSAFunct info carg1 = %s, carg2 = %s\n",wsa->carg1,wsa->carg2);
    WriteLog("WSOCK32: WSAFunct info buf = %p, %d\n",wsa->buf,wsa->buflen);
#endif

    switch (wsa->CallingWhat) {
        case GETSERVBYNAME:
            yy = (char *)OS2getservbyname(wsa->carg1,wsa->carg2);
            ss = sizeof(WSERVENT);
            break;
        case GETSERVBYPORT:
            yy = (char *)OS2getservbyport(wsa->iarg1,wsa->carg1);
            break;
        case GETPROTOBYNUMBER:
            yy = (char *)OS2getprotobynumber(wsa->iarg1);
            break;
        case GETPROTOBYNAME:
            yy = (char *)OS2getprotobyname(wsa->carg1);
            break;
        case GETHOSTBYNAME:
            yy = (char *)OS2gethostbyname(wsa->carg1);
            break;
        case GETHOSTBYADDR:
            yy = (char *)OS2gethostbyaddr(wsa->carg1,wsa->iarg1,wsa->iarg2);
            break;
        default:
            yy = (char *)NULL;
            OS2WSASetLastError(-5000);
            break;
    } // end switch

#ifdef DEBUG
    WriteLog("WSOCK32: THREAD id = %lu\n",(unsigned long)id);
    if(yy) {
        ooo = (WSERVENT *)yy;
        WriteLog("WSOCK32: WSAFunct service name = %s\n",ooo->s_name);
        WriteLog("WSOCK32: WSAFunct service port = %d\n",(int)ooo->s_port);
    }
#endif

    wp = id;

    if(yy == (char *)NULL) {
#ifdef DEBUG
        WriteLog("WSOCK32: WSAFunct error\n");
        WriteLog("WSOCK32: WSAFunct error carg1 = %s, carg2 = %s\n",wsa->carg1,wsa->carg2);
#endif
        ii = OS2WSAGetLastError();
        lp = OS2WSAMAKEASYNCREPLY(0,ii);
    } // end if

    else {
        if(wsa->buflen < ss) ii = WSAENOBUFS;
        else ii = 0;
        lp = OS2WSAMAKEASYNCREPLY(ss,ii);
        if(ii == 0) memmove(wsa->buf,yy,ss);
    }

#ifdef DEBUG
#endif

    
    do {
        if(WinQueryAnchorBlock(wsa->hw))
            ii = NotifyWSA(wsa->hw,wsa->msg,wp,lp);
    } while(ii != TRUE);

    free(wsa);

return;
}



    

int WIN32API OS2WSARecvEx (SOCKET s, char FAR * buf, int len, int FAR *flags)
{
#ifdef DEBUG
    WriteLog("WSOCK32: WSARecvEx not implemented.\n");
#endif

//    return WSARecvEx(s,buf,len,flags);
    return 0;
}

void WIN32API OS2s_perror(char *pszMessage,
                          void *pUnknown)
{
  perror(pszMessage);
}


//typedef struct _TRANSMIT_FILE_BUFFERS {
//    PVOID Head;
//    DWORD HeadLength;
//    PVOID Tail;
//    DWORD TailLength;
//} TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, *LPTRANSMIT_FILE_BUFFERS;
//
//BOOL WIN32API OS2TransmitFile (
//    IN SOCKET hSocket,
//    IN HANDLE hFile,
//    IN DWORD nNumberOfBytesToWrite,
//    IN DWORD nNumberOfBytesPerSend,
//    IN LPOVERLAPPED lpOverlapped,
//    IN LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
//    IN DWORD dwReserved)
//{
//    return FALSE;
//}











