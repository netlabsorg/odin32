#ifndef __INITDLL_H__
#define __INITDLL_H__

#if (defined(__IBMCPP__) || defined(__IBMC__))

#define DLLENTRYPOINT_CCONV SYSTEM
#define DLLENTRYPOINT_NAME  _DLL_InitTerm

#ifdef __cplusplus
extern "C" {
#endif

#if (__IBMCPP__ == 300) || (__IBMC__ == 300)
void _Optlink __ctordtorInit( void );
#define ctordtorInit()  __ctordtorInit()

void _Optlink __ctordtorTerm( void );
#define ctordtorTerm()  __ctordtorTerm()

#elif (__IBMCPP__ == 360) || (__IBMC__ == 360)
void _Optlink __ctordtorInit( int flag );
#define ctordtorInit()  __ctordtorInit(0)

void _Optlink __ctordtorTerm( int flag );
#define ctordtorTerm()  __ctordtorTerm(0)

#else
#error "Unknown compiler!"
#endif


/*-------------------------------------------------------------------*/
/* _CRT_init is the C run-time environment initialization function.  */
/* It will return 0 to indicate success and -1 to indicate failure.  */
/*-------------------------------------------------------------------*/
int  _Optlink _CRT_init(void);

/*-------------------------------------------------------------------*/
/* _CRT_term is the C run-time environment termination function.     */
/* It only needs to be called when the C run-time functions are      */
/* statically linked.                                                */
/*-------------------------------------------------------------------*/
void _Optlink _CRT_term(void);


#ifdef __cplusplus
}
#endif

#elif defined(__WATCOMC__)

#define DLLENTRYPOINT_CCONV APIENTRY
#define DLLENTRYPOINT_NAME  LibMain

#define ctordtorInit()
#define ctordtorTerm()

#ifdef __cplusplus
extern "C" {
//prevent Watcom from mucking with this name
extern DWORD _Resource_PEResTab;
#pragma aux _Resource_PEResTab "*";
}
#endif

#endif

ULONG APIENTRY inittermKernel32(ULONG hModule, ULONG ulFlag);
void  APIENTRY cleanupKernel32(ULONG ulReason);

ULONG APIENTRY inittermUser32(ULONG hModule, ULONG ulFlag);
void  APIENTRY cleanupUser32(ULONG ulReason);

ULONG APIENTRY inittermWinmm(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermShell32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermOle32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermComdlg32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermComctl32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermGdi32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWsock32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWininet(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermRpcrt4(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermAvifil32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermQuartz(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermRiched32(ULONG hModule, ULONG ulFlag);
ULONG APIENTRY inittermWnaspi32(ULONG hModule, ULONG ulFlag);

ULONG APIENTRY InitializeKernel32();

#endif //__INITDLL_H__
