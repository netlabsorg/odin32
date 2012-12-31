#include <stdio.h>
#include <windows.h>
#include <excpt.h>
#include <setjmp.h>

#ifndef _MSC_VER

#include <odinlx.h>

int _main();
int _argc;
char **_argv;

int WIN32API WinMain(HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR  lpCmdLine,
                     int    nCmdShow)
{
    return _main(_argc, _argv);
}

int main(int argc, char **argv)
{
    _argc = argc;
    _argv = argv;
#ifdef ODIN_FORCE_WIN32_TIB
    ForceWin32TIB();
#endif
    RegisterLxExe(WinMain, NULL);
}

#else
#define _main main
#endif

int exc_filter(DWORD code, PEXCEPTION_POINTERS pPtrs)
{
    PEXCEPTION_RECORD pRec = pPtrs->ExceptionRecord;

    printf("Filter: code %08lx\n", code);
    printf("ExceptionCode %p\n", pRec->ExceptionCode);
    printf("ExceptionAddress %p\n", pRec->ExceptionAddress);
    printf("NumberParameters %d\n", pRec->NumberParameters);

    return EXCEPTION_EXECUTE_HANDLER;
}

jmp_buf jmp_buffer;

void foo()
{
    printf("In foo.\n");
    longjmp(jmp_buffer, 1);
}

int test_1()
{
    __try
    {
        printf("__try begin.\n");

        if (!setjmp(jmp_buffer))
        {
            printf("After setjmp.\n");
            foo();
            printf("FAILED: After foo.\n");
            return 1;
        }
        else
        {
            printf("After longjmp.\n");
        }

        printf("__try end.\n");
    }
    __except(exc_filter(GetExceptionCode(), GetExceptionInformation()))
    {
        // handle exception
        printf("Exception handled.\n");
    }

    return 0;
}

int test_2()
{
    if (!setjmp(jmp_buffer))
    {
        printf("After setjmp.\n");

        __try
        {
            printf("__try begin.\n");
            foo();
            printf("FAILED: After foo.\n");
            return 1;
        }
        __except(exc_filter(GetExceptionCode(), GetExceptionInformation()))
        {
            // handle exception
            printf("Exception handled.\n");
        }
    }
    else
    {
        printf("After longjmp.\n");
    }

    return 0;
}

int _main(int argc, char **argv)
{
    int code = argc > 1 ? atoi(argv[1]) : 0;

    int rc = 0;

    switch (code)
    {
        case 1:
            rc = test_1();
            break;
            
            // Note that since we disabled Win32 exception handler unwinding
            // support (see comments in OS2ExceptionHandler3ndLevel()), test_2()
            // will always crash now
#if 0            
        case 2:
            // note that test_2() will actually only work if the top exception
            // handler is our SEH handler (i.e. __try block). See #15 for more
            // details.
            rc = test_2();
            break;
#endif            
        default:
            break;
    }

    printf("Return (%d).\n", rc);

    return rc;
}
