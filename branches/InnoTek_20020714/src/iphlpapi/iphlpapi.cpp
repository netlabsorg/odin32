/* $Id: iphlpapi.cpp,v 1.8 2002-04-28 15:46:31 sandervl Exp $ */
/*
 *	IPHLPAPI library
 *
 */


/****************************************************************************
 * includes
 ****************************************************************************/


#include <stdio.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winversion.h>

#include <string.h>
#include "iphlpapi.h"

#define BSD_SELECT 1
#define OS2 1

#include <types.h>
#include <sys\socket.h>
#include <sys\ioctl.h>
#include <net\route.h>
#include <net\if.h>
#include <net\if_arp.h>
#ifdef TCPV40HDRS
#include <netinet\in.h>
#include <arpa\NAMESER.H>
#endif
#include <resolv.h>
#include <unistd.h>

#include "iphlwrap.h"

#pragma pack(1)
typedef struct
{
	unsigned long IPAddress;
	unsigned short interfaceNum;
	unsigned long netmask;
	unsigned long broadcastAddress;
} interfaceInformation;

/* Incomplete but will do for us */

#ifndef TCPV40HDRS
struct ifnet {
        char    *if_name;               /* name, e.g. ``en'' or ``lo'' */
        short   if_unit;                /* sub-unit for lower level driver */
        short   if_mtu;                 /* maximum transmission unit */
};
#endif

struct ilist {
  struct ilist *next;
  unsigned long addr;
};

struct rlist {
  struct rlist *next;
  unsigned long gate;
  unsigned long net;
  unsigned long mask;
  char done;
};

#pragma pack()

ODINDEBUGCHANNEL(IPHLPAPI-IPHLPAPI)


/****************************************************************************
 * module global variables
 ****************************************************************************/

static PIP_ADAPTER_INFO pipAdapter = NULL;


//******************************************************************************
//******************************************************************************

void stringIPAddress(char* dst,u_long data)
{
    sprintf(dst, "%u.%u.%u.%u", 
			(char)data,
			(char)(*(((char*)&data) + 1)),
			(char)(*(((char*)&data) + 2)),
			(char)(*(((char*)&data) + 3)));
}

void stringNetmask(char* dst,u_long data)
{
   sprintf(dst,"%u.%u.%u.%u",
			(char)(*(((char*)&data) + 3)),
			(char)(*(((char*)&data) + 2)),
			(char)(*(((char*)&data) + 1)),
			(char)data);
}

static void i_initializeAdapterInformation(void)
{
  unsigned char *p;
  char iShortName[6];
  struct rlist * rnode = NULL, * rdef = NULL;
  PIP_ADAPTER_INFO oldAdapter = NULL,topAdapter = NULL;
  struct sockaddr_in * sin;
  struct ifmib ifmibget;
  int rc,i;
  char *buffer=NULL,*buffer2=NULL;
  struct ifnet *pifnet;
  short int cInterfaces;
#ifndef TCPV40HDRS
  struct ortentry *r;
#else
  struct rtentry *r;
#endif

  // Init Subsystem and open a socket for ioctl() calls	
  sock_init();

  int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  dprintf(("IPHLPAPI(Init): Opened socket %d\n", clientSocket));

  // Whole buf minimum size is 65536 and memsets are really needed in other case 
  // we will get garbage in adapter names.

  buffer = (char*)malloc(64 * 1024);
  memset(buffer, 0, 65536);
  memset(&ifmibget,0, sizeof(struct ifmib));

  rc = ioctl(clientSocket, SIOSTATIF, (char*)&ifmibget, sizeof(struct ifmib));
  dprintf(("IPHLPAPI(SIOSTATIF) ioctl returned: %d\n", rc));
  if (rc == -1)
  {
    free(buffer);
    soclose(clientSocket);	   
    return; 
  }        

  rc = ioctl(clientSocket, SIOSTATAT, (char*)buffer, 65536);
  dprintf(("IPHLPAPI(SIOSTATAT) ioctl returned: %d\n", rc));
  if (rc == -1)
  {
    free(buffer);
    soclose(clientSocket);	   
    return; 
  }        
  cInterfaces = *(short int*)buffer;
  dprintf(("IPHLPAPI Call returned %d interfaces\n", cInterfaces));

  for (int currentInterface = 0; currentInterface < cInterfaces; currentInterface++)
  {
    interfaceInformation *ifInfo = (interfaceInformation*)(buffer + 2 + (currentInterface * sizeof(interfaceInformation)));
    // For now let's dump only lanX and pppX adapters, loopback (?)
//    if (ifInfo->interfaceNum ==9) continue;

    // Allocate and clear mem 
    pipAdapter = (PIP_ADAPTER_INFO)malloc (sizeof (IP_ADAPTER_INFO));
    memset(pipAdapter, 0, sizeof(IP_ADAPTER_INFO));
    if (oldAdapter) 
       oldAdapter->Next = pipAdapter;

    pipAdapter->Next = NULL;
    pipAdapter->ComboIndex = 1;
    i =  ifInfo->interfaceNum;
    // Gather some other stats 
    dprintf(("IPHLPAPI: interface number: %u\n", ifInfo->interfaceNum));

    if (ifInfo->interfaceNum>=0 && ifInfo->interfaceNum<9) // lanX
       { strcpy(iShortName,"lan"); iShortName[3] = ifInfo->interfaceNum+48;
         iShortName[4] = 0;}

    // I do not like this very much, but seems there is no other documented
    // way

    if (strstr(ifmibget.iftable[i].ifDescr,"back")) // lo
        strcpy(iShortName,"lo");       

    if (strstr(ifmibget.iftable[i].ifDescr,"ace ppp")) // pppX
        strcpy(iShortName,strstr(ifmibget.iftable[i].ifDescr,"ppp"));       

    if (strstr(ifmibget.iftable[i].ifDescr,"ace sl")) // slX
        strcpy(iShortName,strstr(ifmibget.iftable[i].ifDescr,"sl"));       

    if (strstr(ifmibget.iftable[i].ifDescr,"ace dod")) // dodX
        strcpy(iShortName,strstr(ifmibget.iftable[i].ifDescr,"dod"));       

    dprintf(("IPHLPAPI: interface name[%s] : %s\n",iShortName, ifmibget.iftable[i].ifDescr));
    strcpy(pipAdapter->AdapterName, ifmibget.iftable[i].ifDescr);
    strcpy(pipAdapter->Description, ifmibget.iftable[i].ifDescr);
         
    pipAdapter->AddressLength = 6; // MAX address 
    memcpy(pipAdapter->Address,ifmibget.iftable[i].ifPhysAddr,6);
    pipAdapter->Index = ifmibget.iftable[i].ifIndex;
    pipAdapter->Type = ifmibget.iftable[i].ifType; // Careful with this (?)
    pipAdapter->DhcpEnabled = 0; // Also a question
  
    // TODO: Adapter may support multiple IP addrs
    IP_ADDR_STRING iasAdapterIP;
    iasAdapterIP.Next = NULL;
    stringIPAddress((char*)&iasAdapterIP.IpAddress,ifInfo->IPAddress);
    stringNetmask((char*)&iasAdapterIP.IpMask,ifInfo->netmask);
    iasAdapterIP.Context = 0;
 
    // Now we are going to catch gateways for this interface  
    buffer2 = (char*) malloc(64*1024);	 
    memset(buffer2, 0, 65536);
    
    rc = ioctl(clientSocket, SIOSTATRT, (char*)buffer2, 65536);
    dprintf(("IPHLPAPI(SIOSTATRT):ioctl returned: %d\n", rc));

    if (rc == -1) 
    {
      free(buffer);
      free(buffer2);
      soclose(clientSocket);	   
      // better return nothing than some trash, unwinding and freeing
      for (topAdapter; pipAdapter; topAdapter = pipAdapter) {
          pipAdapter = topAdapter -> Next;
          free(topAdapter); 
      }
      return; 
     }        
         
     rtentries *routeEntries = (rtentries*)buffer2;
     p = (unsigned char *)&routeEntries->rttable[0];

     IP_ADDR_STRING iasGateway;    
     memset(&iasGateway,0,sizeof(iasGateway));

     for (int currentRoute = 0; currentRoute < (routeEntries->hostcount+routeEntries->netcount); currentRoute++)
     {
     // First check if this route is for our interface    
#ifndef TCPV40HDRS
         r = (struct ortentry *) (p);
#else
         r = (struct rtentry *) (p);
#endif
         if (strcmp((const char*)(p + sizeof(struct rtentry)),iShortName)==0)
         {
            sin = (struct sockaddr_in *)(&r->rt_dst);
            if (strcmp(inet_ntoa(sin->sin_addr),"0.0.0.0")==0)
            {  
               iasGateway.Next = NULL;
	       // TODO : Some systems may have many gateways	
               sin = (struct sockaddr_in *)(&r->rt_gateway);
               strcpy(iasGateway.IpAddress.String,inet_ntoa(sin->sin_addr));
               sin = (struct sockaddr_in *)&p[-4];
               strcpy(iasGateway.IpMask.String,inet_ntoa(sin->sin_addr));
               iasGateway.Context = 0;
            }
         }
#ifndef TCPV40HDRS
       p+=sizeof(struct ortentry);
#else
       p+=sizeof(struct rtentry);
#endif
       p+=strlen((char *)p)+1;
    }
  
  memcpy((char*)&pipAdapter->IpAddressList, (char*)&iasAdapterIP, sizeof(iasAdapterIP));
  pipAdapter->CurrentIpAddress = &pipAdapter->IpAddressList;
  memcpy((char*)&pipAdapter->GatewayList,   (char*)&iasGateway, sizeof(iasGateway));
  // what about OS/2 DHCP?
  memset((char*)&pipAdapter->DhcpServer, 0, sizeof( IP_ADDR_STRING ) );
  pipAdapter->HaveWins = 0;
  memset((char*)&pipAdapter->PrimaryWinsServer, 0, sizeof( IP_ADDR_STRING ) );
  memset((char*)&pipAdapter->SecondaryWinsServer, 0, sizeof( IP_ADDR_STRING ) );
  pipAdapter->LeaseObtained = 0;
  pipAdapter->LeaseExpires = 0;
  if (!topAdapter) topAdapter = pipAdapter;
  oldAdapter = pipAdapter;
 }
 pipAdapter = topAdapter;

 // Cleanup
 if (buffer) free(buffer);
 if (buffer2) free(buffer2);
 soclose(clientSocket); 
}

// copy over the whole list and advance the target pointer and correct new list
static void i_copyIP_ADDRESS_STRING(PBYTE *ppTarget, PIP_ADDR_STRING pstruct,PIP_ADDR_STRING pias)
{
  PIP_ADDR_STRING dummy = NULL;
  // We already have this copied
  pias = pias -> Next;
  while (pias)
  {
    memcpy(*ppTarget, pias, sizeof( IP_ADDR_STRING ) );
    pstruct->Next = (PIP_ADDR_STRING) *ppTarget;
    *ppTarget += sizeof ( IP_ADDR_STRING );
    pias = pias->Next;
    pstruct = pstruct->Next;
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
  dprintf(("GetAdaptersInfo API called"));
  dprintf(("Address passed is %p",pAdapterInfo)); 
  if (NULL == pOutBufLen)
    return ERROR_INVALID_PARAMETER;
  
  // verify first block of memory to write to
  if (IsBadWritePtr(pAdapterInfo, 4))
    return ERROR_INVALID_PARAMETER;
  
  if (NULL == pipAdapter)
  {
    // gather the information and save it
    i_initializeAdapterInformation();
  }
  
  if (NULL == pipAdapter)
    return ERROR_NO_DATA;
  
  // OK, just copy over the information as far as possible
  LONG  lSpaceLeft     = *pOutBufLen;
  PBYTE pTarget        = (PBYTE)pAdapterInfo;
  PIP_ADAPTER_INFO pip;
  
  // calculate overall required buffer size
  DWORD dwRequiredBuffer = 0;
  
  for( pip = pipAdapter ; pip ; pip = pip->Next )
  {
    // check for sufficient space
    dwRequiredBuffer += i_sizeOfIP_ADAPTER_INFO(pip);
  }
  
  for( pip = pipAdapter ; pip ; pip = pip->Next )
  {
    // check for sufficient space
    DWORD dwRequired = i_sizeOfIP_ADAPTER_INFO(pip);
    
    if (lSpaceLeft - dwRequired > 0)
    {
      lSpaceLeft -= dwRequired;
      
      // copy over the whole structure hierarchy
      memcpy(pTarget, pip, sizeof( IP_ADAPTER_INFO ));
      pTarget += sizeof( IP_ADAPTER_INFO );
       
      // point to currentIPAddress 
      ((PIP_ADAPTER_INFO)(pTarget))->CurrentIpAddress = &((PIP_ADAPTER_INFO)(pTarget))->IpAddressList;
      //@@PF Need to fix lots of stuff here
//      i_copyIP_ADDRESS_STRING(&pTarget, &pip->IpAddressList);
//      i_copyIP_ADDRESS_STRING(&pTarget, &pip->GatewayList);
//      i_copyIP_ADDRESS_STRING(&pTarget, &pip->DhcpServer);
//      i_copyIP_ADDRESS_STRING(&pTarget, &pip->PrimaryWinsServer);
//      i_copyIP_ADDRESS_STRING(&pTarget, &pip->SecondaryWinsServer);
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
  struct sockaddr_in * sin;
  PFIXED_INFO fi = pFixedInfo; 
  DWORD memNeeded;
  PIP_ADDR_STRING dnslist = NULL, pdnslist = NULL;
  PBYTE pTarget        = (PBYTE)pFixedInfo;

  dprintf(("GetNetworkParams pFixedInfo:%x pOutBufLen:%d",pFixedInfo,*pOutBufLen));
  res_init();

  // Check how much mem we will need 
  memNeeded = sizeof(FIXED_INFO)+_res.nscount*sizeof(IP_ADDR_STRING);

  if (((LONG)(*pOutBufLen - memNeeded)) < 0)
  {
   // return overall size of required buffer
   *pOutBufLen = memNeeded;
   return ERROR_BUFFER_OVERFLOW;
  }
  
  // This is dynamically updated info
  memset(pFixedInfo,0,memNeeded);
  
  gethostname(fi->HostName,128);
  strcpy(fi->DomainName,_res.defdname);

  // For VPC DNS Servers are pretty much enough for now
  fi->CurrentDnsServer = &fi->DnsServerList;   
  pTarget += sizeof( FIXED_INFO );
  dnslist = &fi->DnsServerList;
   
  for (int i = 0; i<_res.nscount; i++)
  {
      if (pdnslist) pdnslist->Next = dnslist;
      sin = (struct sockaddr_in *)&_res.nsaddr_list[i];               
      strcpy(dnslist->IpAddress.String,inet_ntoa(sin->sin_addr));
      dprintf(("IPHLPAPI: GetNetworkParams Adding DNS Server %s",inet_ntoa(sin->sin_addr)));
      pdnslist = dnslist;
      if ( pdnslist == &fi->DnsServerList) dnslist = (PIP_ADDR_STRING)(pTarget + sizeof(IP_ADDR_STRING));
      else dnslist += sizeof(IP_ADDR_STRING);
  }	
  fi->EnableDns = 1;
  return ERROR_SUCCESS;
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
