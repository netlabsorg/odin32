// $Id: wsock32.h,v 1.13 2000-03-22 20:11:04 sandervl Exp $

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

#ifndef _WINSOCK32CONST_
#define _WINSOCK32CONST_


/*
 * Structure used for manipulating linger option.
 */
struct  Wlinger {
        u_short l_onoff;                /* option on/off */
        u_short l_linger;               /* linger time */
};


/*
 * Structures returned by network data base library, taken from the
 * BSD file netdb.h.  All addresses are supplied in host order, and
 * returned in network order (suitable for use in system calls).
 */

struct  Whostent {
        char     * h_name;           /* official name of host */
        char     *  * h_aliases;  /* alias list */
        short   h_addrtype;             /* host address type */
        short   h_length;               /* length of address */
        char     *  * h_addr_list; /* list of addresses */
#define h_addr  h_addr_list[0]          /* address, for backward compat */
};

/*
 * It is assumed here that a network number
 * fits in 32 bits.
 */
struct  Wnetent {
        char     * n_name;           /* official name of net */
        char     *  * n_aliases;  /* alias list */
        short   n_addrtype;             /* net address type */
        u_long  n_net;                  /* network # */
};
typedef struct Whostent WHOSTENT;
typedef WHOSTENT *PWHOSTENT;

struct  Wservent {
        char     * s_name;           /* official service name */
        char     *  * s_aliases;  /* alias list */
        short   s_port;                 /* port # */
        char     * s_proto;          /* protocol to use */
};
typedef struct Wservent WSERVENT;
typedef WSERVENT *PWSERVENT;

struct  Wprotoent {
        char     * p_name;           /* official protocol name */
        char     *  * p_aliases;  /* alias list */
        short   p_proto;                /* protocol # */
};
typedef struct Wprotoent WPROTOENT;
typedef WPROTOENT *PWPROTOENT;

typedef struct tagWsockThreadData
{
  int              dwLastError; // Get/SetLastError
  struct Whostent  whsnt;       // database conversion buffers
  struct Wservent  wsvnt;
  struct Wprotoent wptnt;
  struct Wnetent   wntnt;
  struct Wlinger   wlinger;
} WSOCKTHREADDATA, *PWSOCKTHREADDATA;

// internal prototype
PWSOCKTHREADDATA iQueryWsockThreadData(void);


extern BOOL fWSAInitialized;

void WIN32API OS2WSASetLastError(int iError);

int  WIN32API OS2WSAGetLastError(void);

BOOL WIN32API OS2WSAIsBlocking(void);

#endif  /* _WINSOCK32CONST_ */


