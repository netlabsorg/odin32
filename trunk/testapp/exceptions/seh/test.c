#include <stdio.h>
#include <windows.h>
#include <excpt.h>

#ifndef _MSC_VER

#include <odinlx.h>

int _main();

int WIN32API WinMain(HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR  lpCmdLine,
                     int    nCmdShow)
{
    return _main();
}

int main()
{
    EnableSEH();
    RegisterLxExe(WinMain, NULL);
}

#else
#define _main main
#endif

int exc_filter(DWORD code, DWORD filtercode, PEXCEPTION_RECORD pRec)
{
    printf("Filter: code %08lx, filtercode %08lx\n", code, filtercode);
    printf("ExceptionCode %p\n", pRec->ExceptionCode);
    printf("ExceptionAddress %p\n", pRec->ExceptionAddress);
    printf("NumberParameters %d\n", pRec->NumberParameters);

//  if (code == filtercode)
        return EXCEPTION_EXECUTE_HANDLER;
//  return EXCEPTION_CONTINUE_SEARCH;
//  return EXCEPTION_CONTINUE_EXECUTION;
}

void throw_EXCEPTION_INT_DIVIDE_BY_ZERO()
{
    printf("Throwing EXCEPTION_INT_DIVIDE_BY_ZERO...\n");
    int x = 0;
    volatile int y = 4 / x;
}

void throw_EXCEPTION_ACCESS_VIOLATION()
{
    printf("Throwing EXCEPTION_ACCESS_VIOLATION...\n");
    volatile int *x = 0;
    *x = 0;
}

int _main()
{
    __try
    {
#if 0
        throw_EXCEPTION_INT_DIVIDE_BY_ZERO();
        printf("FAILED: No outer exception!\n");
        return 1;
#endif
        __try
        {
#if 1
            throw_EXCEPTION_ACCESS_VIOLATION();
#else
            throw_EXCEPTION_INT_DIVIDE_BY_ZERO();
#endif
            printf("FAILED: No inner exception!\n");
            return 1;
        }
        __except(exc_filter(GetExceptionCode(),
                 EXCEPTION_ACCESS_VIOLATION,
                 _exception_info()->ExceptionRecord))
        {
            // handle exception
            printf("Inner exception handled.\n");
        }
    }
    __except(exc_filter(GetExceptionCode(),
             EXCEPTION_INT_DIVIDE_BY_ZERO,
             _exception_info()->ExceptionRecord))
    {
        // handle exception
        printf("Outer exception handled.\n");
    }

    printf("Return.\n");

    return 0;
}
