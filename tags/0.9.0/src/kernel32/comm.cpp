/* $Id: comm.cpp,v 1.9 2002-02-09 12:45:11 sandervl Exp $ */

/*
 * Comport functions
 *
 * Copyright 1998 Patrick Haller (?)
 * Copyright 1998 Felix Maschek  (?)
 * Copyright 2000 Markus Montkowski
 *
 * Partly based on Wine code (BuildCommDCBAndTimeoutsA; misc\comm.c)
 * 
 * Copyright 1996 Marcus Meissner, Erik Bos
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <odinwrap.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "winreg.h"
#include "global.h"
#include "winnt.h"
#include "winerror.h"
#include "winreg.h"

#include "unicode.h"

#include "handlemanager.h"
#include "hmhandle.h"
#include "hmcomm.h"


#define DBG_LOCALLOG  DBG_comm
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-COMM)

static BOOL HMCommGetDefaultCommConfig( HANDLE hCommDev,
                                        LPCOMMCONFIG lpCC,
                                        LPDWORD lpdwSize);
static BOOL HMCommSetDefaultCommConfig( HANDLE hCommDev,
                                        LPCOMMCONFIG lpCC,
                                        DWORD dwSize);

/*****************************************************************************
 * @returns   True on success and fills the COMMCONFIG structure
 * @param     lpDef        Pointer to device-control string
 * @param     lpDCB        Pointer to device-control buffer
 * @remark
 * @status    untested
 * @author    Markus Montkowski
 *****************************************************************************/

BOOL WIN32API BuildCommDCBA( LPCSTR lpDef, LPDCB lpDCB )
{
  return BuildCommDCBAndTimeoutsA(lpDef,lpDCB,NULL);
}

//------------------------------------------------------------------------------

BOOL WIN32API BuildCommDCBW( LPCWSTR lpDef, LPDCB lpDCB )
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpDef);
  rc = BuildCommDCBAndTimeoutsA(asciiname, lpDCB, NULL);
  FreeAsciiString(asciiname);
  return(rc);
}

//------------------------------------------------------------------------------

BOOL ValidCOMPort(int Port)
{
  // @@@ Todo check in the handlemanager for a registered COM DeviceHandle for this number
  //     We currently only do static add COM1-COM8 so we simply check for 8
  return (Port <=8);
}

/*****************************************************************************
 * @returns   True on success and fills the COMMCONFIG structure
 * @param     lpDef          Pointer to device-control string
 * @param     lpDCB          Pointer to device-control buffer
 * @param     lpCommTimeouts Pointer to COMMTIMEOUTS
 * @remark
 * @status    partly implemented
 * @author    Markus Montkowski
 *****************************************************************************/


BOOL WIN32API BuildCommDCBAndTimeoutsA( LPCSTR lpDef, LPDCB lpDCB, LPCOMMTIMEOUTS lpCommTimeouts )
{
  int port,i;
  char szNumber[4];
  char  *ptr,*temp;

  dprintf(("(%s,%p,%p)\n",lpDef,lpDCB,lpCommTimeouts));

  port = -1;

  if (!strnicmp(lpDef,"COM",3))
  {

    for(i=0;((lpDef[3+i]!=':') && (i<3));i++)
      szNumber[i] = lpDef[3+i];
    szNumber[i] = 0;

    port = atoi(szNumber);
    if (port==0 || lpDef[i]!=':')
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (!ValidCOMPort(port))
    {
      SetLastError(ERROR_FILE_NOT_FOUND);
      return FALSE;
    }
    temp=(LPSTR)(lpDef+4+i);
  }
  else
    temp=(LPSTR)lpDef;

  lpDCB->DCBlength  = sizeof(DCB);
  if (strchr(temp,','))
  {
    // old style

    char  last=temp[strlen(temp)-1];

    ptr = strtok(temp, ", ");


    lpDCB->BaudRate = atoi(ptr);
    dprintf(("baudrate (%d)\n", lpDCB->BaudRate));

    ptr = strtok(NULL, ", ");
    if (islower(*ptr))
      *ptr = toupper(*ptr);

    dprintf(("parity (%c)\n", *ptr));
    lpDCB->fParity = TRUE;
    switch (*ptr)
    {
      case 'N':
        lpDCB->Parity = NOPARITY;
        lpDCB->fParity = FALSE;
        break;
      case 'E':
        lpDCB->Parity = EVENPARITY;
        break;
      case 'M':
        lpDCB->Parity = MARKPARITY;
        break;
      case 'O':
        lpDCB->Parity = ODDPARITY;
        break;
      default:
        SetLastError(ERROR_INVALID_PARAMETER);
        dprintf(("Unknown parity `%c'!\n", *ptr));
        return FALSE;
    }

    ptr = strtok(NULL, ", ");
    dprintf(("charsize (%c)\n", *ptr));
    lpDCB->ByteSize = *ptr - '0';

    if((lpDCB->ByteSize<5) ||
       (lpDCB->ByteSize>8))
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      dprintf(("Unsupported bytesize `%d'!\n", lpDCB->ByteSize));
      return FALSE;
    }

    ptr = strtok(NULL, ", ");
    dprintf(("stopbits (%c%c%c)\n", *ptr,*(ptr+1)=='.'?'.':' ',*(ptr+1)=='.'?*(ptr+2):' '));
    switch (*ptr)
    {
      case '1':
        if(*(ptr+1)=='.')
        {
          if(*(ptr+2)=='5')
            lpDCB->StopBits = ONE5STOPBITS;
          else
          {
            SetLastError(ERROR_INVALID_PARAMETER);
            dprintf(("Unsupported # of stopbits !\n"));
            return FALSE;
          }
        }
        else
          lpDCB->StopBits = ONESTOPBIT;
        break;
      case '2':
        lpDCB->StopBits = TWOSTOPBITS;
        break;
      default:
        SetLastError(ERROR_INVALID_PARAMETER);
        dprintf(("Unknown # of stopbits `%c'!\n", *ptr));
        return FALSE;
    }

    if(lpDCB->BaudRate==110)
      lpDCB->StopBits =2;

    if(((lpDCB->ByteSize==5)&&(lpDCB->StopBits==TWOSTOPBITS))||
       ((lpDCB->ByteSize!=5)&&(lpDCB->StopBits==ONE5STOPBITS)) )
    {
      dprintf(("Unsupported Combination of Bytesize `%d' and StopBits %s!\n",
              lpDCB->ByteSize,lpDCB->StopBits==ONE5STOPBITS?"1.5":"2"));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    lpDCB->fBinary    = TRUE;
    lpDCB->fNull      = FALSE;

    if (last == 'x')
    {
      lpDCB->fInX   = TRUE;
      lpDCB->fOutX    = TRUE;
      lpDCB->fOutxCtsFlow = FALSE;
      lpDCB->fOutxDsrFlow = FALSE;
      lpDCB->fDtrControl  = DTR_CONTROL_ENABLE;
      lpDCB->fRtsControl  = RTS_CONTROL_ENABLE;
    }
    else
      if (last=='p')
      {
        lpDCB->fInX   = FALSE;
        lpDCB->fOutX    = FALSE;
        lpDCB->fOutxCtsFlow = TRUE;
        lpDCB->fOutxDsrFlow = TRUE;
        lpDCB->fDtrControl  = DTR_CONTROL_HANDSHAKE;
        lpDCB->fRtsControl  = RTS_CONTROL_HANDSHAKE;
      }
      else
      {
        lpDCB->fInX   = FALSE;
        lpDCB->fOutX    = FALSE;
        lpDCB->fOutxCtsFlow = FALSE;
        lpDCB->fOutxDsrFlow = FALSE;
        lpDCB->fDtrControl  = DTR_CONTROL_ENABLE;
        lpDCB->fRtsControl  = RTS_CONTROL_ENABLE;
      }
    lpDCB->XonChar    = 0;
    lpDCB->XoffChar   = 0;
    lpDCB->ErrorChar  = 0;
    lpDCB->fErrorChar = 0;
    lpDCB->EofChar    = 0;
    lpDCB->EvtChar    = 0;
    lpDCB->XonLim     = 0;
    lpDCB->XoffLim    = 0;
    return TRUE;
  }

  ptr=strtok(temp," ");
  while (ptr)
  {
    DWORD flag,x;

    flag=0;
    if (!strnicmp("baud=",ptr,5))
    {
      if (!sscanf(ptr+5,"%ld",&x))
      {
        dprintf(("Couldn't parse %s\n",ptr));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }
      lpDCB->BaudRate = x;
      flag=1;
    }
    if (!strnicmp("stop=",ptr,5))
    {
      if (!sscanf(ptr+5,"%ld",&x))
      {
        dprintf(("Couldn't parse %s\n",ptr));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }
      lpDCB->StopBits = x;
      flag=1;
    }
    if (!strnicmp("data=",ptr,5))
    {
      if (!sscanf(ptr+5,"%ld",&x))
      {
        dprintf(("Couldn't parse %s\n",ptr));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }
      lpDCB->ByteSize = x;
      flag=1;
    }
    if (!strnicmp("parity=",ptr,7))
    {
      lpDCB->fParity  = TRUE;
      switch (ptr[8])
      {
        case 'N':case 'n':
          lpDCB->fParity  = FALSE;
          lpDCB->Parity = NOPARITY;
          break;
        case 'E':case 'e':
          lpDCB->Parity = EVENPARITY;
          break;
        case 'O':case 'o':
          lpDCB->Parity = ODDPARITY;
          break;
        case 'M':case 'm':
          lpDCB->Parity = MARKPARITY;
          break;
      }
      flag=1;
    }
    if (!strnicmp("to=",ptr,3))
    {
      if (!strnicmp("on",ptr+3,2))
      {
        if(NULL==lpCommTimeouts)
        {
          dprintf(("TO=ON and no lpCommTimeout"));
          SetLastError(ERROR_INVALID_PARAMETER);
          return FALSE;
        }
        else
        {
          // @@@ Todo Implement timout handling
          flag=1;
        }
      }
      if (!strnicmp("off",ptr+3,3))
      {
        flag=1;
      }
    }

    if (!flag)
    {
      dprintf(("Unhandled specifier '%s', please report.\n",ptr));
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }
    ptr=strtok(NULL," ");
  }

  if (lpDCB->BaudRate==110)
    lpDCB->StopBits = 2;
  return TRUE;
}

//------------------------------------------------------------------------------

BOOL WIN32API BuildCommDCBAndTimeoutsW( LPCWSTR lpDef, LPDCB lpDCB, LPCOMMTIMEOUTS lpCommTimeouts )
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpDef);
  rc = BuildCommDCBAndTimeoutsA(asciiname, lpDCB, lpCommTimeouts);
  FreeAsciiString(asciiname);
  return(rc);
}

//------------------------------------------------------------------------------

typedef BOOL (* WIN32API COMMDLGFUNC)(LPCSTR, HWND, LPCOMMCONFIG );

BOOL WIN32API CommConfigDialogA( LPCSTR lpszName, HWND hWnd, LPCOMMCONFIG lpCC )
{
  COMMDLGFUNC lpfnCommDialog;
  HMODULE hConfigModule;
  char    szSerialUI[MAX_PATH+1];
  char    szKeyname[5];
  BOOL r;
  HKEY hkPorts, hkName;
  LONG rc;
  DWORD dwType,dwSize;
  int port;

  dprintf(("CommConfigDialogA (%p %x %p)\n",lpszName, hWnd, lpCC));

  if( strnicmp(lpszName,"com",3) ||
      strlen(lpszName)<4 ||
      strlen(lpszName)>7)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  port = atoi(lpszName+3);
  if( (0==port) ||(port>9999))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  port--;

  sprintf(szKeyname,"%04d",port);
  dprintf(("CommConfigDialogA look in reg for port %s",szKeyname));
  rc = RegOpenKeyExA( HKEY_LOCAL_MACHINE,
                      "System\\CurrentControlSet\\Services\\Class\\Ports",
                      0,
                      KEY_READ,
                      &hkPorts);

  if(rc!=ERROR_SUCCESS)
  {
    SetLastError(ERROR_DEV_NOT_EXIST);
    return FALSE;
  }

  rc = RegOpenKeyExA( hkPorts,
                      szKeyname,
                      0,
                      KEY_READ,
                      &hkName);

  if(rc!=ERROR_SUCCESS)
  {
    SetLastError(ERROR_DEV_NOT_EXIST);
    RegCloseKey(hkPorts);
    return FALSE;
  }

  dwSize = sizeof(szSerialUI);

  rc = RegQueryValueExA( hkName,
                         "ConfigDialog",
                         NULL,
                         &dwType,
                         (LPBYTE)szSerialUI,
                         &dwSize);

  RegCloseKey(hkName);
  RegCloseKey(hkPorts);
  if( (rc!=ERROR_SUCCESS) && (dwType!=REG_SZ) )
  {
    SetLastError(ERROR_DEV_NOT_EXIST);
    return FALSE;
  }

  hConfigModule = LoadLibraryA(szSerialUI);
  if(!hConfigModule)
    return FALSE;

  lpfnCommDialog = (COMMDLGFUNC)GetProcAddress(hConfigModule, (LPCSTR)3L);

  if(!lpfnCommDialog)
    return FALSE;

  SetLastError(ERROR_SUCCESS);

  r = lpfnCommDialog(lpszName,hWnd,lpCC);

  FreeLibrary(hConfigModule);

  return r;
}

//------------------------------------------------------------------------------

BOOL WIN32API CommConfigDialogW( LPCWSTR lpszName, HWND hWnd, LPCOMMCONFIG lpCC )
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpszName);
  rc = CommConfigDialogA(asciiname,hWnd,lpCC);
  FreeAsciiString(asciiname);
  return rc;
}

/*****************************************************************************
 * @returns   True on success and fills the COMMCONFIG structure
 * @param     lpszName     Pointer to devicename
 * @param     lpCC         Pointer to COMMCONFIG buffer.
 * @param     lpdwSize     [in]    Pointer to size of Buffer pointed to by lpCC
 *                         [out]   Number of bytes copied to the buffer
 *                         [error] If buffer to small Number of bytes needed
 * @remark
 * @status    untested
 * @author    Markus Montkowski
 *****************************************************************************/

BOOL WIN32API GetDefaultCommConfigA(LPCSTR lpszName, LPCOMMCONFIG lpCC,
                                    LPDWORD lpdwSize)
{
  HFILE hCOM;
  BOOL rc;

  dprintf(("GetDefaultCommConfigA untested stub \n"));
  SetLastError(ERROR_SUCCESS);

  if(IsBadReadPtr(lpszName,5) ||
     IsBadWritePtr(lpdwSize,sizeof(DWORD)) ||
     IsBadWritePtr(lpCC,*lpdwSize) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);    /* set win32 error information */
    return(FALSE);
  }

  if(strnicmp(lpszName,"COM",3) &&
     strnicmp(lpszName,"\\COM",4) &&
     strnicmp(lpszName,"\\dev\\COM",8) )
  {
    SetLastError(ERROR_FILE_NOT_FOUND);       /* set win32 error information */
    return(FALSE);
  }

  hCOM = HMCreateFile( lpszName,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        NULL);
  if(0!=hCOM)
  {
    rc = HMCommGetDefaultCommConfig(hCOM, lpCC, lpdwSize);
    HMCloseHandle(hCOM);
    return(rc);
  }
  return(FALSE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetDefaultCommConfigW( LPCWSTR lpszName, LPCOMMCONFIG lpCC, LPDWORD lpdwSize )
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpszName);
  rc = GetDefaultCommConfigA(asciiname, lpCC, lpdwSize);
  FreeAsciiString(asciiname);
  return(rc);
}

/*****************************************************************************
 * @returns   True on Success
 * @param     lpszName     Pointer to devicename
 * @param     lpCC         Pointer to COMMCONFIG buffer.
 * @param     dwSize       Size of Buffer pointed to by lpCC
 * @remark
 * @status    untested
 * @author    Markus Montkowski
 *****************************************************************************/

BOOL SetDefaultCommConfigA(LPCSTR lpszName, LPCOMMCONFIG lpCC, DWORD dwSize )
{
  HFILE hCOM;
  BOOL rc;

  dprintf(("SetDefaultCommConfigA untested stub \n"));
  SetLastError(ERROR_INVALID_PARAMETER);    /* set win32 error information */
  rc = FALSE;

  if(!IsBadReadPtr(lpszName,5) &&
     !IsBadWritePtr(lpCC,dwSize)&&
     lpCC->dwSize== dwSize )
  {
    switch(lpCC->dwProviderSubType)
    {
      case PST_RS232:
        if(strnicmp(lpszName,"COM",3) &&
           strnicmp(lpszName,"\\COM",4) &&
           strnicmp(lpszName,"\\dev\\COM",8) )
        {
          SetLastError(ERROR_FILE_NOT_FOUND);       /* set win32 error information */
          return(FALSE);
        }

        SetLastError(ERROR_SUCCESS);

        hCOM = HMCreateFile( lpszName,
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                              NULL);
        if(0!=hCOM)
        {
          rc = HMCommSetDefaultCommConfig(hCOM, lpCC, dwSize);
          HMCloseHandle(hCOM);
          return(rc);
        }
        break;
      case PST_PARALLELPORT:
      case PST_MODEM:
      default:
        SetLastError(ERROR_FILE_NOT_FOUND);
        dprintf(("SetDefaultCommConfigA: ProviderSubType &d Not implemented (FALSE)\n",lpCC->dwProviderSubType));
        break;
    }
  }
  return(rc);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetDefaultCommConfigW( LPCWSTR lpszName, LPCOMMCONFIG lpCC, DWORD dwSize )
{
  char *asciiname;
  BOOL rc;

  asciiname  = UnicodeToAsciiString((LPWSTR)lpszName);
  rc = SetDefaultCommConfigA(asciiname, lpCC, dwSize);
  FreeAsciiString(asciiname);
  return(rc);
}

/*****************************************************************************
 * Name      : HMCOMGetCommState
 * Purpose   : router function for GetCommState
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Achim Hasenmueller [Sat, 1999/11/27 13:40]
 *****************************************************************************/

BOOL WIN32API GetCommState(HANDLE hCommDev, LPDCB lpdcb)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpdcb,sizeof(DCB)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetCommState(&pHMHandle->hmHandleData,
                                                    lpdcb);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API WaitCommEvent( HANDLE hCommDev,
                          LPDWORD lpfdwEvtMask,
                          LPOVERLAPPED lpo)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(NULL!=lpo && IsBadReadPtr(lpo,sizeof(OVERLAPPED)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->WaitCommEvent( &pHMHandle->hmHandleData,
                                                      lpfdwEvtMask,
                                                      lpo);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCommProperties( HANDLE hCommDev,
                              LPCOMMPROP lpcmmp)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpcmmp,sizeof(COMMPROP)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetCommProperties( &pHMHandle->hmHandleData,
                                                          lpcmmp);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCommMask( HANDLE hCommDev,
                        LPDWORD lpfdwEvtMask)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpfdwEvtMask,sizeof(DWORD)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetCommMask( &pHMHandle->hmHandleData,
                                                    lpfdwEvtMask);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCommMask( HANDLE hCommDev,
                        DWORD fdwEvtMask)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  bResult = pHMHandle->pDeviceHandler->SetCommMask( &pHMHandle->hmHandleData,
                                                    fdwEvtMask);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PurgeComm( HANDLE hCommDev,
                      DWORD fdwAction)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  bResult = pHMHandle->pDeviceHandler->PurgeComm( &pHMHandle->hmHandleData,
                                                  fdwAction);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClearCommError( HANDLE hCommDev,
                           LPDWORD lpdwErrors,
                           LPCOMSTAT lpcst)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if((lpdwErrors != NULL && IsBadWritePtr(lpdwErrors,sizeof(DWORD))) ||
     (NULL!=lpcst && IsBadWritePtr(lpcst,sizeof(COMSTAT)) ) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->ClearCommError(&pHMHandle->hmHandleData,
                                                      lpdwErrors,
                                                      lpcst);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCommState( HANDLE hCommDev,
                         LPDCB lpdcb)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadReadPtr(lpdcb,sizeof(DCB)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->SetCommState(&pHMHandle->hmHandleData,
                                                    lpdcb);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCommTimeouts( HANDLE hCommDev,
                            LPCOMMTIMEOUTS lpctmo)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpctmo,sizeof(COMMTIMEOUTS)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetCommTimeouts( &pHMHandle->hmHandleData,
                                                        lpctmo);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCommModemStatus( HANDLE hCommDev,
                               LPDWORD lpModemStat )
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpModemStat,sizeof(DWORD)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetCommModemStatus( &pHMHandle->hmHandleData,
                                                           lpModemStat);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCommTimeouts( HANDLE hCommDev,
                            LPCOMMTIMEOUTS lpctmo)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadReadPtr(lpctmo,sizeof(COMMTIMEOUTS)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->SetCommTimeouts( &pHMHandle->hmHandleData,
                                                        lpctmo);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TransmitCommChar( HANDLE hCommDev,
                             CHAR cChar )
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  bResult = pHMHandle->pDeviceHandler->TransmitCommChar( &pHMHandle->hmHandleData,
                                                         cChar);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCommConfig( HANDLE hCommDev,
                          LPCOMMCONFIG lpCC,
                          DWORD dwSize )
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if( IsBadReadPtr(lpCC,sizeof(COMMCONFIG)) ||
      dwSize < sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->SetCommConfig( &pHMHandle->hmHandleData,
                                                      lpCC,
                                                      dwSize);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCommBreak( HANDLE hCommDev )
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  bResult = pHMHandle->pDeviceHandler->SetCommBreak( &pHMHandle->hmHandleData);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetCommConfig( HANDLE hCommDev,
                          LPCOMMCONFIG lpCC,
                          LPDWORD lpdwSize )
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpdwSize,sizeof(DWORD)) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  if( IsBadWritePtr(lpCC,sizeof(COMMCONFIG)) ||
      *lpdwSize< sizeof(COMMCONFIG) )
  {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    *lpdwSize= sizeof(COMMCONFIG);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetCommConfig( &pHMHandle->hmHandleData,
                                                      lpCC,
                                                      lpdwSize);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EscapeCommFunction( HANDLE hCommDev,
                                      UINT dwFunc )
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  switch(dwFunc)
  {
    case CLRDTR:
    case CLRRTS:
    case SETDTR:
    case SETRTS:
    case SETXOFF:
    case SETXON:
      bResult = pHMHandle->pDeviceHandler->EscapeCommFunction( &pHMHandle->hmHandleData,
                                                               dwFunc);
      break;
    case SETBREAK:
      bResult = pHMHandle->pDeviceHandler->SetCommBreak(&pHMHandle->hmHandleData);
      break;
    case CLRBREAK:
      bResult = pHMHandle->pDeviceHandler->ClearCommBreak(&pHMHandle->hmHandleData);
      break;
    default:
      SetLastError(ERROR_INVALID_PARAMETER);
      bResult = FALSE;
  }


  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetupComm( HANDLE hCommDev,
                      DWORD dwInQueue,
                      DWORD dwOutQueue)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  bResult = pHMHandle->pDeviceHandler->SetupComm(&pHMHandle->hmHandleData,
                                                 dwInQueue,
                                                 dwOutQueue);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ClearCommBreak(HANDLE hCommDev)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  bResult = pHMHandle->pDeviceHandler->ClearCommBreak(&pHMHandle->hmHandleData);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
static BOOL HMCommSetDefaultCommConfig( HANDLE hCommDev,
                                        LPCOMMCONFIG lpCC,
                                        DWORD dwSize)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if( (lpCC!=NULL) &&
      ( IsBadReadPtr(lpCC,sizeof(COMMCONFIG)) ||
        dwSize != sizeof(COMMCONFIG) ) )
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->SetDefaultCommConfig(&pHMHandle->hmHandleData,
                                                            lpCC,
                                                            dwSize);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
static BOOL HMCommGetDefaultCommConfig( HANDLE hCommDev,
                                 LPCOMMCONFIG lpCC,
                                 LPDWORD lpdwSize)
{
  BOOL      bResult;                /* result from the device handler's API */
  PHMHANDLE pHMHandle;       /* pointer to the handle structure in the table */

                                                          /* validate handle */
  pHMHandle = HMHandleQueryPtr(hCommDev);              /* get the index */
  if (pHMHandle == NULL)                                     /* error ? */
  {
    return FALSE; //last error set by HMHandleQueryPtr (ERROR_INVALID_HANDLE)
  }

  if(IsBadWritePtr(lpdwSize,sizeof(DWORD)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  bResult = pHMHandle->pDeviceHandler->GetDefaultCommConfig( &pHMHandle->hmHandleData,
                                                             lpCC,
                                                             lpdwSize);

  return (bResult);                                  /* deliver return code */
}
//******************************************************************************
//******************************************************************************
