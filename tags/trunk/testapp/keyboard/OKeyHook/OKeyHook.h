
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OKEYHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OKEYHOOK_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef OKEYHOOK_EXPORTS
#define OKEYHOOK_API __declspec(dllexport)
#else
#define OKEYHOOK_API __declspec(dllimport)
#endif

extern OKEYHOOK_API int nOKeyHook;

OKEYHOOK_API int fnOKeyHook(void);

