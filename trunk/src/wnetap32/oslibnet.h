/* $Id: oslibnet.h,v 1.3 2001-09-06 22:23:40 phaller Exp $ */
/*
 * Wrappers for OS/2 Netbios/Network/LAN API
 *
 * Copyright 2000 Patrick Haller (patrick.haller@innotek.de)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
 
#ifndef __OSLIBNET_H__
#define __OSLIBNET_H__


#undef NET_API_STATUS
#define NET_API_STATUS DWORD

#define NERR_OK   0
#define NERR_BASE 1 /* @@@PH DUMMY ! */



// forwarders to NETAPI32 internal APIs
NET_API_STATUS WIN32API OS2NetApiBufferAllocate(DWORD ByteCount,
                                                LPVOID * Buffer);

NET_API_STATUS WIN32API OS2NetApiBufferFree(LPVOID Buffer);

NET_API_STATUS WIN32API OS2NetApiBufferReallocate(LPVOID OldBuffer,
                                                  DWORD NewByteCount,
                                                  LPVOID* NewBuffer);

NET_API_STATUS WIN32API OS2NetApiBufferSize(LPVOID buffer,
                                            LPDWORD lpByteCount);


// forwarders to OS/2 Lan Manager APIs

DWORD OSLibNetWkstaGetInfo (const unsigned char * pszServer,
                    unsigned long         ulLevel,       
                    unsigned char       * pbBuffer,
                    unsigned long         ulBuffer,      
                    unsigned long       * pulTotalAvail);

DWORD OSLibNetStatisticsGet(const unsigned char * pszServer,
                            const unsigned char * pszService,
                            unsigned long         ulReserved,
                            unsigned long         ulLevel,
                            unsigned long         flOptions,
                            unsigned char       * pbBuffer,
                            unsigned long         ulBuffer,
                            unsigned long       * pulTotalAvail);

DWORD OSLibNetServerEnum(const unsigned char * pszServer,
                         unsigned long         ulLevel,
                         unsigned char       * pbBuffer,
                         unsigned long         ulBufferLength,
                         unsigned long       * pulEntriesReturned,
                         unsigned long       * pulEntriesAvail,
                         unsigned long         ulServerType,
                         unsigned char       * pszDomain);

#endif
