/* $Id: msvcrt.cpp,v 1.13 2000-08-22 02:45:40 phaller Exp $ */

/*
 * The Visual C RunTime DLL (MSVCRT/MSVCRT20/MSVCRT40)
 * 
 * Implements Visual C run-time functionality
 *
 * Copyright 1999 Jens Wiessner
 */


#include <os2win.h>
#include <odinwrap.h>
#include <wchar.h>
#include <math.h>
#include <string.h>

#include <heapstring.h>
#include <crtdll.h>

#include <win/winbase.h>
#include <win/winnt.h>
#include "msvcrt.h"

ODINDEBUGCHANNEL(msvcrt)

/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.1)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP1(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP1 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.2)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP2(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP2 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.3)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP3(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP3 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.4)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP4(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP4 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.5)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP5(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP5 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.6)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP6(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP6 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.7)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP7(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP7 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.8)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP8(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP8 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.9)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP9(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP9 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.10)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP10(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP10 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.11)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP11(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP11 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.12)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP12(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP12 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.13)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP13(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP13 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??1type_info@@UAE@XZ    (MSVCRT.14)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP14(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP14 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??2@YAPAXI@Z    (MSVCRT.15)
 */
VOID* CDECL MSVCRT_new(DWORD size)
{
    dprintf(("MSVCRT: ??2@YAPAXI@Z\n"));
    VOID* result;
    if(!(result = HeapAlloc(GetProcessHeap(),0,size)) && new_handler)
	(*new_handler)();
    return result;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.16)
 */
VOID CDECL MSVCRT_delete(VOID* ptr)
{
    dprintf(("MSVCRT: ??3@YAXPAX@Z\n"));
    HeapFree(GetProcessHeap(),0,ptr);
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.17)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP17(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP17 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.18)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP18(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP18 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.19)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP19(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP19 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.20)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP20(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP20 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.21)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP21(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP21 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.22)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP22(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP22 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.23)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP23(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP23 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.24)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP24(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP24 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.25)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP25(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP25 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.26)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP26(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP26 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.27)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP27(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP27 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.28)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP28(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP28 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.29)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP29(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP29 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.30)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP30(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP30 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.31)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP31(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP31 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.32)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP32(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP32 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.33)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP33(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP33 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.34)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP34(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP34 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.35)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP35(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP35 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.36)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP36(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP36 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ?_set_new_handler@@YAP6AHI@ZP6AHI@Z@Z    (MSVCRT.37)
 */
new_handler_type CDECL MSVCRT_set_new_handler(new_handler_type func)
{
    dprintf(("MSVCRT: ?_set_new_handler@@YAP6AHI@ZP6AHI@Z@Z\n"));
    new_handler_type old_handler = new_handler;
    new_handler = func;
    return old_handler;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.38)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP38(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP38 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.39)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP39(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP39 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.40)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP40(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP40 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.41)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP41(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP41 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.42)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP42(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP42 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.43)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP43(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP43 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.44)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP44(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP44 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.45)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP45(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP45 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.46)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP46(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP46 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.47)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP47(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP47 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.48)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP48(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP48 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  ??3@YAXPAX@Z    (MSVCRT.49)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT_EXP49(DWORD ret)
{
  dprintf(("MSVCRT: MSVCRT_EXP49 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _CxxThrowException    (MSVCRT.66)
 *	FIXME - Could not find anything about it
 */

static EXCEPTION_RECORD cxxExceptionFrame =
{ 0x0e06d763, // Exception Code
  0x00000001, // Exception Flags
  0x00000000, // Exception Record Pointer
  0x00000000, // Exception Address
  0x00000003, // Number Of Parameters
  0x19930520, // Parameter #1
  0x00000000, // Parameter #2
  0x00000000  // Parameter #3
};

VOID CDECL MSVCRT__CxxThrowException(DWORD arg1,
                                     DWORD arg2)
{
  EXCEPTION_RECORD er;
  
  dprintf(("MSVCRT: _CxxThrowException %08xh %08xh\n",
           arg1,
           arg2));
  
  //memcpy(er, cxxExceptionFrame, sizeof(er));
  // PH: optimization - why copying values when they're overwritten anyway...
  memcpy(&er, &cxxExceptionFrame, 6);
  
  // Note: er.ExceptionInformation[0] is already set!
  er.ExceptionInformation[1] = arg1;
  er.ExceptionInformation[2] = arg2;
  
  RaiseException(er.ExceptionCode,
                 er.ExceptionFlags,
                 er.NumberParameters,
                 &er.ExceptionInformation[0]);
}

/*********************************************************************
 *                  __CxxFrameHandler    (MSVCRT.74)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT___CxxFrameHandler(DWORD ret)
{
  dprintf(("MSVCRT: __CxxFrameHandler not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __CxxLongjmpUnwind    (MSVCRT.75) ?
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT___CxxLongjmpUnwind(DWORD ret)
{
  dprintf(("MSVCRT: __CxxLongjmpUnwind not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __GetMainArgs  (MSVCRT.89)
 */
DWORD CDECL MSVCRT___GetMainArgs(LPDWORD argc,LPSTR **argv,
                                LPSTR *environ,DWORD flag)
{
        char *cmdline;
        char  **xargv;
	int	xargc,i,afterlastspace;
	DWORD	version;

	dprintf(("MSVCRT: GetMainArgs\n"));

	MSVCRT__acmdln = cmdline = HEAP_strdupA( GetProcessHeap(), 0,
                                                    GetCommandLineA() );

	version	= GetVersion();
	MSVCRT__osver       = version >> 16;
	MSVCRT__winminor    = version & 0xFF;
	MSVCRT__winmajor    = (version>>8) & 0xFF;
	MSVCRT__baseversion = version >> 16;
	MSVCRT__winver      = ((version >> 8) & 0xFF) + ((version & 0xFF) << 8);
	MSVCRT__baseminor   = (version >> 16) & 0xFF;
	MSVCRT__basemajor   = (version >> 24) & 0xFF;
	MSVCRT__osversion   = version & 0xFFFF;
	MSVCRT__osminor     = version & 0xFF;
	MSVCRT__osmajor     = (version>>8) & 0xFF;

	/* missing threading init */

	i=0;xargv=NULL;xargc=0;afterlastspace=0;
/*
	while (cmdline[i]) {
		if (cmdline[i]==' ') {
			xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                                   sizeof(char*)*(++xargc));
			cmdline[i]='\0';
			xargv[xargc-1] = HEAP_strdupA( GetProcessHeap(), 0,
                                                       cmdline+afterlastspace);
			i++;
			while (cmdline[i]==' ')
				i++;
			if (cmdline[i])
				afterlastspace=i;

		} else
			i++;

	}

	xargv=(char**)HeapReAlloc( GetProcessHeap(), 0, xargv,
                                   sizeof(char*)*(++xargc));
	cmdline[i]='\0';
	xargv[xargc-1] = HEAP_strdupA( GetProcessHeap(), 0,
                                       cmdline+afterlastspace);
*/
	MSVCRT___argc  	= xargc;
	*argc		= xargc;
	MSVCRT___argv  	= xargv;
	*argv		= xargv;
	MSVCRT__environ = *environ = GetEnvironmentStringsA();
	dprintf(("MSVCRT: GetMainArgs end\n"));
	return 0;
}


/*********************************************************************
 *                  __initenv    (MSVCRT.90)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT___initenv(DWORD ret)
{
  dprintf(("MSVCRT: __initenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __p___argc  (MSVCRT.99)
 */
int * CDECL MSVCRT___p___argc()
{
  dprintf(("MSVCRT: __p___argc\n"));
	return (int*)&MSVCRT___argc;
}


/*********************************************************************
 *                  __p___argv  (MSVCRT.100)
 */
char ** CDECL MSVCRT___p___argv()
{
  dprintf(("MSVCRT: __p___argv\n"));
	return (char**)&MSVCRT___argv;
}


/*********************************************************************
 *                  __p__initenv  (MSVCRT.101)
 */
char ** CDECL MSVCRT___p__initenv()
{
  dprintf(("MSVCRT: __p__initenv not implemented\n"));
	return &MSVCRT__acmdln;
}


/*********************************************************************
 *                  __p___mb_cur_max  (MSVCRT.102)
 */
int * CDECL MSVCRT___p___mb_cur_max()
{
  dprintf(("MSVCRT: __p___mb_cur_max\n"));
	return &MSVCRT___mb_cur_max;
}


/*********************************************************************
 *                  __p__acmdln  (MSVCRT.105)
 */
char ** CDECL MSVCRT___p__acmdln()
{
  dprintf(("MSVCRT: __p__acmdln\n"));
	return &MSVCRT__acmdln;
}


/*********************************************************************
 *                  __p__fmode  (MSVCRT.107)
 */
int * CDECL MSVCRT___p__fmode()
{
  dprintf(("MSVCRT: __p__fmode\n"));
	return (int*)&MSVCRT__fmode;
}


/*********************************************************************
 *                  __p__daylight  (MSVCRT.108)
 */
int * CDECL MSVCRT___p__daylight()
{
  dprintf(("MSVCRT: __p__daylight\n"));
	return (int*)&MSVCRT__daylight;
}


/*********************************************************************
 *                  __p__environ  (MSVCRT.110)
 */
char ** CDECL MSVCRT___p__environ()
{
  dprintf(("MSVCRT: __p__environ\n"));
	return &MSVCRT__environ;
}


/*********************************************************************
 *                  __p__fileinfo  (MSVCRT.111)
 */
char ** CDECL MSVCRT___p__fileinfo()
{
  dprintf(("MSVCRT: __p__fileinfo\n"));
	return &MSVCRT__fileinfo;
}


/*********************************************************************
 *                  __p__commode  (MSVCRT.112)
 */
int * CDECL MSVCRT___p__commode()
{
  dprintf(("MSVCRT: __p__commode\n"));
	return (int*)&MSVCRT__commode;
}


/*********************************************************************
 *                  __p__iob  (MSVCRT.113)
 */
CRTDLL_FILE * CDECL MSVCRT___p__iob()
{
  dprintf(("MSVCRT: __p__iob\n"));
	return &_iob;
}


/*********************************************************************
 *                  __p__osver  (MSVCRT.116)
 */
int * CDECL MSVCRT___p__osver()
{
  dprintf(("MSVCRT: __p__osver\n"));
	return (int*)&MSVCRT__osver;
}


/*********************************************************************
 *                  __p__pctype  (MSVCRT.117)
 */
USHORT * CDECL MSVCRT___p__pctype()
{
  dprintf(("MSVCRT: __p__pctype\n"));
	return (USHORT*)&MSVCRT__pctype;
}


/*********************************************************************
 *                  __p__pgmptr  (MSVCRT.118)
 */
char ** CDECL MSVCRT___p__pgmptr()
{
  dprintf(("MSVCRT: __p__pgmptr\n"));
	return (char**)&MSVCRT__pgmptr;
}


/*********************************************************************
 *                  __p__pwctype  (MSVCRT.119)
 */
USHORT * CDECL MSVCRT___p__pwctype()
{
  dprintf(("MSVCRT: __p__pwctype\n"));
	return (USHORT*)&MSVCRT__pwctype;
}


/*********************************************************************
 *                  __p__timezone  (MSVCRT.120)
 */
int * CDECL MSVCRT___p__timezone()
{
  dprintf(("MSVCRT: __p__timezone\n"));
	return (int*)&MSVCRT__timezone;
}


/*********************************************************************
 *                  __p__tzname  (MSVCRT.121)
 */
char ** CDECL MSVCRT___p__tzname()
{
  dprintf(("MSVCRT: __p__tzname\n"));
	return (char**)&MSVCRT__tzname;
}


/*********************************************************************
 *                  __p__winmajor  (MSVCRT.124)
 */
int * CDECL MSVCRT___p__winmajor()
{
  dprintf(("MSVCRT: __p__winmajor\n"));
	return (int*)&MSVCRT__winmajor;
}


/*********************************************************************
 *                  __p__winminor  (MSVCRT.125)
 */
int * CDECL MSVCRT___p__winminor()
{
  dprintf(("MSVCRT: __p__winminor\n"));
	return (int*)&MSVCRT__winminor;
}


/*********************************************************************
 *                  __p__winver  (MSVCRT.126)
 */
int * CDECL MSVCRT___p__winver()
{
  dprintf(("MSVCRT: __p__winver\n"));
	return (int*)&MSVCRT__winver;
}


/*********************************************************************
 *                  __set_app_type    (MSVCRT.130)
 */
int CDECL MSVCRT___set_app_type(int app_type)
{
  dprintf(("MSVCRT: __set_app_type\n"));
	return __app_type = app_type;
}	


/*********************************************************************
 *                  __setusermatherr    (MSVCRT.132)
 */
int CDECL MSVCRT___setusermatherr(int matherr)
{
  dprintf(("MSVCRT: __setusermatherr\n"));
	return __usermatherr = matherr;
}	


/*********************************************************************
 *                  _adjust_fdiv    (MSVCRT.158)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__adjust_fdiv(DWORD ret)
{
  dprintf(("MSVCRT: _adjust_fdiv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _aexit_rtn    (MSVCRT.159)
 */
VOID CDECL MSVCRT__aexit_rtn(int exitcode)
{
  dprintf(("MSVCRT: _aexit_rtn\n"));
  ExitProcess(exitcode);
}


/*********************************************************************
 *                  _atoi64    (MSVCRT.163)
 */
__int64 CDECL MSVCRT__atoi64(const char *s)
{
  dprintf(("MSVCRT: _atoi64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _beginthreadex   (MSVCRT.167)
 */
unsigned long CDECL MSVCRT__beginthreadex( void *security, unsigned stack_size, 
		unsigned ( __stdcall *start_address )( void * ), void *arglist, 
		unsigned initflag, unsigned *thrdaddr )
{
  dprintf(("MSVCRT: _beginthreadex not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *           _endthreadex  (MSVCRT.198)
 */
void CDECL MSVCRT__endthreadex(unsigned retval)
{
  dprintf(("MSVCRT: _endthreadex not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _except_handler3  (MSVCRT.203)
 */
INT CDECL MSVCRT__except_handler3 ( PEXCEPTION_RECORD rec,
	PEXCEPTION_FRAME frame, PCONTEXT context, 
	PEXCEPTION_FRAME  *dispatcher)
{
        dprintf(("MSVCRT: _except_handler3\n"));
	return 1;
}


/*********************************************************************
 *                  _filelengthi64    (MSVCRT.222)
 */
__int64 CDECL MSVCRT__filelengthi64(int handle)
{
  dprintf(("MSVCRT: _filelengthi64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _findfirsti64    (MSVCRT.226)
 */
__int64 CDECL MSVCRT__findfirsti64( char *filespec, struct _finddata_t *fileinfo)
{
  dprintf(("MSVCRT: _findfirsti64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _findnexti64    (MSVCRT.228)
 */
__int64 CDECL MSVCRT__findnexti64(long handle, struct _finddata_t *fileinfo)
{
  dprintf(("MSVCRT: _findnexti64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _fstati64    (MSVCRT.240)
 */
__int64 CDECL MSVCRT__fstati64(int handle)
{
  dprintf(("MSVCRT: _fstati64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _get_sbh_threshold    (MSVCRT.247)
 */
size_t CDECL MSVCRT__get_sbh_threshold( void )
{
  dprintf(("MSVCRT: _get_sbh_threshold not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getmaxstdio    (MSVCRT.256)
 */
int CDECL MSVCRT__getmaxstdio( void )
{
  dprintf(("MSVCRT: _getmaxstdio not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getmbcp    (MSVCRT.257)
 */
int CDECL MSVCRT__getmbcp( void )
{
  dprintf(("MSVCRT: _getmbcp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _getws    (MSVCRT.261)
 */
wchar_t * CDECL MSVCRT__getws( wchar_t *s )
{
  dprintf(("MSVCRT: _getws not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _heapadd    (MSVCRT.263)
 */
int CDECL MSVCRT__heapadd(void *v, size_t s)
{
  dprintf(("MSVCRT: _heapadd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _i64toa    (MSVCRT.270)
 */
char * CDECL MSVCRT__i64toa(__int64 i1, char *s, int i2)
{
  dprintf(("MSVCRT: _i64toa not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _i64tow    (MSVCRT.271)
 */
wchar_t * CDECL MSVCRT__i64tow(__int64 i1, wchar_t *s, int i2)
{
  dprintf(("MSVCRT: _i64tow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _inp    (MSVCRT.273)
 */
int CDECL MSVCRT__inp( unsigned short port )
{
  dprintf(("MSVCRT: _inp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _inpw    (MSVCRT.274)
 */
unsigned short CDECL MSVCRT__inpw( unsigned short port )
{
  dprintf(("MSVCRT: _inpw not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _inpd    (MSVCRT.275)
 */
unsigned long CDECL MSVCRT__inpd( unsigned short port )
{
  dprintf(("MSVCRT: _inpd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkprint    (MSVCRT.284)
 */
int CDECL MSVCRT__ismbbkprint( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbbkprint not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcalnum    (MSVCRT.290)
 */
int CDECL MSVCRT__ismbcalnum( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcalnum not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcgraph    (MSVCRT.293)
 */
int CDECL MSVCRT__ismbcgraph( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcgraph not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcpunct    (MSVCRT.302)
 */
int CDECL MSVCRT__ismbcpunct( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcpunct not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _itow    (MSVCRT.310)
 */
wchar_t * CDECL MSVCRT__itow( int value, wchar_t *string, int radix )
{
  dprintf(("MSVCRT: _itow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _unlock    (MSVCRT.480)
 */
// Note: PH 2000/08/22 array size is probably larger than 0x12!
static CRITICAL_SECTION* arrpCriticalSections[0x12] = {0};

#define CRITSEC_TABLE_LOCK 0x11
 
VOID CDECL MSVCRT__unlock(unsigned long ulIndex)
{
  dprintf(("MSVCRT: _unlock\n"));
  
  CRITICAL_SECTION *pCS = arrpCriticalSections[ulIndex];
  LeaveCriticalSection(pCS);
}


/*********************************************************************
 *                  _lock    (MSVCRT.318)
 */
// Prototype from CRTDLL.CPP
VOID CDECL amsg_exit(int errnum);

VOID CDECL MSVCRT__lock(unsigned long ulIndex)
{
  dprintf(("MSVCRT: _lock\n"));
  
  CRITICAL_SECTION *pCS = arrpCriticalSections[ulIndex];
  if (pCS == NULL)
  {
    CRITICAL_SECTION *pCSNew;
    
    pCSNew = (CRITICAL_SECTION*)malloc( sizeof(CRITICAL_SECTION) );
    if (pCSNew == NULL)
      amsg_exit(0x11); // yield error message
    
    MSVCRT__lock(CRITSEC_TABLE_LOCK); // lock table
    if (pCS != NULL) // has been modified meanwhile by other thread ?
      free(pCSNew);
    else
    {
      InitializeCriticalSection(pCSNew);
      arrpCriticalSections[ulIndex] = pCSNew;
    }
      
    MSVCRT__unlock(CRITSEC_TABLE_LOCK); // unlock table
  }
    
  EnterCriticalSection(pCS);
}


/*********************************************************************
 *                  _lseeki64    (MSVCRT.326)
 */
__int64 CDECL MSVCRT__lseeki64( int handle, __int64 offset, int origin )
{
  dprintf(("MSVCRT: _lseeki64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ltow    (MSVCRT.328)
 */
wchar_t * CDECL MSVCRT__ltow( long value, wchar_t *string, int radix )
{
  dprintf(("MSVCRT: _ltow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _outp    (MSVCRT.395)
 */
int CDECL MSVCRT__outp( unsigned short port, int databyte )
{
  dprintf(("MSVCRT: _outp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _outpw    (MSVCRT.396)
 */
unsigned short CDECL MSVCRT__outpw( unsigned short port, unsigned short dataword )
{
  dprintf(("MSVCRT: _outpw not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _outpd    (MSVCRT.397)
 */
unsigned long CDECL MSVCRT__outpd( unsigned short port, unsigned long dataword )
{
  dprintf(("MSVCRT: _outpd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _putws    (MSVCRT.407)
 */
int CDECL MSVCRT__putws( const wchar_t *s )
{
  dprintf(("MSVCRT: _putws not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _set_error_mode    (MSVCRT.421)
 */
int CDECL MSVCRT__set_error_mode( int modeval )
{
  dprintf(("MSVCRT: _set_error_mode not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _set_sbh_threshold    (MSVCRT.422)
 */
int CDECL MSVCRT__set_sbh_threshold( size_t size )
{
  dprintf(("MSVCRT: _set_sbh_threshold not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _setmaxstdio    (MSVCRT.426)
 */
int CDECL MSVCRT__setmaxstdio(int i)
{
  dprintf(("MSVCRT: _setmaxstdio not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _setmbcp    (MSVCRT.427)
 */
int CDECL MSVCRT__setmbcp( int codepage )
{
  dprintf(("MSVCRT: _setmbcp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _stati64    (MSVCRT.444)
 */
__int64 CDECL MSVCRT__stati64( const char *path, struct _stat *buffer )
{
  dprintf(("MSVCRT: _stati64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _strncoll    (MSVCRT.453)
 */
int CDECL MSVCRT__strncoll( const char *s1, const char *s2, size_t n )
{
  dprintf(("MSVCRT: _strncoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _strnicoll    (MSVCRT.455)
 */
int CDECL MSVCRT__strnicoll( const char *s1, const char *s2, size_t n )
{
  dprintf(("MSVCRT: _strnicoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _telli64    (MSVCRT.465)
 */
__int64 CDECL MSVCRT__telli64( int handle )
{
  dprintf(("MSVCRT: _telli64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ui64toa    (MSVCRT.472)
 */
/*
char * CDECL MSVCRT__ui64toa(unsigned __int64 value, char *s, int i)
{
  dprintf(("MSVCRT: _ui64toa not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
*/

/*********************************************************************
 *                  _ui64tow    (MSVCRT.473)
 */
/*
wchar_t * CDECL MSVCRT__ui64tow(unsigned __int64 value, wchar_t *s, int i)
{
  dprintf(("MSVCRT: _ui64tow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
*/

/*********************************************************************
 *                  _ultow    (MSVCRT.475)
 */
wchar_t * CDECL MSVCRT__ultow( unsigned long value, wchar_t *string, int radix )
{
  dprintf(("MSVCRT: _ultow not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _umask    (MSVCRT.476)
 */
int CDECL MSVCRT__umask( int pmode )
{
  dprintf(("MSVCRT: _umask not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _waccess    (MSVCRT.484)
 */
int CDECL MSVCRT__waccess( const wchar_t *path, int mode )
{
  dprintf(("MSVCRT: _waccess not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wasctime    (MSVCRT.485)
 */
wchar_t * CDECL MSVCRT__wasctime( const struct tm *timeptr )
{
  dprintf(("MSVCRT: _wasctime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wchdir    (MSVCRT.486)
 */
int CDECL MSVCRT__wchdir( const wchar_t *dirname )
{
  dprintf(("MSVCRT: _wchdir not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wchmod    (MSVCRT.487)
 */
int CDECL MSVCRT__wchmod( const wchar_t *filename, int pmode )
{
  dprintf(("MSVCRT: _wchmod not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wcreat    (MSVCRT.489)
 */
int CDECL MSVCRT__wcreat( const wchar_t *filename, int pmode )
{
  dprintf(("MSVCRT: _wcreat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wcsncoll    (MSVCRT.494)
 */
int CDECL MSVCRT__wcsncoll( const wchar_t *string1, const wchar_t *string2, size_t count )
{
  dprintf(("MSVCRT: _wcsncoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wcsnicoll    (MSVCRT.496)
 */
int CDECL MSVCRT__wcsnicoll( const wchar_t *string1, const wchar_t *string2 , size_t count )
{
  dprintf(("MSVCRT: _wcsnicoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wctime       (MSVCRT.501)
 */
wchar_t * CDECL MSVCRT__wctime( const time_t *timer )
{
  dprintf(("MSVCRT: _wctime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecl       (MSVCRT.503)
 */
int CDECL MSVCRT__wexecl( const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexecl not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecle       (MSVCRT.504)
 */
int CDECL MSVCRT__wexecle( const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexecle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexeclp       (MSVCRT.505)
 */
int CDECL MSVCRT__wexeclp( const wchar_t *file, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexeclp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexeclpe      (MSVCRT.506)
 */
int CDECL MSVCRT__wexeclpe( const wchar_t *file, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wexeclpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecv      (MSVCRT.507)
 */
int CDECL MSVCRT__wexecv( const wchar_t *path, const wchar_t *const argv[] )
{
  dprintf(("MSVCRT: _wexecv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecve     (MSVCRT.508)
 */
int CDECL MSVCRT__wexecve( const wchar_t *path, const wchar_t *const argv[], 
				  const wchar_t *const envp[] )
{
  dprintf(("MSVCRT: _wexecve not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecvp     (MSVCRT.509)
 */
int CDECL MSVCRT__wexecvp( const wchar_t *file, const wchar_t *const argv[] )
{
  dprintf(("MSVCRT: _wexecvp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wexecvpe     (MSVCRT.510)
 */
int CDECL MSVCRT__wexecvpe( const wchar_t *file, const wchar_t *const argv[], 
				   const wchar_t *const envp[] )
{
  dprintf(("MSVCRT: _wexecvpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfdopen     (MSVCRT.511)
 */
FILE * CDECL MSVCRT__wfdopen( int, const wchar_t *s )
{
  dprintf(("MSVCRT: _wfdopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfindfirst     (MSVCRT.512)
 */
long CDECL MSVCRT__wfindfirst( const wchar_t *filespec, struct _wfinddata_t *fileinfo )
{
  dprintf(("MSVCRT: _wfindfirst not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfindfirsti64     (MSVCRT.513)
 */
__int64 CDECL MSVCRT__wfindfirsti64( wchar_t *filespec, struct _wfinddata_t *fileinfo )
{
  dprintf(("MSVCRT: _wfindfirsti64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfindnext     (MSVCRT.514)
 */
int CDECL MSVCRT__wfindnext( long handle, struct _wfinddata_t *fileinfo )
{
  dprintf(("MSVCRT: _wfindnext not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfindnexti64     (MSVCRT.515)
 */
/*
int CDECL MSVCRT__wfindnexti64(intptr_t x, struct _wfinddatai64_t *s)
{
  dprintf(("MSVCRT: _wfindnexti64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
*/


/*********************************************************************
 *                  _wfopen        (MSVCRT.516)
 */
FILE * CDECL MSVCRT__wfopen( const wchar_t *s1, const wchar_t *s2 )
{
  dprintf(("MSVCRT: _wfopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfreopen        (MSVCRT.517)
 */
FILE * CDECL MSVCRT__wfreopen( const wchar_t *s1, const wchar_t *s2, FILE * f)
{
  dprintf(("MSVCRT: _wfreopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfsopen        (MSVCRT.518)
 */
FILE * CDECL MSVCRT__wfsopen( const wchar_t *filename, const wchar_t *mode,  int shflag )
{
  dprintf(("MSVCRT: _wfsopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wfullpath        (MSVCRT.519)
 */
wchar_t * CDECL MSVCRT__wfullpath( wchar_t *s1, const wchar_t *s2, size_t n )
{
  dprintf(("MSVCRT: _wfullpath not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wgetcwd          (MSVCRT.520)
 */
wchar_t * CDECL MSVCRT__wgetcwd( wchar_t *buf, size_t size )
{
  dprintf(("MSVCRT: _wgetcwd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wgetdcwd          (MSVCRT.521)
 */
wchar_t * CDECL MSVCRT__wgetdcwd( int drive, wchar_t *buffer, size_t maxlen )
{
  dprintf(("MSVCRT: _wgetdcwd not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wgetenv          (MSVCRT.522)
 */
wchar_t * CDECL MSVCRT__wgetenv( const wchar_t *name )
{
  dprintf(("MSVCRT: _wgetenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wmakepath          (MSVCRT.526)
 */
void CDECL MSVCRT__wmakepath( wchar_t *path, const wchar_t *drive,
	    const wchar_t *dir, const wchar_t *fname, const wchar_t *ext )
{
  dprintf(("MSVCRT: _wmakepath not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wmkdir           (MSVCRT.527)
 */
int CDECL MSVCRT__wmkdir( const wchar_t *path )
{
  dprintf(("MSVCRT: _wmkdir not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wmktemp           (MSVCRT.528)
 */
wchar_t * CDECL MSVCRT__wmktemp( wchar_t*  )
{
  dprintf(("MSVCRT: _wmktemp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wopen           (MSVCRT.529)
 */
int CDECL MSVCRT__wopen( const wchar_t *s, int n, ... )
{
  dprintf(("MSVCRT: _wopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wperror          (MSVCRT.530)
 */
void CDECL MSVCRT__wperror( const wchar_t *s )
{
  dprintf(("MSVCRT: _wperror not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wpopen           (MSVCRT.532)
 */
FILE * CDECL MSVCRT__wpopen( const wchar_t *command, const wchar_t *mode )
{
  dprintf(("MSVCRT: _wpopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wputenv          (MSVCRT.533)
 */
int CDECL MSVCRT__wputenv( const wchar_t *env_string )
{
  dprintf(("MSVCRT: _wputenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wremove          (MSVCRT.534)
 */
int CDECL MSVCRT__wremove( const wchar_t *s )
{
  dprintf(("MSVCRT: _wremove not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wrename          (MSVCRT.535)
 */
int CDECL MSVCRT__wrename( const wchar_t *s1, const wchar_t *s2 )
{
  dprintf(("MSVCRT: _wrename not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wrmdir          (MSVCRT.537)
 */
int CDECL MSVCRT__wrmdir( const wchar_t *path )
{
  dprintf(("MSVCRT: _wrmdir not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsearchenv          (MSVCRT.538)
 */
void CDECL MSVCRT__wsearchenv( const wchar_t *name, const wchar_t *env_var, wchar_t *buf )
{
  dprintf(("MSVCRT: _wsearchenv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wsetlocale          (MSVCRT.539)
 */
wchar_t * CDECL MSVCRT__wsetlocale(int category,const wchar_t *locale)
{
  dprintf(("MSVCRT: _wsetlocale not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsopen          (MSVCRT.540)
 */
int CDECL MSVCRT__wsopen( const wchar_t *s, int n1, int n2, ... )
{
  dprintf(("MSVCRT: _wsopen not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnl         (MSVCRT.541)
 */
int CDECL MSVCRT__wspawnl( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnl not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnle          (MSVCRT.542)
 */
int CDECL MSVCRT__wspawnle( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnle not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnlp          (MSVCRT.543)
 */
int CDECL MSVCRT__wspawnlp( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnlp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnlpe          (MSVCRT.544)
 */
int CDECL MSVCRT__wspawnlpe( int mode, const wchar_t *path, const wchar_t *arg0, ... )
{
  dprintf(("MSVCRT: _wspawnlpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnv          (MSVCRT.545)
 */
int CDECL MSVCRT__wspawnv( int mode, const wchar_t *path, const wchar_t * const *argv )
{
  dprintf(("MSVCRT: _wspawnv not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnve          (MSVCRT.546)
 */
int CDECL MSVCRT__wspawnve( int mode, const wchar_t *path, 
	   const wchar_t * const *argv, const wchar_t * const *envp )
{
  dprintf(("MSVCRT: _wspawnve not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnvp          (MSVCRT.547)
 */
int CDECL MSVCRT__wspawnvp( int mode, const wchar_t *path, const wchar_t * const *argv )
{
  dprintf(("MSVCRT: _wspawnvp not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wspawnvpe          (MSVCRT.548)
 */
int CDECL MSVCRT__wspawnvpe( int mode, const wchar_t *path, 
	   const wchar_t * const *argv, const wchar_t * const *envp )
{
  dprintf(("MSVCRT: _wspawnvpe not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsplitpath         (MSVCRT.549)
 */
void CDECL MSVCRT__wsplitpath( const wchar_t *path, wchar_t *drive, 
		 wchar_t *dir, wchar_t *fname, wchar_t *ext )
{
  dprintf(("MSVCRT: _wsplitpath not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *                  _wstat          (MSVCRT.550)
 */
int CDECL MSVCRT__wstat( const wchar_t *s, struct _wstat *w )
{
  dprintf(("MSVCRT: _wstat not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wstati64          (MSVCRT.551)
 */
int CDECL MSVCRT__wstati64(const wchar_t *s, struct _stati64 *w)
{
  dprintf(("MSVCRT: _wstati64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wstrdate          (MSVCRT.552)
 */
wchar_t * CDECL MSVCRT__wstrdate( wchar_t *buf )
{
  dprintf(("MSVCRT: _wstrdate not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wstrtime          (MSVCRT.553)
 */
wchar_t * CDECL MSVCRT__wstrtime( wchar_t *buf )
{
  dprintf(("MSVCRT: _wstrtime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wsystem           (MSVCRT.554)
 */
int CDECL MSVCRT__wsystem( const wchar_t *cmd )
{
  dprintf(("MSVCRT: _wsystem not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wtempnam          (MSVCRT.555)
 */
wchar_t * CDECL MSVCRT__wtempnam( wchar_t *dir, wchar_t *prefix )
{
  dprintf(("MSVCRT: _wtempnam not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wtmpnam          (MSVCRT.558)
 */
wchar_t * CDECL MSVCRT__wtmpnam( wchar_t *tn )
{
  dprintf(("MSVCRT: _wtmpnam not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wtoi64          (MSVCRT.556)
 */
__int64 CDECL MSVCRT__wtoi64(const wchar_t *wt)
{
  dprintf(("MSVCRT: _wtoi64 not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wunlink     (MSVCRT.560)
 */
int CDECL MSVCRT__wunlink(const wchar_t *path)
{
  dprintf(("MSVCRT: _wunlink not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _wutime     (MSVCRT.561)
 */
int CDECL MSVCRT__wutime( const wchar_t *path, const struct utimbuf * times )
{
  dprintf(("MSVCRT: _wutime not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  fgetws    (MSVCRT.597)
 */
wchar_t * CDECL MSVCRT_fgetws( wchar_t *s, int n, FILE *strm )
{
  dprintf(("MSVCRT: fgetws\n"));
  return (fgetws(s, n, strm));
}


/*********************************************************************
 *                  fputws    (MSVCRT.605)
 */
int CDECL MSVCRT_fputws( const wchar_t *s, FILE *strm )
{
  dprintf(("MSVCRT: fputws\n"));
  return (fputws(s, strm));
}


/*********************************************************************
 *                  getwc    (MSVCRT.621)
 */
wint_t CDECL MSVCRT_getwc( FILE * strm)
{
  dprintf(("MSVCRT: getwc\n"));
  return (getwc(strm));
}


/*********************************************************************
 *                  getwchar    (MSVCRT.622)
 */
wint_t CDECL MSVCRT_getwchar( void )
{
  dprintf(("MSVCRT: getwchar\n"));
  return (getwchar());
}


/*********************************************************************
 *                  putwc    (MSVCRT.675)
 */
wint_t CDECL MSVCRT_putwc( wint_t t, FILE * strm)
{
  dprintf(("MSVCRT: putwc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  putwchar    (MSVCRT.676)
 */
wint_t CDECL MSVCRT_putwchar( wint_t t)
{
  dprintf(("MSVCRT: putwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/**************************************************************************/
/* MSVCRT20 & MSVCRT40 only functions  					  */
/**************************************************************************/


/*********************************************************************
 *                  _matherr    (MSVCRT20.677)
 */
int CDECL MSVCRT__matherr( struct exception *except )
{
  dprintf(("MSVCRT20: _matherr\n"));
  return _matherr(except);
}
