/*
 * CRTDLL exit/abort/atexit functions
 * 
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997,2000 Uwe Bonnes
 * Copyright 2000 Jon Griffiths
 *
 * exit functions differ in whether they perform cleanup
 * and whether they return to the caller (really!).
 *            return      do
 *  Name      to caller?  cleanup? 
 *  _c_exit     Y           N
 *  _cexit      Y           Y
 *  _exit       N           N
 *  exit        N           Y
 *
 * Implementation Notes:
 * Not MT Safe - Adding/Executing exit() functions should be locked
 * for MT safety.
 *
 * FIXME:
 * Need a better way of printing errors for GUI programs(MsgBox?).
 * Is there really a difference between onexit/atexit?
 */
#include "crtdll.h"
#include <errno.h>
#include "process.h"


DEFAULT_DEBUG_CHANNEL(crtdll);

/* INTERNAL: Table of registered atexit() functions */
/* FIXME: This should be dynamically allocated */
#define CRTDLL_ATEXIT_TABLE_SIZE 16

static atexit_function atexit_table[CRTDLL_ATEXIT_TABLE_SIZE];
static int atexit_registered = 0; /* Points to free slot */


/* INTERNAL: call atexit functions */
void __CRTDLL__call_atexit(VOID);
void __CRTDLL__call_atexit(VOID)
{
  /* Last registered gets executed first */
  while (atexit_registered > 0)
  {
    atexit_registered--;
    TRACE(":call function (%p)\n",atexit_table[atexit_registered]);
    (*atexit_table[atexit_registered])();
  }
}


/*********************************************************************
 *	__dllonexit           			(CRTDLL.25)
 */
VOID CDECL CRTDLL___dllonexit ()
{	
  dprintf(("__dllonexit not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}


/*********************************************************************
 *	_abnormal_termination			(CRTDLL.36)
 */
int CDECL CRTDLL__abnormal_termination(void)
{
  dprintf(("CRTDLL: _abnormal_termination  not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;  /* FIXME: Can we determine if we are in an exception? */
}


/*********************************************************************
 *                  _amsg_exit     (CRTDLL.040)
 *
 * Print an error message and terminate execution. Returns 255 to the
 * host OS.
 */
VOID CDECL CRTDLL__amsg_exit(int errnum)
{
  dprintf2(("CRTDLL: _amsg_exit\n"));
  
  // fprintf(stderr,strerror(errnum));
  // ExitProcess(-1);
  
  CRTDLL_fprintf(CRTDLL_stderr,"\nrun-time error:\nError Code %d\n",errnum);
  CRTDLL__exit(255);
}


/*********************************************************************
 *                  _assert     (CRTDLL.041)
 *
 * Print an assertion message and call abort(). Really only present 
 * for win binaries. Winelib programs would typically use libc's
 * version.
 */
VOID CDECL CRTDLL__assert(LPVOID str, LPVOID file, UINT line)
{
  dprintf2(("CRTDLL: _assert\n"));
  
  CRTDLL_fprintf(CRTDLL_stderr,"Assertion failed: %s, file %s, line %d\n\n",
                 (char*)str,(char*)file, line);
  CRTDLL_abort();
  
  // _assert(str, file, line);
}


/*********************************************************************
 *                  _c_exit           (CRTDLL.047)
 * @@@PH verify if argument ret is correct, or void is correct (WINE)
 */
VOID CDECL CRTDLL__c_exit(INT ret)
{
  dprintf2(("_c_exit(%d)\n",ret));
  FIXME("not calling CRTDLL cleanup\n");
  
  /* dont exit, return to caller */
  
  ExitProcess(ret);
}


/*********************************************************************
 *                  _cexit           (CRTDLL.049)
 * @@@PH verify if argument ret is correct, or void is correct (WINE)
 */
VOID CDECL CRTDLL__cexit(INT ret)
{
  dprintf2(("_cexit(%d)\n",ret));
  FIXME("not calling CRTDLL cleanup\n");
  /* dont exit, return to caller */
  
  ExitProcess(ret);
}


/*********************************************************************
 *                  _exit           (CRTDLL.087)
 */
VOID CDECL CRTDLL__exit(LONG ret)
{
  dprintf2(("CRTDLL: _exit (%08xh)\n", 
            ret));
  TRACE(":exit code %ld\n",ret);
  CRTDLL__c_exit(ret);
  ExitProcess(ret);
}


/*********************************************************************
 *                  _onexit           (CRTDLL.236)
 *
 * Register a function to be called when the process terminates.
 */
atexit_function CDECL CRTDLL__onexit( atexit_function func)
{
  TRACE("registering function (%p)\n",func);
  if (func && atexit_registered <= CRTDLL_ATEXIT_TABLE_SIZE - 1)
  {
    atexit_table[atexit_registered] = (atexit_function)func;
    atexit_registered++;
    return func; /* successful */
  }
  ERR(":Too many onexit() functions, or NULL function - not registered!\n");
  return NULL;

  // onexit_t CDECL CRTDLL__onexit(onexit_t t)
  // {
  //   dprintf2(("CRTDLL: _onexit\n"));
  //   return (_onexit(t));
  // }
}


/*********************************************************************
 *                  exit          (CRTDLL.359)
 */
VOID CDECL CRTDLL_exit(DWORD ret)
{
  TRACE(":exit code %ld\n",ret);
  __CRTDLL__call_atexit();
  CRTDLL__cexit(ret);
  ExitProcess(ret);
}


/*********************************************************************
 *                   abort     (CRTDLL.335)
 *
 * Terminate the progam with an abnormal termination message. Returns
 * 3 to the host OS.
 */
VOID CDECL CRTDLL_abort()
{
  dprintf2(("CRTDLL: abort\n"));
  
  CRTDLL_fprintf(CRTDLL_stderr,"\nabnormal program termination\n");
  CRTDLL__exit(3);
  //abort();
}


/*********************************************************************
 *                  atexit	(CRTDLL.342)
 *
 * Register a function to be called when the process terminates.
 */
INT CDECL CRTDLL_atexit(void (*func)(void))
{
  dprintf(("CRTDLL: atexit\n"));
  
  return CRTDLL__onexit(func) == func ? 0 : -1;
  
  // if (_atexit_n >= sizeof (_atexit_v) / sizeof (_atexit_v[0]))
  //   return -1;
  // _atexit_v[_atexit_n++] = func;
  // return 0;
}


/*********************************************************************
 *                  _aexit_rtn_dll    (CRTDLL.39)
 */
VOID CDECL CRTDLL__aexit_rtn_dll(int exitcode)
{
  dprintf2(("CRTDLL: _aexit_rtn_dll\n"));
  ExitProcess(exitcode);
}

