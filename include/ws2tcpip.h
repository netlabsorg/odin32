#ifndef _WS2TCPIPODIN_H_
#define _WS2TCPIPODIN_H_

#define WS2_IPPROTO_OFFSET	100

#define WS2_IPPROTO_OPT(a)	(a+WS2_IPPROTO_OFFSET)

#define	IP_OPTIONS_WS2		    1 /* set/get IP options */
#define	IP_HDRINCL_WS2		    2 /* header is included with data */
#define	IP_TOS_WS2		    3 /* IP type of service and preced*/
#define	IP_TTL_WS2		    4 /* IP time to live */
#define	IP_MULTICAST_IF_WS2	    9 /* set/get IP multicast i/f  */
#define	IP_MULTICAST_TTL_WS2       10 /* set/get IP multicast ttl */
#define	IP_MULTICAST_LOOP_WS2      11 /* set/get IP multicast loopback */
#define	IP_ADD_MEMBERSHIP_WS2      12 /* add an IP group membership */
#define	IP_DROP_MEMBERSHIP_WS2     13 /* drop an IP group membership */
#define IP_DONTFRAGMENT_WS2        14 /* don't fragment IP datagrams */

#endif	/* _WS2TCPIP_H_ */



