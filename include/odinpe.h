#ifndef _ODINRPE_H
#define _ODINRPE_H

//loader errors
#define LDRERROR_SUCCESS		0
#define LDRERROR_INVALID_HEADER		1
#define LDRERROR_NO_ENTRYPOINT		2
#define LDRERROR_INVALID_MODULE 	3
#define LDRERROR_INTERNAL               4
#define LDRERROR_MEMORY                 5
#define LDRERROR_INVALID_CPU            6
#define LDRERROR_FILE_SYSTEM            7
#define LDRERROR_INVALID_SECTION        8
#define LDRERROR_EXPORTS                9
#define LDRERROR_IMPORTS                10


//CreateWin32PeLdrExe entrypoint (KERNEL32.1236)

//DWORD WIN32API CreateWin32PeLdrExe(char *szFileName, char *szCmdLine,
//                                   char *peoptions, 
//                                   ULONG reservedMem, ULONG ulPEOffset,
//                                   BOOL fConsoleApp, BOOL fVioConsole,
//                                   char *pszErrorModule, ULONG cbErrorModule)
typedef DWORD (* WIN32API WIN32CTOR)(char *, char *, char *, ULONG, ULONG, BOOL, BOOL, char *, ULONG);


#endif //_ODINRPE_H
