/* $Id: internet.c,v 1.7 2002-11-27 14:28:17 sandervl Exp $
 *
 * Wininet
 *
 * Copyright 1999 Corel Corporation
 *
 * Ulrich Czekalla
 *
 */

#include <windows.h>
#include <wininet.h>
#include <debugtools.h>
#include <winerror.h>
#include <shlwapi.h>
#include <tchar.h>
#include <winsock.h>
#include <ipexport.h>
#include <icmpapi.h>

#ifdef __WIN32OS2__
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <win/windef.h>
#define strncasecmp strnicmp
#define MAXHOSTNAME 100
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#endif

#include "internet.h"

DEFAULT_DEBUG_CHANNEL(wininet)

#define MAX_IDLE_WORKER 1000*60*1
#define MAX_WORKER_THREADS 10
#define RESPONSE_TIMEOUT 30

#define GET_HWININET_FROM_LPWININETFINDNEXT(lpwh)\
(LPWININETAPPINFOA)(((LPWININETFTPSESSIONA)(lpwh->hdr.lpwhparent))->hdr.lpwhparent)

typedef struct
{
  DWORD dwError;
  CHAR  response[ MAX_REPLY_LEN];
} WITHREADERROR, *LPWITHREADERROR;

INTERNET_SCHEME GetInternetScheme(LPCSTR lpszScheme, int nMaxCmp);
BOOL INTERNET_FindNextFileA(HINTERNET hFind, LPVOID lpFindData);
VOID INTERNET_ExecuteWork(void);

DWORD g_dwTlsErrIndex = TLS_OUT_OF_INDEXES;
LONG  dwNumThreads;
LONG  dwNumIdleThreads;
HANDLE hEventArray[2];
#define hQuitEvent hEventArray[0]
#define hWorkEvent hEventArray[1]
CRITICAL_SECTION csQueue;
LPWORKREQUEST lpHeadWorkQueue;
LPWORKREQUEST lpWorkQueueTail;

/***********************************************************************
 * WININET_LibMain [Internal] Initializes the internal 'WININET.DLL'.
 *
 * PARAMS
 *     hinstDLL    [I] handle to the 'dlls' instance
 *     fdwReason   [I]
 *     lpvReserved [I] reserverd, must be NULL
 *
 * RETURNS
 *     Success: TRUE
 *     Failure: FALSE
 */

BOOL WINAPI
WININET_LibMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
        TRACE("%x,%lx,%p\n", hinstDLL, fdwReason, lpvReserved);

        switch (fdwReason) {
                case DLL_PROCESS_ATTACH:
                {
#ifdef __WIN32OS2__
                        WORD ver = MAKEWORD (2, 2);
                        WSADATA data;
#endif
                        g_dwTlsErrIndex = TlsAlloc();

                        if(g_dwTlsErrIndex == TLS_OUT_OF_INDEXES)
                          return FALSE;

                        hQuitEvent = CreateEventA(0, TRUE, FALSE, NULL);
                        hWorkEvent = CreateEventA(0, FALSE, FALSE, NULL);
                        InitializeCriticalSection(&csQueue);

                        dwNumThreads=0;
                        dwNumIdleThreads=0;
#ifdef __WIN32OS2__
                        WSAStartup ( ver, &data );
#endif
                }
                case DLL_THREAD_ATTACH:
                    {
                      LPWITHREADERROR lpwite = HeapAlloc(GetProcessHeap(), 0, sizeof(WITHREADERROR));

                      if (NULL == lpwite)
                        return FALSE;

                      TlsSetValue(g_dwTlsErrIndex, (LPVOID)lpwite);
                    }
                    break;

                case DLL_THREAD_DETACH:
                    if (g_dwTlsErrIndex != TLS_OUT_OF_INDEXES)
                      {
                        LPVOID lpwite = TlsGetValue(g_dwTlsErrIndex);
                        if(lpwite)
                          HeapFree(GetProcessHeap(), 0, lpwite);
                      }
                    break;

                case DLL_PROCESS_DETACH:
                    if (g_dwTlsErrIndex != TLS_OUT_OF_INDEXES)
                      {
                        HeapFree(GetProcessHeap(), 0, TlsGetValue(g_dwTlsErrIndex));
                        TlsFree(g_dwTlsErrIndex);
                      }

                    SetEvent(hQuitEvent);

                    CloseHandle(hQuitEvent);
                    CloseHandle(hWorkEvent);
                    DeleteCriticalSection(&csQueue);
#ifdef __WIN32OS2__
 		    WSACleanup();
#endif
                    break;
        }

        return TRUE;
}


/***********************************************************************
 *           InternetOpenA   (WININET.113)
 *
 * Per-application initialization of wininet
 *
 * RETURNS
 *    HINTERNET on success
 *    NULL on failure
 *
 */
INTERNETAPI HINTERNET WINAPI InternetOpenA(LPCSTR lpszAgent,
        DWORD dwAccessType, LPCSTR lpszProxy,
        LPCSTR lpszProxyBypass, DWORD dwFlags)
{
        LPWININETAPPINFOA lpwai = NULL;

        TRACE("InternetOpenA\n");

        /* Clear any error information */
        INTERNET_SetLastError(0);

        TRACE("InternetOpenA...\n");

        lpwai = HeapAlloc(GetProcessHeap(), 0, sizeof(WININETAPPINFOA));
        if (NULL == lpwai)
                INTERNET_SetLastError(ERROR_OUTOFMEMORY);
        else
        {
                memset(lpwai, 0, sizeof(WININETAPPINFOA));
                lpwai->hdr.htype = WH_HINIT;
                lpwai->hdr.lpwhparent = NULL;
                lpwai->hdr.dwFlags = dwFlags;
                if (NULL != lpszAgent)
                        lpwai->lpszAgent = strdup(lpszAgent);
                if (NULL != lpszProxy)
                        lpwai->lpszProxy = strdup(lpszProxy);
                if (NULL != lpszProxyBypass)
                        lpwai->lpszProxyBypass = strdup(lpszProxyBypass);
                lpwai->dwAccessType = dwAccessType;
        }

        return (HINTERNET)lpwai;
}


/***********************************************************************
 *           InternetGetLastResponseInfoA (WININET.108)
 *
 * Return last wininet error description on the calling thread
 *
 * RETURNS
 *    TRUE on success of writting to buffer
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetGetLastResponseInfoA(LPDWORD lpdwError,
    LPSTR lpszBuffer, LPDWORD lpdwBufferLength)
{
    LPWITHREADERROR lpwite=(LPWITHREADERROR)TlsGetValue(g_dwTlsErrIndex);

    *lpdwError = lpwite->dwError;
    if (lpwite->dwError)
    {
      strncpy(lpszBuffer, lpwite->response, *lpdwBufferLength);
      *lpdwBufferLength = strlen( lpszBuffer);
    }
    else
      *lpdwBufferLength=0;

    return TRUE;
}

/***********************************************************************
 *           InternetGetConnectedState (WININET.105)
 *
 * Return connected state
 *
 * RETURNS
 *    True if connected
 *    if lpdwStatus is not NULL, return the status (off line,
 *    modem, lan...) in it.
 *    FALSE if not connected
 */
BOOL WINAPI InternetGetConnectedState(LPDWORD lpdwStatus, DWORD dwReserved)
{
  if (lpdwStatus) {
    FIXME("always returning LAN connection.\n");
    *lpdwStatus = INTERNET_CONNECTION_LAN;
  }
  return TRUE;
}

/***********************************************************************
 *           InternetConnectA (WININET.93)
 *
 * Open a ftp, gopher or http session
 *
 * RETURNS
 *    HINTERNET a session handle on success
 *    NULL on failure
 *
 */
INTERNETAPI HINTERNET WINAPI InternetConnectA(HINTERNET hInternet,
    LPCSTR lpszServerName, INTERNET_PORT nServerPort,
    LPCSTR lpszUserName, LPCSTR lpszPassword,
    DWORD dwService, DWORD dwFlags, DWORD dwContext)
{
        HINTERNET rc = (HINTERNET) NULL;

        TRACE("\n");

        /* Clear any error information */
        INTERNET_SetLastError(0);

        switch (dwService)
        {
                case INTERNET_SERVICE_FTP:
                        rc = FTP_Connect(hInternet, lpszServerName, nServerPort,
                                lpszUserName, lpszPassword, dwFlags, dwContext);
                        break;

                case INTERNET_SERVICE_HTTP:
                        rc = HTTP_Connect(hInternet, lpszServerName, nServerPort,
                        lpszUserName, lpszPassword, dwFlags, dwContext);
                        break;

                case INTERNET_SERVICE_GOPHER:
                default:
                        break;
        }

        return rc;
}


/***********************************************************************
 *           InternetFindNextFileA (WININET.102)
 *
 * Continues a file search from a previous call to FindFirstFile
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetFindNextFileA(HINTERNET hFind, LPVOID lpvFindData)
{
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFINDNEXTA lpwh = (LPWININETFINDNEXTA) hFind;

        TRACE("\n");
        if (NULL == lpwh || lpwh->hdr.htype != WH_HFINDNEXT)
        {
                INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
                return FALSE;
        }

        hIC = GET_HWININET_FROM_LPWININETFINDNEXT(lpwh);
        if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = INTERNETFINDNEXTA;
          workRequest.HFTPSESSION = (DWORD)hFind;
          workRequest.LPFINDFILEDATA = (DWORD)lpvFindData;

          return INTERNET_AsyncCall(&workRequest);
        }
        else
          return INTERNET_FindNextFileA(hFind, lpvFindData);
}


/***********************************************************************
 *           INTERNET_FindNextFileA (Internal)
 *
 * Continues a file search from a previous call to FindFirstFile
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL INTERNET_FindNextFileA(HINTERNET hFind, LPVOID lpvFindData)
{
        BOOL bSuccess = TRUE;
        LPWININETAPPINFOA hIC = NULL;
        LPWIN32_FIND_DATAA lpFindFileData;
        LPWININETFINDNEXTA lpwh = (LPWININETFINDNEXTA) hFind;

        if (NULL == lpwh || lpwh->hdr.htype != WH_HFINDNEXT)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        if(lpwh->hdr.lpwhparent->htype != WH_HFTPSESSION)
        {
          FIXME("Only FTP find next supported\n");
          INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
          return FALSE;
        }

        lpFindFileData = (LPWIN32_FIND_DATAA) lpvFindData;
        memset(lpFindFileData, 0, sizeof(WIN32_FIND_DATAA));

        if(lpwh->index >= lpwh->size)
        {
          INTERNET_SetLastError(ERROR_NO_MORE_FILES);
          bSuccess = FALSE;
          goto lend;
        }

        FTP_ConvertFileProp(&lpwh->lpafp[lpwh->index], lpFindFileData);
        lpwh->index++;

lend:
        hIC = GET_HWININET_FROM_LPWININETFINDNEXT(lpwh);
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
        {
          INTERNET_ASYNC_RESULT iar;

          iar.dwResult = (DWORD)bSuccess;
          iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();

          hIC->lpfnStatusCB(hFind, lpwh->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
                &iar, sizeof(INTERNET_ASYNC_RESULT));
        }

        return bSuccess;
}


/***********************************************************************
 *           INTERNET_CloseHandle (Internal)
 *
 * Closes internet handle
 *
 * RETURNS
 *    void
 *
 */
void INTERNET_CloseHandle(LPWININETAPPINFOA lpwai)
{
  if(lpwai->lpszAgent)
    free(lpwai->lpszAgent);

  if(lpwai->lpszProxy)
    free(lpwai->lpszProxy);

  if(lpwai->lpszProxyBypass)
    free(lpwai->lpszProxyBypass);

  HeapFree(GetProcessHeap(), 0, lpwai);
}

/***********************************************************************
 *           InternetCloseHandle (WININET.89)
 *
 * Continues a file search from a previous call to FindFirstFile
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetCloseHandle(HINTERNET hInternet)
{
        BOOL retval = FALSE;
        LPWININETHANDLEHEADER lpwh = (LPWININETHANDLEHEADER) hInternet;

        TRACE("\n");
        if (NULL == lpwh)
                return FALSE;

        /* Clear any error information */
        INTERNET_SetLastError(0);

        switch (lpwh->htype)
        {
                case WH_HINIT:
                        INTERNET_CloseHandle((LPWININETAPPINFOA) lpwh);
                        retval = TRUE;
                        break;

                case WH_HHTTPSESSION:
                        HTTP_CloseHTTPSessionHandle((LPWININETHTTPSESSIONA) lpwh);
                        retval = TRUE;
                        break;

                case WH_HHTTPREQ:
                        HTTP_CloseHTTPRequestHandle((LPWININETHTTPREQA) lpwh);
                        retval = TRUE;
                        break;

                case WH_HFTPSESSION:
                        retval = FTP_CloseSessionHandle((LPWININETFTPSESSIONA) lpwh);
                        break;

                case WH_HFINDNEXT:
                        retval = FTP_CloseFindNextHandle((LPWININETFINDNEXTA) lpwh);
                        break;

                default:
                        break;
        }

        return retval;
}

/***********************************************************************
 *           SetUrlComponentValue (Internal)
 *
 * Helper function for InternetCrackUrlA
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL SetUrlComponentValue(LPSTR* lppszComponent, LPDWORD dwComponentLen, 
    LPCSTR lpszStart, INT len)
{
    TRACE("%s (%d)\n", lpszStart, len);

    if (*dwComponentLen != 0)
    {
	if (*lppszComponent == NULL)
	{
            *lppszComponent = (LPSTR)lpszStart;
	    *dwComponentLen = len;
	}
	else
	{
            INT ncpylen = min((*dwComponentLen)-1, len);
            if (lpszStart)
                strncpy(*lppszComponent, lpszStart, ncpylen);
            (*lppszComponent)[ncpylen] = '\0';
	    *dwComponentLen = ncpylen;
	}
    }

    return TRUE;
}



/***********************************************************************
 *           InternetAttemptConnect (WININET.81)
 *
 * Attempt to make a connection to the internet
 *
 * RETURNS
 *    ERROR_SUCCESS on success
 *    Error value   on failure
 *
 */
INTERNETAPI DWORD WINAPI InternetAttemptConnect(DWORD dwReserved)
{
    FIXME("Stub\n");
    return ERROR_SUCCESS;
}


/***********************************************************************
 *           InternetCanonicalizeUrlA (WININET.85)
 *
 * Escape unsafe characters and spaces
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetCanonicalizeUrlA(LPCSTR lpszUrl, LPSTR lpszBuffer,
        LPDWORD lpdwBufferLength, DWORD dwFlags)
{
  HRESULT hr;
  TRACE("%s %p %p %08lx\n",debugstr_a(lpszUrl), lpszBuffer,
        lpdwBufferLength, dwFlags);
  
  /* Flip this bit to correspond to URL_ESCAPE_UNSAFE */
  dwFlags ^= ICU_NO_ENCODE;
  
  dwFlags |= 0x80000000; /* Don't know what this means */
  
  hr = UrlCanonicalizeA(lpszUrl, lpszBuffer, lpdwBufferLength, dwFlags);
  
  return (hr == S_OK) ? TRUE : FALSE;
}

/***********************************************************************
 *           InternetSetStatusCallback (WININET.133)
 *
 * Sets up a callback function which is called as progress is made
 * during an operation.
 *
 * RETURNS
 *    Previous callback or NULL         on success
 *    INTERNET_INVALID_STATUS_CALLBACK  on failure
 *
 */
INTERNETAPI INTERNET_STATUS_CALLBACK WINAPI InternetSetStatusCallback(
        HINTERNET hInternet ,INTERNET_STATUS_CALLBACK lpfnIntCB)
{
        INTERNET_STATUS_CALLBACK retVal;
        LPWININETAPPINFOA lpwai = (LPWININETAPPINFOA)hInternet;

        TRACE("\n");
        if (lpwai->hdr.htype != WH_HINIT)
                return INTERNET_INVALID_STATUS_CALLBACK;

        retVal = lpwai->lpfnStatusCB;
        lpwai->lpfnStatusCB = lpfnIntCB;

        return retVal;
}


/***********************************************************************
 *           InternetWriteFile (WININET.138)
 *
 * Write data to an open internet file
 *
 * RETURNS
 *    TRUE  on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetWriteFile(HINTERNET hFile, LPCVOID lpBuffer ,
        DWORD dwNumOfBytesToWrite, LPDWORD lpdwNumOfBytesWritten)
{
        BOOL retval = FALSE;
        int nSocket = INVALID_SOCKET;
        int nBytesWritten;
        LPWININETHANDLEHEADER lpwh = (LPWININETHANDLEHEADER) hFile;

        TRACE("\n");
        if (NULL == lpwh)
                return FALSE;

        switch (lpwh->htype)
        {
                case WH_HHTTPREQ:
                        nSocket = ((LPWININETHTTPREQA)hFile)->nSocketFD;
                        break;

                case WH_HFILE:
                        nSocket = ((LPWININETFILE)hFile)->nDataSocket;
                        break;

                default:
                        break;
        }

        if (INVALID_SOCKET != nSocket)
        {
                nBytesWritten = INTERNET_WriteDataToStream(nSocket, lpBuffer, dwNumOfBytesToWrite);
                if (nBytesWritten < 0)
                        *lpdwNumOfBytesWritten = 0;
                else
                {
                        *lpdwNumOfBytesWritten = nBytesWritten;
                        retval = TRUE;
                }
        }

        return retval;
}


/***********************************************************************
 *           InternetReadFile (WININET.121)
 *
 * Read data from an open internet file
 *
 * RETURNS
 *    TRUE  on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetReadFile(HINTERNET hFile, LPVOID lpBuffer,
        DWORD dwNumOfBytesToRead, LPDWORD dwNumOfBytesRead)
{
        BOOL retval = FALSE;
        int nBytesRead;
        int nSocket = INVALID_SOCKET;
        LONG *lpBytesAvailable = 0;
        LPWININETHANDLEHEADER lpwh = (LPWININETHANDLEHEADER) hFile;

        TRACE("\n");
        if (NULL == lpwh)
                return FALSE;

        switch (lpwh->htype)
        {
                case WH_HHTTPREQ:
                        nSocket = ((LPWININETHTTPREQA)hFile)->nSocketFD;
                        break;

                case WH_HFILE:
                        nSocket = ((LPWININETFILE)hFile)->nDataSocket;
                        lpBytesAvailable = &(((LPWININETFILE)hFile)->lBytesAvailable);
                        break;

                default:
                        break;
        }

        if (INVALID_SOCKET != nSocket)
        {
                nBytesRead = INTERNET_ReadDataFromStream(nSocket, lpBuffer, dwNumOfBytesToRead);
                if (nBytesRead < 0)
                        *dwNumOfBytesRead = 0;
                else
                {
                        *dwNumOfBytesRead = nBytesRead;
                        retval = TRUE;
                }
        }

        if(lpBytesAvailable)
          *lpBytesAvailable -= *dwNumOfBytesRead;

        return retval;
}

/***********************************************************************
 *           InternetQueryOptionA
 *
 * Queries an options on the specified handle
 *
 * RETURNS
 *    TRUE  on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetQueryOptionA(HINTERNET hInternet, DWORD dwOption,
        LPVOID lpBuffer, LPDWORD lpdwBufferLength)
{
    LPWININETHANDLEHEADER lpwhh;
    BOOL bSuccess = FALSE;

    TRACE("0x%08lx\n", dwOption);

    if (NULL == hInternet)
    {
        INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
        return FALSE;
    }

    lpwhh = (LPWININETHANDLEHEADER) hInternet;

    switch (dwOption)
    {
       case INTERNET_OPTION_HANDLE_TYPE:
       {
         ULONG type = lpwhh->htype;
         TRACE("INTERNET_OPTION_HANDLE_TYPE: %ld\n", type);

         if (*lpdwBufferLength < sizeof(ULONG))
            INTERNET_SetLastError(ERROR_INSUFFICIENT_BUFFER);
         else
         {
             memcpy(lpBuffer, &type, sizeof(ULONG));
             *lpdwBufferLength = sizeof(ULONG);
             bSuccess = TRUE;
         }

         break;
       }

       default:
         FIXME("Stub!");
         break;
    }

    return bSuccess;
}


/***********************************************************************
 *           GetInternetScheme (internal)
 *
 * Get scheme of url
 *
 * RETURNS
 *    scheme on success
 *    INTERNET_SCHEME_UNKNOWN on failure
 *
 */
INTERNET_SCHEME GetInternetScheme(LPCSTR lpszScheme, int nMaxCmp)
{
  if(lpszScheme==NULL)
        return INTERNET_SCHEME_UNKNOWN;

  if( (strncasecmp("ftp", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_FTP;

  else if( (strncasecmp("gopher", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_GOPHER;

  else if( (strncasecmp("http", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_HTTP;

  else if( (strncasecmp("https", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_HTTPS;

  else if( (strncasecmp("file", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_FILE;

  else if( (strncasecmp("news", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_NEWS;

  else if( (strncasecmp("mailto", lpszScheme, nMaxCmp) == 0))
    return INTERNET_SCHEME_MAILTO;
  else
    return INTERNET_SCHEME_UNKNOWN;
}

/***********************************************************************
 *           InternetCheckConnectionA
 *
 * Pings a requested host to check internet connection
 *
 * RETURNS
 *      TRUE on success and FALSE on failure.
 * On failure ERROR_NOT_CONNECTED is placed into GetLastError
 */
BOOL WINAPI InternetCheckConnectionA( LPCSTR lpszUrl, DWORD dwFlags, DWORD dwReserved)
{
  /* Instead of WINE's shelling out and executing a 'ping' we'll use the */
  /* ICMP.DLL instead.                                                   */
  DWORD rc=0;
  HANDLE hIcmp;
  char RequestBuffer[]="Odin - PING request, please reply...";
  char lpszHost[1024],ReplyBuffer[256];
  URL_COMPONENTSA UrlComponents;
  ULONG dest_addr;
  struct hostent *host;

  if(!lpszUrl)
  {
    FIXME("Unimplemented with URL of NULL\n");
    return TRUE;
  }

  memset(&UrlComponents, 0, sizeof(URL_COMPONENTSA));

  UrlComponents.lpszHostName = lpszHost;
  UrlComponents.dwHostNameLength = 1024;

  if(!InternetCrackUrlA(lpszUrl, 0, 0, &UrlComponents))
  {
      INTERNET_SetLastError(ERROR_NOT_CONNECTED);
      return FALSE;
  }

  hIcmp = IcmpCreateFile();

  if(!hIcmp)
    {
      INTERNET_SetLastError(ERROR_NOT_CONNECTED);
      return FALSE;
    }

  dest_addr = inet_addr(lpszHost);

  if(dest_addr == 0)
    host = gethostbyname(lpszHost);
  else
    host = gethostbyaddr(lpszHost, strlen(lpszHost), PF_INET);

  if(host->h_addr)
    rc=IcmpSendEcho(hIcmp,
                    (IPAddr)host->h_addr,
                    RequestBuffer,
                    strlen(RequestBuffer),
                    NULL,
                    ReplyBuffer,
                    256,
                    5000);
  else
    rc = 0;

  IcmpCloseHandle(hIcmp);

  return rc>0;
}

/***********************************************************************
 *           WriteDataToStream (internal)
 *
 * Send data to server
 *
 * RETURNS
 *
 *   number of characters sent on success
 *   -1 on error
 */
int INTERNET_WriteDataToStream(int nDataSocket, LPCVOID Buffer, DWORD BytesToWrite)
{
        if (INVALID_SOCKET == nDataSocket)
            return SOCKET_ERROR;

        return send(nDataSocket, Buffer, BytesToWrite, 0);
}


/***********************************************************************
 *           INTERNET_ReadDataFromStream (internal)
 *
 * Read data from http server
 *
 * RETURNS
 *
 *   number of characters sent on success
 *   -1 on error
 */
int INTERNET_ReadDataFromStream(int nDataSocket, LPVOID Buffer, DWORD BytesToRead)
{
        if (INVALID_SOCKET == nDataSocket)
            return SOCKET_ERROR;

        return recv(nDataSocket, Buffer, BytesToRead, 0);
}


/***********************************************************************
 *           INTERNET_SetLastError (internal)
 *
 * Set last thread specific error
 *
 * RETURNS
 *
 */
void INTERNET_SetLastError(DWORD dwError)
{
    LPWITHREADERROR lpwite;

    lpwite = (LPWITHREADERROR)TlsGetValue(g_dwTlsErrIndex);

    SetLastError(dwError);
    lpwite->dwError = dwError;
}


/************************************************************************
 *           INTERNET_GetLastError (internal)
 *
 * Get last thread specific error
 *
 * RETURNS
 *
 */
DWORD INTERNET_GetLastError()
{
    LPWITHREADERROR lpwite = (LPWITHREADERROR)TlsGetValue(g_dwTlsErrIndex);
    return lpwite->dwError;
}


/************************************************************************
 *           INTERNET_WorkerThreadFunc (internal)
 *
 * Worker thread execution function
 *
 * RETURNS
 *
 */
DWORD INTERNET_WorkerThreadFunc(LPVOID *lpvParam)
{
    DWORD dwWaitRes;

    while (1)
    {
        dwWaitRes = WaitForMultipleObjects(2, hEventArray, FALSE, MAX_IDLE_WORKER);

        if (dwWaitRes == WAIT_OBJECT_0 + 1)
            INTERNET_ExecuteWork();
        else
            break;

        InterlockedIncrement(&dwNumIdleThreads);
    }

    InterlockedDecrement(&dwNumIdleThreads);
    InterlockedDecrement(&dwNumThreads);
    TRACE("Worker thread exiting\n");
    return TRUE;
}


/************************************************************************
 *           INTERNET_InsertWorkRequest (internal)
 *
 * Insert work request into queue
 *
 * RETURNS
 *
 */
BOOL INTERNET_InsertWorkRequest(LPWORKREQUEST lpWorkRequest)
{
    BOOL bSuccess = FALSE;
    LPWORKREQUEST lpNewRequest;

    TRACE("\n");

    lpNewRequest = HeapAlloc(GetProcessHeap(), 0, sizeof(WORKREQUEST));
    if (lpNewRequest)
    {
        memcpy(lpNewRequest, lpWorkRequest, sizeof(WORKREQUEST));
        lpNewRequest->prev = NULL;

        EnterCriticalSection(&csQueue);

        lpNewRequest->next = lpWorkQueueTail;
        if (lpWorkQueueTail)
            lpWorkQueueTail->prev = lpNewRequest;
        lpWorkQueueTail = lpNewRequest;
        if (!lpHeadWorkQueue)
            lpHeadWorkQueue = lpWorkQueueTail;

        LeaveCriticalSection(&csQueue);

        bSuccess = TRUE;
    }

    return bSuccess;
}


/************************************************************************
 *           INTERNET_GetWorkRequest (internal)
 *
 * Retrieves work request from queue
 *
 * RETURNS
 *
 */
BOOL INTERNET_GetWorkRequest(LPWORKREQUEST lpWorkRequest)
{
    BOOL bSuccess = FALSE;
    LPWORKREQUEST lpRequest = NULL;

    TRACE("\n");

    EnterCriticalSection(&csQueue);

    if (lpHeadWorkQueue)
    {
        lpRequest = lpHeadWorkQueue;
        lpHeadWorkQueue = lpHeadWorkQueue->prev;
        if (lpRequest == lpWorkQueueTail)
            lpWorkQueueTail = lpHeadWorkQueue;
    }

    LeaveCriticalSection(&csQueue);

    if (lpRequest)
    {
        memcpy(lpWorkRequest, lpRequest, sizeof(WORKREQUEST));
        HeapFree(GetProcessHeap(), 0, lpRequest);
        bSuccess = TRUE;
    }

    return bSuccess;
}


/************************************************************************
 *           INTERNET_AsyncCall (internal)
 *
 * Retrieves work request from queue
 *
 * RETURNS
 *
 */
BOOL INTERNET_AsyncCall(LPWORKREQUEST lpWorkRequest)
{
    HANDLE hThread;
    DWORD dwTID;
    BOOL bSuccess = FALSE;

    TRACE("\n");

    if (InterlockedDecrement(&dwNumIdleThreads) < 0)
    {
        InterlockedIncrement(&dwNumIdleThreads);

        if (InterlockedIncrement(&dwNumThreads) > MAX_WORKER_THREADS ||
            !(hThread = CreateThread(NULL, 0,
            (LPTHREAD_START_ROUTINE)INTERNET_WorkerThreadFunc, NULL, 0, &dwTID)))
        {
            InterlockedDecrement(&dwNumThreads);
            INTERNET_SetLastError(ERROR_INTERNET_ASYNC_THREAD_FAILED);
            goto lerror;
        }

        TRACE("Created new thread\n");
    }

    bSuccess = TRUE;
    INTERNET_InsertWorkRequest(lpWorkRequest);
    SetEvent(hWorkEvent);

lerror:

    return bSuccess;
}


/************************************************************************
 *           INTERNET_ExecuteWork (internal)
 *
 * RETURNS
 *
 */
VOID INTERNET_ExecuteWork()
{
    WORKREQUEST workRequest;

    TRACE("\n");

    if (INTERNET_GetWorkRequest(&workRequest))
    {
        switch (workRequest.asyncall)
        {
            case FTPPUTFILEA:
                FTP_FtpPutFileA((HINTERNET)workRequest.HFTPSESSION, (LPCSTR)workRequest.LPSZLOCALFILE,
                    (LPCSTR)workRequest.LPSZNEWREMOTEFILE, workRequest.DWFLAGS, workRequest.DWCONTEXT);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZLOCALFILE);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZNEWREMOTEFILE);
                break;

            case FTPSETCURRENTDIRECTORYA:
                FTP_FtpSetCurrentDirectoryA((HINTERNET)workRequest.HFTPSESSION,
                        (LPCSTR)workRequest.LPSZDIRECTORY);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZDIRECTORY);
                break;

            case FTPCREATEDIRECTORYA:
                FTP_FtpCreateDirectoryA((HINTERNET)workRequest.HFTPSESSION,
                        (LPCSTR)workRequest.LPSZDIRECTORY);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZDIRECTORY);
                break;

            case FTPFINDFIRSTFILEA:
                FTP_FtpFindFirstFileA((HINTERNET)workRequest.HFTPSESSION,
                        (LPCSTR)workRequest.LPSZSEARCHFILE,
                   (LPWIN32_FIND_DATAA)workRequest.LPFINDFILEDATA, workRequest.DWFLAGS,
                   workRequest.DWCONTEXT);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZSEARCHFILE);
                break;

            case FTPGETCURRENTDIRECTORYA:
                FTP_FtpGetCurrentDirectoryA((HINTERNET)workRequest.HFTPSESSION,
                        (LPSTR)workRequest.LPSZDIRECTORY, (LPDWORD)workRequest.LPDWDIRECTORY);
                break;

            case FTPOPENFILEA:
                 FTP_FtpOpenFileA((HINTERNET)workRequest.HFTPSESSION,
                    (LPCSTR)workRequest.LPSZFILENAME,
                    workRequest.FDWACCESS,
                    workRequest.DWFLAGS,
                    workRequest.DWCONTEXT);
                 HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZFILENAME);
                 break;

            case FTPGETFILEA:
                FTP_FtpGetFileA((HINTERNET)workRequest.HFTPSESSION,
                    (LPCSTR)workRequest.LPSZREMOTEFILE,
                    (LPCSTR)workRequest.LPSZNEWFILE,
                    (BOOL)workRequest.FFAILIFEXISTS,
                    workRequest.DWLOCALFLAGSATTRIBUTE,
                    workRequest.DWFLAGS,
                    workRequest.DWCONTEXT);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZREMOTEFILE);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZNEWFILE);
                break;

            case FTPDELETEFILEA:
                FTP_FtpDeleteFileA((HINTERNET)workRequest.HFTPSESSION,
                        (LPCSTR)workRequest.LPSZFILENAME);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZFILENAME);
                break;

            case FTPREMOVEDIRECTORYA:
                FTP_FtpRemoveDirectoryA((HINTERNET)workRequest.HFTPSESSION,
                        (LPCSTR)workRequest.LPSZDIRECTORY);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZDIRECTORY);
                break;

            case FTPRENAMEFILEA:
                FTP_FtpRenameFileA((HINTERNET)workRequest.HFTPSESSION,
                        (LPCSTR)workRequest.LPSZSRCFILE,
                        (LPCSTR)workRequest.LPSZDESTFILE);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZSRCFILE);
                HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZDESTFILE);
                break;

            case INTERNETFINDNEXTA:
                INTERNET_FindNextFileA((HINTERNET)workRequest.HFTPSESSION,
                    (LPWIN32_FIND_DATAA)workRequest.LPFINDFILEDATA);
                break;

            case HTTPSENDREQUESTA:
               HTTP_HttpSendRequestA((HINTERNET)workRequest.HFTPSESSION,
                       (LPCSTR)workRequest.LPSZHEADER,
                       workRequest.DWHEADERLENGTH,
                       (LPVOID)workRequest.LPOPTIONAL,
                       workRequest.DWOPTIONALLENGTH);
               HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZHEADER);
               break;

            case HTTPOPENREQUESTA:
               HTTP_HttpOpenRequestA((HINTERNET)workRequest.HFTPSESSION,
                       (LPCSTR)workRequest.LPSZVERB,
                       (LPCSTR)workRequest.LPSZOBJECTNAME,
                       (LPCSTR)workRequest.LPSZVERSION,
                       (LPCSTR)workRequest.LPSZREFERRER,
                       (LPCSTR*)workRequest.LPSZACCEPTTYPES,
                       workRequest.DWFLAGS,
                       workRequest.DWCONTEXT);
               HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZVERB);
               HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZOBJECTNAME);
               HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZVERSION);
               HeapFree(GetProcessHeap(), 0, (LPVOID)workRequest.LPSZREFERRER);
                break;

        }
    }
}


/************************************************************************
 *          INTERNET_GetResponseBuffer
 *
 * RETURNS
 *
 */
LPSTR INTERNET_GetResponseBuffer()
{
    LPWITHREADERROR lpwite = (LPWITHREADERROR)TlsGetValue(g_dwTlsErrIndex);
    return lpwite->response;
}


/************************************************************************
 *           INTERNET_GetNextLine  (internal)
 *
 * Parse next line in directory string listing
 *
 * RETURNS
 *   Pointer to begining of next line
 *   NULL on failure
 *
 */
LPSTR INTERNET_GetNextLine(INT nSocket, LPSTR lpszBuffer, LPDWORD dwBuffer)
{
    struct timeval tv;
    fd_set infd;
    BOOL bSuccess = FALSE;
    INT nRecv = 0;

    TRACE("\n");

    FD_ZERO(&infd);
    FD_SET(nSocket, &infd);
    tv.tv_sec=RESPONSE_TIMEOUT;
    tv.tv_usec=0;

    while (nRecv < *dwBuffer)
    {
        if (select(nSocket+1,&infd,NULL,NULL,&tv) > 0)
        {
            if (recv(nSocket, &lpszBuffer[nRecv], 1, 0) <= 0)
            {
                INTERNET_SetLastError(ERROR_FTP_TRANSFER_IN_PROGRESS);
                goto lend;
            }

            if (lpszBuffer[nRecv] == '\n')
            {
                bSuccess = TRUE;
                break;
            }
            if (lpszBuffer[nRecv] != '\r')
                nRecv++;
        }
        else
        {
            INTERNET_SetLastError(ERROR_INTERNET_TIMEOUT);
            goto lend;
}
    }

lend:
    if (bSuccess)
    {
        lpszBuffer[nRecv] = '\0';
        *dwBuffer = nRecv - 1;
        TRACE(":%d %s\n", nRecv, lpszBuffer);
        return lpszBuffer;
    }
    else
    {
        return NULL;
    }
}


/************************************************************************
 *           InternetQueryDataAvailable (WININET.118)
 *
 * Fetch the amount of data available for retrieval
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL WINAPI InternetQueryDataAvailable(HINTERNET hFile,LPDWORD lpdwNumberOfBytesAvailable,
                                       DWORD dwFlags, DWORD dwContext)
{
    LONG lBytesAvailable;
    LPWININETHANDLEHEADER lpwh = (LPWININETHANDLEHEADER) hFile;

    if (NULL == lpwh)
    {
        INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
        return FALSE;
    }

    WriteLog("InternetQueryDataAvailable for hHandle %08x",lpwh);

    switch (lpwh->htype)
    {
            case WH_HHTTPREQ:
                    *lpdwNumberOfBytesAvailable = -1;
                    break;

            case WH_HFILE:
                    *lpdwNumberOfBytesAvailable = ((LPWININETFILE)lpwh)->lBytesAvailable;
                    break;

            default:
                    INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
                    return FALSE;
    }

    if(*lpdwNumberOfBytesAvailable == -1)
      return FALSE;

    WriteLog("InternetQueryDataAvailable returns %d",*lpdwNumberOfBytesAvailable);

    return TRUE;
}


/**********************************************************
 *	InternetOpenUrlA (WININET.@)
 *
 * Opens an URL
 *
 * RETURNS
 *   handle of connection or NULL on failure
 */
HINTERNET WINAPI InternetOpenUrlA(HINTERNET hInternet, LPCSTR lpszUrl,
    LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD dwContext)
{
  URL_COMPONENTSA urlComponents;
  char protocol[32], hostName[MAXHOSTNAME], userName[1024];
  char password[1024], path[2048], extra[1024];
  HINTERNET client = NULL, client1 = NULL;
  urlComponents.dwStructSize = sizeof(URL_COMPONENTSA);
  urlComponents.lpszScheme = protocol;
  urlComponents.dwSchemeLength = 32;
  urlComponents.lpszHostName = hostName;
  urlComponents.dwHostNameLength = MAXHOSTNAME;
  urlComponents.lpszUserName = userName;
  urlComponents.dwUserNameLength = 1024;
  urlComponents.lpszPassword = password;
  urlComponents.dwPasswordLength = 1024;
  urlComponents.lpszUrlPath = path;
  urlComponents.dwUrlPathLength = 2048;
  urlComponents.lpszExtraInfo = extra;
  urlComponents.dwExtraInfoLength = 1024;
  if(!InternetCrackUrlA(lpszUrl, strlen(lpszUrl), 0, &urlComponents))
    return NULL;
  switch(urlComponents.nScheme) {
  case INTERNET_SCHEME_FTP:
    if(urlComponents.nPort == 0)
      urlComponents.nPort = INTERNET_DEFAULT_FTP_PORT;
    client = InternetConnectA(hInternet, hostName, urlComponents.nPort,
        userName, password, INTERNET_SERVICE_FTP, dwFlags, dwContext);
    return FtpOpenFileA(client, path, GENERIC_READ, dwFlags, dwContext);
    break;
  case INTERNET_SCHEME_HTTP:
  case INTERNET_SCHEME_HTTPS:
  {
    LPCSTR accept[2] = { "*/*", NULL };
    char *hostreq=(char*)malloc(strlen(hostName)+9);
    sprintf(hostreq, "Host: %s\r\n", hostName);
    if(urlComponents.nPort == 0) {
      if(urlComponents.nScheme == INTERNET_SCHEME_HTTP)
        urlComponents.nPort = INTERNET_DEFAULT_HTTP_PORT;
      else
	urlComponents.nPort = INTERNET_DEFAULT_HTTPS_PORT;
    }
    client = InternetConnectA(hInternet, hostName, urlComponents.nPort, userName,
        password, INTERNET_SERVICE_HTTP, dwFlags, dwContext);
    if(client == NULL)
      return NULL;
    client1 = HttpOpenRequestA(client, NULL, path, NULL, NULL, accept, dwFlags, dwContext);
    if(client1 == NULL) {
      InternetCloseHandle(client);
      return NULL;
    }
    if (lpszHeaders) 
      HttpAddRequestHeadersA(client1, lpszHeaders, dwHeadersLength, HTTP_ADDREQ_FLAG_ADD);
    HttpAddRequestHeadersA(client1, hostreq, -1L, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
    if(!HttpSendRequestA(client1, NULL, 0, NULL, 0)) {
      InternetCloseHandle(client1);
      InternetCloseHandle(client);
      return NULL;
    }
    return client1;
    break;
  }
  case INTERNET_SCHEME_GOPHER:
    /* gopher doesn't seem to be implemented in wine, but it's supposed
     * to be supported by InternetOpenUrlA. */
  default:
    return NULL;
  }
  if(client != NULL)
    InternetCloseHandle(client);
}

/**********************************************************
 *	InternetOpenUrlW (WININET.@)
 *
 * Opens an URL
 *
 * RETURNS
 *   handle of connection or NULL on failure
 */
HINTERNET WINAPI InternetOpenUrlW(HINTERNET hInternet, LPCWSTR lpszUrl,
    LPCWSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD dwContext)
{
    HINTERNET rc = (HINTERNET)NULL;

    INT lenUrl = lstrlenW(lpszUrl)+1;
    INT lenHeaders = lstrlenW(lpszHeaders)+1;
    CHAR *szUrl = (CHAR *)malloc(lenUrl*sizeof(CHAR));
    CHAR *szHeaders = (CHAR *)malloc(lenHeaders*sizeof(CHAR));

    if (!szUrl || !szHeaders)
    {
        if (szUrl)
            free(szUrl);
        if (szHeaders)
            free(szHeaders);
        return (HINTERNET)NULL;
    }

    WideCharToMultiByte(CP_ACP, -1, lpszUrl, -1, szUrl, lenUrl,
        NULL, NULL);
    WideCharToMultiByte(CP_ACP, -1, lpszHeaders, -1, szHeaders, lenHeaders,
        NULL, NULL);

    rc = InternetOpenUrlA(hInternet, szUrl, szHeaders,
        dwHeadersLength, dwFlags, dwContext);

    free(szUrl);
    free(szHeaders);

    return rc;
}

/***********************************************************************
 *           InternetCrackUrlA (WININET.@)
 *
 * Break up URL into its components
 *
 * TODO: Handle dwFlags
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL WINAPI InternetCrackUrlA(LPCSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags,
    LPURL_COMPONENTSA lpUrlComponents)
{
  /*
   * RFC 1808
   * <protocol>:[//<net_loc>][/path][;<params>][?<query>][#<fragment>]
   *
   */
    LPSTR lpszParam    = NULL;
    BOOL  bIsAbsolute = FALSE;
    LPSTR lpszap = (char*)lpszUrl;
    LPSTR lpszcp = NULL;

    TRACE("\n");

    /* Determine if the URI is absolute. */
    while (*lpszap != '\0')
    {
        if (isalnum(*lpszap))
        {
            lpszap++;
            continue;
        }
        if ((*lpszap == ':') && (lpszap - lpszUrl >= 2))
        {
            bIsAbsolute = TRUE;
            lpszcp = lpszap;
        }
	else
	{
	    lpszcp = (LPSTR)lpszUrl; /* Relative url */
        }

        break;
    }

    /* Parse <params> */
    lpszParam = strpbrk(lpszap, ";?");
    if (lpszParam != NULL)
    {
        if (!SetUrlComponentValue(&lpUrlComponents->lpszExtraInfo,
	     &lpUrlComponents->dwExtraInfoLength, lpszParam+1, strlen(lpszParam+1)))
        {
	    return FALSE;
        }
        }

    if (bIsAbsolute) /* Parse <protocol>:[//<net_loc>] */
        {
	LPSTR lpszNetLoc;

        /* Get scheme first. */
        lpUrlComponents->nScheme = GetInternetScheme(lpszUrl, lpszcp - lpszUrl);
        if (!SetUrlComponentValue(&lpUrlComponents->lpszScheme,
		    &lpUrlComponents->dwSchemeLength, lpszUrl, lpszcp - lpszUrl))
	    return FALSE;

        /* Eat ':' in protocol. */
        lpszcp++;

        /* Skip over slashes. */
        if (*lpszcp == '/')
        {
            lpszcp++;
            if (*lpszcp == '/')
            {
                lpszcp++;
                if (*lpszcp == '/')
                    lpszcp++;
            }
        }

        lpszNetLoc = strpbrk(lpszcp, "/");
	if (lpszParam)
        {
	    if (lpszNetLoc)
               lpszNetLoc = min(lpszNetLoc, lpszParam);
        else
               lpszNetLoc = lpszParam;
        }
        else if (!lpszNetLoc)
            lpszNetLoc = lpszcp + strlen(lpszcp);

        /* Parse net-loc */
        if (lpszNetLoc)
        {
	    LPSTR lpszHost;
	    LPSTR lpszPort;

                /* [<user>[<:password>]@]<host>[:<port>] */
            /* First find the user and password if they exist */

            lpszHost = strchr(lpszcp, '@');
            if (lpszHost == NULL || lpszHost > lpszNetLoc)
                {
                /* username and password not specified. */
		SetUrlComponentValue(&lpUrlComponents->lpszUserName,
			&lpUrlComponents->dwUserNameLength, NULL, 0);
		SetUrlComponentValue(&lpUrlComponents->lpszPassword,
			&lpUrlComponents->dwPasswordLength, NULL, 0);
                }
            else /* Parse out username and password */
                {
		LPSTR lpszUser = lpszcp;
		LPSTR lpszPasswd = lpszHost;

		while (lpszcp < lpszHost)
                        {
		   if (*lpszcp == ':')
		       lpszPasswd = lpszcp;

		   lpszcp++;
                    }

		SetUrlComponentValue(&lpUrlComponents->lpszUserName,
			&lpUrlComponents->dwUserNameLength, lpszUser, lpszPasswd - lpszUser);

                if (lpszPasswd != lpszHost)
                    lpszPasswd++;
		SetUrlComponentValue(&lpUrlComponents->lpszPassword,
			&lpUrlComponents->dwPasswordLength,
			lpszPasswd == lpszHost ? NULL : lpszPasswd,
			lpszHost - lpszPasswd);

		lpszcp++; /* Advance to beginning of host */
                }

            /* Parse <host><:port> */

	    lpszHost = lpszcp;
	    lpszPort = lpszNetLoc;

	    while (lpszcp < lpszNetLoc)
		    {
		if (*lpszcp == ':')
                    lpszPort = lpszcp;

		lpszcp++;
                }

            SetUrlComponentValue(&lpUrlComponents->lpszHostName,
               &lpUrlComponents->dwHostNameLength, lpszHost, lpszPort - lpszHost);

	    if (lpszPort != lpszNetLoc)
                lpUrlComponents->nPort = atoi(++lpszPort);
            else
                lpUrlComponents->nPort = 0;
            }
        }

    /* Here lpszcp points to:
     *
     * <protocol>:[//<net_loc>][/path][;<params>][?<query>][#<fragment>]
     *                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     */
    if (lpszcp != 0 && *lpszcp != '\0' && (!lpszParam || lpszcp < lpszParam))
    {
        INT len;

        /* Only truncate the parameter list if it's already been saved
         * in lpUrlComponents->lpszExtraInfo.
         */
        if (lpszParam && lpUrlComponents->dwExtraInfoLength)
            len = lpszParam - lpszcp;
        else
        {
            /* Leave the parameter list in lpszUrlPath.  Strip off any trailing
             * newlines if necessary.
             */
            LPSTR lpsznewline = strchr (lpszcp, '\n');
            if (lpsznewline != NULL)
                len = lpsznewline - lpszcp;
            else
                len = strlen(lpszcp);
        }

        if (!SetUrlComponentValue(&lpUrlComponents->lpszUrlPath,
         &lpUrlComponents->dwUrlPathLength, lpszcp, len))
         return FALSE;
    }
    else
    {
        lpUrlComponents->dwUrlPathLength = 0;
    }

    TRACE("%s: host(%s) path(%s) extra(%s)\n", lpszUrl, lpUrlComponents->lpszHostName,
          lpUrlComponents->lpszUrlPath, lpUrlComponents->lpszExtraInfo);

    return TRUE;
}

