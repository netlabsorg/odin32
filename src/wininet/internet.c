/*
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
#include <tchar.h>
#include <winsock.h>

#ifdef __WIN32OS2__
#include <stdlib.h>
#include <string.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#endif

#include "internet.h"

INTERNET_SCHEME GetInternetScheme(LPSTR lpszScheme);

DEFAULT_DEBUG_CHANNEL(wininet)

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
			break;

		case DLL_PROCESS_DETACH:
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

	TRACE("\n");

	if (dwFlags & INTERNET_FLAG_ASYNC)
	{
	    FIXME("INTERNET_FLAG_ASYNC not supported\n");
	}

	lpwai = HeapAlloc(GetProcessHeap(), 0, sizeof(WININETAPPINFOA));
	if (NULL == lpwai)
		SetLastError(ERROR_OUTOFMEMORY);
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
BOOLAPI InternetGetLastResponseInfoA(LPDWORD lpdwError,
    LPSTR lpszBuffer, LPDWORD lpdwBufferLength)
{
    FIXME("stub!\n");
    return FALSE;
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
	SetLastError(0);

	switch (dwService)
	{
		case INTERNET_SERVICE_FTP:
			rc = FTP_Connect(hInternet, lpszServerName, nServerPort,
				lpszUserName, lpszPassword, dwFlags, dwContext);
			break;

		case INTERNET_SERVICE_HTTP:
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
BOOLAPI InternetFindNextFileA(HINTERNET hFind, LPVOID lpvFindData)
{
	DWORD access;
	LPWIN32_FIND_DATAA lpFindFileData;
	LPWININETFINDNEXTA lpwh = (LPWININETFINDNEXTA) hFind;

	TRACE("\n");
	if (NULL == lpwh || lpwh->hdr.htype != WH_HFINDNEXT)
		return FALSE;

	if (lpwh->hdr.lpwhparent->htype != WH_HFTPSESSION)
	{
	    FIXME("Only FTP find next supported\n");
	    return FALSE;
	}

	TRACE("index(%d) size(%d)\n", lpwh->index, lpwh->size);
	if (lpwh->index >= lpwh->size)
	{
	    SetLastError(ERROR_NO_MORE_FILES);
	    return FALSE;
	}

	lpFindFileData = (LPWIN32_FIND_DATAA) lpvFindData;
	access = mktime(&lpwh->lpafp[lpwh->index].tmLastModified);
		
	/* Not all fields are filled in */
	lpFindFileData->ftLastAccessTime.dwHighDateTime = HIWORD(access);
	lpFindFileData->ftLastAccessTime.dwLowDateTime  = LOWORD(access);
	lpFindFileData->nFileSizeHigh = HIWORD(lpwh->lpafp[lpwh->index].nSize);
	lpFindFileData->nFileSizeLow = LOWORD(lpwh->lpafp[lpwh->index].nSize);
	if (lpwh->lpafp[lpwh->index].lpszName)
		strncpy(lpFindFileData->cFileName, lpwh->lpafp[lpwh->index].lpszName, MAX_PATH);
	lpwh->index++;

	return TRUE;
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
BOOLAPI InternetCloseHandle(HINTERNET hInternet)
{
    	BOOL retval = FALSE;
	LPWININETHANDLEHEADER lpwh = (LPWININETHANDLEHEADER) hInternet;

	TRACE("\n");
	if (NULL == lpwh)
		return FALSE;

	switch (lpwh->htype)
	{
		case WH_HINIT:
		case WH_HHTTPSESSION:
		case WH_HHTTPREQ:
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
 *           InternetCrackUrlA (WININET.95)
 *
 * Break up URL into its components
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI InternetCrackUrlA(LPCSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags, 
		LPURL_COMPONENTSA lpUrlComponents)
{
  /*
   * RFC 1808
   * <protocol>:[//<net_loc>][/path][;<params>][?<query>][#<fragment>]
   *
   */
	char* szScheme   = NULL;
	char* szUser     = NULL;
	char* szPass     = NULL;
	char* szHost     = NULL;
	char* szUrlPath  = NULL;
	char* szParam    = NULL;
	char* szNetLoc   = NULL;
	int   nPort      = 80;
	int   nSchemeLen = 0;
	int   nUserLen   = 0;
	int   nPassLen   = 0;
	int   nHostLen   = 0;
	int   nUrlLen    = 0;

  // Find out if the URI is absolute...
  BOOL  bIsAbsolute = FALSE;
  char  cAlphanum;
  char* ap = (char*)lpszUrl;
  char* cp = NULL;

	TRACE("\n");
  while( (cAlphanum = *ap) != '\0' )
  {
    if( ((cAlphanum >= 'a') && (cAlphanum <= 'z')) ||
        ((cAlphanum >= 'A') && (cAlphanum <= 'Z')) ||
        ((cAlphanum >= '0') && (cAlphanum <= '9')) )
    {
      ap++;
      continue;
    }
    if( (cAlphanum == ':') && (ap - lpszUrl >= 2) )
    {
      bIsAbsolute = TRUE;
      cp = ap;
      break;
    }
    break;
  }

  // Absolute URI...
  //FIXME!!!! This should work on relative urls too!
  if( bIsAbsolute )
  {
    // Get scheme first...
    nSchemeLen = cp - lpszUrl;
    szScheme   = strdup( lpszUrl );
    szScheme[ nSchemeLen ] = '\0';

    // Eat ':' in protocol...
    cp++;

    // Parse <params>...
    szParam = strpbrk( lpszUrl, ";" );
    if( szParam != NULL )
    {
       char* sParam;
       // Eat ';' in Params...
       szParam++;
       sParam    = strdup( szParam );
       *szParam = '\0';
     }

     // Skip over slashes...
     if( *cp == '/' )
     {
      cp++;
      if( *cp == '/' )
      {
        cp++;
        if( *cp == '/' )
          cp++;
      }
     }

     // Parse the <net-loc>...
     if( GetInternetScheme( szScheme ) == INTERNET_SCHEME_FILE )
     {
      szUrlPath = strdup( cp );
      nUrlLen   = strlen( szUrlPath );
      if( nUrlLen >= 2 && szUrlPath[ 1 ] == '|' )
        szUrlPath[ 1 ] = ':';
     }
     else
     {
      size_t nNetLocLen;
      szUrlPath = strpbrk(cp, "/");
      if( szUrlPath != NULL )
	nUrlLen = strlen( szUrlPath );

      // Find the end of our net-loc...
      nNetLocLen = strcspn( cp, "/" );
      szNetLoc   = strdup( cp );
      szNetLoc[ nNetLocLen ] = '\0';

      if( szNetLoc != NULL )
      {
	char* lpszPort;
	int   nPortLen;
	// [<user>[<:password>]@]<host>[:<port>]
	// First find the user and password if they exist...
			
	szHost = strchr( szNetLoc, '@' );
	if( szHost == NULL )
	{
		// username and password not specified...
		szHost   = szNetLoc;
		nHostLen = nNetLocLen;
	}
	else
	{
		int   nUserPassLen = nNetLocLen - nHostLen - 1;
		char* szUserPass         = strdup( szNetLoc );
		// Get username and/or password...
		// Eat '@' in domain...
		++szHost;
		nHostLen = strlen( szHost );

		szUserPass[ nUserPassLen ] = '\0';
		if( szUserPass != NULL )
		{
			szPass = strpbrk( szUserPass, ":" );
			if( szPass != NULL )
			{
				// Eat ':' in UserPass...
				++szPass;
				nPassLen = strlen( szPass );
					nUserLen = nUserPassLen - nPassLen - 1;
				szUser   = strdup( szUserPass );
				szUser[ nUserLen ] = '\0';
			}
			else
			{
				// password not specified...
				szUser = strdup( szUserPass );
				nUserLen = strlen( szUser );
			}
		}
	}

	// <host><:port>...
	// Then get the port if it exists...
	lpszPort = strpbrk( szHost, ":" );
	nPortLen = 0;
	if( lpszPort != NULL )
	{
		char* szPort = lpszPort + 1;
		if( szPort != NULL )
		{
			nPortLen = strlen( szPort );
			nPort    = atoi( szPort );
		}
		*lpszPort = '\0';
		nHostLen = strlen(szHost);
	}
      }
    }
  }
  // Relative URI...
  else
      return FALSE;

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
 *           InternetSetStatusCallback (WININET.133)
 *
 * Sets up a callback function which is called as progress is made
 * during an operation. 
 *
 * RETURNS
 *    Previous callback or NULL 	on success
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
BOOLAPI InternetWriteFile(HINTERNET hFile, LPCVOID lpBuffer ,
	DWORD dwNumOfBytesToWrite, LPDWORD lpdwNumOfBytesWritten)
{
    	BOOL retval = FALSE;
	int nSocket = INVALID_SOCKET;
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
		*lpdwNumOfBytesWritten = WriteDataToStream(nSocket, lpBuffer, dwNumOfBytesToWrite);
		if (*lpdwNumOfBytesWritten < 0)
			*lpdwNumOfBytesWritten = 0;
		else
			retval = TRUE;
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
BOOLAPI InternetReadFile(HINTERNET hFile, LPVOID lpBuffer, 
	DWORD dwNumOfBytesToRead, LPDWORD dwNumOfBytesRead)
{
    	BOOL retval = FALSE;
	int nSocket = INVALID_SOCKET;
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
		*dwNumOfBytesRead = ReadDataFromStream(nSocket, lpBuffer, dwNumOfBytesToRead);
		if (*dwNumOfBytesRead < 0)
			*dwNumOfBytesRead = 0;
		else
			retval = TRUE;
	}

	return retval;
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
INTERNET_SCHEME GetInternetScheme(LPSTR lpszScheme)
{
  if(lpszScheme==NULL)
	return INTERNET_SCHEME_UNKNOWN;

  if( (strcmp("ftp", lpszScheme) == 0) ||
		  (strcmp("FTP", lpszScheme) == 0) )
    return INTERNET_SCHEME_FTP;

  else if( (strcmp("gopher", lpszScheme) == 0) ||
		       (strcmp("GOPHER", lpszScheme) == 0) )
    return INTERNET_SCHEME_GOPHER;

  else if( (strcmp("http", lpszScheme) == 0) ||
		       (strcmp("HTTP", lpszScheme) == 0) )
    return INTERNET_SCHEME_HTTP;

  else if( (strcmp("https", lpszScheme) == 0) ||
		       (strcmp("HTTPS", lpszScheme) == 0) )
    return INTERNET_SCHEME_HTTPS;

  else if( (strcmp("file", lpszScheme) == 0) ||
		       (strcmp("FILE", lpszScheme) == 0) )
    return INTERNET_SCHEME_FILE;

  else if( (strcmp("news", lpszScheme) == 0) ||
		       (strcmp("NEWS", lpszScheme) == 0) )
    return INTERNET_SCHEME_NEWS;

  else if( (strcmp("mailto", lpszScheme) == 0) ||
		       (strcmp("MAILTO", lpszScheme) == 0) )
    return INTERNET_SCHEME_MAILTO;

  else
    return INTERNET_SCHEME_UNKNOWN;
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
int WriteDataToStream(int nDataSocket, LPCVOID Buffer, DWORD BytesToWrite)
{
	if (INVALID_SOCKET == nDataSocket)
	    return SOCKET_ERROR;

	return send(nDataSocket, Buffer, BytesToWrite, 0);
}


/***********************************************************************
 *           ReadDataFromStream (internal)
 *
 * Read data from http server
 *
 * RETURNS
 *
 *   number of characters sent on success
 *   -1 on error
 */
int ReadDataFromStream(int nDataSocket, LPVOID Buffer, DWORD BytesToRead)
{
	if (INVALID_SOCKET == nDataSocket)
	    return SOCKET_ERROR;

	return recv(nDataSocket, Buffer, BytesToRead, 0);
}
