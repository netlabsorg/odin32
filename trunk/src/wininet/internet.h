#ifndef _WINE_INTERNET_H_
#define _WINE_INTERNET_H_

#ifdef __WIN32OS2__
#define close closesocket
typedef int socklen_t;

#include <time.h>

#endif

typedef enum
{
    WH_HINIT,
    WH_HFTPSESSION,
    WH_HGOPHERSESSION,
    WH_HHTTPSESSION,
    WH_HHTTPREQ,
    WH_HFILE,
    WH_HFINDNEXT,
} WH_TYPE;

typedef struct _WININETHANDLEHEADER
{
    WH_TYPE htype;
    DWORD  dwFlags;
    DWORD  dwContext;
    struct _WININETHANDLEHEADER *lpwhparent;
} WININETHANDLEHEADER, *LPWININETHANDLEHEADER;


typedef struct
{
	WININETHANDLEHEADER hdr;
	LPSTR  lpszAgent;
	LPSTR  lpszProxy;
	LPSTR  lpszProxyBypass;
	DWORD   dwAccessType;
	INTERNET_STATUS_CALLBACK lpfnStatusCB;
} WININETAPPINFOA, *LPWININETAPPINFOA;


typedef struct
{
	WININETHANDLEHEADER hdr;
	LPSTR  lpszServerName;
	LPSTR  lpszUserName;
	INTERNET_PORT nServerPort;
	struct sockaddr_in socketAddress;
	struct hostent *phostent;
} WININETHTTPSESSIONA, *LPWININETHTTPSESSIONA;


typedef struct
{
	WININETHANDLEHEADER hdr;
	LPSTR lpszPath;
	LPSTR lpszReferrer;
	LPSTR lpszAcceptTypes;
	LPSTR lpszVerb;
	LPSTR lpszHostName;
	LPSTR lpszRedirect;
	int	nSocketFD;
	int	statusCode;
	int	contentLength;
	time_t  nSystemTime;
} WININETHTTPREQA, *LPWININETHTTPREQA;


typedef struct
{
	WININETHANDLEHEADER hdr;
	int sndSocket;
	int lstnSocket;
	struct sockaddr_in socketAddress;
	struct sockaddr_in lstnSocketAddress;
	struct hostent *phostent;
	LPSTR  lpszPassword;
	LPSTR  lpszUserName;
	LPSTR  lpszResponseBuffer;
} WININETFTPSESSIONA, *LPWININETFTPSESSIONA;


typedef struct
{
	WININETHANDLEHEADER hdr;
	int nDataSocket;
} WININETFILE, *LPWININETFILE;


typedef struct
{
	BOOL	  bIsDirectory;
	LPSTR     lpszName;
	DWORD	  nSize;
	struct tm tmLastModified;
	unsigned short permissions;
} FILEPROPERTIESA, *LPFILEPROPERTIESA;


typedef struct
{
	WININETHANDLEHEADER hdr;
	int index;
	DWORD size;
	LPFILEPROPERTIESA lpafp;
} WININETFINDNEXTA, *LPWININETFINDNEXTA;

time_t ConvertTimeString(LPCSTR asctime);

HINTERNET FTP_Connect(HINTERNET hInterent, LPCSTR lpszServerName, 
	INTERNET_PORT nServerPort, LPCSTR lpszUserName,
	LPCSTR lpszPassword, DWORD dwFlags, DWORD dwContext);

BOOL GetAddress(LPCSTR lpszServerName, INTERNET_PORT nServerPort,
	struct hostent **phe, struct sockaddr_in *psa);

int WriteDataToStream(int nDataSocket, LPCVOID Buffer, DWORD BytesToWrite);
int ReadDataFromStream(int nDataSocket, LPVOID Buffer, DWORD BytesToRead);
BOOL FTP_CloseSessionHandle(LPWININETFTPSESSIONA lpwfs);
BOOL FTP_CloseFindNextHandle(LPWININETFINDNEXTA lpwfn);

#endif /* _WINE_INTERNET_H_ */
