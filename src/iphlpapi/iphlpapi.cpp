/* $Id: iphlpapi.cpp,v 1.14 2003-05-05 15:11:42 sandervl Exp $ */
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
#include <winnls.h>

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

//We don't want to use the OS2 version directly, but the one in wsock32
int WIN32API ODIN_gethostname (char * name, int namelen);

ODINDEBUGCHANNEL(IPHLPAPI-IPHLPAPI)


/****************************************************************************
 * module global variables
 ****************************************************************************/

static PIP_ADAPTER_INFO pipAdapter    = NULL;
static PMIB_IFTABLE     pmibTable     = NULL;
static PMIB_IPADDRTABLE pmipaddrTable = NULL;

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
  int rc,i;
  char *buffer=NULL,*buffer2=NULL;
  struct ifnet *pifnet;
  struct ifmib  ifmibget;
  int cInterfaces;
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

  pmibTable = (PMIB_IFTABLE)malloc(cInterfaces*sizeof(MIB_IFTABLE));
  memset(pmibTable, 0, cInterfaces*sizeof(MIB_IFTABLE));
  pmibTable->dwNumEntries = cInterfaces;

  pmipaddrTable = (PMIB_IPADDRTABLE)malloc(cInterfaces*sizeof(MIB_IPADDRTABLE));
  memset(pmipaddrTable, 0, cInterfaces*sizeof(MIB_IPADDRTABLE));
  pmipaddrTable->dwNumEntries = cInterfaces;

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

    MultiByteToWideChar(CP_ACP, 0, iShortName, strlen(iShortName),
                        pmibTable->table[currentInterface].wszName, 
                        MAX_INTERFACE_NAME_LEN);
  
    pmibTable->table[currentInterface].dwIndex = ifmibget.iftable[i].ifIndex;
    pmibTable->table[currentInterface].dwType  = ifmibget.iftable[i].ifType;         /* type of the interface   */
    pmibTable->table[currentInterface].dwMtu   = ifmibget.iftable[i].ifMtu;          /* MTU of the interface   */
    pmibTable->table[currentInterface].dwSpeed = ifmibget.iftable[i].ifSpeed;

    pmibTable->table[currentInterface].dwPhysAddrLen = 0; //??
    memcpy(pmibTable->table[currentInterface].bPhysAddr, ifmibget.iftable[i].ifPhysAddr, sizeof(ifmibget.iftable[i].ifPhysAddr));

    pmibTable->table[currentInterface].dwAdminStatus = ifmibget.iftable[i].ifOperStatus;
    pmibTable->table[currentInterface].dwOperStatus = (ifmibget.iftable[i].ifOperStatus == IFF_UP) ? MIB_IF_OPER_STATUS_OPERATIONAL : MIB_IF_OPER_STATUS_NON_OPERATIONAL;

    pmibTable->table[currentInterface].dwLastChange = ifmibget.iftable[i].ifLastChange;
    pmibTable->table[currentInterface].dwInOctets   = ifmibget.iftable[i].ifInOctets;
    pmibTable->table[currentInterface].dwInUcastPkts = ifmibget.iftable[i].ifInUcastPkts;
    pmibTable->table[currentInterface].dwInNUcastPkts = ifmibget.iftable[i].ifInNUcastPkts;
    pmibTable->table[currentInterface].dwInDiscards = ifmibget.iftable[i].ifInDiscards;
    pmibTable->table[currentInterface].dwInErrors = ifmibget.iftable[i].ifInErrors;
    pmibTable->table[currentInterface].dwInUnknownProtos = ifmibget.iftable[i].ifInUnknownProtos;
    pmibTable->table[currentInterface].dwOutOctets = ifmibget.iftable[i].ifOutOctets;
    pmibTable->table[currentInterface].dwOutUcastPkts = ifmibget.iftable[i].ifOutUcastPkts;
    pmibTable->table[currentInterface].dwOutNUcastPkts = ifmibget.iftable[i].ifOutNUcastPkts;
    pmibTable->table[currentInterface].dwOutDiscards = ifmibget.iftable[i].ifOutDiscards;
    pmibTable->table[currentInterface].dwOutErrors = ifmibget.iftable[i].ifOutErrors;
//    pmibTable->table[currentInterface].dwOutQLen

    pmibTable->table[currentInterface].dwDescrLen = strlen(ifmibget.iftable[i].ifDescr);
    strncpy((char *)pmibTable->table[currentInterface].bDescr, ifmibget.iftable[i].ifDescr, sizeof(pmibTable->table[currentInterface].bDescr));


    pmipaddrTable->table[currentInterface].dwAddr = ifInfo->IPAddress;
    pmipaddrTable->table[currentInterface].dwMask = ifInfo->netmask;
    pmipaddrTable->table[currentInterface].dwBCastAddr = 0; //??
    pmipaddrTable->table[currentInterface].dwIndex = ifmibget.iftable[i].ifIndex;

;  /* MTU of the interface   */



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

    lSpaceLeft -= dwRequired;
    if (lSpaceLeft >= 0)
    {     
      // @PF revised - this thing works because we currently do not support
      // multi-ip, multi-gateway or multi-DHCP servers lists
      // TODO - add lists support
      memcpy(pTarget, pip, sizeof( IP_ADAPTER_INFO ));
      // point to currentIPAddress 
      ((PIP_ADAPTER_INFO)(pTarget))->CurrentIpAddress = &((PIP_ADAPTER_INFO)(pTarget))->IpAddressList;
      pTarget += sizeof( IP_ADAPTER_INFO );
       
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
  
  ODIN_gethostname(fi->HostName,128);
  strcpy(fi->DomainName,_res.defdname);

  // Fill in DNS Servers 
  fi->CurrentDnsServer = &fi->DnsServerList;   
  dnslist = &fi->DnsServerList;
   
  for (int i = 0; i<_res.nscount; i++)
  {
      if (pdnslist) pdnslist->Next = dnslist;
      sin = (struct sockaddr_in *)&_res.nsaddr_list[i];               
      strcpy(dnslist->IpAddress.String,inet_ntoa(sin->sin_addr));
      dprintf(("IPHLPAPI: GetNetworkParams Adding DNS Server %s",inet_ntoa(sin->sin_addr)));
      pdnslist = dnslist;
      if ( pdnslist == &fi->DnsServerList) dnslist = (PIP_ADDR_STRING)(fi + 1);
      else dnslist += 1;
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

//******************************************************************************
//******************************************************************************
DWORD WIN32API GetIpAddrTable(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize,
                              BOOL bOrder)
{
    DWORD buflen;
    DWORD rc;

    dprintf(("GetIpAddrTable %x %x %d", pIpAddrTable, pdwSize, bOrder));

    if(pdwSize == NULL) {
        rc = ERROR_INVALID_PARAMETER;
        goto end;
    }

    if(pmipaddrTable == NULL)
    {
        // gather the information and save it
        i_initializeAdapterInformation();
    }
    if(pmipaddrTable == NULL)
        return ERROR_NO_DATA;


    buflen = sizeof(MIB_IPADDRTABLE) - sizeof(MIB_IPADDRROW);
    buflen+= pmipaddrTable->dwNumEntries*sizeof(MIB_IPADDRROW);

    if(buflen > *pdwSize) {
        *pdwSize = buflen;
        rc = ERROR_BUFFER_OVERFLOW;
        goto end;
    }
    rc = ERROR_SUCCESS;

    memcpy(pIpAddrTable, pmipaddrTable, buflen);

end:
    dprintf(("GetIpAddrTable returned %d", rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetIfTable(PMIB_IFTABLE pIfTable, PULONG pdwSize, BOOL bOrder)
{
    DWORD buflen;
    DWORD rc;

    dprintf(("GetIfTable %x %x %d", pIfTable, pdwSize, bOrder));

    if(pdwSize == NULL) {
        rc = ERROR_INVALID_PARAMETER;
        goto end;
    }

    if(pmibTable == NULL)
    {
        // gather the information and save it
        i_initializeAdapterInformation();
    }
    if(pmibTable == NULL)
        return ERROR_NO_DATA;


    buflen = sizeof(MIB_IFTABLE) - sizeof(MIB_IFROW);
    buflen+= pmibTable->dwNumEntries*sizeof(MIB_IFROW);

    if(buflen > *pdwSize) {
        *pdwSize = buflen;
        rc = ERROR_BUFFER_OVERFLOW;
        goto end;
    }

    memcpy(pIfTable, pmibTable, buflen);

    rc = ERROR_SUCCESS;
end:
    dprintf(("GetIfTable returned %d", rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetFriendlyIfIndex(DWORD IfIndex)
{
    dprintf(("GetFriendlyIfIndex %d; returns the same index", IfIndex));
    return IfIndex;
}
//******************************************************************************
//******************************************************************************
