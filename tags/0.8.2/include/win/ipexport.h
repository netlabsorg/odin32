/* $Id: ipexport.h,v 1.1 2000-05-19 18:44:45 jeroen Exp $ */

/* IPEXPORT.H */

#ifndef IP_EXPORT_H
#define IP_EXPORT_H

typedef ULONG  IPAddr;
typedef ULONG  IPMask;
typedef ULONG  IP_STATUS;

typedef struct ip_option_information {
    u_char      Ttl;
    u_char      Tos;
    u_char      Flags;
    u_char      OptionsSize;
    u_char     *OptionsData;
} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

typedef struct icmp_echo_reply {
    IPAddr                        Address;
    u_long                        Status;
    u_long                        RoundTripTime;
    u_short                       DataSize;
    u_short                       Reserved;
    void                         *Data;
    struct ip_option_information  Options;
} ICMP_ECHO_REPLY, *PICMP_ECHO_REPLY;

#define IP_STATUS_BASE              11000

#define IP_SUCCESS                  0
#define IP_BUF_TOO_SMALL            (IP_STATUS_BASE + 1)
#define IP_DEST_NET_UNREACHABLE     (IP_STATUS_BASE + 2)
#define IP_DEST_HOST_UNREACHABLE    (IP_STATUS_BASE + 3)
#define IP_DEST_PROT_UNREACHABLE    (IP_STATUS_BASE + 4)
#define IP_DEST_PORT_UNREACHABLE    (IP_STATUS_BASE + 5)
#define IP_NO_RESOURCES             (IP_STATUS_BASE + 6)
#define IP_BAD_OPTION               (IP_STATUS_BASE + 7)
#define IP_HW_ERROR                 (IP_STATUS_BASE + 8)
#define IP_PACKET_TOO_BIG           (IP_STATUS_BASE + 9)
#define IP_REQ_TIMED_OUT            (IP_STATUS_BASE + 10)
#define IP_BAD_REQ                  (IP_STATUS_BASE + 11)
#define IP_BAD_ROUTE                (IP_STATUS_BASE + 12)
#define IP_TTL_EXPIRED_TRANSIT      (IP_STATUS_BASE + 13)
#define IP_TTL_EXPIRED_REASSEM      (IP_STATUS_BASE + 14)
#define IP_PARAM_PROBLEM            (IP_STATUS_BASE + 15)
#define IP_SOURCE_QUENCH            (IP_STATUS_BASE + 16)
#define IP_OPTION_TOO_BIG           (IP_STATUS_BASE + 17)
#define IP_BAD_DESTINATION          (IP_STATUS_BASE + 18)
#define IP_ADDR_DELETED             (IP_STATUS_BASE + 19)
#define IP_SPEC_MTU_CHANGE          (IP_STATUS_BASE + 20)
#define IP_MTU_CHANGE               (IP_STATUS_BASE + 21)
#define IP_UNLOAD                   (IP_STATUS_BASE + 22)
#define IP_ADDR_ADDED               (IP_STATUS_BASE + 23)
#define IP_GENERAL_FAILURE          (IP_STATUS_BASE + 50)
#define MAX_IP_STATUS                IP_GENERAL_FAILURE
#define IP_PENDING                  (IP_STATUS_BASE + 255)


#define IP_FLAG_DF                  0x2

#define IP_OPT_EOL                  0
#define IP_OPT_NOP                  1
#define IP_OPT_SECURITY             0x82
#define IP_OPT_LSRR                 0x83
#define IP_OPT_SSRR                 0x89
#define IP_OPT_RR                   0x7
#define IP_OPT_TS                   0x44
#define IP_OPT_SID                  0x88

#define MAX_OPT_SIZE                40

#endif                                 /* IP_EXPORT_H                      */
