/* $Id: wininet.cpp,v 1.3 2000-12-27 23:06:17 sandervl Exp $ */
/*
 * WININET stubs
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <win\wininet.h>

//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetTimeFromSystemTime(CONST SYSTEMTIME *pst, DWORD dwRFC,
                                       LPSTR lpszTime, DWORD cbTime)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetCrackUrlW(LPCWSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags,
                              LPURL_COMPONENTSW lpUrlComponents)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetCreateUrlA(LPURL_COMPONENTSA lpUrlComponents, DWORD dwFlags,
                               LPSTR lpszUrl, LPDWORD lpdwUrlLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetCreateUrlW(LPURL_COMPONENTSW lpUrlComponents, DWORD dwFlags,
                               LPWSTR lpszUrl, LPDWORD lpdwUrlLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetCanonicalizeUrlW(LPCWSTR lpszUrl, LPWSTR lpszBuffer,
                                     LPDWORD lpdwBufferLength, DWORD dwFlags)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetCombineUrlA(LPCSTR lpszBaseUrl, LPCSTR lpszRelativeUrl,
                                LPSTR lpszBuffer, LPDWORD lpdwBufferLength,
                                DWORD dwFlags)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetCombineUrlW(LPCWSTR lpszBaseUrl, LPCWSTR lpszRelativeUrl,
                                LPWSTR lpszBuffer, LPDWORD lpdwBufferLength,
                                DWORD dwFlags)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType,
                               LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass,
                               DWORD dwFlags)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI InternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName,
                                  INTERNET_PORT nServerPort, LPCWSTR lpszUserName,
                                  LPCWSTR lpszPassword, DWORD dwService,
                                  DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI InternetOpenUrlA(HINTERNET hInternet, LPCSTR lpszUrl, LPCSTR lpszHeaders,
                                  DWORD dwHeadersLength, DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI InternetOpenUrlW(HINTERNET hInternet, LPCWSTR lpszUrl, LPCWSTR lpszHeaders,
                                  DWORD dwHeadersLength, DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI InternetSetFilePointer(HINTERNET hFile, LONG lDistanceToMove,
                                    PVOID pReserved, DWORD dwMoveMethod,
                                    DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetFindNextFileW(HINTERNET hFind, LPVOID lpvFindData)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetQueryOptionW(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 LPDWORD lpdwBufferLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetSetOptionA(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                               DWORD dwBufferLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetSetOptionW(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                               DWORD dwBufferLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetSetOptionExA(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 DWORD dwBufferLength, DWORD dwFlags)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetSetOptionExW(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 DWORD dwBufferLength, DWORD dwFlags)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetGetLastResponseInfoW(LPDWORD lpdwError, LPWSTR lpszBuffer,
                                         LPDWORD lpdwBufferLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI FtpFindFirstFileW(HINTERNET hConnect, LPCWSTR lpszSearchFile,
                                   LPWIN32_FIND_DATAW lpFindFileData,
                                   DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpGetFileW(HINTERNET hConnect, LPCWSTR lpszRemoteFile,
                        LPCWSTR lpszNewFile, BOOL fFailIfExists,
                        DWORD dwFlagsAndAttributes, DWORD dwFlags,
                        DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpPutFileW(HINTERNET hConnect, LPCWSTR lpszLocalFile, LPCWSTR lpszNewRemoteFile,
                        DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpDeleteFileW(HINTERNET hConnect, LPCWSTR lpszFileName)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpRenameFileW(HINTERNET hConnect, LPCWSTR lpszExisting,LPCWSTR lpszNew)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI FtpOpenFileW(HINTERNET hConnect, LPCWSTR lpszFileName,
                              DWORD dwAccess, DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpCreateDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpRemoveDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpSetCurrentDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpGetCurrentDirectoryW(HINTERNET hConnect, LPWSTR lpszCurrentDirectory,
                                    LPDWORD lpdwCurrentDirectory)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpCommandA(HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags,
                        LPCSTR lpszCommand, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FtpCommandW(HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags,
                        LPCWSTR lpszCommand, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GopherCreateLocatorA(LPCSTR lpszHost,INTERNET_PORT nServerPort,
                                 LPCSTR lpszDisplayString, LPCSTR lpszSelectorString,
                                 DWORD dwGopherType, LPSTR lpszLocator,
                                 LPDWORD lpdwBufferLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GopherCreateLocatorW(LPCWSTR lpszHost,INTERNET_PORT nServerPort,
                                 LPCWSTR lpszDisplayString, LPCWSTR lpszSelectorString,
                                 DWORD dwGopherType, LPWSTR lpszLocator,
                                 LPDWORD lpdwBufferLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GopherGetLocatorTypeA(LPCSTR lpszLocator, LPDWORD lpdwGopherType)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GopherGetLocatorTypeW(LPCWSTR lpszLocator, LPDWORD lpdwGopherType)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI GopherFindFirstFileA(HINTERNET hConnect, LPCSTR lpszLocator,
                                      LPCSTR lpszSearchString, LPGOPHER_FIND_DATAA lpFindData,
                                      DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI GopherFindFirstFileW(HINTERNET hConnect, LPCWSTR lpszLocator,
                                      LPCWSTR lpszSearchString, LPGOPHER_FIND_DATAA lpFindData,
                                      DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI GopherOpenFileA(HINTERNET hConnect, LPCSTR lpszLocator,
                                 LPCSTR lpszView, DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI GopherOpenFileW(HINTERNET hConnect, LPCWSTR lpszLocator,
                                 LPCWSTR lpszView, DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GopherGetAttributeA(HINTERNET hConnect, LPCSTR lpszLocator,
                                LPCSTR lpszAttributeName, LPBYTE lpBuffer,
                                DWORD dwBufferLength, LPDWORD lpdwCharactersReturned,
                                GOPHER_ATTRIBUTE_ENUMERATOR lpfnEnumerator,
                                DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GopherGetAttributeW(HINTERNET hConnect, LPCWSTR lpszLocator,
                                LPCWSTR lpszAttributeName, LPBYTE lpBuffer,
                                DWORD dwBufferLength, LPDWORD lpdwCharactersReturned,
                                GOPHER_ATTRIBUTE_ENUMERATOR lpfnEnumerator,
                                DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HINTERNET WINAPI HttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb,
                                  LPCWSTR lpszObjectName, LPCWSTR lpszVersion,
                                  LPCWSTR lpszReferrer, LPCWSTR * lplpszAcceptTypes,
                                  DWORD dwFlags, DWORD dwContext)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI HttpAddRequestHeadersW(HINTERNET hRequest, LPCWSTR lpszHeaders,
                                   DWORD dwHeadersLength, DWORD dwModifiers)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI HttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders,
                             DWORD dwHeadersLength, LPVOID lpOptional,
                             DWORD dwOptionalLength)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI HttpQueryInfoW(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer,
                           LPDWORD lpdwBufferLength, LPDWORD lpdwIndex)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetSetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetSetCookieW(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetGetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPSTR lpCookieData,
                               LPDWORD lpdwSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI InternetGetCookieW(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPWSTR lpCookieData,
                               LPDWORD lpdwSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//* ******************************************************************************/
//* ******************************************************************************/
DWORD WINAPI InternetErrorDlg(HWND hWnd, HINTERNET hRequest,
                              DWORD dwError, DWORD dwFlags, LPVOID * lppvData)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//* ******************************************************************************/
//* ******************************************************************************/
DWORD WINAPI InternetCheckConnectionW(LPCWSTR lpszUrl, DWORD dwFlags, DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//* ******************************************************************************/
//******************************************************************************
DWORD WINAPI InternetConfirmZoneCrossing(HWND hWnd, LPSTR szUrlPrev,
                                         LPSTR szUrlNew, BOOL bPost)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CreateUrlCacheEntryA(LPCSTR lpszUrlName, DWORD dwExpectedFileSize,
                                 LPCSTR lpszFileExtension, LPSTR lpszFileName,
                                 DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CreateUrlCacheEntryW(LPCWSTR lpszUrlName, DWORD dwExpectedFileSize,
                                 LPCWSTR lpszFileExtension, LPWSTR lpszFileName,
                                 DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CommitUrlCacheEntryA(LPCSTR lpszUrlName, LPCSTR lpszLocalFileName,
                                 FILETIME ExpireTime, FILETIME LastModifiedTime,
                                 DWORD CacheEntryType, LPBYTE lpHeaderInfo,
                                 DWORD dwHeaderSize, LPCTSTR lpszFileExtension,
                                 DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI CommitUrlCacheEntryW(LPCWSTR lpszUrlName, LPCWSTR lpszLocalFileName,
                                 FILETIME ExpireTime, FILETIME LastModifiedTime,
                                 DWORD CacheEntryType, LPBYTE lpHeaderInfo,
                                 DWORD dwHeaderSize, LPCTSTR lpszFileExtension,
                                 DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI RetrieveUrlCacheEntryFileA(LPCSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                       LPDWORD lpdwCacheEntryInfoBufferSize,
                                       DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI RetrieveUrlCacheEntryFileW(LPCWSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                       LPDWORD lpdwCacheEntryInfoBufferSize,
                                       DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI UnlockUrlCacheEntryFile(LPCSTR lpszUrlName, DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WINAPI RetrieveUrlCacheEntryStreamA(LPCSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                           LPDWORD lpdwCacheEntryInfoBufferSize,
                                           BOOL fRandomRead, DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WINAPI RetrieveUrlCacheEntryStreamW(LPCWSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                           LPDWORD lpdwCacheEntryInfoBufferSize,
                                           BOOL fRandomRead, DWORD dwReserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI ReadUrlCacheEntryStream(HANDLE hUrlCacheStream, DWORD dwLocation,
                                    LPVOID lpBuffer, LPDWORD lpdwLen,
                                    DWORD Reserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI UnlockUrlCacheEntryStream(HANDLE hUrlCacheStream, DWORD Reserved)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GetUrlCacheEntryInfoA(LPCSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                  LPDWORD lpdwCacheEntryInfoBufferSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI GetUrlCacheEntryInfoW(LPCWSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                  LPDWORD lpdwCacheEntryInfoBufferSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI SetUrlCacheEntryInfoA(LPCSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                  DWORD dwFieldControl)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI SetUrlCacheEntryInfoW(LPCWSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                  DWORD dwFieldControl)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WINAPI FindFirstUrlCacheEntryA(LPCSTR lpszUrlSearchPattern,
                                      LPINTERNET_CACHE_ENTRY_INFOA lpFirstCacheEntryInfo,
                                      LPDWORD lpdwFirstCacheEntryInfoBufferSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
HANDLE WINAPI FindFirstUrlCacheEntryW(LPCWSTR lpszUrlSearchPattern,
                                      LPINTERNET_CACHE_ENTRY_INFOW lpFirstCacheEntryInfo,
                                      LPDWORD lpdwFirstCacheEntryInfoBufferSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FindNextUrlCacheEntryA(HANDLE hEnumHandle,
                                   LPINTERNET_CACHE_ENTRY_INFOA lpNextCacheEntryInfo,
                                   LPDWORD lpdwNextCacheEntryInfoBufferSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FindNextUrlCacheEntryW(HANDLE hEnumHandle,
                                   LPINTERNET_CACHE_ENTRY_INFOW lpNextCacheEntryInfo,
                                   LPDWORD lpdwNextCacheEntryInfoBufferSize)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI FindCloseUrlCache(HANDLE hEnumHandle)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI DeleteUrlCacheEntry(LPCSTR lpszUrlName)
{dprintf(("ERROR:"__FUNCTION__" not implemented"));
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return 0;
}
//******************************************************************************
//******************************************************************************
