/* $Id: iphlpapi.cpp,v 1.15 2003-05-05 15:26:03 sandervl Exp $ */
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
#include <iprtrmib.h>
#include <winnls.h>

#define BSD_SELECT 1
#define OS2 1

#include <types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_arp.h>
#ifdef TCPV40HDRS
#include <netinet/in.h>
#include <arpa/NAMESER.H>
#endif
#include <resolv.h>
#include <unistd.h>

#include "iphlwrap.h"

/* from ipexport.h */
typedef ULONG  IPAddr;
typedef ULONG  IPMask;
typedef ULONG  IP_STATUS;

#pragma pack(1)

typedef struct
{
    unsigned long IPAddress;
    unsigned short interfaceIndex;
    unsigned long netmask;
    unsigned long broadcastAddress;
}
AddrInfo;

#pragma pack()

//We don't want to use the OS2 version directly, but the one in wsock32
int WIN32API OS2gethostname (char * name, int namelen);

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
            data & 0xFF,
            (data >> 8) & 0xFF,
            (data >> 16) & 0xFF,
            (data >> 24) & 0xFF);
}

static void i_initializeAdapterInformation(void)
{
    char iShortName[8];
    PIP_ADAPTER_INFO oldAdapter = NULL, topAdapter = NULL;
    int rc;
    char *buffer = NULL, *buffer2 = NULL;
    struct ifmib  ifmibget;
    int cAddresses;
    AddrInfo *addrInfo;
    struct rtentries *rtentries;

    // Init Subsystem and open a socket for ioctl() calls
    sock_init();

    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    dprintf(("IPHLPAPI: Init: Opened socket %d\n", clientSocket));

    // Whole buf minimum size is 65536 and memsets are really needed in other case
    // we will get garbage in adapter names.

    memset(&ifmibget,0, sizeof(struct ifmib));
    buffer = (char*) malloc(65536);
    memset(buffer, 0, 65536);
    buffer2 = (char*) malloc(65536);
    memset(buffer2, 0, 65536);

    rc = ioctl(clientSocket, SIOSTATIF, (char*)&ifmibget, sizeof(struct ifmib));
    dprintf(("IPHLPAPI: ioctl(SIOSTATIF) returned: %d\n", rc));
    if (rc == -1)
    {
        free(buffer2);
        free(buffer);
        soclose(clientSocket);
        return;
    }
    dprintf(("IPHLPAPI: ioctl(SIOSTATIF) returned %d interfaces\n", ifmibget.ifNumber));

    rc = ioctl(clientSocket, SIOSTATAT, buffer, 65536);
    dprintf(("IPHLPAPI: ioctl(SIOSTATAT) returned: %d\n", rc));
    if (rc == -1)
    {
        free(buffer2);
        free(buffer);
        soclose(clientSocket);
        return;
    }
    cAddresses = *(short int *) buffer;
    addrInfo = (AddrInfo *) (buffer + sizeof(short int));
    dprintf(("IPHLPAPI: ioctl(SIOSTATAT) returned %d addresses\n", cAddresses));

    rc = ioctl(clientSocket, SIOSTATRT, buffer2, 65536);
    dprintf(("IPHLPAPI: ioctl(SIOSTATRT) returned: %d\n", rc));
    if (rc == -1)
    {
        free(buffer2);
        free(buffer);
        soclose(clientSocket);
        return;
    }
    rtentries = (struct rtentries *) buffer2;
    dprintf(("IPHLPAPI: ioctl(SIOSTATRT) returned %d host and %d net routes\n",
             rtentries->hostcount, rtentries->netcount));

    pmibTable = (PMIB_IFTABLE) malloc(ifmibget.ifNumber * sizeof(MIB_IFTABLE));
    memset(pmibTable, 0, ifmibget.ifNumber * sizeof(MIB_IFTABLE));
    pmibTable->dwNumEntries = ifmibget.ifNumber;

    pmipaddrTable = (PMIB_IPADDRTABLE) malloc(cAddresses * sizeof(MIB_IPADDRTABLE));
    memset(pmipaddrTable, 0, cAddresses * sizeof(MIB_IPADDRTABLE));
    pmipaddrTable->dwNumEntries = cAddresses;

    // loop over interfaces
    int idx, i;
    for (i = idx = 0; i < IFMIB_ENTRIES && idx < ifmibget.ifNumber; ++i)
    {
        // skip empty interface entries
        if (ifmibget.iftable[i].ifType == 0)
            continue;

        short ifIndex = ifmibget.iftable[i].ifIndex;
        dprintf(("IPHLPAPI: interface index: %u\n", ifIndex));

        // Guess the symbolic interface name. I do not like this very much, but
        // seems there is no other documented way

        if (ifIndex >= 0 && ifIndex < 9) // lanX
        {
            strcpy(iShortName,"lan");
            iShortName[3] = ifIndex + 48;
            iShortName[4] = 0;
        }
        else
        if (strstr(ifmibget.iftable[i].ifDescr, "back")) // loopback
        {
            strcpy(iShortName,"lo");
        }
        else
        if (strstr(ifmibget.iftable[i].ifDescr, "ace ppp")) // pppX
        {
            strcpy(iShortName, strstr(ifmibget.iftable[i].ifDescr, "ppp"));
        }
        else
        if (strstr(ifmibget.iftable[i].ifDescr,"ace sl")) // slX
        {
            strcpy(iShortName,strstr(ifmibget.iftable[i].ifDescr, "sl"));
        }
        else
        if (strstr(ifmibget.iftable[i].ifDescr,"ace dod")) // dodX
        {
            strcpy(iShortName,strstr(ifmibget.iftable[i].ifDescr, "dod"));
        }
        else // something else...
        {
            strcpy(iShortName,"unk");
            iShortName[3] = ifIndex + 48;
            iShortName[4] = 0;
        }

        dprintf(("IPHLPAPI: interface name: %s [%s]\n", iShortName,
                 ifmibget.iftable[i].ifDescr));

        // Allocate the adapter info entry
        pipAdapter = (PIP_ADAPTER_INFO)malloc (sizeof(IP_ADAPTER_INFO));
        memset(pipAdapter, 0, sizeof(IP_ADAPTER_INFO));
        if (oldAdapter)
           oldAdapter->Next = pipAdapter;

        // Fill the adapter info entry
        pipAdapter->Next = NULL;
        pipAdapter->ComboIndex = 1; // unused according to MSDN
        strcpy(pipAdapter->AdapterName, ifmibget.iftable[i].ifDescr);
        strcpy(pipAdapter->Description, ifmibget.iftable[i].ifDescr);

        pipAdapter->AddressLength = sizeof(ifmibget.iftable[i].ifPhysAddr);
        memcpy(pipAdapter->Address,ifmibget.iftable[i].ifPhysAddr, sizeof(ifmibget.iftable[i].ifPhysAddr));
        pipAdapter->Index = ifIndex;
        pipAdapter->Type = ifmibget.iftable[i].ifType; // Careful with this (?)

        // what about OS/2 DHCP?
        pipAdapter->DhcpEnabled = 0; // Also a question
        memset((char*)&pipAdapter->DhcpServer, 0, sizeof(IP_ADDR_STRING));

        pipAdapter->HaveWins = 0;
        memset((char*)&pipAdapter->PrimaryWinsServer, 0, sizeof(IP_ADDR_STRING));
        memset((char*)&pipAdapter->SecondaryWinsServer, 0, sizeof(IP_ADDR_STRING));
        pipAdapter->LeaseObtained = 0;
        pipAdapter->LeaseExpires = 0;

        // Fill the interface table entry
        MultiByteToWideChar(CP_ACP, 0, iShortName, strlen(iShortName),
                            pmibTable->table[idx].wszName,
                            MAX_INTERFACE_NAME_LEN);

        pmibTable->table[idx].dwIndex = ifIndex;
        pmibTable->table[idx].dwType  = ifmibget.iftable[i].ifType;
        pmibTable->table[idx].dwMtu   = ifmibget.iftable[i].ifMtu;
        pmibTable->table[idx].dwSpeed = ifmibget.iftable[i].ifSpeed;

        pmibTable->table[idx].dwPhysAddrLen = sizeof(ifmibget.iftable[i].ifPhysAddr);
        memcpy(pmibTable->table[idx].bPhysAddr, ifmibget.iftable[i].ifPhysAddr,
               sizeof(ifmibget.iftable[i].ifPhysAddr));

        pmibTable->table[idx].dwAdminStatus =
            (ifmibget.iftable[i].ifOperStatus == IFF_UP) ?
            MIB_IF_ADMIN_STATUS_UP : MIB_IF_ADMIN_STATUS_DOWN;
        pmibTable->table[idx].dwOperStatus =
            (ifmibget.iftable[i].ifOperStatus == IFF_UP) ?
            MIB_IF_OPER_STATUS_OPERATIONAL : MIB_IF_OPER_STATUS_NON_OPERATIONAL;

        pmibTable->table[idx].dwLastChange = ifmibget.iftable[i].ifLastChange;
        pmibTable->table[idx].dwInOctets   = ifmibget.iftable[i].ifInOctets;
        pmibTable->table[idx].dwInUcastPkts = ifmibget.iftable[i].ifInUcastPkts;
        pmibTable->table[idx].dwInNUcastPkts = ifmibget.iftable[i].ifInNUcastPkts;
        pmibTable->table[idx].dwInDiscards = ifmibget.iftable[i].ifInDiscards;
        pmibTable->table[idx].dwInErrors = ifmibget.iftable[i].ifInErrors;
        pmibTable->table[idx].dwInUnknownProtos = ifmibget.iftable[i].ifInUnknownProtos;
        pmibTable->table[idx].dwOutOctets = ifmibget.iftable[i].ifOutOctets;
        pmibTable->table[idx].dwOutUcastPkts = ifmibget.iftable[i].ifOutUcastPkts;
        pmibTable->table[idx].dwOutNUcastPkts = ifmibget.iftable[i].ifOutNUcastPkts;
        pmibTable->table[idx].dwOutDiscards = ifmibget.iftable[i].ifOutDiscards;
        pmibTable->table[idx].dwOutErrors = ifmibget.iftable[i].ifOutErrors;
        pmibTable->table[idx].dwOutQLen = 0; // unused according to MSDN

        pmibTable->table[idx].dwDescrLen = strlen(ifmibget.iftable[i].ifDescr);
        strncpy((char *)pmibTable->table[idx].bDescr, ifmibget.iftable[i].ifDescr,
                sizeof(pmibTable->table[idx].bDescr));

        // fill pipAdapter->IpAddressList
        int cIfAddresses = 0;
        int j;
        for (j = 0; j < cAddresses; ++j)
        {
#ifdef DEBUG
            if (i == 0) // print only once
            {
                dprintf(("IPHLPAPI: ADDR %d:", j));
                dprintf(("IPHLPAPI:   IPAddress         0x%08X", addrInfo[j].IPAddress));
                dprintf(("IPHLPAPI:   interfaceIndex    %d", addrInfo[j].interfaceIndex));
                dprintf(("IPHLPAPI:   netmask           0x%08X", addrInfo[j].netmask));
                dprintf(("IPHLPAPI:   broadcastAddress  0x%08X", addrInfo[j].broadcastAddress));
            }
#endif
            if (addrInfo[j].interfaceIndex == ifIndex)
            {
                ++cIfAddresses;

                IP_ADDR_STRING *addr;
                if (cIfAddresses == 1)
                {
                    addr = &pipAdapter->IpAddressList;
                }
                else
                {
                    addr->Next = (IP_ADDR_STRING*) malloc(sizeof(IP_ADDR_STRING));
                    addr = addr->Next;
                }

                memset((char *) addr, 0, sizeof(IP_ADDR_STRING));
                addr->Next = NULL;
                stringIPAddress(addr->IpAddress.String, addrInfo[j].IPAddress);
                // mask is in network byte order for some reason
                stringIPAddress(addr->IpMask.String, ntohl(addrInfo[j].netmask));
                addr->Context = 0;
            }
        }

        // fill pipAdapter->GatewayList
        int cIfGateways = 0;
        struct rtentry *rtentry = rtentries->rttable;
        for (j = 0; j < rtentries->hostcount + rtentries->netcount; ++j)
        {
#ifdef DEBUG
            if (i == 0) // print only once
            {
                dprintf(("IPHLPAPI: RTENTRY %d:", j));
                dprintf(("IPHLPAPI:   rt_hash     0x%08X", rtentry->rt_hash));
                dprintf(("IPHLPAPI:   rt_dst      0x%08X", ((struct sockaddr_in *)(&rtentry->rt_dst))->sin_addr.s_addr));
                dprintf(("IPHLPAPI:   rt_gateway  0x%08X", ((struct sockaddr_in *)(&rtentry->rt_gateway))->sin_addr.s_addr));
                dprintf(("IPHLPAPI:   rt_flags    0x%08X", rtentry->rt_flags));
                dprintf(("IPHLPAPI:   rt_refcnt   %d", rtentry->rt_refcnt));
                dprintf(("IPHLPAPI:   rt_use      %d", rtentry->rt_use));
                dprintf(("IPHLPAPI:   rt_ifp      0x%p", rtentry->rt_ifp));
                //dprintf(("IPHLPAPI:     if_name   %s", rtentry->rt_ifp->if_name));
                dprintf(("IPHLPAPI:   metric1     %d", rtentry->metric1));
                dprintf(("IPHLPAPI:   metric2     %d", rtentry->metric2));
                dprintf(("IPHLPAPI:   metric3     %d", rtentry->metric3));
                dprintf(("IPHLPAPI:   metric4     %d", rtentry->metric4));
            }
#endif
            // only take default gateways for this interface
#if 1
            // rtentry->rt_ifp is (always?) a high address (0xFxxxxxxx) and for
            // some reason reading it causes an access violation on some systems
            // while works great on other systems. I don't know why. Luckily
            // (luckily???), the definition of struct rtentries in headers is
            // wrong -- each entry in the rtentries::rttable array is
            // additionally followed by an ASCIIZ string containing the
            // interface name. And the interface name is what we need. Owse.
            char *if_name = (char *)(rtentry + 1);
            dprintf(("IPHLPAPI:   if_name     %s", if_name));

            if (strcmp(if_name, iShortName) == 0 &&
#else
            if (strcmp(rtentry->rt_ifp->if_name, iShortName) == 0 &&
#endif
                ((struct sockaddr_in *)(&rtentry->rt_dst))->sin_addr.s_addr == 0)
            {
                ++cIfGateways;

                IP_ADDR_STRING *addr;
                if (cIfGateways == 1)
                {
                    addr = &pipAdapter->GatewayList;
                }
                else
                {
                    addr->Next = (IP_ADDR_STRING *) malloc(sizeof(IP_ADDR_STRING));
                    addr = addr->Next;
                }

                memset((char *) addr, 0, sizeof(IP_ADDR_STRING));
                addr->Next = NULL;
                struct sockaddr_in * sin =
                    (struct sockaddr_in *)(&rtentry->rt_gateway);
                strcpy((char *) &addr->IpAddress.String, inet_ntoa(sin->sin_addr));
                strcpy((char *) &addr->IpMask.String, "255.255.255.255");
                addr->Context = 0;
            }

            // Compensate for the interface name following the rtentry
            ++rtentry;
            rtentry = (struct rtentry *)
                (((char *) rtentry) + strlen(((char *) rtentry)) + 1);
        }


        // loop over
        if (!topAdapter)
            topAdapter = pipAdapter;
        oldAdapter =
            pipAdapter;

        ++idx;
    }
    pipAdapter = topAdapter;

    // loop over addressees to fill pmipaddrTable
    for (i = 0; i < cAddresses; ++i)
    {
        // skip addresses referring to empty interface entries
        if (addrInfo[i].interfaceIndex >= IFMIB_ENTRIES ||
            ifmibget.iftable[addrInfo[i].interfaceIndex].ifType == 0)
            continue;

        pmipaddrTable->table[i].dwAddr = addrInfo[i].IPAddress;
        pmipaddrTable->table[i].dwIndex = addrInfo[i].interfaceIndex;
        // mask is in network byte order for some reason
        pmipaddrTable->table[i].dwMask = ntohl(addrInfo[i].netmask);
        pmipaddrTable->table[i].dwBCastAddr = addrInfo[i].broadcastAddress;
        pmipaddrTable->table[i].dwReasmSize = 0; // ?
    }

    // current address is the first address so far
    pipAdapter->CurrentIpAddress = &pipAdapter->IpAddressList;

    // Cleanup
    soclose(clientSocket);
    free(buffer2);
    free(buffer);
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

  OS2gethostname(fi->HostName,128);
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
