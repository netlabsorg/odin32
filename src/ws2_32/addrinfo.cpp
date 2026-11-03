/*
 * WS2_32: protocol independent name resolution (getaddrinfo & friends) and
 * the text <-> binary address converters inet_pton/inet_ntop.
 *
 * Built on the Winsock 1.1 calls Odin already has (gethostbyname,
 * gethostbyaddr, getservbyname, getservbyport), so only IPv4 results are
 * produced: AF_INET6 requests fail with WSAEAFNOSUPPORT / WSAHOST_NOT_FOUND
 * as on a host without IPv6.  inet_pton/inet_ntop do handle IPv6 text.
 *
 * Current Windows programs (the Java runtime, curl, ...) import these
 * functions directly, so they must exist for such programs to load.
 */
#include <odin.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define A_AF_UNSPEC         0
#define A_AF_INET           2
#define A_AF_INET6          23
#define A_SOCK_STREAM       1
#define A_SOCK_DGRAM        2
#define A_IPPROTO_TCP       6
#define A_IPPROTO_UDP       17

#define A_AI_PASSIVE        0x01
#define A_AI_CANONNAME      0x02
#define A_AI_NUMERICHOST    0x04
#define A_AI_NUMERICSERV    0x08

#define A_NI_NOFQDN         0x01
#define A_NI_NUMERICHOST    0x02
#define A_NI_NAMEREQD       0x04
#define A_NI_NUMERICSERV    0x08
#define A_NI_DGRAM          0x10

#define A_WSAEFAULT          10014
#define A_WSAEINVAL          10022
#define A_WSAEAFNOSUPPORT    10047
#define A_WSAESOCKTNOSUPPORT 10044
#define A_WSATYPE_NOT_FOUND  10109
#define A_WSAHOST_NOT_FOUND  11001
#define A_WSA_NOT_ENOUGH_MEM 8

typedef struct a_sockaddr_in {
    short          sin_family;
    unsigned short sin_port;        /* network order */
    unsigned char  sin_addr[4];
    char           sin_zero[8];
} a_sockaddr_in;                    /* 16 bytes, as sockaddr_in */

typedef struct a_addrinfo {
    int                 ai_flags;
    int                 ai_family;
    int                 ai_socktype;
    int                 ai_protocol;
    unsigned int        ai_addrlen;
    char               *ai_canonname;
    a_sockaddr_in      *ai_addr;
    struct a_addrinfo  *ai_next;
} a_addrinfo;

static unsigned short a_htons(unsigned short v)
{
    return (unsigned short)((v << 8) | (v >> 8));
}

/* strict dotted quad a.b.c.d */
static int parse_ipv4(const char *s, unsigned char *out)
{
    int part = 0;

    for (part = 0; part < 4; part++) {
        int val = 0, digits = 0;

        while (*s >= '0' && *s <= '9') {
            val = val * 10 + (*s - '0');
            if (++digits > 3 || val > 255)
                return 0;
            s++;
        }
        if (digits == 0)
            return 0;
        out[part] = (unsigned char)val;
        if (part < 3) {
            if (*s != '.')
                return 0;
            s++;
        }
    }
    return *s == 0;
}

static a_addrinfo *new_node(const unsigned char *addr, unsigned short port, int socktype,
                            int protocol, int flags, const char *canon)
{
    size_t         canonlen = canon ? strlen(canon) + 1 : 0;
    a_addrinfo    *ai = (a_addrinfo *)calloc(1, sizeof(a_addrinfo) + sizeof(a_sockaddr_in) + canonlen);

    if (!ai)
        return NULL;
    ai->ai_flags    = flags;
    ai->ai_family   = A_AF_INET;
    ai->ai_socktype = socktype;
    ai->ai_protocol = protocol;
    ai->ai_addrlen  = sizeof(a_sockaddr_in);
    ai->ai_addr     = (a_sockaddr_in *)(ai + 1);
    ai->ai_addr->sin_family = A_AF_INET;
    ai->ai_addr->sin_port   = a_htons(port);
    memcpy(ai->ai_addr->sin_addr, addr, 4);
    if (canon) {
        ai->ai_canonname = (char *)(ai->ai_addr + 1);
        memcpy(ai->ai_canonname, canon, canonlen);
    }
    return ai;
}

extern "C" {

void WINAPI freeaddrinfo(a_addrinfo *ai)
{
    while (ai) {
        a_addrinfo *next = ai->ai_next;
        free(ai);               /* node, address and name are one block */
        ai = next;
    }
}

int WINAPI getaddrinfo(const char *node, const char *service, const a_addrinfo *hints, a_addrinfo **res)
{
    int            flags = 0, family = A_AF_UNSPEC, socktype = 0, protocol = 0;
    unsigned short port = 0;
    unsigned char  addrs[16][4];
    int            naddr = 0, i, err;
    const char    *canon = NULL;
    char           canonbuf[256];
    a_addrinfo    *head = NULL, **tail = &head;

    if (!res) {
        WSASetLastError(A_WSAEINVAL);
        return A_WSAEINVAL;
    }
    *res = NULL;
    if ((!node || !*node) && (!service || !*service)) {
        WSASetLastError(A_WSAHOST_NOT_FOUND);
        return A_WSAHOST_NOT_FOUND;
    }
    if (hints) {
        flags    = hints->ai_flags;
        family   = hints->ai_family;
        socktype = hints->ai_socktype;
        protocol = hints->ai_protocol;
    }
    if (family != A_AF_UNSPEC && family != A_AF_INET) {
        err = (family == A_AF_INET6) ? A_WSAHOST_NOT_FOUND : A_WSAEAFNOSUPPORT;
        WSASetLastError(err);
        return err;
    }
    if (socktype != 0 && socktype != A_SOCK_STREAM && socktype != A_SOCK_DGRAM) {
        WSASetLastError(A_WSAESOCKTNOSUPPORT);
        return A_WSAESOCKTNOSUPPORT;
    }

    /* port */
    if (service && *service) {
        const char *p = service;
        int         numeric = 1;

        for (; *p; p++)
            if (*p < '0' || *p > '9')
                numeric = 0;
        if (numeric) {
            long v = atol(service);
            if (v > 65535) {
                WSASetLastError(A_WSATYPE_NOT_FOUND);
                return A_WSATYPE_NOT_FOUND;
            }
            port = (unsigned short)v;
        }
        else {
            struct servent *se = NULL;

            if (!(flags & A_AI_NUMERICSERV)) {
                if (socktype != A_SOCK_DGRAM)
                    se = getservbyname(service, "tcp");
                if (!se && socktype != A_SOCK_STREAM)
                    se = getservbyname(service, "udp");
            }
            if (!se) {
                WSASetLastError(A_WSATYPE_NOT_FOUND);
                return A_WSATYPE_NOT_FOUND;
            }
            port = a_htons((unsigned short)se->s_port);     /* s_port is network order: back to host */
        }
    }

    /* addresses */
    if (!node || !*node) {
        unsigned char a[4] = { 0, 0, 0, 0 };
        if (!(flags & A_AI_PASSIVE)) {
            a[0] = 127; a[3] = 1;
        }
        memcpy(addrs[naddr++], a, 4);
    }
    else if (parse_ipv4(node, addrs[0])) {
        naddr = 1;
    }
    else {
        struct hostent *he;

        if (flags & A_AI_NUMERICHOST) {
            WSASetLastError(A_WSAHOST_NOT_FOUND);
            return A_WSAHOST_NOT_FOUND;
        }
        he = gethostbyname(node);
        if (!he || he->h_addrtype != A_AF_INET || !he->h_addr_list || !he->h_addr_list[0]) {
            err = WSAGetLastError();
            if (err == 0)
                err = A_WSAHOST_NOT_FOUND;
            WSASetLastError(err);
            return err;
        }
        for (i = 0; he->h_addr_list[i] && naddr < 16; i++) {
            memcpy(addrs[naddr++], he->h_addr_list[i], 4);
        }
        if (he->h_name) {
            strncpy(canonbuf, he->h_name, sizeof(canonbuf) - 1);
            canonbuf[sizeof(canonbuf) - 1] = 0;
            canon = canonbuf;
        }
    }
    if ((flags & A_AI_CANONNAME) && !canon && node)
        canon = node;

    /* build the list: per address one entry per socket type */
    for (i = 0; i < naddr; i++) {
        int st;

        for (st = A_SOCK_STREAM; st <= A_SOCK_DGRAM; st++) {
            a_addrinfo *ai;
            int         proto;

            if (socktype != 0 && socktype != st)
                continue;
            proto = protocol ? protocol : (st == A_SOCK_STREAM ? A_IPPROTO_TCP : A_IPPROTO_UDP);
            ai = new_node(addrs[i], port, st, proto, 0,
                          (i == 0 && st == (socktype ? socktype : A_SOCK_STREAM) && (flags & A_AI_CANONNAME)) ? canon : NULL);
            if (!ai) {
                freeaddrinfo(head);
                WSASetLastError(A_WSA_NOT_ENOUGH_MEM);
                return A_WSA_NOT_ENOUGH_MEM;
            }
            *tail = ai;
            tail = &ai->ai_next;
        }
    }
    *res = head;
    return 0;
}

int WINAPI getnameinfo(const a_sockaddr_in *sa, int salen, char *host, DWORD hostlen,
                       char *serv, DWORD servlen, int flags)
{
    unsigned short port;

    if (!sa || salen < (int)sizeof(a_sockaddr_in)) {
        WSASetLastError(A_WSAEFAULT);
        return A_WSAEFAULT;
    }
    if (sa->sin_family != A_AF_INET) {
        WSASetLastError(A_WSAEAFNOSUPPORT);
        return A_WSAEAFNOSUPPORT;
    }
    if (!host && !serv) {
        WSASetLastError(A_WSAEINVAL);
        return A_WSAEINVAL;
    }

    if (host && hostlen) {
        char        text[32];
        const char *name = NULL;

        sprintf(text, "%u.%u.%u.%u", sa->sin_addr[0], sa->sin_addr[1], sa->sin_addr[2], sa->sin_addr[3]);
        if (!(flags & A_NI_NUMERICHOST)) {
            struct hostent *he = gethostbyaddr((const char *)sa->sin_addr, 4, A_AF_INET);
            if (he && he->h_name)
                name = he->h_name;
            else if (flags & A_NI_NAMEREQD) {
                WSASetLastError(A_WSAHOST_NOT_FOUND);
                return A_WSAHOST_NOT_FOUND;
            }
        }
        if (!name)
            name = text;
        if (strlen(name) + 1 > hostlen) {
            WSASetLastError(A_WSAEFAULT);
            return A_WSAEFAULT;
        }
        strcpy(host, name);
    }

    if (serv && servlen) {
        char            text[16];
        const char     *name = NULL;
        struct servent *se = NULL;

        port = a_htons(sa->sin_port);                /* to host order */
        if (!(flags & A_NI_NUMERICSERV))
            se = getservbyport(sa->sin_port, (flags & A_NI_DGRAM) ? "udp" : "tcp");
        if (se && se->s_name)
            name = se->s_name;
        else {
            sprintf(text, "%u", port);
            name = text;
        }
        if (strlen(name) + 1 > servlen) {
            WSASetLastError(A_WSAEFAULT);
            return A_WSAEFAULT;
        }
        strcpy(serv, name);
    }
    return 0;
}

/* ------------------------------------------------------------------------ */

static int parse_ipv6(const char *s, unsigned char *out)
{
    unsigned short words[8];
    int            nwords = 0, gap = -1;
    int            i;

    memset(words, 0, sizeof(words));
    if (s[0] == ':') {
        if (s[1] != ':')
            return 0;
        s++;
    }
    while (*s) {
        if (*s == ':') {
            if (gap >= 0)
                return 0;
            gap = nwords;
            s++;
            if (!*s)
                break;
            continue;
        }
        if (nwords >= 8)
            return 0;
        {
            unsigned long v = 0;
            int           digits = 0;
            const char   *p = s;

            while ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
                v = v * 16 + (*p <= '9' ? *p - '0' : ((*p | 0x20) - 'a' + 10));
                p++;
                if (++digits > 4)
                    break;
            }
            if (*p == '.') {
                /* trailing dotted quad */
                unsigned char q[4];
                if (!parse_ipv4(s, q) || nwords > 6)
                    return 0;
                words[nwords++] = (unsigned short)((q[0] << 8) | q[1]);
                words[nwords++] = (unsigned short)((q[2] << 8) | q[3]);
                s += strlen(s);
                break;
            }
            if (digits == 0 || digits > 4)
                return 0;
            words[nwords++] = (unsigned short)v;
            s = p;
            if (*s == ':') {
                s++;
                if (*s == ':') {
                    if (gap >= 0)
                        return 0;
                    gap = nwords;
                    s++;
                    if (!*s)
                        break;
                }
                else if (!*s)
                    return 0;
            }
            else if (*s)
                return 0;
        }
    }
    if (gap < 0) {
        if (nwords != 8)
            return 0;
    }
    else {
        int fill = 8 - nwords;
        if (fill < 1)
            return 0;
        for (i = nwords - 1; i >= gap; i--)
            words[i + fill] = words[i];
        for (i = gap; i < gap + fill; i++)
            words[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        out[i * 2]     = (unsigned char)(words[i] >> 8);
        out[i * 2 + 1] = (unsigned char)(words[i] & 0xFF);
    }
    return 1;
}

int WINAPI inet_pton(int family, const char *src, void *dst)
{
    if (!src || !dst) {
        WSASetLastError(A_WSAEFAULT);
        return -1;
    }
    if (family == A_AF_INET)
        return parse_ipv4(src, (unsigned char *)dst) ? 1 : 0;
    if (family == A_AF_INET6)
        return parse_ipv6(src, (unsigned char *)dst) ? 1 : 0;
    WSASetLastError(A_WSAEAFNOSUPPORT);
    return -1;
}

const char * WINAPI inet_ntop(int family, const void *src, char *dst, unsigned int size)
{
    char text[64];

    if (!src || !dst) {
        WSASetLastError(A_WSAEFAULT);
        return NULL;
    }
    if (family == A_AF_INET) {
        const unsigned char *a = (const unsigned char *)src;
        sprintf(text, "%u.%u.%u.%u", a[0], a[1], a[2], a[3]);
    }
    else if (family == A_AF_INET6) {
        const unsigned char *a = (const unsigned char *)src;
        unsigned short       w[8];
        int                  i, best = -1, bestlen = 0, cur = -1, curlen = 0;
        char                *p = text;

        for (i = 0; i < 8; i++)
            w[i] = (unsigned short)((a[i * 2] << 8) | a[i * 2 + 1]);
        for (i = 0; i < 8; i++) {                   /* longest run of zero words */
            if (w[i] == 0) {
                if (cur < 0) { cur = i; curlen = 0; }
                if (++curlen > bestlen) { best = cur; bestlen = curlen; }
            }
            else
                cur = -1;
        }
        if (bestlen < 2)
            best = -1;
        for (i = 0; i < 8; i++) {
            if (best >= 0 && i >= best && i < best + bestlen) {
                if (i == best)
                    *p++ = ':';                     /* the run becomes "::" */
                continue;
            }
            if (i != 0)
                *p++ = ':';
            p += sprintf(p, "%x", w[i]);
        }
        if (best >= 0 && best + bestlen == 8)
            *p++ = ':';
        *p = 0;
    }
    else {
        WSASetLastError(A_WSAEAFNOSUPPORT);
        return NULL;
    }
    if (strlen(text) + 1 > size) {
        WSASetLastError(A_WSAEFAULT);
        return NULL;
    }
    strcpy(dst, text);
    return dst;
}

} // extern "C"
