/* $Id: socketodin.cpp,v 1.1 2001-10-10 17:19:34 sandervl Exp $ */

#include <odin.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <debugtools.h>
#include <misc.h>

ODINDEBUGCHANNEL(WS2_32-SOCKETODIN)

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(int,WS2setsockopt,
              SOCKET,s,
              int,level,
              int,optname,
              const char *,optval,
              int,optlen)
{
   int ret;

   if(level == IPPROTO_IP) {
       switch(optname) {
       case IP_OPTIONS_WS2:	     /* set/get IP options */
       case IP_HDRINCL_WS2:	     /* header is included with data */
       case IP_TOS_WS2:		     /* IP type of service and preced*/
       case IP_TTL_WS2:	    	     /* IP time to live */
       case IP_MULTICAST_IF_WS2:     /* set/get IP multicast i/f  */
       case IP_MULTICAST_TTL_WS2:    /* set/get IP multicast ttl */
       case IP_MULTICAST_LOOP_WS2:   /* set/get IP multicast loopback */
       case IP_ADD_MEMBERSHIP_WS2:   /* add an IP group membership */
       case IP_DROP_MEMBERSHIP_WS2:  /* drop an IP group membership */
       case IP_DONTFRAGMENT_WS2:     /* don't fragment IP datagrams */
           optname += WS2_IPPROTO_OFFSET;
           break;

       default:
           dprintf(("setsockopt: IPPROTO_IP, unknown option %x", optname));
       	   WSASetLastError(WSAENOPROTOOPT);
      	   return SOCKET_ERROR;
       }
   }
   ret = setsockopt(s, level, optname, (char *)optval, optlen);
   return ret;
}
//******************************************************************************
//******************************************************************************
