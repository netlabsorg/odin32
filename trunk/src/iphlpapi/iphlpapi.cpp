/* $Id: iphlpapi.cpp,v 1.2 2001-10-10 16:22:19 phaller Exp $ */
/*
 *	IPHLPAPI library
 *
 */


/****************************************************************************
 * includes
 ****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <odinwrap.h>
#include <winversion.h>

#include "iphlpapi.h"

ODINDEBUGCHANNEL(IPHLPAPI-IPHLPAPI)


//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD,            GetAdaptersInfo,
              PIP_ADAPTER_INFO, pAdapterInfo,
              PULONG,           pOutBufLen)
{
  dprintf(("GetAdaptersInfo not implemented"));
  
  static PIP_ADAPTER_INFO pipAdapters = NULL;
  static ULONG ulAdaptersSize = 0;
  
  if (NULL == pipAdapters)
  {
    // gather the information and save it
    
    // determine number of IP adapters (interfaces) in the system
    
    // os2_ioctl() SIOSTATIF42
  }
  
  // OK, just copy over the information as far as possible
  
  return ERROR_SUCCESS;
}


//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD,       GetNetworkParams,
              PFIXED_INFO, pFixedInfo,
              PULONG,      pOutBufLen)
{
  dprintf(("GetNetworkParams not implemented"));
  return ERROR_NOT_SUPPORTED; //NT returns this 
}
//******************************************************************************
//******************************************************************************


DWORD AddIPAddress(IPAddr Address, // IP address to add
                   IPMask IpMask, // subnet mask for IP address
                   DWORD IfIndex, // index of adapter
                   PULONG NTEContext, // Net Table Entry context
                   PULONG NTEInstance // Net Table Entry Instance
                  );
// SIOCAIFADDR

DWORD DeleteIPAddress(
                      ULONG NTEContext // net table entry context
                     );
// SIOCDIFADDR
