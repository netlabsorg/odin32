/*
	oldnls32
	
	obsolete APIs for FarEast-compliant NLS support
*/


#ifndef _OLDNLS32_
#define _OLDNLS32_


#ifdef __cplusplus
extern "C" {
#endif


/*
	IMP APIs and structures (winnls32.h)
*/

typedef struct _tagDATETIME {
	WORD	year;
	WORD	month;
	WORD	day;
	WORD	hour;
	WORD	min;
	WORD	sec;
} DATETIME;

typedef struct _tagIMEPROA {
	HWND		hWnd;
	DATETIME	InstDate;
	UINT		wVersion;
	BYTE		szDescription[50];
	BYTE		szName[80];
	BYTE		szOptions[30];
#if defined(TAIWAN)
	BYTE		szUsrFontName[80];
	BOOL		fEnable;
#endif
} IMEPROA;

typedef struct _tagIMEPROW {
	HWND		hWnd;
	DATETIME	InstDate;
	UINT		wVersion;
	WCHAR		szDescription[50];
	WCHAR		szName[80];
	WCHAR		szOptions[30];
#if defined(TAIWAN)
	WCHAR		szUsrFontName[80];
	BOOL		fEnable;
#endif
} IMEPROW;

typedef IMEPROA		*PIMEPROA, *LPIMEPROA;
typedef IMEPROW		*PIMEPROW, *LPIMEPROW;


BOOL WINAPI IMPGetIMEA(HWND, LPIMEPROA);
BOOL WINAPI IMPGetIMEW(HWND, LPIMEPROW);
BOOL WINAPI IMPQueryIMEA(LPIMEPROA);
BOOL WINAPI IMPQueryIMEW(LPIMEPROW);
BOOL WINAPI IMPSetIMEA(HWND, LPIMEPROA);
BOOL WINAPI IMPSetIMEW(HWND, LPIMEPROW);


#ifdef UNICODE
typedef IMEPROW		IMEPRO;
typedef PIMEPROW	PIMEPRO;
typedef LPIMEPROW	LPIMEPRO;
#define IMPGetIME	IMPGetIMEW
#define IMPQueryIME	IMPQueryIMEW
#define IMPSetIME	IMPSetIMEW
#else
typedef IMEPROA		IMEPRO;
typedef PIMEPROA	PIMEPRO;
typedef LPIMEPROA	LPIMEPRO;
#define IMPGetIME	IMPGetIMEA
#define IMPQueryIME	IMPQueryIMEA
#define IMPSetIME	IMPSetIMEA
#endif

/*
	IMM APIs (winnls32.h)
*/

UINT WINAPI WINNLSGetIMEHotkey(HWND);
BOOL WINAPI WINNLSEnableIME(HWND, BOOL);
BOOL WINAPI WINNLSGetEnableStatus(HWND);


/*
	IME APIs (ime.h)
*/

#if !defined(SendIMEMessageEx)
LRESULT WINAPI SendIMEMessageExA(HWND, LPARAM);
LRESULT WINAPI SendIMEMessageExW(HWND, LPARAM);
#ifdef UNICODE
#define SendIMEMessageEx	SendIMEMessageExW
#else
#define SendIMEMessageEx	SendIMEMessageExA
#endif
#endif	/* !defined(SendIMEMessageEx) */


#ifdef __cplusplus
}
#endif

#endif	/* _OLDNLS32_ */
