/* $Id: iphlpapi.cpp,v 1.5 2001-10-29 13:37:29 phaller Exp $ */
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
#include <winversion.h>

#include <string.h>

#include "iphlpapi.h"

ODINDEBUGCHANNEL(IPHLPAPI-IPHLPAPI)


/****************************************************************************
 * module global variables
 ****************************************************************************/

static PIP_ADAPTER_INFO pipAdapters = NULL;


//******************************************************************************
//******************************************************************************
static void i_initializeAdapterInformation(void)
{
  // @@@PH
  // yet a fake to test some app
  pipAdapters = (PIP_ADAPTER_INFO)malloc (sizeof (IP_ADAPTER_INFO) );
  
  memset(pipAdapters, 0, sizeof( IP_ADAPTER_INFO ));
  pipAdapters->Next = NULL;
  pipAdapters->ComboIndex = 1;
  strcpy(pipAdapters->AdapterName, "ODIN IPHLPAPI Test Adapter");
  strcpy(pipAdapters->Description, "ODIN IPHLPAPI Test Adapter (faked information)");
  pipAdapters->AddressLength = 6; // MAX address
  pipAdapters->Address[0] = 'V';
  pipAdapters->Address[1] = 'P';
  pipAdapters->Address[2] = 'C';
  pipAdapters->Address[3] = 'O';
  pipAdapters->Address[4] = 'S';
  pipAdapters->Address[5] = '2';
  pipAdapters->Index = 16777218;
  pipAdapters->Type = 6;
  pipAdapters->DhcpEnabled = 0;
  
  static IP_ADDR_STRING iasLocalhost;
  iasLocalhost.Next = NULL;
  strcpy((char*)&iasLocalhost.IpAddress,"127.0.0.1");
  strcpy((char*)&iasLocalhost.IpMask, "255.0.0.0");
  iasLocalhost.Context = 0;
  
  static IP_ADDR_STRING iasGateway;
  iasGateway.Next = NULL;
  strcpy((char*)&iasGateway.IpAddress,"192.168.1.1");
  strcpy((char*)&iasGateway.IpMask, "255.255.255.0");
  iasGateway.Context = 0;
  
  
  memcpy((char*)&pipAdapters->IpAddressList, (char*)&iasLocalhost, sizeof(iasLocalhost));
  pipAdapters->CurrentIpAddress = &pipAdapters->IpAddressList;
  memcpy((char*)&pipAdapters->GatewayList,   (char*)&iasGateway, sizeof(iasGateway));
  memset((char*)&pipAdapters->DhcpServer, 0, sizeof( IP_ADDR_STRING ) );
  pipAdapters->HaveWins = 0;
  memset((char*)&pipAdapters->PrimaryWinsServer, 0, sizeof( IP_ADDR_STRING ) );
  memset((char*)&pipAdapters->SecondaryWinsServer, 0, sizeof( IP_ADDR_STRING ) );
  pipAdapters->LeaseObtained = 0;
  pipAdapters->LeaseExpires = 0;
}

// copy over the whole list and advance the target pointer
static void i_copyIP_ADDRESS_STRING(PBYTE *ppTarget, PIP_ADDR_STRING pias)
{
  while (pias)
  {
    memcpy(*ppTarget, pias, sizeof( IP_ADDR_STRING ) );
    *ppTarget += sizeof ( IP_ADDR_STRING );
    pias = pias->Next;
  }
}

static DWORD i_sizeOfIP_ADAPTER_INFO(PIP_ADAPTER_INFO piai)
{
  PIP_ADDR_STRING pias;
  
  // check for sufficient space
  DWORD dwRequired = sizeof( IP_ADAPTER_INFO );
    
  // follow the IP_ADDR_STRING lists
  pias = &piai->IpAddressList;
  while( pias )
  {
    dwRequired += sizeof( IP_ADDR_STRING );
    pias = pias->Next;
  }

  pias = &piai->GatewayList;
  while( pias )
  {
    dwRequired += sizeof( IP_ADDR_STRING );
    pias = pias->Next;
  }

  pias = &piai->DhcpServer;
  while( pias )
  {
    dwRequired += sizeof( IP_ADDR_STRING );
    pias = pias->Next;
  }
    
  pias = &piai->PrimaryWinsServer;
  while( pias )
  {
    dwRequired += sizeof( IP_ADDR_STRING );
    pias = pias->Next;
  }

  pias = &piai->SecondaryWinsServer;
  while( pias )
  {
    dwRequired += sizeof( IP_ADDR_STRING );
    pias = pias->Next;
  }
  
  return dwRequired;
}


//******************************************************************************
//******************************************************************************

// Note: returns error 50 under NT4 (NOT_SUPPORTED)
// so we better check out alternative ways, too.

ODINFUNCTION2(DWORD,            GetAdaptersInfo,
              PIP_ADAPTER_INFO, pAdapterInfo,
              PULONG,           pOutBufLen)
{
  dprintf(("GetAdaptersInfo incorrectly implemented"));
  
  if (NULL == pOutBufLen)
    return ERROR_INVALID_PARAMETER;
  
  // verify first block of memory to write to
  if (IsBadWritePtr(pAdapterInfo, 4))
    return ERROR_INVALID_PARAMETER;
  
  if (NULL == pipAdapters)
  {
    // gather the information and save it
    i_initializeAdapterInformation();
    
    // determine number of IP adapters (interfaces) in the system
    
    // os2_ioctl() SIOSTATIF42
  }
  
  if (NULL == pipAdapters)
    return ERROR_NO_DATA;
  
  // OK, just copy over the information as far as possible
  LONG  lSpaceLeft     = *pOutBufLen;
  PBYTE pTarget        = (PBYTE)pAdapterInfo;
  PIP_ADAPTER_INFO pip;
  
  // calculate overall required buffer size
  DWORD dwRequiredBuffer = 0;
  
  for( pip = pipAdapters ; pip ; pip = pip->Next )
  {
    // check for sufficient space
    dwRequiredBuffer += i_sizeOfIP_ADAPTER_INFO(pip);
  }
  
  for( pip = pipAdapters ; pip ; pip = pip->Next )
  {
    // check for sufficient space
    DWORD dwRequired = i_sizeOfIP_ADAPTER_INFO(pip);
    
    if (lSpaceLeft - dwRequired > 0)
    {
      lSpaceLeft -= dwRequired;
      
      // copy over the whole structure hierarchy
      memcpy(pTarget, pip, sizeof( IP_ADAPTER_INFO ));
      pTarget += sizeof( IP_ADAPTER_INFO );
      
      // @@@PH shall point to somewhere within the current buffer
      pip->CurrentIpAddress = (PIP_ADDR_STRING)pTarget;
      
      i_copyIP_ADDRESS_STRING(&pTarget, &pip->IpAddressList);
      i_copyIP_ADDRESS_STRING(&pTarget, &pip->GatewayList);
      i_copyIP_ADDRESS_STRING(&pTarget, &pip->DhcpServer);
      i_copyIP_ADDRESS_STRING(&pTarget, &pip->PrimaryWinsServer);
      i_copyIP_ADDRESS_STRING(&pTarget, &pip->SecondaryWinsServer);
    }
    else
    {
      // return overall size of required buffer
      *pOutBufLen = dwRequiredBuffer;
      return ERROR_BUFFER_OVERFLOW;
    }
  }
  
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
