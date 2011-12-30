/*
 * Additional Winsock 2 definitions - used for ws2_32.dll
 */

#ifndef __WS2TCPIP_H__
#define __WS2TCPIP_H__

#define	IP_OPTIONS		    1 /* set/get IP options */
#define	IP_HDRINCL		    2 /* header is included with data */
#define	IP_TOS		    3 /* IP type of service and preced*/
#define	IP_TTL		    4 /* IP time to live */
#define	IP_MULTICAST_IF	    9 /* set/get IP multicast i/f  */
#define	IP_MULTICAST_TTL       10 /* set/get IP multicast ttl */
#define	IP_MULTICAST_LOOP      11 /* set/get IP multicast loopback */
#define	IP_ADD_MEMBERSHIP      12 /* add an IP group membership */
#define	IP_DROP_MEMBERSHIP     13 /* drop an IP group membership */
#define IP_DONTFRAGMENT        14 /* don't fragment IP datagrams */

#endif	/* __WS2TCPIP_H__ */



