/* $Id: protocol.cpp,v 1.2 2002-03-09 16:07:11 achimha Exp $ */
/*
 * based on Windows Sockets 1.1 specs
 * (ftp.microsoft.com:/Advsys/winsock/spec11/WINSOCK.TXT)
 * 
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 *
 * NOTE: If you make any changes to fix a particular app, make sure 
 * they don't break something else like Netscape or telnet and ftp 
 * clients and servers (www.winsite.com got a lot of those).
 *
 * NOTE 2: Many winsock structs such as servent, hostent, protoent, ...
 * are used with 1-byte alignment for Win16 programs and 4-byte alignment
 * for Win32 programs in winsock.h. winsock2.h uses forced 4-byte alignment.
 * So we have non-forced (just as MSDN) ws_XXXXent (winsock.h), 4-byte forced
 * ws_XXXXent32 (winsock2.h) and 1-byte forced ws_XXXXent16 (winsock16.h).
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <winsock2.h>
#include <debugtools.h>
#include <odinwrap.h>
#include <wchar.h>

//#include "nspapi.h"

ODINDEBUGCHANNEL(WS2_32-PROTOCOL)


#ifdef __WIN32OS2__
#define WS_IPPROTO_TCP IPPROTO_TCP
#define WS_IPPROTO_UDP IPPROTO_UDP
#define WS_AF_NETBIOS AF_NETBIOS
#define WS_AF_INET AF_INET
#define strlenW lstrlenW
#define strcpyW lstrcpyW
#define strncpyW lstrcpynW

#define WS_IPPROTO_NETBIOS_MAGIC 0x4e455442

#endif


/***
 * Predefined protocol structures for the known protocols
 * for OS/2
 ***/

#define myDEFINE_GUID(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
{ l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }


static WSAPROTOCOL_INFOW sProtocol_TcpIp =
{
  0x20066,    // XP_FRAGMENTATION | XP_EXPEDITED_DATA | XP_GRACEFUL_CLOSE | XP_GUARANTEED_ORDER | XP_GUARANTEED_DELIVERY,
  0,          // service flags
  0,
  0,
  0x00000008, // provider flags
  myDEFINE_GUID(0xe70f1aa0, 0xab8b, 0x11cf, 0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92),
  0x000003e9, // catalog entry ID
  {0x00000001,}, // protocol chain
  0,          // version
  WS_AF_INET, // address family
  0,          // max sock addr
  0,          // min sock addr
  SOCK_STREAM,    // socket type
  WS_IPPROTO_TCP, // protocol
  0,          // protocol max. offset
  0,          // network byte order
  0,          // security scheme
  0,          // message size
  0,          // provider reserved
  'M','S','A','F','D',' ','T','c','p','I','p',' ','[','T','C','P','/','I','P',']',']'
};


static WSAPROTOCOL_INFOW sProtocol_UdpIp =
{
  0x20609, // XP_FRAGMENTATION | XP_SUPPORTS_BROADCAST | XP_SUPPORTS_MULTICAST | XP_MESSAGE_ORIENTED | XP_CONNECTIONLESS
  0,          // service flags
  0,
  0,
  0x00000008, // provider flags
  myDEFINE_GUID(0xe70f1aa0, 0xab8b, 0x11cf, 0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92),
  0x000003ea, // catalog entry ID
  {0x00000001,}, // protocol chain
  0,          // version
  WS_AF_INET, // address family
  0,          // max sock addr
  0,          // min sock addr
  SOCK_DGRAM,      // socket type
  WS_IPPROTO_UDP,  // protocol
  0,          // protocol max. offset
  0,          // network byte order
  0,          // security scheme
  65467,      // message size
  0,          // provider reserved
  'M','S','A','F','D',' ','T','c','p','I','p',' ','[','U','D','P','/','I','P',']',']'
};


static WSAPROTOCOL_INFOW sProtocol_NetBtSeq =
{
  0x2000e, 
  0,          // service flags
  0,
  0,
  0x00000008, // provider flags
  myDEFINE_GUID(0x8d5f1830, 0xc273, 0x11cf, 0x95, 0xc8, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92),
  0x000003ee, // catalog entry ID
  {0x00000001,}, // protocol chain
  0,          // version
  WS_AF_NETBIOS, // address family
  0,          // max sock addr
  0,          // min sock addr
  SOCK_SEQPACKET, // socket type
  WS_IPPROTO_NETBIOS_MAGIC,    // protocol (encoded LANA Number)
  0,          // protocol max. offset
  0,          // network byte order
  0,          // security scheme
  64000,      // message size
  0,          // provider reserved
  
  // is completed dynamically,
  // Note: "NetBT" is a signature string, don't change it.
  'M','S','A','F','D',' ','N','e','t','B','I','O','S',' ',
    '[','\\','D','e','v','i','c','e','\\','N','e','t','B','T',
    '_','T','c','p','i','p','%','l','s',' ','S','E','Q','P','A',
    'C','K','E','T',' ','%','d' 
};


static WSAPROTOCOL_INFOW sProtocol_NetBtDgram =
{
  0x20209,
  0,          // service flags
  0,
  0,
  0x00000008, // provider flags
  myDEFINE_GUID(0x8d5f1830, 0xc273, 0x11cf, 0x95, 0xc8, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92),
  0x000003ea, // catalog entry ID
  {0x00000001,}, // protocol chain
  0,          // version
  WS_AF_NETBIOS, // address family
  0,          // max sock addr
  0,          // min sock addr
  SOCK_DGRAM, // socket type
  WS_IPPROTO_NETBIOS_MAGIC,    // protocol (encoded LANA Number)
  0,          // protocol max. offset
  0,          // network byte order
  0,          // security scheme
  64000,      // message size
  0,          // provider reserved
  
  // is completed dynamically,
  // Note: "NetBT" is a signature string, don't change it.
  'M','S','A','F','D',' ','N','e','t','B','I','O','S',' ',
    '[','\\','D','e','v','i','c','e','\\','N','e','t','B','T',
    '_','T','c','p','i','p','%','l','s',' ','D','A','T','A','G',
    'R','A','M',' ','%','d'
};


/*
 * descriptor structure for a LAN adapter
 */
typedef struct _LANAdapter
{
  ULONG LANA;    // Lan adapter number
  ULONG encLANA; // Win32 "encoded" LAN Adapter number
                 // LANA   encoded LANA
                 // ------ ------------
                 // 0      0x80000000
                 // 1      -1
                 // 2      -2
                 // ...    ...
  LPWSTR lpszGUID;    // GUID of adapter driver
} LANADAPTER, *PLANADAPTER;


static LANADAPTER LANAdapter_0 =
{
  0,          // LAN adapter number 0
  0x80000000, // corresponding to adapter 0:
  // myDEFINE_GUID(0xe125fc45, 0x8a8c, 0x4aef, 0xad, 0xc2, 0xe2, 0x51, 0xc2, 0x82, 0x7c, 0x8f)
  // manual specification of GUID saves StringFromCLSID call and OLE32 linkage
  (LPWSTR)L"{E125FC45-8A8C-4AEF-ADC2-E251C2827C8F}"
};


typedef struct _ProtocolBinding
{
  LANADAPTER*         pLANA;
  WSAPROTOCOL_INFOW * pProtocol;
} PROTOCOLBINDING, *PPROTOCOLBINDING;


// this is our sample, static binding setup
// To do: determine this dynamically in i_WSAEnumInitialize
static PROTOCOLBINDING arrBindingInfo[] =
{
  { &LANAdapter_0, &sProtocol_TcpIp },
  { &LANAdapter_0, &sProtocol_UdpIp },
  { &LANAdapter_0, &sProtocol_NetBtSeq },
  { &LANAdapter_0, &sProtocol_NetBtDgram },
  NULL
};


/*****************************************************************************
 * Name      : static int i_WSAEnumInitialize
 * Purpose   : access Protocol Manager and obtain binding information,
 *             in case this fails a standard setup is returned
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : 
 *
 * Author    : Patrick Haller [2002-03-04]
 *****************************************************************************/

static int i_WSAEnumInitialize(void)
{
  // 1 - open protman$
  // 2 - obtain binding information
  // 3 - close protman$
  // 4 - parse and analyse binding info
  //     and build ready-to-eat WSAEnumProtocol pieces
  return 0;
}



/*****************************************************************************
*          WSOCK32_WSAEnterSingleProtocol [internal]
*
*    enters the protocol informations of one given protocol into the
*    given buffer. If the given buffer is too small only the required size for
*    the protocols are returned.
*
* RETURNS
*    The number of protocols entered into the buffer
*
* BUGS
*    - only implemented for IPX, SPX, SPXII, TCP, UDP
*    - there is no check that the operating system supports the returned
*      protocols
*/

// Note @@@PH 2002-03-04
// - we currently do not determine if a specified protocol is really installed,
//   but return a "standard configuration" for adapter 0 instead.
static INT WSOCK32_WSAEnterSingleProtocol(PPROTOCOLBINDING pBinding,
                                          DWORD dwCatalogID,
                                          WSAPROTOCOL_INFOA* lpBuffer,
                                          LPDWORD lpSize, 
                                          BOOL unicode)
{ 
  DWORD  dwLength = 0;
  DWORD  dwOldSize = *lpSize;
  INT    iAnz = 1;
  WCHAR  *lpProtName = NULL;
  BOOL   flagDoCopy = TRUE;
  WCHAR  szBuf[256];
  
  
  if (FALSE == unicode)
    *lpSize = sizeof( WSAPROTOCOL_INFOA );
  else
    *lpSize = sizeof( WSAPROTOCOL_INFOW );
  
  if ( !lpBuffer || !*lpSize || (*lpSize > dwOldSize))
    flagDoCopy = FALSE;
  
  // copy the information
  if (flagDoCopy)
  {
    memcpy(lpBuffer, (const void*)pBinding->pProtocol, *lpSize );
    lpProtName = pBinding->pProtocol->szProtocol;

    // for NETBIOS protocols, recalculate the name
    if( WS_AF_NETBIOS == pBinding->pProtocol->iAddressFamily)
    {
      // recalculate LANA -> iProtocol
      lpBuffer->iProtocol = pBinding->pLANA->encLANA;
      
      swprintf((wchar_t*)szBuf,
               256, // sizeof szBuf
               (const wchar_t*)pBinding->pProtocol->szProtocol,
               pBinding->pLANA->lpszGUID,
               pBinding->pLANA->LANA);
      
      lpProtName = szBuf;
    }
    
    // set catalog entry ID
    lpBuffer->dwCatalogEntryId = dwCatalogID;
    
    // convert and copy the name
    if (unicode)
      strncpyW( (LPWSTR)lpBuffer->szProtocol, lpProtName, WSAPROTOCOL_LEN);
    else
      WideCharToMultiByte( CP_ACP, 0, lpProtName, -1, lpBuffer->szProtocol,
                          WSAPROTOCOL_LEN, NULL, NULL);
  }
  else
    iAnz = 0;
    
  return iAnz;
}


/*****************************************************************************
*          WSOCK32_EnumProtocol [internal]
*
*    Enters the information about installed protocols into a given buffer
*
* RETURNS
*    SOCKET_ERROR if the buffer is to small for the requested protocol infos
*    on success the number of protocols inside the buffer
*
* NOTE
*    NT4SP5 does not return SPX if lpiProtocols == NULL
*
* BUGS
*    - NT4SP5 returns in addition these list of NETBIOS protocols
*      (address family 17), each entry two times one for socket type 2 and 5
*
*      iProtocol   lpProtocol
*      0x80000000  \Device\NwlnkNb
*      0xfffffffa  \Device\NetBT_CBENT7
*      0xfffffffb  \Device\Nbf_CBENT7
*      0xfffffffc  \Device\NetBT_NdisWan5
*      0xfffffffd  \Device\NetBT_El9202
*      0xfffffffe  \Device\Nbf_El9202
*      0xffffffff  \Device\Nbf_NdisWan4
*
*    - there is no check that the operating system supports the returned
*      protocols
*/
static INT WSOCK32_EnumProtocol( LPINT lpiProtocols, 
                                WSAPROTOCOL_INFOA* lpBuffer,
                                LPDWORD lpdwLength, 
                                BOOL unicode)
{
  DWORD dwCurSize, 
        dwOldSize = *lpdwLength;
  DWORD dwSpaceTaken = 0;
  INT   anz = 0;
  INT   rc;
  DWORD dwCatalogID = 1;
  
  PPROTOCOLBINDING pBinding = arrBindingInfo;
  LPINT lpIter;
  
  *lpdwLength = 0;
  
  // iterate over all available protocol bindings
  for(;
      pBinding->pLANA != NULL;
      pBinding++)
  {
    // keep identical catalog IDs everywhere
    dwCatalogID++;
    
    // check if the current protocol is in the "selection list"
    if (lpiProtocols != NULL)
    {
      BOOL fFound = FALSE;
      
      for (lpIter = lpiProtocols;
           *lpIter;
           lpIter++)
      {
        if (pBinding->pProtocol->iProtocol == *lpIter)
        {
          fFound = TRUE;
          break;
        }
      }
      
      // don't add this protocol entry
      if (!fFound)
        continue;
    }
    
    
    // ok, add the current protocol to the list
    
    // calculate how much space is left
    dwCurSize = dwOldSize - dwSpaceTaken;
    
    // determine how much space this will take up
    rc = WSOCK32_WSAEnterSingleProtocol( pBinding, dwCatalogID, &lpBuffer[ anz ], &dwCurSize, unicode);
    if (rc > 0)
    {
      // update pointers if protocol was added
      anz += rc;
      dwSpaceTaken += dwCurSize;
    }
    
    // deliver the number of bytes required to hold ALL information
    *lpdwLength += dwCurSize;
  }
  
  
  // returncode for signalling "more data"
  if (dwOldSize < *lpdwLength) 
    anz = SOCKET_ERROR;
  
  return anz;
}



/*****************************************************************************
 * Name      : int WSAEnumProtocolsA
 * Purpose   : retrieves binding information on protocols and adapters
 * Parameters: lpiProtocols
 *               [in] Null-terminated array of iProtocol values. This 
 *               parameter is optional; if lpiProtocols is NULL, information 
 *               on all available protocols is returned. Otherwise,
 *               information is retrieved only for those protocols listed in 
 *               the array.
 *             lpProtocolBuffer
 *               [out] Buffer that is filled with WSAPROTOCOL_INFOA structures.
 *             lpdwBufferLength
 *               [in, out] On input, the count of bytes in the 
 *               lpProtocolBuffer buffer passed to WSAEnumProtocols. On 
 *               output, the minimum buffer size that can be passed to
 *               WSAEnumProtocols to retrieve all the requested information. 
 *               This routine has no ability to enumerate over multiple calls; 
 *               the passed-in buffer must be large enough to hold all entries 
 *               in order for the routine to succeed. This reduces the 
 *               complexity of the API and should not pose a problem because 
 *               the number of protocols loaded on a computer is typically 
 *               small.
 * Variables :
 * Result    :   If no error occurs, WSAEnumProtocols returns the number of 
 *               protocols to be reported. Otherwise, a value of SOCKET_ERROR 
 *               is returned and a specific error code can be retrieved by 
 *               calling WSAGetLastError.
 * Remark    :
 * Status    : UNTESTED 
 *
 * Author    : Patrick Haller [2002-03-01]
 *****************************************************************************/

ODINFUNCTION3(int, WSAEnumProtocolsA, LPINT, lpiProtocols, 
                                      LPWSAPROTOCOL_INFOA, lpProtocolBuffer,
                                      LPDWORD, lpdwBufferLength)
{
  dprintf(("NOT COMPLETELY IMPLEMENTED"));
  
  return WSOCK32_EnumProtocol(lpiProtocols,
                              lpProtocolBuffer,
                              lpdwBufferLength,
                              FALSE);
}


/*****************************************************************************
 * Name      : int WSAEnumProtocolsW
 * Purpose   : retrieves binding information on protocols and adapters
 * Parameters: lpiProtocols
 *               [in] Null-terminated array of iProtocol values. This 
 *               parameter is optional; if lpiProtocols is NULL, information 
 *               on all available protocols is returned. Otherwise,
 *               information is retrieved only for those protocols listed in 
 *               the array.
 *             lpProtocolBuffer
 *               [out] Buffer that is filled with WSAPROTOCOL_INFOW structures.
 *             lpdwBufferLength
 *               [in, out] On input, the count of bytes in the 
 *               lpProtocolBuffer buffer passed to WSAEnumProtocols. On 
 *               output, the minimum buffer size that can be passed to
 *               WSAEnumProtocols to retrieve all the requested information. 
 *               This routine has no ability to enumerate over multiple calls; 
 *               the passed-in buffer must be large enough to hold all entries 
 *               in order for the routine to succeed. This reduces the 
 *               complexity of the API and should not pose a problem because 
 *               the number of protocols loaded on a computer is typically 
 *               small.
 * Variables :
 * Result    :   If no error occurs, WSAEnumProtocols returns the number of 
 *               protocols to be reported. Otherwise, a value of SOCKET_ERROR 
 *               is returned and a specific error code can be retrieved by 
 *               calling WSAGetLastError.
 * Remark    :
 * Status    : UNTESTED 
 *
 * Author    : Patrick Haller [2002-03-01]
 *****************************************************************************/

ODINFUNCTION3(int, WSAEnumProtocolsW, LPINT, lpiProtocols, 
                                      LPWSAPROTOCOL_INFOW, lpProtocolBuffer,
                                      LPDWORD, lpdwBufferLength)
{
  dprintf(("NOT COMPLETELY IMPLEMENTED"));
  
  return WSOCK32_EnumProtocol(lpiProtocols,
                              (LPWSAPROTOCOL_INFOA)lpProtocolBuffer,
                              lpdwBufferLength,
                              TRUE);
}
//******************************************************************************
//******************************************************************************
