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

#define IP_OPTIONS_OS2              1    /* buf/ip_opts; set/get IP options */
#define IP_MULTICAST_IF_OS2         2    /* u_char; set/get IP multicast i/f  */
#define IP_MULTICAST_TTL_OS2        3    /* u_char; set/get IP multicast ttl */
#define IP_MULTICAST_LOOP_OS2       4    /* u_char; set/get IP multicast loopback */
#define IP_ADD_MEMBERSHIP_OS2       5    /* ip_mreq; add an IP group membership */
#define IP_DROP_MEMBERSHIP_OS2      6    /* ip_mreq; drop an IP group membership */
#define IP_HDRINCL_OS2              7    /* int; header is included with data */
#define IP_TOS_OS2                  8    /* int; IP type of service and preced. */
#define IP_TTL_OS2                  9    /* int; IP time to live */
#define IP_RECVOPTS_OS2             10   /* bool; receive all IP opts w/dgram */
#define IP_RECVRETOPTS_OS2          11   /* bool; receive IP opts for response */
#define IP_RECVDSTADDR_OS2          12   /* bool; receive IP dst addr w/dgram */
#define IP_RETOPTS_OS2              13   /* ip_opts; set/get IP options */
#define IP_RECVTRRI_OS2             14   /* bool; receive token ring routing inf */

#endif	/* _WS2TCPIP_H_ */



