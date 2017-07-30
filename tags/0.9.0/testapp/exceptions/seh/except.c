#include <stdio.h>
#include <windows.h>
#include <excpt.h>

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

int testCode = 0;

int tries = 3;

int exc_filter(DWORD code, DWORD filtercode, PEXCEPTION_POINTERS pPtrs)
{
    PEXCEPTION_RECORD pRec = pPtrs->ExceptionRecord;

    printf("Filter: code %08lx, filtercode %08lx\n", code, filtercode);
    printf("ExceptionCode %p\n", pRec->ExceptionCode);
    printf("ExceptionAddress %p\n", pRec->ExceptionAddress);
    printf("NumberParameters %d\n", pRec->NumberParameters);

    if (code == filtercode)
    {
        if (testCode == 4 && tries)
        {
            --tries;
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
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

int _main(int argc, char **argv)
{
    testCode = argc > 1 ? atoi(argv[1]) : 0;

    switch (testCode)
    {
        case 1:
            printf("Target: Throw EXCEPTION_ACCESS_VIOLATION from inner __try\n"
                   "and catch it in inner __except.\n\n");
            break;
        case 2:
            printf("Target: Throw EXCEPTION_INT_DIVIDE_BY_ZERO from inner __try\n"
                   "and catch it in outer __except.\n\n");
            break;
        case 3:
            printf("Target: Throw EXCEPTION_INT_DIVIDE_BY_ZERO from outer __try\n"
                   "and catch it in outer __except.\n\n");
            break;
        case 4:
            printf("Target: Throw EXCEPTION_INT_DIVIDE_BY_ZERO from outer __try,\n"
                   "fix it in the handler and continue execution.\n\n");
            break;
        default:
            printf("Invalid test code %d\n", testCode);
            return 1;
    }

    __try
    {
        if (testCode == 3 || testCode == 4)
        {
            throw_EXCEPTION_INT_DIVIDE_BY_ZERO();

            printf("FAILED: No outer exception!\n");
            return 1;
        }

        __try
        {
            if (testCode == 1)
                throw_EXCEPTION_ACCESS_VIOLATION();
            else if (testCode == 2)
                throw_EXCEPTION_INT_DIVIDE_BY_ZERO();

            printf("FAILED: No inner exception!\n");
            return 1;
        }
        __except(exc_filter(GetExceptionCode(),
                            EXCEPTION_ACCESS_VIOLATION,
                            exception_info()))
        {
            // handle exception
            printf("Inner exception handled.\n");
        }
    }
    __except(exc_filter(GetExceptionCode(),
                        EXCEPTION_INT_DIVIDE_BY_ZERO,
                        exception_info()))
    {
        if (testCode == 4 && tries)
        {
            printf("FAILED: Outer exception (tries = %d)!\n", tries);
            return 1;
        }

        // handle exception
        printf("Outer exception handled.\n");
    }

    printf("Return.\n");

    return 0;
}
