/* $Id: ftp.c,v 1.4 2000-12-27 23:15:05 sandervl Exp $
 *
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

#define NOACCOUNT               "noaccount"
#define DATA_PACKET_SIZE        0x2000
#define szCRLF                  "\r\n"
#define MAX_BACKLOG             5

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

BOOL FTP_SendCommand(int nSocket, FTP_COMMAND ftpCmd, LPCSTR lpszParam,
        INTERNET_STATUS_CALLBACK lpfnStatusCB, HINTERNET hHandle, DWORD dwContext);
BOOL FTP_SendStore(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType);
BOOL FTP_InitDataSocket(LPWININETFTPSESSIONA lpwfs, LPINT nDataSocket);
BOOL FTP_SendData(LPWININETFTPSESSIONA lpwfs, int nDataSocket, HANDLE hFile);
int FTP_ReceiveResponse(int nSocket, LPSTR lpszResponse, DWORD dwResponse,
     INTERNET_STATUS_CALLBACK lpfnStatusCB, HINTERNET hHandle, DWORD dwContext);
DWORD FTP_SendRetrieve(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType, LONG *lBytesAvailable);
BOOL FTP_RetrieveFileData(LPWININETFTPSESSIONA lpwfs, int nDataSocket, DWORD nBytes, HANDLE hFile);
BOOL FTP_InitListenSocket(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_ConnectToHost(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_SendPassword(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_SendAccount(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_SendType(LPWININETFTPSESSIONA lpwfs, DWORD dwType);
BOOL FTP_SendPort(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_ParsePermission(LPCSTR lpszPermission, LPFILEPROPERTIESA lpfp);
BOOL FTP_ParseDirectory(LPWININETFTPSESSIONA lpwfs, int nSocket, LPFILEPROPERTIESA *lpafp, LPDWORD dwfp);
HINTERNET FTP_ReceiveFileList(LPWININETFTPSESSIONA lpwfs, int nSocket,
                 LPWIN32_FIND_DATAA lpFindFileData, DWORD dwContext);
/*LPSTR FTP_GetNextLine(int nSocket, LPSTR lpszBuffer, DWORD dwBuffer);*/
DWORD FTP_SetResponseError(DWORD dwResponse);

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
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;

        TRACE(" lpszLocalFile(%s) lpszNewRemoteFile(%s)\n", lpszLocalFile, lpszNewRemoteFile);
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPPUTFILEA;
          workRequest.HFTPSESSION = (DWORD)hConnect;
          workRequest.LPSZLOCALFILE = (DWORD)strdup(lpszLocalFile);
          workRequest.LPSZNEWREMOTEFILE = (DWORD)strdup(lpszNewRemoteFile);
          workRequest.DWFLAGS = dwFlags;
          workRequest.DWCONTEXT = dwContext;

          return INTERNET_AsyncCall(&workRequest);
        }
        else
        {
          return FTP_FtpPutFileA(hConnect, lpszLocalFile,
                        lpszNewRemoteFile, dwFlags, dwContext);
        }
}

/***********************************************************************
 *           FTP_FtpPutFileA (Internal)
 *
 * Uploads a file to the FTP server
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_FtpPutFileA(HINTERNET hConnect, LPCSTR lpszLocalFile,
                  LPCSTR lpszNewRemoteFile, DWORD dwFlags, DWORD dwContext)
{
        HANDLE hFile = (HANDLE)NULL;
        BOOL bSuccess = FALSE;
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
        INT nResCode;

        TRACE(" lpszLocalFile(%s) lpszNewRemoteFile(%s)\n", lpszLocalFile, lpszNewRemoteFile);
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        /* Open file to be uploaded */
        if (INVALID_HANDLE_VALUE == (hFile = CreateFileA(lpszLocalFile, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0)))
        {
            INTERNET_SetLastError(ERROR_FILE_NOT_FOUND);
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
                        close(nDataSocket);
                        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                        MAX_REPLY_LEN, 0, 0, 0);

                        if(nResCode)
                        {
                          if(nResCode == 226)
                            bSuccess = TRUE;
                          else
                            FTP_SetResponseError(nResCode);
                        }
                }
        }

lend:
        if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC  && hIC->lpfnStatusCB)
        {
                INTERNET_ASYNC_RESULT iar;

                iar.dwResult = (DWORD)bSuccess;
                iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
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
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
        LPWININETAPPINFOA hIC = NULL;

        TRACE("lpszDirectory(%s)\n", lpszDirectory);

        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPSETCURRENTDIRECTORYA;
          workRequest.HFTPSESSION = (DWORD)hConnect;
          workRequest.LPSZDIRECTORY = (DWORD)strdup(lpszDirectory);

          return INTERNET_AsyncCall(&workRequest);
        }
        else
        {
          return FTP_FtpSetCurrentDirectoryA(hConnect, lpszDirectory);
        }
}

/***********************************************************************
 *           FTP_FtpSetCurrentDirectoryA (Internal)
 *
 * Change the working directory on the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpSetCurrentDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory)
{
        int nResCode;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
        LPWININETAPPINFOA hIC = NULL;
        DWORD bSuccess = FALSE;

        TRACE("lpszDirectory(%s)\n", lpszDirectory);

        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_CWD, lpszDirectory,
                             hIC->lpfnStatusCB, hConnect, lpwfs->hdr.dwContext))
                goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, hIC->lpfnStatusCB, hConnect, lpwfs->hdr.dwContext);
        if (nResCode)
        {
                if (nResCode == 250)
                        bSuccess = TRUE;
                else
                    FTP_SetResponseError(nResCode);
        }

lend:
        if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
        {
                INTERNET_ASYNC_RESULT iar;

                iar.dwResult = (DWORD)bSuccess;
                iar.dwError = bSuccess ? ERROR_SUCCESS : ERROR_INTERNET_EXTENDED_ERROR;
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
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
        LPWININETAPPINFOA hIC = NULL;

        TRACE("\n");
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPCREATEDIRECTORYA;
          workRequest.HFTPSESSION = (DWORD)hConnect;
          workRequest.LPSZDIRECTORY = (DWORD)strdup(lpszDirectory);

          return INTERNET_AsyncCall(&workRequest);
        }
        else
        {
          return FTP_FtpCreateDirectoryA(hConnect, lpszDirectory);
        }
}

/***********************************************************************
 *           FTP_FtpCreateDirectoryA (Internal)
 *
 * Create new directory on the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpCreateDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory)
{
        int nResCode;
        BOOL bSuccess = FALSE;
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;

        TRACE("\n");
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_MKD, lpszDirectory, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
        if (nResCode)
        {
                if (nResCode == 257)
                        bSuccess = TRUE;
                else
                    FTP_SetResponseError(nResCode);
        }
lend:
        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
        {
                INTERNET_ASYNC_RESULT iar;

                iar.dwResult = (DWORD)bSuccess;
                iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
                hIC->lpfnStatusCB(hConnect, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
                        &iar, sizeof(INTERNET_ASYNC_RESULT));
        }

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
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
        LPWININETAPPINFOA hIC = NULL;

        TRACE("\n");

        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPFINDFIRSTFILEA;
          workRequest.HFTPSESSION = (DWORD)hConnect;
          workRequest.LPSZSEARCHFILE = (DWORD)strdup(lpszSearchFile);
          workRequest.LPFINDFILEDATA = (DWORD)lpFindFileData;
          workRequest.DWFLAGS = dwFlags;
          workRequest.DWCONTEXT = dwContext;

          INTERNET_AsyncCall(&workRequest);
          return NULL;
        }
        else
        {
          return FTP_FtpFindFirstFileA(hConnect, lpszSearchFile, lpFindFileData,
                dwFlags, dwContext);
        }
}

/***********************************************************************
 *           FTP_FtpFindFirstFileA (Internal)
 *
 * Search the specified directory
 *
 * RETURNS
 *    HINTERNET on success
 *    NULL on failure
 *
 */
HINTERNET FTP_FtpFindFirstFileA(HINTERNET hConnect,
    LPCSTR lpszSearchFile, LPWIN32_FIND_DATAA lpFindFileData, DWORD dwFlags, DWORD dwContext)
{
        int nResCode;
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hConnect;
        LPWININETFINDNEXTA hFindNext = NULL;

        TRACE("\n");

        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        if (!FTP_InitListenSocket(lpwfs))
            goto lend;

        if (!FTP_SendType(lpwfs, INTERNET_FLAG_TRANSFER_ASCII))
            goto lend;

        if (!FTP_SendPort(lpwfs))
            goto lend;

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_LIST, lpszSearchFile,
                             hIC->lpfnStatusCB, hConnect, lpwfs->hdr.dwContext))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                       MAX_REPLY_LEN, hIC->lpfnStatusCB, hConnect, lpwfs->hdr.dwContext);
        if (nResCode)
        {
            if (nResCode == 125 || nResCode == 150)
            {
                int nDataSocket;

                if (FTP_InitDataSocket(lpwfs, &nDataSocket))
                {
                        hFindNext = FTP_ReceiveFileList(lpwfs, nDataSocket, lpFindFileData, dwContext);

                        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, hIC->lpfnStatusCB, hConnect, lpwfs->hdr.dwContext);
                        if (nResCode != 226 && nResCode != 250)
                                INTERNET_SetLastError(ERROR_NO_MORE_FILES);

                        close(nDataSocket);
                }
            }
            else
              FTP_SetResponseError(nResCode);
        }

lend:
        if (lpwfs->lstnSocket != INVALID_SOCKET)
            close(lpwfs->lstnSocket);

        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
        {
                INTERNET_ASYNC_RESULT iar;

                if(hFindNext)
                {
                  iar.dwResult = (DWORD)hFindNext;
                  iar.dwError = ERROR_SUCCESS;
                  hIC->lpfnStatusCB(hConnect, lpwfs->hdr.dwContext, INTERNET_STATUS_HANDLE_CREATED,
                        &iar, sizeof(INTERNET_ASYNC_RESULT));
                }

                iar.dwResult = (DWORD)hFindNext;
                iar.dwError = hFindNext ? ERROR_SUCCESS : INTERNET_GetLastError();
                hIC->lpfnStatusCB(hConnect, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
                        &iar, sizeof(INTERNET_ASYNC_RESULT));
        }

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
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
        LPWININETAPPINFOA hIC = NULL;

        TRACE("\n");

        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA)lpwfs->hdr.lpwhparent;
        if( hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPGETCURRENTDIRECTORYA;
          workRequest.HFTPSESSION = (DWORD)hFtpSession;
          workRequest.LPSZDIRECTORY = (DWORD)lpszCurrentDirectory;
          workRequest.LPDWDIRECTORY = (DWORD)lpdwCurrentDirectory;

          return INTERNET_AsyncCall(&workRequest);
        }
        else
        {
          return FTP_FtpGetCurrentDirectoryA(hFtpSession, lpszCurrentDirectory,
                        lpdwCurrentDirectory);
        }
}

/***********************************************************************
 *           FTP_FtpGetCurrentDirectoryA (Internal)
 *
 * Retrieves the current directory
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpGetCurrentDirectoryA(HINTERNET hFtpSession, LPSTR lpszCurrentDirectory,
        LPDWORD lpdwCurrentDirectory)
{
        int nResCode;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
        LPWININETAPPINFOA hIC = NULL;
        DWORD bSuccess = FALSE;

        TRACE("\n");

        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        memset(lpszCurrentDirectory, 0, *lpdwCurrentDirectory);

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_PWD, NULL,
                    hIC->lpfnStatusCB, hFtpSession, lpwfs->hdr.dwContext))
                goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                      MAX_REPLY_LEN, hIC->lpfnStatusCB, hFtpSession, lpwfs->hdr.dwContext);
        if (nResCode)
        {
                if (nResCode == 257)              /* Extract directory name*/
                {
                        int firstpos, lastpos, len;
                        LPSTR lpszResponseBuffer = INTERNET_GetResponseBuffer();

                        for (firstpos = 0, lastpos = 0; lpszResponseBuffer[lastpos]; lastpos++)
                        {
                                if ('"' == lpszResponseBuffer[lastpos])
                                {
                                        if (!firstpos)
                                                firstpos = lastpos;
                                        else
                                                break;
                                }
                        }

                        len = lastpos - firstpos + 1;
                        strncpy(lpszCurrentDirectory, &lpszResponseBuffer[firstpos+1],
                                len < *lpdwCurrentDirectory ? len : *lpdwCurrentDirectory);
                        *lpdwCurrentDirectory = len;
                        bSuccess = TRUE;
                }
                else
                        FTP_SetResponseError(nResCode);
        }

lend:
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
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
        LPWININETAPPINFOA hIC = NULL;

        TRACE("\n");
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;

        if( hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPOPENFILEA;
          workRequest.HFTPSESSION = (DWORD)hFtpSession;
          workRequest.LPSZFILENAME = (DWORD)strdup(lpszFileName);
          workRequest.FDWACCESS = fdwAccess;
          workRequest.DWFLAGS = dwFlags;
          workRequest.DWCONTEXT = dwContext;

          INTERNET_AsyncCall(&workRequest);
          return NULL;
        }
        else
        {
          return FTP_FtpOpenFileA(hFtpSession, lpszFileName, fdwAccess, dwFlags, dwContext);
        }
}

/***********************************************************************
 *           FTP_FtpOpenFileA (Internal)
 *
 * Open a remote file for writing or reading
 *
 * RETURNS
 *    HINTERNET handle on success
 *    NULL on failure
 *
 */
HINTERNET FTP_FtpOpenFileA(HINTERNET hFtpSession,
        LPCSTR lpszFileName, DWORD fdwAccess, DWORD dwFlags,
        DWORD dwContext)
{
        int nDataSocket;
        LONG lBytesAvailable = 0;
        BOOL bSuccess = 0;
        LPWININETFILE hFile = NULL;
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;

        TRACE("\n");
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        if (GENERIC_READ == fdwAccess)
        {
                /* Set up socket to retrieve data */
                bSuccess = FTP_SendRetrieve(lpwfs, lpszFileName, dwFlags, &lBytesAvailable);
        }
        else if (GENERIC_WRITE == fdwAccess)
        {
                /* Set up socket to send data */
                bSuccess = FTP_SendStore(lpwfs, lpszFileName, dwFlags);
        }

        /* Accept connection from server */
        if (bSuccess && FTP_InitDataSocket(lpwfs, &nDataSocket))
        {
                hFile = HeapAlloc(GetProcessHeap(), 0, sizeof(WININETFILE));
          WriteLog("Created hFile (OPEN) at %x (%d)",hFile,lBytesAvailable);
                hFile->hdr.htype = WH_HFILE;
                hFile->hdr.dwFlags = dwFlags;
                hFile->hdr.dwContext = dwContext;
                hFile->hdr.lpwhparent = hFtpSession;
                hFile->nDataSocket = nDataSocket;
                hFile->lBytesAvailable = lBytesAvailable;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
        {
          INTERNET_ASYNC_RESULT iar;

          if(hFile)
          {
            iar.dwResult = (DWORD)hFile;
            iar.dwError = ERROR_SUCCESS;
            hIC->lpfnStatusCB(hFtpSession, lpwfs->hdr.dwContext, INTERNET_STATUS_HANDLE_CREATED,
                &iar, sizeof(INTERNET_ASYNC_RESULT));
          }

          iar.dwResult = (DWORD)bSuccess;
          iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
          hIC->lpfnStatusCB(hFtpSession, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
                &iar, sizeof(INTERNET_ASYNC_RESULT));
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
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hInternet;
        LPWININETAPPINFOA hIC = NULL;

        TRACE("lpszRemoteFile(%s) lpszNewFile(%s)\n", lpszRemoteFile, lpszNewFile);
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
        {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPGETFILEA;
          workRequest.HFTPSESSION = (DWORD)hInternet;
          workRequest.LPSZREMOTEFILE = (DWORD)strdup(lpszRemoteFile);
          workRequest.LPSZNEWFILE = (DWORD)strdup(lpszNewFile);
          workRequest.DWLOCALFLAGSATTRIBUTE = dwLocalFlagsAttribute;
          workRequest.FFAILIFEXISTS = (DWORD)fFailIfExists;
          workRequest.DWFLAGS = dwInternetFlags;
          workRequest.DWCONTEXT = dwContext;

          return INTERNET_AsyncCall(&workRequest);
        }
        else
        {
          return FTP_FtpGetFileA(hInternet, lpszRemoteFile, lpszNewFile,
                fFailIfExists, dwLocalFlagsAttribute, dwInternetFlags, dwContext);
        }
}

/***********************************************************************
 *           FTP_FtpGetFileA (Internal)
 *
 * Retrieve file from the FTP server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpGetFileA(HINTERNET hInternet, LPCSTR lpszRemoteFile, LPCSTR lpszNewFile,
        BOOL fFailIfExists, DWORD dwLocalFlagsAttribute, DWORD dwInternetFlags,
        DWORD dwContext)
{
        DWORD nBytes;
        BOOL bSuccess = FALSE;
        HANDLE hFile;
        LPWININETAPPINFOA hIC = NULL;
        LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hInternet;

        TRACE("lpszRemoteFile(%s) lpszNewFile(%s)\n", lpszRemoteFile, lpszNewFile);
        if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
        {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
        }

        /* Clear any error information */
        INTERNET_SetLastError(0);

        /* Ensure we can write to lpszNewfile by opening it */
        hFile = CreateFileA(lpszNewFile, GENERIC_WRITE, 0, 0, fFailIfExists ?
                CREATE_NEW : CREATE_ALWAYS, dwLocalFlagsAttribute, 0);
        if (INVALID_HANDLE_VALUE == hFile)
            goto lend;

        /* Set up socket to retrieve data */
        nBytes = FTP_SendRetrieve(lpwfs, lpszRemoteFile, dwInternetFlags, NULL);

        if (nBytes > 0)
        {
                int nDataSocket;

                /* Accept connection from ftp server */
                if (FTP_InitDataSocket(lpwfs, &nDataSocket))
                {
                        int nResCode;

                        /* Receive data */
                        FTP_RetrieveFileData(lpwfs, nDataSocket, nBytes, hFile);
                        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                            MAX_REPLY_LEN, 0, 0, 0);
                        if (nResCode)
                        {
                                if (nResCode == 226)
                                        bSuccess = TRUE;
                                else
                                        FTP_SetResponseError(nResCode);

                        }
                        close(nDataSocket);
                }
        }

lend:
        if (hFile)
                CloseHandle(hFile);

        hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
        if(hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
        {
          INTERNET_ASYNC_RESULT iar;

          iar.dwResult = (DWORD)bSuccess;
          iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
          hIC->lpfnStatusCB(hInternet, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
                &iar, sizeof(INTERNET_ASYNC_RESULT));
        }

        return bSuccess;
}

/***********************************************************************
 *           FTP_DeleteFileA (WININET.33)
 *
 * Delete a file on the ftp server
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOLAPI FtpDeleteFileA(HINTERNET hFtpSession, LPCSTR lpszFileName)
{
    LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
    LPWININETAPPINFOA hIC = NULL;

    if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
    {
        INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
        return FALSE;
    }

    hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
    if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
    {
          WORKREQUEST workRequest;

          workRequest.asyncall = FTPDELETEFILEA;
          workRequest.HFTPSESSION = (DWORD)hFtpSession;
          workRequest.LPSZFILENAME = (DWORD)strdup(lpszFileName);

          return INTERNET_AsyncCall(&workRequest);
    }
    else
    {
          return FTP_FtpDeleteFileA(hFtpSession, lpszFileName);
    }
}

/***********************************************************************
 *           FTP_FtpDeleteFileA  (Internal)
 *
 * Delete a file on the ftp server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpDeleteFileA(HINTERNET hFtpSession, LPCSTR lpszFileName)
{
      INT nResCode;
      BOOL bSuccess = FALSE;
      LPWININETAPPINFOA hIC = NULL;
      LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;

      TRACE("0x%08lx\n", (ULONG) hFtpSession);
      if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
      {
            INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
            return FALSE;
      }

                                    /* Clear any error information*/
      INTERNET_SetLastError(0);

      if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_DELE, lpszFileName, 0, 0, 0))
          goto lend;

      nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
            MAX_REPLY_LEN, 0, 0, 0);
      if (nResCode)
      {
            if (nResCode == 250)
                bSuccess = TRUE;
            else
                FTP_SetResponseError(nResCode);
      }
lend:
      hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
      if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
      {
            INTERNET_ASYNC_RESULT iar;

            iar.dwResult = (DWORD)bSuccess;
            iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
                hIC->lpfnStatusCB(hFtpSession, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
                    &iar, sizeof(INTERNET_ASYNC_RESULT));
      }

      return bSuccess;
}

/***********************************************************************
 *           FtpRemoveDirectoryA  (WININET.45)
 *
 * Remove a directory on the ftp server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOLAPI FtpRemoveDirectoryA(HINTERNET hFtpSession, LPCSTR lpszDirectory)
{
     LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
     LPWININETAPPINFOA hIC = NULL;

     if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
     {
           INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
           return FALSE;
     }

     hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
     if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
     {
              WORKREQUEST workRequest;

              workRequest.asyncall = FTPREMOVEDIRECTORYA;
              workRequest.HFTPSESSION = (DWORD)hFtpSession;
              workRequest.LPSZDIRECTORY = (DWORD)strdup(lpszDirectory);

              return INTERNET_AsyncCall(&workRequest);
      }
      else
      {
              return FTP_FtpRemoveDirectoryA(hFtpSession, lpszDirectory);
      }
}

/************************************************************************
 *           FTP_FtpRemoveDirectoryA  (Internal)
 *
 * Remove a directory on the ftp server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpRemoveDirectoryA(HINTERNET hFtpSession, LPCSTR lpszDirectory)
{
     INT nResCode;
     BOOL bSuccess = FALSE;
     LPWININETAPPINFOA hIC = NULL;
     LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;

     TRACE("\n");
     if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
     {
         INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
         return FALSE;
     }

                             /* Clear any error information*/
     INTERNET_SetLastError(0);

     if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_RMD, lpszDirectory, 0, 0, 0))
         goto lend;

     nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
         MAX_REPLY_LEN, 0, 0, 0);
     if (nResCode)
     {
         if (nResCode == 250)
             bSuccess = TRUE;
         else
             FTP_SetResponseError(nResCode);
     }

lend:
     hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
     if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
     {
         INTERNET_ASYNC_RESULT iar;

         iar.dwResult = (DWORD)bSuccess;
         iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
         hIC->lpfnStatusCB(hFtpSession, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
             &iar, sizeof(INTERNET_ASYNC_RESULT));
     }

     return bSuccess;
}

/***********************************************************************
 *           FtpRenameFileA  (WININET.47)
 *
 * Rename a file on the ftp server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL WINAPI FtpRenameFileA(HINTERNET hFtpSession, LPCSTR lpszSrc, LPCSTR lpszDest)
{
   LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;
   LPWININETAPPINFOA hIC = NULL;

   if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
   {
         INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
         return FALSE;
   }

   hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
   if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC)
   {
       WORKREQUEST workRequest;

       workRequest.asyncall = FTPRENAMEFILEA;
       workRequest.HFTPSESSION = (DWORD)hFtpSession;
       workRequest.LPSZSRCFILE = (DWORD)strdup(lpszSrc);
       workRequest.LPSZDESTFILE = (DWORD)strdup(lpszDest);

       return INTERNET_AsyncCall(&workRequest);
   }
   else
   {
       return FTP_FtpRenameFileA(hFtpSession, lpszSrc, lpszDest);
   }
}

/************************************************************************
 *           FTP_FtpRenameFileA  (Internal)
 *
 * Rename a file on the ftp server
 *
 * RETURNS
 *    TRUE on success
 *    FALSE on failure
 *
 */
BOOL FTP_FtpRenameFileA(HINTERNET hFtpSession, LPCSTR lpszSrc, LPCSTR lpszDest)
{
    INT nResCode;
    BOOL bSuccess = FALSE;
    LPWININETAPPINFOA hIC = NULL;
    LPWININETFTPSESSIONA lpwfs = (LPWININETFTPSESSIONA) hFtpSession;

    TRACE("\n");
    if (NULL == lpwfs || WH_HFTPSESSION != lpwfs->hdr.htype)
    {
        INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_HANDLE_TYPE);
        return FALSE;
    }

                            /* Clear any error information*/
    INTERNET_SetLastError(0);

    if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_RNFR, lpszSrc, 0, 0, 0))
        goto lend;

    nResCode = FTP_ReceiveResponse(lpwfs->sndSocket,
        INTERNET_GetResponseBuffer(), MAX_REPLY_LEN, 0, 0, 0);
    if (nResCode == 350)
    {
        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_RNTO, lpszDest, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket,
                    INTERNET_GetResponseBuffer(), MAX_REPLY_LEN, 0, 0, 0);
    }

    if (nResCode == 250)
        bSuccess = TRUE;
    else
        FTP_SetResponseError(nResCode);

lend:
    hIC = (LPWININETAPPINFOA) lpwfs->hdr.lpwhparent;
    if (hIC->hdr.dwFlags & INTERNET_FLAG_ASYNC && hIC->lpfnStatusCB)
    {
        INTERNET_ASYNC_RESULT iar;

        iar.dwResult = (DWORD)bSuccess;
        iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
        hIC->lpfnStatusCB(hFtpSession, lpwfs->hdr.dwContext, INTERNET_STATUS_REQUEST_COMPLETE,
            &iar, sizeof(INTERNET_ASYNC_RESULT));
    }

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
        int nsocket = INVALID_SOCKET, sock_namelen;
        LPWININETAPPINFOA hIC = NULL;
        BOOL result, bSuccess = FALSE;
        LPWININETFTPSESSIONA lpwfs = NULL;

        TRACE(" Server(%s) Port(%d) User(%s)\n", lpszServerName, nServerPort,
                lpszUserName);

        if (((LPWININETHANDLEHEADER)hInternet)->htype != WH_HINIT)
                goto lerror;

        hIC = (LPWININETAPPINFOA) hInternet;

        if (NULL == lpszUserName && NULL != lpszPassword)
        {
                INTERNET_SetLastError(ERROR_INTERNET_INCORRECT_USER_NAME);
                goto lerror;
        }

        if(nServerPort == INTERNET_INVALID_PORT_NUMBER)
           nServerPort = INTERNET_DEFAULT_FTP_PORT;

        if (hIC->lpfnStatusCB)
                hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_RESOLVING_NAME,
                        (LPSTR)lpszServerName, strlen(lpszServerName));

        if (!GetAddress(lpszServerName, nServerPort, &phe, &socketAddr))
        {
                INTERNET_SetLastError(ERROR_INTERNET_NAME_NOT_RESOLVED);
                goto lerror;
        }

        if (hIC->lpfnStatusCB)
                hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_NAME_RESOLVED,
                        (LPSTR)lpszServerName, strlen(lpszServerName));

        if (INVALID_SOCKET == (nsocket = socket(AF_INET, SOCK_STREAM, 0)))
        {
                INTERNET_SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
                goto lerror;
        }

        if(hIC->lpfnStatusCB)
                hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_CONNECTING_TO_SERVER,
                        &socketAddr, sizeof(struct sockaddr_in));

        if(connect(nsocket, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0)
        {
          INTERNET_SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
        }
        else
        {
                TRACE("Connected to server\n");

                if(hIC->lpfnStatusCB)
                   hIC->lpfnStatusCB(hInternet, dwContext, INTERNET_STATUS_CONNECTED_TO_SERVER,
                       &socketAddr, sizeof(struct sockaddr_in));

                lpwfs = HeapAlloc(GetProcessHeap(), 0, sizeof(WININETFTPSESSIONA));
                if (NULL == lpwfs)
                {
                    INTERNET_SetLastError(ERROR_OUTOFMEMORY);
                    goto lerror;
                }

                lpwfs->hdr.htype = WH_HFTPSESSION;
                lpwfs->hdr.dwFlags = dwFlags;
                lpwfs->hdr.dwContext = dwContext;
                lpwfs->hdr.lpwhparent = (LPWININETHANDLEHEADER)hInternet;
                lpwfs->sndSocket = nsocket;
                sock_namelen = sizeof(lpwfs->socketAddress);
                getsockname(nsocket,(struct sockaddr *)&lpwfs->socketAddress, &sock_namelen);
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

                if (FTP_ConnectToHost(lpwfs))
                {
                        if (hIC->lpfnStatusCB)
                        {
                                INTERNET_ASYNC_RESULT iar;

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
                iar.dwError = bSuccess ? ERROR_SUCCESS : INTERNET_GetLastError();
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
        FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(), MAX_REPLY_LEN, 0, 0, 0);

        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_USER, lpwfs->lpszUserName, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
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
                else
                  FTP_SetResponseError(nResCode);
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
BOOL FTP_SendCommand(int nSocket, FTP_COMMAND ftpCmd, LPCSTR lpszParam,
        INTERNET_STATUS_CALLBACK lpfnStatusCB, HINTERNET hHandle, DWORD dwContext)
{
        int len;
        char *buf;
        int nBytesSent = 0;
        int nRC        = 0;
        BOOL bParamHasLen;

        TRACE("%d: (%s) %d\n", ftpCmd, lpszParam, nSocket);

        if(lpfnStatusCB)
          lpfnStatusCB(hHandle, dwContext, INTERNET_STATUS_SENDING_REQUEST, NULL, 0);

        bParamHasLen = lpszParam && strlen(lpszParam) > 0;
        len = (bParamHasLen ? strlen(lpszParam) : -1) + strlen(szFtpCommands[ftpCmd]) +
            strlen(szCRLF)+ 1;
        if (NULL == (buf = HeapAlloc(GetProcessHeap(), 0, len+1)))
        {
            INTERNET_SetLastError(ERROR_OUTOFMEMORY);
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

        if(lpfnStatusCB)
          lpfnStatusCB(hHandle, dwContext, INTERNET_STATUS_REQUEST_SENT,
                &nBytesSent, sizeof(DWORD));

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

int FTP_ReceiveResponse(int nSocket, LPSTR lpszResponse, DWORD dwResponse,
        INTERNET_STATUS_CALLBACK lpfnStatusCB, HINTERNET hHandle, DWORD dwContext)
{
        DWORD nRecv = 0;
        int rc = 0;
        char firstprefix[5];
        BOOL multiline = FALSE;

        TRACE("\n");

        if(lpfnStatusCB)
          lpfnStatusCB(hHandle, dwContext, INTERNET_STATUS_RECEIVING_RESPONSE, NULL, 0);

        while(1)
        {
                nRecv = dwResponse;

                if(!INTERNET_GetNextLine(nSocket, lpszResponse, &nRecv))
                  goto lerror;

                if (nRecv >= 3)
                {
                    if(!multiline)
                    {
                        if(lpszResponse[3] != '-')
                                break;
                        else
                        {
                          /* Start multiline response. Loop until we get 'nnn' */
                          multiline = TRUE;
                          memcpy(firstprefix, lpszResponse, 3);
                          firstprefix[3] = ' ';
                          firstprefix[4] = '\0';
                        }
                    }
                    else
                    {
                      if(!memcmp(firstprefix, lpszResponse, 4))
                        break;
                    }
                }
        }

        if (nRecv >= 3)
        {
                lpszResponse[nRecv] = '\0';
                rc = atoi(lpszResponse);

                if(lpfnStatusCB)
                        lpfnStatusCB(hHandle, dwContext, INTERNET_STATUS_RESPONSE_RECEIVED,
                                &nRecv, sizeof(DWORD));
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
        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_PASS, lpwfs->lpszPassword, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
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
                        FTP_SetResponseError(nResCode);
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
        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_ACCT, NOACCOUNT, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
        if (nResCode)
        {
            bSuccess = TRUE;
        }
        else
            FTP_SetResponseError(nResCode);

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

        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_STOR, lpszRemoteFile, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
        if (nResCode)
        {
                if (nResCode == 150)
                    bSuccess = TRUE;
                else
                    FTP_SetResponseError(nResCode);
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

        /* We obtain our IP addr from the name of the command cgannel socket */
        lpwfs->lstnSocketAddress = lpwfs->socketAddress;

        /* and get the system to assign us a port */
        lpwfs->lstnSocketAddress.sin_port = htons((u_short) 0);

        if (SOCKET_ERROR == bind(lpwfs->lstnSocket, (struct sockaddr *)&lpwfs->lstnSocketAddress, sizeof(struct sockaddr_in)))
        {
                TRACE("Unable to bind socket: %d\n", errno);
                goto lend;
        }

        if (SOCKET_ERROR == listen(lpwfs->lstnSocket, MAX_BACKLOG))
        {
                TRACE("listen failed\n");
                goto lend;
        }

        if (SOCKET_ERROR != getsockname(lpwfs->lstnSocket, (struct sockaddr *)&lpwfs->lstnSocketAddress, &namelen))
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

        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_TYPE, type, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0)/100;
        if (nResCode)
        {
                if (nResCode == 2)
                        bSuccess = TRUE;
                else
                        FTP_SetResponseError(nResCode);
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

        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_PORT, szIPAddress, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
        if (nResCode)
        {
                if (nResCode == 200)
                        bSuccess = TRUE;
                else
                        FTP_SetResponseError(nResCode);
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
        *nDataSocket = accept(lpwfs->lstnSocket, (struct sockaddr *)&saddr, &addrlen);
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
        LONG  nBytesToSend;
        DWORD nLen, nRC = 1;
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
DWORD FTP_SendRetrieve(LPWININETFTPSESSIONA lpwfs, LPCSTR lpszRemoteFile, DWORD dwType, LONG *lBytesAvailable)
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

        if (!FTP_SendCommand(lpwfs->sndSocket, FTP_CMD_RETR, lpszRemoteFile, 0, 0, 0))
            goto lend;

        nResCode = FTP_ReceiveResponse(lpwfs->sndSocket, INTERNET_GetResponseBuffer(),
                                       MAX_REPLY_LEN, 0, 0, 0);
        if (nResCode)
        {
                if (nResCode == 125 || nResCode == 150)
                {
                        /* Parse size of data to be retrieved */
                        int i, sizepos = -1;
                        LPSTR lpszResponseBuffer = INTERNET_GetResponseBuffer();
                        for (i = strlen(lpszResponseBuffer) - 1; i >= 0; i--)
                        {
                                if ('(' == lpszResponseBuffer[i])
                                {
                                        sizepos = i;
                                        break;
                                }
                        }

                        if (sizepos >= 0)
                        {
                                nResult = atol(&lpszResponseBuffer[sizepos+1]);
                                TRACE("Waiting to receive %d bytes\n", nResult);

                                if(lBytesAvailable)
                                  *lBytesAvailable = nResult;
                        }
                        else
                                if(lBytesAvailable)
                                  *lBytesAvailable = -1;
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
                INTERNET_SetLastError(ERROR_OUTOFMEMORY);
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

//                      INTERNET_SubstractFromDataAvailable(lpwfs,nRC);
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

    if (INTERNET_GetResponseBuffer())
        HeapFree(GetProcessHeap(), 0, INTERNET_GetResponseBuffer());

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
HINTERNET FTP_ReceiveFileList(LPWININETFTPSESSIONA lpwfs, int nSocket,
                LPWIN32_FIND_DATAA lpFindFileData, DWORD dwContext)
{
        DWORD dwSize;
        LPFILEPROPERTIESA lpafp = NULL;
        LPWININETFINDNEXTA lpwfn = NULL;

        TRACE("\n");

        if (FTP_ParseDirectory(lpwfs, nSocket, &lpafp, &dwSize))
        {
                FTP_ConvertFileProp(lpafp, lpFindFileData);

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
 *           FTP_ConvertFileProp (internal)
 *
 Converts FILEPROPERTIESA struct to WIN32_FILE_DATAA
 *
 * RETURNS
 *   TRUE on success
 *   FALSE on failure
 *
 */
BOOL FTP_ConvertFileProp(LPFILEPROPERTIESA lpafp, LPWIN32_FIND_DATAA lpFindFileData)
{
        BOOL bSuccess = FALSE;

        ZeroMemory(lpFindFileData, sizeof(WIN32_FIND_DATAA));

        if(lpafp)
        {
                DWORD access = mktime(&lpafp->tmLastModified);

                /* Not all fields are filled in */
                lpFindFileData->ftLastAccessTime.dwHighDateTime = HIWORD(access);
                lpFindFileData->ftLastAccessTime.dwLowDateTime  = LOWORD(access);
                lpFindFileData->nFileSizeHigh = HIWORD(lpafp->nSize);
                lpFindFileData->nFileSizeLow = LOWORD(lpafp->nSize);

                if(lpafp->bIsDirectory)
                  lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;

                if(lpafp->lpszName)
                  strncpy(lpFindFileData->cFileName, lpafp->lpszName, MAX_PATH);

                bSuccess = TRUE;
        }

    return bSuccess;
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
#define MIN_LEN_DIR_ENTRY 15

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
        DWORD nBufLen = MAX_REPLY_LEN;
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

        *lpafp = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FILEPROPERTIESA)*(sizeFilePropArray));
        if(NULL == lpafp)
          {
            bSuccess = FALSE;
            goto lend;
          }

        while ((pszLine = INTERNET_GetNextLine(nSocket, INTERNET_GetResponseBuffer(), &nBufLen)) != NULL)
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

                if(!pszToken || 10 != strlen(pszToken) || nBufLen < MIN_LEN_DIR_ENTRY)
                {
                  nBufLen = MAX_REPLY_LEN;
                  continue;
                }

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

                nBufLen = MAX_REPLY_LEN;
                indexFilePropArray++;
        }

        if (bSuccess && indexFilePropArray)
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
                INTERNET_SetLastError(ERROR_NO_MORE_FILES);
                bSuccess = FALSE;
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
 *           FTP_SetResponseError (internal)
 *
 * Set the appropriate error code for a given response from the server
 *
 * RETURNS
 *
 */
DWORD FTP_SetResponseError(DWORD dwResponse)
{
    DWORD dwCode = 0;

    switch(dwResponse)
    {
        case 421:   /* Service not available - Server may be shutting down.*/
            dwCode = ERROR_INTERNET_TIMEOUT;
            break;

        case 425:                          /* Cannot open data connection.*/
            dwCode = ERROR_INTERNET_CANNOT_CONNECT;
            break;

        case 426:                   /* Connection closed, transer aborted.*/
            dwCode = ERROR_INTERNET_CONNECTION_ABORTED;
            break;

        case 500:                   /* Syntax error. Command unrecognized.*/
        case 501:       /* Syntax error. Error in parameters or arguments.*/
            dwCode = ERROR_INTERNET_INCORRECT_FORMAT;
            break;

        case 530:                       /* Not logged in. Login incorrect.*/
            dwCode = ERROR_INTERNET_LOGIN_FAILURE;
            break;

        case 550:   /* File action not taken. File not found or no access.*/
            dwCode = ERROR_INTERNET_ITEM_NOT_FOUND;
            break;

        case 450:              /* File action not taken. File may be busy.*/
        case 451:                         /* Action aborted. Server error.*/
        case 452:/* Action not taken. Insufficient storage space on server.*/
        case 502:                              /* Command not implemented.*/
        case 503:                              /* Bad sequence of command.*/
        case 504:           /* Command not implemented for that parameter.*/
        case 532:                        /* Need account for storing files*/
        case 551:   /* Requested action aborted. Page type unknown*/
        case 552:   /* Action aborted. Exceeded storage allocation*/
        case 553:      /* Action not taken. File name not allowed.*/

        default:
            dwCode = ERROR_INTERNET_INTERNAL_ERROR;
            break;
    }

    INTERNET_SetLastError(dwCode);
    return dwCode;
}
