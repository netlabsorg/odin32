/*
 * WININET - Ftp implementation
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
#include <netdb.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#endif

#include "internet.h"

DEFAULT_DEBUG_CHANNEL(wininet)

#define NOACCOUNT 		"noaccount"
#define MAX_REPLY_LEN	 	0x5B4
#define DATA_PACKET_SIZE 	0x2000
#define szCRLF 			"\r\n"
#define MAX_BACKLOG 		5

typedef enum {
  /* FTP commands with arguments. */
  FTP_CMD_ACCT, 
  FTP_CMD_CWD,  
  FTP_CMD_DELE, 
  FTP_CMD_MKD,  
  FTP_CMD_PASS, 
  FTP_CMD_PORT, 
  FTP_CMD_RETR, 
  FTP_CMD_RMD,  
  FTP_CMD_RNFR, 
  FTP_CMD_RNTO, 
  FTP_CMD_STOR, 
  FTP_CMD_TYPE, 
  FTP_CMD_USER, 

  /* FTP commands without arguments. */
  FTP_CMD_ABOR,
  FTP_CMD_LIST,
  FTP_CMD_NLST,
  FTP_CMD_PWD, 
  FTP_CMD_QUIT,
} FTP_COMMAND; 

static const char *szFtpCommands[] = {
  "ACCT",
  "CWD",
  "DELE",
  "MKD",
  "PASS",
  "PORT",
  "RETR",
  "RMD",
  "RNFR",
  "RNTO",
  "STOR",
  "TYPE",
  "USER",
  "ABOR",
  "LIST",
  "NLST",
  "PWD",
  "QUIT",
};

static const char szMonths[] = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";

BOOL FTP_SendCommand(int nSocket, FTP_COMMAND ftpCmd, LPCSTR lpszParam);
BOOL FTP_SendStore(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType);
BOOL FTP_InitDataSocket(LPWININETFTPSESSIONA lpwfs, LPINT nDataSocket);
BOOL FTP_SendData(LPWININETFTPSESSIONA lpwfs, int nDataSocket, HANDLE hFile);
int FTP_ReceiveResponse(int nSocket, LPSTR lpszResponse, DWORD dwResponse);
DWORD FTP_SendRetrieve(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType);
BOOL FTP_RetrieveFileData(LPWININETFTPSESSIONA lpwfs, int nDataSocket, DWORD nBytes, HANDLE hFile);
BOOL FTP_InitListenSocket(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_ConnectToHost(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_SendPassword(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_SendAccount(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_SendType(LPWININETFTPSESSIONA lpwfs, DWORD dwType);
BOOL FTP_SendPort(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_ParsePermission(LPCSTR lpszPermission, LPFILEPROPERTIESA lpfp);
BOOL FTP_ParseDirectory(LPWININETFTPSESSIONA lpwfs, int nSocket, LPFILEPROPERTIESA *lpafp, LPDWORD dwfp);
HINTERNET FTP_ReceiveFileList(LPWININETFTPSESSIONA lpwfs, int nSocket, LPWIN32_FIND_DATAA lpFindFileData);
LPSTR FTP_GetNextLine(int nSocket, LPSTR lpszBuffer, DWORD dwBuffer);

/***********************************************************************
 *           FtpPutFileA (WININET.43)
 *
 * Uploads a file to the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI FtpPutFileA(HINTERNET hConnect, LPCSTR lpszLocalFile,
    LPCSTR lpszNewRemoteFile, DWORD dwFlags, DWORD dwContext)
{
    	HANDLE hFile = (HANDLE)NULL;
	BOOL bSuccess = FALSE;
	DWORD nRC = ERROR_SUCCESS;
	LPWININETAPPINFOA hIC = NULL;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;

	TRACE(" lpszLocalFile(%s) lpszNewRemoteFile(%s)\n", lpszLocalFile, lpszNewRemoteFile);
	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    nRC = ERROR_INTERNET_INCORRECT_HANDLE_TYPE;
	    goto lend;
	}

	/* Open file to be uploaded */
	if (NULL == (hFile = CreateFileA(lpszLocalFile, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0)))
	{
	    SetLastError(ERROR_FILE_NOT_FOUND);
	    nRC = ERROR_FILE_NOT_FOUND;
	    goto lend;
	}

	hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
	if (hIC->lpfnStatusCB)
		hIC->lpfnStatusCB(hConnect, lpwfs->hdr.dwContext, INTERNET_STATUS_SENDING_REQUEST, NULL, 0);

	if (FTP_SendStore(lpwfs, lpszNewRemoteFile, dwFlags))
	{
		int nDataSocket;

		/* Accept connection from ftp server */
		if (FTP_InitDataSocket(lpwfs, &nDataSocket)) 
		{
	    		FTP_SendData(lpwfs, nDataSocket, hFile);
			bSuccess = TRUE;
			close(nDataSocket);
		}
	}

lend:
	if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC  && hIC->lpfnStatusCB)
	{
	    	INTERNET_ASYNC_RESULT iar;
	       
		iar.dwResult = (DWORD)bSuccess;
		iar.dwError = nRC;
		hIC->lpfnStatusCB(hConnect, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
			&iar, sizeof(INTERNET_ASYNC_RESULT));
	}

	if (hFile)
	    CloseHandle(hFile);

	return bSuccess;
}


/***********************************************************************
 *           FtpSetCurrentDirectoryA (WININET.49)
 *
 * Change the working directory on the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI FtpSetCurrentDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory)
{
	int nResCode;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
	LPWININETAPPINFOA hIC = NULL;
	DWORD nRC = ERROR_SUCCESS;
	DWORD bSuccess = FALSE;

	TRACE("lpszDirectory(%s)\n", lpszDirectory);

	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    nRC = ERROR_INTERNET_INCORRECT_HANDLE_TYPE;
	    return FALSE;
	}

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_CWD, lpszDirectory))
		goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		if (nResCode == 250)
			bSuccess = TRUE;
		else
		    ERR("Unable to set directory %s\n", lpszDirectory);
	}

lend:
	hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
	if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
	{
	    	INTERNET_ASYNC_RESULT iar;
	       
		iar.dwResult = (DWORD)bSuccess;
		iar.dwError = nRC;
		hIC->lpfnStatusCB(hConnect, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
			&iar, sizeof(INTERNET_ASYNC_RESULT));
	}
	return bSuccess;
}


/***********************************************************************
 *           FtpCreateDirectoryA (WININET.31)
 *
 * Create new directory on the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI FtpCreateDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory)
{
	int nResCode;
	BOOL bSuccess = FALSE;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;

	TRACE("\n");
	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    return FALSE;
	}

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_MKD, lpszDirectory))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		if (nResCode == 257)
			bSuccess = TRUE;
		else
		    ERR("Unable to create directory: %s\n", lpszDirectory);
	}
lend:
	return bSuccess;
}


/***********************************************************************
 *           FtpFindFirstFileA (WININET.35)
 *
 * Search the specified directory
 *
 * RETURNS
 *    HINTERNET on success
 *    NULL on failure
 *
 */
INTERNETAPI HINTERNET WINAPI FtpFindFirstFileA(HINTERNET hConnect,
    LPCSTR lpszSearchFile, LPWIN32_FIND_DATAA lpFindFileData, DWORD dwFlags, DWORD dwContext)
{
	int nResCode;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
	LPWININETFINDNEXTA hFindNext = NULL;

	TRACE("\n");

	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    return FALSE;
	}

	if (!FTP_InitListenSocket(lpwfs))
	    goto lend;

	if (!FTP_SendType(lpwfs, INTERNET_FLAG_TRANSFER_ASCII))
	    goto lend;

	if (!FTP_SendPort(lpwfs))
	    goto lend;

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_LIST, lpszSearchFile))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
	    if (nResCode == 125 || nResCode == 150)
	    {
		int nDataSocket;

		if (FTP_InitDataSocket(lpwfs, &nDataSocket))
		{
			nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
			if (nResCode == 226)
				SetLastError(ERROR_NO_MORE_FILES);
			else
		        	hFindNext = FTP_ReceiveFileList(lpwfs, nDataSocket, lpFindFileData);
			close(nDataSocket);
		}
	    }
	    else if (nResCode == 226)
	    {
		/* Closing data connection, requested file action successful */
	    }
	    else
	    {
		TRACE("Unable to retrieve directory listing\n");
	    }
	}

lend:
	if (lpwfs->lstnSocket != INVALID_SOCKET)
	    close(lpwfs->lstnSocket);

	return (HINTERNET)hFindNext;
}


/***********************************************************************
 *           FtpGetCurrentDirectoryA (WININET.37)
 *
 * Retrieves the current directory
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI FtpGetCurrentDirectoryA(HINTERNET hFtpSession, LPSTR lpszCurrentDirectory, 
	LPDWORD lpdwCurrentDirectory)
{
	int nResCode;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
	LPWININETAPPINFOA hIC = NULL;
	DWORD bSuccess = FALSE;

	TRACE("\n");

	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    return FALSE;
	}

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_PWD, NULL))
		goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		if (nResCode == 257) /* Extract directory name */
		{
	    		int firstpos, lastpos, len;

			for (firstpos = 0, lastpos = 0; lpwfs->lpszResponseBuffer[lastpos]; lastpos++)
			{
		    		if ('"' == lpwfs->lpszResponseBuffer[lastpos])
		    		{
					if (!firstpos)
						firstpos = lastpos;
					else
						break;
				}
			}

			len = lastpos - firstpos + 1;
			strncpy(lpszCurrentDirectory, &lpwfs->lpszResponseBuffer[firstpos],
				len < *lpdwCurrentDirectory ? len : *lpdwCurrentDirectory);
			lpszCurrentDirectory[len] = '\0';
			*lpdwCurrentDirectory = len;
			bSuccess = TRUE;
		}
		else
			ERR("Unable to get current directory\n");
	}

lend:
	hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
	if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
	{
	    	INTERNET_ASYNC_RESULT iar;
	       
		iar.dwResult = (DWORD)bSuccess;
		iar.dwError = bSuccess ? ERROR_SUCCESS : ERROR_SUCCESS;
		hIC->lpfnStatusCB(hFtpSession, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
			&iar, sizeof(INTERNET_ASYNC_RESULT));

	}

	return (DWORD) bSuccess;
}


/***********************************************************************
 *           FtpOpenFileA (WININET.41)
 *
 * Open a remote file for writing or reading
 *
 * RETURNS
 *    HINTERNET handle on success
 *    NULL on failure
 *
 */
INTERNETAPI HINTERNET WINAPI FtpOpenFileA(HINTERNET hFtpSession,
	LPCSTR lpszFileName, DWORD fdwAccess, DWORD dwFlags,
	DWORD dwContext)
{
	int nDataSocket;
    	DWORD nSuccess = 0;
	LPWININETFILE hFile = NULL;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;

	TRACE("\n");
	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    return FALSE;
	}

	if (GENERIC_READ == fdwAccess)
	{
		/* Set up socket to retrieve data */
		nSuccess = FTP_SendRetrieve(lpwfs, lpszFileName, dwFlags);
	}
	else if (GENERIC_WRITE == fdwAccess)
	{
		/* Set up socket to send data */
		nSuccess = FTP_SendStore(lpwfs, lpszFileName, dwFlags);
	}

	/* Accept connection from server */ 
	if (nSuccess && FTP_InitDataSocket(lpwfs, &nDataSocket)) 
	{
		hFile = HeapAlloc(GetProcessHeap(), 0, sizeof(WININETFILE));
		hFile->hdr.htype = WH_HFILE;
		hFile->hdr.dwFlags = dwFlags;
		hFile->hdr.dwContext = dwContext;
		hFile->hdr.lpwhparent = hFtpSession;
		hFile->nDataSocket = nDataSocket;
	}

	return (HINTERNET)hFile;
}


/***********************************************************************
 *           FtpGetFileA (WININET.39)
 *
 * Retrieve file from the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI FtpGetFileA(HINTERNET hInternet, LPCSTR lpszRemoteFile, LPCSTR lpszNewFile,
	BOOL fFailIfExists, DWORD dwLocalFlagsAttribute, DWORD dwInternetFlags,
	DWORD dwContext)
{
    	DWORD nBytes;
	BOOL bSuccess = FALSE;
	HANDLE hFile;
	LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hInternet;

	TRACE("lpszRemoteFile(%s) lpszNewFile(%s)\n", lpszRemoteFile, lpszNewFile);
	if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
	{
	    SetLastError(ERROR_INVALID_HANDLE);
	    return FALSE;
	}

	/* Ensure we can write to lpszNewfile by opening it */
	hFile = CreateFileA(lpszNewFile, GENERIC_WRITE, 0, 0, fFailIfExists ? 
		CREATE_NEW : CREATE_ALWAYS, dwLocalFlagsAttribute, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	    return FALSE;

	/* Set up socket to retrieve data */
	nBytes = FTP_SendRetrieve(lpwfs, lpszRemoteFile, dwInternetFlags);

	if (nBytes > 0)
	{
		int nDataSocket;

		/* Accept connection from ftp server */
		if (FTP_InitDataSocket(lpwfs, &nDataSocket)) 
		{
		    	int nResCode;

			/* Receive data */
			FTP_RetrieveFileData(lpwfs, nDataSocket, nBytes, hFile);
			nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
			if (nResCode)
			{
				if (nResCode == 226)
					bSuccess = TRUE;

			}
			close(nDataSocket);
		}
	}

	if (hFile)
		CloseHandle(hFile);    
	return bSuccess;
}


/***********************************************************************
 *           FTP_Connect (internal)
 *
 * Connect to a ftp server
 *
 * RETURNS
 *   HINTERNET a session handle on success
 *   NULL on failure
 *
 */

HINTERNET FTP_Connect(HINTERNET hInternet, LPCSTR lpszServerName, 
	INTERNET_PORT nServerPort, LPCSTR lpszUserName,
	LPCSTR lpszPassword, DWORD dwFlags, DWORD dwContext)
{
	struct sockaddr_in socketAddr;
	struct hostent *phe = NULL;
	int nsocket = INVALID_SOCKET;
	LPWININETAPPINFOA hIC = NULL;
	BOOL result, bSuccess = FALSE;
	LPWININETFTPSESSIONA lpwfs = NULL;

	TRACE(" Server(%s) Port(%d) User(%s)\n", lpszServerName, nServerPort,
		lpszUserName);

	if (((LPWININETHANDLEHEADER)hInternet)->htype != WH_HINIT)
		goto lerror;

	hIC = (LPWININETAPPINFOA) hInternet;

	if (NULL == lpszUserName && NULL != lpszPassword)
		goto lerror;

	if (hIC->lpfnStatusCB)
		hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_RESOLVING_NAME,
			lpszServerName, strlen(lpszServerName));

	if (!GetAddress(lpszServerName, nServerPort, &phe, &socketAddr))
		goto lerror;

	if (hIC->lpfnStatusCB)
		hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_NAME_RESOLVED,
			lpszServerName, strlen(lpszServerName));

	if (INVALID_SOCKET == (nsocket = socket(phe->h_addrtype,SOCK_STREAM,0)))
	{
	    	ERR("Unable to create socket\n");
	    	goto lerror;
	}

	result = connect(nsocket, (struct sockaddr *)&socketAddr,
			sizeof(socketAddr));

	if (!result)
	{
	        TRACE("Connected to server\n"); 
		lpwfs = HeapAlloc(GetProcessHeap(), 0, sizeof(WININETFTPSESSIONA));
		if (NULL == lpwfs)
		{
		    SetLastError(ERROR_OUTOFMEMORY);
		    goto lerror;
		}

		lpwfs->hdr.htype = WH_HFTPSESSION;
		lpwfs->hdr.dwFlags = dwFlags;
		lpwfs->hdr.dwContext = dwContext;
		lpwfs->hdr.lpwhparent = (LPWININETHANDLEHEADER)hInternet;
		lpwfs->sndSocket = nsocket;
		memcpy(&lpwfs->socketAddress, &socketAddr, sizeof(socketAddr));
		lpwfs->phostent = phe;

		if (NULL == lpszUserName)
		{
		    lpwfs->lpszUserName = strdup("anonymous");
		    lpwfs->lpszPassword = strdup("user@server");
		}
		else
		{
		    lpwfs->lpszUserName = strdup(lpszUserName);
		    lpwfs->lpszPassword = strdup(lpszPassword);
		}

		lpwfs->lpszResponseBuffer = HeapAlloc(GetProcessHeap(), 0, MAX_REPLY_LEN);
		if (NULL == lpwfs)
		{
		    SetLastError(ERROR_OUTOFMEMORY);
		    goto lerror;
		}

		if (hIC->lpfnStatusCB)
			hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_CONNECTING_TO_SERVER,
				&lpwfs->socketAddress, sizeof(struct sockaddr_in));

		if (FTP_ConnectToHost(lpwfs))
		{
			if (hIC->lpfnStatusCB)
			{
	    			INTERNET_ASYNC_RESULT iar;

				hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_CONNECTED_TO_SERVER,
					&lpwfs->socketAddress, sizeof(struct sockaddr_in));

				iar.dwResult = (DWORD)lpwfs;
				iar.dwError = ERROR_SUCCESS;

				hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_HANDLE_CREATED,
					&iar, sizeof(INTERNET_ASYNC_RESULT));
			}
		    	TRACE("Successfully logged into server\n");
			bSuccess = TRUE;
		}
	}

lerror:
	if (!bSuccess && INVALID_SOCKET != nsocket)
		close(nsocket);

	if (!bSuccess && lpwfs)
	{
		HeapFree(GetProcessHeap(), 0, lpwfs);
		lpwfs = NULL;
	}

	if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
	{
	    	INTERNET_ASYNC_RESULT iar;
	       
		iar.dwResult = (DWORD)lpwfs;
		iar.dwError = bSuccess ? ERROR_SUCCESS : ERROR_INTERNET_CANNOT_CONNECT;
		hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
			&iar, sizeof(INTERNET_ASYNC_RESULT));

	}

	return (HINTERNET) lpwfs;
}


/***********************************************************************
 *           FTP_ConnectHost (internal)
 *
 * Connect to a ftp server
 *
 * RETURNS
 *   TRUE on success
 *   NULL on failure
 *
 */
BOOL FTP_ConnectToHost(LPWININETFTPSESSIONA lpwfs)
{
	int nResCode;
	BOOL bSuccess = FALSE;

	TRACE("\n");
	FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_USER, lpwfs->lpszUserName))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		/* Login successful... */
		if (nResCode == 230)
			bSuccess = TRUE;
		/* User name okay, need password... */
		else if (nResCode == 331)
			bSuccess = FTP_SendPassword(lpwfs);
		/* Need account for login... */
		else if (nResCode == 332)
		    	bSuccess = FTP_SendAccount(lpwfs);
	}

	TRACE("Returning %d\n", bSuccess);
lend:
	return bSuccess;
}


/***********************************************************************
 *           FTP_SendCommand (internal)
 *
 * Send command to server
 *
 * RETURNS
 *   TRUE on success
 *   NULL on failure
 *
 */
BOOL FTP_SendCommand(int nSocket, FTP_COMMAND ftpCmd, LPCSTR lpszParam)
{
    	int len;
	char *buf;
	int nBytesSent = 0;
	int nRC        = 0;
	BOOL bParamHasLen;

	TRACE("%d: (%s)\n", ftpCmd, lpszParam);

	bParamHasLen = lpszParam && strlen(lpszParam) > 0;
	len = (bParamHasLen ? strlen(lpszParam) : -1) + strlen(szFtpCommands[ftpCmd]) +
	    strlen(szCRLF)+ 1;
	if (NULL == (buf = HeapAlloc(GetProcessHeap(), 0, len+1)))
	{
	    SetLastError(ERROR_OUTOFMEMORY);
	    return FALSE;
	}
	sprintf(buf, "%s%s%s%s", szFtpCommands[ftpCmd], bParamHasLen ? " " : "", 
		bParamHasLen ? lpszParam : "", szCRLF);

	TRACE("Sending (%s)\n", buf);
	while((nBytesSent < len) && (nRC != SOCKET_ERROR))
	{
		nRC = send(nSocket, buf+nBytesSent, len - nBytesSent, 0);
    		/* Here, we shall call the callback function to update the status. */
		nBytesSent += nRC;
	}

	HeapFree(GetProcessHeap(), 0, buf);
	return (nRC != SOCKET_ERROR);
}


/***********************************************************************
 *           FTP_ReceiveResponse (internal)
 *
 * Receive response from server
 *
 * RETURNS
 *   Reply code on success
 *   0 on failure
 *
 */

int FTP_ReceiveResponse(int nSocket, LPSTR lpszResponse, DWORD dwResponse)
{
    	int nRecv = 0;
	char resp[4];
	int rc = 0;

	TRACE("\n");
	while(1)
	{
		while (nRecv < dwResponse)
		{
			if (recv(nSocket, &lpszResponse[nRecv], 1, 0) < 0)
			    goto lerror;

			if (lpszResponse[nRecv] == '\n')
			    break;
			if (lpszResponse[nRecv] != '\r')
			    nRecv++;
		}

		if (nRecv > 3 && lpszResponse[3] != '-')
		    break;

		nRecv = 0;
	}
		   
	if (nRecv > 0)
	{
		memset(lpszResponse+nRecv-1, 0, dwResponse-nRecv);
		memcpy(resp, lpszResponse, 3*sizeof(char));
		resp[3] = '\0';
		rc = atoi(resp);
		TRACE(" Reply(%d) bytes(%d) %s\n", rc, nRecv, lpszResponse);
	}

lerror:
	return rc;
}


/***********************************************************************
 *           FTP_SendPassword (internal)
 *
 * Send password to ftp server
 *
 * RETURNS
 *   TRUE on success
 *   NULL on failure
 *
 */
BOOL FTP_SendPassword(LPWININETFTPSESSIONA lpwfs)
{
	int nResCode;
	BOOL bSuccess = FALSE;

	TRACE("\n");
	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_PASS, lpwfs->lpszPassword))
	    goto lend;
	   
	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
	    	TRACE("Received reply code %d\n", nResCode);
		/* Login successful... */
		if (nResCode == 230)
			bSuccess = TRUE;
		/* Command not implemented, superfluous at the server site... */
		/* Need account for login... */
		else if (nResCode == 332)
			bSuccess = FTP_SendAccount(lpwfs);
		else
			TRACE("Password failed\n");
	}
lend:
	TRACE("Returning %d\n", bSuccess);
	return bSuccess;
}


/***********************************************************************
 *           FTP_SendAccount (internal)
 *
 * 
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_SendAccount(LPWININETFTPSESSIONA lpwfs)
{
	int nResCode;
	BOOL bSuccess = FALSE;

	TRACE("\n");
	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_ACCT, NOACCOUNT))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
	    bSuccess = TRUE;
	}

lend:
	return bSuccess;
}


/***********************************************************************
 *           FTP_SendStore (internal)
 *
 * Send request to upload file to ftp server
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_SendStore(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType)
{
	int nResCode;
	BOOL bSuccess = FALSE;

	TRACE("\n");
	if (!FTP_InitListenSocket(lpwfs))
	    goto lend;

	if (!FTP_SendType(lpwfs, dwType))
	    goto lend;

	if (!FTP_SendPort(lpwfs))
	    goto lend;

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_STOR, lpszRemoteFile))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		if (nResCode == 150)
		    bSuccess = TRUE;
	}

lend:
	if (!bSuccess && INVALID_SOCKET != lpwfs->lstnSocket)
	{
	    close(lpwfs->lstnSocket);
	    lpwfs->lstnSocket = INVALID_SOCKET;
	}

	return bSuccess;
}


/***********************************************************************
 *           FTP_InitListenSocket (internal)
 *
 * Create a socket to listen for server response
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_InitListenSocket(LPWININETFTPSESSIONA lpwfs)
{
	BOOL bSuccess = FALSE;
	socklen_t namelen = sizeof(struct sockaddr_in);

	TRACE("\n");

	lpwfs->lstnSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == lpwfs->lstnSocket)
	{
	    	TRACE("Unable to create listening socket\n");
		goto lend;
	}

	lpwfs->lstnSocketAddress.sin_family = AF_INET;
	lpwfs->lstnSocketAddress.sin_port = htons((u_short) 0);
	lpwfs->lstnSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); 
	if (SOCKET_ERROR == bind(lpwfs->lstnSocket,&lpwfs->lstnSocketAddress, sizeof(struct sockaddr_in)))
	{
		TRACE("Unable to bind socket: %d\n", errno);
		goto lend;
	}

	if (SOCKET_ERROR == listen(lpwfs->lstnSocket, MAX_BACKLOG))
	{
	    	TRACE("listen failed\n");
		goto lend;
	}

	if (SOCKET_ERROR != getsockname(lpwfs->lstnSocket, &lpwfs->lstnSocketAddress, &namelen))
		bSuccess = TRUE;

lend:
	if (!bSuccess && INVALID_SOCKET == lpwfs->lstnSocket)
	{
		close(lpwfs->lstnSocket);
		lpwfs->lstnSocket = INVALID_SOCKET;
	}

	return bSuccess;
}


/***********************************************************************
 *           FTP_SendType (internal)
 *
 * Tell server type of data being transfered
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_SendType(LPWININETFTPSESSIONA lpwfs, DWORD dwType)
{
	int nResCode;
	char type[2] = { "I\0" };
	BOOL bSuccess = FALSE;

	TRACE("\n");
	if (dwType & INTERNET_FLAG_TRANSFER_ASCII)
	    *type = 'A';

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_TYPE, type))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN)/100;
	if (nResCode)
	{
		if (nResCode == 2)
			bSuccess = TRUE;
		else if (nResCode == 4)
		{
			/* Possible to restart */
		}
		else if (nResCode == 5)
		{
			/* Nothing can be done here */
		}
	}

lend:
	return bSuccess;
}


/***********************************************************************
 *           FTP_SendPort (internal)
 *
 * Tell server which port to use
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_SendPort(LPWININETFTPSESSIONA lpwfs)
{
	int nResCode;
	char szIPAddress[64];
	BOOL bSuccess = FALSE;

	TRACE("\n");

	sprintf(szIPAddress, "%d,%d,%d,%d,%d,%d",
		lpwfs->socketAddress.sin_addr.s_addr&0x000000FF,
		(lpwfs->socketAddress.sin_addr.s_addr&0x0000FF00)>>8,
		(lpwfs->socketAddress.sin_addr.s_addr&0x00FF0000)>>16,
		(lpwfs->socketAddress.sin_addr.s_addr&0xFF000000)>>24,
		lpwfs->lstnSocketAddress.sin_port & 0xFF,
		(lpwfs->lstnSocketAddress.sin_port & 0xFF00)>>8);

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_PORT, szIPAddress))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		if (nResCode == 200)
			bSuccess = TRUE;
		else
		{
	    		TRACE("SendPort failed\n");
		}
	}

lend:
	return bSuccess;
}


/***********************************************************************
 *           FTP_InitDataSocket (internal)
 *
 * 
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_InitDataSocket(LPWININETFTPSESSIONA lpwfs, LPINT nDataSocket)
{
	struct sockaddr_in saddr;
	socklen_t addrlen = sizeof(struct sockaddr);

	TRACE("\n");
	*nDataSocket = accept(lpwfs->lstnSocket, &saddr, &addrlen);
	close(lpwfs->lstnSocket);
	lpwfs->lstnSocket = INVALID_SOCKET;

	return *nDataSocket != INVALID_SOCKET;
}


/***********************************************************************
 *           FTP_SendData (internal)
 *
 * Send data to the server
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_SendData(LPWININETFTPSESSIONA lpwfs, int nDataSocket, HANDLE hFile)
{
    	BY_HANDLE_FILE_INFORMATION fi;
	DWORD nBytesRead = 0;
	DWORD nBytesSent = 0;
	DWORD nTotalSent = 0;
	DWORD nBytesToSend, nLen, nRC = 1;
	time_t s_long_time, e_long_time;
	long nSeconds;
	char *lpszBuffer;

	TRACE("\n");
	lpszBuffer = HeapAlloc(GetProcessHeap(), 0, sizeof(char)*DATA_PACKET_SIZE);
	memset(lpszBuffer, 0, sizeof(char)*DATA_PACKET_SIZE);

	/* Get the size of the file. */
	GetFileInformationByHandle(hFile, &fi);
	time(&s_long_time);

	do
	{
		nBytesToSend = nBytesRead - nBytesSent;

		if (nBytesToSend <= 0)
		{
			/* Read data from file. */
			nBytesSent = 0;
			if (!ReadFile(hFile, lpszBuffer, DATA_PACKET_SIZE, &nBytesRead, 0))
			    ERR("Failed reading from file\n");

			if (nBytesRead > 0)
				nBytesToSend = nBytesRead;
			else
				break;
		}

		nLen = DATA_PACKET_SIZE < nBytesToSend ? 
		    DATA_PACKET_SIZE : nBytesToSend;
		nRC  = send(nDataSocket, lpszBuffer, nLen, 0);

		if (nRC != SOCKET_ERROR)
		{
			nBytesSent += nRC;
			nTotalSent += nRC;
		}

		/* Do some computation to display the status. */
		time(&e_long_time);
		nSeconds = e_long_time - s_long_time;
		if( nSeconds / 60 > 0 )
		{
/*
			TRACE( "%d bytes of %d bytes (%d%%) in %d min %d sec estimated remainig time %d sec\t\t\r",
				nTotalSent, fi.nFileSizeLow, nTotalSent*100/st.st_size, nSeconds / 60, 
				nSeconds % 60, (st.st_size - nTotalSent) * nSeconds / nTotalSent );
*/
		}
		else
		{
/*
			TRACE( "%d bytes of %d bytes (%d%%) in %d sec estimated remainig time %d sec\t\t\r",
				nTotalSent, fi.nFileSizeLow, nTotalSent*100/st.st_size, nSeconds,
				(st.st_size - nTotalSent) * nSeconds / nTotalSent);
*/

		}
	} while (nRC != SOCKET_ERROR);

	TRACE("file transfer complete!\n");

	if(lpszBuffer != NULL)
		HeapFree(GetProcessHeap(), 0, lpszBuffer);

	return nTotalSent;
}


/***********************************************************************
 *           FTP_SendRetrieve (internal)
 *
 * Send request to retrieve a file
 *
 * RETURNS
 *   Number of bytes to be received on success
 *   0 on failure
 *
 */
DWORD FTP_SendRetrieve(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType)
{
	int nResCode;
	DWORD nResult = 0;

	TRACE("\n");
	if (!FTP_InitListenSocket(lpwfs))
	    goto lend;

	if (!FTP_SendType(lpwfs, dwType))
	    goto lend;

	if (!FTP_SendPort(lpwfs))
	    goto lend;

	if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_RETR, lpszRemoteFile))
	    goto lend;

	nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);
	if (nResCode)
	{
		if (nResCode == 125 || nResCode == 150)
		{
		        /* Parse size of data to be retrieved */
	    		int i, sizepos = -1;
			for (i = strlen(lpwfs->lpszResponseBuffer) - 1; i >= 0; i--)
			{
				if ('(' == lpwfs->lpszResponseBuffer[i])
				{
					sizepos = i;
					break;
				}
			}

			if (sizepos >= 0)
			{
		    		nResult = atol(&lpwfs->lpszResponseBuffer[sizepos+1]);
				TRACE("Waiting to receive %d bytes\n", nResult);
			}
		}
	}

lend:
	if (0 == nResult && INVALID_SOCKET != lpwfs->lstnSocket)
	{
	    close(lpwfs->lstnSocket);
	    lpwfs->lstnSocket = INVALID_SOCKET;
	}

	return nResult;
}


/***********************************************************************
 *           FTP_RetrieveData  (internal)
 *
 * Retrieve data from server
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_RetrieveFileData(LPWININETFTPSESSIONA lpwfs, int nDataSocket, DWORD nBytes, HANDLE hFile)
{
	DWORD nBytesWritten;
	DWORD nBytesReceived = 0;
	int nRC = 0;
	char *lpszBuffer;

	TRACE("\n");

	if (INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	lpszBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(char)*DATA_PACKET_SIZE);
	if (NULL == lpszBuffer)
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return FALSE;
	}

	while (nBytesReceived < nBytes && nRC != SOCKET_ERROR)
	{
		nRC = recv(nDataSocket, lpszBuffer, DATA_PACKET_SIZE, 0);
		if (nRC != SOCKET_ERROR)
		{
			/* other side closed socket. */
			if (nRC == 0)
				goto recv_end;
			WriteFile(hFile, lpszBuffer, nRC, &nBytesWritten, NULL); 
			nBytesReceived += nRC;
		}

		TRACE("%d bytes of %ld (%ld%%)\r", nBytesReceived, nBytes, 
                  nBytesReceived * 100 / nBytes);
	}

	TRACE("Data transfer complete\n");
	if (NULL != lpszBuffer)
	    HeapFree(GetProcessHeap(), 0, lpszBuffer);

recv_end:
	return  (nRC != SOCKET_ERROR);
}


/***********************************************************************
 *           FTP_CloseSessionHandle (internal)
 *
 * Deallocate session handle
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_CloseSessionHandle(LPWININETFTPSESSIONA lpwfs)
{
    if (INVALID_SOCKET != lpwfs->sndSocket)
	close(lpwfs->sndSocket);

    if (INVALID_SOCKET != lpwfs->lstnSocket)
	close(lpwfs->lstnSocket);

    if (lpwfs->lpszPassword)
	HeapFree(GetProcessHeap(), 0, lpwfs->lpszPassword);

    if (lpwfs->lpszUserName)
	HeapFree(GetProcessHeap(), 0, lpwfs->lpszUserName);

    if (lpwfs->lpszResponseBuffer)
	HeapFree(GetProcessHeap(), 0, lpwfs->lpszResponseBuffer);

    HeapFree(GetProcessHeap(), 0, lpwfs);

    return TRUE;
}


/***********************************************************************
 *           FTP_CloseSessionHandle (internal)
 *
 * Deallocate session handle
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_CloseFindNextHandle(LPWININETFINDNEXTA lpwfn)
{
    int i; 

    for (i = 0; i < lpwfn->size; i++)
    {
	if (NULL != lpwfn->lpafp[i].lpszName)
	    HeapFree(GetProcessHeap(), 0, lpwfn->lpafp[i].lpszName);
    }

    HeapFree(GetProcessHeap(), 0, lpwfn->lpafp);
    HeapFree(GetProcessHeap(), 0, lpwfn);

    return TRUE;
}


/***********************************************************************
 *           FTP_ReceiveFileList (internal)
 *
 * Read file list from server
 *
 * RETURNS
 *   Handle to file list on success
 *   NULL on failure
 *
 */
HINTERNET FTP_ReceiveFileList(LPWININETFTPSESSIONA lpwfs, int nSocket, LPWIN32_FIND_DATAA lpFindFileData)
{
	DWORD dwSize;
	LPFILEPROPERTIESA lpafp = NULL;
	LPWININETFINDNEXTA lpwfn = NULL;

	TRACE("\n");

	if (FTP_ParseDirectory(lpwfs, nSocket, &lpafp, &dwSize))
	{
		DWORD access = mktime(&lpafp->tmLastModified);
		
		/* Not all fields are filled in */
		lpFindFileData->ftLastAccessTime.dwHighDateTime = HIWORD(access);
		lpFindFileData->ftLastAccessTime.dwLowDateTime  = LOWORD(access);
		lpFindFileData->nFileSizeHigh = HIWORD(lpafp->nSize);
		lpFindFileData->nFileSizeLow = LOWORD(lpafp->nSize);
		strncpy(lpFindFileData->cFileName, lpafp->lpszName, MAX_PATH);

		lpwfn = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WININETFINDNEXTA));
		if (NULL != lpwfn)
		{
		    lpwfn->hdr.htype = WH_HFINDNEXT;
		    lpwfn->hdr.lpwhparent = (LPWININETHANDLEHEADER)lpwfs;
		    lpwfn->index = 1;
		    lpwfn->size = dwSize;
		    lpwfn->lpafp = lpafp;
		}
	} 

	return (HINTERNET)lpwfn;
}


/***********************************************************************
 *           FTP_ParseDirectory (internal)
 *
 * Parse string of directory information
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
#define MAX_MONTH_LEN 10

BOOL FTP_ParseDirectory(LPWININETFTPSESSIONA lpwfs, int nSocket, LPFILEPROPERTIESA *lpafp, LPDWORD dwfp)
{
  /*
   * <Permissions> <NoLinks> <owner>   <group> <size> <date>  <time or year> <filename>
   *
   * For instance:
   * drwx--s---     2         pcarrier  ens     512    Sep 28  1995           pcarrier
   */
	char* pszMinutes;
	char* pszHour;
	time_t aTime;
	struct tm* apTM;
	char pszMonth[MAX_MONTH_LEN];
	char* pszMatch;
	BOOL bSuccess = TRUE;
	LPFILEPROPERTIESA curFileProp = NULL;
	char* pszLine  = NULL;
	char* pszToken = NULL;
	int nTokenToSkip = 3;
	int nCount = 0;
	int nSeconds = 0;
	int nMinutes = 0;
	int nHour    = 0;
	int nDay     = 0;
	int nMonth   = 0;
	int nYear    = 0;
	int sizeFilePropArray = 20;
	int indexFilePropArray = 0;

	TRACE("\n");

	/* Skip over the first line */
	FTP_GetNextLine(nSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN);

	/* Allocate intial file properties array */
	*lpafp = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FILEPROPERTIESA)*(sizeFilePropArray));
	if (NULL == lpafp)
	{
	    bSuccess = FALSE;
	    goto lend;
	}

	while ((pszLine = FTP_GetNextLine(nSocket, lpwfs->lpszResponseBuffer, MAX_REPLY_LEN)) != NULL)
	{
	    	if (sizeFilePropArray < indexFilePropArray)
		{
		    	LPFILEPROPERTIESA tmpafp;

			sizeFilePropArray *= 2;
			tmpafp = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *lpafp, 
				sizeof(FILEPROPERTIESA)*sizeFilePropArray);
			if (NULL == tmpafp)
			{
			    bSuccess = FALSE;
			    goto lend;
			}

			*lpafp = tmpafp;
		}

		curFileProp = &((*lpafp)[indexFilePropArray]);

		/* First Parse the permissions. */
		pszToken = strtok(pszLine, " \t" );
		FTP_ParsePermission(pszToken, curFileProp);

		nTokenToSkip = 3;
		nCount = 0;
    		do
    		{
      			pszToken = strtok( NULL, " \t" );
      			nCount++;
    		} while( nCount <= nTokenToSkip );

		/* Store the size of the file in the param list. */
		if (pszToken != NULL)
			curFileProp->nSize = atol(pszToken);

		/* Parse last modified time. */
		nSeconds = 0;
		nMinutes = 0;
		nHour    = 0;
		nDay     = 0;
		nMonth   = 0;
		nYear    = 0;

		pszToken = strtok( NULL, " \t" );
		strncpy(pszMonth, pszToken, MAX_MONTH_LEN);
    		strupr(pszMonth);
		pszMatch = strstr(szMonths, pszMonth);
		if( pszMatch != NULL )
			nMonth = (pszMatch - szMonths) / 3;

		pszToken = strtok(NULL, " \t");
		if (pszToken != NULL)
			nDay = atoi(pszToken);

		pszToken = strtok(NULL, " \t");
		pszMinutes = strchr(pszToken, ':');
		if( pszMinutes != NULL )
		{
			pszMinutes++;
			nMinutes = atoi(pszMinutes);
      			pszHour = pszMinutes - 3;
			if (pszHour != NULL)
				nHour = atoi(pszHour);
			time(&aTime);
      			apTM = localtime( &aTime );
			nYear = apTM->tm_year;
    		}
		else
		{
			nYear  = atoi(pszToken);
			nYear -= 1900;
			nHour  = 12;
		}

		curFileProp->tmLastModified.tm_sec  = nSeconds;
		curFileProp->tmLastModified.tm_min  = nMinutes;
		curFileProp->tmLastModified.tm_hour = nHour;
		curFileProp->tmLastModified.tm_mday = nDay;
		curFileProp->tmLastModified.tm_mon  = nMonth;
		curFileProp->tmLastModified.tm_year = nYear;

    		pszToken = strtok(NULL, " \t");
		if(pszToken != NULL)
		{
			curFileProp->lpszName = strdup(pszToken);
			TRACE(": %s\n", curFileProp->lpszName);
		}

		indexFilePropArray++;
	}

	if (bSuccess)
	{
		if (indexFilePropArray < sizeFilePropArray - 1)
		{
			LPFILEPROPERTIESA tmpafp;

			tmpafp = HeapReAlloc(GetProcessHeap(), 0, *lpafp, 
				sizeof(FILEPROPERTIESA)*indexFilePropArray);
			if (NULL == tmpafp)
				*lpafp = tmpafp;
		}
		*dwfp = indexFilePropArray;
	}
	else
	{
		HeapFree(GetProcessHeap(), 0, *lpafp);
	}

lend:	
  return bSuccess;
}


/***********************************************************************
 *           FTP_ParsePermission (internal)
 *
 * Parse permission string of directory information
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_ParsePermission(LPCSTR lpszPermission, LPFILEPROPERTIESA lpfp)
{
	BOOL bSuccess = TRUE;
	unsigned short nPermission = 0;
	int nPos = 1;
	int nLast  = 9;

	TRACE("\n");
	if ((*lpszPermission != 'd') && (*lpszPermission != '-') && (*lpszPermission != 'l'))
	{
		bSuccess = FALSE;
		return bSuccess;
	}

	lpfp->bIsDirectory = (*lpszPermission == 'd');
	do
	{
		switch (nPos)
		{
			case 1:
				nPermission |= (*(lpszPermission+1) == 'r' ? 1 : 0) << 8;
				break;
			case 2:
				nPermission |= (*(lpszPermission+2) == 'w' ? 1 : 0) << 7;
				break;
			case 3:
				nPermission |= (*(lpszPermission+3) == 'x' ? 1 : 0) << 6;
				break;
			case 4:
				nPermission |= (*(lpszPermission+4) == 'r' ? 1 : 0) << 5;
				break;
			case 5:
				nPermission |= (*(lpszPermission+5) == 'w' ? 1 : 0) << 4;
				break;
			case 6:
				nPermission |= (*(lpszPermission+6) == 'x' ? 1 : 0) << 3;
				break;
			case 7:
				nPermission |= (*(lpszPermission+7) == 'r' ? 1 : 0) << 2;
				break;
			case 8:
				nPermission |= (*(lpszPermission+8) == 'w' ? 1 : 0) << 1;
				break;
			case 9:
				nPermission |= (*(lpszPermission+9) == 'x' ? 1 : 0);
				break;
		}
		nPos++;
	}while (nPos <= nLast);

	lpfp->permissions = nPermission;
	return bSuccess;
}


/***********************************************************************
 *           FTP_GetNextLine  (internal)
 *
 * Parse next line in directory string listing
 *
 * RETURNS
 *   Pointer to begining of next line
 *   NULL on failure
 *
 */

LPSTR FTP_GetNextLine(int nSocket, LPSTR lpszBuffer, DWORD dwBuffer)
{
    	BOOL bSuccess = FALSE;
	int nRecv = 0;

	TRACE("\n");
	while (nRecv < dwBuffer)
	{
		if (recv(nSocket, &lpszBuffer[nRecv], 1, 0) < 0)
		    goto lend;
		if (lpszBuffer[nRecv] == '\n')
		{
		    bSuccess = TRUE;
		    break;
		}
		if (lpszBuffer[nRecv] != '\r')
		    nRecv++;
	}

lend:
	if (bSuccess)
	{
		lpszBuffer[nRecv] = '\0';
		TRACE(": %s\n", lpszBuffer);
		return lpszBuffer;
	}
	else
	    return NULL;
}

