/** @file
 *
 * INITDLL library interface.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __INITDLL_H__
#define __INITDLL_H__

#include <odin.h>

/**
 * Called to initialize resources of the DLL module when it is loaded.
 *
 * This callback function is implemented by modules that link to initldll.lib
 * and want to override the default DLL initialization procedure called when the
 * DLL is loaded by the system (according to the INIT/TERM policy specified in
 * the LIBRARY statement in the .DEF file). See DLL_InitDefault() for more
 * information about the default initialization procedure.
 *
 * On success, the returned value must be the DosExitList() order code (high
 * byte of the low word) that defines the order in which DLL_Term() will be
 * called WRT other uninitialization routines and exit list handlers. Returning
 * 0 will cause DLL_Term() to be called first. Note that if several handlers
 * use the same order code they are called in LIFO order. For DLLs, this means
 * that DLL_Term() will be called in the order opposite to DLL_Init().
 *
 * @param hModule DLL module handle.
 * @return Exit list order on success or -1 to indicate a failure.
 */
ULONG SYSTEM DLL_Init(ULONG hModule);

/**
 * Called to free resources of the DLL module when it is unloaded.
 *
 * This callback function is implemented by modules that link to initldll.lib
 * and want to override the default DLL uninitialization procedure called when
 * the DLL is unloaded by the system (according to the INIT/TERM policy
 * specified in the LIBRARY statement in the .DEF file). See DLL_TermDefault()
 * for more information about the default uninitialization procedure.
 *
 * @param hModule DLL module handle.
 */
void SYSTEM DLL_Term(ULONG hModule);

/**
 * Default DLL initialization procedure.
 *
 * This procedure is called if your module does not implement the DLL_Init()
 * callback function. It performs steps necessary to initializes the C/C++
 * runtime.
 *
 * You may call this procedure from your DLL_Init() implementation to perform
 * the standard initialization steps described above.
 *
 * @param hModule DLL module handle.
 * @return 0 on success or -1 to indicate a failure.
 */
ULONG SYSTEM DLL_InitDefault(ULONG hModule);

/**
 * Default DLL uninitialization procedure.
 *
 * This procedure is called if your module does not implement the DLL_Term()
 * callback function. It performs steps necessary to terminate the C/C++
 * runtime.
 *
 * You may call this procedure from your DLL_Uniit() implementation to perform
 * the standard initialization steps described above.
 *
 * @param hModule DLL module handle.
 */
void SYSTEM DLL_TermDefault(ULONG hModule);

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__IBMCPP__) || defined(__IBMC__)

/**
 * C run-time environment initialization function.
 * Returns 0 to indicate success and -1 to indicate failure.
 */
int  _Optlink _CRT_init(void);

/**
 * C run-time environment termination function.
 * It only needs to be called when the C run-time functions are statically
 * linked.
 */
void _Optlink _CRT_term(void);

#if (__IBMCPP__ == 300) || (__IBMC__ == 300)

void _Optlink __ctordtorInit(void);
void _Optlink __ctordtorTerm(void);

#elif (__IBMCPP__ == 360) || (__IBMC__ == 360) || (__IBMC__ == 430)

void _Optlink __ctordtorInit(int flag);
#define __ctordtorInit()  __ctordtorInit(0)

void _Optlink __ctordtorTerm(int flag);
#define __ctordtorTerm()  __ctordtorTerm(0)

#else
#error "Unknown VAC compiler version!"
#endif

#elif defined(__EMX__)

int  _CRT_init(void);
void _CRT_term(void);

void __ctordtorInit(void);
void __ctordtorTerm(void);

#elif defined(__WATCOMC__)

#define _DLL_InitTerm LibMain

int  _Optlink _CRT_init(void);
void _Optlink _CRT_term(void);

#define __ctordtorInit()
#define __ctordtorTerm()

//prevent Watcom from mucking with this name
extern DWORD _Resource_PEResTab;
#pragma aux _Resource_PEResTab "*";

#else
#error "Unknown compiler!"
#endif

BOOL APIENTRY InitializeKernel32();
VOID APIENTRY ReportFatalDllInitError(PCSZ pszModName);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__INITDLL_H__
