/* $Id */
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

DWORD OSLibNetWkstaGetInfo (const unsigned char * pszServer,
                    unsigned long         ulLevel,       
                    unsigned char       * pbBuffer,
                    unsigned long         ulBuffer,      
                    unsigned long       * pulTotalAvail);

#endif
