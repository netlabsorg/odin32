/* $Id: uuid.cpp,v 1.3 2000-04-06 11:54:03 bird Exp $ */
/*
 * RPCRT4 library
 * UUID manipulation
 * Partially based on OSF sources (see below)
 *
 * 2000/02/05
 *
 * Copyright 2000 David J. Raison
 *           2000 Edgar Buerkle
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 *
 * (c) Copyright 1989 OPEN SOFTWARE FOUNDATION, INC.
 * (c) Copyright 1989 HEWLETT-PACKARD COMPANY
 * (c) Copyright 1989 DIGITAL EQUIPMENT CORPORATION
 * To anyone who acknowledges that this file is provided "AS IS"
 * without any express or implied warranty:
 *                 permission to use, copy, modify, and distribute this
 * file for any purpose is hereby granted without fee, provided that
 * the above copyright notices and this notice appears in all source
 * code copies, and that none of the names of Open Software
 * Foundation, Inc., Hewlett-Packard Company, or Digital Equipment
 * Corporation be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Neither Open Software Foundation, Inc., Hewlett-
 * Packard Company, nor Digital Equipment Corporation makes any
 * representations about the suitability of this software for any
 * purpose.
 *
 */


#define TCPV40HDRS
#include "rpcrt4.h"
#include "uuidp.h"

#include "io.h"
#include "sys/timeb.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#undef interface
#include <net/if.h>
#include <netinet/if_ether.h>
#include <nerrno.h>
#include <sys/time.h>

ULONG WINAPI RtlExtendedIntegerMultiply(LARGE_INTEGER factor1, INT factor2);
ULONG WINAPI RtlLargeIntegerAdd(LARGE_INTEGER arg1, LARGE_INTEGER arg2);

extern "C" ULONG getEAX();
extern "C" ULONG getEDX();

static UUID		uuid_nil = { 0 };
static unsigned char	uuid_addr[6];

// ----------------------------------------------------------------------
// UuidInit
// Init the Uuid subsystem (i.e. retrieve & cache the eth. addr)
// ----------------------------------------------------------------------
extern	void	UuidInit(void )
{
    struct ifmib	ifm = {0};
    int			fFound = FALSE;
    int    		sd;
    int			ii;

    // Ensure that uuid_nil is initialised.
    memset(&uuid_nil, 0, sizeof(uuid_nil));

    // retrieve & cache the eth. addr

    /* BSD 4.4 defines the size of an ifreq to be
     * max(sizeof(ifreq), sizeof(ifreq.ifr_name)+ifreq.ifr_addr.sa_len
     * However, under earlier systems, sa_len isn't present, so
     * the size is just sizeof(struct ifreq)
     */
    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sd > 0)
    {
	if (ioctl(sd, SIOSTATIF42, (char *)&ifm, sizeof(ifm)) >= 0)
	{
	    for(ii = 0; ii < ifm.ifNumber; ii++)
	    {
		if (!ifm.iftable[ii].ifPhysAddr[0] && !ifm.iftable[ii].ifPhysAddr[1] &&
		    !ifm.iftable[ii].ifPhysAddr[2] && !ifm.iftable[ii].ifPhysAddr[3] &&
		    !ifm.iftable[ii].ifPhysAddr[4] && !ifm.iftable[ii].ifPhysAddr[5])
		    continue;
		fFound = TRUE;
		memcpy(uuid_addr, (unsigned char *)&ifm.iftable[ii].ifPhysAddr, 6);
	    }
	}
	close(sd);
    }

    if (!fFound)
    {
	/* if we can't find the physical net address use random numbers */
	/* set the multicast bit to prevent conflicts with real cards */

	uuid_addr[0] = (rand() & 0xff) | 0x80;
	uuid_addr[1] = rand() & 0xff;
	uuid_addr[2] = rand() & 0xff;
	uuid_addr[3] = rand() & 0xff;
	uuid_addr[4] = rand() & 0xff;
	uuid_addr[5] = rand() & 0xff;
    }
}

// ----------------------------------------------------------------------
// gettimeofday
// ----------------------------------------------------------------------
static int
__gettimeofday(struct timeval* tp, void* tzp)
{
    struct timeb tb;

    ftime(&tb);
    tp->tv_sec = tb.time;
    tp->tv_usec = tb.millitm * 1000;

    return 0;
}

// ----------------------------------------------------------------------
// UuidCreate
// Implemented according the DCE specification for UUID generation.
// Code is based upon uuid library in e2fsprogs by Theodore Ts'o.
// Copyright (C) 1996, 1997 Theodore Ts'o.
//
// Returns
//  S_OK if successful.
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
UuidCreate (
    OUT UUID __RPC_FAR * pUuid
    )
{
    static int                  adjustment = 0;
    static struct timeval       last = {0, 0};
    static UINT16               clock_seq;
    struct timeval              tv;
    LARGE_INTEGER               clock_reg={0};
    LARGE_INTEGER               clock_reg_tmp={0};
    UINT   			clock_high;
    UINT			clock_low;
    UINT16			temp_clock_seq;
    UINT16			temp_clock_mid;
    UINT16			temp_clock_hi_and_version;
    BOOL			got_no_time;

    dprintf(("RPCRT4: %s", __FUNCTION__));

    /* The OS/2 gettimeofday has a >1ms granularity (~17 bps) */
    #define MAX_ADJUSTMENT 10000

    do
    {
	got_no_time = FALSE;

	__gettimeofday(&tv, 0);
	if ((last.tv_sec == 0) && (last.tv_usec == 0))
	{
	    clock_seq = ((rand() & 0xff) << 8) + (rand() & 0xff);
	    clock_seq &= 0x1FFF;
	    last = tv;
	    last.tv_sec--;
	}
	if ((tv.tv_sec < last.tv_sec) || ((tv.tv_sec == last.tv_sec) &&
	   (tv.tv_usec < last.tv_usec)))
	{
	    clock_seq = (clock_seq+1) & 0x1FFF;
	    adjustment = 0;
	}
	else if ((tv.tv_sec == last.tv_sec) && (tv.tv_usec == last.tv_usec))
	{
	    if (adjustment >= MAX_ADJUSTMENT)
	    {
		got_no_time = TRUE;
		Sleep(1);	// Allow someone else to run.
	    }
	    else
		adjustment++;
	}
	else
	{
	    adjustment = 0;
	    last = tv;      // ><EB missing in wine and maybe(not tested) in ef2prog
	}

    } while(got_no_time);

    clock_reg_tmp.LowPart = tv.tv_usec*10 + adjustment;
    clock_reg_tmp.HighPart = tv.tv_sec;

    RtlExtendedIntegerMultiply(clock_reg_tmp, 10000000);
    clock_reg.LowPart = getEAX();
    clock_reg.HighPart = getEDX();

    RtlLargeIntegerAdd(clock_reg, clock_reg_tmp );
    clock_reg.LowPart = getEAX();
    clock_reg.HighPart = getEDX();

    clock_reg_tmp.LowPart = 0x13814000;
    clock_reg_tmp.HighPart = 0x01B21DD2;
    RtlLargeIntegerAdd(clock_reg, clock_reg_tmp );
    clock_reg.LowPart = getEAX();
    clock_reg.HighPart = getEDX();

    clock_high = clock_reg.HighPart;
    clock_low = clock_reg.LowPart;

    temp_clock_seq = clock_seq | 0x8000;
    temp_clock_mid = (UINT16)clock_high;
    temp_clock_hi_and_version = (clock_high >> 16) | 0x1000;

    /* pack the information into the GUID structure */
    ((unsigned char*)&pUuid->Data1)[3] = (unsigned char)clock_low;
    clock_low >>= 8;
    ((unsigned char*)&pUuid->Data1)[2] = (unsigned char)clock_low;
    clock_low >>= 8;
    ((unsigned char*)&pUuid->Data1)[1] = (unsigned char)clock_low;
    clock_low >>= 8;
    ((unsigned char*)&pUuid->Data1)[0] = (unsigned char)clock_low;

    ((unsigned char*)&pUuid->Data2)[1] = (unsigned char)temp_clock_mid;
    temp_clock_mid >>= 8;
    ((unsigned char*)&pUuid->Data2)[0] = (unsigned char)temp_clock_mid;

    ((unsigned char*)&pUuid->Data3)[1] = (unsigned char)temp_clock_hi_and_version;
    temp_clock_hi_and_version >>= 8;
    ((unsigned char*)&pUuid->Data3)[0] = (unsigned char)temp_clock_hi_and_version;

    ((unsigned char*)pUuid->Data4)[1] = (unsigned char)temp_clock_seq;
    temp_clock_seq >>= 8;
    ((unsigned char*)pUuid->Data4)[0] = (unsigned char)temp_clock_seq;

    ((unsigned char*)pUuid->Data4)[2] = uuid_addr[0];
    ((unsigned char*)pUuid->Data4)[3] = uuid_addr[1];
    ((unsigned char*)pUuid->Data4)[4] = uuid_addr[2];
    ((unsigned char*)pUuid->Data4)[5] = uuid_addr[3];
    ((unsigned char*)pUuid->Data4)[6] = uuid_addr[4];
    ((unsigned char*)pUuid->Data4)[7] = uuid_addr[5];

    return S_OK;
}

// ----------------------------------------------------------------------
// UuidCreateNil
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
UuidCreateNil (
    OUT UUID __RPC_FAR * NilUuid
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));

    memset (NilUuid, 0, sizeof (uuid_t));

    return RPC_S_OK;
}


// ----------------------------------------------------------------------
// UuidToStringA
// Memory allocated here should be released via RpcStringFreeA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
UuidToStringA (
    IN UUID __RPC_FAR * Uuid,
    OUT unsigned char __RPC_FAR * __RPC_FAR * StringUuid
    )
{
    char *	pString;

    dprintf(("RPCRT4: %s", __FUNCTION__));

    // jic
    *StringUuid = 0;

    if ((pString = (char *)HeapAlloc(GetProcessHeap(), 0, 40)) == NULL)
	return RPC_S_OUT_OF_MEMORY;

    // Setup new string...
    sprintf(pString, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
	    Uuid->Data1,
	    Uuid->Data2,
	    Uuid->Data3,
	    Uuid->Data4[0],
	    Uuid->Data4[1],
	    Uuid->Data4[2],
	    Uuid->Data4[3],
	    Uuid->Data4[4],
	    Uuid->Data4[5],
	    Uuid->Data4[6],
	    Uuid->Data4[7]);

    *StringUuid = (unsigned char *)pString;

    return RPC_S_OK;
}


// ----------------------------------------------------------------------
// UuidFromStringA
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
UuidFromStringA (
    IN unsigned char __RPC_FAR * StringUuid,
    OUT UUID __RPC_FAR * Uuid
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));

    // Convert to binary CLSID
    char *s = (char *) StringUuid;
    char *p;
    int   i;
    char table[256];

    /* quick lookup table */
    memset(table, 0, 256);

    for (i = 0; i < 10; i++)
    {
	table['0' + i] = i;
    }
    for (i = 0; i < 6; i++)
    {
	table['A' + i] = i+10;
	table['a' + i] = i+10;
    }

    /* in form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} */

    if (lstrlenA(s) != 38)
	return RPC_S_INVALID_STRING_UUID;

    p = (char *) Uuid;

    s++;  /* skip leading brace  */
    for (i = 0; i < 4; i++)
    {
	p[3 - i] = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    p += 4;
    s++;  /* skip - */

    for (i = 0; i < 2; i++)
    {
	p[1-i] = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    p += 2;
    s++;  /* skip - */

    for (i = 0; i < 2; i++)
    {
	p[1-i] = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    p += 2;
    s++;  /* skip - */

    /* these are just sequential bytes */
    for (i = 0; i < 2; i++)
    {
	*p++ = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }
    s++;  /* skip - */

    for (i = 0; i < 6; i++)
    {
	*p++ = table[*s]<<4 | table[*(s+1)];
	s += 2;
    }

    return RPC_S_OK;
}


// ----------------------------------------------------------------------
// UuidToStringW
// Memory allocated here should be released via RpcStringFreeW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
UuidToStringW (
    IN UUID __RPC_FAR * Uuid,
    OUT unsigned short __RPC_FAR * __RPC_FAR * StringUuid
    )
{
    RPC_STATUS		rc;
    unsigned char *	pStringA;
    WCHAR *		pStringW;
    size_t		strLen;

    dprintf(("RPCRT4: %s", __FUNCTION__));

    // jic
    *StringUuid = 0;

    // Setup new string...
    if ((rc = UuidToStringA(Uuid, &pStringA)) != RPC_S_OK)
	return rc;	// Boom...

    strLen = strlen((char *)pStringA);

    // Grab buffer for string...
    if ((pStringW = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, (strLen + 1) * sizeof(WCHAR))) == NULL)
	rc = RPC_S_OUT_OF_MEMORY;
    else
    {
	AsciiToUnicode((char *)pStringA, pStringW);
	*StringUuid = (LPOLESTR)pStringW;
	rc = RPC_S_OK;
    }

    // Free the ASCII string
    RpcStringFreeA(&pStringA);

    return rc;
}


// ----------------------------------------------------------------------
// UuidFromStringW
// ----------------------------------------------------------------------
RPCRTAPI RPC_STATUS RPC_ENTRY
UuidFromStringW (
    IN unsigned short __RPC_FAR * StringUuid,
    OUT UUID __RPC_FAR * Uuid
    )
{
    unsigned char	tmp[40];

    dprintf(("RPCRT4: %s", __FUNCTION__));

    UnicodeToAscii((LPWSTR)StringUuid, (char *)tmp);
    return UuidFromStringA(tmp, Uuid);
}


// ----------------------------------------------------------------------
// UuidCompare
// ----------------------------------------------------------------------
RPCRTAPI signed int RPC_ENTRY
UuidCompare (
    IN UUID __RPC_FAR * Uuid1,
    IN UUID __RPC_FAR * Uuid2,
    OUT RPC_STATUS __RPC_FAR * Status
    )
{
    int		ii;

    dprintf(("RPCRT4: %s", __FUNCTION__));

    /*
     * check to see if either of the arguments is a NULL pointer
     * - if so, compare the other argument to the nil Uuid
     */
    if (Uuid1 == NULL)
    {
        /*
         * if both arguments are NULL, so is this routine
         */
        if (Uuid2 == NULL)
        {
            *Status = RPC_S_OK;
            return (0);
        }

        return (UuidIsNil (Uuid2, Status) ? 0 : -1);
    }

    if (Uuid2 == NULL)
    {
        return (UuidIsNil (Uuid1, Status) ? 0 : 1);
    }
    *Status = RPC_S_OK;

    if (Uuid1->Data1 == Uuid2->Data1)
    {
        if (Uuid1->Data2 == Uuid2->Data2)
        {
            if (Uuid1->Data3 == Uuid2->Data3)
            {
                if (Uuid1->Data4[0] == Uuid2->Data4[0])
                {
                    if (Uuid1->Data4[1] == Uuid2->Data4[1])
                    {
                        for (ii = 2; ii < 8; ii++)
                        {
                            if (Uuid1->Data4[ii] < Uuid2->Data4[ii])
                                return (-1);
                            if (Uuid1->Data4[ii] > Uuid2->Data4[ii])
                                return (1);
                        }
                        return (0);
                    }       /* end if - clock_seq_low */
                    else
                    {
                        if (Uuid1->Data4[1] < Uuid2->Data4[1])
                            return (-1);
                        else
                            return (1);
                    }       /* end else - clock_seq_low */
                }           /* end if - clock_seq_hi_and_reserved */
                else
                {
                    if (Uuid1->Data4[0] < Uuid2->Data4[0])
                        return (-1);
                    else
                        return (1);
                }           /* end else - clock_seq_hi_and_reserved */
            }               /* end if - time_hi_and_version */
            else
            {
                if (Uuid1->Data3 < Uuid2->Data3)
                    return (-1);
                else
                    return (1);
            }               /* end else - time_hi_and_version */
        }                   /* end if - time_mid */
        else
        {
            if (Uuid1->Data2 < Uuid2->Data2)
                return (-1);
            else
                return (1);
        }                   /* end else - time_mid */
    }                       /* end if - time_low */
    else
    {
        if (Uuid1->Data1 < Uuid2->Data1)
            return (-1);
        else
            return (1);
    }                       /* end else - time_low */
}


// ----------------------------------------------------------------------
// UuidEqual
// ----------------------------------------------------------------------
RPCRTAPI int RPC_ENTRY
UuidEqual (
    IN UUID __RPC_FAR * Uuid1,
    IN UUID __RPC_FAR * Uuid2,
    OUT RPC_STATUS __RPC_FAR * Status
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));
    *Status = RPC_S_OK;
    return IsEqualGUID(Uuid1, Uuid2);
}


// ----------------------------------------------------------------------
// UuidHash
// ----------------------------------------------------------------------
RPCRTAPI unsigned short RPC_ENTRY
UuidHash (
    IN UUID __RPC_FAR * Uuid,
    OUT RPC_STATUS __RPC_FAR * Status
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));

    short               c0, c1;
    short               x, y;
    char *              next_uuid;

    /*
     * initialize counters
     */
    c0 = c1 = 0;
    next_uuid = (char *) Uuid;

    /*
     * For speed lets unroll the following loop:
     *
     *   for (i = 0; i < UUID_K_LENGTH; i++)
     *   {
     *       c0 = c0 + *next_uuid++;
     *       c1 = c1 + c0;
     *   }
     */
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;

    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;

    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;

    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;
    c0 = c0 + *next_uuid++;
    c1 = c1 + c0;

    /*
     *  Calculate the value for "First octet" of the hash
     */
    x = -c1 % 255;
    if (x < 0)
    {
        x = x + 255;
    }

    /*
     *  Calculate the value for "second octet" of the hash
     */
    y = (c1 - c0) % 255;
    if (y < 0)
    {
        y = y + 255;
    }

    /*
     * return the pieces put together
     */
    *Status = RPC_S_OK;

    return ((y * 256) + x);
}


// ----------------------------------------------------------------------
// UuidIsNil
// ----------------------------------------------------------------------
RPCRTAPI int RPC_ENTRY
UuidIsNil (
    IN UUID __RPC_FAR * Uuid,
    OUT RPC_STATUS __RPC_FAR * Status
    )
{
    dprintf(("RPCRT4: %s", __FUNCTION__));

    *Status = RPC_S_OK;

    return IsEqualGUID(Uuid, &uuid_nil);
}

